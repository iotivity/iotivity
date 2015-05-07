//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include "InProcClientWrapper.h"
#include "ocstack.h"

#include "OCPlatform.h"
#include "OCResource.h"
#include <OCSerialization.h>
using namespace std;

namespace OC
{
    InProcClientWrapper::InProcClientWrapper(
        std::weak_ptr<std::recursive_mutex> csdkLock, PlatformConfig cfg)
            : m_threadRun(false), m_csdkLock(csdkLock),
              m_cfg { cfg }
    {
        // if the config type is server, we ought to never get called.  If the config type
        // is both, we count on the server to run the thread and do the initialize

        if(m_cfg.mode == ModeType::Client)
        {
            OCStackResult result = OCInit(m_cfg.ipAddress.c_str(), m_cfg.port, OC_CLIENT);

            if(OC_STACK_OK != result)
            {
                throw InitializeException(OC::InitException::STACK_INIT_ERROR, result);
            }

            m_threadRun = true;
            m_listeningThread = std::thread(&InProcClientWrapper::listeningFunc, this);
        }
    }

    InProcClientWrapper::~InProcClientWrapper()
    {
        if(m_threadRun && m_listeningThread.joinable())
        {
            m_threadRun = false;
            m_listeningThread.join();
        }

        // only stop if we are the ones who actually called 'init'.  We are counting
        // on the server to do the stop.
        if(m_cfg.mode == ModeType::Client)
        {
            OCStop();
        }
    }

    void InProcClientWrapper::listeningFunc()
    {
        while(m_threadRun)
        {
            OCStackResult result;
            auto cLock = m_csdkLock.lock();
            if(cLock)
            {
                std::lock_guard<std::recursive_mutex> lock(*cLock);
                result = OCProcess();
            }
            else
            {
                result = OC_STACK_ERROR;
            }

            if(result != OC_STACK_OK)
            {
                // TODO: do something with result if failed?
            }

            // To minimize CPU utilization we may wish to do this with sleep
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    OCRepresentation parseGetSetCallback(OCClientResponse* clientResponse)
    {
        if(clientResponse->resJSONPayload == nullptr || clientResponse->resJSONPayload[0] == '\0')
        {
            return OCRepresentation();
        }

        MessageContainer oc;
        try
        {
            oc.setJSONRepresentation(clientResponse->resJSONPayload);
        }
        catch (cereal::RapidJSONException& ex)
        {
            oclog() <<"RapidJSON Exception in parseGetSetCallback: "<<ex.what() <<std::endl<<
                "Data was:"<< clientResponse->resJSONPayload<< ":" << std::flush;
            throw OCException(OC::Exception::INVALID_REPRESENTATION, OC_STACK_INVALID_JSON);
        }
        catch (cereal::Exception& ex)
        {
            oclog() <<"Cereal Exception in parseGetSetCallback: "<<ex.what() <<std::endl<<
                "Data was:"<< clientResponse->resJSONPayload<< ":" << std::flush;
            throw OCException(OC::Exception::INVALID_REPRESENTATION, OC_STACK_INVALID_JSON);
        }

        std::vector<OCRepresentation>::const_iterator it = oc.representations().begin();
        if(it == oc.representations().end())
        {
            return OCRepresentation();
        }

        // first one is considered the root, everything else is considered a child of this one.
        OCRepresentation root = *it;
        ++it;

        std::for_each(it, oc.representations().end(),
                [&root](const OCRepresentation& repItr)
                {root.addChild(repItr);});
        return root;

    }

    OCStackApplicationResult listenCallback(void* ctx, OCDoHandle handle,
        OCClientResponse* clientResponse)
    {
        ClientCallbackContext::ListenContext* context =
            static_cast<ClientCallbackContext::ListenContext*>(ctx);

        if(clientResponse->result != OC_STACK_OK)
        {
            oclog() << "listenCallback(): failed to create resource. clientResponse: "
                    << clientResponse->result
                    << std::flush;

            return OC_STACK_KEEP_TRANSACTION;
        }

        auto clientWrapper = context->clientWrapper.lock();

        if(!clientWrapper)
        {
            oclog() << "listenCallback(): failed to get a shared_ptr to the client wrapper"
                    << std::flush;
            return OC_STACK_KEEP_TRANSACTION;
        }

        std::stringstream requestStream;
        requestStream << clientResponse->resJSONPayload;

        try
        {

            ListenOCContainer container(clientWrapper, *clientResponse->addr,
                    clientResponse->connType, requestStream);
            // loop to ensure valid construction of all resources
            for(auto resource : container.Resources())
            {
                std::thread exec(context->callback, resource);
                exec.detach();
            }

        }
        catch(const std::exception& e)
        {
            oclog() << "listenCallback failed to parse a malformed message: "
                    << e.what()
                    << std::endl
                    << clientResponse->resJSONPayload
                    << std::endl
                    << clientResponse->result
                    << std::flush;
            return OC_STACK_KEEP_TRANSACTION;
        }

        return OC_STACK_KEEP_TRANSACTION;
    }

    OCStackResult InProcClientWrapper::ListenForResource(const std::string& serviceUrl,
        const std::string& resourceType, OCConnectivityType connectivityType,
        FindCallback& callback, QualityOfService QoS)
    {
        if(!callback)
        {
            return OC_STACK_INVALID_PARAM;
        }

        OCStackResult result;

        OCCallbackData cbdata = {0};

        ClientCallbackContext::ListenContext* context = new ClientCallbackContext::ListenContext();
        context->callback = callback;
        context->clientWrapper = shared_from_this();

        cbdata.context =  static_cast<void*>(context);
        cbdata.cb = listenCallback;
        cbdata.cd = [](void* c){delete static_cast<ClientCallbackContext::ListenContext*>(c);};

        auto cLock = m_csdkLock.lock();
        if(cLock)
        {
            std::lock_guard<std::recursive_mutex> lock(*cLock);
            result = OCDoResource(nullptr, OC_REST_GET,
                                  resourceType.c_str(),
                                  nullptr, nullptr, connectivityType,
                                  static_cast<OCQualityOfService>(QoS),
                                  &cbdata,
                                  nullptr, 0);
        }
        else
        {
            delete context;
            result = OC_STACK_ERROR;
        }
        return result;
    }

    OCStackApplicationResult listenDeviceCallback(void* ctx, OCDoHandle handle,
            OCClientResponse* clientResponse)
    {
        ClientCallbackContext::DeviceListenContext* context =
            static_cast<ClientCallbackContext::DeviceListenContext*>(ctx);

        try
        {
            OCRepresentation rep = parseGetSetCallback(clientResponse);
            std::thread exec(context->callback, rep);
            exec.detach();
        }
        catch(OC::OCException& e)
        {
            oclog() <<"Exception in listenDeviceCallback, ignoring response: "
                <<e.what() <<std::flush;
        }

        return OC_STACK_KEEP_TRANSACTION;
    }

    OCStackResult InProcClientWrapper::ListenForDevice(const std::string& serviceUrl,
        const std::string& deviceURI, OCConnectivityType connectivityType,
        FindDeviceCallback& callback, QualityOfService QoS)
    {
        if(!callback)
        {
            return OC_STACK_INVALID_PARAM;
        }
        OCStackResult result;

        OCCallbackData cbdata = {0};
        ClientCallbackContext::DeviceListenContext* context =
            new ClientCallbackContext::DeviceListenContext();
        context->callback = callback;
        context->clientWrapper = shared_from_this();
        cbdata.context =  static_cast<void*>(context);
        cbdata.cb = listenDeviceCallback;
        cbdata.cd = [](void* c){delete static_cast<ClientCallbackContext::DeviceListenContext*>(c);};

        auto cLock = m_csdkLock.lock();
        if(cLock)
        {
            std::lock_guard<std::recursive_mutex> lock(*cLock);
            result = OCDoResource(nullptr, OC_REST_GET,
                                  deviceURI.c_str(),
                                  nullptr, nullptr, connectivityType,
                                  static_cast<OCQualityOfService>(QoS),
                                  &cbdata,
                                  nullptr, 0);
        }
        else
        {
            delete context;
            result = OC_STACK_ERROR;
        }
        return result;
    }

    void parseServerHeaderOptions(OCClientResponse* clientResponse,
                    HeaderOptions& serverHeaderOptions)
    {
        if(clientResponse)
        {
            // Parse header options from server
            uint16_t optionID;
            std::string optionData;

            for(int i = 0; i < clientResponse->numRcvdVendorSpecificHeaderOptions; i++)
            {
                optionID = clientResponse->rcvdVendorSpecificHeaderOptions[i].optionID;
                optionData = reinterpret_cast<const char*>
                                (clientResponse->rcvdVendorSpecificHeaderOptions[i].optionData);
                HeaderOption::OCHeaderOption headerOption(optionID, optionData);
                serverHeaderOptions.push_back(headerOption);
            }
        }
        else
        {
            // clientResponse is invalid
            // TODO check proper logging
            std::cout << " Invalid response " << std::endl;
        }
    }

    OCStackApplicationResult getResourceCallback(void* ctx, OCDoHandle handle,
        OCClientResponse* clientResponse)
    {
        ClientCallbackContext::GetContext* context =
            static_cast<ClientCallbackContext::GetContext*>(ctx);

        OCRepresentation rep;
        HeaderOptions serverHeaderOptions;
        OCStackResult result = clientResponse->result;
        if(result == OC_STACK_OK)
        {
            parseServerHeaderOptions(clientResponse, serverHeaderOptions);
            try
            {
                rep = parseGetSetCallback(clientResponse);
            }
            catch(OC::OCException& e)
            {
                result = e.code();
            }
        }

        std::thread exec(context->callback, serverHeaderOptions, rep, result);
        exec.detach();
        return OC_STACK_DELETE_TRANSACTION;
    }

    OCStackResult InProcClientWrapper::GetResourceRepresentation(const std::string& host,
        const std::string& uri, OCConnectivityType connectivityType,
        const QueryParamsMap& queryParams, const HeaderOptions& headerOptions,
        GetCallback& callback, QualityOfService QoS)
    {
        if(!callback)
        {
            return OC_STACK_INVALID_PARAM;
        }
        OCStackResult result;
        OCCallbackData cbdata = {0};

        ClientCallbackContext::GetContext* ctx = new ClientCallbackContext::GetContext();
        ctx->callback = callback;
        cbdata.context = static_cast<void*>(ctx);
        cbdata.cb = &getResourceCallback;
        cbdata.cd = [](void* c){delete static_cast<ClientCallbackContext::GetContext*>(c);};

        auto cLock = m_csdkLock.lock();

        if(cLock)
        {
            std::ostringstream os;
            os << host << assembleSetResourceUri(uri, queryParams).c_str();

            std::lock_guard<std::recursive_mutex> lock(*cLock);
            OCHeaderOption options[MAX_HEADER_OPTIONS];

            result = OCDoResource(nullptr, OC_REST_GET, os.str().c_str(),
                                  nullptr, nullptr, connectivityType,
                                  static_cast<OCQualityOfService>(QoS),
                                  &cbdata,
                                  assembleHeaderOptions(options, headerOptions),
                                  headerOptions.size());
        }
        else
        {
            delete ctx;
            result = OC_STACK_ERROR;
        }
        return result;
    }


    OCStackApplicationResult setResourceCallback(void* ctx, OCDoHandle handle,
        OCClientResponse* clientResponse)
    {
        ClientCallbackContext::SetContext* context =
            static_cast<ClientCallbackContext::SetContext*>(ctx);
        OCRepresentation attrs;
        HeaderOptions serverHeaderOptions;

        OCStackResult result = clientResponse->result;
        if (OC_STACK_OK               == result ||
            OC_STACK_RESOURCE_CREATED == result ||
            OC_STACK_RESOURCE_DELETED == result)
        {
            parseServerHeaderOptions(clientResponse, serverHeaderOptions);
            try
            {
                attrs = parseGetSetCallback(clientResponse);
            }
            catch(OC::OCException& e)
            {
                result = e.code();
            }
        }

        std::thread exec(context->callback, serverHeaderOptions, attrs, result);
        exec.detach();
        return OC_STACK_DELETE_TRANSACTION;
    }

    std::string InProcClientWrapper::assembleSetResourceUri(std::string uri,
        const QueryParamsMap& queryParams)
    {
        if(uri.back() == '/')
        {
            uri.resize(uri.size()-1);
        }

        ostringstream paramsList;
        if(queryParams.size() > 0)
        {
            paramsList << '?';
        }

        for(auto& param : queryParams)
        {
            paramsList << param.first <<'='<<param.second<<'&';
        }

        std::string queryString = paramsList.str();
        if(queryString.back() == '&')
        {
            queryString.resize(queryString.size() - 1);
        }

        std::string ret = uri + queryString;
        return ret;
    }

    std::string InProcClientWrapper::assembleSetResourcePayload(const OCRepresentation& rep)
    {
        MessageContainer ocInfo;
        ocInfo.addRepresentation(rep);
        return ocInfo.getJSONRepresentation(OCInfoFormat::IncludeOC);
    }

    OCStackResult InProcClientWrapper::PostResourceRepresentation(const std::string& host,
        const std::string& uri, OCConnectivityType connectivityType, const OCRepresentation& rep,
        const QueryParamsMap& queryParams, const HeaderOptions& headerOptions,
        PostCallback& callback, QualityOfService QoS)
    {
        if(!callback)
        {
            return OC_STACK_INVALID_PARAM;
        }
        OCStackResult result;
        OCCallbackData cbdata = {0};

        ClientCallbackContext::SetContext* ctx = new ClientCallbackContext::SetContext();
        ctx->callback = callback;
        cbdata.cb = &setResourceCallback;
        cbdata.cd = [](void* c){delete static_cast<ClientCallbackContext::SetContext*>(c);};
        cbdata.context = static_cast<void*>(ctx);

        // TODO: in the future the cstack should be combining these two strings!
        ostringstream os;
        os << host << assembleSetResourceUri(uri, queryParams).c_str();
        // TODO: end of above

        auto cLock = m_csdkLock.lock();

        if(cLock)
        {
            std::lock_guard<std::recursive_mutex> lock(*cLock);
            OCHeaderOption options[MAX_HEADER_OPTIONS];

            result = OCDoResource(nullptr, OC_REST_POST,
                                  os.str().c_str(), nullptr,
                                  assembleSetResourcePayload(rep).c_str(), connectivityType,
                                  static_cast<OCQualityOfService>(QoS),
                                  &cbdata,
                                  assembleHeaderOptions(options, headerOptions),
                                  headerOptions.size());
        }
        else
        {
            delete ctx;
            result = OC_STACK_ERROR;
        }

        return result;
    }

    OCStackResult InProcClientWrapper::PutResourceRepresentation(const std::string& host,
        const std::string& uri, OCConnectivityType connectivityType, const OCRepresentation& rep,
        const QueryParamsMap& queryParams, const HeaderOptions& headerOptions,
        PutCallback& callback, QualityOfService QoS)
    {
        if(!callback)
        {
            return OC_STACK_INVALID_PARAM;
        }
        OCStackResult result;
        OCCallbackData cbdata = {0};

        ClientCallbackContext::SetContext* ctx = new ClientCallbackContext::SetContext();
        ctx->callback = callback;
        cbdata.cb = &setResourceCallback;
        cbdata.cd = [](void* c){delete static_cast<ClientCallbackContext::SetContext*>(c);};
        cbdata.context = static_cast<void*>(ctx);

        // TODO: in the future the cstack should be combining these two strings!
        ostringstream os;
        os << host << assembleSetResourceUri(uri, queryParams).c_str();
        // TODO: end of above

        auto cLock = m_csdkLock.lock();

        if(cLock)
        {
            std::lock_guard<std::recursive_mutex> lock(*cLock);
            OCDoHandle handle;
            OCHeaderOption options[MAX_HEADER_OPTIONS];

            result = OCDoResource(&handle, OC_REST_PUT,
                                  os.str().c_str(), nullptr,
                                  assembleSetResourcePayload(rep).c_str(), connectivityType,
                                  static_cast<OCQualityOfService>(QoS),
                                  &cbdata,
                                  assembleHeaderOptions(options, headerOptions),
                                  headerOptions.size());
        }
        else
        {
            delete ctx;
            result = OC_STACK_ERROR;
        }

        return result;
    }

    OCStackApplicationResult deleteResourceCallback(void* ctx, OCDoHandle handle,
        OCClientResponse* clientResponse)
    {
        ClientCallbackContext::DeleteContext* context =
            static_cast<ClientCallbackContext::DeleteContext*>(ctx);
        HeaderOptions serverHeaderOptions;

        if(clientResponse->result == OC_STACK_OK)
        {
            parseServerHeaderOptions(clientResponse, serverHeaderOptions);
        }
        std::thread exec(context->callback, serverHeaderOptions, clientResponse->result);
        exec.detach();
        return OC_STACK_DELETE_TRANSACTION;
    }

    OCStackResult InProcClientWrapper::DeleteResource(const std::string& host,
        const std::string& uri, OCConnectivityType connectivityType,
        const HeaderOptions& headerOptions, DeleteCallback& callback, QualityOfService QoS)
    {
        if(!callback)
        {
            return OC_STACK_INVALID_PARAM;
        }
        OCStackResult result;
        OCCallbackData cbdata = {0};

        ClientCallbackContext::DeleteContext* ctx = new ClientCallbackContext::DeleteContext();
        ctx->callback = callback;
        cbdata.cb = &deleteResourceCallback;
        cbdata.cd = [](void* c){delete static_cast<ClientCallbackContext::DeleteContext*>(c);};
        cbdata.context = static_cast<void*>(ctx);

        ostringstream os;
        os << host << uri;

        auto cLock = m_csdkLock.lock();

        if(cLock)
        {
            OCHeaderOption options[MAX_HEADER_OPTIONS];

            std::lock_guard<std::recursive_mutex> lock(*cLock);

            result = OCDoResource(nullptr, OC_REST_DELETE,
                                  os.str().c_str(), nullptr,
                                  nullptr, connectivityType,
                                  static_cast<OCQualityOfService>(m_cfg.QoS),
                                  &cbdata,
                                  assembleHeaderOptions(options, headerOptions),
                                  headerOptions.size());
        }
        else
        {
            delete ctx;
            result = OC_STACK_ERROR;
        }

        return result;
    }

    OCStackApplicationResult observeResourceCallback(void* ctx, OCDoHandle handle,
        OCClientResponse* clientResponse)
    {
        ClientCallbackContext::ObserveContext* context =
            static_cast<ClientCallbackContext::ObserveContext*>(ctx);
        OCRepresentation attrs;
        HeaderOptions serverHeaderOptions;
        uint32_t sequenceNumber = clientResponse->sequenceNumber;
        OCStackResult result = clientResponse->result;
        if(clientResponse->result == OC_STACK_OK)
        {
            parseServerHeaderOptions(clientResponse, serverHeaderOptions);
            try
            {
                attrs = parseGetSetCallback(clientResponse);
            }
            catch(OC::OCException& e)
            {
                result = e.code();
            }
        }
        std::thread exec(context->callback, serverHeaderOptions, attrs,
                    result, sequenceNumber);
        exec.detach();
        if(sequenceNumber == OC_OBSERVE_DEREGISTER)
        {
            return OC_STACK_DELETE_TRANSACTION;
        }
        return OC_STACK_KEEP_TRANSACTION;
    }

    OCStackResult InProcClientWrapper::ObserveResource(ObserveType observeType, OCDoHandle* handle,
        const std::string& host, const std::string& uri, OCConnectivityType connectivityType,
        const QueryParamsMap& queryParams, const HeaderOptions& headerOptions,
        ObserveCallback& callback, QualityOfService QoS)
    {
        if(!callback)
        {
            return OC_STACK_INVALID_PARAM;
        }
        OCStackResult result;
        OCCallbackData cbdata = {0};

        ClientCallbackContext::ObserveContext* ctx = new ClientCallbackContext::ObserveContext();
        ctx->callback = callback;
        cbdata.context = static_cast<void*>(ctx);
        cbdata.cb = &observeResourceCallback;
        cbdata.cd = [](void* c){delete static_cast<ClientCallbackContext::ObserveContext*>(c);};

        OCMethod method;
        if (observeType == ObserveType::Observe)
        {
            method = OC_REST_OBSERVE;
        }
        else if (observeType == ObserveType::ObserveAll)
        {
            method = OC_REST_OBSERVE_ALL;
        }
        else
        {
            method = OC_REST_OBSERVE_ALL;
        }

        auto cLock = m_csdkLock.lock();

        if(cLock)
        {
            std::ostringstream os;
            os << host << assembleSetResourceUri(uri, queryParams).c_str();

            std::lock_guard<std::recursive_mutex> lock(*cLock);
            OCHeaderOption options[MAX_HEADER_OPTIONS];

            result = OCDoResource(handle, method,
                                  os.str().c_str(), nullptr,
                                  nullptr, connectivityType,
                                  static_cast<OCQualityOfService>(QoS),
                                  &cbdata,
                                  assembleHeaderOptions(options, headerOptions),
                                  headerOptions.size());
        }
        else
        {
            delete ctx;
            return OC_STACK_ERROR;
        }

        return result;
    }

    OCStackResult InProcClientWrapper::CancelObserveResource(OCDoHandle handle,
        const std::string& host, const std::string& uri, const HeaderOptions& headerOptions,
        QualityOfService QoS)
    {
        OCStackResult result;
        auto cLock = m_csdkLock.lock();

        if(cLock)
        {
            std::lock_guard<std::recursive_mutex> lock(*cLock);
            OCHeaderOption options[MAX_HEADER_OPTIONS];

            result = OCCancel(handle,
                    static_cast<OCQualityOfService>(QoS),
                    assembleHeaderOptions(options, headerOptions),
                    headerOptions.size());
        }
        else
        {
            result = OC_STACK_ERROR;
        }

        return result;
    }

    OCStackApplicationResult subscribePresenceCallback(void* ctx, OCDoHandle handle,
            OCClientResponse* clientResponse)
    {
        ostringstream os;
        uint16_t port;
        uint8_t a;
        uint8_t b;
        uint8_t c;
        uint8_t d;

        if(OCDevAddrToIPv4Addr(clientResponse->addr, &a, &b, &c, &d) == 0 &&
                OCDevAddrToPort(clientResponse->addr, &port) == 0)
        {
            os<<static_cast<int>(a)<<"."<<static_cast<int>(b)<<"."<<static_cast<int>(c)
                    <<"."<<static_cast<int>(d)<<":"<<static_cast<int>(port);

            ClientCallbackContext::SubscribePresenceContext* context =
                static_cast<ClientCallbackContext::SubscribePresenceContext*>(ctx);

            std::thread exec(context->callback, clientResponse->result,
                    clientResponse->sequenceNumber, os.str());

            exec.detach();
        }
        else
        {
            oclog() << "subscribePresenceCallback(): OCDevAddrToIPv4Addr() or OCDevAddrToPort() "
                    <<"failed"<< std::flush;
        }
        return OC_STACK_KEEP_TRANSACTION;
    }

    OCStackResult InProcClientWrapper::SubscribePresence(OCDoHandle* handle,
        const std::string& host, const std::string& resourceType,
        OCConnectivityType connectivityType, SubscribeCallback& presenceHandler)
    {
        if(!presenceHandler)
        {
            return OC_STACK_INVALID_PARAM;
        }
        OCCallbackData cbdata = {0};

        ClientCallbackContext::SubscribePresenceContext* ctx =
            new ClientCallbackContext::SubscribePresenceContext();
        ctx->callback = presenceHandler;
        cbdata.cb = &subscribePresenceCallback;
        cbdata.context = static_cast<void*>(ctx);
        cbdata.cd = [](void* c)
            {delete static_cast<ClientCallbackContext::SubscribePresenceContext*>(c);};
        auto cLock = m_csdkLock.lock();

        std::ostringstream os;
        os << host << OC_PRESENCE_URI;

        if(!resourceType.empty())
        {
            os << "?rt=" << resourceType;
        }

        if(!cLock)
        {
            delete ctx;
            return OC_STACK_ERROR;
        }

        return OCDoResource(handle, OC_REST_PRESENCE, os.str().c_str(), nullptr, nullptr,
                            connectivityType, OC_LOW_QOS, &cbdata, NULL, 0);
    }

    OCStackResult InProcClientWrapper::UnsubscribePresence(OCDoHandle handle)
    {
        OCStackResult result;
        auto cLock = m_csdkLock.lock();

        if(cLock)
        {
            std::lock_guard<std::recursive_mutex> lock(*cLock);
            result = OCCancel(handle, OC_LOW_QOS, NULL, 0);
        }
        else
        {
            result = OC_STACK_ERROR;
        }

        return result;
    }

    OCStackResult InProcClientWrapper::GetDefaultQos(QualityOfService& qos)
    {
        qos = m_cfg.QoS;
        return OC_STACK_OK;
    }

    OCHeaderOption* InProcClientWrapper::assembleHeaderOptions(OCHeaderOption options[],
           const HeaderOptions& headerOptions)
    {
        int i = 0;

        if( headerOptions.size() == 0)
        {
            return nullptr;
        }

        for (auto it=headerOptions.begin(); it != headerOptions.end(); ++it)
        {
            options[i].protocolID = OC_COAP_ID;
            options[i].optionID = static_cast<uint16_t>(it->getOptionID());
            options[i].optionLength = (it->getOptionData()).length() + 1;
            memcpy(options[i].optionData, (it->getOptionData()).c_str(),
                    (it->getOptionData()).length() + 1);
            i++;
        }

        return options;
    }
}

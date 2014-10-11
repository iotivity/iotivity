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

#include <new>

#include "InProcClientWrapper.h"
#include "ocstack.h"

#include "OCPlatform.h"
#include "OCResource.h"

#include <cereal/cereal.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/archives/json.hpp>

using namespace std;

namespace OC
{
    InProcClientWrapper::InProcClientWrapper(OC::OCPlatform& owner,
        std::weak_ptr<std::recursive_mutex> csdkLock, PlatformConfig cfg)
            : IClientWrapper(owner),
              m_threadRun(false), m_csdkLock(csdkLock),
              m_owner(owner),
              m_cfg { cfg }
    {
        // if the config type is server, we ought to never get called.  If the config type
        // is both, we count on the server to run the thread and do the initialize
        if(m_cfg.mode == ModeType::Client)
        {
            OCStackResult result = OCInit(m_cfg.ipAddress.c_str(), m_cfg.port, OC_CLIENT);

            if(OC_STACK_OK != result)
            {
                throw InitializeException("Error Initializing Stack", result);
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

        OCStop();
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



    std::string convertOCAddrToString(OCDevAddr& addr)
    {
        // TODO: we currently assume this is a IPV4 address, need to figure out the actual value

        uint8_t a, b, c, d;
        uint16_t port;

        if(OCDevAddrToIPv4Addr(&addr, &a, &b, &c, &d) ==0 && OCDevAddrToPort(&addr, &port)==0)
        {
            ostringstream os;
            os << "coap://"<<(int)a<<'.'<<(int)b<<'.'<<(int)c<<'.'<<(int)d<<':'<<(int)port;
            return os.str();
        }
        else
        {
            return "INVALID IP";
        }
    }

    struct ListenContext
    {
        FindCallback              callback;
        IClientWrapper::Ptr       clientWrapper;
        OC::OCPlatform const*     owner;          // observing ptr
    };


    const std::string URIKEY = "href";
    const std::string OBSERVABLEKEY = "obs";
    const std::string RESOURCETYPESKEY= "rt";
    const std::string INTERFACESKEY = "if";
    const std::string PROPERTYKEY = "prop";
    const std::string REPKEY = "rep";

    class ListenOCContainer
    {
        class ListenResourceContainer
        {
            class ListenResourcePropertiesContainer
            {
                friend class cereal::access;
                friend class ListenResourceContainer;

                template<class Archive>
                void serialize(Archive& ar)
                {
                    try
                    {
                        ar(cereal::make_nvp(OBSERVABLEKEY, m_observable));
                    }
                    catch(cereal::Exception&)
                    {
                        // we swallow this exception, since it means the key
                        // doesn't exist, allowing these to be optional
                    }

                    try
                    {
                        ar(cereal::make_nvp(RESOURCETYPESKEY,m_resourceTypes));
                    }
                    catch(cereal::Exception&)
                    {}
                    try
                    {
                        ar(cereal::make_nvp(INTERFACESKEY, m_interfaces));
                    }
                    catch(cereal::Exception&)
                    {}
                }

                bool m_observable;
                std::vector<std::string> m_resourceTypes;
                std::vector<std::string> m_interfaces;
            };

            friend class cereal::access;
            friend class ListenOCContainer;
            template <class Archive>
            void serialize(Archive& ar)
            {
                ar(cereal::make_nvp(URIKEY, m_uri));
                ar(cereal::make_nvp(PROPERTYKEY, m_props));
                // todo
            }


            std::string m_uri;
            ListenResourcePropertiesContainer m_props;
            bool observable()
            {
                return m_props.m_observable;
            }
            std::vector<std::string> resourceTypes()
            {
                return m_props.m_resourceTypes;
            }
            std::vector<std::string> interfaces()
            {
                return m_props.m_interfaces;
            }
        };

        private:
            friend class cereal::access;
            template <class Archive>
            void serialize(Archive& ar)
            {
                std::vector<ListenResourceContainer> resources;
                ar(resources);
            }
        public:
            ListenOCContainer(std::weak_ptr<IClientWrapper> cw, const std::string& host):
                m_clientWrapper(cw), m_host(host)
            {
            }
            void LoadFromJson(std::stringstream& json)
            {
                m_resources.clear();
                cereal::JSONInputArchive archive(json);

                std::vector<ListenResourceContainer> resources;
                archive(cereal::make_nvp("oc", resources));

                for(auto& res : resources)
                {
                    try
                    {
                        m_resources.push_back(std::shared_ptr<OCResource>(
                            new OCResource(m_clientWrapper, m_host,
                            res.m_uri, res.observable(), res.resourceTypes(),
                            res.interfaces())));

                    }
                    catch(ResourceInitException& e)
                    {
                        oclog() << "listenCallback(): failed to create resource: " << e.what()
                                << std::flush;
                    }
                }
            }
            const std::vector<std::shared_ptr<OCResource>>& Resources() const
            {
                return m_resources;
            }
        private:
            std::vector<std::shared_ptr<OC::OCResource>> m_resources;
            std::weak_ptr<IClientWrapper> m_clientWrapper;
            std::string m_host;
    };


    OCStackApplicationResult listenCallback(void* ctx, OCDoHandle handle,
        OCClientResponse* clientResponse)
    {
        ListenContext* context = static_cast<ListenContext*>(ctx);

        if(clientResponse->result != OC_STACK_OK)
        {
            oclog() << "listenCallback(): failed to create resource. clientResponse: "
                    << clientResponse->result
                    << std::flush;

            return OC_STACK_KEEP_TRANSACTION;
        }

        std::stringstream requestStream;
        requestStream << clientResponse->resJSONPayload;
std::cout<<"ERICH::::"<<clientResponse->resJSONPayload<<std::endl;
        std::string host = convertOCAddrToString(*clientResponse->addr);
        ListenOCContainer container(context->clientWrapper,host);
        container.LoadFromJson(requestStream);


        // loop to ensure valid construction of all resources
        for(auto resource : container.Resources())
        {
            std::thread exec(context->callback, resource);
            exec.detach();
        }
        return OC_STACK_KEEP_TRANSACTION;
    }

    OCStackResult InProcClientWrapper::ListenForResource(const std::string& serviceUrl,
        const std::string& resourceType, FindCallback& callback)
    {
        OCStackResult result;

        OCCallbackData cbdata = {0};

        ListenContext* context = new ListenContext();
        context->callback = callback;
        context->clientWrapper = shared_from_this();
        context->owner = &m_owner;

        cbdata.context =  static_cast<void*>(context);
        cbdata.cb = listenCallback;
        cbdata.cd = [](void* c){delete static_cast<ListenContext*>(c);};

        auto cLock = m_csdkLock.lock();
        if(cLock)
        {
            std::lock_guard<std::recursive_mutex> lock(*cLock);
            OCDoHandle handle;
            result = OCDoResource(&handle, OC_REST_GET,
                                  resourceType.c_str(),
                                  nullptr, nullptr,
                                  static_cast<OCQualityOfService>(m_cfg.QoS),
                                  &cbdata,
                                  NULL, 0);
        }
        else
        {
            result = OC_STACK_ERROR;
        }
        return result;
    }

    struct GetContext
    {
        GetCallback callback;
    };

    struct SetContext
    {
        PutCallback callback;
    };


    OCRepresentation parseGetSetCallback(OCClientResponse* clientResponse)
    {
        std::stringstream requestStream;

        requestStream<<clientResponse->resJSONPayload;
        if(strlen((char*)clientResponse->resJSONPayload) == 0)
        {
            return OCRepresentation();
        }

        boost::property_tree::ptree root;
        try
        {
            boost::property_tree::read_json(requestStream, root);
        }
        catch(boost::property_tree::json_parser::json_parser_error &e)
        {
            return OCRepresentation();
        }
        boost::property_tree::ptree payload = root.get_child("oc", boost::property_tree::ptree());
        OCRepresentation root_resource;
        std::vector<OCRepresentation> children;
        bool isRoot = true;
        for ( auto payloadItr : payload)
        {
            OCRepresentation child;
            try
            {
                auto resourceNode = payloadItr.second;
                std::string uri = resourceNode.get<std::string>(URIKEY, "");

                if (isRoot)
                {
                    root_resource.setUri(uri);
                }
                else
                {
                    child.setUri(uri);
                }

                if( resourceNode.count(PROPERTYKEY) != 0 )
                {
                    std::vector<std::string> rTs;
                    std::vector<std::string> ifaces;
                    boost::property_tree::ptree properties =
                        resourceNode.get_child(PROPERTYKEY, boost::property_tree::ptree());

                    boost::property_tree::ptree rT =
                        properties.get_child(RESOURCETYPESKEY, boost::property_tree::ptree());
                    for(auto itr : rT)
                    {
                        rTs.push_back(itr.second.data());
                    }

                    boost::property_tree::ptree iF =
                        properties.get_child(INTERFACESKEY, boost::property_tree::ptree());
                    for(auto itr : iF)
                    {
                        ifaces.push_back(itr.second.data());
                    }
                    if (isRoot)
                    {
                        root_resource.setResourceInterfaces(ifaces);
                        root_resource.setResourceTypes(rTs);
                    }
                    else
                    {
                        child.setResourceInterfaces(ifaces);
                        child.setResourceTypes(rTs);
                    }
                }

                if( resourceNode.count(REPKEY) != 0 )
                {
                    boost::property_tree::ptree rep =
                        resourceNode.get_child(REPKEY, boost::property_tree::ptree());
                    AttributeMap attrs;
                    for( auto item : rep)
                    {
                        std::string name = item.first.data();
                        std::string value = item.second.data();
                        attrs[name] = value;
                    }
                    if (isRoot)
                    {
                        root_resource.setAttributeMap(attrs);
                    }
                    else
                    {
                        child.setAttributeMap(attrs);
                    }
                }

                if (!isRoot)
                    children.push_back(child);
            }
            catch (...)
            {
                // TODO
            }

            isRoot = false;
         }

         root_resource.setChildren(children);

        return root_resource;
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
        GetContext* context = static_cast<GetContext*>(ctx);

        OCRepresentation rep;
        HeaderOptions serverHeaderOptions;
        if(clientResponse->result == OC_STACK_OK)
        {
            parseServerHeaderOptions(clientResponse, serverHeaderOptions);
            rep = parseGetSetCallback(clientResponse);
        }

        std::thread exec(context->callback, serverHeaderOptions, rep, clientResponse->result);
        exec.detach();
        return OC_STACK_DELETE_TRANSACTION;
    }

    OCStackResult InProcClientWrapper::GetResourceRepresentation(const std::string& host,
        const std::string& uri, const QueryParamsMap& queryParams,
        const HeaderOptions& headerOptions, GetCallback& callback)
    {
        OCStackResult result;
        OCCallbackData cbdata = {0};

        GetContext* ctx = new GetContext();
        ctx->callback = callback;
        cbdata.context = static_cast<void*>(ctx);
        cbdata.cb = &getResourceCallback;
        cbdata.cd = [](void* c){delete static_cast<GetContext*>(c);};

        auto cLock = m_csdkLock.lock();

        if(cLock)
        {
            std::ostringstream os;
            os << host << assembleSetResourceUri(uri, queryParams).c_str();

            std::lock_guard<std::recursive_mutex> lock(*cLock);
            OCDoHandle handle;
            OCHeaderOption options[MAX_HEADER_OPTIONS];

            assembleHeaderOptions(options, headerOptions);
            result = OCDoResource(&handle, OC_REST_GET, os.str().c_str(),
                                  nullptr, nullptr,
                                  static_cast<OCQualityOfService>(m_cfg.QoS),
                                  &cbdata,
                                  options, headerOptions.size());
        }
        else
        {
            result = OC_STACK_ERROR;
        }
        return result;
    }


    OCStackApplicationResult setResourceCallback(void* ctx, OCDoHandle handle,
        OCClientResponse* clientResponse)
    {
        SetContext* context = static_cast<SetContext*>(ctx);
        OCRepresentation attrs;
        HeaderOptions serverHeaderOptions;

        if(clientResponse->result == OC_STACK_OK)
        {
            parseServerHeaderOptions(clientResponse, serverHeaderOptions);
            attrs = parseGetSetCallback(clientResponse);
        }
        std::thread exec(context->callback, serverHeaderOptions, attrs, clientResponse->result);
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
        ostringstream payload;
        // TODO need to change the format to "{"oc":[]}"
        payload << "{\"oc\":";

        payload << rep.getJSONRepresentation();

        payload << "}";
        return payload.str();
    }

    OCStackResult InProcClientWrapper::PostResourceRepresentation(const std::string& host,
        const std::string& uri, const OCRepresentation& rep,
        const QueryParamsMap& queryParams, const HeaderOptions& headerOptions,
         PostCallback& callback)
    {
        OCStackResult result;
        OCCallbackData cbdata = {0};

        SetContext* ctx = new SetContext();
        ctx->callback = callback;
        cbdata.cb = &setResourceCallback;
        cbdata.cd = [](void* c){delete static_cast<SetContext*>(c);};
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
            OCDoHandle handle;

            assembleHeaderOptions(options, headerOptions);
            result = OCDoResource(&handle, OC_REST_POST,
                                  os.str().c_str(), nullptr,
                                  assembleSetResourcePayload(rep).c_str(),
                                  static_cast<OCQualityOfService>(m_cfg.QoS),
                                  &cbdata, options, headerOptions.size());
        }
        else
        {
            result = OC_STACK_ERROR;
        }

        return result;
    }


    OCStackResult InProcClientWrapper::PutResourceRepresentation(const std::string& host,
        const std::string& uri, const OCRepresentation& rep,
        const QueryParamsMap& queryParams, const HeaderOptions& headerOptions,
        PutCallback& callback)
    {
        OCStackResult result;
        OCCallbackData cbdata = {0};

        SetContext* ctx = new SetContext();
        ctx->callback = callback;
        cbdata.cb = &setResourceCallback;
        cbdata.cd = [](void* c){delete static_cast<SetContext*>(c);};
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

            assembleHeaderOptions(options, headerOptions);
            result = OCDoResource(&handle, OC_REST_PUT,
                                  os.str().c_str(), nullptr,
                                  assembleSetResourcePayload(rep).c_str(),
                                  static_cast<OCQualityOfService>(m_cfg.QoS),
                                  &cbdata,
                                  options, headerOptions.size());
        }
        else
        {
            result = OC_STACK_ERROR;
        }

        return result;
    }

    struct ObserveContext
    {
        ObserveCallback callback;
    };

    OCStackApplicationResult observeResourceCallback(void* ctx, OCDoHandle handle,
        OCClientResponse* clientResponse)
    {
        ObserveContext* context = static_cast<ObserveContext*>(ctx);
        OCRepresentation attrs;
        HeaderOptions serverHeaderOptions;
        uint32_t sequenceNumber = clientResponse->sequenceNumber;

        if(clientResponse->result == OC_STACK_OK)
        {
            parseServerHeaderOptions(clientResponse, serverHeaderOptions);
            attrs = parseGetSetCallback(clientResponse);
        }
        std::thread exec(context->callback, serverHeaderOptions, attrs,
                    clientResponse->result, sequenceNumber);
        exec.detach();
        return OC_STACK_KEEP_TRANSACTION;
    }

    OCStackResult InProcClientWrapper::ObserveResource(ObserveType observeType, OCDoHandle* handle,
        const std::string& host, const std::string& uri, const QueryParamsMap& queryParams,
        const HeaderOptions& headerOptions, ObserveCallback& callback)
    {
        OCStackResult result;
        OCCallbackData cbdata = {0};

        ObserveContext* ctx = new ObserveContext();
        ctx->callback = callback;
        cbdata.context = static_cast<void*>(ctx);
        cbdata.cb = &observeResourceCallback;
        cbdata.cd = [](void* c){delete static_cast<ObserveContext*>(c);};

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

            assembleHeaderOptions(options, headerOptions);
            result = OCDoResource(handle, method,
                                  os.str().c_str(), nullptr,
                                  nullptr,
                                  static_cast<OCQualityOfService>(m_cfg.QoS),
                                  &cbdata,
                                  options, headerOptions.size());
        }
        else
        {
            return OC_STACK_ERROR;
        }

        return result;
    }

    OCStackResult InProcClientWrapper::CancelObserveResource(OCDoHandle handle,
        const std::string& host, const std::string& uri, const HeaderOptions& headerOptions)
    {
        OCStackResult result;
        auto cLock = m_csdkLock.lock();

        if(cLock)
        {
            std::lock_guard<std::recursive_mutex> lock(*cLock);
            OCHeaderOption options[MAX_HEADER_OPTIONS];

            assembleHeaderOptions(options, headerOptions);
            result = OCCancel(handle, OC_NON_CONFIRMABLE, options, headerOptions.size());
        }
        else
        {
            result = OC_STACK_ERROR;
        }

        return result;
    }

    struct SubscribePresenceContext
    {
        SubscribeCallback callback;
    };

    OCStackApplicationResult subscribePresenceCallback(void* ctx, OCDoHandle handle,
        OCClientResponse* clientResponse)
    {
        SubscribePresenceContext* context = static_cast<SubscribePresenceContext*>(ctx);
        std::thread exec(context->callback, clientResponse->result, clientResponse->sequenceNumber);

        exec.detach();
        return OC_STACK_KEEP_TRANSACTION;
    }

    OCStackResult InProcClientWrapper::SubscribePresence(OCDoHandle* handle,
        const std::string& host, SubscribeCallback& presenceHandler)
    {
        OCCallbackData cbdata = {0};

        SubscribePresenceContext* ctx = new SubscribePresenceContext();
        ctx->callback = presenceHandler;
        cbdata.cb = &subscribePresenceCallback;
        cbdata.context = static_cast<void*>(ctx);
        cbdata.cd = [](void* c){delete static_cast<SubscribePresenceContext*>(c);};
        auto cLock = m_csdkLock.lock();

        std::ostringstream os;

        os << host << "/oc/presence";

        if(!cLock)
            return OC_STACK_ERROR;

        return OCDoResource(handle, OC_REST_PRESENCE, os.str().c_str(), nullptr, nullptr,
                            OC_NON_CONFIRMABLE, &cbdata, NULL, 0);
    }

    OCStackResult InProcClientWrapper::UnsubscribePresence(OCDoHandle handle)
    {
        OCStackResult result;
        auto cLock = m_csdkLock.lock();

        if(cLock)
        {
            std::lock_guard<std::recursive_mutex> lock(*cLock);
            result = OCCancel(handle, OC_NON_CONFIRMABLE, NULL, 0);
        }
        else
        {
            result = OC_STACK_ERROR;
        }

        return result;
    }

    void InProcClientWrapper::assembleHeaderOptions(OCHeaderOption options[],
           const HeaderOptions& headerOptions)
    {
        int i = 0;

        for (auto it=headerOptions.begin(); it != headerOptions.end(); ++it)
        {
            options[i].protocolID = OC_COAP_ID;
            options[i].optionID = static_cast<uint16_t>(it->getOptionID());
            options[i].optionLength = (it->getOptionData()).length() + 1;
            memcpy(options[i].optionData, (it->getOptionData()).c_str(),
                    (it->getOptionData()).length() + 1);
            i++;
        }
    }
}

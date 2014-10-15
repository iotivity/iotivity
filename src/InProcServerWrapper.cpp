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

#include <random>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <map>
#include <sstream>
#include <string>

#include <InProcServerWrapper.h>
#include <InitializeException.h>
#include <OCResourceRequest.h>
#include <OCResourceResponse.h>
#include <ocstack.h>
#include <OCApi.h>
#include <OCPlatform.h>
#include <OCUtilities.h>

using namespace std;
using namespace OC;

std::map <OCResourceHandle, OC::EntityHandler>  entityHandlerMap;
std::map <OCResourceHandle, std::string> resourceUriMap;
EntityHandler defaultDeviceEntityHandler = 0;

void formResourceRequest(OCEntityHandlerFlag flag,
                         OCEntityHandlerRequest * entityHandlerRequest,
                         std::shared_ptr<OCResourceRequest> pRequest)
{
    if(flag & OC_INIT_FLAG)
    {
        pRequest->setRequestHandlerFlag(OC::RequestHandlerFlag::InitFlag);
    }

    if(flag & OC_REQUEST_FLAG)
    {
        pRequest->setRequestHandlerFlag(OC::RequestHandlerFlag::RequestFlag);

        if(entityHandlerRequest)
        {
            if(entityHandlerRequest->query)
            {
                std::string querystr(reinterpret_cast<char*>(entityHandlerRequest->query));

                OC::Utilities::QueryParamsKeyVal qp = OC::Utilities::getQueryParams(querystr);

                if(qp.size() > 0)
                {
                    pRequest->setQueryParams(qp);
                }
            }
            if(entityHandlerRequest->numRcvdVendorSpecificHeaderOptions != 0)
            {
                //Set the header options here.
                uint16_t optionID;
                std::string optionData;
                HeaderOptions headerOptions;

                for(int i = 0; i < MAX_HEADER_OPTIONS; i++)
                {
                    optionID = entityHandlerRequest->rcvdVendorSpecificHeaderOptions[i].optionID;
                    optionData = reinterpret_cast<const char*>
                             (entityHandlerRequest->rcvdVendorSpecificHeaderOptions[i].optionData);
                    HeaderOption::OCHeaderOption headerOption(optionID, optionData);
                    headerOptions.push_back(headerOption);
                }
                pRequest->setHeaderOptions(headerOptions);
            }

            if(OC_REST_GET == entityHandlerRequest->method)
            {
                // TODO Why strings : "GET"??
                pRequest->setRequestType("GET");
            }
            else if(OC_REST_PUT == entityHandlerRequest->method)
            {
                pRequest->setRequestType("PUT");
                pRequest->setPayload(std::string(reinterpret_cast<const char*>
                                            (entityHandlerRequest->reqJSONPayload)));
            }
            else if(OC_REST_POST == entityHandlerRequest->method)
            {
                pRequest->setRequestType("POST");
                pRequest->setPayload(std::string(reinterpret_cast<const char*>
                                            (entityHandlerRequest->reqJSONPayload)));
            }
            else if(OC_REST_DELETE == entityHandlerRequest->method)
            {
                pRequest->setRequestType("DELETE");
            }
        }
    }

    if(flag & OC_OBSERVE_FLAG)
    {
        pRequest->setRequestHandlerFlag(
                   OC::RequestHandlerFlag::RequestFlag | OC::RequestHandlerFlag::ObserverFlag);
        if(entityHandlerRequest->obsInfo)
        {
            OC::ObservationInfo observationInfo;
            observationInfo.action = (OC::ObserveAction) entityHandlerRequest->obsInfo->action;
            observationInfo.obsId = entityHandlerRequest->obsInfo->obsId;
            pRequest->setObservationInfo(observationInfo);
        }
    }
}

void processResourceResponse(OCEntityHandlerFlag flag,
                             OCEntityHandlerRequest * entityHandlerRequest,
                             std::shared_ptr<OCResourceResponse> pResponse)
{
    if(flag & OC_REQUEST_FLAG)
    {
        // TODO we could use const reference
        std::string payLoad;
        HeaderOptions serverHeaderOptions;

        if(pResponse)
        {
            payLoad = pResponse->getPayload();
            serverHeaderOptions = pResponse->getHeaderOptions();
        }
        else
        {
            throw OCException("Response is NULL", OC_STACK_MALFORMED_RESPONSE);
        }

        if (payLoad.size() < entityHandlerRequest->resJSONPayloadLen)
        {
            int i = 0;
            entityHandlerRequest->numSendVendorSpecificHeaderOptions =
                        serverHeaderOptions.size();
            for (auto it=serverHeaderOptions.begin(); it != serverHeaderOptions.end(); ++it)
            {
                entityHandlerRequest->sendVendorSpecificHeaderOptions[i].protocolID = OC_COAP_ID;
                entityHandlerRequest->sendVendorSpecificHeaderOptions[i].optionID =
                        static_cast<uint16_t>(it->getOptionID());
                entityHandlerRequest->sendVendorSpecificHeaderOptions[i].optionLength =
                        (it->getOptionData()).length() + 1;
                memcpy(entityHandlerRequest->sendVendorSpecificHeaderOptions[i].optionData,
                        (it->getOptionData()).c_str(),
                        (it->getOptionData()).length() + 1);
                i++;
            }

            strncpy((char*)entityHandlerRequest->resJSONPayload,
                        payLoad.c_str(),
                        entityHandlerRequest->resJSONPayloadLen);
        }
        else
        {
            throw OCException("Payload overflow", OC_STACK_MALFORMED_RESPONSE);
        }
    }

}

OCEntityHandlerResult DefaultEntityHandlerWrapper(OCEntityHandlerFlag flag,
                                                  OCEntityHandlerRequest * entityHandlerRequest,
                                                  char* uri)
{
    OC::oclog() << "In Default device entity handler wrapper";

    if(NULL == entityHandlerRequest)
    {
        oclog() << "Entity handler request is NULL.";
        return OC_EH_ERROR;
    }

    auto pRequest = std::make_shared<OC::OCResourceRequest>();
    auto pResponse = std::make_shared<OC::OCResourceResponse>();

    formResourceRequest(flag, entityHandlerRequest, pRequest);

    pRequest->setResourceUri(std::string(uri));

    if(defaultDeviceEntityHandler)
    {
        defaultDeviceEntityHandler(pRequest, pResponse);
    }
    else
    {
        oclog() << "Default device entity handler was not set.";
        return OC_EH_ERROR;
    }

    processResourceResponse(flag, entityHandlerRequest, pResponse);

    return OC_EH_OK;
}


OCEntityHandlerResult EntityHandlerWrapper(OCEntityHandlerFlag flag,
                                           OCEntityHandlerRequest * entityHandlerRequest)
{
    oclog() << "\nIn entity handler wrapper: " << endl;

    if(NULL == entityHandlerRequest)
    {
        oclog() << "Entity handler request is NULL."  << endl;
        return OC_EH_ERROR;
    }

    auto pRequest = std::make_shared<OC::OCResourceRequest>();
    auto pResponse = std::make_shared<OC::OCResourceResponse>();

    formResourceRequest(flag, entityHandlerRequest, pRequest);

    // Finding the corresponding URI for a resource handle and set the URI in the request
    auto resourceUriEntry = resourceUriMap.find(entityHandlerRequest->resource);
    if(resourceUriEntry != resourceUriMap.end())
    {
        pRequest->setResourceUri(resourceUriEntry->second);
    }
    else
    {
        oclog() << "Resource handle not found; Resource URI not set in request";
        return OC_EH_ERROR;
    }

    // Finding the corresponding CPP Application entityHandler for a given resource
    auto entityHandlerEntry = entityHandlerMap.find(entityHandlerRequest->resource);

    if(entityHandlerEntry != entityHandlerMap.end())
    {
        // Call CPP Application Entity Handler
        // TODO CPP Application also should return OC_EH_OK or OC_EH_ERROR
        if(entityHandlerEntry->second)
        {
            entityHandlerEntry->second(pRequest, pResponse);
        }
        else
        {
            oclog() << "C stack should not call again for parent resource\n";
            return OC_EH_ERROR;
        }
    }
    else
    {
        oclog() << "No entity handler found."  << endl;
        return OC_EH_ERROR;
    }

    processResourceResponse(flag, entityHandlerRequest, pResponse);

    return OC_EH_OK;
}

namespace OC
{
    InProcServerWrapper::InProcServerWrapper(OC::OCPlatform& owner,
        std::weak_ptr<std::recursive_mutex> csdkLock, PlatformConfig cfg)
     : IServerWrapper(owner),
       m_csdkLock(csdkLock)
    {
        OCMode initType;

        if(cfg.mode == ModeType::Server)
        {
            initType = OC_SERVER;
        }
        else if (cfg.mode == ModeType::Both)
        {
            initType = OC_CLIENT_SERVER;
        }
        else
        {
            throw InitializeException("Cannot construct a Server when configured as a client",
                                      OC_STACK_INVALID_PARAM);
        }

        OCStackResult result = OCInit(cfg.ipAddress.c_str(), cfg.port, initType);

        if(OC_STACK_OK != result)
        {
            throw InitializeException("Error Initializing Stack", result);
        }

        m_threadRun = true;
        m_processThread = std::thread(&InProcServerWrapper::processFunc, this);
    }

    void InProcServerWrapper::processFunc()
    {
        auto cLock = m_csdkLock.lock();
        while(cLock && m_threadRun)
        {
            OCStackResult result;

            {
                std::lock_guard<std::recursive_mutex> lock(*cLock);
                result = OCProcess();
            }

            // ...the value of variable result is simply ignored for now.
            if(OC_STACK_ERROR == result)
             ;

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    OCStackResult InProcServerWrapper::registerResource(
                    OCResourceHandle& resourceHandle,
                    std::string& resourceURI,
                    const std::string& resourceTypeName,
                    const std::string& resourceInterface,
                    EntityHandler& eHandler,
                    uint8_t resourceProperties)

    {
        OCStackResult result = OC_STACK_ERROR;

        auto cLock = m_csdkLock.lock();

        if(cLock)
        {
            std::lock_guard<std::recursive_mutex> lock(*cLock);

            if(NULL != eHandler)
            {
                result = OCCreateResource(&resourceHandle, // OCResourceHandle *handle
                            resourceTypeName.c_str(), // const char * resourceTypeName
                            resourceInterface.c_str(), //const char * resourceInterfaceName //TODO fix this
                            resourceURI.c_str(), // const char * uri
                            EntityHandlerWrapper, // OCEntityHandler entityHandler
                            resourceProperties // uint8_t resourceProperties
                            );
            }
            else
            {
                result = OCCreateResource(&resourceHandle, // OCResourceHandle *handle
                            resourceTypeName.c_str(), // const char * resourceTypeName
                            resourceInterface.c_str(), //const char * resourceInterfaceName //TODO fix this
                            resourceURI.c_str(), // const char * uri
                            NULL, // OCEntityHandler entityHandler
                            resourceProperties // uint8_t resourceProperties
                            );
            }

            if(result != OC_STACK_OK)
            {
                resourceHandle = (OCResourceHandle) 0;
            }
            else
            {
                entityHandlerMap[resourceHandle] = eHandler;
                resourceUriMap[resourceHandle] = resourceURI;
            }
        }
        else
        {
            result = OC_STACK_ERROR;
        }

        return result;
    }

    OCStackResult InProcServerWrapper::setDefaultDeviceEntityHandler
                                        (EntityHandler entityHandler)
    {
        OCStackResult result = OC_STACK_ERROR;

        defaultDeviceEntityHandler = entityHandler;

        if(entityHandler)
        {
            result = OCSetDefaultDeviceEntityHandler(DefaultEntityHandlerWrapper);
        }
        else
        {
            // If Null passed we unset
            result = OCSetDefaultDeviceEntityHandler(NULL);
        }

        return result;
    }

    OCStackResult InProcServerWrapper::unregisterResource(const OCResourceHandle& resourceHandle)
    {
        auto cLock = m_csdkLock.lock();
        OCStackResult result = OC_STACK_ERROR;

        if(cLock)
        {
            std::lock_guard<std::recursive_mutex> lock(*cLock);
            result = OCDeleteResource(resourceHandle);

            if(result == OC_STACK_OK)
            {
                resourceUriMap.erase(resourceHandle);
            }
            else
            {
                throw OCException("Unregistering resource failed", result);
            }
        }
        else
        {
            result = OC_STACK_ERROR;
        }

        return result;
    }

    OCStackResult InProcServerWrapper::bindTypeToResource(const OCResourceHandle& resourceHandle,
                     const std::string& resourceTypeName)
    {
        auto cLock = m_csdkLock.lock();
        OCStackResult result;
        if(cLock)
        {
            std::lock_guard<std::recursive_mutex> lock(*cLock);
            result = OCBindResourceTypeToResource(resourceHandle, resourceTypeName.c_str());
        }
        else
        {
            result = OC_STACK_ERROR;
        }

        if (result != OC_STACK_OK)
        {
            throw OCException("Bind Type to resource failed", result);
        }
        return result;
    }

    OCStackResult InProcServerWrapper::bindInterfaceToResource(
                     const OCResourceHandle& resourceHandle,
                     const std::string& resourceInterfaceName)
    {
        auto cLock = m_csdkLock.lock();
        OCStackResult result;
        if(cLock)
        {
            std::lock_guard<std::recursive_mutex> lock(*cLock);
            result = OCBindResourceInterfaceToResource(resourceHandle,
                        resourceInterfaceName.c_str());
        }
        else
        {
            result = OC_STACK_ERROR;
        }

        if (result != OC_STACK_OK)
        {
            throw OCException("Bind Interface to resource failed", result);
        }
        return result;
    }

    OCStackResult InProcServerWrapper::startPresence(const unsigned int seconds)
    {
        auto cLock = m_csdkLock.lock();
        OCStackResult result = OC_STACK_ERROR;
        if(cLock)
        {
            std::lock_guard<std::recursive_mutex> lock(*cLock);
            result = OCStartPresence(seconds);
        }

        if(result != OC_STACK_OK)
        {
            throw OCException("startPresence failed", result);
        }
        return result;
    }

    OCStackResult InProcServerWrapper::stopPresence()
    {
        auto cLock = m_csdkLock.lock();
        OCStackResult result = OC_STACK_ERROR;
        if(cLock)
        {
            std::lock_guard<std::recursive_mutex> lock(*cLock);
            result = OCStopPresence();
        }

        if(result != OC_STACK_OK)
        {
            throw OCException("stopPresence failed", result);
        }
        return result;
    }

    InProcServerWrapper::~InProcServerWrapper()
    {
        if(m_processThread.joinable())
        {
            m_threadRun = false;
            m_processThread.join();
        }

        OCStop();
    }
}

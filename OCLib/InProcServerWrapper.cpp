//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
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
#include <OCReflect.h>
#include <OCResourceRequest.h>
#include <OCResourceResponse.h>
#include <ocstack.h>
#include <OCApi.h>
#include <OCUtilities.h>


using namespace OC::OCReflect;

using namespace std;

std::map <OCResourceHandle, std::function<void(const OC::OCResourceRequest::Ptr, const OC::OCResourceResponse::Ptr)>> entityHandlerMap;

void defaultEntityHandler(const OC::OCResourceRequest::Ptr request, const OC::OCResourceResponse::Ptr response)
{
    cout << "\nSomething wrong: We are in default entity handler: " << endl;
}

OCEntityHandlerResult EntityHandler(OCEntityHandlerFlag flag, OCEntityHandlerRequest * entityHandlerRequest ) {

    // TODO @SASHI we need to have a better way of logging (with various levels of logging)
    cout << "\nIn C entity handler: " << endl;

    // TODO @SASHI dow we need shared pointer?
    auto pRequest = std::make_shared<OC::OCResourceRequest>();
    auto pResponse = std::make_shared<OC::OCResourceResponse>();

    // TODO @ SASHI Utility to convert from C to C++ (every).
    switch (flag) {
        case OC_INIT_FLAG:
            // TODO @SASHI We can fill the common data (resource Handle, etc.. )
            // init time.
            pRequest->setRequestHandlerFlag(OC::RequestHandlerFlag::InitFlag);
            break;
        case OC_REQUEST_FLAG:
            pRequest->setRequestHandlerFlag(OC::RequestHandlerFlag::RequestFlag);

            if(entityHandlerRequest)
            {
                if(entityHandlerRequest->query)
                {
                    std::string querystr(reinterpret_cast<char*>(entityHandlerRequest->query));

                    OC::Utilities::QueryParamsKeyVal qp = OC::Utilities::getQueryParams(querystr);

                    if(qp.size() > 0)
                        pRequest->setQueryParams(qp);
                }
                if(OC_REST_GET == entityHandlerRequest->method)
                {
                    // TODO @SASHI Why strings : "GET"??
                    pRequest->setRequestType("GET");
                }

                if(OC_REST_PUT == entityHandlerRequest->method)
                {
                    pRequest->setRequestType("PUT");
                    pRequest->setPayload(std::string(reinterpret_cast<const char*>(entityHandlerRequest->reqJSONPayload)));
                }
            }
            break;
        case OC_OBSERVE_FLAG:
            pRequest->setRequestHandlerFlag(OC::RequestHandlerFlag::ObserverFlag);
            break;
    }


    // Finding the corresponding CPP Application entityHandler for a given resource
    auto entityHandlerEntry = entityHandlerMap.find(entityHandlerRequest->resource);

    if(entityHandlerEntry != entityHandlerMap.end()) {
        // Call CPP Application Entity Handler
        // TODO CPP Application also should return OC_EH_OK or OC_EH_ERROR
        if(entityHandlerEntry->second)
        {
            entityHandlerEntry->second(pRequest, pResponse);
        }
        else
        {
            // TODO Logging
            std::cout << "C stack should not call again for parent resource\n";
        }
    }
    else {
        std::cout << "No entity handler found."  << endl;
        return OC_EH_ERROR;
    }


    if(flag == OC_REQUEST_FLAG)
    {
        // TODO @SASHI we could use const reference
        std::string payLoad = pResponse->getPayload();

        if(OC_REST_GET == entityHandlerRequest->method)
        {
            cout << "\t\t\tGoing from stack for GET: ";
        }
        else if (OC_REST_PUT == entityHandlerRequest->method)
        {
            cout << "\t\t\tGoing from stack for PUT: ";
        }
        else
        {
            cout << "\t\t\tUnknown method...!!!" << endl;
        }

        if (payLoad.size() < entityHandlerRequest->resJSONPayloadLen)
        {
            strncpy((char*)entityHandlerRequest->resJSONPayload, payLoad.c_str(), entityHandlerRequest->resJSONPayloadLen);
            cout << "Payload: " << (char*)entityHandlerRequest->resJSONPayload << endl;
        }
        else
        {
            // TODO throw appropriate runtime error
            cout << "Payload is larger than the PayloadLen" << endl;
        }
    }

    return OC_EH_OK;
}

namespace OC
{
    InProcServerWrapper::InProcServerWrapper(std::weak_ptr<std::mutex> csdkLock, PlatformConfig cfg)
                            :m_csdkLock(csdkLock)
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
            throw InitializeException("Cannot construct a Server when configured as a client", OC_STACK_INVALID_PARAM);
        }

        OCStackResult result = OCInit(cfg.ipAddress.c_str(), cfg.port, initType);

        // Setting default entity Handler
        entityHandlerMap[(OCResourceHandle) 0] = defaultEntityHandler;

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
                std::lock_guard<std::mutex> lock(*cLock);
                result = OCProcess();
            }

            if(result != OC_STACK_OK)
            {
                cout << "Something wrong in OCProcess" << endl;
                // TODO: SASHI
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            // To minimize CPU utilization we may wish to do this with sleep
            //std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    OCStackResult InProcServerWrapper::registerResource(
                    OCResourceHandle& resourceHandle,
                    std::string& resourceURI,
                    const std::string& resourceTypeName,
                    const std::string& resourceInterface,
                    std::function<void(const OCResourceRequest::Ptr, const OCResourceResponse::Ptr)> eHandler,
                    uint8_t resourceProperties)

    {
        OCStackResult  result;

        cout << "Registering Resource: \n";
        cout << "\tResource URI: " << resourceURI << endl;
        cout << "\tResource TypeName: " << resourceTypeName  << endl;
        cout << "\tResource Interface: " << resourceInterface << endl;

        auto cLock = m_csdkLock.lock();

        if(cLock)
        {
            std::lock_guard<std::mutex> lock(*cLock);

            if(NULL != eHandler)
            {
                result = OCCreateResource(&resourceHandle, // OCResourceHandle *handle
                            resourceTypeName.c_str(), // const char * resourceTypeName
                            resourceInterface.c_str(), //const char * resourceInterfaceName //TODO fix this
                            resourceURI.c_str(), // const char * uri
                            EntityHandler, // OCEntityHandler entityHandler
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
                cout << "\tSomething wrong in creating the resource" << endl;
                resourceHandle = (OCResourceHandle) 0;
                // TODO: SASHI
            }
            else
            {
                cout << "\tResource creation is successful with resource handle:  " << resourceHandle << endl;
                entityHandlerMap[resourceHandle] = eHandler;
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
        cout << "Binding Type to Resource: \n";
        cout << "\tTypeName: " << resourceTypeName  << endl;

        auto cLock = m_csdkLock.lock();
        OCStackResult result;
        if(cLock)
        {
            std::lock_guard<std::mutex> lock(*cLock);
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

    OCStackResult InProcServerWrapper::bindInterfaceToResource(const OCResourceHandle& resourceHandle,
                     const std::string& resourceInterfaceName)
    {
        cout << "Binding Interface to Resource: \n";
        cout << "\tInterfaceName: " << resourceInterfaceName  << endl;

        auto cLock = m_csdkLock.lock();
        OCStackResult result;
        if(cLock)
        {
            std::lock_guard<std::mutex> lock(*cLock);
            result = OCBindResourceInterfaceToResource(resourceHandle, resourceInterfaceName.c_str());
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

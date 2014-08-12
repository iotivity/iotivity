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

//******************************************************************
// File name:
//     OCPlatform.cpp
//
// Description: Implementation of the OCPlatform.
//
//
//
//*********************************************************************

#include <random>

#include "OCPlatform.h"
#include "OCApi.h"
#include "OCException.h"

namespace OC
{
    // Constructor. Internally calls private init function
    OCPlatform::OCPlatform(const PlatformConfig& config)
    {
        init(config);
    }

    // Destructor
    OCPlatform::~OCPlatform(void)
    {
        std::cout << "platform destructor called" << std::endl;
        cleanup();
    }

    OCStackResult OCPlatform::notifyObservers(OCResourceHandle resourceHandle)
    {
        return OCNotifyObservers(resourceHandle);
    }

    void OCPlatform::init(const PlatformConfig& config)
    {
        std::unique_ptr<WrapperFactory> wrapperInstance(new WrapperFactory());
        m_WrapperInstance = std::move(wrapperInstance);

        if(config.mode == ModeType::Server)
        {
            // Call server wrapper init
            m_server = m_WrapperInstance->CreateServerWrapper(config);
        }
        else if(config.mode == ModeType::Client)
        {
            // Call client wrapper init
            m_client = m_WrapperInstance->CreateClientWrapper(config);
        }
        else
        {
            // This must be both server and client
            m_server = m_WrapperInstance->CreateServerWrapper(config);
            m_client = m_WrapperInstance->CreateClientWrapper(config);
        }
    }

    void OCPlatform::cleanup()
    {
        if(m_server)
        {
            //delete m_server;
        }

        if(m_client)
        {
            //delete m_client;
        }
    }

    OCResource::Ptr OCPlatform::constructResourceObject(const std::string& host, const std::string& uri,
                bool isObservable, const std::vector<std::string>& resourceTypes,
                const std::vector<std::string>& interfaces)
    {
        if(m_client)
        {
            return std::shared_ptr<OCResource>(new OCResource(m_client, host, uri, isObservable, resourceTypes, interfaces));
        }

        else
        {
            return std::shared_ptr<OCResource>();
        }
    }

    OCStackResult OCPlatform::findResource(const std::string& host, const std::string& resourceName,
                std::function<void(OCResource::Ptr)> resourceHandler)
    {
        if(m_client)
        {
            return m_client->ListenForResource(host, resourceName, resourceHandler);
        }
        return OC_STACK_ERROR;
    }


    OCStackResult OCPlatform::registerResource(OCResourceHandle& resourceHandle,
                std::string& resourceURI,
                const std::string& resourceTypeName,
                const std::string& resourceInterface,
                std::function<void(const OCResourceRequest::Ptr, const OCResourceResponse::Ptr)> entityHandler,
                uint8_t resourceProperty)
    {
        OCStackResult result = OC_STACK_OK;

        if(m_server)
        {
            try{
                result = m_server->registerResource(resourceHandle, resourceURI, resourceTypeName, resourceInterface, entityHandler, resourceProperty);
            }
            catch(std::exception e) // TODO: define our own expception.
            {
                throw e;
            }
        }

        return result;
    }

    OCStackResult OCPlatform::unbindResource(OCResourceHandle collectionHandle, OCResourceHandle resourceHandle)
    {
        OCStackResult result = OC_STACK_OK;

        try {
            result = OCUnBindResource(collectionHandle, resourceHandle);
        }
        catch(std::exception e) // TODO: define our own expception.
        {
            throw e;
        }

        return result;
    }

    OCStackResult OCPlatform::unbindResources(const OCResourceHandle collectionHandle, const std::vector<OCResourceHandle>& resourceHandles)
    {
        OCStackResult result = OC_STACK_OK;

        try {

            for(const OCResourceHandle& handle : resourceHandles)
            {
                result = OCUnBindResource(collectionHandle, handle);

                if(result != OC_STACK_OK)
                {
                    // TODO Should we unbind the previous successful ones?
                    // TODO should we return which are succesful
                    // Currently just returns with any failure
                    return result;
                }
            }
        }
        catch(std::exception e) // TODO : define our own exception 
        {
            throw e;
        }

        return result;
    }

    OCStackResult bindResource(const OCResourceHandle collectionHandle, const OCResourceHandle resourceHandle)
    {
        OCStackResult result = OC_STACK_OK;

        try {
            result = OCBindResource(collectionHandle, resourceHandle);
        }
        catch(std::exception e) // TODO : define our own exception
        {
            throw e;
        }

        return result;
    }

    OCStackResult bindResources(const OCResourceHandle collectionHandle, const std::vector<OCResourceHandle>& resourceHandles)
    {
        OCStackResult result = OC_STACK_OK;

        try {

            for(const OCResourceHandle& handle : resourceHandles)
            {
                result = OCBindResource(collectionHandle, handle);

                if(result != OC_STACK_OK)
                {
                    // TODO Should we unbind the previous successful ones?
                    // TODO should we return which are succesful
                    // Currently just returns with any failure
                    return result;
                }
            }
        }
        catch(std::exception e) // TODO : define our own exception
        {
            throw e;
        }

        return result;
    }

    OCStackResult OCPlatform::bindTypeToResource(const OCResourceHandle& resourceHandle,
                     const std::string& resourceTypeName) const
    {
        OCStackResult result = OC_STACK_ERROR;
        if(m_server)
        {
            try
            {
                result = m_server->bindTypeToResource(resourceHandle, resourceTypeName);
            }
            catch (OCException& e)
            {
                cout << "Caught an exception..." << endl;
                cout << "\tMessage: " << e.what()  << endl;
                cout << "\t Reason: " << e.reason() << endl;
            }
        }
        return result;
    }

    OCStackResult OCPlatform::bindInterfaceToResource(const OCResourceHandle& resourceHandle,
                     const std::string& resourceInterfaceName) const
    {
        OCStackResult result = OC_STACK_ERROR;
        if(m_server)
        {
            try
            {
                result = m_server->bindInterfaceToResource(resourceHandle, resourceInterfaceName);
            }
            catch (OCException& e)
            {
                cout << "Caught an exception..." << endl;
                cout << "\tMessage: " << e.what()  << endl;
                cout << "\t Reason: " << e.reason() << endl;
            }
        }
        return result;

    }

} //namespace OC

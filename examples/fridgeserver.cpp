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

/// The purpose of this server is to simulate a refridgerator that contains a device resource for
/// its description, a light resource for the internal light, and 2 door resources for the purpose
/// of representing the doors attached to this fridge.  This is used by the fridgeclient to
/// demonstrate using std::bind to attach to instances of a class as well as using
/// constructResourceObject

#include <chrono>
#include <iostream>
#include <thread>
#include <stdexcept>

#include "OCPlatform.h"
#include "OCApi.h"

using namespace OC;
namespace PH = std::placeholders;

class Resource
{
    protected:
    OCResourceHandle m_resourceHandle;
    OCRepresentation m_rep;
    virtual void entityHandler(std::shared_ptr<OCResourceRequest> request,
            std::shared_ptr<OCResourceResponse> response)=0;
};

class DeviceResource : public Resource
{
    public:
    DeviceResource(OCPlatform& platform)
    {
        std::string resourceURI = "/device";
        std::string resourceTypeName = "intel.fridge";
        std::string resourceInterface = DEFAULT_INTERFACE;
        EntityHandler cb = std::bind(&DeviceResource::entityHandler, this,PH::_1, PH::_2);

        EntityHandler defaultEH = std::bind(&DeviceResource::defaultEntityHandler, this,
                                                                         PH::_1, PH::_2);

        std::cout << "Setting device default entity handler\n";
        platform.setDefaultDeviceEntityHandler(defaultEH);

        uint8_t resourceProperty = OC_DISCOVERABLE;
        OCStackResult result = platform.registerResource(m_resourceHandle,
            resourceURI,
            resourceTypeName,
            resourceInterface,
            cb,
            resourceProperty);

        if(OC_STACK_OK != result)
        {
            throw std::runtime_error(
                std::string("Device Resource failed to start")+std::to_string(result));
        }
    }
    private:
    OCRepresentation get()
    {
        m_rep.setValue("device_name", std::string("Intel Powered 2 door, 1 light refridgerator"));
        return m_rep;
    }

    std::string m_modelName;
    protected:
    virtual void entityHandler(std::shared_ptr<OCResourceRequest> request,
            std::shared_ptr<OCResourceResponse> response)
    {
        if(request)
        {
            if(request->getRequestHandlerFlag() == RequestHandlerFlag::RequestFlag)
            {
                if(request->getRequestType() == "GET")
                {
                    if(response)
                    {
                        std::cout<<"DeviceResource Get Request"<<std::endl;
                        response->setErrorCode(200);
                        response->setResourceRepresentation(get(), "");
                    }
                }
                else
                {
                    std::cout <<"DeviceResource unsupported request type "
                    << request->getRequestType() << std::endl;
                }
            }
            else
            {
                std::cout << "DeviceResource unsupported request flag" <<std::endl;
            }
        }
    }

    virtual void defaultEntityHandler(std::shared_ptr<OCResourceRequest> request,
            std::shared_ptr<OCResourceResponse> response)
    {
        if(request)
        {
            std::cout << "In Default Entity Handler, uri received: "
                        << request->getResourceUri() << std::endl;

            if(request->getRequestHandlerFlag() == RequestHandlerFlag::RequestFlag)
            {
                if(request->getRequestType() == "GET")
                {
                    if(response)
                    {
                        std::cout<<"Default Entity Handler: Get Request"<<std::endl;
                        response->setErrorCode(200);
                        response->setResourceRepresentation(get(), "");
                    }
                }
                else
                {
                    std::cout <<"Default Entity Handler: unsupported request type "
                    << request->getRequestType() << std::endl;
                }
            }
            else
            {
                std::cout << "Default Entity Handler: unsupported request flag" <<std::endl;
            }
        }
   }

};

class LightResource : public Resource
{
    public:
    LightResource(OCPlatform& platform)
    {
        std::string resourceURI = "/light";
        std::string resourceTypeName = "intel.fridge.light";
        std::string resourceInterface = DEFAULT_INTERFACE;
        EntityHandler cb = std::bind(&LightResource::entityHandler, this,PH::_1, PH::_2);
        uint8_t resourceProperty = 0;
        OCStackResult result = platform.registerResource(m_resourceHandle,
            resourceURI,
            resourceTypeName,
            resourceInterface,
            cb,
            resourceProperty);

        if(OC_STACK_OK != result)
        {
            throw std::runtime_error(
                std::string("Light Resource failed to start")+std::to_string(result));
        }
    }
    private:
    OCRepresentation get()
    {
        m_rep.setValue("on", m_isOn);
        return m_rep;
    }

    void put(OCRepresentation rep)
    {
        rep.getValue("on", m_isOn);
    }

    bool m_isOn;
    protected:
    virtual void entityHandler(std::shared_ptr<OCResourceRequest> request,
            std::shared_ptr<OCResourceResponse> response)
    {
        if(request)
        {
            std::cout << "In entity handler for Light, URI is : "
                      << request->getResourceUri() << std::endl;

            if(request->getRequestHandlerFlag() == RequestHandlerFlag::RequestFlag)
            {
                if(request->getRequestType() == "GET")
                {
                    if(response)
                    {
                        std::cout<<"Light Get Request"<<std::endl;
                        response->setErrorCode(200);
                        response->setResourceRepresentation(get(), "");
                    }
                }
                else if(request->getRequestType() == "PUT")
                {
                    if(response)
                    {
                        std::cout <<"Light Put Request"<<std::endl;
                        put(request->getResourceRepresentation());

                        response->setErrorCode(200);
                        response->setResourceRepresentation(get(),"");
                    }
                }
                else
                {
                    std::cout <<"Light unsupported request type "
                    << request->getRequestType() << std::endl;
                }
            }
            else
            {
                std::cout << "Light unsupported request flag" <<std::endl;
            }
        }
    }
};

class DoorResource : public Resource
{
    public:
    DoorResource(OCPlatform& platform, const std::string& side):m_side(side)
    {

        std::string resourceURI = "/door/"+ side;
        std::string resourceTypeName = "intel.fridge.door";
        std::string resourceInterface = DEFAULT_INTERFACE;
        EntityHandler cb = std::bind(&DoorResource::entityHandler, this,PH::_1, PH::_2);

        uint8_t resourceProperty = 0;
        OCStackResult result = platform.registerResource(m_resourceHandle,
            resourceURI,
            resourceTypeName,
            resourceInterface,
            cb,
            resourceProperty);

        if(OC_STACK_OK != result)
        {
            throw std::runtime_error(
                std::string("Door Resource failed to start")+std::to_string(result));
        }
    }

    private:
    OCRepresentation get()
    {
        m_rep.setValue("open", m_isOpen);
        m_rep.setValue("side", m_side);
        return m_rep;
    }

    void put(OCRepresentation rep)
    {
        rep.getValue("open", m_isOpen);
        // Note, we won't let the user change the door side!
    }
    bool m_isOpen;
    std::string m_side;
    protected:
    virtual void entityHandler(std::shared_ptr<OCResourceRequest> request,
            std::shared_ptr<OCResourceResponse> response)
    {
        if(request)
        {
            std::cout << "In entity handler for Door, URI is : "
                      << request->getResourceUri() << std::endl;

            if(request->getRequestHandlerFlag() == RequestHandlerFlag::RequestFlag)
            {
                if(request->getRequestType() == "GET")
                {
                    if(response)
                    {
                        // note that we know the side because std::bind gives us the
                        // appropriate object
                        std::cout<<m_side << " Door Get Request"<<std::endl;
                        response->setErrorCode(200);
                        response->setResourceRepresentation(get(), "");
                    }
                }
                else if(request->getRequestType() == "PUT")
                {
                    if(response)
                    {
                        std::cout <<m_side <<" Door Put Request"<<std::endl;
                        put(request->getResourceRepresentation());
                        response->setErrorCode(200);
                        response->setResourceRepresentation(get(),"");
                    }
                }
                else
                {
                    std::cout <<m_side<<" Door unsupported request type "
                    << request->getRequestType() << std::endl;
                }
            }
            else
            {
                std::cout << "Door unsupported request flag" <<std::endl;
            }
        }
    }

};

class Refridgerator
{
    public:
    Refridgerator(PlatformConfig &cfg)
        : m_platform(cfg),
        m_light(m_platform),
        m_device(m_platform),
        m_leftdoor(m_platform, "left"),
        m_rightdoor(m_platform, "right")
    {
    }
    private:
    OCPlatform m_platform;
    LightResource m_light;
    DeviceResource m_device;
    DoorResource m_leftdoor;
    DoorResource m_rightdoor;
};

int main ()
{
    PlatformConfig cfg
    {
        ServiceType::InProc,
        ModeType::Server,
        "0.0.0.0", // By setting to "0.0.0.0", it binds to all available interfaces
        0,         // Uses randomly available port
        QualityOfService::NonConfirmable
    };

    Refridgerator rf(cfg);

    // we will keep the server alive for at most 30 minutes
    std::this_thread::sleep_for(std::chrono::minutes(30));
    return 0;
}

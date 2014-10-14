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

/// This fridgeclient represents a client trying to discover the associated
/// fridgeserver.  The device resource is the only one available for discovery
/// on the server, so we have to take the fact that we know the device tag
/// to then generate a Resource object

#include <iostream>
#include <stdexcept>
#include <condition_variable>
#include <mutex>
#include "OCPlatform.h"
#include "OCApi.h"

using namespace OC;
namespace PH = std::placeholders;

// Option ID for API version and client token
const uint16_t API_VERSION = 2048;
const uint16_t TOKEN = 3000;

class ClientFridge
{
    public:
    ClientFridge(PlatformConfig &cfg) : m_platform(cfg)
    {
        std::cout << "Fridge Client has started " <<std::endl;
        FindCallback f (std::bind(&ClientFridge::foundDevice, this, PH::_1));

        OCStackResult result = m_platform.findResource(
                "", "coap://224.0.1.187/oc/core?rt=intel.fridge", f);

        if(OC_STACK_OK != result)
        {
            throw new std::runtime_error("Fridge Find Resource Failed");
        }

        std::cout << "Waiting to discover fridge... "<<std::endl;
        {
            // we want to block this thread until the client has finished
            // its duties, so we block on the CV until we have completed
            // what we are looking to do
            std::unique_lock<std::mutex> lk(m_mutex);
            m_cv.wait(lk);
        }
    }

    private:
    void foundDevice(std::shared_ptr<OCResource> resource)
    {
        if(resource && resource->uri() == "/device")
        {
            std::cout << "Discovered a device object"<<std::endl;
            std::cout << "\tHost: "<<resource->host()<<std::endl;
            std::cout << "\tURI:  "<<resource->uri() <<std::endl;
        }

        // we have now found a resource, so lets create a few resource objects
        // for the other resources that we KNOW are associated with the intel.fridge
        // server, and query them.
        std::vector<std::string> lightTypes = {"intel.fridge.light"};
        std::vector<std::string> ifaces = {DEFAULT_INTERFACE};
        OCResource::Ptr light = m_platform.constructResourceObject(resource->host(),
                                "/light", false, lightTypes, ifaces);

        std::vector<std::string> doorTypes = {"intel.fridge.door"};

        OCResource::Ptr leftdoor = m_platform.constructResourceObject(resource->host(),
                                "/door/left", false, doorTypes, ifaces);
        OCResource::Ptr rightdoor = m_platform.constructResourceObject(resource->host(),
                                "/door/right", false, doorTypes, ifaces);
        OCResource::Ptr randomdoor = m_platform.constructResourceObject(resource->host(),
                                "/door/random", false, doorTypes, ifaces);

        // Set header options with API version and token
        HeaderOptions headerOptions;
        try
        {
            // Set API version and client token
            HeaderOption::OCHeaderOption apiVersion(API_VERSION, "v.1.0");
            HeaderOption::OCHeaderOption clientToken(TOKEN, "21ae43gf");
            headerOptions.push_back(apiVersion);
            headerOptions.push_back(clientToken);
        }
        catch(OCException& e)
        {
            std::cout << "Error creating HeaderOption: " << e.what() << std::endl;
        }


        // Setting header options will send above options in all requests
        // Header options are set per resource.
        // Below, header options are set only for device resource
        resource->setHeaderOptions(headerOptions);

        resource->get(QueryParamsMap(), GetCallback(
                std::bind(&ClientFridge::getResponse, this, "Device", PH::_1,
                    PH::_2, PH::_3, resource, 0)
                ));
        light->get(QueryParamsMap(), GetCallback(
                std::bind(&ClientFridge::getResponse, this, "Fridge Light", PH::_1,
                    PH::_2, PH::_3, light, 1)
                ));
        leftdoor->get(QueryParamsMap(), GetCallback(
                std::bind(&ClientFridge::getResponse, this, "Left Door", PH::_1,
                    PH::_2, PH::_3, leftdoor, 2)
                ));
        rightdoor->get(QueryParamsMap(), GetCallback(
                std::bind(&ClientFridge::getResponse, this, "Right Door", PH::_1,
                    PH::_2, PH::_3, rightdoor, 3)
                ));
        randomdoor->get(QueryParamsMap(), GetCallback(
                std::bind(&ClientFridge::getResponse, this, "Random Door", PH::_1,
                    PH::_2, PH::_3, randomdoor, 4)
                ));
    }

    // Note that resourceName, resource, and getId are all bound via the std::bind mechanism.
    // it is possible to attach ANY arbitrary data to do whatever you would like here.  It may,
    // however be a better fit to wrap each call in an object so a fuller context (and additional
    // requests) can be easily made inside of a simple context
    void getResponse(const std::string& resourceName, const HeaderOptions& headerOptions,
                const OCRepresentation rep, const int eCode, OCResource::Ptr resource, int getId)
    {
        std::cout << "Got a response from get from the "<<resourceName<< std::endl;
        std::cout << "Get ID is "<<getId<<" and resource URI is "<<resource->uri()<<std::endl;

        std::cout << "The Attribute Data is: "<<std::endl;

        for (auto it = headerOptions.begin(); it != headerOptions.end(); ++it)
        {
            if(it->getOptionID() == API_VERSION)
            {
                std::cout << "Server API version in GET response: " <<
                        it->getOptionData() << std::endl;
            }
        }

        switch(getId)
        {
            case 0:
                {
                    // Get on device
                    std::string name;
                    rep.getValue("device_name", name);
                    std::cout << "Name of device: "<< name << std::endl;
                    break;
                }
            case 1:
                {
                    bool isOn;
                    rep.getValue("on",isOn);
                    std::cout<<"The fridge light is "<< ((isOn)?"":"not ") <<"on"<<std::endl;
                }
                break;
            case 2:
            case 3:
                {
                    bool isOpen;
                    std::string side;
                    rep.getValue("open", isOpen);
                    rep.getValue("side", side);
                    std::cout << "Door is "<<isOpen<<" and is on the "<<side<<std::endl;
                }
                break;
            case 4:
                {
                    // Get on random resource called.
                    std::string name;
                    rep.getValue("device_name", name);
                    std::cout << "Name of fridge: "<< name << std::endl;
                    break;
                }
        }
    }

    OCPlatform m_platform;
    std::mutex m_mutex;
    std::condition_variable m_cv;
};

int main()
{
    PlatformConfig cfg
    {
        ServiceType::InProc,
        ModeType::Client,
        "0.0.0.0",
        0,
        QualityOfService::LowQos
    };

    ClientFridge cf (cfg);
    return 0;
}

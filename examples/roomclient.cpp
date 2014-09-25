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

// OCClient.cpp : Defines the entry point for the console application.
//
#include <string>
#include <cstdlib>
#include <pthread.h>
#include "OCPlatform.h"
#include "OCApi.h"

using namespace OC;

const int SUCCESS_RESPONSE = 0;
std::shared_ptr<OCResource> curResource;

int observe_count()
{
    static int oc = 0;
    return ++oc;
}

// Forward declaration
void putRoomRepresentation(std::shared_ptr<OCResource> resource);
void onPut(const OCRepresentation& rep, const int eCode);

// callback handler on GET request
void onGet(const OCRepresentation& rep, const int eCode)
{
    if(eCode == SUCCESS_RESPONSE)
    {
        std::cout << "GET request was successful" << std::endl;

        std::cout << "\tResource URI: " << rep.getUri() << std::endl;

        std::vector<OCRepresentation> children = rep.getChildren();

        for(auto oit = children.begin(); oit != children.end(); ++oit)
        {
            std::cout << "\t\tChild Resource URI: " << oit->getUri() << std::endl;
            if(oit->getUri().find("light") != std::string::npos)
            {
                bool state = false;
                int  color = 0;
                oit->getValue("state", state);
                oit->getValue("color", color);

                std::cout << "\t\tstate:" << state << std::endl;
                std::cout << "\t\tcolor:" << color << std::endl;
            }
            else if(oit->getUri().find("fan") != std::string::npos)
            {
                bool state = false;
                int  speed = 0;
                oit->getValue("state", state);
                oit->getValue("speed", speed);

                std::cout << "\t\tstate:" << state << std::endl;
                std::cout << "\t\tspeed:" << speed << std::endl;
            }
        }

        putRoomRepresentation(curResource);
    }
    else
    {
        std::cout << "onGET Response error: " << eCode << std::endl;
        std::exit(-1);
    }
}

// Local function to put a different state for this resource
void putRoomRepresentation(std::shared_ptr<OCResource> resource)
{
    if(resource)
    {
        OCRepresentation rep;
        std::cout << "Putting room representation..."<<std::endl;

        bool state = true;
        int speed = 10;
        rep.setValue("state", state);
        rep.setValue("speed", speed);

        // Invoke resource's pit API with attribute map, query map and the callback parameter
        resource->put("core.room", BATCH_INTERFACE, rep, QueryParamsMap(), &onPut);
    }
}

// callback handler on PUT request
void onPut(const OCRepresentation& rep, const int eCode)
{
    if(eCode == SUCCESS_RESPONSE)
    {
        std::cout << "PUT request was successful" << std::endl;

        std::cout << "\tResource URI: " << rep.getUri() << std::endl;

        std::vector<OCRepresentation> children = rep.getChildren();

        for(auto oit = children.begin(); oit != children.end(); ++oit)
        {
            std::cout << "\t\tChild Resource URI: " << oit->getUri() << std::endl;
            if(oit->getUri().find("light") != std::string::npos)
            {
                bool state = false;
                int  color = 0;
                oit->getValue("state", state);
                oit->getValue("color", color);
                std::cout << "\t\tstate:" << state << std::endl;
                std::cout << "\t\tcolor:" << color << std::endl;
            }
            else if(oit->getUri().find("fan") != std::string::npos)
            {
                bool state = false;
                int  speed = 0;
                oit->getValue("state", state);
                oit->getValue("speed", speed);
                std::cout << "\t\tstate:" << state << std::endl;
                std::cout << "\t\tspeed:" << speed << std::endl;
            }

        }

    }
    else
    {
        std::cout << "onPut Response error: " << eCode << std::endl;
        std::exit(-1);
    }
}


// Local function to get representation of light resource
void getRoomRepresentation(std::shared_ptr<OCResource> resource)
{
    if(resource)
    {
        std::cout << "Getting Room Representation..."<<std::endl;
        // Invoke resource's get API with the callback parameter

        QueryParamsMap qp;
        resource->get("core.room", BATCH_INTERFACE, qp, &onGet);
    }
}

// Callback to found resources
void foundResource(std::shared_ptr<OCResource> resource)
{
    if(curResource)
    {
        std::cout << "Found another resource, ignoring"<<std::endl;
    }

    std::string resourceURI;
    std::string hostAddress;
    try
    {
        // Do some operations with resource object.
        if(resource)
        {
            std::cout<<"DISCOVERED Resource:"<<std::endl;
            // Get the resource URI
            resourceURI = resource->uri();
            std::cout << "\tURI of the resource: " << resourceURI << std::endl;

            // Get the resource host address
            hostAddress = resource->host();
            std::cout << "\tHost address of the resource: " << hostAddress << std::endl;

            // Get the resource types
            std::cout << "\tList of resource types: " << std::endl;
            for(auto &resourceTypes : resource->getResourceTypes())
            {
                std::cout << "\t\t" << resourceTypes << std::endl;
            }

            // Get the resource interfaces
            std::cout << "\tList of resource interfaces: " << std::endl;
            for(auto &resourceInterfaces : resource->getResourceInterfaces())
            {
                std::cout << "\t\t" << resourceInterfaces << std::endl;
            }

            if(resourceURI == "/a/room")
            {
                curResource = resource;
                // Call a local function which will internally invoke get API on the resource pointer
                getRoomRepresentation(resource);
            }
        }
        else
        {
            // Resource is invalid
            std::cout << "Resource is invalid" << std::endl;
        }

    }
    catch(std::exception& e)
    {
        //log(e.what());
    }
}

int main(int argc, char* argv[]) {

    // Create PlatformConfig object
    PlatformConfig cfg {
        OC::ServiceType::InProc,
        OC::ModeType::Client,
        "0.0.0.0",
        0,
        OC::QualityOfService::NonConfirmable
    };

    // Create a OCPlatform instance.
    // Note: Platform creation is synchronous call.

    try
    {
        OCPlatform platform(cfg);
        std::cout << "Created Platform..."<<std::endl;
        // Find all resources
        platform.findResource("", "coap://224.0.1.187/oc/core", &foundResource);
        std::cout<< "Finding Resource... " <<std::endl;
        while(true)
        {
            // some operations
        }

    }catch(OCException& e)
    {
        //log(e.what());
    }

    return 0;
}


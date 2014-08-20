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

void onObserve(const AttributeMap& attributeMap, const int& eCode, const int& sequenceNumber)
{
    if(eCode == SUCCESS_RESPONSE)
    {
        std::cout << "OBSERVE RESULT:"<<std::endl;
        std::cout << "\tSequenceNumber: "<< sequenceNumber << endl;
        for(auto it = attributeMap.begin(); it != attributeMap.end(); ++it)
        {
            std::cout << "\tAttribute name: "<< it->first << " value: ";
            for(auto valueItr = it->second.begin(); valueItr != it->second.end(); ++valueItr)
            {
                std::cout <<"\t"<< *valueItr << " ";
            }

            std::cout << std::endl;
        }
        
        if(observe_count() > 30)
        {
            std::cout<<"Cancelling Observe..."<<std::endl;
            OCStackResult result = curResource->cancelObserve();

            std::cout << "Cancel result: "<< result <<std::endl;
            sleep(10);
            std::cout << "DONE"<<std::endl;
            std::exit(0);
        }
    }
    else
    {
        std::cout << "onObserve Response error: " << eCode << std::endl;
        std::exit(-1);
    }
}
// callback handler on PUT request
void onGetRep(OCRepresentation& rep, const int eCode)
{
    if(eCode == SUCCESS_RESPONSE)
    {
        std::cout << "GET request was successful" << std::endl;

        AttributeMap attributeMap = rep.getAttributeMap();

        for(auto it = attributeMap.begin(); it != attributeMap.end(); ++it)
        {
            std::cout << "\tAttribute name: "<< it->first << " value: ";
            for(auto valueItr = it->second.begin(); valueItr != it->second.end(); ++valueItr)
            {
                std::cout <<"\t"<< *valueItr << " ";
            }

            std::cout << std::endl;
        }

        std::vector<OCRepresentation> children = rep.getChildren();

        for(auto oit = children.begin(); oit != children.end(); ++oit)
        {
            attributeMap = oit->getAttributeMap();

            for(auto it = attributeMap.begin(); it != attributeMap.end(); ++it)
            {
                std::cout << "\tAttribute name: "<< it->first << " value: ";
                for(auto valueItr = it->second.begin(); valueItr != it->second.end(); ++valueItr)
                {
                    std::cout <<"\t"<< *valueItr << " ";
                }

                std::cout << std::endl;
            }
        }

    }
    else
    {
        std::cout << "onGET Response error: " << eCode << std::endl;
        std::exit(-1);
    }
}

// callback handler on PUT request
void onPutRep(OCRepresentation& rep, const int eCode)
{
    if(eCode == SUCCESS_RESPONSE)
    {
        std::cout << "PUT request was successful" << std::endl;

        AttributeMap attributeMap = rep.getAttributeMap();

        for(auto it = attributeMap.begin(); it != attributeMap.end(); ++it)
        {
            std::cout << "\tAttribute name: "<< it->first << " value: ";
            for(auto valueItr = it->second.begin(); valueItr != it->second.end(); ++valueItr)
            {
                std::cout <<"\t"<< *valueItr << " ";
            }

            std::cout << std::endl;
        }

        std::vector<OCRepresentation> children = rep.getChildren();

        for(auto oit = children.begin(); oit != children.end(); ++oit)
        {
            attributeMap = oit->getAttributeMap();

            for(auto it = attributeMap.begin(); it != attributeMap.end(); ++it)
            {
                std::cout << "\tAttribute name: "<< it->first << " value: ";
                for(auto valueItr = it->second.begin(); valueItr != it->second.end(); ++valueItr)
                {
                    std::cout <<"\t"<< *valueItr << " ";
                }

                std::cout << std::endl;
            }
        }

    }
    else
    {
        std::cout << "onPut Response error: " << eCode << std::endl;
        std::exit(-1);
    }
}

// callback handler on PUT request
void onPut(const AttributeMap attributeMap, const int eCode)
{
    if(eCode == SUCCESS_RESPONSE)
    {
        std::cout << "PUT request was successful" << std::endl;

        for(auto it = attributeMap.begin(); it != attributeMap.end(); ++it)
        {
            std::cout << "\tAttribute name: "<< it->first << " value: ";
            for(auto valueItr = it->second.begin(); valueItr != it->second.end(); ++valueItr)
            {
                std::cout <<"\t"<< *valueItr << " ";
            }

            std::cout << std::endl;
        }
    }
    else
    {
        std::cout << "onPut Response error: " << eCode << std::endl;
        std::exit(-1);
    }
}

// Local function to put a different state for this resource
void putRoomRepresentation(std::shared_ptr<OCResource> resource)
{
    if(resource)
    {
        std::cout << "Putting room representation..."<<std::endl;
        // Create AttributeMap
        AttributeMap attributeMap;
        // Add the attribute name and values in the attribute map
        AttributeValues stateVal;
        stateVal.push_back("true");

        AttributeValues powerVal;
        powerVal.push_back("8");

        attributeMap["state"] = stateVal;
        attributeMap["speed"] = powerVal;

        // Create QueryParameters Map and add query params (if any)
        QueryParamsMap qp;
        qp["if"] = BATCH_INTERFACE;

        // Invoke resource's pit API with attribute map, query map and the callback parameter
        resource->put(attributeMap, qp, &onPut);
    }
}

// callback handler on GET request
void onGet(const AttributeMap attributeMap, const int eCode)
{
    if(eCode == SUCCESS_RESPONSE)
    {
        std::cout << "GET Succeeded:"<<std::endl;
        for(auto it = attributeMap.begin(); it != attributeMap.end(); ++it)
        {
            std::cout << "\tAttribute name: "<< it->first << " value: ";
            for(auto valueItr = it->second.begin(); valueItr != it->second.end(); ++valueItr)
            {
                std::cout <<"\t"<< *valueItr << " ";
            }

            std::cout << std::endl;
        }

        putRoomRepresentation(curResource);
    }
    else
    {
        std::cout << "onGet Response error: " << eCode << std::endl;
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
        qp["if"] = BATCH_INTERFACE; // LINK_INTERFACE; 
        resource->get(qp, &onGet);
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

            if(resourceURI == "/a/room")
            {
                curResource = resource;
                // Call a local function which will internally invoke get API on the resource pointer
                // TODO change this back when getRoomRepresentation works
                getRoomRepresentation(resource);
                 //putRoomRepresentation(resource);
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
    PlatformConfig cfg;
    cfg.ipAddress = "192.168.1.10";
    cfg.port = 5683;
    cfg.mode = ModeType::Client;
    cfg.serviceType = ServiceType::InProc;

    // Create a OCPlatform instance.
    // Note: Platform creation is synchronous call.

    try
    {
        OCPlatform platform(cfg);
        std::cout << "Created Platform..."<<std::endl;
        // Find all resources
        platform.findResource("", "coap://224.0.1.187/oc/core?rt=core.room", &foundResource);
        //platform.findResource("", "coap://224.0.1.187/oc/core", &foundResource);
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


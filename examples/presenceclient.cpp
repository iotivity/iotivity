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

// PresenceClient.cpp : A client example for presence notification
//
#include <string>
#include <cstdlib>
#include <pthread.h>
#include "OCPlatform.h"
#include "OCApi.h"

using namespace OC;

std::shared_ptr<OCResource> curResource;
static ObserveType OBSERVE_TYPE_TO_USE = ObserveType::Observe;

OCPlatform* platformPtr;

// Callback to presence
void presenceHandler(OCStackResult result, const unsigned int nonce)
{
    switch(result)
    {
        case OC_STACK_OK:
            std::cout << "Nonce# " << nonce << std::endl;
            break;
        case OC_STACK_PRESENCE_STOPPED:
            std::cout << "Presence Stopped\n";
            break;
        case OC_STACK_PRESENCE_DO_NOT_HANDLE:
            std::cout << "Presence do not handle\n";
            break;
        default:
            std::cout << "Error\n";
            break;
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

            if(resourceURI == "/a/light")
            {
                curResource = resource;
                OCPlatform::OCPresenceHandle presenceHandle;
                platformPtr->subscribePresence(presenceHandle, hostAddress, &presenceHandler);
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

void PrintUsage()
{
    std::cout << std::endl;
    std::cout << "Usage : simpleclient <ObserveType>" << std::endl;
    std::cout << "   ObserveType : 1 - Observe" << std::endl;
    std::cout << "   ObserveType : 2 - ObserveAll" << std::endl;
}


int main(int argc, char* argv[]) {
    if (argc == 1)
    {
        OBSERVE_TYPE_TO_USE = ObserveType::Observe;
    }
    else if (argc == 2)
    {
        int value = atoi(argv[1]);
        if (value == 1)
            OBSERVE_TYPE_TO_USE = ObserveType::Observe;
        else if (value == 2)
            OBSERVE_TYPE_TO_USE = ObserveType::ObserveAll;
        else
            OBSERVE_TYPE_TO_USE = ObserveType::Observe;
    }
    else
    {
        PrintUsage();
        return -1;
    }

    // Create PlatformConfig object
    PlatformConfig cfg {
        OC::ServiceType::InProc,
        OC::ModeType::Client,
        "192.168.1.10",
        5683,
        OC::QualityOfService::NonConfirmable
    };

    // Create a OCPlatform instance.
    // Note: Platform creation is synchronous call.

    try
    {
        OCPlatform platform(cfg);
        // PlatformPtr is used in another function
        platformPtr = &platform;
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


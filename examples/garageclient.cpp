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

// garageclient.cpp is the client program for garageserver.cpp, which
// uses different representation in OCRepresention.

#include <string>
#include <cstdlib>
#include <pthread.h>
#include "OCPlatform.h"
#include "OCApi.h"

using namespace OC;

const int SUCCESS_RESPONSE = 0;
std::shared_ptr<OCResource> curResource;

class Garage
{
public:

    bool m_state;
    std::string m_name;
    bool m_lightState;
    int m_lightPower;
    OCRepresentation m_lightRep;

    Garage() : m_state(false), m_name("")
    {
    }
};

Garage myGarage;

// callback handler on PUT request
void onPut(const OCRepresentation& rep, const int eCode)
{
    if(eCode == SUCCESS_RESPONSE)
    {
        std::cout << "PUT request was successful" << std::endl;

        rep.getValue("state", myGarage.m_state);
        rep.getValue("name", myGarage.m_name);
        rep.getValue("light", myGarage.m_lightRep);

        myGarage.m_lightRep.getValue("state", myGarage.m_lightState);
        myGarage.m_lightRep.getValue("power", myGarage.m_lightPower);

        std::cout << "\tstate: " << myGarage.m_state << std::endl;
        std::cout << "\tname: " << myGarage.m_name << std::endl;
        std::cout << "\tlightRep: state: " << myGarage.m_lightState << std::endl;
        std::cout << "\tlightRep: power: " << myGarage.m_lightPower << std::endl;
    }
    else
    {
        std::cout << "onPut Response error: " << eCode << std::endl;
        std::exit(-1);
    }
}

// Local function to put a different state for this resource
void putLightRepresentation(std::shared_ptr<OCResource> resource)
{
    if(resource)
    {
        OCRepresentation rep;

        std::cout << "Putting light representation..."<<std::endl;

        myGarage.m_state = true;

        rep.setValue("state", myGarage.m_state);

        // Create QueryParameters Map and add query params (if any)
        QueryParamsMap queryParamsMap;

        // Invoke resource's pit API with rep, query map and the callback parameter
        resource->put(rep, queryParamsMap, &onPut);
    }
}

// Callback handler on GET request
void onGet(const OCRepresentation& rep, const int eCode)
{
    if(eCode == SUCCESS_RESPONSE)
    {
        std::cout << "GET request was successful" << std::endl;
        std::cout << "Resource URI: " << rep.getUri() << std::endl;

        rep.getValue("state", myGarage.m_state);
        rep.getValue("name", myGarage.m_name);
        rep.getValue("light", myGarage.m_lightRep);

        myGarage.m_lightRep.getValue("state", myGarage.m_lightState);
        myGarage.m_lightRep.getValue("power", myGarage.m_lightPower);

        std::cout << "\tstate: " << myGarage.m_state << std::endl;
        std::cout << "\tname: " << myGarage.m_name << std::endl;
        std::cout << "\tlightRep: state: " << myGarage.m_lightState << std::endl;
        std::cout << "\tlightRep: power: " << myGarage.m_lightPower << std::endl;

        putLightRepresentation(curResource);
    }
    else
    {
        std::cout << "onGET Response error: " << eCode << std::endl;
        std::exit(-1);
    }
}

// Local function to get representation of light resource
void getLightRepresentation(std::shared_ptr<OCResource> resource)
{
    if(resource)
    {
        std::cout << "Getting Light Representation..."<<std::endl;
        // Invoke resource's get API with the callback parameter

        QueryParamsMap test;
        resource->get(test, &onGet);
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

            if(resourceURI == "/a/garage")
            {
                curResource = resource;
                // Call a local function which will internally invoke
                // get API on the resource pointer
                getLightRepresentation(resource);
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
        "192.168.1.10",
        5683,
        OC::QualityOfService::NonConfirmable
    };

    // Create a OCPlatform instance.
    // Note: Platform creation is synchronous call.

    try
    {
        OCPlatform platform(cfg);
        std::cout << "Created Platform..."<<std::endl;
        // Find all resources
        platform.findResource("", "coap://224.0.1.187/oc/core?rt=core.garage", &foundResource);
        std::cout<< "Finding Resource... " <<std::endl;
        while(true)
        {
            // some operations
        }

    }
    catch(OCException& e)
    {
        //log(e.what());
    }

    return 0;
}


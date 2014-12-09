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
#include <mutex>
#include <condition_variable>
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
    std::vector<bool> m_lightStates;
    std::vector<int> m_lightPowers;
    OCRepresentation m_lightRep;
    std::vector<OCRepresentation> m_reps;

    Garage() : m_state(false), m_name("")
    {
    }
};

Garage myGarage;

void printRepresentation(const OCRepresentation& rep)
{

        // Check if attribute "name" exists, and then getValue
        if(rep.hasAttribute("name"))
        {
            myGarage.m_name = rep.getValue<std::string>("name");
        }
        std::cout << "\tname: " << myGarage.m_name << std::endl;

        // You can directly try to get the value. this function
        // return false if there is no attribute "state"
        if(!rep.getValue("state", myGarage.m_state))
        {
            std::cout << "Attribute state doesn't exist in the representation\n";
        }
        std::cout << "\tstate: " << myGarage.m_state << std::endl;

        OCRepresentation rep2 = rep;

        std::cout << "Number of attributes in rep2: "
                  << rep2.numberOfAttributes() << std::endl;

        if(rep2.erase("name"))
        {
            std::cout << "attribute: name, was removed successfully from rep2.\n";
        }

        std::cout << "Number of attributes in rep2: "
                  << rep2.numberOfAttributes() << std::endl;


        if(rep.isNULL("nullAttribute"))
        {
            std::cout << "\tnullAttribute is null." << std::endl;
        }
        else
        {
            std::cout << "\tnullAttribute is not null." << std::endl;
        }

        rep.getValue("light", myGarage.m_lightRep);

        myGarage.m_lightRep.getValue("states", myGarage.m_lightStates);
        myGarage.m_lightRep.getValue("powers", myGarage.m_lightPowers);

        std::cout << "\tlightRep: states: ";

        int first = 1;
        for(auto state: myGarage.m_lightStates)
        {
            if(first)
            {
                std::cout << state;
                first = 0;
            }
            else
            {
                std::cout << "," << state;
            }
        }

        std::cout << std::endl;
        std::cout << "\tlightRep: powers: ";
        first = 1;
        for(auto power: myGarage.m_lightPowers)
        {
            if(first)
            {
                std::cout << power;
                first = 0;
            }
            else
            {
                std::cout << "," << power;
            }
        }
        std::cout << std::endl;

        // Get vector of representations
        rep.getValue("reps", myGarage.m_reps);
        // Client know that server is sending two representations
        // and has key1 and key2 repsectively
        std::cout << "\treps[0].key1: " << myGarage.m_reps[0].getValue<int>("key1") << std::endl;
        std::cout << "\treps[0].key2: " << myGarage.m_reps[1].getValue<int>("key2") << std::endl;

        std::cout << "\tjson: " << rep.getValue<std::string>("json") << std::endl;
}
// callback handler on PUT request
void onPut(const HeaderOptions& headerOptions, const OCRepresentation& rep, const int eCode)
{
    if(eCode == SUCCESS_RESPONSE)
    {
        std::cout << "PUT request was successful" << std::endl;

        printRepresentation(rep);
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
void onGet(const HeaderOptions& headerOptions, const OCRepresentation& rep, const int eCode)
{
    if(eCode == SUCCESS_RESPONSE)
    {
        std::cout << "GET request was successful" << std::endl;
        std::cout << "Resource URI: " << rep.getUri() << std::endl;

        printRepresentation(rep);

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
        "0.0.0.0",
        0,
        OC::QualityOfService::LowQos
    };

    OCPlatform::Configure(cfg);
    try
    {
        // Find all resources
        OCPlatform::findResource("", "coap://224.0.1.187/oc/core?rt=core.garage",
                    &foundResource);
        std::cout<< "Finding Resource... " <<std::endl;

        // A condition variable will free the mutex it is given, then do a non-
        // intensive block until 'notify' is called on it.  In this case, since we
        // don't ever call cv.notify, this should be a non-processor intensive version
        // of while(true);
        std::mutex blocker;
        std::condition_variable cv;
        std::unique_lock<std::mutex> lock(blocker);
        cv.wait(lock);
    }
    catch(OCException& e)
    {
        std::cerr << "Exception in GarageClient: "<<e.what();
    }

    return 0;
}


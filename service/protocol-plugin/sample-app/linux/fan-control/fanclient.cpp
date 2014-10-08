/******************************************************************
*
* Copyright 2014 Samsung Electronics All Rights Reserved.
* : Initialization Part has been referenced from OCClient.cpp file of Intel copyright.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*******************************************************************/

// FanClient.cpp : Defines the entry point for the console application for controlling fan.
//
#include <string>
#include <cstdlib>
#include <pthread.h>
#include "OCPlatform.h"
#include "OCApi.h"

using namespace OC;

const int SUCCESS_RESPONSE = 0;
std::shared_ptr<OCResource> curResource;
static ObserveType OBSERVE_TYPE_TO_USE = ObserveType::Observe;

int observe_count()
{
    static int oc = 0;
    return ++oc;
}

void onObserve(const OCRepresentation &rep, const int &eCode, const int &sequenceNumber)
{
    if (eCode == SUCCESS_RESPONSE)
    {
        AttributeMap attributeMap = rep.getAttributeMap();

        std::cout << "OBSERVE RESULT:" << std::endl;
        std::cout << "\tSequenceNumber: " << sequenceNumber << endl;
        for (auto it = attributeMap.begin(); it != attributeMap.end(); ++it)
        {
            std::cout << "\tAttribute name: " << it->first << " value: ";
            for (auto valueItr = it->second.begin(); valueItr != it->second.end(); ++valueItr)
            {
                std::cout << "\t" << *valueItr << " ";
            }
            std::cout << std::endl;
        }
    }
    else
    {
        std::cout << "onObserve Response error: " << eCode << std::endl;
        std::exit(-1);
    }
}

// callback handler on PUT request
void onPut(const OCRepresentation &rep, const int eCode)
{
    if (eCode == SUCCESS_RESPONSE)
    {
        std::cout << "PUT request was successful" << std::endl;

        AttributeMap attributeMap = rep.getAttributeMap();

        for (auto it = attributeMap.begin(); it != attributeMap.end(); ++it)
        {
            std::cout << "\tAttribute name: " << it->first << " value: ";
            for (auto valueItr = it->second.begin(); valueItr != it->second.end(); ++valueItr)
            {
                std::cout << "\t" << *valueItr << " ";
            }

            std::cout << std::endl;
        }

        std::vector<OCRepresentation> children = rep.getChildren();

        for (auto oit = children.begin(); oit != children.end(); ++oit)
        {
            attributeMap = oit->getAttributeMap();

            for (auto it = attributeMap.begin(); it != attributeMap.end(); ++it)
            {
                std::cout << "\tAttribute name: " << it->first << " value: ";
                for (auto valueItr = it->second.begin(); valueItr != it->second.end(); ++valueItr)
                {
                    std::cout << "\t" << *valueItr << " ";
                }

                std::cout << std::endl;
            }
        }

        if (OBSERVE_TYPE_TO_USE == ObserveType::Observe)
            std::cout << endl << "Observe is used." << endl << endl;
        else if (OBSERVE_TYPE_TO_USE == ObserveType::ObserveAll)
            std::cout << endl << "ObserveAll is used." << endl << endl;

        QueryParamsMap test;

        curResource->observe(OBSERVE_TYPE_TO_USE, test, &onObserve);

    }
    else
    {
        std::cout << "onPut Response error: " << eCode << std::endl;
        std::exit(-1);
    }
}

// Local function to put a different state for this resource
void putFanRepresentation(std::shared_ptr<OCResource> resource)
{
    if (resource)
    {
        OCRepresentation rep;

        std::cout << "Putting light representation..." << std::endl;
        // Create AttributeMap
        AttributeMap attributeMap;
        // Add the attribute name and values in the attribute map
        AttributeValues stateVal;
        stateVal.push_back("true");

        AttributeValues powerVal;
        powerVal.push_back("1");

        AttributeValues healthVal;
        healthVal.push_back("1");


        attributeMap["state"] = stateVal;
        attributeMap["power"] = powerVal;
        attributeMap["health"] = healthVal;

        // Create QueryParameters Map and add query params (if any)
        QueryParamsMap queryParamsMap;

        rep.setAttributeMap(attributeMap);

        // Invoke resource's pit API with attribute map, query map and the callback parameter
        resource->put(rep, queryParamsMap, &onPut);
    }
}


void putFanRepresentationON(std::shared_ptr<OCResource> resource)
{
    if (resource)
    {
        OCRepresentation rep;

        std::cout << "Putting Fan representation..." << std::endl;
        // Create AttributeMap
        AttributeMap attributeMap;
        // Add the attribute name and values in the attribute map
        AttributeValues stateVal;
        stateVal.push_back("true");

        AttributeValues powerVal;
        powerVal.push_back("1");

        AttributeValues healthVal;
        healthVal.push_back("1");


        attributeMap["state"] = stateVal;
        attributeMap["power"] = powerVal;
        attributeMap["health"] = healthVal;

        // Create QueryParameters Map and add query params (if any)
        QueryParamsMap queryParamsMap;

        rep.setAttributeMap(attributeMap);

        // Invoke resource's pit API with attribute map, query map and the callback parameter
        resource->put(rep, queryParamsMap, &onPut);
    }
}

void putFanRepresentationOFF(std::shared_ptr<OCResource> resource)
{
    if (resource)
    {
        OCRepresentation rep;

        std::cout << "Putting fan representation..." << std::endl;
        // Create AttributeMap
        AttributeMap attributeMap;
        // Add the attribute name and values in the attribute map
        AttributeValues stateVal;
        stateVal.push_back("false");

        AttributeValues powerVal;
        powerVal.push_back("0");

        AttributeValues healthVal;
        healthVal.push_back("0");


        attributeMap["state"] = stateVal;
        attributeMap["power"] = powerVal;
        attributeMap["health"] = healthVal;

        // Create QueryParameters Map and add query params (if any)
        QueryParamsMap queryParamsMap;

        // Invoke resource's pit API with attribute map, query map and the callback parameter
        rep.setAttributeMap(attributeMap);

        // Invoke resource's pit API with attribute map, query map and the callback parameter
        resource->put(rep, queryParamsMap, &onPut);
    }
}


// callback handler on GET request
void onGet(const OCRepresentation &rep, const int eCode)
{
    if (eCode == SUCCESS_RESPONSE)
    {
        std::cout << "GET Succeeded:" << std::endl;

        AttributeMap attributeMap = rep.getAttributeMap();
        std::cout << "Resource URI: " << rep.getUri() << std::endl;

        for (auto it = attributeMap.begin(); it != attributeMap.end(); ++it)
        {
            std::cout << "\tAttribute name: " << it->first << " value: ";
            for (auto valueItr = it->second.begin(); valueItr != it->second.end(); ++valueItr)
            {
                std::cout << "\t" << *valueItr << " ";
            }

            std::cout << std::endl;
        }

        std::vector<OCRepresentation> children = rep.getChildren();

        for (auto oit = children.begin(); oit != children.end(); ++oit)
        {
            std::cout << "Child Resource URI: " << oit->getUri() << std::endl;

            attributeMap = oit->getAttributeMap();

            for (auto it = attributeMap.begin(); it != attributeMap.end(); ++it)
            {
                std::cout << "\tAttribute name: " << it->first << " value: ";
                for (auto valueItr = it->second.begin(); valueItr != it->second.end(); ++valueItr)
                {
                    std::cout << "\t" << *valueItr << " ";
                }

                std::cout << std::endl;
            }
        }
        putFanRepresentation(curResource);
    }
    else
    {
        std::cout << "onGet Response error: " << eCode << std::endl;
        std::exit(-1);
    }
}
// Local function to get representation of fan resource
void getFanRepresentation(std::shared_ptr<OCResource> resource)
{
    if (resource)
    {
        std::cout << "Getting Light Representation..." << std::endl;
        // Invoke resource's get API with the callback parameter

        QueryParamsMap test;
        resource->get(test, &onGet);
    }
}

// Callback to found resources
void foundResource(std::shared_ptr<OCResource> resource)
{

    if (curResource)
    {
        std::cout << "Found another resource, ignoring" << std::endl;
    }

    std::string resourceURI;
    std::string hostAddress;
    try
    {
        // Do some operations with resource object.
        if (resource)
        {
            std::cout << "DISCOVERED Resource:" << std::endl;
            // Get the resource URI
            resourceURI = resource->uri();
            std::cout << "\tURI of the resource: " << resourceURI << std::endl;

            // Get the resource host address
            hostAddress = resource->host();
            std::cout << "\tHost address of the resource: " << hostAddress << std::endl;
            // Get the resource types
            std::cout << "\tList of resource types: " << std::endl;
            for (auto &resourceTypes : resource->getResourceTypes())
            {
                std::cout << "\t\t" << resourceTypes << std::endl;
            }

            // Get the resource interfaces
            std::cout << "\tList of resource interfaces: " << std::endl;
            for (auto &resourceInterfaces : resource->getResourceInterfaces())
            {
                std::cout << "\t\t" << resourceInterfaces << std::endl;
            }

            if (resourceURI == "/a/fan")
            {
                curResource = resource;
                // Call a local function which will internally invoke get API on the resource pointer
                getFanRepresentation(resource);
            }
        }
        else
        {
            // Resource is invalid
            std::cout << "Resource is invalid" << std::endl;
        }

    }
    catch (std::exception &e)
    {
        //log(e.what());
    }
}

int main(int argc, char *argv[])
{
    PlatformConfig cfg
    {
        OC::ServiceType::InProc,
        OC::ModeType::Client,
        "192.168.2.5",
        5683,
        OC::QualityOfService::NonConfirmable
    };

    // Create a OCPlatform instance.
    // Note: Platform creation is synchronous call.

    try
    {
        OCPlatform platform(cfg);
        std::cout << "Created Platform..." << std::endl;
        // Find all resources
        platform.findResource("", "coap://224.0.1.187/oc/core?rt=core.fan", &foundResource);
        std::cout << "Finding Resource... " << std::endl;
        sleep(20);
        while (true)
        {
            //Test Simple Scenario
            std::cout << "Turn off the Fan" << std::endl;
            putFanRepresentationOFF(curResource);
            sleep(15);
            std::cout << "Turn on the Fan" << std::endl;
            putFanRepresentationON(curResource);
            sleep(30);
            // some operations
        }
    }
    catch (OCException e)
    {
        //log(e.what());
    }

    return 0;
}


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

// Client_test.cpp : Defines the entry point for the console application.
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
int color;
int bri;


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

        if (observe_count() > 3)
        {
            std::cout << "Cancelling Observe..." << std::endl;
            OCStackResult result = curResource->cancelObserve();

            std::cout << "Cancel result: " << result << std::endl;
            sleep(10);
            std::cout << "DONE" << std::endl;
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

//        QueryParamsMap test;
//        curResource->observe(OBSERVE_TYPE_TO_USE, test,&onObserve);
    }
    else
    {
        std::cout << "onPut Response error: " << eCode << std::endl;
        std::exit(-1);
    }
}

// Local function to put a different state for this resource
void putLightRepresentation(std::shared_ptr<OCResource> resource, char *action, int color,
                            int bright)
{
    if (resource)
    {
        OCRepresentation rep;
        std::cout << "Putting light representation..." << std::endl;
        // Create AttributeMap
        AttributeMap attributeMap;
        // Add the attribute name and values in the attribute map
        AttributeValues stateVal;
        AttributeValues colorVal;
        AttributeValues brightVal;
        stateVal.push_back(action);
        colorVal.push_back(to_string(color));
        brightVal.push_back(to_string(bright));

        attributeMap["state"] = stateVal;
        attributeMap["color"] = colorVal;
        attributeMap["bright"] = brightVal;

        // Create QueryParameters Map and add query params (if any)
        QueryParamsMap queryParamsMap;
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

//        putLightRepresentation(curResource);
    }
    else
    {
        std::cout << "onGet Response error: " << eCode << std::endl;
        std::exit(-1);
    }
}
// Local function to get representation of light resource
void getLightRepresentation(std::shared_ptr<OCResource> resource)
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
        exit(-1);
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

            if (!resourceURI.empty())
            {
                curResource = resource;
                // Call a local function which will internally invoke get API on the resource pointer
                getLightRepresentation(resource);
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


void UpResource(std::shared_ptr<OCResource> resource)
{
    foundResource(resource);
    putLightRepresentation(resource, "true", color, bri);
}


void DownResource(std::shared_ptr<OCResource> resource)
{
    foundResource(resource);
    putLightRepresentation(resource, "false", 0, 0);
}


int main(int argc, char *argv[])
{
    // Create PlatformConfig object
/*
    PlatformConfig cfg;
    cfg.ipAddress = "192.168.1.10";
    cfg.port = 5683;
    cfg.mode = ModeType::Client;
    cfg.serviceType = ServiceType::InProc;
*/
    PlatformConfig cfg {
        OC::ServiceType::InProc,
        OC::ModeType::Client,
        "192.168.2.5",
        56832,
        OC::QualityOfService::NonConfirmable
    };
    // Create a OCPlatform instance.
    // Note: Platform creation is synchronous call.
    try
    {
        OCPlatform platform(cfg);
        if (argc == 1)
        {
            std::cout << "Created Platform..." << std::endl;
            // Find all resources
            platform.findResource("", "coap://224.0.1.187/oc/core?rt=core.container", &foundResource);
            std::cout << "Finding Resource... " << std::endl;
        }
        else if ((argc == 2) && strcmp(argv[1], "off") == 0)
        {
            std::cout << "Created Platform..." << std::endl;
            // Find all resources
            platform.findResource("", "coap://224.0.1.187/oc/core?rt=core.container", &DownResource);
            std::cout << "Finding Resource and Down... " << std::endl;
        }
        else if ((argc == 4) && strcmp(argv[1], "on") == 0)
        {
            std::cout << "Created Platform..." << std::endl;
            // Find all resources
            color = atoi(argv[2]);
            bri = atoi(argv[3]);
            platform.findResource("", "coap://224.0.1.187/oc/core?rt=core.container", &UpResource);
            std::cout << "Finding Resource  Up and Set color and bright... " << std::endl;
        }
        else
        {
            std::cout <<
                      "Usage: oic-light-client off \n    oic-light-client on 20000 200 [color:0~65535, bri:0~255]" <<
                      std::endl;
        }
        sleep(5);
    }
    catch (OCException e)
    {
        //log(e.what());
    }

    return 0;
}


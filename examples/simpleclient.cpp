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

#include "OCPlatform.h"

using namespace OC;

const int SUCCESS_RESPONSE = 200; 

// callback handler on GET request
void onGet(const AttributeMap& attributeMap, const int& eCode)
{
    if(eCode == SUCCESS_RESPONSE)
    {
        for(auto it = attributeMap.begin(); it != attributeMap.end(); ++it)
        {
            std::cout << "Attribute name: "<< it->first << " value: ";
            for(auto valueItr = it->second.begin(); valueItr != it->second.end(); ++valueItr)
            {
                std::cout << *valueItr << " ";
            }

            std::cout << std::endl;
        }
    }
    else
    {
        std::cout << "Response error: " << eCode << std::endl;
    }
}

// callback handler on PUT request
void onPut(const int& eCode)
{
    if(eCode == SUCCESS_RESPONSE)
    {
        std::cout << "Put request was successful" << std::endl;
    }
    else
    {
        std::cout << "Response error: " << eCode << std::endl;
    }
}

// callback handler on observation
void onObserve(const AttributeMap& attributeMap, const int& eCode)
{
    if(eCode == SUCCESS_RESPONSE)
    {
        std::cout << "Observe callback invoked" << std::endl;
        for(auto it = attributeMap.begin(); it != attributeMap.end(); ++it)
        {
            std::cout << "Attribute name: "<< it->first << " value: ";
            for(auto valueItr = it->second.begin(); valueItr != it->second.end(); ++valueItr)
            {
                std::cout << *valueItr << " ";
            }

            std::cout << std::endl;
        }
    }
    else
    {
        std::cout << "Response error: " << eCode << std::endl;
    }
}

// callback handler for observe cancellation
void onCancelObservation(const int& eCode)
{
    if(eCode == SUCCESS_RESPONSE)
    {
        std::cout << "Put request was successful" << std::endl;
    }
    else
    {
        std::cout << "Response error: " << eCode << std::endl;
    }
}

// Local function to put a different state for this resource
void putLightRepresentation(std::shared_ptr<OCResource> resource)
{
    if(resource)
    {
        // Create AttributeMap
        AttributeMap attributeMap;
        // Add the attribute name and values in the attribute map
        AttributeValues stateVal; 
        stateVal.push_back("true");

        AttributeValues powerVal; 
        powerVal.push_back("100");

        attributeMap["state"] = stateVal;
        attributeMap["power"] = powerVal; 

        // Create QueryParameters Map and add query params (if any)
        QueryParamsMap queryParamsMap; 
        
        // Invoke resource's pit API with attribute map, query map and the callback parameter
        resource->put(attributeMap, queryParamsMap, &onPut);
    }
}

// Local function to get representation of light resource
void getLightRepresentation(std::shared_ptr<OCResource> resource)
{
    if(resource)
    {
        // Invoke resource's get API with the callback parameter
        resource->get(&onGet);
    }
}

// Local function to observe on a resource (in this case light)
void observeOnLight(std::shared_ptr<OCResource> resource)
{
    if(resource)
    {
        // Invoke resource's observe API with the callback parameter
        resource->observe(&onObserve);
    }
}

// Local function to cancel the observation on a resource
void cancelObservation(std::shared_ptr<OCResource> resource)
{
    if(resource)
    {
        // Invoke resource's cancelObserve API with the callback parameter
        resource->cancelObserve(&onCancelObservation);
    }
}

// Callback to found resources
void foundResource(std::shared_ptr<OCResource> resource)
{
    std::string resourceURI;
    std::string hostAddress;
    try
    {
        // Do some operations with resource object. 
        if(resource)
        {
            // Get the resource URI
            resourceURI = resource->uri();
            std::cout << "URI of the resource: " << resourceURI << std::endl;
            
            // Get the resource host address
            hostAddress = resource->host();
            std::cout << "Host address of the resource: " << hostAddress << std::endl;      

            if(resourceURI == "/a/light")
            {
                // Call a local function which will internally invoke get API on the resource pointer
                getLightRepresentation(resource); 

                // Do some operations 

                // Call a local function which will internally invoke get API on the resource pointer
                putLightRepresentation(resource); 

                // Do some operations 

                // Call a local function which will internally observe on this resource for further notifications
                observeOnLight(resource);

                // Do some operations

                // Call a local function which will internally cancel observation on this resource
                cancelObservation(resource);
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


int main()
{
    // Create PlatformConfig object

    PlatformConfig cfg;
    cfg.ipAddress = "134.134.161.33";
    cfg.port = 5683;
    cfg.mode = ModeType::Client;
    cfg.serviceType = ServiceType::InProc;

    // Create a OCPlatform instance. 
    // Note: Platform creation is synchronous call. 

    try
    {
        OCPlatform platform(cfg);

        // Find all resources
        platform.findResource("", "coap://224.0.1.187/oc/core", &foundResource);

        while(true)
        {
            // some operations
        }

    }catch(OCException e)
    {
        //log(e.what());
    }

    return 0;
}


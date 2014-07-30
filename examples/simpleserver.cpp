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

///
/// This sample provides steps to define an interface for a resource
/// (properties and methods) and host this resource on the server. 
///

#include <functional>

#include "OCPlatform.h"
#include "OCApi.h"

using namespace OC;

// This is just a sample implementation of entity handler. 
// Entity handler can be implemented in several ways by the manufacturer
void entityHandler(std::shared_ptr<OCResourceRequest> request, std::shared_ptr<OCResourceResponse> response)
{
    // add the headers in this map before sending the response
    HeadersMap headersMap; 
    headersMap["content-type"] = "text";
    headersMap["server"] = "serverName";

    if(request)
    {
        // Get the request type and request flag
        std::string requestType = request->getRequestType();
        RequestHandlerFlag requestFlag = request->getRequestHandlerFlag();

        if(requestFlag == RequestHandlerFlag::InitFlag)
        {
            // entity handler to perform resource initialization operations
        }
        else if(requestFlag == RequestHandlerFlag::RequestFlag)
        {
            // If the request type is GET
            if(requestType == "GET")
            {
                // Check for query params (if any)
                QueryParamsMap queryParamsMap = request->getQueryParameters();

                // Process query params and do required operations ..

                // Get the representation of this resource at this point and send it as response
                AttributeMap attributeMap; 
                AttributeValues stateVal; 
                stateVal.push_back("false");

                AttributeValues powerVal; 
                powerVal.push_back("0");

                attributeMap["state"] = stateVal;
                attributeMap["power"] = powerVal; 

                if(response)
                {
                    response->setResponseHeaders(headersMap);
                    response->setHTTPErrorCode(200);
                    response->setResourceRepresentation(attributeMap);
                }
            }
            else if(requestType == "PUT")
            {
                // Check for query params (if any)
                QueryParamsMap queryParamsMap = request->getQueryParameters();

                // Check queryParamsMap and do required operations ..

                // Get the representation from the request
                AttributeMap attributeMap = request->getResourceRepresentation();
                
                // Do related operations related to PUT request 
                // Change the attribute map accordingly and send a response

                AttributeValues stateVal; 
                stateVal.push_back("true");

                AttributeValues powerVal; 
                powerVal.push_back("100");

                attributeMap["state"] = stateVal;
                attributeMap["power"] = powerVal; 

                if(response)
                {
                    response->setResponseHeaders(headersMap);
                    response->setHTTPErrorCode(200);
                    response->setResourceRepresentation(attributeMap);
                }
            }
            else if(requestType == "POST")
            {
                // POST request operations
            }
            else if(requestType == "DELETE")
            {
                // DELETE request operations
            }
        }
        else if(requestFlag == RequestHandlerFlag::ObserverFlag)
        {
            // perform observe related operations on the resource. 
            // Add the attributes to the map and send a response

            // on any attribute change on the light resource hardware, 
            // set the attributes and send response
            AttributeMap attributeMap;
            
            AttributeValues stateVal; 
            stateVal.push_back("false");

            AttributeValues powerVal; 
            powerVal.push_back("0");

            attributeMap["state"] = stateVal;
            attributeMap["power"] = powerVal; 

            if(response)
            {
                response->setResponseHeaders(headersMap);
                response->setHTTPErrorCode(200);
                response->setResourceRepresentation(attributeMap);
            }
        }
    }
    else
    {
        std::cout << "Request invalid" << std::endl;
    }
}

/// This class represents a single resource named 'lightResource'. This resource has 
/// two simple properties named 'state' and 'power' 

class LightResource
{
public:
    /// Access this property from a TB client 
    bool m_state;
    int m_power;

public:
    /// Constructor
    LightResource(): m_state(false), m_power(0){}

    /* Note that this does not need to be a member function: for classes you do not have
    access to, you can accomplish this with a free function: */
    
    /// This function internally calls registerResource API.
    void createResource(OC::OCPlatform& platform)
    {
        std::string resourceURI = "a/light"; // URI of the resource
        std::string resourceTypeName = "light"; // resource type name. In this case, it is light
        std::string resourceInterface = LINK_INTERFACE; // resource interface.
        ResourceFlag resourceFlag = ResourceFlag::ObserverFlag; // set the resource flag to Observerable

        // This will internally create and register the resource. 
        platform.registerResource(resourceURI, resourceTypeName, resourceInterface, &entityHandler, resourceFlag); 
    }
};

int main()
{
    // Create PlatformConfig object

    PlatformConfig cfg;
    cfg.ipAddress = "134.134.161.33";
    cfg.port = 5683;
    cfg.mode = ModeType::Server;
    cfg.serviceType = ServiceType::InProc;
    
    // Create a OCPlatform instance. 
    // Note: Platform creation is synchronous call. 
    try
    {
        OCPlatform platform(cfg);

        // Create the instance of the resource class (in this case instance of class 'LightResource'). 
        // Invoke bindTo function of class light. 

        LightResource myLightResource;
        myLightResource.createResource(platform);
        
        // Perform app tasks
        while(true)
        {
            // some tasks
        }
    }
    catch(OCException e)
    {
        //log(e.what());
    }

        
    // No explicit call to stop the platform. 
    // When OCPlatform destructor is invoked, internally we do platform cleanup
}

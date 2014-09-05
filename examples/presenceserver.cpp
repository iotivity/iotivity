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

///
/// This sample provides steps to define an interface for a resource
/// (properties and methods) and host this resource on the server.
///

#include <functional>

#include <pthread.h>

#include "OCPlatform.h"
#include "OCApi.h"

using namespace OC;
using namespace std;

// Forward declaring the entityHandler
void entityHandler(std::shared_ptr<OCResourceRequest> request, std::shared_ptr<OCResourceResponse> response);

/// This class represents a single resource named 'lightResource'. This resource has
/// two simple properties named 'state' and 'power'

class LightResource
{
public:
    /// Access this property from a TB client
    bool m_state;
    int m_power;
    std::string m_lightUri;
    std::string m_lightUri2;
    std::string m_lightUri3;
    OCResourceHandle m_resourceHandle;
    OCResourceHandle m_resourceHandle2;
    OCResourceHandle m_resourceHandle3;

public:
    /// Constructor
    LightResource(): m_state(false), m_power(0), m_lightUri("/a/light"), 
                     m_lightUri2("/a/light2"),m_lightUri3("/a/light3") {}

    /* Note that this does not need to be a member function: for classes you do not have
    access to, you can accomplish this with a free function: */

    /// This function internally calls registerResource API.
    void createResource(OC::OCPlatform& platform)
    {
        std::string resourceURI = m_lightUri; // URI of the resource
        std::string resourceTypeName = "core.light"; // resource type name. In this case, it is light
        std::string resourceInterface = DEFAULT_INTERFACE; // resource interface.

        // OCResourceProperty is defined ocstack.h
        uint8_t resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE;

        // This will internally create and register the resource.
        OCStackResult result = platform.registerResource(
                                    m_resourceHandle, resourceURI, resourceTypeName,
                                    resourceInterface, &entityHandler, resourceProperty);

        if (OC_STACK_OK != result)
        {
            cout << "Resource creation was unsuccessful\n";
        }
    }

    /// This function internally calls registerResource API.
    void createResource2(OC::OCPlatform& platform)
    {
        std::string resourceURI = m_lightUri2; // URI of the resource
        std::string resourceTypeName = "core.light"; // resource type name. In this case, it is light
        std::string resourceInterface = DEFAULT_INTERFACE; // resource interface.

        // OCResourceProperty is defined ocstack.h
        uint8_t resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE;

        // This will internally create and register the resource.
        OCStackResult result = platform.registerResource(
                                    m_resourceHandle2, resourceURI, resourceTypeName,
                                    resourceInterface, &entityHandler, resourceProperty);

        if (OC_STACK_OK != result)
        {
            cout << "Resource creation was unsuccessful\n";
        }
    }

    void createResource3(OC::OCPlatform& platform)
    {
        std::string resourceURI = m_lightUri3; // URI of the resource
        std::string resourceTypeName = "core.light";
        std::string resourceInterface = DEFAULT_INTERFACE; // resource interface.

        // OCResourceProperty is defined ocstack.h
        uint8_t resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE;

        // This will internally create and register the resource.
        OCStackResult result = platform.registerResource(
                                    m_resourceHandle3, resourceURI, resourceTypeName,
                                    resourceInterface, &entityHandler, resourceProperty);

        if (OC_STACK_OK != result)
        {
            cout << "Resource creation was unsuccessful\n";
        }
    }

    OCResourceHandle getHandle()
    {
        return m_resourceHandle;
    }

    void setRepresentation(OCRepresentation& light)
    {
        AttributeMap attributeMap = light.getAttributeMap();

        if(attributeMap.find("state") != attributeMap.end() && attributeMap.find("power") != attributeMap.end())
        {
            cout << "\t\t\t" << "Received representation: " << endl;
            cout << "\t\t\t\t" << "power: " << attributeMap["power"][0] << endl;
            cout << "\t\t\t\t" << "state: " << attributeMap["state"][0] << endl;

            m_state = attributeMap["state"][0].compare("true") == 0;
            m_power= std::stoi(attributeMap["power"][0]);
        }
    }

    OCRepresentation getRepresentation()
    {
        OCRepresentation light;

        light.setUri(m_lightUri);

        std::vector<std::string> interfaces;
        //interfaces.push_back(m_lightInterface);

        light.setResourceInterfaces(interfaces);

        std::vector<std::string> types;
        //types.push_back(m_lightType);

        light.setResourceTypes(types);

        AttributeMap attributeMap;
        AttributeValues stateVal;
        if(m_state)
        {
            stateVal.push_back("true");
        }
        else
        {
            stateVal.push_back("false");
        }

        AttributeValues powerVal;
        powerVal.push_back(to_string(m_power));

        attributeMap["state"] = stateVal;
        attributeMap["power"] = powerVal;

        light.setAttributeMap(attributeMap);

        return light;
    }

    void addType(const OC::OCPlatform& platform, const std::string& type) const
    {
        OCStackResult result = platform.bindTypeToResource(m_resourceHandle, type);
        if (OC_STACK_OK != result)
        {
            cout << "Binding TypeName to Resource was unsuccessful\n";
        }
    }

    void addInterface(const OC::OCPlatform& platform, const std::string& interface) const
    {
        OCStackResult result = platform.bindInterfaceToResource(m_resourceHandle, interface);
        if (OC_STACK_OK != result)
        {
            cout << "Binding TypeName to Resource was unsuccessful\n";
        }
    }
};

// Create the instance of the resource class (in this case instance of class 'LightResource').
LightResource myLightResource;

// This is just a sample implementation of entity handler.
// Entity handler can be implemented in several ways by the manufacturer
void entityHandler(std::shared_ptr<OCResourceRequest> request, std::shared_ptr<OCResourceResponse> response)
{
    cout << "\tIn Server CPP entity handler:\n";

    if(request)
    {
        // Get the request type and request flag
        std::string requestType = request->getRequestType();
        RequestHandlerFlag requestFlag = request->getRequestHandlerFlag();

        if(requestFlag == RequestHandlerFlag::InitFlag)
        {
            cout << "\t\trequestFlag : Init\n";

            // entity handler to perform resource initialization operations
        }
        else if(requestFlag == RequestHandlerFlag::RequestFlag)
        {
            cout << "\t\trequestFlag : Request\n";

            // If the request type is GET
            if(requestType == "GET")
            {
                cout << "\t\t\trequestType : GET\n";

                // Check for query params (if any)
                QueryParamsMap queryParamsMap = request->getQueryParameters();

                cout << "\t\t\tquery params: \n";
                for(QueryParamsMap::iterator it = queryParamsMap.begin(); it != queryParamsMap.end(); it++)
                {
                    cout << "\t\t\t\t" << it->first << ":" << it->second << endl;
                }

                // Process query params and do required operations ..

                // Get the representation of this resource at this point and send it as response
                // AttributeMap attributeMap;
                OCRepresentation rep;
                rep = myLightResource.getRepresentation();

                if(response)
                {
                    // TODO Error Code
                    response->setErrorCode(200);

                    auto findRes = queryParamsMap.find("if");

                    if(findRes != queryParamsMap.end())
                    {
                        response->setResourceRepresentation(rep, findRes->second);
                    }
                    else
                    {
                        response->setResourceRepresentation(rep, DEFAULT_INTERFACE);
                    }
                }
            }
            else if(requestType == "PUT")
            {
                cout << "\t\t\trequestType : PUT\n";

                // Check for query params (if any)
                QueryParamsMap queryParamsMap = request->getQueryParameters();

                cout << "\t\t\tquery params: \n";
                for(auto it = queryParamsMap.begin(); it != queryParamsMap.end(); it++)
                {
                    cout << "\t\t\t\t" << it->first << ":" << it->second << endl;
                }

                // Get the representation from the request
                OCRepresentation rep = request->getResourceRepresentation();

                myLightResource.setRepresentation(rep);

                // Do related operations related to PUT request
                rep = myLightResource.getRepresentation();

                if(response)
                {
                    // TODO Error Code
                    response->setErrorCode(200);

                    auto findRes = queryParamsMap.find("if");

                    if(findRes != queryParamsMap.end())
                    {
                        response->setResourceRepresentation(rep, findRes->second);
                    }
                    else
                    {
                        response->setResourceRepresentation(rep, DEFAULT_INTERFACE);
                    }
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
            // OBSERVE flag operations
        }
    }
    else
    {
        std::cout << "Request invalid" << std::endl;
    }
}

int main()
{
    // Create PlatformConfig object
    PlatformConfig cfg {
        OC::ServiceType::InProc,
        OC::ModeType::Server,
        "134.134.161.33",
        56832,
        OC::QualityOfService::NonConfirmable
    };

    // Create a OCPlatform instance.
    // Note: Platform creation is synchronous call.
    try
    {
        OCPlatform platform(cfg);

        // Time to Live is 30 seconds
        platform.startPresence(30);

        // Invoke createResource function of class light.
        myLightResource.createResource(platform);

        printf("\nEnter a key to create the second resource\n");
        getchar();

        myLightResource.createResource2(platform);

        printf("\nEnter a key to stop the presence\n");
        getchar();
        platform.stopPresence();

        printf("\nEnter a key to restart the presence\n");
        getchar();

        platform.startPresence(30);

        printf("\nEnter a key to create the third resource\n");
        getchar();

        myLightResource.createResource3(platform);

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

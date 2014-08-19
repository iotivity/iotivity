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

#include <pthread.h>

#include "OCPlatform.h"
#include "OCApi.h"

using namespace OC;
using namespace std;

int gObservation = 0;

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
    OCResourceHandle m_resourceHandle;

public:
    /// Constructor
    LightResource(): m_state(false), m_power(0){}

    /* Note that this does not need to be a member function: for classes you do not have
    access to, you can accomplish this with a free function: */

    /// This function internally calls registerResource API.
    void createResource(OC::OCPlatform& platform)
    {
        std::string resourceURI = "/a/light"; // URI of the resource
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

    OCResourceHandle getHandle()
    {
        return m_resourceHandle;
    }

    void setRepresentation(AttributeMap& attributeMap)
    {
        cout << "\t\t\t" << "Received representation: " << endl;
        cout << "\t\t\t\t" << "power: " << attributeMap["power"][0] << endl;
        cout << "\t\t\t\t" << "state: " << attributeMap["state"][0] << endl;

        m_state = attributeMap["state"][0].compare("true") == 0;
        m_power = std::stoi(attributeMap["power"][0]);
    }

    void getRepresentation(AttributeMap& attributeMap)
    {
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

// ChangeLightRepresentaion is an observation function,
// which notifies any changes to the resource to stack
// via notifyObservers
void * ChangeLightRepresentation (void *param)
{
    // This function continuously monitors for the changes
    while (1)
    {
        sleep (5);

        if (gObservation)
        {
            // If under observation if there are any changes to the light resource
            // we call notifyObservors
            //
            // For demostration we are changing the power value and notifying.
            myLightResource.m_power += 10;

            cout << "\nPower updated to : " << myLightResource.m_power << endl;
            cout << "Notifying observers with resource handle: " << myLightResource.getHandle() << endl;

            OCStackResult result = OCPlatform::notifyObservers(myLightResource.getHandle());

            if(OC_STACK_NO_OBSERVERS == result)
            {
                cout << "No More observers, stopping notifications" << endl;
                gObservation = 0;
            }
        }
    }

    return NULL;
}


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
                AttributeMap attributeMap;

                myLightResource.getRepresentation(attributeMap);

                if(response)
                {
                    // TODO Error Code
                    response->setErrorCode(200);
                    response->setResourceRepresentation(attributeMap);
                }
            }
            else if(requestType == "PUT")
            {
                cout << "\t\t\trequestType : PUT\n";

                // Check for query params (if any)
                QueryParamsMap queryParamsMap = request->getQueryParameters();

                // Check queryParamsMap and do required operations ..

                // Get the representation from the request
                AttributeMap attributeMap = request->getAttributeRepresentation();

                myLightResource.setRepresentation(attributeMap);

                // Do related operations related to PUT request

                myLightResource.getRepresentation(attributeMap);

                if(response)
                {
                    response->setErrorCode(200);
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
            pthread_t threadId;

            cout << "\t\trequestFlag : Observer\n";
            gObservation = 1;

            static int startedThread = 0;

            // Observation happens on a different thread in ChangeLightRepresentation function.
            // If we have not created the thread already, we will create one here.
            if(!startedThread)
            {
                pthread_create (&threadId, NULL, ChangeLightRepresentation, (void *)NULL);
                startedThread = 1;
            }

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

    PlatformConfig cfg;
    cfg.ipAddress = "134.134.161.33";
    cfg.port = 56832;
    cfg.mode = ModeType::Server;
    cfg.serviceType = ServiceType::InProc;

    // Create a OCPlatform instance.
    // Note: Platform creation is synchronous call.
    try
    {
        OCPlatform platform(cfg);

        // Invoke createResource function of class light.

        myLightResource.createResource(platform);
        myLightResource.addType(platform, std::string("core.brightlight"));
        myLightResource.addInterface(platform, std::string("oc.mi.ll"));
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

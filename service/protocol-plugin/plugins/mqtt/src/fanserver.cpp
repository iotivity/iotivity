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
#include <cpluff.h>

#include "OCPlatform.h"
#include "OCApi.h"
#include "../lib/mosquitto.h"


using namespace OC;
using namespace std;

int gObservation = 0;

// Forward declaring the entityHandler
void entityHandler(std::shared_ptr<OCResourceRequest> request,
                   std::shared_ptr<OCResourceResponse> response);

/// This class represents a single resource named 'fanResource'. This resource has
/// two simple properties named 'state' and 'power'
typedef struct plugin_data_t plugin_data_t;

struct plugin_data_t
{
    cp_context_t *ctx;
    pthread_t m_thread;                                 // 2
    void *str;
    bool flag;
};


class FanResource
{
    public:
        /// Access this property from a TB client
        bool m_state;
        int m_power;
        int m_health;
        std::string m_fanUri;
        OCResourceHandle m_resourceHandle;

    public:
        /// Constructor
        FanResource(): m_state(false), m_power(0), m_health(0), m_fanUri {"/a/fan"} { }

        /* Note that this does not need to be a member function: for classes you do not have
        access to, you can accomplish this with a free function: */

        /// This function internally calls registerResource API.
        void createResource(OC::OCPlatform &platform)
        {
            std::string resourceURI = m_fanUri; // URI of the resource
            std::string resourceTypeName = "core.fan"; // resource type name. In this case, it is fan
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

        void setRepresentation(OCRepresentation &fan)
        {
            AttributeMap attributeMap = fan.getAttributeMap();

            if (attributeMap.find("state") != attributeMap.end()
                && attributeMap.find("power") != attributeMap.end()
                && attributeMap.find("health") != attributeMap.end())
            {
                cout << "\t\t\t" << "Received representation: " << endl;
                cout << "\t\t\t\t" << "power: " << attributeMap["power"][0] << endl;
                cout << "\t\t\t\t" << "state: " << attributeMap["state"][0] << endl;
                cout << "\t\t\t\t" << "health: " << attributeMap["health"][0] << endl;

                m_state = attributeMap["state"][0].compare("true") == 0;
                m_power = std::stoi(attributeMap["power"][0]);
                m_health = std::stoi(attributeMap["health"][0]);
            }

        }

        OCRepresentation getRepresentation()
        {
            OCRepresentation fan;

            fan.setUri(m_fanUri);

            std::vector<std::string> interfaces;
            fan.setResourceInterfaces(interfaces);

            std::vector<std::string> types;
            fan.setResourceTypes(types);

            AttributeMap attributeMap;
            AttributeValues stateVal;

            if (m_state)
            {
                stateVal.push_back("true");
            }
            else
            {
                stateVal.push_back("false");
            }

            AttributeValues powerVal;
            powerVal.push_back(to_string(m_power));

            AttributeValues healthVal;
            healthVal.push_back(to_string(m_health));

            attributeMap["state"] = stateVal;
            attributeMap["power"] = powerVal;
            attributeMap["health"] = healthVal;

            fan.setAttributeMap(attributeMap);

            return fan;
        }

        void addType(const OC::OCPlatform &platform, const std::string &type) const
        {
            OCStackResult result = platform.bindTypeToResource(m_resourceHandle, type);
            if (OC_STACK_OK != result)
            {
                cout << "Binding TypeName to Resource was unsuccessful\n";
            }
        }

        void addInterface(const OC::OCPlatform &platform, const std::string &interface) const
        {
            OCStackResult result = platform.bindInterfaceToResource(m_resourceHandle, interface);
            if (OC_STACK_OK != result)
            {
                cout << "Binding TypeName to Resource was unsuccessful\n";
            }
        }
};

// Create the instance of the resource class (in this case instance of class 'FanResource').
FanResource myFanResource;
struct mosquitto *myMosquitto;

// ChangeFanRepresentaion is an observation function,
// which notifies any changes to the resource to stack
// via notifyObservers
void *ChangeFanRepresentation (void *param)
{
    // This function continuously monitors for the changes
    while (1)
    {
        sleep (5);

        if (gObservation)
        {
            // If under observation if there are any changes to the fan resource
            // we call notifyObservors
            //
            // For demostration we are changing the power value and notifying.
            if (myFanResource.m_health != 0)
                myFanResource.m_health += 1;

            cout << "\nPower updated to : " << myFanResource.m_health << endl;
            cout << "Notifying observers with resource handle: " << myFanResource.getHandle() << endl;

            OCStackResult result = OCPlatform::notifyObservers(myFanResource.getHandle());

            if (OC_STACK_NO_OBSERVERS == result)
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
void entityHandler(std::shared_ptr<OCResourceRequest> request,
                   std::shared_ptr<OCResourceResponse> response)
{
    cout << "\tIn Server CPP entity handler:\n";

    if (request)
    {
        // Get the request type and request flag
        std::string requestType = request->getRequestType();
        RequestHandlerFlag requestFlag = request->getRequestHandlerFlag();

        if (requestFlag == RequestHandlerFlag::InitFlag)
        {
            cout << "\t\trequestFlag : Init\n";

            // entity handler to perform resource initialization operations
        }
        else if (requestFlag == RequestHandlerFlag::RequestFlag)
        {
            cout << "\t\trequestFlag : Request\n";

            // If the request type is GET
            if (requestType == "GET")
            {
                cout << "\t\t\trequestType : GET\n";

                // Check for query params (if any)
                QueryParamsMap queryParamsMap = request->getQueryParameters();

                cout << "\t\t\tquery params: \n";
                for (QueryParamsMap::iterator it = queryParamsMap.begin(); it != queryParamsMap.end(); it++)
                {
                    cout << "\t\t\t\t" << it->first << ":" << it->second << endl;
                }

                // Process query params and do required operations ..

                // Get the representation of this resource at this point and send it as response
                //AttributeMap attributeMap;
                OCRepresentation rep;
                rep = myFanResource.getRepresentation();

                if (response)
                {
                    // TODO Error Code
                    response->setErrorCode(200);
                    auto findRes = queryParamsMap.find("if");

                    if (findRes != queryParamsMap.end())
                    {
                        response->setResourceRepresentation(rep, findRes->second);
                    }
                    else
                    {
                        response->setResourceRepresentation(rep, DEFAULT_INTERFACE);
                    }
                }
            }
            else if (requestType == "PUT")
            {
                cout << "\t\t\trequestType : PUT\n";

                // Check for query params (if any)
                QueryParamsMap queryParamsMap = request->getQueryParameters();

                cout << "\t\t\tquery params: \n";
                for (auto it = queryParamsMap.begin(); it != queryParamsMap.end(); it++)
                {
                    cout << "\t\t\t\t" << it->first << ":" << it->second << endl;
                }

                // Get the representation from the request
                OCRepresentation rep = request->getResourceRepresentation();
                AttributeMap attributeMap = rep.getAttributeMap();

                myFanResource.setRepresentation(rep);
                // Do related operations related to PUT request
                rep = myFanResource.getRepresentation();

                int power = std::stoi(attributeMap["power"][0]);

                if (power == 1)
                {
                    mosquitto_publish(myMosquitto, NULL, "actuators/fan", 32, "onfan", 0, true);
                }
                else
                {
                    mosquitto_publish(myMosquitto, NULL, "actuators/fan", 32, "offfan", 0, true);
                }

                if (response)
                {
                    response->setErrorCode(200);
                    auto findRes = queryParamsMap.find("if");

                    if (findRes != queryParamsMap.end())
                    {
                        response->setResourceRepresentation(rep, findRes->second);
                    }
                    else
                    {
                        response->setResourceRepresentation(rep, DEFAULT_INTERFACE);
                    }
                }
            }
            else if (requestType == "POST")
            {
                // POST request operations
            }
            else if (requestType == "DELETE")
            {
                // DELETE request operations
            }
        }
        else if (requestFlag == RequestHandlerFlag::ObserverFlag)
        {
            pthread_t threadId;

            cout << "\t\trequestFlag : Observer\n";
            gObservation = 1;

            static int startedThread = 0;

            // Observation happens on a different thread in ChangeFanRepresentation function.
            // If we have not created the thread already, we will create one here.
            if (!startedThread)
            {
                pthread_create (&threadId, NULL, ChangeFanRepresentation, (void *)NULL);
                startedThread = 1;
            }

        }
    }
    else
    {
        std::cout << "Request invalid" << std::endl;
    }
}

//int start_fanserver(void*)                // 1
void *start_fanserver(void *d)      // 2
{
    /*PlatformConfig cfg;
    cfg.ipAddress = "192.168.2.5";
    cfg.port = 56832;
    cfg.mode = ModeType::Server;
    cfg.serviceType = ServiceType::InProc;*/
    PlatformConfig cfg
    {
        OC::ServiceType::InProc,
        OC::ModeType::Server,
        "192.168.2.5",
        56832,
        OC::QualityOfService::NonConfirmable
    };

    // Create PlatformConfig object

    // Create a OCPlatform instance.
    // Note: Platform creation is synchronous call.
    try
    {
        mosquitto_lib_init();
        myMosquitto = mosquitto_new("MQTT plug-in", true, NULL);
        if (!myMosquitto)
        {
            printf("[mosquitto] Null\n");
            //return 1;                     // 1
        }

        plugin_data_t *data = (plugin_data_t *)d;
        //OCPlatform *platform = (OCPlatform*)data->str;
        OCPlatform platform(cfg);
        // Invoke createResource function of class fan.

        mosquitto_connect(myMosquitto, "192.168.2.5", 1883, 60);
        myFanResource.createResource(platform);
        myFanResource.addType(platform, std::string("core.fan"));
        myFanResource.addInterface(platform, std::string("oc.mi.ll"));
        // Perform app tasks
        while (data->flag)
        {
            // some tasks
        }
    }
    catch (OCException e)
    {
        //log(e.what());
    }

    // No explicit call to stop the platform.
    // When OCPlatform destructor is invoked, internally we do platform cleanup

    gObservation = 0;
    mosquitto_destroy(myMosquitto);

    mosquitto_lib_cleanup();
    printf("start_fanserver finish\n");
    pthread_exit((void *)0);
}

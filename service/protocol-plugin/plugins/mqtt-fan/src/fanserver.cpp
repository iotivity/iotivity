//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
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
#include "fanserver.h"

#include <time.h>


using namespace OC;
using namespace std;
namespace PH = std::placeholders;

time_t timer;                // Define the timer
struct tm *tblock;           // Define a structure for time block


int gObservation = 0;
void *ChangeFanRepresentation (void *param);

// Specifies where to notify all observers or list of observers
// 0 - notifies all observers
// 1 - notifies list of observers
int isListOfObservers = 0;

// Forward declaring the entityHandler

/// This class represents a single resource named 'fanResource'. This resource has
/// two simple properties named 'state' and 'power'



// Forward declaring the entityHandler
// void entityHandler(std::shared_ptr<OCResourceRequest> request,
//                    std::shared_ptr<OCResourceResponse> response);

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
        std::string m_name;
        bool m_state;
        int m_power;
        int m_health;
        std::string m_fanUri;
        OCResourceHandle m_resourceHandle;
        OCRepresentation m_fanRep;
        ObservationIds m_interestedObservers;

    public:
        /// Constructor
        FanResource(): m_name("John's fan"), m_state(false), m_power(0), m_fanUri("/a/fan")
        {
            // Initialize representation
            m_fanRep.setUri(m_fanUri);

            m_fanRep.setValue("state", m_state);
            m_fanRep.setValue("power", m_power);
            m_fanRep.setValue("name", m_name);
        }


        /* Note that this does not need to be a member function: for classes you do not have
        access to, you can accomplish this with a free function: */

        /// This function internally calls registerResource API.
        void createResource()
        {
            std::string resourceURI = m_fanUri; // URI of the resource
            std::string resourceTypeName = "core.fan"; // resource type name. In this case, it is fan
            std::string resourceInterface = DEFAULT_INTERFACE; // resource interface.

            // OCResourceProperty is defined ocstack.h
            uint8_t resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE;

            EntityHandler cb = std::bind(&FanResource::entityHandler, this, PH::_1, PH::_2);

            // This will internally create and register the resource.
            OCStackResult result = OCPlatform::registerResource(
                                       m_resourceHandle, resourceURI, resourceTypeName,
                                       resourceInterface, cb, resourceProperty);

            if (OC_STACK_OK != result)
            {
                cout << "Resource creation was unsuccessful\n";
            }
        }

        OCStackResult createResource1()
        {
            std::string resourceURI = "/a/fan1"; // URI of the resource
            std::string resourceTypeName = "core.fan"; // resource type name. In this case, it is fan
            std::string resourceInterface = DEFAULT_INTERFACE; // resource interface.

            // OCResourceProperty is defined ocstack.h
            uint8_t resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE;

            EntityHandler cb = std::bind(&FanResource::entityHandler, this, PH::_1, PH::_2);

            OCResourceHandle resHandle;

            // This will internally create and register the resource.
            OCStackResult result = OCPlatform::registerResource(
                                       resHandle, resourceURI, resourceTypeName,
                                       resourceInterface, cb, resourceProperty);

            if (OC_STACK_OK != result)
            {
                cout << "Resource creation was unsuccessful\n";
            }

            return result;
        }

        OCResourceHandle getHandle()
        {
            return m_resourceHandle;
        }

        // Puts representation.
        // Gets values from the representation and
        // updates the internal state
        void put(OCRepresentation &rep)
        {
            try
            {
                if (rep.getValue("state", m_state))
                {
                    cout << "\t\t\t\t" << "state: " << m_state << endl;
                }
                else
                {
                    cout << "\t\t\t\t" << "state not found in the representation" << endl;
                }

                if (rep.getValue("power", m_power))
                {
                    cout << "\t\t\t\t" << "power: " << m_power << endl;
                }
                else
                {
                    cout << "\t\t\t\t" << "power not found in the representation" << endl;
                }
            }
            catch (exception &e)
            {
                cout << e.what() << endl;
            }

        }

        // Post representation.
        // Post can create new resource or simply act like put.
        // Gets values from the representation and
        // updates the internal state
        OCRepresentation post(OCRepresentation &rep)
        {
            static int first = 1;

            // for the first time it tries to create a resource
            if (first)
            {
                first = 0;

                if (OC_STACK_OK == createResource1())
                {
                    OCRepresentation rep1;
                    rep1.setValue("createduri", std::string("/a/fan1"));

                    return rep1;
                }
            }

            // from second time onwards it just puts
            put(rep);
            return get();
        }


        // gets the updated representation.
        // Updates the representation with latest internal state before
        // sending out.
        OCRepresentation get()
        {
            m_fanRep.setValue("state", m_state);
            m_fanRep.setValue("power", m_power);

            return m_fanRep;
        }


        void addType(const std::string &type) const
        {
            OCStackResult result = OCPlatform::bindTypeToResource(m_resourceHandle, type);
            if (OC_STACK_OK != result)
            {
                cout << "Binding TypeName to Resource was unsuccessful\n";
            }
        }

        void addInterface(const std::string &interface) const
        {
            OCStackResult result = OCPlatform::bindInterfaceToResource(m_resourceHandle, interface);
            if (OC_STACK_OK != result)
            {
                cout << "Binding TypeName to Resource was unsuccessful\n";
            }
        }

    private:
        // This is just a sample implementation of entity handler.
        // Entity handler can be implemented in several ways by the manufacturer
        OCEntityHandlerResult entityHandler(std::shared_ptr<OCResourceRequest> request,
                                            std::shared_ptr<OCResourceResponse> response)
        {
            OCEntityHandlerResult result = OC_EH_OK;

            cout << "\tIn Server CPP entity handler:\n";

            if (request)
            {
                // Get the request type and request flag
                std::string requestType = request->getRequestType();
                int requestFlag = request->getRequestHandlerFlag();

                if (requestFlag & RequestHandlerFlag::InitFlag)
                {
                    cout << "\t\trequestFlag : Init\n";

                    // entity handler to perform resource initialization operations
                }
                if (requestFlag & RequestHandlerFlag::RequestFlag)
                {
                    cout << "\t\trequestFlag : Request === Handle by FanServer\n";

                    // If the request type is GET
                    if (requestType == "GET")
                    {
                        cout << "\t\t\trequestType : GET\n";

                        if (response)
                        {
                            // TODO Error Code
                            response->setErrorCode(200);

                            response->setResourceRepresentation(get());
                        }
                    }
                    else if (requestType == "PUT")
                    {
                        cout << "\t\t\trequestType : PUT\n";

                        OCRepresentation rep = request->getResourceRepresentation();

                        // Do related operations related to PUT request

                        // Update the fanResource
                        put(rep);

                        if (response)
                        {
                            // TODO Error Code
                            response->setErrorCode(200);

                            response->setResourceRepresentation(get());
                        }

                    }
                    else if (requestType == "POST")
                    {
                        cout << "\t\t\trequestType : POST\n";

                        OCRepresentation rep = request->getResourceRepresentation();

                        // Do related operations related to POST request

                        OCRepresentation rep_post = post(rep);

                        if (response)
                        {
                            // TODO Error Code
                            response->setErrorCode(200);

                            response->setResourceRepresentation(rep_post);

                            if (rep_post.hasAttribute("createduri"))
                            {
                                result = OC_EH_RESOURCE_CREATED;

                                response->setNewResourceUri(rep_post.getValue<std::string>("createduri"));
                            }

                        }

                        // POST request operations
                    }
                    else if (requestType == "DELETE")
                    {
                        // DELETE request operations
                    }
                }

                if (requestFlag & RequestHandlerFlag::ObserverFlag)
                {
                    ObservationInfo observationInfo = request->getObservationInfo();
                    if (ObserveAction::ObserveRegister == observationInfo.action)
                    {
                        m_interestedObservers.push_back(observationInfo.obsId);
                    }
                    else if (ObserveAction::ObserveUnregister == observationInfo.action)
                    {
                        m_interestedObservers.erase(std::remove(
                                                        m_interestedObservers.begin(),
                                                        m_interestedObservers.end(),
                                                        observationInfo.obsId),
                                                    m_interestedObservers.end());
                    }

                    pthread_t threadId;

                    cout << "\t\trequestFlag : Observer\n";
                    gObservation = 1;
                    static int startedThread = 0;

                    // Observation happens on a different thread in ChangeFanRepresentation function.
                    // If we have not created the thread already, we will create one here.
                    if (!startedThread)
                    {
                        pthread_create (&threadId, NULL, ChangeFanRepresentation, (void *)this);
                        startedThread = 1;
                    }
                }
            }
            else
            {
                std::cout << "Request invalid" << std::endl;
            }

            return result;
        }
};

// Create the instance of the resource class (in this case instance of class 'FanResource').
struct mosquitto *myMosquitto;

// ChangeFanRepresentaion is an observation function,
// which notifies any changes to the resource to stack
// via notifyObservers
void *ChangeFanRepresentation (void *param)
{
    FanResource *fanPtr = (FanResource *) param;

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
            fanPtr->m_power += 10;

            cout << "\nPower updated to : " << fanPtr->m_power << endl;
            cout << "Notifying observers with resource handle: " << fanPtr->getHandle() << endl;

            OCStackResult result = OC_STACK_OK;

            if (isListOfObservers)
            {
                std::shared_ptr<OCResourceResponse> resourceResponse(new OCResourceResponse());

                resourceResponse->setErrorCode(200);
                resourceResponse->setResourceRepresentation(fanPtr->get(), DEFAULT_INTERFACE);

                result = OCPlatform::notifyListOfObservers(  fanPtr->getHandle(),
                         fanPtr->m_interestedObservers,
                         resourceResponse);
            }
            else
            {
                result = OCPlatform::notifyAllObservers(fanPtr->getHandle());
            }

            if (OC_STACK_NO_OBSERVERS == result)
            {
                cout << "No More observers, stopping notifications" << endl;
                gObservation = 0;
            }
        }
    }

    return NULL;
}


//int start_fanserver(void*)                // 1
void *start_fanserver(void *d)      // 2
{
    /*PlatformConfig cfg;
    cfg.ipAddress = "192.168.2.5";
    cfg.port = 56832;
    cfg.mode = ModeType::Server;
    cfg.serviceType = ServiceType::InProc;*/
    // PlatformConfig cfg
    // {
    //     OC::ServiceType::InProc,
    //     OC::ModeType::Server,
    //     "192.168.2.5",
    //     56832,
    //     OC::QualityOfService::NonConfirmable
    // };

    // Create PlatformConfig object

    // Create a OCPlatform instance.
    // Note: Platform creation is synchronous call.

    // Create PlatformConfig object
    PlatformConfig cfg
    {
        OC::ServiceType::InProc,
        OC::ModeType::Server,
        "0.0.0.0", // By setting to "0.0.0.0", it binds to all available interfaces
        0,         // Uses randomly available port
        OC::QualityOfService::LowQos
    };

    OCPlatform::Configure(cfg);

    printf("start_fanserver [mosquitto] Null\n");
    try
    {
        FanResource myFanResource;
        mosquitto_lib_init();
        myMosquitto = mosquitto_new("MQTT plug-in", true, NULL);
        if (!myMosquitto)
        {
            printf("[mosquitto] Null\n");
            printf("You need to install mqtt broker\n");
        }
        else
        {
            printf("Mosquitto is working\n");
        }

        //plugin_data_t *data = (plugin_data_t *)d;
        //OCPlatform *platform = (OCPlatform*)data->str;
        //myFanResource.m_platform = (OCPlatform*)data->str;
        //OCPlatform platform(cfg);
        // Invoke createResource function of class fan.

        //mosquitto_connect(myMosquitto, "192.168.2.5", 1883, 60);
        mosquitto_connect(myMosquitto, "127.0.0.1", 1883, 60);
        printf("Mosquitto Connection is done\n");
        myFanResource.createResource();
        //myFanResource.addType(std::string("core.fan"));
        //myFanResource.addInterface(std::string("oc.mi.ll"));
        // Get time of day
        timer = time(NULL);
        // Converts date/time to a structure
        tblock = localtime(&timer);
        // Output ASCII data/time
        printf("FanReousrce reigishter time is: %s", asctime(tblock));
        // Perform app tasks
        while (true)
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
    mosquitto_destroy(myMosquitto);

    mosquitto_lib_cleanup();
    printf("start_fanserver finish\n");
    pthread_exit((void *)0);
}

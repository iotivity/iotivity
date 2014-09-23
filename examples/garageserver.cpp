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
/// This sample provides using varous json types in the representation.
///

#include <functional>

#include <pthread.h>

#include "OCPlatform.h"
#include "OCApi.h"

using namespace OC;
using namespace std;

// Forward declaring the entityHandler
void entityHandler(std::shared_ptr<OCResourceRequest> request,
                   std::shared_ptr<OCResourceResponse> response);

/// This class represents a single resource named 'lightResource'. This resource has

class GarageResource
{
public:
    /// Access this property from a TB client
    std::string m_name;
    bool m_state;
    std::string m_garageUri;
    OCResourceHandle m_resourceHandle;
    OCRepresentation m_garageRep;
    ObservationIds m_interestedObservers;

    // Light representation with in GarageResource
    OCRepresentation m_lightRep;
    bool m_lightState;
    int m_lightPower;

public:
    /// Constructor
    GarageResource(): m_name("John's Garage"), m_state(false), m_garageUri("/a/garage") {
        // Initialize representation
        m_garageRep.setUri(m_garageUri);

        m_garageRep.setValue("state", m_state);
        m_garageRep.setValue("name", m_name);

        m_lightState = true;
        m_lightPower = 10;
        m_lightRep.setValue("state", m_lightState);
        m_lightRep.setValue("power", m_lightPower);

        // Storing another representation within a representation
        m_garageRep.setValue("light", m_lightRep);
    }

    /* Note that this does not need to be a member function: for classes you do not have
    access to, you can accomplish this with a free function: */

    /// This function internally calls registerResource API.
    void createResource(OC::OCPlatform& platform)
    {
        std::string resourceURI = m_garageUri; // URI of the resource
        std::string resourceTypeName = "core.garage"; // resource type name.
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

    // Puts representation.
    // Gets values from the representation and
    // updates the internal state
    void put(OCRepresentation& rep)
    {
        try {
            if (rep.getValue("state", m_state))
            {
                cout << "\t\t\t\t" << "state: " << m_state << endl;
            }
            else
            {
                cout << "\t\t\t\t" << "state not found in the representation" << endl;
            }
        }
        catch (exception& e)
        {
            cout << e.what() << endl;
        }

    }

    // gets the updated representation.
    // Updates the representation with latest internal state before
    // sending out.
    OCRepresentation get()
    {
        m_garageRep.setValue("state", m_state);

        return m_garageRep;
    }

};

// Create the instance of the resource class (in this case instance of class 'GarageResource').
GarageResource myGarage;

void entityHandler(std::shared_ptr<OCResourceRequest> request,
                   std::shared_ptr<OCResourceResponse> response)
{
    cout << "\tIn Server CPP entity handler:\n";

    if(request)
    {
        // Get the request type and request flag
        std::string requestType = request->getRequestType();
        int requestFlag = request->getRequestHandlerFlag();

        if(requestFlag & RequestHandlerFlag::InitFlag)
        {
            cout << "\t\trequestFlag : Init\n";

            // entity handler to perform resource initialization operations
        }
        if(requestFlag & RequestHandlerFlag::RequestFlag)
        {
            cout << "\t\trequestFlag : Request\n";

            // If the request type is GET
            if(requestType == "GET")
            {
                cout << "\t\t\trequestType : GET\n";

                if(response)
                {
                    // TODO Error Code
                    response->setErrorCode(200);

                    response->setResourceRepresentation(myGarage.get());
                }
            }
            else if(requestType == "PUT")
            {
                cout << "\t\t\trequestType : PUT\n";

                OCRepresentation rep = request->getResourceRepresentation();

                // Do related operations related to PUT request

                // Update the lightResource
                myGarage.put(rep);

                if(response)
                {
                    // TODO Error Code
                    response->setErrorCode(200);

                    response->setResourceRepresentation(myGarage.get());
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
        if(requestFlag & RequestHandlerFlag::ObserverFlag)
        {
            // OBSERVE operations
        }
    }
    else
    {
        std::cout << "Request invalid" << std::endl;
    }
}

int main(int argc, char* argv[1])
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

        // Invoke createResource function of class light.
        myGarage.createResource(platform);

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

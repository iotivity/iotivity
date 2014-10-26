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
/// This sample shows how one could create a resource (collection) with children.
///

#include <functional>

#include <pthread.h>

#include "OCPlatform.h"
#include "OCApi.h"

using namespace OC;
using namespace std;


// Forward declaring the entityHandler (room)
OCEntityHandlerResult entityHandlerRoom(std::shared_ptr<OCResourceRequest> request,
                                        std::shared_ptr<OCResourceResponse> response);
OCEntityHandlerResult entityHandlerLight(std::shared_ptr<OCResourceRequest> request,
                                         std::shared_ptr<OCResourceResponse> response);
OCEntityHandlerResult entityHandlerFan(std::shared_ptr<OCResourceRequest> request,
                                       std::shared_ptr<OCResourceResponse> response);

class RoomResource
{
public:

    // Room members
    std::string m_roomUri;
    std::string m_roomName;
    std::vector<std::string> m_roomTypes;
    std::vector<std::string> m_roomInterfaces;
    OCResourceHandle m_roomHandle;
    OCRepresentation m_roomRep;

    // light members
    bool m_lightState;
    int m_lightColor;
    std::string m_lightUri;
    std::vector<std::string> m_lightTypes;
    std::vector<std::string> m_lightInterfaces;
    OCResourceHandle m_lightHandle;
    OCRepresentation m_lightRep;

    // fan members
    bool m_fanState;
    int m_fanSpeed;
    std::string m_fanUri;
    std::vector<std::string> m_fanTypes;
    std::vector<std::string> m_fanInterfaces;
    OCResourceHandle m_fanHandle;
    OCRepresentation m_fanRep;

public:
    /// Constructor
    RoomResource(): m_roomName("John's Room"), m_lightState(false),
                    m_lightColor(0), m_fanState(false), m_fanSpeed(0)
    {
        m_lightUri = "/a/light"; // URI of the resource
        m_lightTypes.push_back("core.light"); // resource type name. In this case, it is light
        m_lightInterfaces.push_back(DEFAULT_INTERFACE); // resource interface.

        m_lightRep.setUri(m_lightUri);
        m_lightRep.setResourceTypes(m_lightTypes);
        m_lightRep.setResourceInterfaces(m_lightInterfaces);
        m_lightRep.setValue("state", m_lightState);
        m_lightRep.setValue("color", m_lightColor);

        m_fanUri = "/a/fan"; // URI of the resource
        m_fanTypes.push_back("core.fan"); // resource type name. In this case, it is light
        m_fanInterfaces.push_back(DEFAULT_INTERFACE); // resource interface.

        m_fanRep.setUri(m_fanUri);
        m_fanRep.setResourceTypes(m_fanTypes);
        m_fanRep.setResourceInterfaces(m_fanInterfaces);
        m_fanRep.setValue("state", m_fanState);
        m_fanRep.setValue("speed", m_fanSpeed);

        m_roomUri = "/a/room"; // URI of the resource
        m_roomTypes.push_back("core.room"); // resource type name. In this case, it is light
        m_roomInterfaces.push_back(DEFAULT_INTERFACE); // resource interface.
        m_roomInterfaces.push_back(BATCH_INTERFACE); // resource interface.
        m_roomInterfaces.push_back(LINK_INTERFACE); // resource interface.
        m_roomRep.setValue("name", m_roomName);
        m_roomRep.setUri(m_roomUri);
        m_roomRep.setResourceTypes(m_roomTypes);
        m_roomRep.setResourceInterfaces(m_roomInterfaces);
    }

    /// This function internally calls registerResource API.
    void createResources(OC::OCPlatform& platform)
    {
        // This will internally create and register the resource.
        OCStackResult result = platform.registerResource(
                                    m_roomHandle, m_roomUri, m_roomTypes[0],
                                    m_roomInterfaces[0], entityHandlerRoom,
                                    OC_DISCOVERABLE | OC_OBSERVABLE
                                  );

        if (OC_STACK_OK != result)
        {
            cout << "Resource creation (room) was unsuccessful\n";
        }

        result = platform.bindInterfaceToResource(m_roomHandle, m_roomInterfaces[1]);
        if (OC_STACK_OK != result)
        {
            cout << "Binding TypeName to Resource was unsuccessful\n";
        }

        result = platform.bindInterfaceToResource(m_roomHandle, m_roomInterfaces[2]);
        if (OC_STACK_OK != result)
        {
            cout << "Binding TypeName to Resource was unsuccessful\n";
        }

        result = platform.registerResource(
                                    m_lightHandle, m_lightUri, m_lightTypes[0],
                                    m_lightInterfaces[0], entityHandlerLight,
                                    OC_DISCOVERABLE | OC_OBSERVABLE
                                   );

        if (OC_STACK_OK != result)
        {
            cout << "Resource creation (light) was unsuccessful\n";
        }

        result = platform.registerResource(
                                    m_fanHandle, m_fanUri, m_fanTypes[0],
                                    m_fanInterfaces[0], entityHandlerFan,
                                    OC_DISCOVERABLE | OC_OBSERVABLE
                                   );

        if (OC_STACK_OK != result)
        {
            cout << "Resource creation (fan) was unsuccessful\n";
        }

        result = platform.bindResource(m_roomHandle, m_lightHandle);
        if (OC_STACK_OK != result)
        {
            cout << "Binding fan resource to room was unsuccessful\n";
        }

        result = platform.bindResource(m_roomHandle, m_fanHandle);
        if (OC_STACK_OK != result)
        {
            cout << "Binding light resource to room was unsuccessful\n";
        }

    }

    void setLightRepresentation(OCRepresentation& rep)
    {
        bool tempState = false;
        int tempColor = 0;

        // If both entries exist
        if(rep.getValue("state", tempState) && rep.getValue("color", tempColor))
        {
            m_lightState = tempState;
            m_lightColor= tempColor;

            cout << "\t\t\t\t" << "state: " << m_lightState << endl;
            cout << "\t\t\t\t" << "color: " << m_lightColor << endl;
        }
    }

    void setFanRepresentation(OCRepresentation& rep)
    {
        bool tempState = false;
        int tempSpeed = 0;

        // If both entries exist
        if(rep.getValue("state", tempState) && rep.getValue("speed", tempSpeed))
        {
            m_fanState = tempState;
            m_fanSpeed = tempSpeed;

            cout << "\t\t\t\t" << "state: " << m_fanState << endl;
            cout << "\t\t\t\t" << "speed: " << m_fanSpeed << endl;
        }
    }


    OCRepresentation getLightRepresentation()
    {
        m_lightRep.setValue("state", m_lightState);
        m_lightRep.setValue("color", m_lightColor);

        return m_lightRep;
    }

    OCRepresentation getFanRepresentation()
    {
        m_fanRep.setValue("state", m_fanState);
        m_fanRep.setValue("speed", m_fanSpeed);
        return m_fanRep;
    }

    OCRepresentation getRoomRepresentation(void)
    {
        std::vector<OCRepresentation> children;

        OCRepresentation light = getLightRepresentation();
        children.push_back(light);

        OCRepresentation fan = getFanRepresentation();
        children.push_back(fan);

        m_roomRep.setChildren(children);

        return m_roomRep;
    }

};

// Create the instance of the resource class (in this case instance of class 'RoomResource').
RoomResource myRoomResource;

OCEntityHandlerResult entityHandlerRoom(std::shared_ptr<OCResourceRequest> request,
                       std::shared_ptr<OCResourceResponse> response)
{
    cout << "\tIn Server CPP entity handler:\n";

    if(request)
    {
        // Get the request type and request flag
        std::string requestType = request->getRequestType();
        int requestFlag = request->getRequestHandlerFlag();

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
                for(auto it = queryParamsMap.begin(); it != queryParamsMap.end(); it++)
                {
                    cout << "\t\t\t\t" << it->first << ":" << it->second << endl;
                }

                OCRepresentation rep;
                rep = myRoomResource.getRoomRepresentation();

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

                QueryParamsMap queryParamsMap = request->getQueryParameters();

                entityHandlerLight(request, response);
                entityHandlerFan(request, response);

                OCRepresentation rep;
                rep = myRoomResource.getRoomRepresentation();

                if(response)
                {
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
            cout << "\t\trequestFlag : Observer\n";
        }
    }
    else
    {
        std::cout << "Request invalid" << std::endl;
    }

    return OC_EH_OK;
}

OCEntityHandlerResult entityHandlerLight(std::shared_ptr<OCResourceRequest> request,
                                         std::shared_ptr<OCResourceResponse> response)
{
    cout << "\tIn Server CPP (Light) entity handler:\n";

    if(request)
    {
        // Get the request type and request flag
        std::string requestType = request->getRequestType();
        int requestFlag = request->getRequestHandlerFlag();

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

                if(response)
                {
                    // TODO Error Code
                    response->setErrorCode(200);
                    response->setResourceRepresentation(myRoomResource.getLightRepresentation());
                }

            }
            else if(requestType == "PUT")
            {
                cout << "\t\t\trequestType : PUT\n";

                OCRepresentation rep = request->getResourceRepresentation();

                // Do related operations related to PUT request
                myRoomResource.setLightRepresentation(rep);

                if(response)
                {
                    // TODO Error Code
                    response->setErrorCode(200);
                    response->setResourceRepresentation(myRoomResource.getLightRepresentation());
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
            cout << "\t\trequestFlag : Observer\n";
        }
    }
    else
    {
        std::cout << "Request invalid" << std::endl;
    }

    return OC_EH_OK;
}

OCEntityHandlerResult entityHandlerFan(std::shared_ptr<OCResourceRequest> request,
                                       std::shared_ptr<OCResourceResponse> response)
{
    cout << "\tIn Server CPP (Fan) entity handler:\n";

    if(request)
    {
        // Get the request type and request flag
        std::string requestType = request->getRequestType();
        int requestFlag = request->getRequestHandlerFlag();

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

                if(response)
                {
                    // TODO Error Code
                    response->setErrorCode(200);

                    response->setResourceRepresentation(myRoomResource.getFanRepresentation());
                }

            }
            else if(requestType == "PUT")
            {
                cout << "\t\t\trequestType : PUT\n";

                OCRepresentation rep = request->getResourceRepresentation();

                // Do related operations related to PUT request
                myRoomResource.setFanRepresentation(rep);

                if(response)
                {
                    // TODO Error Code
                    response->setErrorCode(200);
                    response->setResourceRepresentation(myRoomResource.getFanRepresentation());
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
            cout << "\t\trequestFlag : Observer\n";
        }
    }
    else
    {
        std::cout << "Request invalid" << std::endl;
    }

    return OC_EH_OK;
}

int main()
{
    // Create PlatformConfig object
    PlatformConfig cfg {
        OC::ServiceType::InProc,
        OC::ModeType::Server,
        "0.0.0.0", // By setting to "0.0.0.0", it binds to all available interfaces
        0,         // Uses randomly available port
        OC::QualityOfService::LowQos
    };

    // Create a OCPlatform instance.
    // Note: Platform creation is synchronous call.
    try
    {
        OCPlatform platform(cfg);

        myRoomResource.createResources(platform);

        // Perform app tasks
        while(true)
        {
            // some tasks
        }
    }
    catch(OCException e)
    {
        std::cout << "Exception in main: " << e.what();
    }

    // No explicit call to stop the platform.
    // When OCPlatform destructor is invoked, internally we do platform cleanup
}

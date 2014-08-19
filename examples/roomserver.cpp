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
/// This sample shows how one could create a resource (collection) with children.
///

#include <functional>

#include <pthread.h>

#include "OCPlatform.h"
#include "OCApi.h"

using namespace OC;
using namespace std;


// Forward declaring the entityHandler (room)
void entityHandlerRoom(std::shared_ptr<OCResourceRequest> request, std::shared_ptr<OCResourceResponse> response);
void entityHandlerLight(std::shared_ptr<OCResourceRequest> request, std::shared_ptr<OCResourceResponse> response);
void entityHandlerFan(std::shared_ptr<OCResourceRequest> request, std::shared_ptr<OCResourceResponse> response);

class RoomResource
{
public:

    // Room members
    std::string m_roomUri;
    std::string m_roomType;
    std::string m_roomInterface1;
    std::string m_roomInterface2;
    std::string m_roomInterface3;
    OCResourceHandle m_roomHandle;

    // light members
    bool m_lightState;
    int m_lightColor;
    std::string m_lightInterface;
    std::string m_lightUri;
    std::string m_lightType;
    OCResourceHandle m_lightHandle;

    // fan members
    bool m_fanState;
    int m_fanSpeed;
    std::string m_fanInterface;
    std::string m_fanUri;
    std::string m_fanType;
    OCResourceHandle m_fanHandle;

public:
    /// Constructor
    RoomResource(): m_lightState(false), m_lightColor(0), m_fanState(false), m_fanSpeed(0) 
    {
        m_roomUri = "/a/room"; // URI of the resource
        m_roomType = "core.room"; // resource type name. In this case, it is light
        m_roomInterface1 = DEFAULT_INTERFACE; // resource interface.
        m_roomInterface2 = BATCH_INTERFACE; // resource interface.
        m_roomInterface3 = LINK_INTERFACE; // resource interface.

        m_lightUri = "/a/light"; // URI of the resource
        m_lightType = "core.light"; // resource type name. In this case, it is light
        m_lightInterface = DEFAULT_INTERFACE; // resource interface.

        m_fanUri = "/a/fan"; // URI of the resource
        m_fanType = "core.fan"; // resource type name. In this case, it is light
        m_fanInterface = DEFAULT_INTERFACE; // resource interface.
    }

    /// This function internally calls registerResource API.
    void createResources(OC::OCPlatform& platform)
    {
        // This will internally create and register the resource.
        OCStackResult result = platform.registerResource(
                                    m_roomHandle, m_roomUri, m_roomType,
                                    m_roomInterface1, &entityHandlerRoom, 
                                    OC_DISCOVERABLE | OC_OBSERVABLE
                                  );

        if (OC_STACK_OK != result)
        {
            cout << "Resource creation (room) was unsuccessful\n";
        }

        result = platform.bindInterfaceToResource(m_roomHandle, m_roomInterface2);
        if (OC_STACK_OK != result)
        {
            cout << "Binding TypeName to Resource was unsuccessful\n";
        }

        result = platform.bindInterfaceToResource(m_roomHandle, m_roomInterface3);
        if (OC_STACK_OK != result)
        {
            cout << "Binding TypeName to Resource was unsuccessful\n";
        }

        result = platform.registerResource(
                                    m_lightHandle, m_lightUri, m_lightType,
                                    m_lightInterface, &entityHandlerLight, 
                                    OC_DISCOVERABLE | OC_OBSERVABLE
                                   );

        if (OC_STACK_OK != result)
        {
            cout << "Resource creation (light) was unsuccessful\n";
        }

        result = platform.registerResource(
                                    m_fanHandle, m_fanUri, m_fanType,
                                    m_fanInterface, &entityHandlerFan, 
                                    OC_DISCOVERABLE | OC_OBSERVABLE
                                   );

        if (OC_STACK_OK != result)
        {
            cout << "Resource creation (fan) was unsuccessful\n";
        }

    }

    void setRoomRepresentation(OCRepresentation& rep)
    {
        setLightRepresentation(rep);
        setFanRepresentation(rep);
    }

    void setLightRepresentation(OCRepresentation& light)
    {
        AttributeMap attributeMap = light.getAttributeMap();

        if(attributeMap.find("state") != attributeMap.end() && attributeMap.find("color") != attributeMap.end())
        {
            m_lightState = attributeMap["state"][0].compare("true") == 0;
            m_lightColor= std::stoi(attributeMap["color"][0]);
        }
    }

    void setFanRepresentation(OCRepresentation& fan)
    {
        AttributeMap attributeMap = fan.getAttributeMap();

        if(attributeMap.find("state") != attributeMap.end() && attributeMap.find("speed") != attributeMap.end())
        {
            m_fanState = attributeMap["state"][0].compare("true") == 0;
            m_fanSpeed = std::stoi(attributeMap["speed"][0]);
        }
    }


    OCRepresentation getLightRepresentation() const
    {
        OCRepresentation light;

        light.setUri(m_lightUri);

        std::vector<std::string> interfaces;
        interfaces.push_back(m_lightInterface);

        light.setResourceInterfaces(interfaces);

        std::vector<std::string> types;
        types.push_back(m_lightType);

        light.setResourceTypes(types);

        AttributeMap attributeMap;
        AttributeValues stateVal;
        if(m_lightState)
        {
            stateVal.push_back("true");
        }
        else
        {
            stateVal.push_back("false");
        }

        AttributeValues colorVal;
        colorVal.push_back(to_string(m_lightColor));

        attributeMap["state"] = stateVal;
        attributeMap["color"] = colorVal;

        light.setAttributeMap(attributeMap);

        return light;
    }

    OCRepresentation getFanRepresentation() const
    {
        OCRepresentation fan;
        fan.setUri(m_fanUri);

        std::vector<std::string> interfaces;
        interfaces.push_back(m_fanInterface);

        fan.setResourceInterfaces(interfaces);

        std::vector<std::string> types;
        types.push_back(m_fanType);

        fan.setResourceTypes(types);

        AttributeMap attributeMap;
        AttributeValues stateVal;
        if(m_fanState)
        {
            stateVal.push_back("true");
        }
        else
        {
            stateVal.push_back("false");
        }

        AttributeValues speedVal;
        speedVal.push_back(to_string(m_fanSpeed));

        attributeMap["state"] = stateVal;
        attributeMap["speed"] = speedVal;

        fan.setAttributeMap(attributeMap);
        
        return fan;
    }

    OCRepresentation getRoomRepresentation(void) const
    {
        OCRepresentation room;

        room.setUri(m_roomUri);

        std::vector<std::string> interfaces;
        interfaces.push_back(m_roomInterface1);
        interfaces.push_back(m_roomInterface2);
        interfaces.push_back(m_roomInterface3);

        room.setResourceInterfaces(interfaces);

        std::vector<std::string> types;
        types.push_back(m_roomType);

        room.setResourceTypes(types);

        std::vector<OCRepresentation> children;

        OCRepresentation light = getLightRepresentation();
        children.push_back(light);

        OCRepresentation fan = getFanRepresentation();
        children.push_back(fan);
        room.setChildren(children);

        return room;
    }

};

// Create the instance of the resource class (in this case instance of class 'RoomResource').
RoomResource myRoomResource;

void entityHandlerRoom(std::shared_ptr<OCResourceRequest> request, std::shared_ptr<OCResourceResponse> response)
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

                 // Check for query params (if any)
                QueryParamsMap queryParamsMap = request->getQueryParameters();

                cout << "\t\t\tquery params: \n";
                for(auto it = queryParamsMap.begin(); it != queryParamsMap.end(); it++)
                {
                    cout << "\t\t\t\t" << it->first << ":" << it->second << endl;
                }

                // Get the representation from the request
                OCRepresentation rep = request->getResourceRepresentation();

                myRoomResource.setRoomRepresentation(rep);

                // Do related operations related to PUT request
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
}

void entityHandlerLight(std::shared_ptr<OCResourceRequest> request, std::shared_ptr<OCResourceResponse> response)
{
    cout << "\tIn Server CPP (Light) entity handler:\n";

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
                for(auto it = queryParamsMap.begin(); it != queryParamsMap.end(); it++)
                {
                    cout << "\t\t\t\t" << it->first << ":" << it->second << endl;
                }

                OCRepresentation rep = myRoomResource.getLightRepresentation();

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

                myRoomResource.setLightRepresentation(rep);

                // Do related operations related to PUT request
                rep = myRoomResource.getLightRepresentation();

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
            cout << "\t\trequestFlag : Observer\n";
        }
    }
    else
    {
        std::cout << "Request invalid" << std::endl;
    }
}

void entityHandlerFan(std::shared_ptr<OCResourceRequest> request, std::shared_ptr<OCResourceResponse> response)
{
    cout << "\tIn Server CPP (Fan) entity handler:\n";

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
                for(auto it = queryParamsMap.begin(); it != queryParamsMap.end(); it++)
                {
                    cout << "\t\t\t\t" << it->first << ":" << it->second << endl;
                }

                OCRepresentation rep = myRoomResource.getFanRepresentation();

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

                myRoomResource.setFanRepresentation(rep);

                // Do related operations related to PUT request
                rep = myRoomResource.getFanRepresentation();

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
            cout << "\t\trequestFlag : Observer\n";
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
    cfg.ipAddress = "192.168.1.10";
    cfg.port = 56832;
    cfg.mode = ModeType::Server;
    cfg.serviceType = ServiceType::InProc;

    // Create a OCPlatform instance.
    // Note: Platform creation is synchronous call.
    try
    {
        OCPlatform platform(cfg);

        // Invoke createResource function of class light.

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

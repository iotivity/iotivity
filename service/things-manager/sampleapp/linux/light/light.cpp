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

#include <functional>

#include <pthread.h>

#include "OCPlatform.h"
#include "OCApi.h"

using namespace OC;
using namespace std;


// Forward declaring the entityHandler of bookmark
void entityHandler(std::shared_ptr<OCResourceRequest> request, std::shared_ptr<OCResourceResponse> response);

class LightResource
{
public:
    //light Members of Bookmark
    std::string m_LightUri;
    std::string m_LightType;
    std::string m_LightInterface;
    std::string m_State;
    OCResourceHandle m_LightHandler;

public:
    /// Constructor
    LightResource()
    {
        m_State = "off";

        m_LightUri= "/core/light"; // URI of the resource
        m_LightType = "core.light"; // resource type name. 

        m_LightInterface = DEFAULT_INTERFACE; // resource interface.
        m_LightHandler = 0;
    }

    /// This function internally calls registerResource API.
    void createResources(OC::OCPlatform& platform)
    {
        // This will internally create and register the resource.
        OCStackResult result = platform.registerResource(
                                    m_LightHandler, m_LightUri, m_LightType,
                                    m_LightInterface, entityHandler,
                                    OC_DISCOVERABLE | OC_OBSERVABLE );

        if (OC_STACK_OK != result)
        {
            cout << "Resource creation (light) was unsuccessful\n";
        }
    }

    void setRepresentation(OCRepresentation& rep)
    {
        AttributeMap attributeMap = rep.getAttributeMap();

        if(attributeMap.find("powerstate") != attributeMap.end())
        {
            std::cout << attributeMap["powerstate"][0] << endl;

            if( attributeMap["powerstate"][0] == "on" )
            {
                cout << "\n\n\n\n LED LIGHT ON \n\n\n\n" << endl;

                // do somethings.
            } 
            else 
            {
                cout << "\n\n\n\n LED LIGHT OFF \n\n\n\n" << endl;
            }
        }
    }

    OCRepresentation getRepresentation()
    {
        OCRepresentation rep;

        AttributeMap attributeMap;

        AttributeValues att_powerstate;
        att_powerstate.push_back(m_State);
        attributeMap["powerstate"] = att_powerstate;

        rep.setAttributeMap(attributeMap);

        return rep;
    }
};


// Create the instance ofpowerstate resource class (in this case instance of class 'BookmarkResource').
LightResource    myLightResource;


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

    PlatformConfig cfg(
        OC::ServiceType::InProc,
        OC::ModeType::Server,
        "192.168.20.120",
        0,
        OC::QualityOfService::NonConfirmable
        );

    //cfg.ipAddress = "192.168.20.120";
    //cfg.port = 0;
    //cfg.mode = ModeType::Server;
    //cfg.serviceType = ServiceType::InProc;

    try
    {
        // Create a OCPlatform instance.
        // Note: Platform creation is synchronous call.
        OCPlatform platform(cfg);

        // Invoke createResource function of class light.

        myLightResource.createResources(platform);

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
}

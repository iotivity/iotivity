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

class BookmarkResource
{
public:
    // Members of Bookmark
    std::string m_BookmarkUri;
    std::string m_BookmarkType;
    std::string m_BookmarkInterface;
    unsigned int m_pressure;
    OCResourceHandle m_BookmarkHandle;

public:
    /// Constructor
    BookmarkResource()
    {
        m_pressure = 0;

        m_BookmarkUri = "/core/bookmark"; // URI of the resource
        m_BookmarkType = "core.bookmark"; // resource type name. In this case, it is light

        m_BookmarkInterface = DEFAULT_INTERFACE; // resource interface.
        m_BookmarkHandle = 0;
    }

    /// This function internally calls registerResource API.
    void createResources(OC::OCPlatform& platform)
    {
        // This will internally create and register the resource.
        OCStackResult result = platform.registerResource(
                                    m_BookmarkHandle, m_BookmarkUri, m_BookmarkType,
                                    m_BookmarkInterface, entityHandler,
                                    OC_DISCOVERABLE | OC_OBSERVABLE );

        if (OC_STACK_OK != result)
        {
            cout << "Resource creation (bookmark) was unsuccessful\n";
        }
    }

    void setRepresentation(OCRepresentation& rep)
    {
        AttributeMap attributeMap = rep.getAttributeMap();

        if(attributeMap.find("level") != attributeMap.end())
        {
            std::cout << attributeMap["level"][0] << endl;
        }
    }

    OCRepresentation getRepresentation()
    {
        OCRepresentation rep;

        AttributeMap attributeMap;

        AttributeValues attr_pressure;
        attr_pressure.push_back(to_string(m_pressure));
        attributeMap["level"] = attr_pressure;

        rep.setAttributeMap(attributeMap);

        return rep;
    }
};


// Create the instance of the resource class (in this case instance of class 'BookmarkResource').
BookmarkResource    myBookmarkResource;
unsigned int        startedThread;
unsigned int        gObservation;
pthread_t           threadId;




void * ObserveHandler(void *param) 
{
    while (startedThread) 
    {
        sleep(1);

        cout << "input a integer(0:opened, 5:close) : ";
        cin >> myBookmarkResource.m_pressure;

        if( myBookmarkResource.m_pressure == 0 ||       // When your book opened.
            myBookmarkResource.m_pressure == 5 )        // When your book closed.
        {
            cout << "notifyObservers call!" << endl;
            
            OCStackResult result = OCPlatform::notifyObservers(myBookmarkResource.m_BookmarkHandle);

            if (OC_STACK_NO_OBSERVERS == result) 
            {
                cout << "No More observers, stopping notifications" << endl;
                gObservation = 0;
                startedThread = 0;
            }
        }
    }

    return NULL;
}


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
                for(auto it = queryParamsMap.begin(); it != queryParamsMap.end(); it++)
                {
                    cout << "\t\t\t\t" << it->first << ":" << it->second << endl;
                }

                OCRepresentation rep;
                rep = myBookmarkResource.getRepresentation();

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

                myBookmarkResource.setRepresentation(rep);

                // Do related operations related to PUT request
                rep = myBookmarkResource.getRepresentation();

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

            if (!startedThread) 
            {
                pthread_create(&threadId, NULL, ObserveHandler, (void *) NULL);
                startedThread = 1;
                gObservation = 1;
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

    PlatformConfig cfg(
        OC::ServiceType::InProc,
        OC::ModeType::Server,
        "192.168.20.102",
        0,
        OC::QualityOfService::NonConfirmable
        );
    //cfg.ipAddress = "192.168.20.102";
    //cfg.port = 0;
    //cfg.mode = ModeType::Server;
    //cfg.serviceType = ServiceType::InProc;

    try
    {
        // Create a OCPlatform instance.
        // Note: Platform creation is synchronous call.
        OCPlatform platform(cfg);

        // Invoke createResource function of class light.

        myBookmarkResource.createResources(platform);

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

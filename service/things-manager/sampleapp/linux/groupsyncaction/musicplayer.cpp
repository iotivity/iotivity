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

#include <string>
#include <cstdlib>
#include <pthread.h>
#include "OCPlatform.h"
#include "OCApi.h"
#include "ThingsManager.h"

using namespace OC;

OCEntityHandlerResult mpEntityHandler(const std::shared_ptr< OCResourceRequest > request)
{
    cout << "mpEntityHandler:\n";

    if (request)
    {
        // Get the request type and request flag
        std::string requestType = request->getRequestType();
        int requestFlag = request->getRequestHandlerFlag();
        std::string action;

        if (requestFlag == RequestHandlerFlag::InitFlag)
        {
            cout << "\trequestFlag : Init\n";

            // entity handler to perform resource initialization operations
        }
        else if (requestFlag == RequestHandlerFlag::RequestFlag)
        {
            cout << "\trequestFlag : Request\n";

            OCRepresentation rp = request->getResourceRepresentation();

            // If the request type is GET
            if (requestType == "GET")
            {
                cout << "\t\trequestType : GET\n";
            }
            else if (requestType == "PUT")
            {
                cout << "\t\trequestType : PUT\n";

                action = rp.getValue< std::string >("play");
                cout << "\t\t\tplay : " << action << endl;
            }
            else if (requestType == "POST")
            {
                cout << "\t\trequestType : POST\n";
            }
            else if (requestType == "DELETE")
            {
                cout << "\t\trequestType : DELETE\n";
            }
        }
        else if (requestFlag == RequestHandlerFlag::ObserverFlag)
        {
            cout << "\trequestFlag : Observer\n";
        }
    }
    else
    {
        cout << "Request invalid" << endl;
    }

    return OC_EH_OK;
}

int main(int argc, char* argv[])
{
    // Create PlatformConfig object
    PlatformConfig cfg
    { OC::ServiceType::InProc, OC::ModeType::Both, "0.0.0.0", 0, OC::QualityOfService::LowQos };

    OCPlatform::Configure(cfg);

    int selectedMenu;
    OCStackResult result;
    OCResourceHandle mpResourceHandle = NULL;

    try
    {
        while (true)
        {
            // some operations
            cout << "(1) CREATE MUSIC PLAYER | (2) DELETE MUSIC PLAYER" << endl;

            std::cin >> selectedMenu;

            if (selectedMenu == 1)
            {
                if (mpResourceHandle)
                {
                    cout << "Music player resource is registered already." << endl;
                    continue;
                }

                std::string resourceURi = "/core/musicplayer";
                std::string resourceTypeName = "core.musicplayer";
                std::string resourceInterface = DEFAULT_INTERFACE;

                result = OCPlatform::registerResource(mpResourceHandle, resourceURi,
                        resourceTypeName, resourceInterface, mpEntityHandler,
                        OC_DISCOVERABLE | OC_OBSERVABLE);
                if (OC_STACK_OK == result)
                {
                    cout << "To register music player resource was successful\n";
                }
                else
                {
                    cout << "To register music player resource was unsuccessful\n";
                }
            }
            else if (selectedMenu == 2)
            {
                if (NULL == mpResourceHandle)
                {
                    cout
                            << "Error! No resource to unregister. Register resource first!"
                            << endl;
                    continue;
                }

                result = OCPlatform::unregisterResource(mpResourceHandle);
                if (OC_STACK_OK == result)
                {
                    cout << "To unregister music player resource was successful\n";
                }
                else
                {
                    cout << "To unregister music player resource was unsuccessful\n";
                }

                mpResourceHandle = NULL;
            }
        }
    }
    catch (OCException& e)
    {
        //log(e.what());
    }

    return 0;
}


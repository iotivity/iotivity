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

#include "OCPlatform.h"
#include "OCApi.h"

#include <functional>
#include <pthread.h>
#include <iostream>

using namespace std;
using namespace OC;

namespace PH = std::placeholders;

OCResourceHandle resourceHandle;
std::vector< OCResourceHandle > resourceHandleVector;

void foundResource(std::shared_ptr< OCResource > resource)
{

    std::string resourceURI;
    std::string hostAddress;

    try
    {
        cout << "FOUND RESOURCE" << endl;

        if (resource)
        {
            resourceURI = resource->uri();
            hostAddress = resource->host();

            cout << "\tResource URI : " << resourceURI << endl;
            cout << "\tResource Host : " << hostAddress << endl;
            cout << "\tResource Interfaces : " << resource->getResourceInterfaces().front() << endl;
            cout << "\tResource Type : " << resource->getResourceTypes().front() << endl;
            if (resourceURI == "/a/light" || resourceURI == "/a/fan")
            {
                OCResourceHandle foundResourceHandle;
                OCStackResult result = OCPlatform::registerResource(foundResourceHandle, resource);
                cout << "\tresource registed!" << endl;
                if (result == OC_STACK_OK)
                {
                    OCPlatform::bindResource(resourceHandle, foundResourceHandle);
                    resourceHandleVector.push_back(foundResourceHandle);
                }
                else
                {
                    cout << "\tresource Error!" << endl;
                }
            }

            // p_platform.bindResource(resourceHandle, foundResourceHandle);

        }
    }
    catch (std::exception& e)
    {
        std::cout << "" << std::endl;
    }

}

int main()
{
    PlatformConfig config
    { OC::ServiceType::InProc, ModeType::Both, "0.0.0.0", 0, OC::QualityOfService::LowQos };

    try
    {
        string resourceURI = "/core/a/collection";
        string resourceTypeName = "a.collection";
        string resourceInterface = BATCH_INTERFACE;
        OCPlatform::Configure(config);

        // EntityHandler cb = std::bind(, PH::_1, PH::_2);

        OCPlatform::registerResource(resourceHandle, resourceURI, resourceTypeName,
                resourceInterface,
                NULL,
                //&entityHandler, // entityHandler
                OC_DISCOVERABLE);

        cout << "registerResource is called." << endl;

        OCPlatform::findResource("", "coap://224.0.1.187/oc/core?rt=core.light", &foundResource);
        OCPlatform::bindInterfaceToResource(resourceHandle, GROUP_INTERFACE);
        OCPlatform::bindInterfaceToResource(resourceHandle, DEFAULT_INTERFACE);

        int selectedMenu;
        while (true)
        {
            std::cin >> selectedMenu;

            if (selectedMenu == 1)
            {
                for (unsigned int i = 0; i < resourceHandleVector.size(); ++i)
                {
                    OCPlatform::unregisterResource(resourceHandleVector.at(i));
                }
            }

        }
    }
    catch (OCException& e)
    {

    }

    return 0;
}

/******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

///
/// This test app is for Backward Compatibilty and Interoperability test
///
#include <condition_variable>
#include <iostream>
#include <vector>
#include <pthread.h>
#include <mutex>

#include "OCPlatform.h"
#include "RCSDiscoveryManager.h"
#include "RCSRemoteResourceObject.h"
#include "RCSResourceAttributes.h"
#include "RCSResourceObject.h"
#include "RCSAddress.h"
#include "SceneList.h"
#include "SampleResource.h"
#include "SMLocalOperations.h"
#include "SMRemoteOperations.h"

#define LIGHT_URI "/a/light"
#define LIGHT_RESOURCE_TYPE "core.light"
#define FAN_URI "/a/fan"
#define FAN_RESOURCE_TYPE "core.fan"

using namespace OC;
using namespace OIC::Service;
using namespace std;
namespace PH = std::placeholders;

int g_steps = 0;
SampleResource* g_lightResource = nullptr;
SampleResource* g_fanResource = nullptr;

SMLocalOperations* g_pSMLocal = SMLocalOperations::getInstance();
SMRemoteOperations* g_pSMRemote = SMRemoteOperations::getInstance();
bool g_checkSingleResource = false;
bool g_checkTotalResource = false;

void createLightResource()
{
    cout << "Creating Light Resource" << endl;
    OCStackResult result = OC_STACK_ERROR;
    uint8_t resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE;
    g_lightResource = new SampleResource();
    g_lightResource->setResourceProperties(LIGHT_URI,
    LIGHT_RESOURCE_TYPE, DEFAULT_INTERFACE);
    OCRepresentation lightRep;
    lightRep.setValue(KEY_POWER, EMPTY_STRING);
    g_lightResource->setResourceRepresentation(lightRep);
    result = g_lightResource->startResource(resourceProperty);

    if (result == OC_STACK_OK)
    {
        cout << "Light Resource created successfully" << endl << endl;
        g_checkSingleResource = true;
    }
    else
    {
        cout << "Unable to create Light Resource" << endl << endl;
    }
}

void createFanResource()
{
    cout << "Creating Fan Resource" << endl;
    OCStackResult result = OC_STACK_ERROR;
    uint8_t resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE;
    g_fanResource = new SampleResource();
    g_fanResource->setResourceProperties(FAN_URI,
    FAN_RESOURCE_TYPE, DEFAULT_INTERFACE);
    OCRepresentation fanRep;
    fanRep.setValue(KEY_SPEED, INT_ZERO);
    g_fanResource->setResourceRepresentation(fanRep);
    result = g_fanResource->startResource(resourceProperty);

    if (result == OC_STACK_OK)
    {
        cout << "Fan Resource created successfully" << endl << endl;
        g_checkSingleResource = true;
    }
    else
    {
        cout << "Unable to create Fan Resource" << endl;
    }
}

void createResources()
{
    if (g_checkTotalResource)
    {
        cout << "Resources were created already" << endl;
        return;
    }
    cout << "Creating Resources" << endl << endl;
    createLightResource();
    if (!g_checkSingleResource)
    {
        return;
    }
    g_checkSingleResource = false;
    createFanResource();
    if (g_checkSingleResource)
    {
        g_checkTotalResource = true;
        cout << "Resources were created successfully" << endl;
    }
}

void stopResources()
{
    cout << "Stopping Resources" << endl;
    if (g_lightResource == NULL_PTR && g_fanResource == NULL_PTR)
    {
        cout << "Resources can not be stopped. First create Resources" << endl;
        return;
    }
    g_lightResource->stopResource();
    g_fanResource->stopResource();
    g_checkTotalResource = false;
    g_lightResource = nullptr;
    g_fanResource = nullptr;
    cout << "Resources stopped successfully" << endl;
}

void configurePlatform()
{
    PlatformConfig config
    { OC::ServiceType::InProc, ModeType::Both, "0.0.0.0", 0, OC::QualityOfService::LowQos };
    OCPlatform::Configure(config);
}

void printMenu()
{
    cout << endl;
    cout << "========================================================" << endl;
    cout << "### Resource Server Options:" << endl;
    cout << "1. Create Resources" << endl;
    cout << "2. Stop Resources" << endl << endl;
    cout << "### Local Operations:" << endl;
    cout << "3. Create Local Scene" << endl;
    cout << "4. Execute Local Scene" << endl << endl;
    cout << "### Remote Operations:" << endl;
    cout << "5. Execute Created Local Scene From Remote End" << endl;
    cout << "6. Create Remote Scene" << endl;
    cout << "7. Execute Remote Scene" << endl << endl;
    cout << "0. Exit" << endl;
    cout << "========================================================" << endl;
    cout << endl;
}

int main()
{
    string str_steps;
    configurePlatform();
    while (true)
    {
        printMenu();
        try
        {
            getline(cin, str_steps);

            if (str_steps == "")
            {
                continue;
            }
            else
            {
                g_steps = stoi(str_steps);
            }
        }
        catch (invalid_argument&)
        {
            cout << "Please put a digit, not string" << endl;
            continue;
        }
        catch (out_of_range&)
        {
            cout << "Please put a number within the supported range" << std::endl;
            continue;
        }

        switch (g_steps)
        {
            case 0:
                return 0;
                break;

            case 1:
                createResources();
                break;

            case 2:
                stopResources();
                break;

            case 3:
                g_pSMLocal->createLocalScene();
                break;

            case 4:
                g_pSMLocal->executeLocalScene();
                break;

            case 5:
                g_pSMRemote->executeCreatedLocalSceneFromRemoteEnd();
                break;

            case 6:
                g_pSMRemote->createRemoteScene();
                break;

            case 7:
                g_pSMRemote->executeRemoteScene();
                break;

            default:
                cout << "You have entered invalid menu" << endl;
        }
    }
}

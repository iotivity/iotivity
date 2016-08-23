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

#include "RCClient.h"

using namespace OC;
using namespace std;
using namespace OIC::Service;

std::mutex RCClient::s_mutexRcClient;
RCClient* RCClient::s_RCClient = NULL;

RCClient::RCClient()
{
    PlatformConfig config
    { OC::ServiceType::InProc, ModeType::Both, "0.0.0.0", 0, OC::QualityOfService::LowQos };
    OCPlatform::Configure(config);
    resourceTypes =
    {   "oic.r.light", "core.fan"};
    relativetUri = OC_RSRVD_WELL_KNOWN_URI;
}

RCClient* RCClient::getInstance(void)
{
    if (s_RCClient == NULL)
    {
        s_mutexRcClient.lock();

        s_RCClient = new RCClient();

        s_mutexRcClient.unlock();
    }

    return s_RCClient;
}

RCClient::~RCClient()
{
    if (s_RCClient == NULL)
    {
        delete s_RCClient;
        s_RCClient = NULL;
    }
}

void RCClient::rcObserveResourceContainer(RCSResourceContainer *container,
        std::vector< std::string > vecResourceTypes)
{
    OCStackResult actualResult = OC_STACK_ERROR;
    cout << "\nFinding resources.Please wait..." << endl;
    bool sendRequest = true;

    RCFoundResourcesListener::g_foundResource = false;

    try
    {
        if (sendRequest)
        {
            sendRequest = false;

            for (int index = 0; index < vecResourceTypes.size(); index++)
            {
                cout << "Searching for... : " << vecResourceTypes[index] << endl;
            }

            cout << endl;

            //Note: here type OC_RSRVD_WELL_KNOWN_URI will search for all kind of resources.
            actualResult = OCPlatform::findResource("", OC_RSRVD_WELL_KNOWN_URI, CT_DEFAULT,
                    RCFoundResourcesListener::rcFoundResourceCB);
            IBtestappCommonUtil::waitForServerResponse();

            if (actualResult == OC_STACK_OK && RCFoundResourcesListener::g_foundResource)
            {
                cout << "Found resource operation has been completed successfully..." << endl;
            }
        }

        if (!RCFoundResourcesListener::g_foundResource)
            cout << "Sorry.No resource has been found..." << endl;
    }
    catch (OC::OCException &ex)
    {
        sendRequest = true;
        cout << "OcException is occurred: " << ex.reason().c_str() << endl;
    }
}

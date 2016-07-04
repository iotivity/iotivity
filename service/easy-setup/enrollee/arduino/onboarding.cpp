//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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

/**
 * @file
 *
 * This file has methods for on-boarding the Enrollee device.
 * This will contain template methods that will have core business logic & negotiation algorithm
 * on various on-boarding methods.
 */

#include "easysetup.h"
#include "softap.h"
#include "onboarding.h"

#include "logger.h"
#include "resourcehandler.h"

/**
 * @var ES_ENROLLEE_TAG
 * @brief Logging tag for module name.
 */
#define ES_ENROLLEE_TAG "ES_SOFTAP"

/**
 * It will return true on-boarding type is soft AP
 */
bool ESSoftapOnboarding()
{
    // TODO: To be changed based on user config. Current version, hardcoded to SoftAp onboarding
    return true;
}

/**
 * It will return true on-boarding type is BLE
 */
bool ESBleOnboarding()
{
    //BLE onboarding is not supported with current version.
    return false;
}

/**
 * It will do onboarding based on the user's configuration.
 */
bool ESOnboard(const char * ssid, const char* passwd, ESEnrolleeNetworkEventCallback cb)
{
    OIC_LOG(DEBUG, ES_ENROLLEE_TAG, "ESOnboard IN");

    if (ESSoftapOnboarding())
    {
        OIC_LOG(DEBUG, ES_ENROLLEE_TAG, "Connecting to Mediator SoftAp");
        ConnectToWiFiNetwork(ssid, passwd, cb);
        return true;
    }
    else if (ESBleOnboarding())
    {
        OIC_LOG(ERROR, ES_ENROLLEE_TAG, "ESOnboard::Ble onboarding is not supported");
        // TODO:
        return false;
    }
    else
    {
        // TODO:
        OIC_LOG(ERROR, ES_ENROLLEE_TAG, "ESOnboard::Invalid onboarding option");
        return false;
    }
}


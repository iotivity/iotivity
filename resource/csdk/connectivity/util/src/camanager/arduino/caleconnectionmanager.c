/* ****************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#include "camanagerleinterface.h"
#include "cacommon.h"
#include "logger.h"

#define TAG "OIC_CA_MANAGER_AR_LE"

static CAAdapterStateChangedCB g_adapterStateCB = NULL;
static CAConnectionStateChangedCB g_connStateCB = NULL;

void CASetLENetworkMonitorCallbacks(CAAdapterStateChangedCB adapterStateCB,
                                    CAConnectionStateChangedCB connStateCB)
{
    OIC_LOG(DEBUG, TAG, "CASetLENetworkMonitorCallbacks");

    g_adapterStateCB = adapterStateCB;
    g_connStateCB = connStateCB;
}

CAResult_t CASetLEClientAutoConnectionDeviceInfo(const char * address)
{
    OIC_LOG(DEBUG, TAG, "CASetClientAutoConnectionDeviceInfo");
    (void)address;
    return CA_NOT_SUPPORTED;
}

CAResult_t CAUnsetLEClientAutoConnectionDeviceInfo(const char * address)
{
    OIC_LOG(DEBUG, TAG, "CAUnsetClientAutoConnectionDeviceInfo");
    (void)address;
    return CA_NOT_SUPPORTED;
}

void CAStartServerLEAdvertising()
{
    OIC_LOG(DEBUG, TAG, "CAStartServerLEAdvertising");
}

void CAStopServerLEAdvertising()
{
    OIC_LOG(DEBUG, TAG, "CAStopServerLEAdvertising");
}

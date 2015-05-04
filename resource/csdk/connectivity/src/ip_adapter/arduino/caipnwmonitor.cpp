/******************************************************************
*
* Copyright 2014 Samsung Electronics All Rights Reserved.
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
/**
 * @file caipnwmonitor.cpp
 * @brief This file is to keep design in sync with other platforms.  Right now there is no
 *        api for network monitioring in arduino.
 */

#include "caipinterface_singlethread.h"

#define TAG "IPNW"

CAResult_t CAIPInitializeNetworkMonitor(void)
{
    return CA_STATUS_OK;
}

CAResult_t CAIPStartNetworkMonitor(void)
{
    return CA_STATUS_OK;
}

CAResult_t CAIPGetInterfaceInfo(char **ipAddress, char **interfaceName)
{
    return CA_STATUS_OK;
}

bool CAIPIsConnected(void)
{
    return true;
}

void CAIPSetConnectionStateChangeCallback(CAIPConnectionStateChangeCallback callback)
{
    return;
}

CAResult_t CAIPStopNetworkMonitor(void)
{
    return CA_STATUS_OK;
}

void CAIPTerminateNetworkMonitor(void)
{
    return;
}

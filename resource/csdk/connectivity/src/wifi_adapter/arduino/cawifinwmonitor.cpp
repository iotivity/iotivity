/* ****************************************************************
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
 * @file
 *
 * This file is to keep design in sync with other platforms.  Right now
 * there is no api for network monitioring in arduino.
 */

#include "CaWiFiInterface_singlethread.h"

#define WIFI_MONITOR_TAG "WIFI_MONITOR"

CAResult_t CAWiFiInitializeNetworkMonitor(void)
{
    return CA_STATUS_OK;
}

CAResult_t CAWiFiStartNetworkMonitor(void)
{
    return CA_STATUS_OK;
}

CAResult_t CAWiFiGetInterfaceInfo(char **interfaceName, char **ipAddress)
{
    return CA_STATUS_OK;
}

bool CAWiFiIsConnected(void)
{
    return true;
}

void CAWiFiSetConnectionStateChangeCallback(CAWiFiConnectionStateChangeCallback callback)
{
    return;
}

CAResult_t CAWiFiStopNetworkMonitor(void)
{
    return CA_STATUS_OK;
}

void CAWiFiTerminateNetworkMonitor(void)
{
    return;
}

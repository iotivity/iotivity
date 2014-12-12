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

#include "caethernetinterface.h"

#include <Arduino.h>
#include <Ethernet.h>
#include <socket.h>
#include <w5100.h>
#include <EthernetUdp.h>
#include <IPAddress.h>
#include "logger.h"
#include "caadapterutils.h"
#include "caethernetadapter.h"

#define ETHERNET_MONITOR_TAG "ETHERNET_MONITOR"

CAResult_t CAEthernetInitializeNetworkMonitor(void)
{
    return CA_STATUS_OK;
}

CAResult_t CAEthernetStartNetworkMonitor(void)
{
    return CA_STATUS_OK;
}

CAResult_t CAEthernetGetInterfaceInfo(char **interfaceName, char **ipAddress)
{
    return CA_STATUS_OK;
}

bool CAEthernetIsConnected(void)
{
    return CA_TRUE;
}

void CAEthernetSetConnectionStateChangeCallback(CAEthernetConnectionStateChangeCallback callback)
{
    return;
}

CAResult_t CAEthernetStopNetworkMonitor(void)
{
    return CA_STATUS_OK;
}

void CAEthernetTerminateNetworkMonitor(void)
{
    return;
}
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

#include "caipinterface.h"

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <SPI.h>
#include <utility/server_drv.h>
#include <utility/wifi_drv.h>
#include <IPAddress.h>

#include "logger.h"
#include "cacommon.h"
#include "caipadapter.h"
#include "caadapterutils.h"
#include "oic_malloc.h"

#define TAG "IPNW"

CAResult_t CAIPInitializeNetworkMonitor(const ca_thread_pool_t threadPool)
{
    return CA_STATUS_OK;
}

CAResult_t CAIPStartNetworkMonitor(void)
{
    return CA_STATUS_OK;
}

void CAIPGetSubnetMask(char *subnetMaskAddr, int32_t addrLen)
{
    OIC_LOG(DEBUG, TAG, "IN");
    if (WiFi.status() != WL_CONNECTED)
    {
        OIC_LOG(DEBUG, TAG, "No WIFI");
        return;
    }

    VERIFY_NON_NULL_VOID(subnetMaskAddr, TAG, "Invalid Input");
    if (addrLen < CA_IPADDR_SIZE)
    {
        OIC_LOG_V(ERROR, TAG, "AddrLen MUST be atleast %d", CA_IPADDR_SIZE);
        return;
    }

    IPAddress ip = WiFi.subnetMask();
    snprintf((char *)subnetMaskAddr, addrLen, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

    OIC_LOG_V(DEBUG, TAG, "Wifi shield subnet mask is: %s", subnetMaskAddr);
    OIC_LOG(DEBUG, TAG, "OUT");
    return;
}

/// Retrieves the IP address assigned to Arduino WiFi shield
void CAArduinoGetInterfaceAddress(char *address, int32_t addrLen)
{
    OIC_LOG(DEBUG, TAG, "IN");
    if (WiFi.status() != WL_CONNECTED)
    {
        OIC_LOG(DEBUG, TAG, "No WIFI");
        return;
    }

    VERIFY_NON_NULL_VOID(address, TAG, "Invalid address");
    if (addrLen < CA_IPADDR_SIZE)
    {
        OIC_LOG_V(ERROR, TAG, "AddrLen MUST be atleast %d", CA_IPADDR_SIZE);
        return;
    }

    IPAddress ip = WiFi.localIP();
    snprintf((char *)address, addrLen, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

    OIC_LOG_V(DEBUG, TAG, "Wifi shield address is: %s", address);
    OIC_LOG(DEBUG, TAG, "OUT");
    return;
}

CAResult_t CAIPGetInterfaceInfo(u_arraylist_t **netInterfaceList)
{
    CANetInfo_t *netInfo = (CANetInfo_t *)OICCalloc(1, sizeof(CANetInfo_t));
    if (!netInfo)
    {
        OIC_LOG(ERROR, TAG, "Malloc failed");
        return CA_STATUS_FAILED;
    }

    CAArduinoGetInterfaceAddress(netInfo->ipAddress, CA_IPADDR_SIZE);

    CAIPGetSubnetMask(netInfo->subnetMask, CA_IPADDR_SIZE);

    // set interface name
    strncpy(netInfo->interfaceName, "WIFI", strlen(netInfo->interfaceName));

    CAResult_t result = u_arraylist_add(*netInterfaceList, (void *)netInfo);
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, TAG, "u_arraylist_add failed");
        return result;
    }
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

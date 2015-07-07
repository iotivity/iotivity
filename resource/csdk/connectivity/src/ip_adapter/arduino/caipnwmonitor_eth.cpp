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
#include <Ethernet.h>
#include <socket.h>
#include <w5100.h>
#include <EthernetUdp.h>
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

void CAIPGetSubnetMask(char *subnetMask, int32_t addrLen)
{
    OIC_LOG(DEBUG, TAG, "IN");
    VERIFY_NON_NULL_VOID(subnetMask, TAG, "subnetMask");

    //TODO : Fix this for scenarios when this API is invoked when device is not connected
    uint8_t rawIPAddr[4];
    if (addrLen < CA_IPADDR_SIZE)
    {
        OIC_LOG(ERROR, TAG, "Invalid addrLen");
        return;
    }

    W5100.getSubnetMask(rawIPAddr);
    snprintf(subnetMask, addrLen, "%d.%d.%d.%d", rawIPAddr[0], rawIPAddr[1], rawIPAddr[2],
             rawIPAddr[3]);

    OIC_LOG_V(DEBUG, TAG, "subnetMask:%s", subnetMask);
    OIC_LOG(DEBUG, TAG, "OUT");
    return;
}

/// Retrieves the IP address assigned to Arduino Ethernet shield
void CAArduinoGetInterfaceAddress(char *address, int32_t addrLen)
{
    OIC_LOG(DEBUG, TAG, "IN");
    VERIFY_NON_NULL_VOID(address, TAG, "address");

    //TODO : Fix this for scenarios when this API is invoked when device is not connected
    uint8_t rawIPAddr[4];
    if (addrLen < CA_IPADDR_SIZE)
    {
        OIC_LOG(ERROR, TAG, "Invalid addrLen");
        return;
    }

    W5100.getIPAddress(rawIPAddr);
    snprintf(address, addrLen, "%d.%d.%d.%d", rawIPAddr[0], rawIPAddr[1], rawIPAddr[2],
             rawIPAddr[3]);

    OIC_LOG_V(DEBUG, TAG, "address:%s", address);
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
    strncpy(netInfo->interfaceName, "ETH", strlen(netInfo->interfaceName));

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

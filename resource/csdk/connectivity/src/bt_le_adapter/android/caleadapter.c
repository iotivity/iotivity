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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jni.h"
#include "caleadapter.h"
#include "calecore.h"
#include "logger.h"

#define TAG PCF("CA")

// received packet callback
static CANetworkPacketReceivedCallback gLEReceivedCallback = NULL;

static void CALEPacketReceiveCallback(const char* address, const char* data)
{
    OIC_LOG_V(DEBUG, TAG,
            "CALEPacketReceiveCallback, from: %s, data: %s", address, data);

    // call the callback
    if (gLEReceivedCallback != NULL)
    {
        CARemoteEndpoint_t* endpoint = NULL;
        endpoint = (CARemoteEndpoint_t*) OICMalloc(sizeof(CARemoteEndpoint_t));

        if (endpoint == NULL)
        {
            OIC_LOG(DEBUG, TAG, "CALEPacketReceiveCallback, Memory allocation failed !");
            OICFree(address);
            return;
        }

        // set address
        memset((void*) endpoint->addressInfo.BT.btMacAddress, 0, CA_MACADDR_SIZE);
        if (CA_MACADDR_SIZE > strlen(address))
        {
            strcpy((char*) endpoint->addressInfo.BT.btMacAddress, address);
        }
        OICFree(address);

        // set connectivity type
        endpoint->connectivityType = CA_LE;

        gLEReceivedCallback(endpoint, (void *) data, strlen(data));
    }
}

CAResult_t CAInitializeLE(CARegisterConnectivityCallback registerCallback,
        CANetworkPacketReceivedCallback reqRespCallback, CANetworkChangeCallback netCallback,
        u_thread_pool_t handle)
{
    OIC_LOG(DEBUG, TAG, "IntializeBLE");

    gLEReceivedCallback = reqRespCallback;

    // register handlers
    CAConnectivityHandler_t handler;
    memset(&handler, 0, sizeof(CAConnectivityHandler_t));

    handler.startAdapter = CAStartLE;
    handler.startListenServer = CAStartLEListeningServer;
    handler.startDiscoverServer = CAStartLEDiscoveryServer;
    handler.sendData = CASendLEUnicastData;
    handler.sendDataToAll = CASendLEMulticastData;
    handler.GetnetInfo = CAGetLEInterfaceInformation;
    handler.readData = CAReadLEData;
    handler.stopAdapter = CAStopLE;
    handler.terminate = CATerminateLE;
    registerCallback(handler, CA_LE);

    CALEInitialize(handle);
    CALESetCallback(CALEPacketReceiveCallback);

    return CA_STATUS_OK;
}

CAResult_t CAStartLE()
{
    OIC_LOG_V(DEBUG, TAG, "CAStartLE");
    CANativeLEStartScan();

    return CA_STATUS_OK;
}

CAResult_t CAStartLEListeningServer()
{
    OIC_LOG_V(DEBUG, TAG, "CAStartLEListeningServer");
    CALEStartMulticastServer();
//    CALESendMulticastMessage("1111", 4);

    return CA_STATUS_OK;
}

CAResult_t CAStartLEDiscoveryServer()
{
    OIC_LOG_V(DEBUG, TAG, "CAStartLEDiscoveryServer");
    CALEStartMulticastServer();

    return CA_STATUS_OK;
}

uint32_t CASendLEUnicastData(const CARemoteEndpoint_t* endpoint, void* data, uint32_t dataLen)
{
    OIC_LOG_V(DEBUG, TAG, "CASendLEUnicastData");
//    "E1:70:56:CF:E3:67"
    CALESendUnicastMessage(endpoint->addressInfo.BT.btMacAddress, data, dataLen);

    return 0;
}

uint32_t CASendLEMulticastData(void* data, uint32_t dataLen)
{
    OIC_LOG_V(DEBUG, TAG, "CASendLEMulticastData");
    CALESendMulticastMessage(data, dataLen);

    return 0;
}

CAResult_t CAStartLENotifyServer()
{
    OIC_LOG_V(DEBUG, TAG, "CAStartLENotifyServer");

    return CA_STATUS_OK;
}

uint32_t CASendLENotification(const CARemoteEndpoint_t* endpoint, void* data, uint32_t dataLen)
{
    OIC_LOG_V(DEBUG, TAG, "CASendLENotification");
    CALESendUnicastMessage(endpoint->addressInfo.BT.btMacAddress, data, dataLen);

    return 0;
}

CAResult_t CAGetLEInterfaceInformation(CALocalConnectivity_t** info, uint32_t* size)
{
    OIC_LOG_V(DEBUG, TAG, "CAGetLEInterfaceInformation");
    CALEGetInterfaceInfo(info, size);

    return CA_STATUS_OK;
}

CAResult_t CAReadLEData()
{
    OIC_LOG_V(DEBUG, TAG, "Read LE Data");

    return CA_STATUS_OK;
}

CAResult_t CAStopLE()
{
    OIC_LOG_V(DEBUG, TAG, "CAStopLE");
    CANativeLEStopScan();

    return CA_STATUS_OK;
}

void CATerminateLE()
{
    OIC_LOG_V(DEBUG, TAG, "TerminatLE");
    CALETerminate();
}

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
#include "caleserver.h"
#include "calenwmonitor.h"
#include "logger.h"
#include "caadapterutils.h"
#include "oic_malloc.h"

#define TAG PCF("CA_LE_ADAPTER")

// GATT server type
static jboolean g_isBluetoothGattServer;

// received packet callback
static CANetworkPacketReceivedCallback gLEReceivedCallback = NULL;
static CANetworkChangeCallback gLENetworkChangeCallback = NULL;

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
            return;
        }

        // set address
        memset((void*) endpoint->addressInfo.BT.btMacAddress, 0, CA_MACADDR_SIZE);
        if (CA_MACADDR_SIZE > strlen(address))
        {
            strcpy((char*) endpoint->addressInfo.BT.btMacAddress, address);
        }

        // set connectivity type
        endpoint->connectivityType = CA_LE;

        gLEReceivedCallback(endpoint, (void *) data, strlen(data));
    }
}

static void CALENetStateChangeCallback(const char* address, const uint32_t status)
{
    OIC_LOG_V(DEBUG, TAG,
            "CALENetStateChangeCallback, status:%d", status);

    // call the callback
    if (gLENetworkChangeCallback != NULL)
    {
        CANetworkStatus_t netStatus = CA_INTERFACE_DOWN;
        if (status == 12)
        {
            netStatus = CA_INTERFACE_UP;
        }
        else if (status == 10)
        {
            netStatus = CA_INTERFACE_DOWN;
        }

        CALocalConnectivity_t *localEndpoint = CAAdapterCreateLocalEndpoint(CA_LE, address);
        if (!localEndpoint)
        {
            OIC_LOG(ERROR, TAG, "Out of memory");
            return;
        }

        gLENetworkChangeCallback(localEndpoint, netStatus);

        CAAdapterFreeLocalEndpoint(localEndpoint);

    }
}

CAResult_t CAInitializeLE(CARegisterConnectivityCallback registerCallback,
                          CANetworkPacketReceivedCallback reqRespCallback,
                          CANetworkChangeCallback netCallback, ca_thread_pool_t handle)
{
    OIC_LOG(DEBUG, TAG, "IntializeBLE");

    gLEReceivedCallback = reqRespCallback;
    gLENetworkChangeCallback = netCallback;

    // register handlers
    CAConnectivityHandler_t handler;
    memset(&handler, 0, sizeof(CAConnectivityHandler_t));

    handler.startAdapter = CAStartLE;
    handler.startListenServer = CAStartLEListeningServer;
    handler.startDiscoveryServer = CAStartLEDiscoveryServer;
    handler.sendData = CASendLEUnicastData;
    handler.sendDataToAll = CASendLEMulticastData;
    handler.GetnetInfo = CAGetLEInterfaceInformation;
    handler.readData = CAReadLEData;
    handler.stopAdapter = CAStopLE;
    handler.terminate = CATerminateLE;
    registerCallback(handler, CA_LE);

    CALEInitialize(handle);
    CALEServerInitialize(handle);

    CALESetCallback(CALEPacketReceiveCallback);
    CALESetNetStateCallback(CALENetStateChangeCallback);
    CALEServerSetCallback(CALEPacketReceiveCallback);

    return CA_STATUS_OK;
}

CAResult_t CAStartLE()
{
    OIC_LOG(DEBUG, TAG, "CAStartLE");
    //CANativeLEStartScan();

    return CA_STATUS_OK;
}

CAResult_t CAStartLEListeningServer()
{
    OIC_LOG(DEBUG, TAG, "CAStartLEListeningServer");

    g_isBluetoothGattServer = JNI_TRUE;

    // start gatt service
    CALEServerStartMulticastServer();

    return CA_STATUS_OK;
}

CAResult_t CAStartLEDiscoveryServer()
{
    OIC_LOG(DEBUG, TAG, "CAStartLEDiscoveryServer");

    g_isBluetoothGattServer = JNI_FALSE;

    // start scan through gatt client
    CALEStartMulticastServer();

    return CA_STATUS_OK;
}

int32_t CASendLEUnicastData(const CARemoteEndpoint_t* endpoint, const void* data, uint32_t dataLen)
{

    if (g_isBluetoothGattServer == JNI_FALSE)
    {
        OIC_LOG(DEBUG, TAG, "CALESendUnicastData");
        CALESendUnicastMessage(endpoint->addressInfo.BT.btMacAddress, data, dataLen);
    }
    else
    {
        OIC_LOG(DEBUG, TAG, "CALEServerSendUnicastData");
        CALEServerSendUnicastMessage(endpoint->addressInfo.BT.btMacAddress, data, dataLen);
    }

    return dataLen;
}

int32_t CASendLEMulticastData(const void* data, uint32_t dataLen)
{
    if (g_isBluetoothGattServer == JNI_FALSE)
    {
        OIC_LOG(DEBUG, TAG, "CASendLEMulticastData");
        CALESendMulticastMessage(data, dataLen);
    }
    else
    {
        OIC_LOG(DEBUG, TAG, "CALEServerSendMulticastMessage");
        CALEServerSendMulticastMessage(data, dataLen);
    }

    return dataLen;
}

CAResult_t CAStartLENotifyServer()
{
    OIC_LOG(DEBUG, TAG, "CAStartLENotifyServer");

    return CA_STATUS_OK;
}

uint32_t CASendLENotification(const CARemoteEndpoint_t* endpoint, const void* data,
                              uint32_t dataLen)
{
    if (g_isBluetoothGattServer == JNI_FALSE)
    {
        OIC_LOG(DEBUG, TAG, "not server mode");
        return -1;
    }
    else
    {
        OIC_LOG(DEBUG, TAG, "CALEServerSendLEUnicastData");
        CALEServerSendUnicastMessage(endpoint->addressInfo.BT.btMacAddress, data, dataLen);
    }

    return 0;
}

CAResult_t CAGetLEInterfaceInformation(CALocalConnectivity_t** info, uint32_t* size)
{
    OIC_LOG(DEBUG, TAG, "CAGetLEInterfaceInformation");

    CALocalConnectivity_t *netInfo = NULL;

    int32_t netInfoSize = 1;

    netInfo = (CALocalConnectivity_t *) OICMalloc(sizeof(CALocalConnectivity_t) * netInfoSize);
    if (NULL == netInfo)
    {
        OIC_LOG(ERROR, TAG, "Invalid input..");
        return CA_MEMORY_ALLOC_FAILED;
    }
    memset(netInfo, 0, sizeof(CALocalConnectivity_t) * netInfoSize);

    char *macAddress = NULL;
    CAResult_t ret = CALEGetInterfaceInfo(&macAddress);
    OIC_LOG_V(ERROR, TAG, "address : %s", macAddress);
    if (CA_STATUS_OK != ret || NULL == macAddress)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to get interface info [%d]", ret);

        OICFree(netInfo);
        OICFree(macAddress);
        return ret;
    }

    // Create local endpoint using util function
    CALocalConnectivity_t *endpoint = CAAdapterCreateLocalEndpoint(CA_LE, macAddress);
    if (NULL == endpoint)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to create Local Endpoint!",
                CA_MEMORY_ALLOC_FAILED);
        OICFree(netInfo);
        OICFree(macAddress);
        return CA_MEMORY_ALLOC_FAILED;
    }

    // copy unciast server information
    endpoint->isSecured = false;
    memcpy(&netInfo[0], endpoint, sizeof(CALocalConnectivity_t));
    *size = 1;
    *info = netInfo;

    OICFree(macAddress);
    CAAdapterFreeLocalEndpoint(endpoint);

    OIC_LOG(INFO, TAG, "GetLEInterfaceInformation success");
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAReadLEData()
{
    OIC_LOG(DEBUG, TAG, "Read LE Data");

    return CA_STATUS_OK;
}

CAResult_t CAStopLE()
{

    if (g_isBluetoothGattServer == JNI_FALSE)
    {
        OIC_LOG(DEBUG, TAG, "CA Stop LE Scan");
        CANativeLEStopScan();
    }
    else
    {
        OIC_LOG(DEBUG, TAG, "CA Stop Gatt Server");
    }

    return CA_STATUS_OK;
}

void CATerminateLE()
{
    if (g_isBluetoothGattServer == JNI_FALSE)
    {
        OIC_LOG(DEBUG, TAG, "Terminat Gatt Client");
        CALETerminate();
    }
    else
    {
        OIC_LOG(DEBUG, TAG, "Terminat Gatt Server");
        CALEServerTerminate();
    }
}

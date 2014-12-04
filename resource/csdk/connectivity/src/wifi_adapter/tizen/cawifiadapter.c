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
#include "cawifiadapter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "caadapterutils.h"
#include "umutex.h"
#include "logger.h"
#include "cawifiserver.h"
#include "cawificlient.h"
#include "cawifimonitor.h"
#include "camessagequeue.h"

/**
 * @def WIFI_ETHERNET_ADAPTER_TAG
 * @brief Logging tag for module name
 */
#define WIFI_ADAPTER_TAG "WIFI_ADAPTER"

/**
 * @def CA_PORT
 * @brief Port to listen for incoming data
 */
#define CA_PORT   5283

/**
 * @def CA_MCAST_PORT
 * @brief Multicast Port Number
 */
#define CA_MCAST_PORT   5298

/**
 * @def CA_MULTICAST_IP
 * @brief Multicast IP Address
 */
#define CA_MULTICAST_IP "224.0.1.187"

/* Skip Queue */
/**
 * @var gNetworkPacketCallback
 * @brief Network Packet Received Callback
 */
static CANetworkPacketReceivedCallback gNetworkPacketCallback = NULL;

/**
 * @var gIsMulticastServerStarted
 * @brief Flag to check if multicast server is started
 */
static int gIsMulticastServerStarted = 0;

/**
 * @var gMutexIsMulticastServerStarted
 * @brief Mutex for gIsMulticastServerStarted
 */
static u_mutex gMutexIsMulticastServerStarted = NULL;

/**
 * @var gSendQueueHandle
 * @brief Queue handle for Send Data
 */
static CAAdapterMessageQueue_t *gSendQueueHandle = NULL;

/**
 * @var gRecvQueueHandle
 * @brief Queue handle for Receive Data
 */
static CAAdapterMessageQueue_t *gRecvQueueHandle = NULL;

/**
 * @var gSendDataCond
 * @brief Condition for Send Data
 */
static u_cond gSendDataCond = NULL;

void CAInitializeMutex()
{
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "IN");
    u_mutex_init();
    if(NULL == gMutexIsMulticastServerStarted)
    {
        gMutexIsMulticastServerStarted = u_mutex_new();
    }
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "OUT");
}

CAResult_t CAInitializeQueueHandles()
{
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "IN");
    if (CA_STATUS_OK != CAAdapterInitializeMessageQueue(&gSendQueueHandle))
    {
        OIC_LOG(ERROR, WIFI_ADAPTER_TAG, "Failed to Initialize Send queue Handle");
        return CA_STATUS_FAILED;
    }
    gSendDataCond = u_cond_new();
    CASetSendQueueHandle(gSendQueueHandle, CA_TRUE, gSendDataCond);

    if (CA_STATUS_OK != CAAdapterInitializeMessageQueue(&gRecvQueueHandle))
    {
        OIC_LOG(ERROR, WIFI_ADAPTER_TAG, "Failed to Initialize Receive queue Handle");
        return CA_STATUS_FAILED;
    }
    CASetRecvQueueHandle(gRecvQueueHandle);
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CADeinitializeSendQueueHandle()
{
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "IN");
    CASetSendQueueHandle(NULL, CA_FALSE, NULL);
    CAAdapterTerminateMessageQueue(gSendQueueHandle);
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "OUT");
}

void CADeinitializeRecvQueueHandle()
{
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "IN");
    CAAdapterTerminateMessageQueue(gRecvQueueHandle);
    CASetRecvQueueHandle(NULL);
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "OUT");
}

int CAWIFIRegisterNetworkNotifications(CANetworkChangeCallback netCallback)
{
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "IN");

    if (NULL != netCallback)
    {
        CAInitializeWIFIAdapter();
        CASetWIFINetworkChangeCallback(netCallback);
    }
    else
    {
        CADeinitializeWIFIAdapter();
    }

    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

#if 0 /* Skip Queue */
void CASetWIFINetworkPacketCallback(CANetworkPacketReceivedCallback callback)
{
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "IN");
    gNetworkPacketCallback = callback;
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "OUT");
}
#endif //#if 0

CAResult_t CAInitializeWifi(CARegisterConnectivityCallback registerCallback,
                            CANetworkPacketReceivedCallback networkPacketCallback,
                            CANetworkChangeCallback netCallback, u_thread_pool_t handle)
{
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "IN");
    VERIFY_NON_NULL(registerCallback, WIFI_ADAPTER_TAG,
            "Invalid argument : registerCallback is NULL");
    VERIFY_NON_NULL(networkPacketCallback, WIFI_ADAPTER_TAG,
                    "Invalid argument : networkPacketCallback is NULL");
    VERIFY_NON_NULL(netCallback, WIFI_ADAPTER_TAG, "Invalid argument : netCallback is NULL");

    CASetWIFINetworkPacketCallback(networkPacketCallback);
    CAWIFIRegisterNetworkNotifications(netCallback);

    CAConnectivityHandler_t wifiHandler;
    wifiHandler.startAdapter = CAStartWIFI;
    wifiHandler.startListenServer = CAStartWIFIListeningServer;
    wifiHandler.startDiscoverServer = CAStartWIFIDiscoveryServer;
    wifiHandler.sendData = CASendWIFIUnicastData;
    wifiHandler.sendDataToAll = CASendWIFIMulticastData;
    wifiHandler.GetnetInfo = CAGetWIFIInterfaceInformation;
    wifiHandler.readData = CAReadWIFIData;
    wifiHandler.stopAdapter = CAStopWIFI;
    wifiHandler.terminate = CATerminateWIfI;
    registerCallback(wifiHandler, CA_WIFI);

    CAInitializeMutex();
    CAInitializeServerMutex();
    CASetThreadHandle(handle);
    CASetMulticastData(CA_MULTICAST_IP, CA_MCAST_PORT);

    if (CA_STATUS_OK != CAInitializeQueueHandles())
    {
        OIC_LOG(ERROR, WIFI_ADAPTER_TAG, "Failed to Initialize Queue Handle");
        return CA_STATUS_FAILED;
    }
    if (CA_STATUS_OK != CAStartSendDataThread(handle))
    {
        OIC_LOG(ERROR, WIFI_ADAPTER_TAG, "Failed to Start Send Data Thread");
        return CA_STATUS_FAILED;
    }

    OIC_LOG(INFO, WIFI_ADAPTER_TAG, "IntializeWifi is Success");

    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAStartWIFI()
{
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "IN");
    int16_t unicastPort = CA_PORT;
    CAResult_t ret = CA_STATUS_OK;

    CASetIsStartServerInvoked();
    CABool_t retVal = CAIsWIFIConnected();
    if (CA_FALSE == retVal)
    {
        OIC_LOG(INFO, WIFI_ADAPTER_TAG, "WIFI is not Connected");
        return ret;
    }

    // Address is hardcoded as we are using Single Interface
    ret = CAStartUnicastServer("0.0.0.0", &unicastPort);

    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "OUT");
    return ret;
}

CAResult_t CAStartWIFIListeningServer()
{
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "IN");

    CAResult_t ret = CA_STATUS_OK;
    int16_t multicastPort = CA_MCAST_PORT;

    if (gIsMulticastServerStarted == 1)
    {
        OIC_LOG_V(ERROR, WIFI_ADAPTER_TAG,
                "Failed to Start Listening Server, Already Started! Return code[%d]",
                CA_SERVER_STARTED_ALREADY);
        return CA_SERVER_STARTED_ALREADY;
    }

    CABool_t retVal = CAIsWIFIConnected();
    if (CA_FALSE == retVal)
    {
        OIC_LOG_V(ERROR, WIFI_ADAPTER_TAG,
                "Failed to Start Listening Server, WIFI is not Connected! Return code[%d]",
                CA_ADAPTER_NOT_ENABLED);
        return CA_ADAPTER_NOT_ENABLED;
    }

    ret = CAStartMulticastServer(CA_MULTICAST_IP, "0.0.0.0", &multicastPort);
    if (CA_STATUS_OK == ret)
    {
        OIC_LOG(INFO, WIFI_ADAPTER_TAG, "Multicast Server is Started Successfully");
        u_mutex_lock(gMutexIsMulticastServerStarted);
        gIsMulticastServerStarted = 1;
        u_mutex_unlock(gMutexIsMulticastServerStarted);
    }

    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "OUT");
    return ret;
}

CAResult_t CAStartWIFIDiscoveryServer()
{
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "IN");

    CAResult_t ret = CA_STATUS_OK;
    int16_t multicastPort = CA_MCAST_PORT;

    if (gIsMulticastServerStarted == 1)
    {
        OIC_LOG_V(ERROR, WIFI_ADAPTER_TAG,
                "Failed to Start Discovery Server, Already Started! Return code[%d]",
                CA_SERVER_STARTED_ALREADY);
        return CA_SERVER_STARTED_ALREADY;
    }

    CABool_t retVal = CAIsWIFIConnected();
    if (CA_FALSE == retVal)
    {
        OIC_LOG_V(ERROR, WIFI_ADAPTER_TAG,
                "Failed to Start Discovery Server, WIFI is not Connected! Return code[%d]",
                CA_ADAPTER_NOT_ENABLED);
        return CA_ADAPTER_NOT_ENABLED;
    }

    ret = CAStartMulticastServer(CA_MULTICAST_IP, "0.0.0.0", &multicastPort);
    if (CA_STATUS_OK == ret)
    {
        OIC_LOG(INFO, WIFI_ADAPTER_TAG, "Multicast Server is Started Successfully");
        u_mutex_lock(gMutexIsMulticastServerStarted);
        gIsMulticastServerStarted = 1;
        u_mutex_unlock(gMutexIsMulticastServerStarted);
    }

    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "OUT");
    return ret;
}

uint32_t CASendWIFIUnicastData(const CARemoteEndpoint_t *remoteEndpoint, void *data,
                               uint32_t dataLen)
{
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "IN");

    uint32_t dataSize = 0;

    VERIFY_NON_NULL_RET(remoteEndpoint, WIFI_ADAPTER_TAG, "Invalid argument : remoteEndpoint is NULL",
                        dataSize);
    VERIFY_NON_NULL_RET(data, WIFI_ADAPTER_TAG, "Invalid argument : data is NULL", dataSize);

    if (dataLen <= 0)
    {
        OIC_LOG(ERROR, WIFI_ADAPTER_TAG, "Invalid argument : Data Length is 0");
        return dataSize;

    }

    if (NULL == gSendQueueHandle)
    {
        OIC_LOG(ERROR, WIFI_ADAPTER_TAG, "Queue Handle is not Initialized");
        return dataSize;
    }
    CAAdapterEnqueueMessage(gSendQueueHandle, remoteEndpoint, data, dataLen);
    u_cond_signal(gSendDataCond);

    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "OUT");
    return dataLen;
}

uint32_t CASendWIFIMulticastData(void *data, uint32_t dataLength)
{
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "IN");

    uint32_t dataSize = 0;

    VERIFY_NON_NULL_RET(data, WIFI_ADAPTER_TAG, "Invalid argument : data is NULL", dataSize);

    if (dataLength <= 0)
    {
        OIC_LOG(ERROR, WIFI_ADAPTER_TAG, "Invalid argument : Data Length is 0");
        return 0;

    }

    if (NULL == gSendQueueHandle)
    {
        OIC_LOG(ERROR, WIFI_ADAPTER_TAG, "Queue Handle is not Initialized");
        return dataSize;
    }
    CAAdapterEnqueueMessage(gSendQueueHandle, NULL, data, dataLength);
    u_cond_signal(gSendDataCond);

    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "OUT");
    return dataLength;
}

CAResult_t CAGetWIFIInterfaceInformation(CALocalConnectivity_t **info, uint32_t *size)
{
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "IN");

    CABool_t retVal = CAIsWIFIConnected();
    if (CA_FALSE == retVal)
    {
        OIC_LOG_V(ERROR, WIFI_ADAPTER_TAG,
                "Failed to get interface address, WIFI is not Connected! Return code[%d]",
                CA_ADAPTER_NOT_ENABLED);
        return CA_ADAPTER_NOT_ENABLED;
    }

    VERIFY_NON_NULL(info, WIFI_ADAPTER_TAG, "Invalid argument : info is NULL");
    char localIpAddress[CA_IPADDR_SIZE];
    int32_t localIpAddressLen = sizeof(localIpAddress);

    CAGetInterfaceAddress(localIpAddress, localIpAddressLen);


    // Create local endpoint using util function
    (*info) = CAAdapterCreateLocalEndpoint(CA_WIFI, localIpAddress, "WiFi");
    if (NULL == (*info))
    {
        OIC_LOG_V(ERROR, WIFI_ADAPTER_TAG, "Failed to create Local Endpoint! Return code[%d]",
                  CA_MEMORY_ALLOC_FAILED);
        return CA_MEMORY_ALLOC_FAILED;
    }

    (*size) = 1;

    OIC_LOG(INFO, WIFI_ADAPTER_TAG, "GetWIFIInterfaceInformation success");

    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAReadWIFIData()
{
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "IN");
    if (NULL == gRecvQueueHandle)
    {
        OIC_LOG(ERROR, WIFI_ADAPTER_TAG, "QueueHandle is NULL");
        return CA_STATUS_FAILED;
    }
    CAAdapterMessage_t *messageReceived;
    if (CA_STATUS_OK != CAAdapterDequeueMessage(gRecvQueueHandle, &messageReceived))
    {
        OIC_LOG(ERROR, WIFI_ADAPTER_TAG, "Dequeue Message failed");
        return CA_STATUS_FAILED;
    }

    if (gNetworkPacketCallback && (NULL != messageReceived))
    {
        gNetworkPacketCallback(messageReceived->remoteEndpoint, messageReceived->data,
                               messageReceived->dataLen);
    }

    CAAdapterFreeMessage(messageReceived);
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAStopWIFI()
{
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "IN");
    CAResult_t result = CA_STATUS_FAILED;
    result = CAStopUnicastServer();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG_V(ERROR, WIFI_ADAPTER_TAG, "Failed to Stop Unicast Server! Return code[%d]", result);
        return result;
    }
    CAUnsetIsStartServerInvoked();
    result = CAStopMulticastServer();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG_V(ERROR, WIFI_ADAPTER_TAG, "Failed to Stop Multicast Server! Return code[%d]", result);
        return result;
    }
    else
    {
        u_mutex_lock(gMutexIsMulticastServerStarted);
        gIsMulticastServerStarted = 0;
        u_mutex_unlock(gMutexIsMulticastServerStarted);
    }
    CADeinitializeRecvQueueHandle();
    CADeinitializeSendQueueHandle();
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CATerminateWIfI()
{
    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "IN");
    CAResult_t result = CA_STATUS_FAILED;

    CASetWIFINetworkPacketCallback(NULL);
    result = CAWIFIRegisterNetworkNotifications(NULL);
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, WIFI_ADAPTER_TAG,
                "Failed to Unregister Network Notifications");
    }
    OIC_LOG(INFO, WIFI_ADAPTER_TAG, "TerminateWifi Success");

    OIC_LOG(DEBUG, WIFI_ADAPTER_TAG, "OUT");
    return;
}

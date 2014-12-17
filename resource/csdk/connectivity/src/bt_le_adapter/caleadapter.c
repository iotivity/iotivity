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
#include "caleadapter.h"

#ifdef __TIZEN__
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <unistd.h>
#include <pthread.h>
#endif //#ifdef __TIZEN__
#include <stdio.h>
#include <stdlib.h>

#ifdef __TIZEN__
#include "cableserver.h"
#include "cableclient.h"
#include "cacommon.h"
#include "umutex.h"
#include "caadapterutils.h"
#else // __ARDUINO__
#include "BLEAdapterArduino.h"
#include "caadapterutils.h"
#endif //#ifdef __TIZEN__
#include "caqueueingthread.h"
#include "camsgparser.h"


#define CALEADAPTER_TAG "CA_BLE_ADAPTER"

static CANetworkChangeCallback gNetworkCallback = NULL;

static char gLocalBLEAddress[16] = { 0, };

static CABool_t gIsServer = CA_FALSE;

static u_mutex gBleIsServerMutex = NULL;

static u_mutex gBleNetworkCbMutex = NULL;

static u_mutex gBleLocalAddressMutex = NULL;

/**
 * @var gBleClientThreadPool
 * @brief reference to threadpool
 */
static u_thread_pool_t gBleAdapterThreadPool = NULL;

/**
 * @var gBleAdapterThreadPoolMutex
 * @brief Mutex to synchronize the task to be pushed to thread pool.
 */
static u_mutex gBleAdapterThreadPoolMutex = NULL;

/**
 * @var gBLEClientSendQueueHandle
 * @brief Queue to process the outgoing packets from GATTClient.
 */
static CAQueueingThread_t *gBLEClientSendQueueHandle = NULL;

/**
 * @var gCABleClientReceiverQueue
 * @brief Queue to process the incoming packets to GATT Client.
 */
static CAQueueingThread_t *gCABleClientReceiverQueue = NULL;

/**
 * @var gBleClientSendDataMutex
 * @brief Mutex to synchronize the queing of the data from SenderQueue.
 */
static u_mutex gBleClientSendDataMutex = NULL;


/**
 * @var gBleClientReceiveDataMutex
 * @brief Mutex to synchronize the queing of the data from ReceiverQueue.
 */
static u_mutex gBleClientReceiveDataMutex = NULL;

/**
 * @var gDataReceiverHandlerState
 * @brief Stop condition of redvhandler.
 */
static bool gDataReceiverHandlerState = false;

/**
 * @var gSendQueueHandle
 * @brief Queue to process the outgoing packets from GATTServer.
 */
static CAQueueingThread_t *gSendQueueHandle = NULL;

/**
 * @var gCABleServerReceiverQueue
 * @brief Queue to process the incoming packets to GATTServer
 */
static CAQueueingThread_t *gCABleServerReceiverQueue = NULL;

/**
 * @var gBleServerSendDataMutex
 * @brief Mutex to synchronize the queing of the data from SenderQueue.
 */
static u_mutex gBleServerSendDataMutex = NULL;

/**
 * @var gBleServerReceiveDataMutex
 * @brief Mutex to synchronize the queing of the data from ReceiverQueue.
 */
static u_mutex gBleServerReceiveDataMutex = NULL;

static u_mutex gBleAdapterReqRespCbMutex = NULL;

static CANetworkPacketReceivedCallback gNetworkPacketReceivedCallback = NULL;
/**
 * @ENUM CALeServerStatus
 * @brief status of BLE Server Status
 *
 *  This ENUM provides information of LE Adapter Server status
 */
typedef enum
{
    CA_SERVER_NOTSTARTED = 0,
    CA_LISTENING_SERVER,
    CA_DISCOVERY_SERVER
} CALeServerStatus;

static CALeServerStatus gLeServerStatus = CA_SERVER_NOTSTARTED;

int32_t CALERegisterNetworkNotifications(CANetworkChangeCallback netCallback);

void CASetBleAdapterThreadPoolHandle(u_thread_pool_t handle);

#ifdef __TIZEN__
void CALEDeviceStateChangedCb(int32_t result, bt_adapter_state_e adapter_state,
                              void *user_data);
CAResult_t CAInitBleAdapterMutex();
CAResult_t CATermiateBleAdapterMutex();

#endif //#ifdef __TIZEN__

CAResult_t CAInitializeLE(CARegisterConnectivityCallback registerCallback,
                          CANetworkPacketReceivedCallback reqRespCallback,
                          CANetworkChangeCallback netCallback,
                          u_thread_pool_t handle)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    //Input validation
    VERIFY_NON_NULL(registerCallback, NULL, "RegisterConnectivity callback is null");
    VERIFY_NON_NULL(reqRespCallback, NULL, "PacketReceived Callback is null");
    VERIFY_NON_NULL(netCallback, NULL, "NetworkChange Callback is null");

    CAResult_t result = CAInitBleAdapterMutex();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "CAInitBleAdapterMutex failed!");
        return CA_STATUS_FAILED;
    }

#ifdef __TIZEN__

    bt_initialize();

#endif //#ifdef __TIZEN__

    CASetBleServerThreadPoolHandle(handle);
    CASetBleClientThreadPoolHandle(handle);
    CASetBleAdapterThreadPoolHandle(handle);
    CASetBLEReqRespServerCallback(CABLEServerReceivedData);
    CASetBLEReqRespClientCallback(CABLEClientReceivedData);
    CASetBLEReqRespAdapterCallback(reqRespCallback);

    CALERegisterNetworkNotifications(netCallback);

    CAConnectivityHandler_t connHandler;
    connHandler.startAdapter = NULL;
    connHandler.stopAdapter = NULL;
    connHandler.startListenServer = CAStartLEListeningServer;
    connHandler.startDiscoverServer = CAStartLEDiscoveryServer;
    connHandler.sendData = CASendLEUnicastData;
    connHandler.sendDataToAll = CASendLEMulticastData;
    connHandler.GetnetInfo = CAGetLEInterfaceInformation;
    connHandler.readData = CAReadLEData;
    connHandler.terminate = CATerminateLE;
    registerCallback(connHandler, CA_LE);

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");

    return CA_STATUS_OK;
}

void CATerminateLE()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    CASetBLEReqRespServerCallback(NULL);
    CASetBLEReqRespClientCallback(NULL);
    CALERegisterNetworkNotifications(NULL);

    u_mutex_lock(gBleIsServerMutex);
    if (CA_TRUE == gIsServer)
    {
        CAStopBleGattServer();
    }
    else
    {
        CAStopBLEGattClient();
    }
    u_mutex_unlock(gBleIsServerMutex);

    CATerminateBleQueues();

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return;
}

CAResult_t CAStartLEListeningServer()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    bt_error_e err = BT_ERROR_NONE;
    bt_adapter_state_e adapterState;

    CAResult_t result = CAInitBleServerQueues();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "CAInitBleClientQueues failed");
        return CA_STATUS_FAILED;
    }

    //Get Bluetooth adapter state
    if (BT_ERROR_NONE != (err = bt_adapter_get_state(&adapterState)))
    {
        OIC_LOG_V(ERROR, CALEADAPTER_TAG, "Bluetooth get state failed!, error num [%x]",
                  err);

        return CA_STATUS_FAILED;
    }

    if (BT_ADAPTER_ENABLED != adapterState)
    {
        gLeServerStatus = CA_LISTENING_SERVER;
        OIC_LOG(DEBUG, CALEADAPTER_TAG, "Listen Server will be started once BT Adapter is enabled");
        return CA_STATUS_OK;
    }

    CAStartBleGattServer();

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    u_mutex_lock(gBleIsServerMutex);
    gIsServer = CA_TRUE;
    u_mutex_unlock(gBleIsServerMutex);
    return CA_STATUS_OK;
}

CAResult_t CAStartLEDiscoveryServer()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    bt_error_e err = BT_ERROR_NONE;
    bt_adapter_state_e adapterState;

    CAResult_t result = CAInitBleClientQueues();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "CAInitBleClientQueues failed");
        return CA_STATUS_FAILED;
    }

    //Get Bluetooth adapter state
    if (BT_ERROR_NONE != (err = bt_adapter_get_state(&adapterState)))
    {
        OIC_LOG_V(ERROR, CALEADAPTER_TAG, "Bluetooth get state failed!, error num [%x]",
                  err);

        return CA_STATUS_FAILED;
    }

    if (BT_ADAPTER_ENABLED != adapterState)
    {
        gLeServerStatus = CA_DISCOVERY_SERVER;
        OIC_LOG(DEBUG, CALEADAPTER_TAG, "Disc Server will be started once BT Adapter is enabled");
        return CA_STATUS_OK;
    }

    CAStartBLEGattClient();

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    u_mutex_lock(gBleIsServerMutex);
    gIsServer = CA_FALSE;
    u_mutex_unlock(gBleIsServerMutex);
    return CA_STATUS_OK;
}

CAResult_t CAStartLENotifyServer()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");

    return CA_STATUS_OK;
}

uint32_t CASendLENotification(const CARemoteEndpoint_t *endpoint, void *data, uint32_t dataLen)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");

    return CA_STATUS_OK;
}

CAResult_t CAReadLEData()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");

    return CA_STATUS_OK;
}

uint32_t CASendLEUnicastData(const CARemoteEndpoint_t *endpoint, void *data, uint32_t dataLen)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    //Input validation
    VERIFY_NON_NULL(endpoint, NULL, "Remote endpoint is null");
    VERIFY_NON_NULL(data, NULL, "Data is null");

    CAResult_t result = CA_STATUS_FAILED;

#ifdef __TIZEN__
    u_mutex_lock(gBleIsServerMutex);
    if (CA_TRUE  == gIsServer)
    {
        result = CABLEServerSendData(endpoint, data, dataLen);
        if (CA_STATUS_OK != result)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG,
                    "[SendLEUnicastData] CABleServerSenderQueueEnqueueMessage failed \n");
            u_mutex_unlock(gBleIsServerMutex);
            return 0;
        }
    }
    else
    {
        result = CABLEClientSendData(endpoint, data, dataLen);
        if (CA_STATUS_OK != result)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG,
                    "[SendLEUnicastData] CABleClientSenderQueueEnqueueMessage failed \n");
            u_mutex_unlock(gBleIsServerMutex);
            return 0;
        }
    }
    u_mutex_unlock(gBleIsServerMutex);
#else
    char *tempPath = "temp_path";
    updateCharacteristicsInGattServer(tempPath, (char *) data, dataLen);
#endif //#ifdef __TIZEN__
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");

    return dataLen;
}

uint32_t CASendLEMulticastData(void *data, uint32_t dataLen)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    //Input validation
    VERIFY_NON_NULL(data, NULL, "Data is null");

    if (0 >= dataLen)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Invalid Parameter");
        return 0;
    }

    CAResult_t result = CA_STATUS_FAILED;
#ifdef __TIZEN__
    u_mutex_lock(gBleIsServerMutex);
    if (CA_TRUE  == gIsServer)
    {
        result = CABLEServerSendData(NULL, data, dataLen);
        if (CA_STATUS_OK != result)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG,
                    "[SendLEMulticastDataToAll] CABleServerSenderQueueEnqueueMessage failed" );
            u_mutex_unlock(gBleIsServerMutex);
            return 0;
        }
    }
    else
    {
        result = CABLEClientSendData(NULL, data, dataLen);
        if (CA_STATUS_OK != result)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG,
                    "[SendLEMulticastDataToAll] CABleClientSenderQueueEnqueueMessage failed" );
            u_mutex_unlock(gBleIsServerMutex);
            return 0;
        }
    }
    u_mutex_unlock(gBleIsServerMutex);
#else
    char *tempPath = "temp_path";
    updateCharacteristicsInGattServer(tempPath, (char *) data, dataLen);
#endif //#ifdef __TIZEN__
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return dataLen;
}

CAResult_t CAGetLEInterfaceInformation(CALocalConnectivity_t **info, uint32_t *size)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    VERIFY_NON_NULL(info, NULL, "CALocalConnectivity info is null");

#if __TIZEN__

    char *local_address = NULL;

    bt_adapter_get_address(&local_address);
    if (NULL == local_address)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Get local bt adapter address failed");
        return CA_STATUS_FAILED;
    }

#endif //#if ARDUINODUE
    *size = 0;
    (*info) = (CALocalConnectivity_t *) OICMalloc(sizeof(CALocalConnectivity_t));
    if (NULL == (*info))
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Malloc failure!");
        return CA_STATUS_FAILED;
    }
    memset((*info), 0x0, sizeof(CALocalConnectivity_t));

    strncpy((*info)->addressInfo.BT.btMacAddress, local_address, strlen(local_address));
    u_mutex_lock(gBleLocalAddressMutex);
    strncpy(gLocalBLEAddress, local_address, sizeof(gLocalBLEAddress));
    u_mutex_unlock(gBleLocalAddressMutex);

    (*info)->type = CA_LE;
    *size = 1;
    OICFree(local_address);

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

int32_t CALERegisterNetworkNotifications(CANetworkChangeCallback netCallback)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    u_mutex_lock(gBleNetworkCbMutex);
    gNetworkCallback = netCallback;
    u_mutex_unlock(gBleNetworkCbMutex);
    int32_t ret = 0;
#ifdef __TIZEN__
    if (netCallback)
    {
        ret = bt_adapter_set_state_changed_cb(CALEDeviceStateChangedCb, NULL);
        if (ret != 0)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "bt_adapter_set_state_changed_cb failed!");
        }
    }
    else
    {
        ret = bt_adapter_unset_state_changed_cb();
        if (ret != 0)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "bt_adapter_set_state_changed_cb failed!");
        }
    }
#endif //#ifdef __TIZEN__
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

#ifdef __TIZEN__

void CALEDeviceStateChangedCb(int32_t result, bt_adapter_state_e adapter_state, void *user_data)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    CALocalConnectivity_t localEndpoint;

    u_mutex_lock(gBleLocalAddressMutex);
    strncpy(localEndpoint.addressInfo.BT.btMacAddress, gLocalBLEAddress, strlen(gLocalBLEAddress));
    u_mutex_unlock(gBleLocalAddressMutex);

    // Start a GattServer/Client if gLeServerStatus is SET
    if (CA_LISTENING_SERVER == gLeServerStatus)
    {
        OIC_LOG(DEBUG, CALEADAPTER_TAG, "Before CAStartBleGattServer");
        CAStartBleGattServer();
    }
    else if (CA_DISCOVERY_SERVER == gLeServerStatus)
    {
        OIC_LOG(DEBUG, CALEADAPTER_TAG, "Before CAStartBleGattClient");
        CAStartBLEGattClient();
    }
    gLeServerStatus = CA_SERVER_NOTSTARTED;

    u_mutex_lock(gBleNetworkCbMutex);
    if (NULL != gNetworkCallback)
    {
        gNetworkCallback(&localEndpoint, adapter_state);
    }
    else
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "gNetworkCallback is NULL");
    }
    u_mutex_unlock(gBleNetworkCbMutex);

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
}

CAResult_t CAInitBleAdapterMutex()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    u_mutex_init();
    if (NULL == gBleIsServerMutex)
    {
        gBleIsServerMutex = u_mutex_new();
        if (NULL == gBleIsServerMutex)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "u_mutex_new failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == gBleNetworkCbMutex)
    {
        gBleNetworkCbMutex = u_mutex_new();
        if (NULL == gBleNetworkCbMutex)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "u_mutex_new failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == gBleLocalAddressMutex)
    {
        gBleLocalAddressMutex = u_mutex_new();
        if (NULL == gBleLocalAddressMutex)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "u_mutex_new failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == gBleAdapterThreadPoolMutex)
    {
        gBleAdapterThreadPoolMutex = u_mutex_new();
        if (NULL == gBleAdapterThreadPoolMutex)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "u_mutex_new failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == gBleClientSendDataMutex)
    {
        gBleClientSendDataMutex = u_mutex_new();
        if (NULL == gBleClientSendDataMutex)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "u_mutex_new failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == gBleClientReceiveDataMutex)
    {
        gBleClientReceiveDataMutex = u_mutex_new();
        if (NULL == gBleClientReceiveDataMutex)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "u_mutex_new failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == gBleServerSendDataMutex)
    {
        gBleServerSendDataMutex = u_mutex_new();
        if (NULL == gBleServerSendDataMutex)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "u_mutex_new failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == gBleServerReceiveDataMutex)
    {
        gBleServerReceiveDataMutex = u_mutex_new();
        if (NULL == gBleServerReceiveDataMutex)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "u_mutex_new failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == gBleAdapterReqRespCbMutex)
    {
        gBleAdapterReqRespCbMutex = u_mutex_new();
        if (NULL == gBleAdapterReqRespCbMutex)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "u_mutex_new failed");
            return CA_STATUS_FAILED;
        }
    }

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");
    return CA_STATUS_OK;
}
CAResult_t CATermiateBleAdapterMutex()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    u_mutex_free(gBleIsServerMutex);
    gBleIsServerMutex = NULL;

    u_mutex_free(gBleNetworkCbMutex);
    gBleNetworkCbMutex = NULL;

    u_mutex_free(gBleLocalAddressMutex);
    gBleLocalAddressMutex = NULL;

    u_mutex_free(gBleAdapterThreadPoolMutex);
    gBleAdapterThreadPoolMutex = NULL;

    u_mutex_free(gBleClientSendDataMutex);
    gBleClientSendDataMutex = NULL;

    u_mutex_free(gBleClientReceiveDataMutex);
    gBleClientReceiveDataMutex = NULL;

    u_mutex_free(gBleServerSendDataMutex);
    gBleServerSendDataMutex = NULL;

    u_mutex_free(gBleServerReceiveDataMutex);
    gBleServerReceiveDataMutex = NULL;

    u_mutex_free(gBleAdapterReqRespCbMutex);
    gBleAdapterReqRespCbMutex = NULL;

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CAInitBleQueues()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    CAResult_t result = CAInitBleServerQueues();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "CAInitBleClientQueues failed");
        return;
    }

    result = CAInitBleClientQueues();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "CAInitBleClientQueues failed");
        return;
    }

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return;

}

CAResult_t CAInitBleServerQueues()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    u_mutex_lock(gBleAdapterThreadPoolMutex);

    CAResult_t result = CAInitBleServerSenderQueue();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "CAInitBleClientQueues failed");
        u_mutex_unlock(gBleAdapterThreadPoolMutex);
        return CA_STATUS_FAILED;
    }

    result = CAInitBleServerReceiverQueue();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "CAInitBleClientQueues failed");
        u_mutex_unlock(gBleAdapterThreadPoolMutex);
        return CA_STATUS_FAILED;
    }

    gDataReceiverHandlerState = true;

    u_mutex_unlock(gBleAdapterThreadPoolMutex);

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAInitBleClientQueues()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    u_mutex_lock(gBleAdapterThreadPoolMutex);

    CAResult_t result = CAInitBleClientSenderQueue();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "CAInitBleClientQueues failed");
        u_mutex_unlock(gBleAdapterThreadPoolMutex);
        return CA_STATUS_FAILED;
    }

    result = CAInitBleClientReceiverQueue();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "CAInitBleClientQueues failed");
        u_mutex_unlock(gBleAdapterThreadPoolMutex);
        return CA_STATUS_FAILED;
    }

    gDataReceiverHandlerState = true;

    u_mutex_unlock(gBleAdapterThreadPoolMutex);

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAInitBleServerSenderQueue()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");
    // Check if the message queue is already initialized
    if (gSendQueueHandle)
    {
        OIC_LOG(DEBUG, CALEADAPTER_TAG, "Already queue is initialized!");
        return CA_STATUS_OK;
    }

    // Create send message queue
    gSendQueueHandle = OICMalloc(sizeof(CAQueueingThread_t));
    if (!gSendQueueHandle)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Memory allocation failed!");
        return CA_MEMORY_ALLOC_FAILED;
    }

    if (CA_STATUS_OK != CAQueueingThreadInitialize(gSendQueueHandle, gBleAdapterThreadPool,
            CABLEServerSendDataThread))
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Failed to Initialize send queue thread");
        OICFree(gSendQueueHandle);
        gSendQueueHandle = NULL;
        return CA_STATUS_FAILED;
    }

    if (CA_STATUS_OK != CAQueueingThreadStart(gSendQueueHandle))
    {
        OIC_LOG_V(ERROR, CALEADAPTER_TAG, "u_thread_pool_add_task failed ");
        OICFree(gSendQueueHandle);
        gSendQueueHandle = NULL;
        return CA_STATUS_FAILED;
    }


    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return CA_STATUS_OK;

}

CAResult_t CAInitBleClientSenderQueue()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    if (gBLEClientSendQueueHandle)
    {
        OIC_LOG(DEBUG, CALEADAPTER_TAG, "Already queue is initialized!");
        return CA_STATUS_OK;
    }

    // Create send message queue
    gBLEClientSendQueueHandle = OICMalloc(sizeof(CAQueueingThread_t));
    if (!gBLEClientSendQueueHandle)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Memory allocation failed!");
        return CA_MEMORY_ALLOC_FAILED;
    }

    if (CA_STATUS_OK != CAQueueingThreadInitialize(gBLEClientSendQueueHandle, gBleAdapterThreadPool,
            CABLEClientSendDataThread))
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Failed to Initialize send queue thread");
        OICFree(gBLEClientSendQueueHandle);
        gBLEClientSendQueueHandle = NULL;
        return CA_STATUS_FAILED;
    }

    if (CA_STATUS_OK != CAQueueingThreadStart(gBLEClientSendQueueHandle))
    {
        OIC_LOG_V(ERROR, CALEADAPTER_TAG, "u_thread_pool_add_task failed ");
        OICFree(gBLEClientSendQueueHandle);
        gBLEClientSendQueueHandle = NULL;
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAInitBleServerReceiverQueue()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");
    // Check if the message queue is already initialized
    if (gCABleServerReceiverQueue)
    {
        OIC_LOG(DEBUG, CALEADAPTER_TAG, "Already queue is initialized!");
        return CA_STATUS_OK;
    }

    // Create send message queue
    gCABleServerReceiverQueue = OICMalloc(sizeof(CAQueueingThread_t));
    if (!gCABleServerReceiverQueue)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Memory allocation failed!");
        OICFree(gSendQueueHandle);
        return CA_MEMORY_ALLOC_FAILED;
    }

    if (CA_STATUS_OK != CAQueueingThreadInitialize(gCABleServerReceiverQueue, gBleAdapterThreadPool,
            CABLEServerDataReceiverHandler))
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Failed to Initialize send queue thread");
        OICFree(gCABleServerReceiverQueue);
        gCABleServerReceiverQueue = NULL;
        return CA_STATUS_FAILED;
    }

    if (CA_STATUS_OK != CAQueueingThreadStart(gCABleServerReceiverQueue))
    {
        OIC_LOG_V(ERROR, CALEADAPTER_TAG, "u_thread_pool_add_task failed ");
        OICFree(gCABleServerReceiverQueue);
        gCABleServerReceiverQueue = NULL;
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAInitBleClientReceiverQueue()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    // Check if the message queue is already initialized
    if (gCABleClientReceiverQueue)
    {
        OIC_LOG(DEBUG, CALEADAPTER_TAG, "Already queue is initialized!");
    }
    else
    {
        // Create send message queue
        gCABleClientReceiverQueue = OICMalloc(sizeof(CAQueueingThread_t));
        if (!gCABleClientReceiverQueue)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "Memory allocation failed!");
            OICFree(gBLEClientSendQueueHandle);
            return CA_MEMORY_ALLOC_FAILED;
        }

        if (CA_STATUS_OK != CAQueueingThreadInitialize(gCABleClientReceiverQueue, gBleAdapterThreadPool,
                CABLEClientDataReceiverHandler))
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "Failed to Initialize send queue thread");
            OICFree(gBLEClientSendQueueHandle);
            OICFree(gCABleClientReceiverQueue);
            gCABleClientReceiverQueue = NULL;
            return CA_STATUS_FAILED;
        }
    }
    //gClientUp = CA_TRUE; //AMOGH

    if (CA_STATUS_OK != CAQueueingThreadStart(gCABleClientReceiverQueue))
    {
        OIC_LOG_V(ERROR, CALEADAPTER_TAG, "u_thread_pool_add_task failed ");
        OICFree(gCABleClientReceiverQueue);
        gCABleClientReceiverQueue = NULL;
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CATerminateBleQueues()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    u_mutex_lock(gBleClientSendDataMutex);
    if (NULL != gBLEClientSendQueueHandle)
    {
        CAQueueingThreadStop(gBLEClientSendQueueHandle);
        gBLEClientSendQueueHandle = NULL;
    }
    u_mutex_unlock(gBleClientSendDataMutex);

    u_mutex_lock(gBleClientReceiveDataMutex);
    if (NULL != gCABleClientReceiverQueue)
    {
        CAQueueingThreadStop(gCABleClientReceiverQueue);
        gCABleClientReceiverQueue = NULL;
    }
    u_mutex_unlock(gBleClientReceiveDataMutex);

    u_mutex_lock(gBleServerSendDataMutex);
    if (NULL != gBLEClientSendQueueHandle)
    {
        CAQueueingThreadStop(gBLEClientSendQueueHandle);
        gBLEClientSendQueueHandle = NULL;
    }
    u_mutex_unlock(gBleServerSendDataMutex);

    u_mutex_lock(gBleServerReceiveDataMutex);
    if (NULL != gCABleServerReceiverQueue)
    {
        CAQueueingThreadStop(gCABleServerReceiverQueue);
        gCABleServerReceiverQueue = NULL;
    }
    u_mutex_unlock(gBleServerReceiveDataMutex);

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
}

void CABLEServerDataReceiverHandler(void *threadData)
{
    OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "IN");

    // CAAdapterMessage_t *message = NULL;
    static const char *remoteAddress = NULL;
    static const char *serviceUUID = NULL;
    static uint32_t recvDataLen = 0;
    static uint32_t totalDataLen = 0;
    static char *defragData = NULL;
    static bool isHeaderAvailable = false;
    static CARemoteEndpoint_t *remoteEndpoint = NULL;

    u_mutex_lock(gBleClientReceiveDataMutex);

    if (gDataReceiverHandlerState)
    {
        OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "checking for DE Fragmentation");

        CABLEData *bleData = (CABLEData *) threadData;
        if (!bleData)
        {
            OIC_LOG(DEBUG, CALEADAPTER_TAG, "Invalid wifidata!");
            return;
        }

        OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "checking for DE Fragmentation");

        if (!isHeaderAvailable)
        {
            OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Parsing the header");
            char *header = (char *) OICMalloc(sizeof(char) * CA_HEADER_LENGTH);
            VERIFY_NON_NULL_VOID(header, CALEADAPTER_TAG, "header is NULL");

            memcpy(header, bleData->data, CA_HEADER_LENGTH);
            totalDataLen = CAParseHeader(header);
            OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Total data to be accumulated [%d] bytes", totalDataLen);
            OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "data received in the first packet [%d] bytes", bleData->dataLen);
            defragData = (char *) OICMalloc(sizeof(char) * totalDataLen);
            if (NULL == defragData)
            {
                OIC_LOG(ERROR, CALEADAPTER_TAG, "defragData is NULL!");
                OICFree(header);
                return;
            }

            OICFree(header);

            remoteAddress = bleData->remoteEndpoint->addressInfo.LE.leMacAddress;
            serviceUUID = bleData->remoteEndpoint->resourceUri;

            remoteEndpoint = CAAdapterCreateRemoteEndpoint(CA_EDR, remoteAddress,
                             serviceUUID);

            memcpy(defragData + recvDataLen, bleData->data + CA_HEADER_LENGTH,
                   bleData->dataLen - CA_HEADER_LENGTH);
            recvDataLen += bleData->dataLen - CA_HEADER_LENGTH;
            isHeaderAvailable = true;
        }
        else
        {
            OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Copying the data of length [%d]", bleData->dataLen);
            memcpy(defragData + recvDataLen, bleData->data, bleData->dataLen);
            recvDataLen += bleData->dataLen ;
            OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "totalDatalength  [%d] recveived Datalen [%d]",
                      totalDataLen, recvDataLen);
        }
        CAFreeBLEData(bleData);
        if (totalDataLen == recvDataLen)
        {
            u_mutex_lock(gBleAdapterReqRespCbMutex);
            if (NULL == gNetworkPacketReceivedCallback)
            {
                OIC_LOG(ERROR, CALEADAPTER_TAG, "gReqRespCallback is NULL!");
                u_mutex_unlock(gBleAdapterReqRespCbMutex);
                return;
            }
            OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Sending data up !");
            gNetworkPacketReceivedCallback(remoteEndpoint, defragData, recvDataLen);
            recvDataLen = 0;
            totalDataLen = 0;
            isHeaderAvailable = false;
            u_mutex_unlock(gBleAdapterReqRespCbMutex);
        }

        if (false == gDataReceiverHandlerState)
        {
            OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "GATTClient is terminating. Cleaning up");
            OICFree(defragData);
            OICFree(remoteEndpoint);
            u_mutex_unlock(gBleClientReceiveDataMutex);
            return;
        }
    }
    u_mutex_unlock(gBleClientReceiveDataMutex);
    OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "OUT");
}

void CABLEClientDataReceiverHandler(void *threadData)
{
    OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "IN");

    // CAAdapterMessage_t *message = NULL;
    static const char *remoteAddress = NULL;
    static const char *serviceUUID = NULL;
    static uint32_t recvDataLen = 0;
    static uint32_t totalDataLen = 0;
    static char *defragData = NULL;
    static bool isHeaderAvailable = false;
    static CARemoteEndpoint_t *remoteEndpoint = NULL;

    u_mutex_lock(gBleClientReceiveDataMutex);

    if (gDataReceiverHandlerState)
    {
        OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "checking for DE Fragmentation");

        CABLEData *bleData = (CABLEData *) threadData;
        if (!bleData)
        {
            OIC_LOG(DEBUG, CALEADAPTER_TAG, "Invalid wifidata!");
            return;
        }

        OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "checking for DE Fragmentation");

        if (!isHeaderAvailable)
        {
            OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Parsing the header");
            char *header = (char *) OICMalloc(sizeof(char) * CA_HEADER_LENGTH);
            VERIFY_NON_NULL_VOID(header, CALEADAPTER_TAG, "header is NULL");

            memcpy(header, bleData->data, CA_HEADER_LENGTH);
            totalDataLen = CAParseHeader(header);
            OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Total data to be accumulated [%d] bytes", totalDataLen);
            OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Data received in the first packet [%d] bytes", bleData->dataLen);
            defragData = (char *) OICMalloc(sizeof(char) * totalDataLen);
            if (NULL == defragData)
            {
                OIC_LOG(ERROR, CALEADAPTER_TAG, "defragData is NULL!");
                OICFree(header);
                return;
            }

            OICFree(header);

            remoteAddress = bleData->remoteEndpoint->addressInfo.LE.leMacAddress;
            serviceUUID = bleData->remoteEndpoint->resourceUri;

            remoteEndpoint = CAAdapterCreateRemoteEndpoint(CA_EDR, remoteAddress,
                             serviceUUID);

            memcpy(defragData , bleData->data + CA_HEADER_LENGTH,
                   bleData->dataLen - CA_HEADER_LENGTH);
            recvDataLen += bleData->dataLen - CA_HEADER_LENGTH;
            isHeaderAvailable = true;
        }
        else
        {
            OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Copying the data of length [%d]", bleData->dataLen);
            memcpy(defragData + recvDataLen, bleData->data, bleData->dataLen);
            recvDataLen += bleData->dataLen ;
            OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "totalDatalength  [%d] recveived Datalen [%d]",
                      totalDataLen, recvDataLen);
        }
        CAFreeBLEData(bleData);
        if (totalDataLen == recvDataLen)
        {
            u_mutex_lock(gBleAdapterReqRespCbMutex);
            if (NULL == gNetworkPacketReceivedCallback)
            {
                OIC_LOG(ERROR, CALEADAPTER_TAG, "gReqRespCallback is NULL!");
                u_mutex_unlock(gBleAdapterReqRespCbMutex);
                return;
            }
            OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Sending data up !");
            gNetworkPacketReceivedCallback(remoteEndpoint, defragData, recvDataLen);
            recvDataLen = 0;
            totalDataLen = 0;
            isHeaderAvailable = false;
            u_mutex_unlock(gBleAdapterReqRespCbMutex);
        }

        if (false == gDataReceiverHandlerState)
        {
            OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "GATTClient is terminating. Cleaning up");
            OICFree(defragData);
            OICFree(remoteEndpoint);
            u_mutex_unlock(gBleClientReceiveDataMutex);
            return;
        }
    }
    u_mutex_unlock(gBleClientReceiveDataMutex);
    OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "OUT");
}

void CABLEServerSendDataThread(void *threadData)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    CABLEData *bleData = (CABLEData *) threadData;
    if (!bleData)
    {
        OIC_LOG(DEBUG, CALEADAPTER_TAG, "Invalid bledata!");
        return;
    }
    char *header = (char *) OICMalloc(sizeof(char) * CA_HEADER_LENGTH);
    VERIFY_NON_NULL_VOID(header, CALEADAPTER_TAG, "Malloc failed");

    char *dataSegment = (char *) OICMalloc(sizeof(char) * bleData->dataLen + CA_HEADER_LENGTH);
    if (NULL == dataSegment)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Malloc failed");
        OICFree(header);
        return;
    }

    memset(header, 0x0, sizeof(char) * CA_HEADER_LENGTH );
    memset(dataSegment, 0x0, sizeof(char) * bleData->dataLen );

    CAResult_t result = CAGenerateHeader(header, bleData->dataLen);
    if (CA_STATUS_OK != result )
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Generate header failed");
        OICFree(header);
        OICFree(dataSegment);
        return ;
    }

    memcpy(dataSegment, header, CA_HEADER_LENGTH);
    OICFree(header);
    int32_t length = 0;
    if (CA_SUPPORTED_BLE_MTU_SIZE >= bleData->dataLen)
    {
        length = bleData->dataLen + CA_HEADER_LENGTH;
        OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "length [%d]", length);
        memcpy(dataSegment + CA_HEADER_LENGTH, bleData->data, bleData->dataLen);
    }
    else
    {
        length =  CA_SUPPORTED_BLE_MTU_SIZE;
        OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "length [%d]", length);
        memcpy(dataSegment + CA_HEADER_LENGTH, bleData->data,
               CA_SUPPORTED_BLE_MTU_SIZE - CA_HEADER_LENGTH);
    }

    int32_t iter = bleData->dataLen / CA_SUPPORTED_BLE_MTU_SIZE;
    int32_t index = 0;
    u_mutex_lock(gBleServerSendDataMutex);
    // Send the first segment with the header.
    result = CAUpdateCharacteristicsInGattServer(dataSegment, length);
    for (index = 1; index < iter; index++)
    {
        // Send the remaining header.
        OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Sending the chunk number [%d]", index);
        result = CAUpdateCharacteristicsInGattServer(
                     bleData->data + ((index * CA_SUPPORTED_BLE_MTU_SIZE) - CA_HEADER_LENGTH),
                     CA_SUPPORTED_BLE_MTU_SIZE);
    }
    if (bleData->dataLen / CA_SUPPORTED_BLE_MTU_SIZE)
    {
        // send the last segment of the data (Ex: 22 bytes of 622 bytes of data when MTU is 200)
        OIC_LOG(DEBUG, CALEADAPTER_TAG, "Sending the last chunk");
        result = CAUpdateCharacteristicsInGattServer(
                     bleData->data + (index * CA_SUPPORTED_BLE_MTU_SIZE) - CA_HEADER_LENGTH,
                     bleData->dataLen % CA_SUPPORTED_BLE_MTU_SIZE + CA_HEADER_LENGTH);
    }

    OICFree(bleData->remoteEndpoint);
    OICFree(bleData);
    u_mutex_unlock(gBleServerSendDataMutex); // AMOGH is this mutex required  ?

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return;

}

void CABLEClientSendDataThread(void *threadData)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    CABLEData *bleData = (CABLEData *) threadData;
    if (!bleData)
    {
        OIC_LOG(DEBUG, CALEADAPTER_TAG, "Invalid bledata!");
        return;
    }
    char *header = (char *) OICMalloc(sizeof(char) * CA_HEADER_LENGTH);
    VERIFY_NON_NULL_VOID(header, CALEADAPTER_TAG, "Malloc failed");

    char *dataSegment = (char *) OICMalloc(sizeof(char) * bleData->dataLen + CA_HEADER_LENGTH);
    if (NULL == dataSegment)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Malloc failed");
        OICFree(header);
        return;
    }

    memset(header, 0x0, sizeof(char) * CA_HEADER_LENGTH );
    memset(dataSegment, 0x0, sizeof(char) * bleData->dataLen );

    CAResult_t result = CAGenerateHeader(header, bleData->dataLen);
    if (CA_STATUS_OK != result )
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Generate header failed");
        OICFree(header);
        OICFree(dataSegment);
        return ;
    }
    memcpy(dataSegment, header, CA_HEADER_LENGTH);
    OICFree(header);
    int32_t length = 0;
    if (CA_SUPPORTED_BLE_MTU_SIZE >= bleData->dataLen)
    {
        length = bleData->dataLen + CA_HEADER_LENGTH;
        OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "length [%d]", length);
        memcpy(dataSegment + CA_HEADER_LENGTH, bleData->data, bleData->dataLen);
    }
    else
    {
        length = CA_SUPPORTED_BLE_MTU_SIZE;
        OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "length [%d]", length);
        memcpy(dataSegment + CA_HEADER_LENGTH, bleData->data,
               CA_SUPPORTED_BLE_MTU_SIZE - CA_HEADER_LENGTH);
    }

    int32_t iter = bleData->dataLen / CA_SUPPORTED_BLE_MTU_SIZE;
    int32_t index = 0;
    u_mutex_lock(gBleClientSendDataMutex);
    if (NULL != bleData->remoteEndpoint)
    {
        // Send the first segment with the header.
        result = CAUpdateCharacteristicsToGattServer(bleData->remoteEndpoint->addressInfo.LE.leMacAddress,
                 dataSegment,
                 length,
                 UNICAST, 0);
        for (index = 1; index < iter; index++)
        {
            // Send the remaining header.
            result = CAUpdateCharacteristicsToGattServer(bleData->remoteEndpoint->addressInfo.LE.leMacAddress,
                     bleData->data + (index * CA_SUPPORTED_BLE_MTU_SIZE) - CA_HEADER_LENGTH,
                     CA_SUPPORTED_BLE_MTU_SIZE,
                     UNICAST, 0);
        }
        if (bleData->dataLen / CA_SUPPORTED_BLE_MTU_SIZE)
        {
            // send the last segment of the data (Ex: 22 bytes of 622 bytes of data when MTU is 200)
            OIC_LOG(DEBUG, CALEADAPTER_TAG, "Sending the last chunk");
            result = CAUpdateCharacteristicsToGattServer(bleData->remoteEndpoint->addressInfo.LE.leMacAddress,
                     bleData->data + (index * CA_SUPPORTED_BLE_MTU_SIZE) - CA_HEADER_LENGTH,
                     bleData->dataLen % CA_SUPPORTED_BLE_MTU_SIZE + CA_HEADER_LENGTH,
                     UNICAST, 0);
        }
    }
    else
    {
        // Send the first segment with the header.
        result = CAUpdateCharacteristicsToAllGattServers(dataSegment + (index * length),
                 length);
        // Send the remaining header.
        for (index = 1; index <= iter; index++)
        {
            result = CAUpdateCharacteristicsToAllGattServers(
                         bleData->data + (index * CA_SUPPORTED_BLE_MTU_SIZE) - CA_HEADER_LENGTH,
                         CA_SUPPORTED_BLE_MTU_SIZE);
        }
        if (bleData->dataLen / CA_SUPPORTED_BLE_MTU_SIZE)
        {
            // send the last segment of the data (Ex: 22 bytes of 622 bytes of data when MTU is 200)
            OIC_LOG(DEBUG, CALEADAPTER_TAG, "Sending the last chunk");
            result = CAUpdateCharacteristicsToAllGattServers(
                         bleData->data + (index * CA_SUPPORTED_BLE_MTU_SIZE) - CA_HEADER_LENGTH,
                         bleData->dataLen % CA_SUPPORTED_BLE_MTU_SIZE + CA_HEADER_LENGTH);
        }
    }

    OICFree(bleData->remoteEndpoint);
    OICFree(bleData);
    u_mutex_unlock(gBleClientSendDataMutex);

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return;

}

CABLEData *CACreateBLEData(const CARemoteEndpoint_t *remoteEndpoint, void *data,
                           uint32_t dataLength)
{
    CABLEData *bleData = (CABLEData *) OICMalloc(sizeof(CABLEData));
    if (!bleData)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Memory allocation failed!");
        return NULL;
    }

    bleData->remoteEndpoint = CAAdapterCopyRemoteEndpoint(remoteEndpoint);
    bleData->data = (void *)OICMalloc(dataLength);
    if (NULL == bleData->data)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Memory allocation failed!");
        CAFreeBLEData(bleData);
        return NULL;
    }
    memcpy(bleData->data, data, dataLength);
    bleData->dataLen = dataLength;

    return bleData;
}

void CAFreeBLEData(CABLEData *bleData)
{
    if (!bleData)
        return;

    CAAdapterFreeRemoteEndpoint(bleData->remoteEndpoint);
    OICFree(bleData->data);
    OICFree(bleData);
}

CAResult_t CABLEClientSendData(const CARemoteEndpoint_t *remoteEndpoint,
                               void *data,
                               uint32_t dataLen)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN ");

    VERIFY_NON_NULL(data, NULL, "Param data is NULL");

    VERIFY_NON_NULL_RET(gBLEClientSendQueueHandle, CALEADAPTER_TAG,
                        "gBLEClientSendQueueHandle is  NULL",
                        CA_STATUS_FAILED);
    VERIFY_NON_NULL_RET(gBleClientSendDataMutex, CALEADAPTER_TAG,
                        "gBleClientSendDataMutex is NULL",
                        CA_STATUS_FAILED);

    VERIFY_NON_NULL_RET(gBLEClientSendQueueHandle, CALEADAPTER_TAG, "gBLEClientSendQueueHandle",
                        CA_STATUS_FAILED);

    OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Data Sending to LE layer [%d]", dataLen);

    CABLEData *bleData = CACreateBLEData(remoteEndpoint, data, dataLen);
    if (!bleData)
    {
        OIC_LOG_V(ERROR, CALEADAPTER_TAG, "Failed to create bledata!");
        return CA_MEMORY_ALLOC_FAILED;
    }
    // Add message to send queue
    u_mutex_lock(gBleClientSendDataMutex);
    CAQueueingThreadAddData(gBLEClientSendQueueHandle, bleData, sizeof(CABLEData));
    u_mutex_unlock(gBleClientSendDataMutex);

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT ");
    return CA_STATUS_OK;
}


CAResult_t CABLEServerSendData(const CARemoteEndpoint_t *remoteEndpoint,
                               void *data,
                               uint32_t dataLen)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN ");

    VERIFY_NON_NULL(data, NULL, "Param data is NULL");

    VERIFY_NON_NULL_RET(gSendQueueHandle, CALEADAPTER_TAG,
                        "BleClientReceiverQueue is  NULL",
                        CA_STATUS_FAILED);
    VERIFY_NON_NULL_RET(gBleClientSendDataMutex, CALEADAPTER_TAG,
                        "BleClientSendDataMutex is NULL",
                        CA_STATUS_FAILED);

    VERIFY_NON_NULL_RET(gSendQueueHandle, CALEADAPTER_TAG, "sendQueueHandle",
                        CA_STATUS_FAILED);

    OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Data Sending to LE layer [%d]", dataLen);

    CABLEData *bleData = CACreateBLEData(remoteEndpoint, data, dataLen);
    if (!bleData)
    {
        OIC_LOG_V(ERROR, CALEADAPTER_TAG, "Failed to create bledata!");
        return CA_MEMORY_ALLOC_FAILED;
    }
    // Add message to send queue
    u_mutex_lock(gBleServerSendDataMutex);
    CAQueueingThreadAddData(gSendQueueHandle, bleData, sizeof(CABLEData));
    u_mutex_unlock(gBleServerSendDataMutex);

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT ");
    return CA_STATUS_OK;
}

CAResult_t CABLEServerReceivedData(const char *remoteAddress, const char *serviceUUID,
                                   void *data, uint32_t dataLength, uint32_t *sentLength)
{
    OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "IN");

    //Input validation
    VERIFY_NON_NULL(serviceUUID, CALEADAPTER_TAG, "service UUID is null");
    VERIFY_NON_NULL(data, CALEADAPTER_TAG, "Data is null");
    VERIFY_NON_NULL(sentLength, CALEADAPTER_TAG, "Sent data length holder is null");
    VERIFY_NON_NULL_RET(gCABleServerReceiverQueue, CALEADAPTER_TAG, "gCABleServerReceiverQueue",
                        CA_STATUS_FAILED);

    //Add message to data queue
    CARemoteEndpoint_t *remoteEndpoint = CAAdapterCreateRemoteEndpoint(CA_LE, remoteAddress,
                                         serviceUUID);
    if (NULL == remoteEndpoint)
    {
        OIC_LOG_V(ERROR, CALEADAPTER_TAG, "Failed to create remote endpoint !");
        return CA_STATUS_FAILED;
    }

    // Create bleData to add to queue
    OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Data received from LE layer [%d]", dataLength);

    CABLEData *bleData = CACreateBLEData(remoteEndpoint, data, dataLength);
    if (!bleData)
    {
        OIC_LOG_V(ERROR, CALEADAPTER_TAG, "Failed to create bledata!");
        return CA_MEMORY_ALLOC_FAILED;
    }

    // Add message to send queue
    CAQueueingThreadAddData(gCABleServerReceiverQueue, bleData, sizeof(CABLEData));

    *sentLength = dataLength;

    OICFree(data);
    data = NULL;

    OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CABLEClientReceivedData(const char *remoteAddress, const char *serviceUUID,
                                   void *data, uint32_t dataLength, uint32_t *sentLength)
{
    OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "IN");

    //Input validation
    VERIFY_NON_NULL(serviceUUID, CALEADAPTER_TAG, "service UUID is null");
    VERIFY_NON_NULL(data, CALEADAPTER_TAG, "Data is null");
    VERIFY_NON_NULL(sentLength, CALEADAPTER_TAG, "Sent data length holder is null");
    VERIFY_NON_NULL_RET(gCABleClientReceiverQueue, CALEADAPTER_TAG, "gCABleClientReceiverQueue",
                        CA_STATUS_FAILED);

    //Add message to data queue
    CARemoteEndpoint_t *remoteEndpoint = CAAdapterCreateRemoteEndpoint(CA_LE, remoteAddress,
                                         serviceUUID);
    if (NULL == remoteEndpoint)
    {
        OIC_LOG_V(ERROR, CALEADAPTER_TAG, "Failed to create remote endpoint !");
        return CA_STATUS_FAILED;
    }

    OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Data received from LE layer [%d]", dataLength);

    // Create bleData to add to queue
    CABLEData *bleData = CACreateBLEData(remoteEndpoint, data, dataLength);
    if (!bleData)
    {
        OIC_LOG_V(ERROR, CALEADAPTER_TAG, "Failed to create bledata!");
        return CA_MEMORY_ALLOC_FAILED;
    }

    // Add message to send queue
    CAQueueingThreadAddData(gCABleClientReceiverQueue, bleData, sizeof(CABLEData));

    *sentLength = dataLength;

    OICFree(data);
    data = NULL;

    OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CASetBleAdapterThreadPoolHandle(u_thread_pool_t handle)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    u_mutex_unlock(gBleAdapterThreadPoolMutex);
    gBleAdapterThreadPool = handle;
    u_mutex_unlock(gBleAdapterThreadPoolMutex);

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
}

void CASetBLEReqRespAdapterCallback(CANetworkPacketReceivedCallback callback)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    u_mutex_lock(gBleAdapterReqRespCbMutex);

    gNetworkPacketReceivedCallback = callback;

    u_mutex_unlock(gBleAdapterReqRespCbMutex);

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
}


#endif //#ifdef OIC_TIZEN

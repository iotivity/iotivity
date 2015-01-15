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

#include <stdio.h>
#include <stdlib.h>

#include "caleinterface.h"
#include "cacommon.h"
#include "umutex.h"
#include "caadapterutils.h"
#include "caqueueingthread.h"
#include "camsgparser.h"


/**
 * @var CALEADAPTER_TAG
 * @brief Logging tag for module name.
 */
#define CALEADAPTER_TAG "CA_BLE_ADAPTER"

/**
 * @var gNetworkCallback
 * @brief Callback to provide the status of the network change to CA layer.
 */
static CANetworkChangeCallback gNetworkCallback = NULL;

/**
 * @var gLocalBLEAddress
 * @brief bleAddress of the local adapter. Value will be initialized to zero, and will be updated later.
 */
static char gLocalBLEAddress[16] = { 0, };

/**
 * @var gIsServer
 * @brief Variable to differentiate btw GattServer and GattClient.
 */
static CABool_t gIsServer = CA_FALSE;

/**
 * @var gBleIsServerMutex
 * @brief Mutex to synchronize the task to be executed on the GattServer function calls.
 */
static u_mutex gBleIsServerMutex = NULL;

/**
 * @var gBleNetworkCbMutex
 * @brief Mutex to synchronize the callback to be called for the network changes.
 */
static u_mutex gBleNetworkCbMutex = NULL;

/**
 * @var gBleLocalAddressMutex
 * @brief Mutex to synchronize the updation of the local LE address of the adapter.
 */
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

/**
 * @var gBleAdapterReqRespCbMutex
 * @brief Mutex to synchronize the callback to be called for the adapterReqResponse.
 */
static u_mutex gBleAdapterReqRespCbMutex = NULL;

/**
 * @var gNetworkPacketReceivedCallback
 * @brief Callback to be called when the network packet recieved from either GattServer or GattClient.
 */
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

/**
 * @var gLeServerStatus
 * @brief structure to maintain the status of the server.
 */
static CALeServerStatus gLeServerStatus = CA_SERVER_NOTSTARTED;

/**
* @fn  CALERegisterNetworkNotifications
* @brief  This function is used to register network change notification callback.
*
* @param[in]  netCallback CANetworkChangeCallback callback which will be set for the change in nwk.
*
* @return  0 for success.
*
*/
int32_t CALERegisterNetworkNotifications(CANetworkChangeCallback netCallback);

/**
* @fn  CASetBleAdapterThreadPoolHandle
* @brief  Used to Set the gThreadPool handle which is required for spawning new thread.
*
* @param[in] handle - Thread pool handle which is given by above layer for using thread creation task.
*
* @return  void
*
*/
void CASetBleAdapterThreadPoolHandle(u_thread_pool_t handle);

/**
* @fn  CALEDeviceStateChangedCb
* @brief  This function is used to call the callback to the upper layer when the device state gets changed.
*
* @param[in]  adapter_state New state of the adapter to be notified to the upper layer.
*
* @return  None.
*
*/
void CALEDeviceStateChangedCb( CAAdapterState_t adapter_state);

/**
* @fn  CAInitBleAdapterMutex
* @brief  Used to initialize all required mutex variable for LE Adapter implementation.
*
* @return  0 on success otherwise a positive error value.
* @retval  CA_STATUS_OK  Successful
* @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
* @retval  CA_STATUS_FAILED Operation failed
*
*/
CAResult_t CAInitBleAdapterMutex();

/**
* @fn  CATermiateBleAdapterMutex
* @brief  Used to terminate all required mutex variable for LE adapter implementation.
*
* @return  void
*
*/
void CATermiateBleAdapterMutex();

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

    result = CAInitializeLENwkMonitor();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG_V(ERROR, CALEADAPTER_TAG, "CAInitializeLENwkMonitor() failed");
        return CA_STATUS_FAILED;
    }

    CAInitializeLEAdapter();

    CASetBleServerThreadPoolHandle(handle);
    CASetBleClientThreadPoolHandle(handle);
    CASetBleAdapterThreadPoolHandle(handle);
    CASetBLEReqRespServerCallback(CABLEServerReceivedData);
    CASetBLEReqRespClientCallback(CABLEClientReceivedData);
    CASetBLEReqRespAdapterCallback(reqRespCallback);

    CALERegisterNetworkNotifications(netCallback);

    CAConnectivityHandler_t connHandler;
    connHandler.startAdapter = NULL;
    connHandler.stopAdapter = CAStopLE;
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

CAResult_t CAStopLE()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    CAStopBleQueues();

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

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");

    return CA_STATUS_OK;
}

void CATerminateLE()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    CASetBLEReqRespServerCallback(NULL);
    CASetBLEReqRespClientCallback(NULL);
    CALERegisterNetworkNotifications(NULL);
    CASetBLEReqRespAdapterCallback(NULL);
    CATerminateLENwkMonitor();

    u_mutex_lock(gBleIsServerMutex);
    if (CA_TRUE == gIsServer)
    {
        CATerminateBleGattServer();
    }
    else
    {
        CATerminateBLEGattClient();
    }
    u_mutex_unlock(gBleIsServerMutex);

    CATerminateBleQueues();

    CATermiateBleAdapterMutex();

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return;
}

CAResult_t CAStartLEListeningServer()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    CAResult_t result = CAInitBleServerQueues();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "CAInitBleClientQueues failed");
        return CA_STATUS_FAILED;
    }

    result = CAGetLEAdapterState();
    if (CA_ADAPTER_NOT_ENABLED == result)
    {
        gLeServerStatus = CA_LISTENING_SERVER;
        OIC_LOG(DEBUG, CALEADAPTER_TAG, "Listen Server will be started once BT Adapter is enabled");
        return CA_STATUS_OK;
    }

    if (CA_STATUS_FAILED == result)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Bluetooth get state failed!");
        return CA_STATUS_FAILED;
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

    CAResult_t result = CAInitBleClientQueues();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "CAInitBleClientQueues failed");
        return CA_STATUS_FAILED;
    }

    result = CAGetLEAdapterState();
    if (CA_ADAPTER_NOT_ENABLED == result)
    {
        gLeServerStatus = CA_DISCOVERY_SERVER;
        OIC_LOG(DEBUG, CALEADAPTER_TAG, "Listen Server will be started once BT Adapter is enabled");
        return CA_STATUS_OK;
    }

    if (CA_STATUS_FAILED == result)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Bluetooth get state failed!");
        return CA_STATUS_FAILED;
    }

    CAStartBLEGattClient();

    u_mutex_lock(gBleIsServerMutex);
    gIsServer = CA_FALSE;
    u_mutex_unlock(gBleIsServerMutex);

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
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

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return dataLen;
}

CAResult_t CAGetLEInterfaceInformation(CALocalConnectivity_t **info, uint32_t *size)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    VERIFY_NON_NULL(info, NULL, "CALocalConnectivity info is null");

    char *local_address = NULL;

    CAGetLEAddress(&local_address);
    if (NULL == local_address)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Get local bt adapter address failed");
        return CA_STATUS_FAILED;
    }

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

    if (netCallback)
    {
        CAResult_t res = CASetLEAdapterStateChangedCb(CALEDeviceStateChangedCb);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "CASetLEAdapterStateChangedCb failed!");
        }
    }
    else
    {
        CAResult_t res = CAUnSetLEAdapterStateChangedCb();
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "CASetLEAdapterStateChangedCb failed!");
        }
    }

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CALEDeviceStateChangedCb( CAAdapterState_t adapter_state)
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
    return;
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

void CATermiateBleAdapterMutex()
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
    return;
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
            CABLEServerSendDataThread, NULL))
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
            CABLEClientSendDataThread, NULL))
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
            CABLEServerDataReceiverHandler, NULL))
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
                CABLEClientDataReceiverHandler, NULL))
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "Failed to Initialize send queue thread");
            OICFree(gBLEClientSendQueueHandle);
            OICFree(gCABleClientReceiverQueue);
            gCABleClientReceiverQueue = NULL;
            return CA_STATUS_FAILED;
        }
    }
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

void CAStopBleQueues()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    u_mutex_lock(gBleClientSendDataMutex);
    if (NULL != gBLEClientSendQueueHandle)
    {
        CAQueueingThreadStop(gBLEClientSendQueueHandle);
    }
    u_mutex_unlock(gBleClientSendDataMutex);

    u_mutex_lock(gBleClientReceiveDataMutex);
    if (NULL != gCABleClientReceiverQueue)
    {
        CAQueueingThreadStop(gCABleClientReceiverQueue);
    }
    u_mutex_unlock(gBleClientReceiveDataMutex);

    u_mutex_lock(gBleServerSendDataMutex);
    if (NULL != gSendQueueHandle)
    {
        CAQueueingThreadStop(gSendQueueHandle);
    }
    u_mutex_unlock(gBleServerSendDataMutex);

    u_mutex_lock(gBleServerReceiveDataMutex);
    if (NULL != gCABleServerReceiverQueue)
    {
        CAQueueingThreadStop(gCABleServerReceiverQueue);
    }
    u_mutex_unlock(gBleServerReceiveDataMutex);

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
}

void CATerminateBleQueues()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    CAQueueingThreadDestroy(gBLEClientSendQueueHandle);
    OICFree(gBLEClientSendQueueHandle);
    gBLEClientSendQueueHandle = NULL;


    CAQueueingThreadDestroy(gCABleClientReceiverQueue);
    OICFree(gCABleClientReceiverQueue);
    gCABleClientReceiverQueue = NULL;


    CAQueueingThreadDestroy(gSendQueueHandle);
    OICFree(gSendQueueHandle);
    gSendQueueHandle = NULL;


    CAQueueingThreadDestroy(gCABleServerReceiverQueue);
    OICFree(gCABleServerReceiverQueue);
    gCABleServerReceiverQueue = NULL;

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

            remoteEndpoint = CAAdapterCreateRemoteEndpoint(CA_LE, remoteAddress,
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

            remoteEndpoint = CAAdapterCreateRemoteEndpoint(CA_LE, remoteAddress,
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
    u_mutex_unlock(gBleServerSendDataMutex); // TODO: this mutex required  ?

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

    char *dataSegment = (char *) OICMalloc(sizeof(char) * bleData->dataLen + CA_HEADER_LENGTH + 1);
    if (NULL == dataSegment)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Malloc failed");
        OICFree(header);
        return;
    }

    memset(header, 0x0, sizeof(char) * CA_HEADER_LENGTH );
    memset(dataSegment, 0x0, sizeof(char) * bleData->dataLen + CA_HEADER_LENGTH + 1);

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
        for (index = 1; index < iter; index++)
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
        CAAdapterFreeRemoteEndpoint(remoteEndpoint);
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
        CAAdapterFreeRemoteEndpoint(remoteEndpoint);
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

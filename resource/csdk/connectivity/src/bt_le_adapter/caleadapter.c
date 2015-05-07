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
#include "camutex.h"
#include "caadapterutils.h"
#include "caqueueingthread.h"
#include "camsgparser.h"
#include "oic_malloc.h"

/**
 * @var CALEADAPTER_TAG
 * @brief Logging tag for module name.
 */
#define CALEADAPTER_TAG "CA_BLE_ADAPTER"

/**
 * @var g_networkCallback
 * @brief Callback to provide the status of the network change to CA layer.
 */
static CANetworkChangeCallback g_networkCallback = NULL;

/**
 * @var g_localBLEAddress
 * @brief bleAddress of the local adapter. Value will be initialized to zero, and will be updated later.
 */
static char g_localBLEAddress[16] = {0};

/**
 * @var g_isServer
 * @brief Variable to differentiate btw GattServer and GattClient.
 */
static bool g_isServer = false;

/**
 * @var g_bleIsServerMutex
 * @brief Mutex to synchronize the task to be executed on the GattServer function calls.
 */
static ca_mutex g_bleIsServerMutex = NULL;

/**
 * @var g_bleNetworkCbMutex
 * @brief Mutex to synchronize the callback to be called for the network changes.
 */
static ca_mutex g_bleNetworkCbMutex = NULL;

/**
 * @var g_bleLocalAddressMutex
 * @brief Mutex to synchronize the updation of the local LE address of the adapter.
 */
static ca_mutex g_bleLocalAddressMutex = NULL;

/**
 * @var g_bleAdapterThreadPool
 * @brief reference to threadpool
 */
static ca_thread_pool_t g_bleAdapterThreadPool = NULL;

/**
 * @var g_bleAdapterThreadPoolMutex
 * @brief Mutex to synchronize the task to be pushed to thread pool.
 */
static ca_mutex g_bleAdapterThreadPoolMutex = NULL;

/**
 * @var g_bleClientSendQueueHandle
 * @brief Queue to process the outgoing packets from GATTClient.
 */
static CAQueueingThread_t *g_bleClientSendQueueHandle = NULL;

/**
 * @var g_bleClientReceiverQueue
 * @brief Queue to process the incoming packets to GATT Client.
 */
static CAQueueingThread_t *g_bleClientReceiverQueue = NULL;

/**
 * @var g_bleClientSendDataMutex
 * @brief Mutex to synchronize the queing of the data from SenderQueue.
 */
static ca_mutex g_bleClientSendDataMutex = NULL;

/**
 * @var g_bleClientReceiveDataMutex
 * @brief Mutex to synchronize the queing of the data from ReceiverQueue.
 */
static ca_mutex g_bleClientReceiveDataMutex = NULL;

/**
 * @var g_dataReceiverHandlerState
 * @brief Stop condition of recvhandler.
 */
static bool g_dataReceiverHandlerState = false;

/**
 * @var g_sendQueueHandle
 * @brief Queue to process the outgoing packets from GATTServer.
 */
static CAQueueingThread_t *g_sendQueueHandle = NULL;

/**
 * @var g_bleServerReceiverQueue
 * @brief Queue to process the incoming packets to GATTServer
 */
static CAQueueingThread_t *g_bleServerReceiverQueue = NULL;

/**
 * @var g_bleServerSendDataMutex
 * @brief Mutex to synchronize the queing of the data from SenderQueue.
 */
static ca_mutex g_bleServerSendDataMutex = NULL;

/**
 * @var g_bleServerReceiveDataMutex
 * @brief Mutex to synchronize the queing of the data from ReceiverQueue.
 */
static ca_mutex g_bleServerReceiveDataMutex = NULL;

/**
 * @var g_bleAdapterReqRespCbMutex
 * @brief Mutex to synchronize the callback to be called for the adapterReqResponse.
 */
static ca_mutex g_bleAdapterReqRespCbMutex = NULL;

/**
 * @var g_networkPacketReceivedCallback
 * @brief Callback to be called when network packet recieved from either GattServer or GattClient.
 */
static CANetworkPacketReceivedCallback g_networkPacketReceivedCallback = NULL;

/**
 * @ENUM CALeServerStatus
 * @brief status of BLE Server Status
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
* @return  0 on success otherwise a positive error value.
* @retval  CA_STATUS_OK  Successful
* @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
* @retval  CA_STATUS_FAILED Operation failed
*
*/
CAResult_t CALERegisterNetworkNotifications(CANetworkChangeCallback netCallback);

/**
* @fn  CASetBleAdapterThreadPoolHandle
* @brief  Used to Set the gThreadPool handle which is required for spawning new thread.
*
* @param[in] handle - Thread pool handle which is given by above layer for using thread creation task.
*
* @return  void
*
*/
void CASetBleAdapterThreadPoolHandle(ca_thread_pool_t handle);

/**
* @fn  CALEDeviceStateChangedCb
* @brief  This function is used to call the callback to the upper layer when the device state gets
*         changed.
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
* @fn  CATerminateBleAdapterMutex
* @brief  Used to terminate all required mutex variable for LE adapter implementation.
*
* @return  void
*/
void CATerminateBleAdapterMutex();

/**
* @fn  CALEDataDestroyer
* @brief  Used to free data
*
* @return  void
*/
static void CALEDataDestroyer(void *data, uint32_t size);

CAResult_t CAInitializeLE(CARegisterConnectivityCallback registerCallback,
                          CANetworkPacketReceivedCallback reqRespCallback,
                          CANetworkChangeCallback netCallback,
                          ca_thread_pool_t handle)
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

    result = CAInitializeLENetworkMonitor();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "CAInitializeLENetworkMonitor() failed");
        return CA_STATUS_FAILED;
    }

    CAInitializeLEAdapter();

    CASetBleClientThreadPoolHandle(handle);
    CASetBleServerThreadPoolHandle(handle);
    CASetBleAdapterThreadPoolHandle(handle);
    CASetBLEReqRespServerCallback(CABLEServerReceivedData);
    CASetBLEReqRespClientCallback(CABLEClientReceivedData);
    CASetBLEReqRespAdapterCallback(reqRespCallback);

    CALERegisterNetworkNotifications(netCallback);

    CAConnectivityHandler_t connHandler;
    connHandler.startAdapter = CAStartLE;
    connHandler.stopAdapter = CAStopLE;
    connHandler.startListenServer = CAStartLEListeningServer;
    connHandler.startDiscoveryServer = CAStartLEDiscoveryServer;
    connHandler.sendData = CASendLEUnicastData;
    connHandler.sendDataToAll = CASendLEMulticastData;
    connHandler.GetnetInfo = CAGetLEInterfaceInformation;
    connHandler.readData = CAReadLEData;
    connHandler.terminate = CATerminateLE;
    registerCallback(connHandler, CA_LE);

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");

    return CA_STATUS_OK;
}

CAResult_t CAStartLE()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");
    OIC_LOG(DEBUG, CALEADAPTER_TAG,
        "There is no concept of unicast/multicast in LE. So This function is not implemented");
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAStopLE()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    CAStopBleQueues();

    ca_mutex_lock(g_bleIsServerMutex);
    if (true == g_isServer)
    {
        CAStopBleGattServer();
    }
    else
    {
        CAStopBLEGattClient();
    }
    ca_mutex_unlock(g_bleIsServerMutex);

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
    CATerminateLENetworkMonitor();

    ca_mutex_lock(g_bleIsServerMutex);
    if (true == g_isServer)
    {
        CATerminateBleGattServer();
    }
    else
    {
        CATerminateBLEGattClient();
    }
    ca_mutex_unlock(g_bleIsServerMutex);

    CATerminateBleQueues();

    CATerminateBleAdapterMutex();

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
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

    ca_mutex_lock(g_bleIsServerMutex);
    g_isServer = true;
    ca_mutex_unlock(g_bleIsServerMutex);

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
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

    ca_mutex_lock(g_bleIsServerMutex);
    g_isServer = false;
    ca_mutex_unlock(g_bleIsServerMutex);

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAStartLENotifyServer()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

uint32_t CASendLENotification(const CARemoteEndpoint_t *endpoint, const void *data, uint32_t dataLen)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return 0;
}

CAResult_t CAReadLEData()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

int32_t CASendLEUnicastData(const CARemoteEndpoint_t *endpoint, const void *data, uint32_t dataLen)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    //Input validation
    VERIFY_NON_NULL_RET(endpoint, NULL, "Remote endpoint is null", -1);
    VERIFY_NON_NULL_RET(data, NULL, "Data is null", -1);

    CAResult_t result = CA_STATUS_FAILED;

    ca_mutex_lock(g_bleIsServerMutex);
    if (true  == g_isServer)
    {
        result = CABLEServerSendData(endpoint, data, dataLen);
        if (CA_STATUS_OK != result)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG,
                    "[SendLEUnicastData] CABleServerSenderQueueEnqueueMessage failed \n");
            ca_mutex_unlock(g_bleIsServerMutex);
            return -1;
        }
    }
    else
    {
        result = CABLEClientSendData(endpoint, data, dataLen);
        if (CA_STATUS_OK != result)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG,
                    "[SendLEUnicastData] CABleClientSenderQueueEnqueueMessage failed \n");
            ca_mutex_unlock(g_bleIsServerMutex);
            return -1;
        }
    }
    ca_mutex_unlock(g_bleIsServerMutex);

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return dataLen;
}

int32_t CASendLEMulticastData(const void *data, uint32_t dataLen)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    //Input validation
    VERIFY_NON_NULL_RET(data, NULL, "Data is null", -1);

    if (0 >= dataLen)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Invalid Parameter");
        return -1;
    }

    CAResult_t result = CA_STATUS_FAILED;

    ca_mutex_lock(g_bleIsServerMutex);
    if (true  == g_isServer)
    {
        result = CABLEServerSendData(NULL, data, dataLen);
        if (CA_STATUS_OK != result)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG,
                    "[SendLEMulticastDataToAll] CABleServerSenderQueueEnqueueMessage failed" );
            ca_mutex_unlock(g_bleIsServerMutex);
            return -1;
        }
    }
    else
    {
        result = CABLEClientSendData(NULL, data, dataLen);
        if (CA_STATUS_OK != result)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG,
                    "[SendLEMulticastDataToAll] CABleClientSenderQueueEnqueueMessage failed" );
            ca_mutex_unlock(g_bleIsServerMutex);
            return -1;
        }
    }
    ca_mutex_unlock(g_bleIsServerMutex);

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return dataLen;
}

CAResult_t CAGetLEInterfaceInformation(CALocalConnectivity_t **info, uint32_t *size)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    VERIFY_NON_NULL(info, NULL, "CALocalConnectivity info is null");

    char *local_address = NULL;

    CAResult_t res = CAGetLEAddress(&local_address);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "CAGetLEAddress has failed");
        return res;
    }

    if (NULL == local_address)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "local_address is NULL");
        return CA_STATUS_FAILED;
    }

    *size = 0;
    (*info) = (CALocalConnectivity_t *) OICCalloc(1, sizeof(CALocalConnectivity_t));
    if (NULL == (*info))
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Malloc failure!");
        OICFree(local_address);
        return CA_STATUS_FAILED;
    }

    size_t local_address_len = strlen(local_address);

    if(local_address_len >= sizeof(g_localBLEAddress) ||
            local_address_len >= sizeof((*info)->addressInfo.BT.btMacAddress))
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "local_address is too long");
        OICFree(*info);
        OICFree(local_address);
        return CA_STATUS_FAILED;
    }

    strncpy((*info)->addressInfo.BT.btMacAddress, local_address,
            sizeof((*info)->addressInfo.BT.btMacAddress) - 1);
    (*info)->addressInfo.BT.btMacAddress[sizeof((*info)->addressInfo.BT.btMacAddress)-1] = '\0';
    ca_mutex_lock(g_bleLocalAddressMutex);
    strncpy(g_localBLEAddress, local_address, sizeof(g_localBLEAddress) - 1);
    g_localBLEAddress[sizeof(g_localBLEAddress)-1] = '\0';
    ca_mutex_unlock(g_bleLocalAddressMutex);

    (*info)->type = CA_LE;
    *size = 1;
    OICFree(local_address);

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CALERegisterNetworkNotifications(CANetworkChangeCallback netCallback)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    ca_mutex_lock(g_bleNetworkCbMutex);
    g_networkCallback = netCallback;
    ca_mutex_unlock(g_bleNetworkCbMutex);
    CAResult_t res = CA_STATUS_OK;
    if (netCallback)
    {
        res = CASetLEAdapterStateChangedCb(CALEDeviceStateChangedCb);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "CASetLEAdapterStateChangedCb failed!");
        }
    }
    else
    {
        res = CAUnSetLEAdapterStateChangedCb();
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "CASetLEAdapterStateChangedCb failed!");
        }
    }

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return res;
}

void CALEDeviceStateChangedCb( CAAdapterState_t adapter_state)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    VERIFY_NON_NULL_VOID(g_localBLEAddress, NULL, "g_localBLEAddress is null");
    CALocalConnectivity_t localEndpoint = {};

    ca_mutex_lock(g_bleLocalAddressMutex);
    strncpy(localEndpoint.addressInfo.BT.btMacAddress, g_localBLEAddress, strlen(g_localBLEAddress));
    ca_mutex_unlock(g_bleLocalAddressMutex);

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

    ca_mutex_lock(g_bleNetworkCbMutex);
    if (NULL != g_networkCallback)
    {
        g_networkCallback(&localEndpoint, adapter_state);
    }
    else
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "g_networkCallback is NULL");
    }
    ca_mutex_unlock(g_bleNetworkCbMutex);

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
}

CAResult_t CAInitBleAdapterMutex()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    if (NULL == g_bleIsServerMutex)
    {
        g_bleIsServerMutex = ca_mutex_new();
        if (NULL == g_bleIsServerMutex)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "ca_mutex_new failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == g_bleNetworkCbMutex)
    {
        g_bleNetworkCbMutex = ca_mutex_new();
        if (NULL == g_bleNetworkCbMutex)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "ca_mutex_new failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == g_bleLocalAddressMutex)
    {
        g_bleLocalAddressMutex = ca_mutex_new();
        if (NULL == g_bleLocalAddressMutex)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "ca_mutex_new failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == g_bleAdapterThreadPoolMutex)
    {
        g_bleAdapterThreadPoolMutex = ca_mutex_new();
        if (NULL == g_bleAdapterThreadPoolMutex)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "ca_mutex_new failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == g_bleClientSendDataMutex)
    {
        g_bleClientSendDataMutex = ca_mutex_new();
        if (NULL == g_bleClientSendDataMutex)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "ca_mutex_new failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == g_bleClientReceiveDataMutex)
    {
        g_bleClientReceiveDataMutex = ca_mutex_new();
        if (NULL == g_bleClientReceiveDataMutex)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "ca_mutex_new failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == g_bleServerSendDataMutex)
    {
        g_bleServerSendDataMutex = ca_mutex_new();
        if (NULL == g_bleServerSendDataMutex)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "ca_mutex_new failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == g_bleServerReceiveDataMutex)
    {
        g_bleServerReceiveDataMutex = ca_mutex_new();
        if (NULL == g_bleServerReceiveDataMutex)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "ca_mutex_new failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == g_bleAdapterReqRespCbMutex)
    {
        g_bleAdapterReqRespCbMutex = ca_mutex_new();
        if (NULL == g_bleAdapterReqRespCbMutex)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "ca_mutex_new failed");
            return CA_STATUS_FAILED;
        }
    }

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");
    return CA_STATUS_OK;
}

void CATerminateBleAdapterMutex()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    ca_mutex_free(g_bleIsServerMutex);
    g_bleIsServerMutex = NULL;

    ca_mutex_free(g_bleNetworkCbMutex);
    g_bleNetworkCbMutex = NULL;

    ca_mutex_free(g_bleLocalAddressMutex);
    g_bleLocalAddressMutex = NULL;

    ca_mutex_free(g_bleAdapterThreadPoolMutex);
    g_bleAdapterThreadPoolMutex = NULL;

    ca_mutex_free(g_bleClientSendDataMutex);
    g_bleClientSendDataMutex = NULL;

    ca_mutex_free(g_bleClientReceiveDataMutex);
    g_bleClientReceiveDataMutex = NULL;

    ca_mutex_free(g_bleServerSendDataMutex);
    g_bleServerSendDataMutex = NULL;

    ca_mutex_free(g_bleServerReceiveDataMutex);
    g_bleServerReceiveDataMutex = NULL;

    ca_mutex_free(g_bleAdapterReqRespCbMutex);
    g_bleAdapterReqRespCbMutex = NULL;

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
}

void CAInitBleQueues()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    CAResult_t result = CAInitBleServerQueues();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "CAInitBleServerQueues failed");
        return;
    }

    result = CAInitBleClientQueues();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "CAInitBleClientQueues failed");
        return;
    }

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
}

CAResult_t CAInitBleServerQueues()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    ca_mutex_lock(g_bleAdapterThreadPoolMutex);

    CAResult_t result = CAInitBleServerSenderQueue();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "CAInitBleServerSenderQueue failed");
        ca_mutex_unlock(g_bleAdapterThreadPoolMutex);
        return CA_STATUS_FAILED;
    }

    result = CAInitBleServerReceiverQueue();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "CAInitBleServerReceiverQueue failed");
        ca_mutex_unlock(g_bleAdapterThreadPoolMutex);
        return CA_STATUS_FAILED;
    }

    g_dataReceiverHandlerState = true;

    ca_mutex_unlock(g_bleAdapterThreadPoolMutex);

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAInitBleClientQueues()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    ca_mutex_lock(g_bleAdapterThreadPoolMutex);

    CAResult_t result = CAInitBleClientSenderQueue();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "CAInitBleClientSenderQueue failed");
        ca_mutex_unlock(g_bleAdapterThreadPoolMutex);
        return CA_STATUS_FAILED;
    }

    result = CAInitBleClientReceiverQueue();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "CAInitBleClientReceiverQueue failed");
        ca_mutex_unlock(g_bleAdapterThreadPoolMutex);
        return CA_STATUS_FAILED;
    }

    g_dataReceiverHandlerState = true;

    ca_mutex_unlock(g_bleAdapterThreadPoolMutex);

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAInitBleServerSenderQueue()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");
    // Check if the message queue is already initialized
    if (g_sendQueueHandle)
    {
        OIC_LOG(DEBUG, CALEADAPTER_TAG, "Queue is already initialized!");
        return CA_STATUS_OK;
    }

    // Create send message queue
    g_sendQueueHandle = (CAQueueingThread_t *) OICMalloc(sizeof(CAQueueingThread_t));
    if (!g_sendQueueHandle)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Memory allocation failed!");
        return CA_MEMORY_ALLOC_FAILED;
    }

    if (CA_STATUS_OK != CAQueueingThreadInitialize(g_sendQueueHandle, g_bleAdapterThreadPool,
            CABLEServerSendDataThread, CALEDataDestroyer))
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Failed to Initialize send queue thread");
        OICFree(g_sendQueueHandle);
        g_sendQueueHandle = NULL;
        return CA_STATUS_FAILED;
    }

    if (CA_STATUS_OK != CAQueueingThreadStart(g_sendQueueHandle))
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "ca_thread_pool_add_task failed ");
        OICFree(g_sendQueueHandle);
        g_sendQueueHandle = NULL;
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAInitBleClientSenderQueue()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    if (g_bleClientSendQueueHandle)
    {
        OIC_LOG(DEBUG, CALEADAPTER_TAG, "Already queue is initialized!");
        return CA_STATUS_OK;
    }

    // Create send message queue
    g_bleClientSendQueueHandle = (CAQueueingThread_t *) OICMalloc(sizeof(CAQueueingThread_t));
    if (!g_bleClientSendQueueHandle)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Memory allocation failed!");
        return CA_MEMORY_ALLOC_FAILED;
    }

    if (CA_STATUS_OK != CAQueueingThreadInitialize(g_bleClientSendQueueHandle, g_bleAdapterThreadPool,
            CABLEClientSendDataThread, CALEDataDestroyer))
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Failed to Initialize send queue thread");
        OICFree(g_bleClientSendQueueHandle);
        g_bleClientSendQueueHandle = NULL;
        return CA_STATUS_FAILED;
    }

    if (CA_STATUS_OK != CAQueueingThreadStart(g_bleClientSendQueueHandle))
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "ca_thread_pool_add_task failed ");
        OICFree(g_bleClientSendQueueHandle);
        g_bleClientSendQueueHandle = NULL;
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAInitBleServerReceiverQueue()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");
    // Check if the message queue is already initialized
    if (g_bleServerReceiverQueue)
    {
        OIC_LOG(DEBUG, CALEADAPTER_TAG, "Already queue is initialized!");
        return CA_STATUS_OK;
    }

    // Create send message queue
    g_bleServerReceiverQueue = (CAQueueingThread_t *) OICMalloc(sizeof(CAQueueingThread_t));
    if (!g_bleServerReceiverQueue)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Memory allocation failed!");
        OICFree(g_sendQueueHandle);
        return CA_MEMORY_ALLOC_FAILED;
    }

    if (CA_STATUS_OK != CAQueueingThreadInitialize(g_bleServerReceiverQueue, g_bleAdapterThreadPool,
            CABLEServerDataReceiverHandler, CALEDataDestroyer))
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Failed to Initialize send queue thread");
        OICFree(g_bleServerReceiverQueue);
        g_bleServerReceiverQueue = NULL;
        return CA_STATUS_FAILED;
    }

    if (CA_STATUS_OK != CAQueueingThreadStart(g_bleServerReceiverQueue))
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "ca_thread_pool_add_task failed ");
        OICFree(g_bleServerReceiverQueue);
        g_bleServerReceiverQueue = NULL;
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAInitBleClientReceiverQueue()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    // Check if the message queue is already initialized
    if (g_bleClientReceiverQueue)
    {
        OIC_LOG(DEBUG, CALEADAPTER_TAG, "Already queue is initialized!");
    }
    else
    {
        // Create send message queue
        g_bleClientReceiverQueue = (CAQueueingThread_t *) OICMalloc(sizeof(CAQueueingThread_t));
        if (!g_bleClientReceiverQueue)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "Memory allocation failed!");
            OICFree(g_bleClientSendQueueHandle);
            return CA_MEMORY_ALLOC_FAILED;
        }

        if (CA_STATUS_OK != CAQueueingThreadInitialize(g_bleClientReceiverQueue, g_bleAdapterThreadPool,
                CABLEClientDataReceiverHandler, NULL))
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "Failed to Initialize send queue thread");
            OICFree(g_bleClientSendQueueHandle);
            OICFree(g_bleClientReceiverQueue);
            g_bleClientReceiverQueue = NULL;
            return CA_STATUS_FAILED;
        }
    }
    if (CA_STATUS_OK != CAQueueingThreadStart(g_bleClientReceiverQueue))
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "ca_thread_pool_add_task failed ");
        OICFree(g_bleClientReceiverQueue);
        g_bleClientReceiverQueue = NULL;
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CAStopBleQueues()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    ca_mutex_lock(g_bleClientSendDataMutex);
    if (NULL != g_bleClientSendQueueHandle)
    {
        CAQueueingThreadStop(g_bleClientSendQueueHandle);
    }
    ca_mutex_unlock(g_bleClientSendDataMutex);

    ca_mutex_lock(g_bleClientReceiveDataMutex);
    if (NULL != g_bleClientReceiverQueue)
    {
        CAQueueingThreadStop(g_bleClientReceiverQueue);
    }
    ca_mutex_unlock(g_bleClientReceiveDataMutex);

    ca_mutex_lock(g_bleServerSendDataMutex);
    if (NULL != g_sendQueueHandle)
    {
        CAQueueingThreadStop(g_sendQueueHandle);
    }
    ca_mutex_unlock(g_bleServerSendDataMutex);

    ca_mutex_lock(g_bleServerReceiveDataMutex);
    if (NULL != g_bleServerReceiverQueue)
    {
        CAQueueingThreadStop(g_bleServerReceiverQueue);
    }
    ca_mutex_unlock(g_bleServerReceiveDataMutex);

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
}

void CATerminateBleQueues()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    CAQueueingThreadDestroy(g_bleClientSendQueueHandle);
    OICFree(g_bleClientSendQueueHandle);
    g_bleClientSendQueueHandle = NULL;


    CAQueueingThreadDestroy(g_bleClientReceiverQueue);
    OICFree(g_bleClientReceiverQueue);
    g_bleClientReceiverQueue = NULL;


    CAQueueingThreadDestroy(g_sendQueueHandle);
    OICFree(g_sendQueueHandle);
    g_sendQueueHandle = NULL;


    CAQueueingThreadDestroy(g_bleServerReceiverQueue);
    OICFree(g_bleServerReceiverQueue);
    g_bleServerReceiverQueue = NULL;

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
}
void CABLEServerDataReceiverHandler(void *threadData)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    static uint32_t recvDataLen = 0;
    static uint32_t totalDataLen = 0;
    static char *defragData = NULL;
    static bool isHeaderAvailable = false;
    static CARemoteEndpoint_t *remoteEndpoint = NULL;

    ca_mutex_lock(g_bleServerReceiveDataMutex);

    if (g_dataReceiverHandlerState)
    {
        OIC_LOG(DEBUG, CALEADAPTER_TAG, "checking for DE Fragmentation");

        CALEData_t *bleData = (CALEData_t *) threadData;
        if (!bleData)
        {
            OIC_LOG(DEBUG, CALEADAPTER_TAG, "Invalid bleData!");
            return;
        }

        OIC_LOG(DEBUG, CALEADAPTER_TAG, "checking for DE Fragmentation");

        if (!isHeaderAvailable)
        {
            OIC_LOG(DEBUG, CALEADAPTER_TAG, "Parsing the header");
            totalDataLen = CAParseHeader((char*)bleData->data);

            OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Total data to be accumulated [%d] bytes", totalDataLen);
            OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "data received in the first packet [%d] bytes", bleData->dataLen);

            defragData = (char *) OICCalloc(totalDataLen + 1, sizeof(char));
            if (NULL == defragData)
            {
                OIC_LOG(ERROR, CALEADAPTER_TAG, "defragData is NULL!");
                return;
            }

            const char *remoteAddress = bleData->remoteEndpoint->addressInfo.LE.leMacAddress;
            const char *serviceUUID = bleData->remoteEndpoint->resourceUri;

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
        if (totalDataLen == recvDataLen)
        {
            ca_mutex_lock(g_bleAdapterReqRespCbMutex);
            if (NULL == g_networkPacketReceivedCallback)
            {
                OIC_LOG(ERROR, CALEADAPTER_TAG, "gReqRespCallback is NULL!");
                ca_mutex_unlock(g_bleAdapterReqRespCbMutex);
                return;
            }
            OIC_LOG(DEBUG, CALEADAPTER_TAG, "Sending data up !");
            g_networkPacketReceivedCallback(remoteEndpoint, defragData, recvDataLen);
            recvDataLen = 0;
            totalDataLen = 0;
            isHeaderAvailable = false;
            remoteEndpoint = NULL;
            defragData = NULL;
            ca_mutex_unlock(g_bleAdapterReqRespCbMutex);
        }

        if (false == g_dataReceiverHandlerState)
        {
            OIC_LOG(DEBUG, CALEADAPTER_TAG, "GATTClient is terminating. Cleaning up");
            recvDataLen = 0;
            totalDataLen = 0;
            isHeaderAvailable = false;
            OICFree(defragData);
            CAAdapterFreeRemoteEndpoint(remoteEndpoint);
            ca_mutex_unlock(g_bleServerReceiveDataMutex);
            return;
        }
    }
    ca_mutex_unlock(g_bleServerReceiveDataMutex);
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
}

void CABLEClientDataReceiverHandler(void *threadData)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    static const char *remoteAddress = NULL;
    static const char *serviceUUID = NULL;
    static uint32_t recvDataLen = 0;
    static uint32_t totalDataLen = 0;
    static char *defragData = NULL;
    static bool isHeaderAvailable = false;
    static CARemoteEndpoint_t *remoteEndpoint = NULL;

    ca_mutex_lock(g_bleClientReceiveDataMutex);

    if (g_dataReceiverHandlerState)
    {
        OIC_LOG(DEBUG, CALEADAPTER_TAG, "checking for DE Fragmentation");

        CALEData_t *bleData = (CALEData_t *) threadData;
        if (!bleData)
        {
            OIC_LOG(DEBUG, CALEADAPTER_TAG, "Invalid wifidata!");
            return;
        }

        OIC_LOG(DEBUG, CALEADAPTER_TAG, "checking for DE Fragmentation");

        if (!isHeaderAvailable)
        {
            OIC_LOG(DEBUG, CALEADAPTER_TAG, "Parsing the header");

            totalDataLen = CAParseHeader(bleData->data);
            OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Total data to be accumulated [%d] bytes",
                      totalDataLen);
            OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Data received in the first packet [%d] bytes",
                      bleData->dataLen);

            defragData = (char *) OICMalloc(sizeof(char) * totalDataLen);
            if (NULL == defragData)
            {
                OIC_LOG(ERROR, CALEADAPTER_TAG, "defragData is NULL!");
                return;
            }

            remoteAddress = bleData->remoteEndpoint->addressInfo.LE.leMacAddress;
            serviceUUID = bleData->remoteEndpoint->resourceUri;

            remoteEndpoint = CAAdapterCreateRemoteEndpoint(CA_LE, remoteAddress,
                                                           serviceUUID);

            memcpy(defragData, bleData->data + CA_HEADER_LENGTH,
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
        if (totalDataLen == recvDataLen)
        {
            ca_mutex_lock(g_bleAdapterReqRespCbMutex);
            if (NULL == g_networkPacketReceivedCallback)
            {
                OIC_LOG(ERROR, CALEADAPTER_TAG, "gReqRespCallback is NULL!");
                ca_mutex_unlock(g_bleAdapterReqRespCbMutex);
                return;
            }
            OIC_LOG(DEBUG, CALEADAPTER_TAG, "Sending data up !");
            g_networkPacketReceivedCallback(remoteEndpoint, defragData, recvDataLen);
            recvDataLen = 0;
            totalDataLen = 0;
            isHeaderAvailable = false;
            remoteEndpoint = NULL;
            defragData = NULL;
            ca_mutex_unlock(g_bleAdapterReqRespCbMutex);
        }

        if (false == g_dataReceiverHandlerState)
        {
            OIC_LOG(DEBUG, CALEADAPTER_TAG, "GATTClient is terminating. Cleaning up");
            OICFree(defragData);
            CAAdapterFreeRemoteEndpoint(remoteEndpoint);
            ca_mutex_unlock(g_bleClientReceiveDataMutex);
            return;
        }
    }
    ca_mutex_unlock(g_bleClientReceiveDataMutex);
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
}

void CABLEServerSendDataThread(void *threadData)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    CALEData_t *bleData = (CALEData_t *) threadData;
    if (!bleData)
    {
        OIC_LOG(DEBUG, CALEADAPTER_TAG, "Invalid bledata!");
        return;
    }

    char *header = (char *) OICCalloc(CA_HEADER_LENGTH, sizeof(char));
    VERIFY_NON_NULL_VOID(header, CALEADAPTER_TAG, "Malloc failed");

    int32_t totalLength = bleData->dataLen + CA_HEADER_LENGTH;

    OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Server total Data length with header is [%d]", totalLength);
    char *dataSegment = (char *) OICCalloc(totalLength + 1, sizeof(char));
    if (NULL == dataSegment)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Malloc failed");
        OICFree(header);
        return;
    }

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
    if (CA_SUPPORTED_BLE_MTU_SIZE > totalLength)
    {
        length = totalLength;
        memcpy(dataSegment + CA_HEADER_LENGTH, bleData->data, bleData->dataLen);
    }
    else
    {
        length =  CA_SUPPORTED_BLE_MTU_SIZE;
        memcpy(dataSegment + CA_HEADER_LENGTH, bleData->data,
               CA_SUPPORTED_BLE_MTU_SIZE - CA_HEADER_LENGTH);
    }

    int32_t iter = totalLength / CA_SUPPORTED_BLE_MTU_SIZE;
    int32_t index = 0;
    // Send the first segment with the header.
     if (NULL != bleData->remoteEndpoint) //Sending Unicast Data
    {
        OIC_LOG(DEBUG, CALEADAPTER_TAG, "Server Sending Unicast Data");
        result = CAUpdateCharacteristicsToGattClient(
                    bleData->remoteEndpoint->addressInfo.LE.leMacAddress, dataSegment, length);
        if (CA_STATUS_OK != result)
        {
            OIC_LOG_V(ERROR, CALEADAPTER_TAG, "Update characteristics failed, result [%d]", result);
            OICFree(dataSegment);
            return;
        }

        OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Server Sent data length [%d]", length);
        for (index = 1; index < iter; index++)
        {
            // Send the remaining header.
            OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Sending the chunk number [%d]", index);
            result = CAUpdateCharacteristicsToGattClient(
                         bleData->remoteEndpoint->addressInfo.LE.leMacAddress,
                         bleData->data + ((index * CA_SUPPORTED_BLE_MTU_SIZE) - CA_HEADER_LENGTH),
                         CA_SUPPORTED_BLE_MTU_SIZE);
            if (CA_STATUS_OK != result)
            {
                OIC_LOG_V(ERROR, CALEADAPTER_TAG,
                            "Update characteristics failed, result [%d]", result);
                OICFree(dataSegment);
                return;
            }
            OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Server Sent data length [%d]",
                                               CA_SUPPORTED_BLE_MTU_SIZE);
        }

        int32_t remainingLen = totalLength % CA_SUPPORTED_BLE_MTU_SIZE;
        if (remainingLen && (totalLength > CA_SUPPORTED_BLE_MTU_SIZE))
        {
            // send the last segment of the data (Ex: 22 bytes of 622 bytes of data when MTU is 200)
            OIC_LOG(DEBUG, CALEADAPTER_TAG, "Sending the last chunk");
            result = CAUpdateCharacteristicsToGattClient(
                         bleData->remoteEndpoint->addressInfo.LE.leMacAddress,
                         bleData->data + (index * CA_SUPPORTED_BLE_MTU_SIZE) - CA_HEADER_LENGTH,
                         remainingLen);
            if (CA_STATUS_OK != result)
            {
                OIC_LOG_V(ERROR, CALEADAPTER_TAG, "Update characteristics failed, result [%d]",
                                                   result);
                OICFree(dataSegment);
                return;
            }
            OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Server Sent data length [%d]", remainingLen);
        }
     }
    else
    {
        OIC_LOG(DEBUG, CALEADAPTER_TAG, "Server Sending Multicast data");
        result = CAUpdateCharacteristicsToAllGattClients(dataSegment, length);
        if (CA_STATUS_OK != result)
        {
            OIC_LOG_V(ERROR, CALEADAPTER_TAG, "Update characteristics failed, result [%d]", result);
            OICFree(dataSegment);
            return;
        }
        OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Server Sent data length [%d]", length);
        for (index = 1; index < iter; index++)
        {
            // Send the remaining header.
            OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Sending the chunk number [%d]", index);
            result = CAUpdateCharacteristicsToAllGattClients(
                         bleData->data + ((index * CA_SUPPORTED_BLE_MTU_SIZE) - CA_HEADER_LENGTH),
                         CA_SUPPORTED_BLE_MTU_SIZE);
            if (CA_STATUS_OK != result)
            {
                OIC_LOG_V(ERROR, CALEADAPTER_TAG, "Update characteristics failed, result [%d]", result);
                OICFree(dataSegment);
                return;
            }
            OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Server Sent data length [%d]", CA_SUPPORTED_BLE_MTU_SIZE);
        }

        int32_t remainingLen = totalLength % CA_SUPPORTED_BLE_MTU_SIZE;
        if (remainingLen && (totalLength > CA_SUPPORTED_BLE_MTU_SIZE))
        {
            // send the last segment of the data (Ex: 22 bytes of 622 bytes of data when MTU is 200)
            OIC_LOG(DEBUG, CALEADAPTER_TAG, "Sending the last chunk");
            result = CAUpdateCharacteristicsToAllGattClients(
                         bleData->data + (index * CA_SUPPORTED_BLE_MTU_SIZE) - CA_HEADER_LENGTH,
                         remainingLen);
            if (CA_STATUS_OK != result)
            {
                OIC_LOG_V(ERROR, CALEADAPTER_TAG, "Update characteristics failed, result [%d]",
                                                   result);
                OICFree(dataSegment);
                return;
            }
            OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Server Sent data length [%d]", remainingLen);
        }
    }
    OICFree(dataSegment);

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
}

void CABLEClientSendDataThread(void *threadData)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    CALEData_t *bleData = (CALEData_t *) threadData;
    if (!bleData)
    {
        OIC_LOG(DEBUG, CALEADAPTER_TAG, "Invalid bledata!");
        return;
    }

    char *header = (char *) OICCalloc(CA_HEADER_LENGTH, sizeof(char));
    VERIFY_NON_NULL_VOID(header, CALEADAPTER_TAG, "Malloc failed");

    uint32_t totalLength = bleData->dataLen + CA_HEADER_LENGTH;
    char *dataSegment = (char *) OICCalloc(totalLength + 1, sizeof(char));
    if (NULL == dataSegment)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Malloc failed");
        OICFree(header);
        return;
    }

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

    uint32_t length = 0;
    if (CA_SUPPORTED_BLE_MTU_SIZE > totalLength)
    {
        length = totalLength;
        OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "length [%d]", length);
        memcpy(dataSegment + CA_HEADER_LENGTH, bleData->data, bleData->dataLen);
    }
    else
    {
        length = CA_SUPPORTED_BLE_MTU_SIZE;
        OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "length  [%d]", length);
        memcpy(dataSegment + CA_HEADER_LENGTH, bleData->data,
               CA_SUPPORTED_BLE_MTU_SIZE - CA_HEADER_LENGTH);
    }

    uint32_t iter = totalLength / CA_SUPPORTED_BLE_MTU_SIZE;
    uint32_t index = 0;
    if (NULL != bleData->remoteEndpoint) //Sending Unicast Data
    {
        OIC_LOG(DEBUG, CALEADAPTER_TAG, "Sending Unicast Data");
        // Send the first segment with the header.
        result = CAUpdateCharacteristicsToGattServer(bleData->remoteEndpoint->addressInfo.LE.leMacAddress,
                 dataSegment,
                 length,
                 LE_UNICAST, 0);

        if (CA_STATUS_OK != result)
        {
            OIC_LOG_V(ERROR, CALEADAPTER_TAG, "Update characteristics failed, result [%d]", result);
            OICFree(dataSegment);
            return ;
        }

        OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Client Sent Data length  is [%d]", length);
        for (index = 1; index < iter; index++)
        {
            // Send the remaining header.
            result = CAUpdateCharacteristicsToGattServer(
                     bleData->remoteEndpoint->addressInfo.LE.leMacAddress,
                     bleData->data + (index * CA_SUPPORTED_BLE_MTU_SIZE) - CA_HEADER_LENGTH,
                     CA_SUPPORTED_BLE_MTU_SIZE,
                     LE_UNICAST, 0);
            if (CA_STATUS_OK != result)
            {
                OIC_LOG_V(ERROR, CALEADAPTER_TAG, "Update characteristics failed, result [%d]",
                                                   result);
                OICFree(dataSegment);
                return;
            }
            OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Client Sent Data length  is [%d]",
                                               CA_SUPPORTED_BLE_MTU_SIZE);
        }

        uint32_t remainingLen = totalLength % CA_SUPPORTED_BLE_MTU_SIZE;
        if (remainingLen && (totalLength > CA_SUPPORTED_BLE_MTU_SIZE))
        {
            // send the last segment of the data (Ex: 22 bytes of 622 bytes of data when MTU is 200)
            OIC_LOG(DEBUG, CALEADAPTER_TAG, "Sending the last chunk");
            result = CAUpdateCharacteristicsToGattServer(
                     bleData->remoteEndpoint->addressInfo.LE.leMacAddress,
                     bleData->data + (index * CA_SUPPORTED_BLE_MTU_SIZE) - CA_HEADER_LENGTH,
                     remainingLen,
                     LE_UNICAST, 0);

            if (CA_STATUS_OK != result)
            {
                OIC_LOG_V(ERROR, CALEADAPTER_TAG, "Update characteristics failed, result [%d]",
                                                   result);
                OICFree(dataSegment);
                return;
            }
            OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Client Sent Data length  is [%d]", remainingLen);
        }
    }
    else
    {
        //Sending Mulitcast Data
        // Send the first segment with the header.
        OIC_LOG(DEBUG, CALEADAPTER_TAG, "Sending Multicast Data");
        result = CAUpdateCharacteristicsToAllGattServers(dataSegment, length);
        if (CA_STATUS_OK != result)
        {
            OIC_LOG_V(ERROR, CALEADAPTER_TAG, "Update characteristics failed (all), result [%d]", result);
            OICFree(dataSegment);
            return ;
        }
        OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Client Sent Data length  is [%d]", length);
        // Send the remaining header.
        for (index = 1; index < iter; index++)
        {
            result = CAUpdateCharacteristicsToAllGattServers(
                         bleData->data + (index * CA_SUPPORTED_BLE_MTU_SIZE) - CA_HEADER_LENGTH,
                         CA_SUPPORTED_BLE_MTU_SIZE);
            if (CA_STATUS_OK != result)
            {
                OIC_LOG_V(ERROR, CALEADAPTER_TAG, "Update characteristics (all) failed, result [%d]", result);
                OICFree(dataSegment);
                return;
            }
            OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Client Sent Data length  is [%d]", CA_SUPPORTED_BLE_MTU_SIZE);
        }

        uint32_t remainingLen = totalLength % CA_SUPPORTED_BLE_MTU_SIZE;
        if ( remainingLen && (totalLength > CA_SUPPORTED_BLE_MTU_SIZE))
        {
            // send the last segment of the data (Ex: 22 bytes of 622 bytes of data when MTU is 200)
            OIC_LOG(DEBUG, CALEADAPTER_TAG, "Sending the last chunk");
            result = CAUpdateCharacteristicsToAllGattServers(
                         bleData->data + (index * CA_SUPPORTED_BLE_MTU_SIZE) - CA_HEADER_LENGTH,
                          remainingLen);
            if (CA_STATUS_OK != result)
            {
                OIC_LOG_V(ERROR, CALEADAPTER_TAG, "Update characteristics (all) failed, result [%d]", result);
                OICFree(dataSegment);
                return;
            }
            OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Client Sent Data length  is [%d]", remainingLen);
        }

    }

    OICFree(dataSegment);

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT - CABLEClientSendDataThread");
}

CALEData_t *CACreateBLEData(const CARemoteEndpoint_t *remoteEndpoint, const void *data,
                           uint32_t dataLength)
{
    CALEData_t *bleData = (CALEData_t *) OICMalloc(sizeof(CALEData_t));
    if (!bleData)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Memory allocation failed!");
        return NULL;
    }

    bleData->remoteEndpoint = CAAdapterCopyRemoteEndpoint(remoteEndpoint);
    bleData->data = (void *)OICCalloc(dataLength + 1, 1);
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

void CAFreeBLEData(CALEData_t *bleData)
{
    VERIFY_NON_NULL_VOID(bleData, NULL, "Param bleData is NULL");

    CAAdapterFreeRemoteEndpoint(bleData->remoteEndpoint);
    OICFree(bleData->data);
    OICFree(bleData);
}

void CALEDataDestroyer(void *data, uint32_t size)
{
    CALEData_t *ledata = (CALEData_t *) data;

    CAFreeBLEData(ledata);
}


CAResult_t CABLEClientSendData(const CARemoteEndpoint_t *remoteEndpoint,
                               const void *data,
                               uint32_t dataLen)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    VERIFY_NON_NULL(data, NULL, "Param data is NULL");

    VERIFY_NON_NULL_RET(g_bleClientSendQueueHandle, CALEADAPTER_TAG,
                        "g_bleClientSendQueueHandle is  NULL",
                        CA_STATUS_FAILED);
    VERIFY_NON_NULL_RET(g_bleClientSendDataMutex, CALEADAPTER_TAG,
                        "g_bleClientSendDataMutex is NULL",
                        CA_STATUS_FAILED);

    VERIFY_NON_NULL_RET(g_bleClientSendQueueHandle, CALEADAPTER_TAG, "g_bleClientSendQueueHandle",
                        CA_STATUS_FAILED);

    OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Data Sending to LE layer [%d]", dataLen);

    CALEData_t *bleData = CACreateBLEData(remoteEndpoint, data, dataLen);
    if (!bleData)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Failed to create bledata!");
        return CA_MEMORY_ALLOC_FAILED;
    }
    // Add message to send queue
    ca_mutex_lock(g_bleClientSendDataMutex);
    CAQueueingThreadAddData(g_bleClientSendQueueHandle, bleData, sizeof(CALEData_t));
    ca_mutex_unlock(g_bleClientSendDataMutex);

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}


CAResult_t CABLEServerSendData(const CARemoteEndpoint_t *remoteEndpoint,
                               const void *data,
                               uint32_t dataLen)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    VERIFY_NON_NULL(data, NULL, "Param data is NULL");

    VERIFY_NON_NULL_RET(g_sendQueueHandle, CALEADAPTER_TAG,
                        "BleClientReceiverQueue is NULL",
                        CA_STATUS_FAILED);
    VERIFY_NON_NULL_RET(g_bleServerSendDataMutex, CALEADAPTER_TAG,
                        "BleClientSendDataMutex is NULL",
                        CA_STATUS_FAILED);

    VERIFY_NON_NULL_RET(g_sendQueueHandle, CALEADAPTER_TAG, "sendQueueHandle",
                        CA_STATUS_FAILED);

    OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Data Sending to LE layer [%d]", dataLen);

    CALEData_t *bleData = CACreateBLEData(remoteEndpoint, data, dataLen);
    if (!bleData)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Failed to create bledata!");
        return CA_MEMORY_ALLOC_FAILED;
    }
    // Add message to send queue
    ca_mutex_lock(g_bleServerSendDataMutex);
    CAQueueingThreadAddData(g_sendQueueHandle, bleData, sizeof(CALEData_t));
    ca_mutex_unlock(g_bleServerSendDataMutex);

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CABLEServerReceivedData(const char *remoteAddress, const char *serviceUUID,
                                   const void *data, uint32_t dataLength, uint32_t *sentLength)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    //Input validation
    VERIFY_NON_NULL(serviceUUID, CALEADAPTER_TAG, "service UUID is null");
    VERIFY_NON_NULL(data, CALEADAPTER_TAG, "Data is null");
    VERIFY_NON_NULL(sentLength, CALEADAPTER_TAG, "Sent data length holder is null");
    VERIFY_NON_NULL_RET(g_bleServerReceiverQueue, CALEADAPTER_TAG, "g_bleServerReceiverQueue",
                        CA_STATUS_FAILED);

    //Add message to data queue
    CARemoteEndpoint_t *remoteEndpoint = CAAdapterCreateRemoteEndpoint(CA_LE, remoteAddress,
                                         serviceUUID);
    if (NULL == remoteEndpoint)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Failed to create remote endpoint !");
        return CA_STATUS_FAILED;
    }

    // Create bleData to add to queue
    OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Data received from LE layer [%d]", dataLength);

    CALEData_t *bleData = CACreateBLEData(remoteEndpoint, data, dataLength);
    if (!bleData)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Failed to create bledata!");
        CAAdapterFreeRemoteEndpoint(remoteEndpoint);
        return CA_MEMORY_ALLOC_FAILED;
    }

    CAAdapterFreeRemoteEndpoint(remoteEndpoint);
    // Add message to send queue
    CAQueueingThreadAddData(g_bleServerReceiverQueue, bleData, sizeof(CALEData_t));

    *sentLength = dataLength;

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CABLEClientReceivedData(const char *remoteAddress, const char *serviceUUID,
                                   const void *data, uint32_t dataLength, uint32_t *sentLength)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    //Input validation
    VERIFY_NON_NULL(serviceUUID, CALEADAPTER_TAG, "service UUID is null");
    VERIFY_NON_NULL(data, CALEADAPTER_TAG, "Data is null");
    VERIFY_NON_NULL(sentLength, CALEADAPTER_TAG, "Sent data length holder is null");
    VERIFY_NON_NULL_RET(g_bleClientReceiverQueue, CALEADAPTER_TAG, "g_bleClientReceiverQueue",
                        CA_STATUS_FAILED);

    //Add message to data queue
    CARemoteEndpoint_t *remoteEndpoint = CAAdapterCreateRemoteEndpoint(CA_LE, remoteAddress,
                                         serviceUUID);
    if (NULL == remoteEndpoint)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Failed to create remote endpoint !");
        return CA_STATUS_FAILED;
    }

    OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Data received from LE layer [%d]", dataLength);

    // Create bleData to add to queue
    CALEData_t *bleData = CACreateBLEData(remoteEndpoint, data, dataLength);
    if (!bleData)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Failed to create bledata!");
        CAAdapterFreeRemoteEndpoint(remoteEndpoint);
        return CA_MEMORY_ALLOC_FAILED;
    }

    CAAdapterFreeRemoteEndpoint(remoteEndpoint);
    // Add message to send queue
    CAQueueingThreadAddData(g_bleClientReceiverQueue, bleData, sizeof(CALEData_t));

    *sentLength = dataLength;

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CASetBleAdapterThreadPoolHandle(ca_thread_pool_t handle)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    ca_mutex_unlock(g_bleAdapterThreadPoolMutex);
    g_bleAdapterThreadPool = handle;
    ca_mutex_unlock(g_bleAdapterThreadPoolMutex);

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
}

void CASetBLEReqRespAdapterCallback(CANetworkPacketReceivedCallback callback)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    ca_mutex_lock(g_bleAdapterReqRespCbMutex);

    g_networkPacketReceivedCallback = callback;

    ca_mutex_unlock(g_bleAdapterReqRespCbMutex);

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
}


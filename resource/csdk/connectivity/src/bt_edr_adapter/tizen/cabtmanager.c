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
 * @file  cabtmanager.c
 * @brief  This file provides the APIs to control Bluetooth transport
 */

#include "cabtmanager.h"
#include "cabtclient.h"
#include "cabtserver.h"
#include "cabtendpoint.h"
#include "cabtdevicelist.h"
#include "cabtutils.h"
#include "caadapterutils.h"
#include "caqueueingthread.h"
#include "camsgparser.h"

/**
 * @struct CABTNetworkEvent
 * @brief Structure to maintain the adapter information and its status.
 */
typedef struct
{
    CALocalConnectivity_t *info;
    CANetworkStatus_t status;
} CABTNetworkEvent;

/**
 * @struct CABTData
 * @brief Structure to maintain the information of data in message queue.
 */
typedef struct
{
    CARemoteEndpoint_t *remoteEndpoint;
    void *data;
    uint32_t dataLen;
} CABTData;
/**
 * @var gNetworkPacketReceivedCallback
 * @brief Maintains the callback to be notified on receival of network packets from other
 *          Bluetooth devices.
 */
static CANetworkPacketReceivedCallback gNetworkPacketReceivedCallback = NULL;

/**
 * @var gNetworkChangeCallback
 * @brief Maintains the callback to be notified on local bluetooth adapter status change.
 */
static CANetworkChangeCallback gNetworkChangeCallback = NULL;

/**
 * @var gBTDeviceListMutex
 * @brief Mutex to synchronize the access to Bluetooth device information list.
 */
static u_mutex gBTDeviceListMutex = NULL;

/**
 * @var gBTDeviceList
 * @brief Peer Bluetooth device information list.
 */
static BTDeviceList *gBTDeviceList = NULL;

/**
 * @var gLocalConnectivity
 * @brief Information of local Bluetooth adapter.
 */
static CALocalConnectivity_t *gLocalConnectivity = NULL;

/**
 * @var gBTThreadPool
 * @brief Reference to threadpool.
 */
static u_thread_pool_t gBTThreadPool = NULL;

/**
 * @var isHeaderAvailable
 * @brief to differentiate btw header and data packet.
 */
static CABool_t isHeaderAvailable = false;

/**
 * @var gSendQueueHandle
 * @brief Queue handle for Send Data
 */
static CAQueueingThread_t *gSendQueueHandle = NULL;

/**
 * @var gRecvQueueHandle
 * @brief Queue handle for Receive Data
 */
static CAQueueingThread_t *gRecvQueueHandle = NULL;

/**
 * @var gAdapterState
 * @brief Storing Adapter state information
 */
static CABool_t gAdapterState = TRUE;

/**
 * @var gServerId
 * @brief Storing RfcommserverUUID
 */
static int32_t gServerId = -1;

/**
 * @var gServerState
 * @brief Storing Rfcommserver state information
 */
static CABool_t gServerState = FALSE;

/**
 * @fn CABTAdapterStateChangeCallback
 * @brief This callback is registered to receive bluetooth adapter state changes.
 */
static void CABTAdapterStateChangeCallback(int result, bt_adapter_state_e adapterState,
        void *userData);

/**
 * @fn CABTSocketConnectionStateCallback
 * @brief This callback is registered to receive bluetooth RFCOMM connection state changes.
 */
static void CABTSocketConnectionStateCallback(int result,
        bt_socket_connection_state_e state,
        bt_socket_connection_s *connection, void *userData);

/**
 * @fn CABTDataRecvCallback
 * @brief This callback is registered to recieve data on any open RFCOMM connection.
 */
static void CABTDataRecvCallback(bt_socket_received_data_s *data, void *userData);

/**
 * @fn CABTDeviceDiscoveryCallback
 * @brief This callback is registered to recieve all bluetooth nearby devices when device
 *           scan is initiated.
 */
static void CABTDeviceDiscoveryCallback(int result,
                                        bt_adapter_device_discovery_state_e state,
                                        bt_adapter_device_discovery_info_s *discoveryInfo,
                                        void *userData);

/**
 * @fn CABTServiceSearchedCallback
 * @brief This callback is registered to recieve all the services remote bluetooth device supports
 *           when service search initiated.
 */
static void CABTServiceSearchedCallback(int result, bt_device_sdp_info_s *sdpInfo,
                                        void *userData);


/**
 * @fn CABTManagerInitializeQueues
 * @brief This function creates and initialize message queues.
 */
static CAResult_t CABTManagerInitializeQueues(void);

/**
 * @fn CABTManagerInitializeSendHandler
 * @brief This function creates and initialize send message queue.
 */
CAResult_t CABTManagerInitializeSendHandler(void);

/**
 * @fn CABTManagerInitializeQueues
 * @brief This function creates and initialize receive message queue.
 */
CAResult_t CABTManagerInitializeReceiveHandler(void);
/**
 * @fn CABTManagerTerminateQueues
 * @brief This function terminates the send message queue.
 */
static void CABTManagerTerminateQueues(void);

/**
 * @fn CABTManagerInitializeMutex
 * @brief This function creates mutex.
 */
static void CABTManagerInitializeMutex(void);

/**
 * @fn CABTManagerTerminateMutex
 * @brief This function frees mutex.
 */
static void CABTManagerTerminateMutex(void);

/**
 * @fn CABTManagerDataSendHandler
 * @brief This function handles data from send message queue.
 */
static void CABTManagerDataSendHandler(void *context);

/**
 * @fn CABTManagerDataReceiverHandler
 * @brief This function handles data from recv message queue.
 */
static void CABTManagerDataReceiverHandler(void *context);

/**
 * @fn CABTManagerSendUnicastData
 * @brief This function sends data to specified remote bluetooth device.
 */
static CAResult_t CABTManagerSendUnicastData(const char *remoteAddress, const char *serviceUUID,
        void *data, uint32_t dataLength, uint32_t *sentLength);

/**
 * @fn CABTManagerSendMulticastData
 * @brief This function sends data to all bluetooth devices running OIC service.
 */
static CAResult_t CABTManagerSendMulticastData(const char *serviceUUID, void *data,
        uint32_t dataLength, uint32_t *sentLength);

/**
 * @fn CABTGetAdapterEnableState
 * @brief This function gets bluetooth adatper enable state.
 */
static CAResult_t CABTGetAdapterEnableState(bool *state);

/**
 * @fn CABTStartDeviceDiscovery
 * @brief This function starts device discovery.
 */
static CAResult_t CABTStartDeviceDiscovery(void);

/**
 * @fn CABTStopDeviceDiscovery
 * @brief This function stops device discovery.
 */
static CAResult_t CABTStopDeviceDiscovery(void);

/**
 * @fn CABTStartServiceSearch
 * @brief This function searches for OIC service for remote Bluetooth device.
 */
static CAResult_t CABTStartServiceSearch(const char *remoteAddress);

/**
 * @fn CABTStopServiceSearch
 * @brief This function stops any ongoing service sevice search.
 */
static CAResult_t CABTStopServiceSearch(void);

/**
 * @fn CABTNotifyNetworkStauts
 * @brief This function creates notification task for network adapter status and add it to thread pool.
 */
static CAResult_t CABTNotifyNetworkStauts(CANetworkStatus_t status);

/**
 * @fn CABTOnNetworkStautsChanged
 * @brief This is task callback function for notifying network adapter status to upper layer.
 */
static void CABTOnNetworkStautsChanged(void *context);

/**
 * @fn CABTCreateNetworkEvent
 * @brief Creates instance of CABTNetworkEvent.
 */
static CABTNetworkEvent *CABTCreateNetworkEvent(CALocalConnectivity_t *connectivity,
        CANetworkStatus_t status);

/**
 * @fn CABTFreeNetworkEvent
 * @brief Free the memory associated with @event.
 */
static void CABTFreeNetworkEvent(CABTNetworkEvent *event);

/**
 * @fn CABTManagerDisconnectAll
 * @brief Closes all the client connection to peer bluetooth devices.
 */
static void CABTManagerDisconnectAll(void);

/**
 * @fn CACreateBTData
 * @brief creating CABTData
 */
static CABTData *CACreateBTData(const CARemoteEndpoint_t *remoteEndpoint, void *data,
                                uint32_t dataLength);

/**
 * @fn CABTManagerDisconnectAll
 * @brief freeing CABTData
 */
static void CAFreeBTData(CABTData *btData);


CAResult_t CABTManagerInitialize(u_thread_pool_t threadPool)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    int32_t err = BT_ERROR_NONE;

    //Initialize Bluetooth service
    if (BT_ERROR_NONE != (err = bt_initialize()))
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Bluetooth initialization failed!, error num [%x]",
                  err);
        return CA_STATUS_FAILED;
    }

    //Create and initialize the mutex
    CABTManagerInitializeMutex();

    if (NULL == gBTThreadPool)
    {
        gBTThreadPool = threadPool;
    }

    //Initialize Send/Receive data message queues
    if (CA_STATUS_OK != CABTManagerInitializeQueues())
    {
        //Deinitialize the Bluetooth stack
        bt_deinitialize();
        return CA_STATUS_FAILED;
    }

    bt_adapter_set_state_changed_cb(CABTAdapterStateChangeCallback, NULL);

    //Notity to upper layer
    // CABTNotifyNetworkStauts(CA_INTERFACE_UP);

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CABTManagerTerminate(void)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    gNetworkPacketReceivedCallback = NULL;
    gNetworkChangeCallback = NULL;

    //Stop the adpater
    CABTManagerStop();

    //Unset bluetooth adapter callbacks
    bt_adapter_unset_state_changed_cb();

    //Terminate Bluetooth service
    bt_deinitialize();

    //Terminate thread pool
    gBTThreadPool = NULL;

    //Free LocalConnectivity information
    CAAdapterFreeLocalEndpoint(gLocalConnectivity);
    gLocalConnectivity = NULL;

    //Free BTDevices list
    if (gBTDeviceListMutex)
    {
        u_mutex_lock(gBTDeviceListMutex);
        CADestroyBTDeviceList(&gBTDeviceList);
        u_mutex_unlock(gBTDeviceListMutex);
    }

    //Free the mutex
    CABTManagerTerminateMutex();

    //Terminate Send/Receive data messages queues
    CABTManagerTerminateQueues();

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
}

CAResult_t CABTManagerStart(void)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    //Get Bluetooth adapter state
    bool adapterState = false;
    if (CA_STATUS_OK != CABTGetAdapterEnableState(&adapterState))
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Failed to get adatper enable state");
        return CA_STATUS_FAILED;
    }

    if (false == adapterState)
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Bluetooth adapter is disabled!");
        gAdapterState = FALSE;
        return CA_STATUS_OK;
    }

    //Register for discovery and rfcomm socket connection callbacks
    bt_adapter_set_device_discovery_state_changed_cb(CABTDeviceDiscoveryCallback, NULL);
    bt_device_set_service_searched_cb(CABTServiceSearchedCallback, NULL);
    bt_socket_set_connection_state_changed_cb(CABTSocketConnectionStateCallback, NULL);
    bt_socket_set_data_received_cb(CABTDataRecvCallback, NULL);

    //Start device discovery
    CABTStartDeviceDiscovery();

    // Start send queue thread
    if (CA_STATUS_OK != CAQueueingThreadStart(gSendQueueHandle))
    {
        OIC_LOG(ERROR, BLUETOOTH_ADAPTER_TAG, "Failed to Start Send Data Thread");
        CABTManagerStop();
        return CA_STATUS_FAILED;
    }
    // Start receive queue thread
    if (CA_STATUS_OK != CAQueueingThreadStart(gRecvQueueHandle))
    {
        OIC_LOG(ERROR, BLUETOOTH_ADAPTER_TAG, "Failed to Start Receive Data Thread");
        CABTManagerStop();
        return CA_STATUS_FAILED;
    }

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CABTManagerStop(void)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    //Stop service search
    CABTStopServiceSearch();

    //Stop the device discovery process
    CABTStopDeviceDiscovery();

    //Stop send queue thread
    CAQueueingThreadStop(gSendQueueHandle);

    //Stop receive queue thread
    CAQueueingThreadStop(gRecvQueueHandle);

    //reset bluetooth adapter callbacks
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "Resetting the callbacks");
    bt_adapter_unset_device_discovery_state_changed_cb();
    bt_device_unset_service_searched_cb();
    bt_socket_unset_connection_state_changed_cb();
    bt_socket_unset_data_received_cb();

    //Disconnect all the client connections
    CABTManagerDisconnectAll();

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
}

void CABTManagerSetPacketReceivedCallback(
    CANetworkPacketReceivedCallback packetReceivedCallback)
{
    gNetworkPacketReceivedCallback = packetReceivedCallback;
}

void CABTManagerSetNetworkChangeCallback(
    CANetworkChangeCallback networkChangeCallback)
{
    gNetworkChangeCallback = networkChangeCallback;
}

CAResult_t CABTManagerPushDataToReceiverQueue(const char *remoteAddress,
        const char *serviceUUID, void *data, uint32_t dataLength, uint32_t *sentLength)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    if (FALSE == gAdapterState)
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Bluetooth adapter is disabled!");
        *sentLength = 0;
        return CA_STATUS_OK;
    }
    //Input validation
    VERIFY_NON_NULL(serviceUUID, BLUETOOTH_ADAPTER_TAG, "service UUID is null");
    VERIFY_NON_NULL(data, BLUETOOTH_ADAPTER_TAG, "Data is null");
    VERIFY_NON_NULL(sentLength, BLUETOOTH_ADAPTER_TAG, "Sent data length holder is null");

    //Add message to data queue
    CARemoteEndpoint_t *remoteEndpoint = CAAdapterCreateRemoteEndpoint(CA_EDR, remoteAddress,
                                         serviceUUID);
    if (NULL == remoteEndpoint)
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Failed to create remote endpoint !");
        return CA_STATUS_FAILED;
    }

    CABTData *btData =  CACreateBTData(remoteEndpoint, data, dataLength);
    //Add message to data queue

    CAQueueingThreadAddData(gRecvQueueHandle, btData, sizeof(btData));

    *sentLength = dataLength;

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}
CAResult_t CABTManagerSendData(const char *remoteAddress, const char *serviceUUID,
                               void *data, uint32_t dataLength, uint32_t *sentLength)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    if (FALSE == gAdapterState)
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Bluetooth adapter is disabled!");
        *sentLength = 0;
        return CA_STATUS_OK;
    }
    //Input validation
    VERIFY_NON_NULL(serviceUUID, BLUETOOTH_ADAPTER_TAG, "service UUID is null");
    VERIFY_NON_NULL(data, BLUETOOTH_ADAPTER_TAG, "Data is null");
    VERIFY_NON_NULL(sentLength, BLUETOOTH_ADAPTER_TAG, "Sent data length holder is null");

    CARemoteEndpoint_t *remoteEndpoint = CAAdapterCreateRemoteEndpoint(CA_EDR, remoteAddress,
                                         serviceUUID);
    if (NULL == remoteEndpoint)
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Failed to create remote endpoint !");
        return CA_STATUS_FAILED;
    }

    CABTData *btData =  CACreateBTData(remoteEndpoint, data, dataLength);
    //Add message to data queue

    CAQueueingThreadAddData(gSendQueueHandle, btData, sizeof(btData));

    *sentLength = dataLength;

    //Signal message handler for processing data for sending
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CABTManagerStartServer(void)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    if (FALSE == gAdapterState)
    {
        OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "Bluetooth adapter is disabled!");
        //Setting gServerState for starting Rfcommserver when adapter starts
        gServerState = TRUE;
        return CA_STATUS_OK;
    }

    if (-1 < gServerId)
    {
        OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "Server is already in running state.");
        return CA_STATUS_OK;
    }
    return CABTServerStart(OIC_BT_SERVICE_ID, &gServerId);
}

CAResult_t CABTManagerStopServer(void)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    if (-1 == gServerId)
    {
        OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "No Server is in running state.");
        return CA_STATUS_OK;
    }

    if (CA_STATUS_OK != CABTServerStop(gServerId))
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Failed to close server socket!");
        return CA_STATUS_FAILED;
    }

    gServerId = -1;
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CABTManagerGetInterface(CALocalConnectivity_t **info)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    int32_t err = BT_ERROR_NONE;
    char *localAddress = NULL;

    //Input validation
    VERIFY_NON_NULL(info, BLUETOOTH_ADAPTER_TAG, "LocalConnectivity info is null");

    //Get the bluetooth adapter local address
    if (BT_ERROR_NONE != (err = bt_adapter_get_address(&localAddress)))
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG,
                  "Getting local adapter address failed!, error num [%x]",
                  err);
        return CA_STATUS_FAILED;
    }

    //Create network info
    *info = CAAdapterCreateLocalEndpoint(CA_EDR, localAddress);
    if (NULL == *info)
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Failed to create LocalConnectivity instance!");

        OICFree(localAddress);
        return CA_MEMORY_ALLOC_FAILED;
    }

    OICFree(localAddress);

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CABTManagerReadData(void)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
    return CA_NOT_SUPPORTED;
}

CAResult_t CABTManagerInitializeQueues(void)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    if (CA_STATUS_OK == CABTManagerInitializeSendHandler()
        && CA_STATUS_OK == CABTManagerInitializeReceiveHandler())
    {
        OIC_LOG(DEBUG, BLUETOOTH_ADAPTER_TAG, "Queue is initialized!");
        return CA_STATUS_OK;
    }

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
    return CA_STATUS_FAILED;
}

CAResult_t CABTManagerInitializeSendHandler(void)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");
    // Check if the message queue is already initialized
    if (gSendQueueHandle)
    {
        OIC_LOG(DEBUG, BLUETOOTH_ADAPTER_TAG, "Already queue is initialized!");
        return CA_STATUS_OK;
    }

    gSendQueueHandle = OICMalloc(sizeof(CAQueueingThread_t));
    if (!gSendQueueHandle)
    {
        OIC_LOG(ERROR, BLUETOOTH_ADAPTER_TAG, "Memory allocation failed!");
        return CA_MEMORY_ALLOC_FAILED;
    }

    if (CA_STATUS_OK != CAQueueingThreadInitialize(gSendQueueHandle, gBTThreadPool,
            CABTManagerDataSendHandler))
    {
        OIC_LOG(ERROR, BLUETOOTH_ADAPTER_TAG, "Failed to Initialize send queue thread");
        return CA_STATUS_FAILED;
    }
    return CA_STATUS_OK;
}

CAResult_t CABTManagerInitializeReceiveHandler(void)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");
    // Check if the message queue is already initialized
    if (gRecvQueueHandle)
    {
        OIC_LOG(DEBUG, BLUETOOTH_ADAPTER_TAG, "Already queue is initialized!");
        return CA_STATUS_OK;
    }

    gRecvQueueHandle = OICMalloc(sizeof(CAQueueingThread_t));
    if (!gRecvQueueHandle)
    {
        OIC_LOG(ERROR, BLUETOOTH_ADAPTER_TAG, "Memory allocation failed!");
        return CA_MEMORY_ALLOC_FAILED;
    }

    if (CA_STATUS_OK != CAQueueingThreadInitialize(gRecvQueueHandle, gBTThreadPool,
            CABTManagerDataReceiverHandler))
    {
        OIC_LOG(ERROR, BLUETOOTH_ADAPTER_TAG, "Failed to Initialize send queue thread");
        return CA_STATUS_FAILED;
    }

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CABTManagerTerminateQueues(void)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    if (gSendQueueHandle)
    {
        CAQueueingThreadDestroy(gSendQueueHandle);
        gSendQueueHandle = NULL;
    }
    if (gRecvQueueHandle)
    {
        CAQueueingThreadDestroy(gRecvQueueHandle);
        gRecvQueueHandle = NULL;
    }

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
}

void CABTManagerInitializeMutex(void)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    u_mutex_init();

    if (!gBTDeviceListMutex)
    {
        gBTDeviceListMutex = u_mutex_new();
    }

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
}

void CABTManagerTerminateMutex(void)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    if (gBTDeviceListMutex)
    {
        u_mutex_free(gBTDeviceListMutex);
        gBTDeviceListMutex = NULL;
    }

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
}


void CABTManagerDataReceiverHandler(void *context)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");


    CABTData *message = (CABTData *) context;
    if (message == NULL)
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Failed to get message!");
        return;
    }
    const char *remoteAddress = NULL;
    const char *serviceUUID = NULL;
    static uint32_t recvDataLen = 0;
    static uint32_t totalDataLen = 0;
    static char *defragData = NULL;
    static CARemoteEndpoint_t *remoteEndpoint = NULL;

    if (!isHeaderAvailable)
    {
        char *header = (char *) OICMalloc(sizeof(char) * CA_HEADER_LENGTH);

        if (NULL == header)
        {
            OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Out of memory (header)!");
            return;
        }
        memcpy(header, (char *)message->data, CA_HEADER_LENGTH);
        totalDataLen = CAParseHeader(header);
        OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "Total data to be accumulated [%d] bytes",
                  totalDataLen);
        defragData = NULL;
        if (totalDataLen == 0)
        {
            OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, " totalDataLen is zero");
            OICFree(header);
            return;
        }
        defragData = (char *) OICMalloc(sizeof(char) * totalDataLen);
        OICFree(header);

        remoteAddress = message->remoteEndpoint->addressInfo.BT.btMacAddress;
        serviceUUID = message->remoteEndpoint->resourceUri;

        remoteEndpoint = CAAdapterCreateRemoteEndpoint(CA_EDR, remoteAddress,
                         serviceUUID);

        memcpy(defragData + recvDataLen, message->data + CA_HEADER_LENGTH,
               message->dataLen - CA_HEADER_LENGTH);
        recvDataLen += message->dataLen - CA_HEADER_LENGTH;
        isHeaderAvailable = true;
    }
    else
    {
        memcpy(defragData + recvDataLen, message->data, message->dataLen);
        recvDataLen += message->dataLen ;
    }
    CAFreeBTData(message);
    if (totalDataLen == recvDataLen)
    {
        OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "Sending data up !");
        gNetworkPacketReceivedCallback(remoteEndpoint, defragData, recvDataLen);
        recvDataLen = 0;
        totalDataLen = 0;
        remoteEndpoint = NULL;
        isHeaderAvailable = false;
    }

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
}


void CABTManagerDataSendHandler(void *context)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    CABTData *message = (CABTData *) context;
    if (message == NULL)
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Failed to get message!");
        return;
    }
    const char *remoteAddress = NULL;
    const char *serviceUUID = NULL;
    uint32_t sentLength = 0;
    int32_t headerAdded = 0;

    remoteAddress = message->remoteEndpoint->addressInfo.BT.btMacAddress;
    serviceUUID = message->remoteEndpoint->resourceUri;

    char *dataSegment = NULL;
    uint32_t offset = 0, ret = 1;
    int32_t datalen = message->dataLen;
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "checking for fragmentation and the dataLen is %d",
              datalen);
    while ( 1)
    {
        OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "fragmenting the data DataLen [%d] Offset [%d]",
                  datalen, offset);
        ret = CAFragmentData((char *)message->data, &dataSegment, datalen, offset);
        if (0 == ret)
        {
            break;
        }
        if (NULL == dataSegment)
        {
            OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Failed to allocated memory fo dataSegment.");
            break;
        }
        if (strlen(remoteAddress)) //Unicast data
        {
            if (CA_STATUS_OK != CABTManagerSendUnicastData(remoteAddress, serviceUUID,
                    dataSegment, ret, &sentLength))
            {
                OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Failed to send unicast data !");
            }
        }
        else
        {
            OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "sending multicast data");
            if (CA_STATUS_OK != CABTManagerSendMulticastData(serviceUUID, dataSegment,
                    ret, &sentLength))
            {
                OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Failed to send multicast data !");
            }
        }
        OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "freeing dataSegment");

        OICFree(dataSegment);
        dataSegment = NULL;
        offset += ret;
        if (headerAdded == 0)
        {
            datalen -= offset - CA_HEADER_LENGTH;
            offset = offset - CA_HEADER_LENGTH;
            headerAdded = 1;
        }
        else
        {
            datalen -= ret;
        }

        if (datalen < 0)
        {
            datalen += ret ;
        }
        if (datalen == message->dataLen)
        {
            OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "All data has been fragmented and sent");
            break;
        }
    }
    //Free message
    CAFreeBTData(message);

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
}

CAResult_t CABTManagerSendUnicastData(const char *remoteAddress, const char *serviceUUID,
                                      void *data, uint32_t dataLength, uint32_t *sentLength)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    BTDevice *device = NULL;

    //Input validation
    VERIFY_NON_NULL(remoteAddress, BLUETOOTH_ADAPTER_TAG, "Remote address is null");
    VERIFY_NON_NULL(serviceUUID, BLUETOOTH_ADAPTER_TAG, "service UUID is null");
    VERIFY_NON_NULL(data, BLUETOOTH_ADAPTER_TAG, "Data is null");
    VERIFY_NON_NULL(sentLength, BLUETOOTH_ADAPTER_TAG, "Sent data length holder is null");

    if (0 >= dataLength)
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Invalid input: Negative data length!");
        return CA_STATUS_INVALID_PARAM;
    }

    //Check the connection existence with remote device
    u_mutex_lock(gBTDeviceListMutex);
    if (CA_STATUS_OK != CAGetBTDevice(gBTDeviceList, remoteAddress, &device))
    {
        //Create new device and add to list
        if (CA_STATUS_OK != CACreateAndAddToDeviceList(&gBTDeviceList, remoteAddress,
                OIC_BT_SERVICE_ID, &device))
        {
            OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Failed create device and add to list!");

            u_mutex_unlock(gBTDeviceListMutex);
            return CA_STATUS_FAILED;
        }

        //Start the OIC service search newly created device
        if (CA_STATUS_OK != CABTStartServiceSearch(remoteAddress))
        {
            OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Failed to initiate service search!");

            //Remove device from list
            CARemoveBTDeviceFromList(&gBTDeviceList, remoteAddress);

            u_mutex_unlock(gBTDeviceListMutex);
            return CA_STATUS_FAILED;
        }
    }
    u_mutex_unlock(gBTDeviceListMutex);

    if (-1 == device->socketFD)
    {
        //Adding to pending list
        if (CA_STATUS_OK != CAAddBTDataToList(&device->pendingDataList, data,
                                              dataLength))
        {
            OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Failed to add data to pending list!");

            //Remove device from list
            CARemoveBTDeviceFromList(&gBTDeviceList, remoteAddress);
            return CA_STATUS_FAILED;
        }

        //Make a rfcomm connection with remote BT Device
        if (1 == device->serviceSearched &&
            CA_STATUS_OK != CABTClientConnect(remoteAddress, serviceUUID))
        {
            OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Failed to make RFCOMM connection!");

            //Remove device from list
            CARemoveBTDeviceFromList(&gBTDeviceList, remoteAddress);
            return CA_STATUS_FAILED;
        }
        *sentLength = dataLength;
    }
    else
    {
        if (CA_STATUS_OK != CABTSendData(device->socketFD, data, dataLength, sentLength))
        {
            OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Failed to send data!");
            return CA_STATUS_FAILED;
        }
    }

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CABTManagerSendMulticastData(const char *serviceUUID, void *data,
                                        uint32_t dataLength, uint32_t *sentLength)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    //Input validation
    VERIFY_NON_NULL(serviceUUID, BLUETOOTH_ADAPTER_TAG, "service UUID is null");
    VERIFY_NON_NULL(data, BLUETOOTH_ADAPTER_TAG, "Data is null");
    VERIFY_NON_NULL(sentLength, BLUETOOTH_ADAPTER_TAG, "Sent data length holder is null");

    if (0 >= dataLength)
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Invalid input: Negative data length!");
        return CA_STATUS_INVALID_PARAM;
    }

    *sentLength = dataLength;

    //Send the packet to all OIC devices
    u_mutex_lock(gBTDeviceListMutex);
    BTDeviceList *curList = gBTDeviceList;
    while (curList != NULL)
    {
        BTDevice *device = curList->device;
        curList = curList->next;

        if (!device)
        {
            OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "There is no device!");
            break;
        }

        if (-1 == device->socketFD)
        {
            //Check if the device service search is finished
            if (0 == device->serviceSearched)
            {
                OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Device services are still unknown!");
                continue;
            }

            //Adding to pendding list
            if (CA_STATUS_OK != CAAddBTDataToList(&device->pendingDataList, data,
                                                  dataLength))
            {
                OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Failed to add data to pending list !");
                continue;
            }

            //Make a rfcomm connection with remote BT Device
            if (CA_STATUS_OK != CABTClientConnect(device->remoteAddress, device->serviceUUID))
            {
                OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Failed to make RFCOMM connection !");

                //Remove the data which added to pending list
                CARemoveBTDataFromList(&device->pendingDataList);
                continue;
            }
        }
        else
        {
            if (CA_STATUS_OK != CABTSendData(device->socketFD, data, dataLength, sentLength))
            {
                OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Failed to send data to [%s] !",
                          device->remoteAddress);
            }
        }
    }
    u_mutex_unlock(gBTDeviceListMutex);

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CABTGetAdapterEnableState(bool *state)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    //Input validation
    VERIFY_NON_NULL(state, BLUETOOTH_ADAPTER_TAG, "state holder is NULL!");

    bt_error_e err = BT_ERROR_NONE;
    bt_adapter_state_e adapterState;

    //Get Bluetooth adapter state
    if (BT_ERROR_NONE != (err = bt_adapter_get_state(&adapterState)))
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Bluetooth get state failed!, error num [%x]",
                  err);

        return CA_STATUS_FAILED;
    }

    *state = false;
    if (BT_ADAPTER_ENABLED == adapterState)
    {
        *state = true;
    }

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CABTStartDeviceDiscovery(void)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    bt_error_e err = BT_ERROR_NONE;
    bool isDiscoveryStarted = false;

    //Check the device discovery state
    if (BT_ERROR_NONE != (err = bt_adapter_is_discovering(&isDiscoveryStarted)))
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Failed to get discovery state!, error num [%x]",
                  err);
        return CA_STATUS_FAILED;
    }

    //Start device discovery if its not started
    if (false == isDiscoveryStarted)
    {
        if (BT_ERROR_NONE != (err = bt_adapter_start_device_discovery()))
        {
            OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Device discovery failed!, error num [%x]",
                      err);
            return CA_STATUS_FAILED;
        }
    }

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CABTStopDeviceDiscovery(void)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    bt_error_e err = BT_ERROR_NONE;
    bool isDiscoveryStarted = false;

    //Check the device discovery state
    if (BT_ERROR_NONE != (err = bt_adapter_is_discovering(&isDiscoveryStarted)))
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Failed to get discovery state!, error num [%x]",
                  err);
        return CA_STATUS_FAILED;
    }

    //stop the device discovery process
    if (true == isDiscoveryStarted)
    {
        OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "Stopping the device search process");
        if (BT_ERROR_NONE != (err = bt_adapter_stop_device_discovery()))
        {
            OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Failed to stop discovery!, error num [%x]",
                      err);
        }
    }

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CABTStartServiceSearch(const char *remoteAddress)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    bt_error_e err = BT_ERROR_NONE;

    //Input validation
    VERIFY_NON_NULL(remoteAddress, BLUETOOTH_ADAPTER_TAG, "Remote address is null");
    if (0 == strlen(remoteAddress))
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Remote address is empty!");
        return CA_STATUS_INVALID_PARAM;
    }

    //Start searching for OIC service
    if (BT_ERROR_NONE != (err = bt_device_start_service_search(remoteAddress)))
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Get bonded device failed!, error num [%x]",
                  err);
        return CA_STATUS_FAILED;
    }

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CABTStopServiceSearch(void)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    bt_error_e err = BT_ERROR_NONE;

    //Stop ongoing service search
    if (BT_ERROR_NONE != (err = bt_device_cancel_service_search()))
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Get bonded device failed!, error num [%x]",
                  err);
        return CA_STATUS_FAILED;
    }

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CABTAdapterStateChangeCallback(int result, bt_adapter_state_e adapterState,
                                    void *userData)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    if (BT_ADAPTER_ENABLED == adapterState)
    {
        // checking for adapter state. If not started then start.
        if (FALSE == gAdapterState)
        {
            CABTManagerStart();
            gAdapterState = TRUE;
            //starting RFcommserver
            if (gServerState == TRUE)
            {
                CABTManagerStartServer();
                gServerState = FALSE;
            }
        }
        //Notity to upper layer
        CABTNotifyNetworkStauts(CA_INTERFACE_UP);
    }
    else if (BT_ADAPTER_DISABLED == adapterState)
    {
        //Notity to upper layer
        CABTNotifyNetworkStauts(CA_INTERFACE_DOWN);
        gAdapterState = FALSE;
    }

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
}

void CABTSocketConnectionStateCallback(int result, bt_socket_connection_state_e state,
                                       bt_socket_connection_s *connection, void *userData)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    BTDevice *device = NULL;

    if (BT_ERROR_NONE != result || NULL == connection)
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Invalid connection state!, error num [%x]",
                  result);
        return;
    }

    switch (state)
    {
        case BT_SOCKET_CONNECTED:
            {
                u_mutex_lock(gBTDeviceListMutex);
                if (CA_STATUS_OK != CAGetBTDevice(gBTDeviceList, connection->remote_address,
                                                  &device))
                {
                    //Create the deviceinfo and add to list
                    if (CA_STATUS_OK != CACreateAndAddToDeviceList(&gBTDeviceList,
                            connection->remote_address, OIC_BT_SERVICE_ID, &device))
                    {
                        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Failed add device to list!");
                        u_mutex_unlock(gBTDeviceListMutex);
                        return;
                    }

                    device->socketFD = connection->socket_fd;
                    u_mutex_unlock(gBTDeviceListMutex);
                    return;
                }

                device->socketFD = connection->socket_fd;
                while (device->pendingDataList)
                {
                    uint32_t sentData = 0;
                    BTData *btData = device->pendingDataList->data;
                    if (CA_STATUS_OK != CABTSendData(device->socketFD, btData->data,
                                                     btData->dataLength, &sentData))
                    {
                        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Failed to send pending data [%s]",
                                  device->remoteAddress);

                        //Remove all the data from pending list
                        CADestroyBTDataList(&device->pendingDataList);
                        break;
                    }

                    //Remove the data which send from pending list
                    CARemoveBTDataFromList(&device->pendingDataList);
                }
                u_mutex_unlock(gBTDeviceListMutex);
            }
            break;

        case BT_SOCKET_DISCONNECTED:
            {
                u_mutex_lock(gBTDeviceListMutex);
                CARemoveBTDeviceFromList(&gBTDeviceList, connection->remote_address);
                u_mutex_unlock(gBTDeviceListMutex);
            }
            break;
    }

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
}

void CABTDataRecvCallback(bt_socket_received_data_s *data, void *userData)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    BTDevice *device = NULL;

    if (NULL == data || 0 >= data->data_size)
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Data is null!");
        return;
    }

    if (NULL == gNetworkPacketReceivedCallback)
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Callback is not registered!");
        return;
    }

    //Get BT device from list
    u_mutex_lock(gBTDeviceListMutex);
    if (CA_STATUS_OK != CAGetBTDeviceBySocketId(gBTDeviceList, data->socket_fd, &device))
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Could not find the device!");

        u_mutex_unlock(gBTDeviceListMutex);
        return;
    }
    u_mutex_unlock(gBTDeviceListMutex);

    uint32_t sentLength = 0;

    CAResult_t res = CABTManagerPushDataToReceiverQueue(device->remoteAddress, OIC_BT_SERVICE_ID,
                     data->data, (uint32_t)data->data_size, &sentLength);

    OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "CABTManagerPushDataToReceiverQueue done [%d]", res);

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
}

void CABTDeviceDiscoveryCallback(int result, bt_adapter_device_discovery_state_e state,
                                 bt_adapter_device_discovery_info_s *discoveryInfo, void *userData)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    BTDevice *device = NULL;

    if (BT_ERROR_NONE != result)
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Received bad state!, error num [%x]",
                  result);
        return;
    }

    switch (state)
    {
        case BT_ADAPTER_DEVICE_DISCOVERY_STARTED:
            {
                OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "Discovery started!");
            }
            break;

        case BT_ADAPTER_DEVICE_DISCOVERY_FINISHED:
            {
                OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "Discovery finished!");
            }
            break;

        case BT_ADAPTER_DEVICE_DISCOVERY_FOUND:
            {
                OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "Device discovered [%s]!",
                          discoveryInfo->remote_name);
                if (true == CABTIsServiceSupported((const char **)discoveryInfo->service_uuid,
                                                   discoveryInfo->service_count,
                                                   OIC_BT_SERVICE_ID))
                {
                    //Check if the deivce is already in the list
                    u_mutex_lock(gBTDeviceListMutex);
                    if (CA_STATUS_OK == CAGetBTDevice(gBTDeviceList, discoveryInfo->remote_address,
                                                      &device))
                    {
                        device->serviceSearched = 1;
                        u_mutex_unlock(gBTDeviceListMutex);
                        return;
                    }

                    //Create the deviceinfo and add to list
                    if (CA_STATUS_OK != CACreateAndAddToDeviceList(&gBTDeviceList,
                            discoveryInfo->remote_address, OIC_BT_SERVICE_ID, &device))
                    {
                        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Failed to add device to list!");
                        u_mutex_unlock(gBTDeviceListMutex);
                        return;
                    }

                    device->serviceSearched = 1;
                    u_mutex_unlock(gBTDeviceListMutex);
                }
                else
                {
                    OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Device does not support OIC service!");
                }
            }
            break;
    }

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
}

void CABTServiceSearchedCallback(int32_t result, bt_device_sdp_info_s *sdpInfo, void *userData)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    if (NULL == sdpInfo)
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "SDP info is null!");
        return;
    }

    u_mutex_lock(gBTDeviceListMutex);

    BTDevice *device = NULL;
    if (CA_STATUS_OK == CAGetBTDevice(gBTDeviceList, sdpInfo->remote_address, &device)
        && NULL != device)
    {
        if (1 == device->serviceSearched)
        {
            OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "Service is already searched for this device!");
            u_mutex_unlock(gBTDeviceListMutex);
            return;
        }

        if (true == CABTIsServiceSupported((const char **)sdpInfo->service_uuid,
                                           sdpInfo->service_count, OIC_BT_SERVICE_ID))
        {
            device->serviceSearched = 1;
            if (CA_STATUS_OK != CABTClientConnect(sdpInfo->remote_address, OIC_BT_SERVICE_ID))
            {
                OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Failed to make rfcomm connection!");

                //Remove the device from device list
                CARemoveBTDeviceFromList(&gBTDeviceList, sdpInfo->remote_address);
            }
        }
        else
        {
            OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "Device does not contain OIC service!");

            //Remove device from list as it does not support OIC service
            CARemoveBTDeviceFromList(&gBTDeviceList, sdpInfo->remote_address);
        }
    }

    u_mutex_unlock(gBTDeviceListMutex);

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
}

CAResult_t CABTNotifyNetworkStauts(CANetworkStatus_t status)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    //Create localconnectivity
    if (NULL == gLocalConnectivity)
    {
        CABTManagerGetInterface(&gLocalConnectivity);
    }

    //Notity to upper layer
    if (gNetworkChangeCallback && gLocalConnectivity && gBTThreadPool)
    {
        //Add notification task to thread pool
        CABTNetworkEvent *event = CABTCreateNetworkEvent(gLocalConnectivity, status);
        if (NULL != event)
        {
            if (CA_STATUS_OK != u_thread_pool_add_task(gBTThreadPool, CABTOnNetworkStautsChanged,
                    event))
            {
                OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Failed to create threadpool!");
                return CA_STATUS_FAILED;
            }
        }
    }

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CABTOnNetworkStautsChanged(void *context)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    if (NULL == context)
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "context is NULL!");
        return;
    }

    CABTNetworkEvent *networkEvent = (CABTNetworkEvent *) context;

    //Notity to upper layer
    if (gNetworkChangeCallback)
    {
        gNetworkChangeCallback(networkEvent->info, networkEvent->status);
    }

    //Free the created Network event
    CABTFreeNetworkEvent(networkEvent);

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
}

CABTNetworkEvent *CABTCreateNetworkEvent(CALocalConnectivity_t *connectivity,
        CANetworkStatus_t status)
{
    VERIFY_NON_NULL_RET(connectivity, BLUETOOTH_ADAPTER_TAG, "connectivity is NULL", NULL);

    //Create CABTNetworkEvent
    CABTNetworkEvent *event = (CABTNetworkEvent *) OICMalloc(sizeof(CABTNetworkEvent));
    if (NULL == event)
    {
        OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Failed to allocate memory to network event!");
        return NULL;
    }

    //Create duplicate of Local connectivity
    event->info = CAAdapterCopyLocalEndpoint(connectivity);
    event->status = status;
    return event;
}

void CABTFreeNetworkEvent(CABTNetworkEvent *event)
{
    if (event)
    {
        CAAdapterFreeLocalEndpoint(event->info);
        OICFree(event);
    }
}

void CABTManagerDisconnectAll(void)
{
    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "IN");

    u_mutex_lock(gBTDeviceListMutex);

    BTDeviceList *cur = gBTDeviceList;
    while (cur != NULL)
    {
        BTDevice *device = cur->device;
        cur = cur->next;

        if (device && 0 <= device->socketFD)
        {
            if (CA_STATUS_OK != CABTClientDisconnect(device->socketFD))
            {
                OIC_LOG_V(ERROR, BLUETOOTH_ADAPTER_TAG, "Failed to disconnect with client :%s",
                          device->remoteAddress);
            }

            device->socketFD = -1;
        }
    }

    u_mutex_unlock(gBTDeviceListMutex);

    OIC_LOG_V(DEBUG, BLUETOOTH_ADAPTER_TAG, "OUT");
}
CABTData *CACreateBTData(const CARemoteEndpoint_t *remoteEndpoint, void *data,
                         uint32_t dataLength)
{
    CABTData *btData = (CABTData *) OICMalloc(sizeof(CABTData));
    if (!btData)
    {
        OIC_LOG(ERROR, BLUETOOTH_ADAPTER_TAG, "Memory allocation failed!");
        return NULL;
    }

    btData->remoteEndpoint = CAAdapterCopyRemoteEndpoint(remoteEndpoint);
    btData->data = (void *)OICMalloc(dataLength);
    if (NULL == btData->data)
    {
        OIC_LOG(ERROR, BLUETOOTH_ADAPTER_TAG, "Memory allocation failed!");
        CAFreeBTData(btData);
        return NULL;
    }
    memcpy(btData->data, data, dataLength);
    btData->dataLen = dataLength;

    return btData;
}

void CAFreeBTData(CABTData *btData)
{
    if (!btData)
        return;

    CAAdapterFreeRemoteEndpoint(btData->remoteEndpoint);
    OICFree(btData->data);
    OICFree(btData);
}

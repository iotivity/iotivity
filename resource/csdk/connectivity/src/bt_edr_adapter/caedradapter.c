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
 * @file caedradapter.c
 * @brief This file contains the APIs for EDR adapter.
 */

#include "caedradapter.h"

#include "caedrinterface.h"
#include "caadapterutils.h"
#include "logger.h"
#include "camsgparser.h"
#include "caqueueingthread.h"

/**
 * @var EDR_ADAPTER_TAG
 * @brief Logging tag for module name.
 */
#define EDR_ADAPTER_TAG "CA_EDR_ADAPTER"

/**
 * @var gEDRThreadPool
 * @brief Reference to threadpool.
 */
static u_thread_pool_t gEDRThreadPool = NULL;

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
 * @var isHeaderAvailable
 * @brief to differentiate btw header and data packet.
 */
static CABool_t isHeaderAvailable = CA_FALSE;

/**
 * @var gAdapterState
 * @brief Storing Adapter state information
 */
static CABool_t gAdapterState = CA_TRUE;

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
 * @var gLocalConnectivity
 * @brief Information of local Bluetooth adapter.
 */
static CALocalConnectivity_t *gLocalConnectivity = NULL;

/**
 * @var gServerId
 * @brief Storing RfcommserverUUID
 */
static int32_t gServerId = -1;

/**
 * @var gServerState
 * @brief Storing Rfcommserver state information
 */
static CABool_t gServerState = CA_FALSE;

static CAResult_t CAStartServer(void);
static CAResult_t CAEDRInitializeQueueHandlers(void);
CAResult_t CAEDRInitializeSendHandler(void);
CAResult_t CAEDRInitializeReceiveHandler(void);
void CAAdapterTerminateQueues(void);
void CAAdapterDataSendHandler(void *context);
void CAAdapterDataReceiverHandler(void *context);
CAResult_t CAAdapterStopQueue();
void CAAdapterRecvData(const char *remoteAddress, void *data,
                                    uint32_t dataLength, uint32_t *sentLength);
void CAEDRNotifyNetworkStatus(CANetworkStatus_t status);
void CAEDROnNetworkStatusChanged(void *context);
CAResult_t CAAdapterSendData(const char *remoteAddress, const char *serviceUUID,
                               void *data, uint32_t dataLength, uint32_t *sentLength);
CAEDRNetworkEvent *CAEDRCreateNetworkEvent(CALocalConnectivity_t *connectivity,
                                           CANetworkStatus_t status);
CAResult_t CAEDRClientSendData(const char *remoteAddress, const char *serviceUUID,
                                      void *data, uint32_t dataLength, uint32_t *sentLength);
/**
 * @fn CACreateEDRData
 * @brief Helper function to create CAEDRData
 */
static CAEDRData *CACreateEDRData(const CARemoteEndpoint_t *remoteEndpoint, void *data,
                                uint32_t dataLength);

/**
 * @fn CAFreeEDRData
 * @brief Free the Created EDR data
 */
static void CAFreeEDRData(CAEDRData *edrData);

/**
 * @fn CAEDRFreeNetworkEvent
 * @brief Free the memory associated with @event.
 */
void CAEDRFreeNetworkEvent(CAEDRNetworkEvent *event);

CAResult_t CAInitializeEDR(CARegisterConnectivityCallback registerCallback,
                           CANetworkPacketReceivedCallback packetReceivedCallback,
                           CANetworkChangeCallback networkStateChangeCallback,
                           u_thread_pool_t handle)
{
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN");

    // Input validation
    VERIFY_NON_NULL(registerCallback, EDR_ADAPTER_TAG, "register callback is NULL");
    VERIFY_NON_NULL(packetReceivedCallback, EDR_ADAPTER_TAG, "data receive callback is NULL");
    VERIFY_NON_NULL(networkStateChangeCallback, EDR_ADAPTER_TAG,
                    "network state change callback is NULL");
    VERIFY_NON_NULL(handle, EDR_ADAPTER_TAG, "Thread pool hanlde is NULL");

    // Register the callbacks

    gEDRThreadPool = handle;
    gNetworkPacketReceivedCallback = packetReceivedCallback;
    gNetworkChangeCallback = networkStateChangeCallback;

    // Initialize EDR Network Monitor
    CAResult_t err = CAEDRInitializeNetworkMonitor();
    if (CA_STATUS_OK != err)
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "EDR N/w Monitor Initialize failed!, error number [%d]",
                  err);
        return err;
    }

    CAEDRSetNetworkChangeCallback(CAEDRNotifyNetworkStatus);
    CAEDRSetPacketReceivedCallback(CAAdapterRecvData);
    CAEDRInitializeClient();

    CAConnectivityHandler_t handler;
    handler.startAdapter = CAStartEDR;
    handler.startListenServer = CAStartEDRListeningServer;
    handler.startDiscoverServer = CAStartEDRDiscoveryServer;
    handler.sendData = CASendEDRUnicastData;
    handler.sendDataToAll = CASendEDRMulticastData;
    handler.GetnetInfo = CAGetEDRInterfaceInformation;
    handler.readData = CAReadEDRData;
    handler.stopAdapter = CAStopEDR;
    handler.terminate = CATerminateEDR;
    registerCallback(handler, CA_EDR);

    // Initialize Send/Receive data message queues
    if (CA_STATUS_OK != CAEDRInitializeQueueHandlers())
    {
        OIC_LOG(ERROR, EDR_ADAPTER_TAG, "CAAdapterInitializeQueues API failed");
        CATerminateEDR();
        return CA_STATUS_FAILED;
    }

    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAStartEDR(void)
{
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN");

    //Start Monitoring EDR Network
    CAResult_t ret = CAEDRStartNetworkMonitor();
    if(CA_STATUS_OK != ret)
    {
       OIC_LOG(ERROR, EDR_ADAPTER_TAG, "Failed to Start n/w monitor");
    }

    // Get Bluetooth adapter state
    CABool_t adapterState = CA_FALSE;
    if (CA_STATUS_OK != CAEDRGetAdapterEnableState(&adapterState))
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Failed to get adapter enable state");
        return CA_STATUS_FAILED;
    }

    if (CA_FALSE == adapterState)
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Bluetooth adapter is disabled!");
        gAdapterState = CA_FALSE;
        return CA_STATUS_OK;
    }

    if (CA_STATUS_OK != (ret = CAEDRClientSetCallbacks()))
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Start Network Monitor failed!, error number [%d] ",
                  ret);
    }

    if (CA_STATUS_OK != (ret = CAAdapterStartQueue()))
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "CAAdapterStartQueue failed!, error number [%d] ",
                  ret);
    }

    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "OUT");
    return ret;
}

CAResult_t CAStartEDRListeningServer(void)
{
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN");

    return CAStartServer();
}

CAResult_t CAStartEDRDiscoveryServer(void)
{
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN");

    return CAStartServer();
}

uint32_t CASendEDRUnicastData(const CARemoteEndpoint_t *remoteEndpoint, void *data,
                              uint32_t dataLength)
{
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN");

    CAResult_t err = CA_STATUS_OK;

    // Input validation
    VERIFY_NON_NULL_RET(remoteEndpoint, EDR_ADAPTER_TAG, "Remote endpoint is null", 0);
    VERIFY_NON_NULL_RET(data, EDR_ADAPTER_TAG, "Data is null", 0);

    if (0 == strlen(remoteEndpoint->addressInfo.BT.btMacAddress))
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Invalid input: EDR Address is empty!");
        return 0;
    }

    if (0 == dataLength)
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Invalid input: data length is zero!");
        return 0;
    }

    uint32_t sentLength = 0;
    const char *serviceUUID = OIC_EDR_SERVICE_ID;
    const char *address = remoteEndpoint->addressInfo.BT.btMacAddress;
    if (CA_STATUS_OK != (err = CAAdapterSendData(address, serviceUUID, data,
                               dataLength, &sentLength)))
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Send unicast data failed!, error num [%d]", err);
        return 0;
    }

    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "OUT");
    return sentLength;
}

uint32_t CASendEDRMulticastData(void *data, uint32_t dataLength)
{
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN");

    CAResult_t err = CA_STATUS_OK;

    // Input validation
    VERIFY_NON_NULL_RET(data, EDR_ADAPTER_TAG, "Data is null", 0);

    if (0 == dataLength)
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Invalid input: data length is zero!");
        return 0;
    }

    uint32_t sentLen = 0;
    const char *serviceUUID = OIC_EDR_SERVICE_ID;
    if (CA_STATUS_OK != (err = CAAdapterSendData(NULL, serviceUUID, data, dataLength,
                               &sentLen)))
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Send multicast data failed!, error num [%d]",
                  err);
        return 0;
    }

    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "OUT");
    return sentLen;
}


CAResult_t CAGetEDRInterfaceInformation(CALocalConnectivity_t **info, uint32_t *size)
{
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN");

    VERIFY_NON_NULL(info, EDR_ADAPTER_TAG, "LocalConnectivity info is null");

    CAResult_t err = CA_STATUS_OK;
    *size = 0;
    if (CA_STATUS_OK != (err = CAEDRGetInterfaceInformation(info)))
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG,
                  "Failed to get local interface information!, error num [%d]", err);
        return err;
    }

    *size = 1;
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "OUT");
    return err;
}

CAResult_t CAReadEDRData(void)
{
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN");

    return CAEDRManagerReadData();
}

CAResult_t CAStopEDR(void)
{
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN");

    // Stop RFComm server if it is running
    if (-1 != gServerId)
    {
        CAEDRServerStop(gServerId);
        gServerId = -1;
    }

    // Stop network monitor
    CAEDRStopNetworkMonitor();

    // Stop the adapter
    CAEDRClientUnsetCallbacks();

    // Disconnect all the client connections
    CAEDRClientDisconnectAll();

    // Stop Send and receive Queue
    CAAdapterStopQueue();

    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CATerminateEDR(void)
{
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN");

    // Terminate EDR Network Monitor
    CAEDRTerminateNetworkMonitor();

    // Terminate BT Client
    CAEDRTerminateClient();

    // Terminate Send/Receive data messages queues
    CAAdapterTerminateQueues();

    gNetworkPacketReceivedCallback = NULL;
    gNetworkChangeCallback = NULL;

    // Terminate thread pool
    gEDRThreadPool = NULL;

    // Free LocalConnectivity information
    CAAdapterFreeLocalEndpoint(gLocalConnectivity);
    gLocalConnectivity = NULL;

    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "OUT");
}

CAResult_t CAStartServer(void)
{
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN");

    CAResult_t err = CA_STATUS_OK;

    if (CA_FALSE == gAdapterState)
    {
        OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "Bluetooth adapter is disabled!");
        // Setting gServerState for starting Rfcommserver when adapter starts
        gServerState = TRUE;
        return CA_STATUS_OK;
    }

    if (-1 < gServerId)
    {
        OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "Server is already in running state.");
        return CA_STATUS_OK;
    }

    if (CA_STATUS_OK != (err = CAEDRServerStart(OIC_EDR_SERVICE_ID, &gServerId)))
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Failed to start RFCOMM server!, error num [%d]",
                  err);
        return err;
    }

    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "OUT");
    return err;
}

CAResult_t CAEDRInitializeQueueHandlers(void)
{
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN");

    if (CA_STATUS_OK == CAEDRInitializeSendHandler()
        && CA_STATUS_OK == CAEDRInitializeReceiveHandler())
    {
        OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "Queue is initialized!");
        return CA_STATUS_OK;
    }

    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "OUT");
    return CA_STATUS_FAILED;
}

CAResult_t CAEDRInitializeSendHandler(void)
{
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN");
    // Check if the message queue is already initialized
    if (gSendQueueHandle)
    {
        OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "Already queue is initialized!");
        return CA_STATUS_OK;
    }

    gSendQueueHandle = OICMalloc(sizeof(CAQueueingThread_t));
    if (!gSendQueueHandle)
    {
        OIC_LOG(ERROR, EDR_ADAPTER_TAG, "Memory allocation failed!");
        return CA_MEMORY_ALLOC_FAILED;
    }

    if (CA_STATUS_OK != CAQueueingThreadInitialize(gSendQueueHandle, gEDRThreadPool,
            CAAdapterDataSendHandler, NULL))
    {
        OIC_LOG(ERROR, EDR_ADAPTER_TAG, "Failed to Initialize send queue thread");
        return CA_STATUS_FAILED;
    }
    return CA_STATUS_OK;
}

CAResult_t CAEDRInitializeReceiveHandler(void)
{
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN");
    // Check if the message queue is already initialized
    if (gRecvQueueHandle)
    {
        OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "Already queue is initialized!");
        return CA_STATUS_OK;
    }

    gRecvQueueHandle = OICMalloc(sizeof(CAQueueingThread_t));
    if (!gRecvQueueHandle)
    {
        OIC_LOG(ERROR, EDR_ADAPTER_TAG, "Memory allocation failed!");
        return CA_MEMORY_ALLOC_FAILED;
    }

    if (CA_STATUS_OK != CAQueueingThreadInitialize(gRecvQueueHandle, gEDRThreadPool,
            CAAdapterDataReceiverHandler, NULL))
    {
        OIC_LOG(ERROR, EDR_ADAPTER_TAG, "Failed to Initialize send queue thread");
        return CA_STATUS_FAILED;
    }

    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CAAdapterTerminateQueues(void)
{
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN");

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

    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "OUT");
}

void CAAdapterDataSendHandler(void *context)
{
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN");

    CAEDRData *message = (CAEDRData *) context;
    if (message == NULL)
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Failed to get message!");
        return;
    }
    const char *remoteAddress = NULL;
    const char *serviceUUID = NULL;
    uint32_t sentLength = 0;

    remoteAddress = message->remoteEndpoint->addressInfo.BT.btMacAddress;
    serviceUUID = message->remoteEndpoint->resourceUri;

    char *dataSegment = NULL;
    int32_t datalen = message->dataLen;
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "checking for fragmentation and the dataLen is %d",
              datalen);


    char *header = (char *) OICMalloc(sizeof(char) * CA_HEADER_LENGTH);
    VERIFY_NON_NULL_VOID(header, EDR_ADAPTER_TAG, "Malloc failed");

    dataSegment = (char *) OICMalloc(sizeof(char) * message->dataLen + CA_HEADER_LENGTH);
    if (NULL == dataSegment)
    {
        OIC_LOG(ERROR, EDR_ADAPTER_TAG, "Malloc failed");
        OICFree(header);
        return;
    }

    memset(header, 0x0, sizeof(char) * CA_HEADER_LENGTH );
    memset(dataSegment, 0x0, sizeof(char) * message->dataLen );

    CAResult_t result = CAGenerateHeader(header, message->dataLen);
    if (CA_STATUS_OK != result )
    {
        OIC_LOG(ERROR, EDR_ADAPTER_TAG, "Generate header failed");
        OICFree(header);
        OICFree(dataSegment);
        return ;
    }

    memcpy(dataSegment, header, CA_HEADER_LENGTH);
    OICFree(header);
    int32_t length = 0;
    if (CA_SUPPORTED_BLE_MTU_SIZE >= message->dataLen)
    {
        length = message->dataLen + CA_HEADER_LENGTH;
        OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "length [%d]", length);
        memcpy(dataSegment + CA_HEADER_LENGTH, message->data, message->dataLen);
    }
    else
    {
        length =  CA_SUPPORTED_BLE_MTU_SIZE;
        OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "length [%d]", length);
        memcpy(dataSegment + CA_HEADER_LENGTH, message->data,
               CA_SUPPORTED_BLE_MTU_SIZE - CA_HEADER_LENGTH);
    }

    int32_t iter = message->dataLen / CA_SUPPORTED_BLE_MTU_SIZE;
    int32_t index = 0;
    if (CA_STATUS_OK != CAEDRClientSendData(remoteAddress, serviceUUID,
                                                dataSegment, length, &sentLength))
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "CAEDRClientSendData API failed");
        OICFree(dataSegment);
        return;
    }
    OICFree(dataSegment);

    for (index = 1; index < iter; index++)
    {
        // Send the remaining header.
        OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "Sending the chunk number [%d]", index);

        if (CA_STATUS_OK != CAEDRClientSendData(remoteAddress, serviceUUID,
                    message->data + ((index * CA_SUPPORTED_BLE_MTU_SIZE) - CA_HEADER_LENGTH),
                    CA_SUPPORTED_BLE_MTU_SIZE, &sentLength))
        {
            OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "CAEDRClientSendData API failed");
            return;
        }
    }

    if (message->dataLen / CA_SUPPORTED_BLE_MTU_SIZE)
    {
        // send the last segment of the data (Ex: 22 bytes of 622 bytes of data when MTU is 200)
        OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "Sending the last chunk");
        if (CA_STATUS_OK != CAEDRClientSendData(remoteAddress, serviceUUID,
                    message->data + (index * CA_SUPPORTED_BLE_MTU_SIZE) - CA_HEADER_LENGTH,
                    message->dataLen % CA_SUPPORTED_BLE_MTU_SIZE + CA_HEADER_LENGTH, &sentLength))
        {
            OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "CAEDRClientSendData API failed");
            return;
        }
    }

    // Free message
    CAFreeEDRData(message);

    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "OUT");
}

CAResult_t CAEDRClientSendData(const char *remoteAddress, const char *serviceUUID,
                                      void *data, uint32_t dataLength, uint32_t *sentLength)
{
    // Send the first segment with the header.
    if (strlen(remoteAddress)) //Unicast data
    {
        if (CA_STATUS_OK != CAEDRClientSendUnicastData(remoteAddress, serviceUUID,
                data, dataLength, sentLength))
        {
            OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Failed to send unicast data !");
            return CA_STATUS_FAILED;
        }
    }
    else
    {
        OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "sending multicast data");
        if (CA_STATUS_OK != CAEDRClientSendMulticastData(serviceUUID, data,
                dataLength, sentLength))
        {
            OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Failed to send multicast data !");
            return CA_STATUS_FAILED;
        }
    }
    return CA_STATUS_OK;
}
void CAAdapterDataReceiverHandler(void *context)
{
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN");


    CAEDRData *message = (CAEDRData *) context;
    if (message == NULL)
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Failed to get message!");
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
            OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Out of memory (header)!");
            return;
        }
        memcpy(header, (char *)message->data, CA_HEADER_LENGTH);
        totalDataLen = CAParseHeader(header);
        OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "Total data to be accumulated [%d] bytes",
                  totalDataLen);
        defragData = NULL;
        if (totalDataLen == 0)
        {
            OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, " totalDataLen is zero");
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
        isHeaderAvailable = CA_TRUE;
    }
    else
    {
        memcpy(defragData + recvDataLen, message->data, message->dataLen);
        recvDataLen += message->dataLen ;
    }
    CAFreeEDRData(message);
    if (totalDataLen == recvDataLen)
    {
        OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "Sending data up !");
        gNetworkPacketReceivedCallback(remoteEndpoint, defragData, recvDataLen);
        recvDataLen = 0;
        totalDataLen = 0;
        remoteEndpoint = NULL;
        isHeaderAvailable = CA_FALSE;
    }

    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "OUT");
}

CAResult_t CAAdapterStartQueue()
{
    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "IN");
    // Start send queue thread
    if (CA_STATUS_OK != CAQueueingThreadStart(gSendQueueHandle))
    {
        OIC_LOG(ERROR, EDR_ADAPTER_TAG, "Failed to Start Send Data Thread");
        CAEDRClientUnsetCallbacks();
        //Disconnect all the client connections
        CAEDRClientDisconnectAll();
        return CA_STATUS_FAILED;
    }

    // Start receive queue thread
    if (CA_STATUS_OK != CAQueueingThreadStart(gRecvQueueHandle))
    {
        OIC_LOG(ERROR, EDR_ADAPTER_TAG, "Failed to Start Receive Data Thread");
        CAEDRClientUnsetCallbacks();
        //Disconnect all the client connections
        CAEDRClientDisconnectAll();
        return CA_STATUS_FAILED;
    }
    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAAdapterStopQueue()
{
    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "IN");
    //Stop send queue thread
    CAQueueingThreadStop(gSendQueueHandle);

    //Stop receive queue thread
    CAQueueingThreadStop(gRecvQueueHandle);
    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CAAdapterRecvData(const char *remoteAddress, void *data,
                                    uint32_t dataLength, uint32_t *sentLength)
{
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN");

    if (CA_FALSE == gAdapterState)
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Bluetooth adapter is disabled!");
        *sentLength = 0;
        return;
    }

    char *serviceUUID = OIC_EDR_SERVICE_ID;

    // Input validation
    VERIFY_NON_NULL_VOID(serviceUUID, EDR_ADAPTER_TAG, "service UUID is null");
    VERIFY_NON_NULL_VOID(data, EDR_ADAPTER_TAG, "Data is null");
    VERIFY_NON_NULL_VOID(sentLength, EDR_ADAPTER_TAG, "Sent data length holder is null");

    // Create remote endpoint
    CARemoteEndpoint_t *remoteEndpoint = CAAdapterCreateRemoteEndpoint(CA_EDR, remoteAddress,
                                         serviceUUID);
    if (NULL == remoteEndpoint)
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Failed to create remote endpoint !");
        return;
    }

    // Add message to data queue
    CAEDRData *edrData =  CACreateEDRData(remoteEndpoint, data, dataLength);
    CAQueueingThreadAddData(gRecvQueueHandle, edrData, sizeof(CARemoteEndpoint_t));
    *sentLength = dataLength;

    // Free remote endpoint
    CAAdapterFreeRemoteEndpoint(remoteEndpoint);

    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "OUT");
    return;
}

CAResult_t CAAdapterSendData(const char *remoteAddress, const char *serviceUUID,
                               void *data, uint32_t dataLength, uint32_t *sentLength)
{
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN");

    if (CA_FALSE == gAdapterState)
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Bluetooth adapter is disabled!");
        *sentLength = 0;
        return CA_STATUS_OK;
    }
    // Input validation
    VERIFY_NON_NULL(serviceUUID, EDR_ADAPTER_TAG, "service UUID is null");
    VERIFY_NON_NULL(data, EDR_ADAPTER_TAG, "Data is null");
    VERIFY_NON_NULL(sentLength, EDR_ADAPTER_TAG, "Sent data length holder is null");

    // Create remote endpoint
    CARemoteEndpoint_t *remoteEndpoint = CAAdapterCreateRemoteEndpoint(CA_EDR, remoteAddress,
                                         serviceUUID);
    if (NULL == remoteEndpoint)
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Failed to create remote endpoint !");
        return CA_STATUS_FAILED;
    }

    // Add message to data queue
    CAEDRData *edrData =  CACreateEDRData(remoteEndpoint, data, dataLength);
    CAQueueingThreadAddData(gSendQueueHandle, edrData, sizeof(CARemoteEndpoint_t));
    *sentLength = dataLength;

    // Free remote endpoint
    CAAdapterFreeRemoteEndpoint(remoteEndpoint);

    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CAEDRNotifyNetworkStatus(CANetworkStatus_t status)
{
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN");

    // Create localconnectivity
    if (NULL == gLocalConnectivity)
    {
        CAEDRGetInterfaceInformation(&gLocalConnectivity);
    }

    if(CA_INTERFACE_UP == status)
    {
        if(CA_FALSE == gAdapterState)
        {
            // Get Bluetooth adapter state
            CABool_t adapterState = CA_FALSE;
            if (CA_STATUS_OK != CAEDRGetAdapterEnableState(&adapterState))
            {
                OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Failed to get adapter enable state");
                return;
            }

            if (CA_FALSE== adapterState)
            {
                OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Bluetooth adapter is disabled!");
                gAdapterState = CA_FALSE;
                return;
            }
            CAEDRClientSetCallbacks();
            gAdapterState = CA_TRUE;
            CAAdapterStartQueue();
            // starting RFCommServer
            if(CA_TRUE == gServerState)
            {
                CAStartServer();
                gServerState = CA_FALSE;
            }
        }
    }
    else
    {
        gAdapterState = CA_FALSE;
    }

    // Notity to upper layer
    if (gNetworkChangeCallback && gLocalConnectivity && gEDRThreadPool)
    {
        // Add notification task to thread pool
        CAEDRNetworkEvent *event = CAEDRCreateNetworkEvent(gLocalConnectivity, status);
        if (NULL != event)
        {
            if (CA_STATUS_OK != u_thread_pool_add_task(gEDRThreadPool, CAEDROnNetworkStatusChanged,
                    event))
            {
                OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Failed to create threadpool!");
                return;
            }
        }
    }

    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "OUT");
}

void CAEDROnNetworkStatusChanged(void *context)
{
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "IN");

    if (NULL == context)
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "context is NULL!");
        return;
    }

    CAEDRNetworkEvent *networkEvent = (CAEDRNetworkEvent *) context;

    // Notity to upper layer
    if (gNetworkChangeCallback)
    {
        gNetworkChangeCallback(networkEvent->info, networkEvent->status);
    }

    // Free the created Network event
    CAEDRFreeNetworkEvent(networkEvent);

    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "OUT");
}

CAEDRNetworkEvent *CAEDRCreateNetworkEvent(CALocalConnectivity_t *connectivity,
                                           CANetworkStatus_t status)
{
    VERIFY_NON_NULL_RET(connectivity, EDR_ADAPTER_TAG, "connectivity is NULL", NULL);

    // Create CAEDRNetworkEvent
    CAEDRNetworkEvent *event = (CAEDRNetworkEvent *) OICMalloc(sizeof(CAEDRNetworkEvent));
    if (NULL == event)
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Failed to allocate memory to network event!");
        return NULL;
    }

    // Create duplicate of Local connectivity
    event->info = CAAdapterCopyLocalEndpoint(connectivity);
    event->status = status;
    return event;
}

void CAEDRFreeNetworkEvent(CAEDRNetworkEvent *event)
{
    if (event)
    {
        CAAdapterFreeLocalEndpoint(event->info);
        OICFree(event);
    }
}

CAEDRData *CACreateEDRData(const CARemoteEndpoint_t *remoteEndpoint, void *data,
                           uint32_t dataLength)
{
    CAEDRData *edrData = (CAEDRData *) OICMalloc(sizeof(CAEDRData));
    if (!edrData)
    {
        OIC_LOG(ERROR, EDR_ADAPTER_TAG, "Memory allocation failed!");
        return NULL;
    }

    edrData->remoteEndpoint = CAAdapterCopyRemoteEndpoint(remoteEndpoint);
    edrData->data = (void *)OICMalloc(dataLength);
    if (NULL == edrData->data)
    {
        OIC_LOG(ERROR, EDR_ADAPTER_TAG, "Memory allocation failed!");
        CAFreeEDRData(edrData);
        return NULL;
    }
    memcpy(edrData->data, data, dataLength);
    edrData->dataLen = dataLength;

    return edrData;
}

void CAFreeEDRData(CAEDRData *edrData)
{
    if (!edrData)
        return;

    CAAdapterFreeRemoteEndpoint(edrData->remoteEndpoint);
    OICFree(edrData->data);
    OICFree(edrData);
}


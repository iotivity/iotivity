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
 * @file
 *
 * This file contains the APIs for EDR adapter.
 */

#include "caedradapter.h"

#include "caedrinterface.h"
#include "caadapterutils.h"
#include "logger.h"
#include "cafragmentation.h"
#include "caqueueingthread.h"
#include "oic_malloc.h"
#include "caremotehandler.h"

/**
 * @var EDR_ADAPTER_TAG
 * @brief Logging tag for module name.
 */
#define EDR_ADAPTER_TAG "CA_EDR_ADAPTER"

/**
 * @var g_edrThreadPool
 * @brief Reference to threadpool.
 */
static ca_thread_pool_t g_edrThreadPool = NULL;

/**
 * @var g_sendQueueHandle
 * @brief Queue handle for Send Data
 */
static CAQueueingThread_t *g_sendQueueHandle = NULL;

/**
 * @var g_recvQueueHandle
 * @brief Queue handle for Receive Data
 */
static CAQueueingThread_t *g_recvQueueHandle = NULL;

/**
 * @var g_isHeaderAvailable
 * @brief to differentiate btw header and data packet.
 */
static bool g_isHeaderAvailable = false;

/**
 * @var g_adapterState
 * @brief Storing Adapter state information
 */
static bool g_adapterState = true;

/**
 * @var g_networkPacketReceivedCallback
 * @brief Maintains the callback to be notified on receival of network packets from other
 *          Bluetooth devices.
 */
static CANetworkPacketReceivedCallback g_networkPacketReceivedCallback = NULL;

/**
 * @var g_networkChangeCallback
 * @brief Maintains the callback to be notified on local bluetooth adapter status change.
 */
static CANetworkChangeCallback g_networkChangeCallback = NULL;

/**
 * @var g_errorCallback
 * @brief error Callback to CA adapter
 */
static CAErrorHandleCallback g_errorCallback = NULL;

/**
 * @var g_localConnectivity
 * @brief Information of local Bluetooth adapter.
 */
static CAEndpoint_t *g_localConnectivity = NULL;

/**
 * @var g_serverId
 * @brief Storing RfcommserverUUID
 */
static int g_serverId = -1;

/**
 * @var g_serverState
 * @brief Storing Rfcommserver state information
 */
static bool g_serverState = false;

static CAResult_t CAStartServer();
static CAResult_t CAEDRInitializeQueueHandlers();
CAResult_t CAEDRInitializeSendHandler();
CAResult_t CAEDRInitializeReceiveHandler();
void CAAdapterTerminateQueues();
void CAAdapterDataSendHandler(void *context);
void CAAdapterDataReceiverHandler(void *context);
CAResult_t CAAdapterStopQueue();
void CAAdapterRecvData(const char *remoteAddress, const void *data, uint32_t dataLength,
                       uint32_t *sentLength);
void CAEDRNotifyNetworkStatus(CANetworkStatus_t status);
void CAEDROnNetworkStatusChanged(void *context);
CAResult_t CAAdapterSendData(const char *remoteAddress, const char *serviceUUID, const void *data,
                             uint32_t dataLength, uint32_t *sentLength);
CAEDRNetworkEvent *CAEDRCreateNetworkEvent(CAEndpoint_t *connectivity,
                                           CANetworkStatus_t status);
CAResult_t CAEDRClientSendData(const char *remoteAddress, const char *serviceUUID,
                               const void *data, uint32_t dataLength, uint32_t *sentLength);
/**
 * @fn CACreateEDRData
 * @brief Helper function to create CAEDRData
 */
static CAEDRData *CACreateEDRData(const CAEndpoint_t *remoteEndpoint, const void *data,
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

static void CAEDRDataDestroyer(void *data, uint32_t size);

static void CAEDRErrorHandler(const char *remoteAddress, const char *serviceUUID, const void *data,
                              uint32_t dataLength, CAResult_t result);

CAResult_t CAInitializeEDR(CARegisterConnectivityCallback registerCallback,
                           CANetworkPacketReceivedCallback packetReceivedCallback,
                           CANetworkChangeCallback networkStateChangeCallback,
                           CAErrorHandleCallback errorCallback, ca_thread_pool_t handle)
{
    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "IN");

    // Input validation
    VERIFY_NON_NULL(registerCallback, EDR_ADAPTER_TAG, "register callback is NULL");
    VERIFY_NON_NULL(packetReceivedCallback, EDR_ADAPTER_TAG, "data receive callback is NULL");
    VERIFY_NON_NULL(networkStateChangeCallback, EDR_ADAPTER_TAG,
                    "network state change callback is NULL");
    VERIFY_NON_NULL(handle, EDR_ADAPTER_TAG, "Thread pool handle is NULL");

    // Register the callbacks

    g_edrThreadPool = handle;
    g_networkPacketReceivedCallback = packetReceivedCallback;
    g_networkChangeCallback = networkStateChangeCallback;
    g_errorCallback = errorCallback;

    // Initialize EDR Network Monitor
    CAResult_t err = CAEDRInitializeNetworkMonitor(handle);
    if (CA_STATUS_OK != err)
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "EDR N/w Monitor Initialize failed!, error number [%d]",
                  err);
        return err;
    }

    CAEDRSetNetworkChangeCallback(CAEDRNotifyNetworkStatus);
    CAEDRSetPacketReceivedCallback(CAAdapterRecvData);
    CAEDRSetErrorHandler(CAEDRErrorHandler);
    CAEDRInitializeClient(handle);

    CAConnectivityHandler_t handler;
    handler.startAdapter = CAStartEDR;
    handler.startListenServer = CAStartEDRListeningServer;
    handler.startDiscoveryServer = CAStartEDRDiscoveryServer;
    handler.sendData = CASendEDRUnicastData;
    handler.sendDataToAll = CASendEDRMulticastData;
    handler.GetnetInfo = CAGetEDRInterfaceInformation;
    handler.readData = CAReadEDRData;
    handler.stopAdapter = CAStopEDR;
    handler.terminate = CATerminateEDR;
    registerCallback(handler, CA_ADAPTER_RFCOMM_BTEDR);

    // Initialize Send/Receive data message queues
    if (CA_STATUS_OK != CAEDRInitializeQueueHandlers())
    {
        OIC_LOG(ERROR, EDR_ADAPTER_TAG, "CAAdapterInitializeQueues API failed");
        CATerminateEDR();
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAStartEDR()
{
    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "IN");

    //Start Monitoring EDR Network
    CAResult_t ret = CAEDRStartNetworkMonitor();
    if (CA_STATUS_OK != ret)
    {
       OIC_LOG(ERROR, EDR_ADAPTER_TAG, "Failed to Start n/w monitor");
    }

    // Get Bluetooth adapter state
    bool adapterState = false;
    if (CA_STATUS_OK != CAEDRGetAdapterEnableState(&adapterState))
    {
        OIC_LOG(ERROR, EDR_ADAPTER_TAG, "Failed to get adapter enable state");
        return CA_STATUS_FAILED;
    }

    if (false == adapterState)
    {
        OIC_LOG(ERROR, EDR_ADAPTER_TAG, "Bluetooth adapter is disabled!");
        g_adapterState = false;
        return CA_ADAPTER_NOT_ENABLED;
    }

    if (CA_STATUS_OK != (ret = CAEDRClientSetCallbacks()))
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Start Network Monitor failed!, error number [%d] ",
                  ret);
    }

    if (CA_STATUS_OK != (ret = CAAdapterStartQueue()))
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "CAAdapterStartQueue failed!, error number [%d] ", ret);
    }

    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "OUT");
    return ret;
}

CAResult_t CAStartEDRListeningServer()
{
    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "IN");

    return CAStartServer();
}

CAResult_t CAStartEDRDiscoveryServer()
{
    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "IN");

    return CAStartServer();
}

int32_t CASendEDRUnicastData(const CAEndpoint_t *remoteEndpoint, const void *data,
                             uint32_t dataLength)
{
    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "IN");

    // Input validation
    VERIFY_NON_NULL_RET(remoteEndpoint, EDR_ADAPTER_TAG, "Remote endpoint is null", -1);
    VERIFY_NON_NULL_RET(data, EDR_ADAPTER_TAG, "Data is null", -1);

    if (0 == dataLength)
    {
        OIC_LOG(ERROR, EDR_ADAPTER_TAG, "Invalid input: data length is zero!");
        return -1;
    }

    if (0 == strlen(remoteEndpoint->addr))
    {
        OIC_LOG(ERROR, EDR_ADAPTER_TAG, "Invalid input: EDR Address is empty!");
        return -1;
    }

    uint32_t sentLength = 0;
    const char *serviceUUID = OIC_EDR_SERVICE_ID;
    const char *address = remoteEndpoint->addr;
    CAResult_t err = CAAdapterSendData(address, serviceUUID, data, dataLength, &sentLength);
    if (CA_STATUS_OK != err)
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Send unicast data failed!, error num [%d]", err);
        g_errorCallback(remoteEndpoint, data, dataLength, err);
        return -1;
    }

    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "OUT");
    return sentLength;
}

int32_t CASendEDRMulticastData(const CAEndpoint_t *endpoint, const void *data, uint32_t dataLength)
{
    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "IN - CASendEDRMulticastData");

    // Input validation
    VERIFY_NON_NULL_RET(data, EDR_ADAPTER_TAG, "Data is null", -1);

    if (0 == dataLength)
    {
        OIC_LOG(ERROR, EDR_ADAPTER_TAG, "Invalid input: data length is zero!");
        return -1;
    }

    uint32_t sentLen = 0;
    const char *serviceUUID = OIC_EDR_SERVICE_ID;
    CAResult_t err = CAAdapterSendData(NULL, serviceUUID, data, dataLength, &sentLen);
    if (CA_STATUS_OK != err)
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Send multicast data failed!, error num [%d]", err);
        g_errorCallback(endpoint, data, dataLength, err);
        return -1;
    }

    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "OUT - CASendEDRMulticastData");
    return sentLen;
}

CAResult_t CAGetEDRInterfaceInformation(CAEndpoint_t **info, uint32_t *size)
{
    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "IN");

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
    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "OUT");
    return err;
}

CAResult_t CAReadEDRData()
{
    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "IN");

    return CAEDRManagerReadData();
}

CAResult_t CAStopEDR()
{
    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "IN");

    // Stop RFComm server if it is running
    if (-1 != g_serverId)
    {
        CAEDRServerStop(g_serverId);
        g_serverId = -1;
    }

    // Stop network monitor
    CAEDRStopNetworkMonitor();

    // Stop the adapter
    CAEDRClientUnsetCallbacks();

    // Disconnect all the client connections
    CAEDRClientDisconnectAll();

    // Stop Send and receive Queue
    CAAdapterStopQueue();

    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CATerminateEDR()
{
    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "IN");

    // Stop EDR adapter
    CAStopEDR();
    // Terminate EDR Network Monitor
    CAEDRTerminateNetworkMonitor();

    // Terminate Send/Receive data messages queues
    CAAdapterTerminateQueues();

    g_networkPacketReceivedCallback = NULL;
    g_networkChangeCallback = NULL;

    // Terminate thread pool
    g_edrThreadPool = NULL;

    // Terminate EDR Client
    CAEDRClientTerminate();

    // Terminate EDR Server
    CAEDRServerTerminate();

    // Free LocalConnectivity information
    CAFreeEndpoint(g_localConnectivity);
    g_localConnectivity = NULL;

    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "OUT");
}

CAResult_t CAStartServer()
{
    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "IN");

    CAResult_t err = CA_STATUS_OK;

    if (false == g_adapterState)
    {
        OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "Bluetooth adapter is disabled!");
        // Setting g_serverState for starting Rfcommserver when adapter starts
        g_serverState = true;
        return CA_STATUS_OK;
    }

    if (-1 < g_serverId)
    {
        OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "Server is already in running state.");
        return CA_SERVER_STARTED_ALREADY;
    }

    if (CA_STATUS_OK != (err = CAEDRServerStart(OIC_EDR_SERVICE_ID, &g_serverId, g_edrThreadPool)))
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Failed to start RFCOMM server!, error num [%d]",
                  err);
        return err;
    }

    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "OUT");
    return err;
}

CAResult_t CAEDRInitializeQueueHandlers()
{
    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "IN");

    if (CA_STATUS_OK == CAEDRInitializeSendHandler()
        && CA_STATUS_OK == CAEDRInitializeReceiveHandler())
    {
        OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "Queue is initialized!");
        return CA_STATUS_OK;
    }

    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "OUT");
    return CA_STATUS_FAILED;
}

CAResult_t CAEDRInitializeSendHandler()
{
    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "IN");
    // Check if the message queue is already initialized
    if (g_sendQueueHandle)
    {
        OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "Already queue is initialized!");
        return CA_STATUS_OK;
    }

    g_sendQueueHandle = (CAQueueingThread_t *) OICMalloc(sizeof(CAQueueingThread_t));
    if (!g_sendQueueHandle)
    {
        OIC_LOG(ERROR, EDR_ADAPTER_TAG, "Memory allocation failed!");
        return CA_MEMORY_ALLOC_FAILED;
    }

    if (CA_STATUS_OK != CAQueueingThreadInitialize(g_sendQueueHandle, g_edrThreadPool,
                                                   CAAdapterDataSendHandler, CAEDRDataDestroyer))
    {
        OIC_LOG(ERROR, EDR_ADAPTER_TAG, "Failed to Initialize send queue thread");
        return CA_STATUS_FAILED;
    }
    return CA_STATUS_OK;
}

CAResult_t CAEDRInitializeReceiveHandler()
{
    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "IN");
    // Check if the message queue is already initialized
    if (g_recvQueueHandle)
    {
        OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "Already queue is initialized!");
        return CA_STATUS_OK;
    }

    g_recvQueueHandle = (CAQueueingThread_t *) OICMalloc(sizeof(CAQueueingThread_t));
    if (!g_recvQueueHandle)
    {
        OIC_LOG(ERROR, EDR_ADAPTER_TAG, "Memory allocation failed!");
        return CA_MEMORY_ALLOC_FAILED;
    }

    if (CA_STATUS_OK != CAQueueingThreadInitialize(g_recvQueueHandle, g_edrThreadPool,
                                                   CAAdapterDataReceiverHandler,
                                                   CAEDRDataDestroyer))
    {
        OIC_LOG(ERROR, EDR_ADAPTER_TAG, "Failed to Initialize send queue thread");
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CAAdapterTerminateQueues()
{
    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "IN");

    if (g_sendQueueHandle)
    {
        CAQueueingThreadDestroy(g_sendQueueHandle);
        g_sendQueueHandle = NULL;
    }
    if (g_recvQueueHandle)
    {
        CAQueueingThreadDestroy(g_recvQueueHandle);
        g_recvQueueHandle = NULL;
    }

    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "OUT");
}

void CAAdapterDataSendHandler(void *context)
{
    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "IN - CAAdapterDataSendHandler");

    CAEDRData *message = (CAEDRData *) context;
    if (NULL == message)
    {
        OIC_LOG(ERROR, EDR_ADAPTER_TAG, "Failed to get message!");
        return;
    }

    const char *remoteAddress = NULL;
    const char *serviceUUID = OIC_EDR_SERVICE_ID;
    uint32_t sentLength = 0;

    if (NULL == message->remoteEndpoint)
    {
        OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "remoteEndpoint is not available");
        return;
    }
    else
    {
        remoteAddress = message->remoteEndpoint->addr;
    }

    if(!remoteAddress || !serviceUUID)
    {
        OIC_LOG(ERROR, EDR_ADAPTER_TAG, "EDR Send Message error");
        //Error cannot be sent if remote address is NULL
        return;
    }

    uint32_t dataSegmentLength = message->dataLen + CA_HEADER_LENGTH;
    uint32_t dataLen = message->dataLen;
    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "checking for fragmentation and the dataLen is %d",
              message->dataLen);

    char *header = (char *) OICCalloc(CA_HEADER_LENGTH, sizeof(char));
    VERIFY_NON_NULL_VOID(header, EDR_ADAPTER_TAG, "Memory allocation failed");

    char* dataSegment = (char *) OICCalloc(dataSegmentLength, sizeof(char));
    if (NULL == dataSegment)
    {
        OIC_LOG(ERROR, EDR_ADAPTER_TAG, "Memory allocation failed");
        CAEDRErrorHandler(remoteAddress, serviceUUID, message->data, message->dataLen, CA_SEND_FAILED);
        OICFree(header);
        return;
    }

    CAResult_t result = CAGenerateHeader(header, dataLen);
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, EDR_ADAPTER_TAG, "Generate header failed");
        OICFree(header);
        OICFree(dataSegment);
        CAEDRErrorHandler(remoteAddress, serviceUUID, message->data, message->dataLen, CA_SEND_FAILED);
        return ;
    }

    memcpy(dataSegment, header, CA_HEADER_LENGTH);
    OICFree(header);
    uint32_t length = 0;
    if (CA_SUPPORTED_EDR_MTU_SIZE > dataSegmentLength)
    {
        length = dataSegmentLength;
        memcpy(dataSegment + CA_HEADER_LENGTH, message->data, dataLen);
    }
    else
    {
        length =  CA_SUPPORTED_EDR_MTU_SIZE;
        memcpy(dataSegment + CA_HEADER_LENGTH, message->data,
               CA_SUPPORTED_EDR_MTU_SIZE - CA_HEADER_LENGTH);
    }

    uint32_t iter = dataSegmentLength / CA_SUPPORTED_EDR_MTU_SIZE;
    uint32_t index = 0;
    result = CAEDRClientSendData(remoteAddress, serviceUUID, dataSegment, length,
                                 &sentLength);
    if(CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, EDR_ADAPTER_TAG, "CAEDRClientSendData API failed");
        OICFree(dataSegment);
        CAEDRErrorHandler(remoteAddress, serviceUUID, message->data, message->dataLen, result);
        return;
    }

    OICFree(dataSegment);

    for (index = 1; index < iter; index++)
    {
        // Send the remaining header.
        OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "Sending the chunk number [%d]", index);

        void *dataPtr = message->data + ((index * CA_SUPPORTED_EDR_MTU_SIZE) - CA_HEADER_LENGTH);
        result = CAEDRClientSendData(remoteAddress, serviceUUID,
                                     dataPtr, CA_SUPPORTED_EDR_MTU_SIZE, &sentLength);
        if(CA_STATUS_OK != result)
        {
            OIC_LOG(ERROR, EDR_ADAPTER_TAG, "CAEDRClientSendData API failed");
            CAEDRErrorHandler(remoteAddress, serviceUUID, message->data, message->dataLen, result);
            return;
        }
    }

    uint32_t remainingLen = dataSegmentLength % CA_SUPPORTED_EDR_MTU_SIZE;
    if (remainingLen &&  (dataSegmentLength > CA_SUPPORTED_EDR_MTU_SIZE))
    {
        // send the last segment of the data (Ex: 22 bytes of 622 bytes of data when MTU is 200)
        OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "Sending the last chunk");
        void *dataPtr = message->data + ((index * CA_SUPPORTED_EDR_MTU_SIZE) - CA_HEADER_LENGTH);
        result = CAEDRClientSendData(remoteAddress, serviceUUID, dataPtr,
                                     remainingLen, &sentLength);
        if(CA_STATUS_OK != result)
        {
            OIC_LOG(ERROR, EDR_ADAPTER_TAG, "CAEDRClientSendData API failed");
            CAEDRErrorHandler(remoteAddress, serviceUUID, message->data, message->dataLen, result);
            return;
        }
    }

    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "OUT");
}

CAResult_t CAEDRClientSendData(const char *remoteAddress, const char *serviceUUID,
                               const void *data, uint32_t dataLength, uint32_t *sentLength)
{

    CAResult_t result = CA_SEND_FAILED;

    // Send the first segment with the header.
    if ((NULL != remoteAddress) && (0 < strlen(remoteAddress))) //Unicast data
    {
        result = CAEDRClientSendUnicastData(remoteAddress, serviceUUID, data,
                                            dataLength, sentLength);
        if (CA_STATUS_OK != result)
        {
            OIC_LOG(ERROR, EDR_ADAPTER_TAG, "Failed to send unicast data !");
            return result;
        }
    }
    else
    {
        OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "sending multicast data : %s", data);
        result = CAEDRClientSendMulticastData(serviceUUID, data, dataLength,
                                              sentLength);

        if (CA_STATUS_OK != result)
        {
            OIC_LOG(ERROR, EDR_ADAPTER_TAG, "Failed to send multicast data !");
            return result;
        }
    }
    return result;
}

void CAAdapterDataReceiverHandler(void *context)
{
    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "IN_CAAdapterDataReceiverHandler");

    CAEDRData *message = (CAEDRData *) context;
    if (NULL == message)
    {
        OIC_LOG(ERROR, EDR_ADAPTER_TAG, "Failed to get message!");
        return;
    }

    static uint32_t recvDataLen = 0;
    static uint32_t totalDataLen = 0;
    static char *defragData = NULL;
    static CAEndpoint_t *remoteEndpoint = NULL;

    if (!g_isHeaderAvailable)
    {
        totalDataLen = CAParseHeader((char*)message->data);
        if (0 == totalDataLen)
        {
            OIC_LOG(ERROR, EDR_ADAPTER_TAG, "totalDataLen is zero");
            return;
        }

        OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "Total data to be accumulated [%d] bytes", totalDataLen);

        defragData = (char *) OICMalloc(sizeof(char) * totalDataLen);
        if (!defragData)
        {
            OIC_LOG(ERROR, EDR_ADAPTER_TAG, "defragData is null");
            return;
        }

        const char *remoteAddress = message->remoteEndpoint->addr;

        remoteEndpoint = CACreateEndpointObject(CA_DEFAULT_FLAGS, CA_ADAPTER_RFCOMM_BTEDR,
                                                remoteAddress, 0);

        memcpy(defragData + recvDataLen, message->data + CA_HEADER_LENGTH,
               message->dataLen - CA_HEADER_LENGTH);
        recvDataLen += message->dataLen - CA_HEADER_LENGTH;
        g_isHeaderAvailable = true;
    }
    else
    {
        memcpy(defragData + recvDataLen, message->data, message->dataLen);
        recvDataLen += message->dataLen ;
    }

    if (totalDataLen == recvDataLen)
    {
        OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "Sending data up !");
        g_networkPacketReceivedCallback(remoteEndpoint, defragData, recvDataLen);
        recvDataLen = 0;
        totalDataLen = 0;
        remoteEndpoint = NULL;
        g_isHeaderAvailable = false;
    }

    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "OUT_CAAdapterDataReceiverHandler");
}

CAResult_t CAAdapterStartQueue()
{
    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "IN");
    // Start send queue thread
    if (CA_STATUS_OK != CAQueueingThreadStart(g_sendQueueHandle))
    {
        OIC_LOG(ERROR, EDR_ADAPTER_TAG, "Failed to Start Send Data Thread");
        CAEDRClientUnsetCallbacks();
        //Disconnect all the client connections
        CAEDRClientDisconnectAll();
        return CA_STATUS_FAILED;
    }

    // Start receive queue thread
    if (CA_STATUS_OK != CAQueueingThreadStart(g_recvQueueHandle))
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
    CAQueueingThreadStop(g_sendQueueHandle);

    //Stop receive queue thread
    CAQueueingThreadStop(g_recvQueueHandle);
    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CAAdapterRecvData(const char *remoteAddress, const void *data, uint32_t dataLength,
                       uint32_t *sentLength)
{
    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "IN");

    if (false == g_adapterState)
    {
        OIC_LOG_V(ERROR, EDR_ADAPTER_TAG, "Bluetooth adapter is disabled!");
        *sentLength = 0;
        return;
    }

    // Input validation
    VERIFY_NON_NULL_VOID(data, EDR_ADAPTER_TAG, "Data is null");
    VERIFY_NON_NULL_VOID(sentLength, EDR_ADAPTER_TAG, "Sent data length holder is null");

    // Create remote endpoint
    CAEndpoint_t *remoteEndpoint = CACreateEndpointObject(CA_DEFAULT_FLAGS,
                                                          CA_ADAPTER_RFCOMM_BTEDR,
                                                          remoteAddress, 0);
    if (NULL == remoteEndpoint)
    {
        OIC_LOG(ERROR, EDR_ADAPTER_TAG, "Failed to create remote endpoint !");
        return;
    }

    // Add message to data queue
    CAEDRData *edrData =  CACreateEDRData(remoteEndpoint, data, dataLength);
    CAQueueingThreadAddData(g_recvQueueHandle, edrData, sizeof(CAEDRData));
    *sentLength = dataLength;

    // Free remote endpoint
    CAFreeEndpoint(remoteEndpoint);

    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "OUT");
}

void CAEDRErrorHandler(const char *remoteAddress, const char *serviceUUID, const void *data,
                       uint32_t dataLength, CAResult_t result)
{
    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "IN");

    // Input validation
    VERIFY_NON_NULL_VOID(data, EDR_ADAPTER_TAG, "Data is null");

    // Create remote endpoint
    CAEndpoint_t *remoteEndpoint = CACreateEndpointObject(0, CA_ADAPTER_RFCOMM_BTEDR,
                                                           remoteAddress, 0);
    if (!remoteEndpoint)
    {
        OIC_LOG(ERROR, EDR_ADAPTER_TAG, "Failed to create remote endpoint !");
        return;
    }

    g_errorCallback(remoteEndpoint, data, dataLength, result);

    // Free remote endpoint
    CAFreeEndpoint(remoteEndpoint);

    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "OUT");
}

CAResult_t CAAdapterSendData(const char *remoteAddress, const char *serviceUUID, const void *data,
                             uint32_t dataLength, uint32_t *sentLength)
{
    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "IN - CAAdapterSendData");

    if (false == g_adapterState)
    {
        OIC_LOG(ERROR, EDR_ADAPTER_TAG, "Bluetooth adapter is disabled!");
        *sentLength = 0;
        return CA_ADAPTER_NOT_ENABLED;
    }
    // Input validation
    VERIFY_NON_NULL(serviceUUID, EDR_ADAPTER_TAG, "service UUID is null");
    VERIFY_NON_NULL(data, EDR_ADAPTER_TAG, "Data is null");
    VERIFY_NON_NULL(sentLength, EDR_ADAPTER_TAG, "Sent data length holder is null");

    // Create remote endpoint
    CAEndpoint_t *remoteEndpoint = CACreateEndpointObject(CA_DEFAULT_FLAGS,
                                                          CA_ADAPTER_RFCOMM_BTEDR,
                                                          remoteAddress, 0);
    if (NULL == remoteEndpoint)
    {
        OIC_LOG(ERROR, EDR_ADAPTER_TAG, "Failed to create remote endpoint !");
        return CA_MEMORY_ALLOC_FAILED;
    }

    // Add message to data queue
    CAEDRData *edrData =  CACreateEDRData(remoteEndpoint, data, dataLength);
    CAQueueingThreadAddData(g_sendQueueHandle, edrData, sizeof (CAEDRData));
    *sentLength = dataLength;

    // Free remote endpoint
    CAFreeEndpoint(remoteEndpoint);

    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "OUT - CAAdapterSendData");
    return CA_STATUS_OK;
}

void CAEDRNotifyNetworkStatus(CANetworkStatus_t status)
{
    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "IN");

    // Create localconnectivity
    if (NULL == g_localConnectivity)
    {
        CAEDRGetInterfaceInformation(&g_localConnectivity);
    }

    if (CA_INTERFACE_UP == status)
    {
        if (false == g_adapterState)
        {
            // Get Bluetooth adapter state
            bool adapterState = false;
            if (CA_STATUS_OK != CAEDRGetAdapterEnableState(&adapterState))
            {
                OIC_LOG(ERROR, EDR_ADAPTER_TAG, "Failed to get adapter enable state");
                return;
            }

            if (false== adapterState)
            {
                OIC_LOG(ERROR, EDR_ADAPTER_TAG, "Bluetooth adapter is disabled!");
                g_adapterState = false;
                return;
            }
            CAEDRClientSetCallbacks();
            g_adapterState = true;
            CAAdapterStartQueue();
            // starting RFCommServer
            if (true == g_serverState)
            {
                CAStartServer();
                g_serverState = false;
            }
        }
    }
    else
    {
        g_adapterState = false;
    }

    // Notify to upper layer
    if (g_networkChangeCallback && g_localConnectivity && g_edrThreadPool)
    {
        // Add notification task to thread pool
        CAEDRNetworkEvent *event = CAEDRCreateNetworkEvent(g_localConnectivity, status);
        if (NULL != event)
        {
            if (CA_STATUS_OK != ca_thread_pool_add_task(g_edrThreadPool,
                                                        CAEDROnNetworkStatusChanged,event))
            {
                OIC_LOG(ERROR, EDR_ADAPTER_TAG, "Failed to create threadpool!");
                return;
            }
        }
    }

    OIC_LOG_V(DEBUG, EDR_ADAPTER_TAG, "OUT");
}

void CAEDROnNetworkStatusChanged(void *context)
{
    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "IN");

    if (NULL == context)
    {
        OIC_LOG(ERROR, EDR_ADAPTER_TAG, "context is NULL!");
        return;
    }

    CAEDRNetworkEvent *networkEvent = (CAEDRNetworkEvent *) context;

    // Notify to upper layer
    if (g_networkChangeCallback)
    {
        g_networkChangeCallback(networkEvent->info, networkEvent->status);
    }

    // Free the created Network event
    CAEDRFreeNetworkEvent(networkEvent);

    OIC_LOG(DEBUG, EDR_ADAPTER_TAG, "OUT");
}

CAEDRNetworkEvent *CAEDRCreateNetworkEvent(CAEndpoint_t *connectivity,
                                           CANetworkStatus_t status)
{
    VERIFY_NON_NULL_RET(connectivity, EDR_ADAPTER_TAG, "connectivity is NULL", NULL);

    // Create CAEDRNetworkEvent
    CAEDRNetworkEvent *event = (CAEDRNetworkEvent *) OICMalloc(sizeof(CAEDRNetworkEvent));
    if (NULL == event)
    {
        OIC_LOG(ERROR, EDR_ADAPTER_TAG, "Failed to allocate memory to network event!");
        return NULL;
    }

    // Create duplicate of Local connectivity
    event->info = CACloneEndpoint(connectivity);
    event->status = status;
    return event;
}

void CAEDRFreeNetworkEvent(CAEDRNetworkEvent *event)
{
    if (event)
    {
        CAFreeEndpoint(event->info);
        OICFree(event);
    }
}

CAEDRData *CACreateEDRData(const CAEndpoint_t *remoteEndpoint,
                                        const void *data, uint32_t dataLength)
{
    CAEDRData *edrData = (CAEDRData *)OICMalloc(sizeof (CAEDRData));
    if (!edrData)
    {
        OIC_LOG(ERROR, EDR_ADAPTER_TAG, "Memory allocation failed!");
        return NULL;
    }

    edrData->remoteEndpoint = CACloneEndpoint(remoteEndpoint);

    edrData->data = OICMalloc(dataLength);
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
    VERIFY_NON_NULL_VOID(edrData, EDR_ADAPTER_TAG, "edrData is NULL");

    CAFreeEndpoint(edrData->remoteEndpoint);
    OICFree(edrData->data);
    OICFree(edrData);
}

void CAEDRDataDestroyer(void *data, uint32_t size)
{
    CAEDRData *edrdata = (CAEDRData *) data;

    CAFreeEDRData(edrdata);
}


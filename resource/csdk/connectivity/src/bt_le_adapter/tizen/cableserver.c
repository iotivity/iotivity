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

#include "cableserver.h"
#include <pthread.h>
#include "cacommon.h"
#include "caadapterutils.h"
#include <gio/gio.h>
#include "umutex.h"
#include "camessagequeue.h"
#include "caadapterutils.h"
#include "camsgparser.h"
#include "cableclientutil.h"

/**
 * @def TZ_BLE_SERVER_TAG
 * @brief Logging tag for module name
 */
#define TZ_BLE_SERVER_TAG "TZ_BLE_GATT_SERVER"

///TODO:: Currently keeping as single service. Later service path will be added in list for supporting multiple services.

/**
 * @var gGattSvcPath
 * @brief attribute handler for OIC server attribute.
 */
static char *gGattSvcPath = NULL;

/**
 * @var gGattReadCharPath
 * @brief attribute handler for readCharacteristic of OIC server
 */
static char *gGattReadCharPath = NULL;

/**
 * @var gGattWriteCharPath
 * @brief attribute handler for writeCharacteristic of OIC server
 */
static char *gGattWriteCharPath = NULL;

/**
 * @var hAdvertiser
 * @brief handler for OIC advertiser.
 */
static bt_advertiser_h hAdvertiser = NULL;

/**
 * @var    gNetworkPacketReceivedServerCallback
 * @brief  Maintains the callback to be notified on receival of network packets from other
 *           BLE devices
 */
static CANetworkPacketReceivedCallback gNetworkPacketReceivedServerCallback = NULL;

/**
 * @var gCABleServerSenderQueue
 * @brief Queue to process the outgoing packets from GATTServer.
 */
static CAAdapterMessageQueue_t *gCABleServerSenderQueue = NULL;

/**
 * @var gCABleServerReceiverQueue
 * @brief Queue to process the incoming packets to GATTServer
 */
static CAAdapterMessageQueue_t *gCABleServerReceiverQueue = NULL;

/**
 * @var gIsBleGattServerStarted
 * @brief Boolean variable to keep the state of the GATTServer
 */
static CABool_t gIsBleGattServerStarted = CA_FALSE;

/**
 * @var gBleServerStateMutex
 * @brief Mutex to synchronize the calls to be done to the platform from GATTServer
 *           interfaces from different threads.
 */
static u_mutex gBleServerStateMutex = NULL;

/**
 * @var gBleCharacteristicMutex
 * @brief Mutex to synchronize writing operations on the characteristics.
 */
static  u_mutex gBleCharacteristicMutex = NULL;

/**
 * @var gBleServiceMutex
 * @brief  Mutex to synchronize to create the OIC service..
 */
static  u_mutex gBleServiceMutex = NULL;

/**
 * @var gBleReqRespCbMutex
 * @brief Mutex to synchronize access to the requestResponse callback to be called
 *           when the data needs to be sent from GATTClient.
 */
static  u_mutex gBleReqRespCbMutex = NULL;

/**
 * @var gBleServerSendDataMutex
 * @brief Mutex to synchronize the queing of the data from SenderQueue.
 */
static u_mutex gBleServerSendDataMutex = NULL;

/**
 * @var gBleServerThreadPoolMutex
 * @brief Mutex to synchronize the task to be pushed to thread pool.
 */
static u_mutex gBleServerThreadPoolMutex = NULL;

/**
 * @var gBleServerSendCondWait
 * @brief Condition used for notifying handler the presence of data in send queue.
 */
static u_cond gBleServerSendCondWait = NULL;

/**
 * @var gReceiverDataCond
 * @brief Condition used for notifying handler the presence of data in recv queue.
 */
static u_cond gReceiverDataCond = NULL;

/**
 * @var gDataReceiverHandlerState
 * @brief Stop condition of redvhandler.
 */
static bool gDataReceiverHandlerState = false;

/**
 * @var isHeaderAvailable
 * @brief to differentiate btw header and data packet.
 */
static CABool_t isHeaderAvailable = false;

/**
 * @struct gRemoteAddress
 * @brief Remote address of Gatt client
 *
 */
static const char *gRemoteClientAddress = "DB:F7:EB:B5:0F:07";
///TODO: later this will be replaced with the platform apis.
/**
 * @var gBleServerReceiveDataMutex
 * @brief Mutex to synchronize the queing of the data from ReceiverQueue.
 */
static u_mutex gBleServerReceiveDataMutex = NULL;

/**
 * @var g_event_loop
 * @brief gmainLoop to manage the threads to receive the callback from the platfrom.
 */
static GMainLoop *g_event_loop = NULL;

/**
 * @var gBleServerThreadPool
 * @brief reference to threadpool
 */
static u_thread_pool_t gBleServerThreadPool = NULL;

/**
 * @var gServerUp
 * @brief Boolean variable to maintain the GATT Server state.
 */
static CABool_t gServerUp = CA_FALSE;

/**
 * @fn CABLEDataReceiverHandler
 * @brief This function handles data from recv message queue.
 */
static void CABLEDataReceiverHandler(void *context);

CAResult_t CABLEPushDataToServerReceiverQueue(const char *remoteAddress, const char *serviceUUID,
        void *data, uint32_t dataLength, uint32_t *sentLength)
{
    OIC_LOG_V(DEBUG, TZ_BLE_SERVER_TAG, "IN");

    //Input validation
    VERIFY_NON_NULL(serviceUUID, TZ_BLE_SERVER_TAG, "service UUID is null");
    VERIFY_NON_NULL(data, TZ_BLE_SERVER_TAG, "Data is null");
    VERIFY_NON_NULL(sentLength, TZ_BLE_SERVER_TAG, "Sent data length holder is null");

    //Add message to data queue
    CARemoteEndpoint_t *remoteEndpoint = CAAdapterCreateRemoteEndpoint(CA_LE, remoteAddress,
                                         serviceUUID);
    if (NULL == remoteEndpoint)
    {
        OIC_LOG_V(ERROR, TZ_BLE_SERVER_TAG, "Failed to create remote endpoint !");
        return CA_STATUS_FAILED;
    }

    if (CA_STATUS_OK != CAAdapterEnqueueMessage(gCABleServerReceiverQueue, remoteEndpoint, data,
            dataLength))
    {
        OIC_LOG_V(ERROR, TZ_BLE_SERVER_TAG, "Failed to add message to queue !");

        CAAdapterFreeRemoteEndpoint(remoteEndpoint);
        return CA_STATUS_FAILED;
    }

    *sentLength = dataLength;

    OICFree(data);
    data = NULL;

    //Signal message handler for processing data for sending
    OIC_LOG_V(DEBUG, TZ_BLE_SERVER_TAG, "Signalling message send handler");
    u_mutex_lock(gBleServerReceiveDataMutex);
    u_cond_signal(gReceiverDataCond);
    u_mutex_unlock(gBleServerReceiveDataMutex);

    OIC_LOG_V(DEBUG, TZ_BLE_SERVER_TAG, "OUT");
    return CA_STATUS_OK;
}
void *CABleServerSenderQueueProcessor()
{
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "IN ");

    while (gServerUp)
    {
        CAAdapterMessage_t *message = NULL;
        const char *remoteAddress = NULL;
        const char *serviceUUID = NULL;
        uint32_t sentLength = 0;
        int32_t headerAdded = 0;

        OIC_LOG_V(DEBUG, TZ_BLE_SERVER_TAG, " conditional wait");

        u_mutex_lock(gBleServerSendDataMutex);
        u_cond_wait(gBleServerSendCondWait, gBleServerSendDataMutex);

        OIC_LOG_V(DEBUG, TZ_BLE_SERVER_TAG, "wait unlocked");

        CAResult_t result = CA_STATUS_FAILED;

        while (CA_STATUS_OK == CAAdapterDequeueMessage(gCABleServerSenderQueue, &message))
        {
            if (message == NULL)
            {
                OIC_LOG_V(DEBUG, TZ_BLE_SERVER_TAG, "senderData is NULL");
                continue;
            }
            char *dataSegment = NULL;
            uint32_t offset = 0, ret = 1;
            int32_t datalen = message->dataLen;
            while ( 1)
            {
                OIC_LOG_V(DEBUG, TZ_BLE_SERVER_TAG, "fragmenting the data DataLen [%d] Offset [%d]", datalen,
                          offset);
                ret = CAFragmentData(message->data, &dataSegment, datalen, offset);
                sleep(1);
                if (0 == ret)
                {
                    break;
                }

                OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "Sending Multicast data");
                result = CAUpdateCharacteristicsInGattServer(dataSegment, ret);
                if (CA_STATUS_OK != result)
                {
                    OIC_LOG_V(ERROR, TZ_BLE_SERVER_TAG,
                              "Failed to CAUpdateCharacteristicsInGattServer !");
                }

                OIC_LOG_V(DEBUG, TZ_BLE_SERVER_TAG, "freeing dataSegment");

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
                    OIC_LOG_V(DEBUG, TZ_BLE_SERVER_TAG, "All data has been fragmented and sent");
                    break;
                }
            }
            CAAdapterFreeMessage(message);
        }

        u_mutex_unlock(gBleServerSendDataMutex);
    }

    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "OUT ");
    return NULL;
}

void CABLEDataReceiverHandler(void *context)
{
    OIC_LOG_V(DEBUG, TZ_BLE_SERVER_TAG, "IN");

    CAAdapterMessage_t *message = NULL;
    const char *remoteAddress = NULL;
    const char *serviceUUID = NULL;
    uint32_t recvDataLen = 0;
    uint32_t totalDataLen = 0;
    char *dataSegment = NULL;
    char *defragData = NULL;
    isHeaderAvailable = false;
    CARemoteEndpoint_t *remoteEndpoint = NULL;

    u_mutex_lock(gBleServerReceiveDataMutex);

    while (gDataReceiverHandlerState)
    {

        OIC_LOG_V(DEBUG, TZ_BLE_SERVER_TAG, " waiting for the data");

        u_cond_wait(gReceiverDataCond, gBleServerReceiveDataMutex);

        OIC_LOG_V(DEBUG, TZ_BLE_SERVER_TAG, "wait unlocked");

        //Extract the message from queue and send to remote ble device
        while (CA_STATUS_OK == CAAdapterDequeueMessage(gCABleServerReceiverQueue, &message))
        {
            OIC_LOG_V(DEBUG, TZ_BLE_SERVER_TAG, "checking for DE Fragmentation");

            if (!isHeaderAvailable)
            {
                OIC_LOG_V(DEBUG, TZ_BLE_SERVER_TAG, "Parsing the header");
                char *header = (char *) OICMalloc(sizeof(char) * CA_HEADER_LENGTH);
                memcpy(header, message->data, CA_HEADER_LENGTH);
                totalDataLen = CAParseHeader(header);
                OIC_LOG_V(DEBUG, TZ_BLE_SERVER_TAG, "Total data to be accumulated [%d] bytes", totalDataLen);
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
                OIC_LOG_V(DEBUG, TZ_BLE_SERVER_TAG, "Copying the data of length [%d]", message->dataLen);
                memcpy(defragData + recvDataLen, message->data, message->dataLen);
                recvDataLen += message->dataLen ;
                OIC_LOG_V(DEBUG, TZ_BLE_SERVER_TAG, "totalDatalength  [%d] recveived Datalen [%d]",
                          totalDataLen, recvDataLen);
            }
            CAAdapterFreeMessage(message);
            if (totalDataLen == recvDataLen)
            {
                u_mutex_lock(gBleReqRespCbMutex);
                if (NULL == gNetworkPacketReceivedServerCallback)
                {
                    OIC_LOG(ERROR, TZ_BLE_SERVER_TAG, "gReqRespCallback is NULL!");
                    u_mutex_unlock(gBleReqRespCbMutex);
                    return;
                }
                OIC_LOG_V(DEBUG, TZ_BLE_SERVER_TAG, "Sending data up !");
                gNetworkPacketReceivedServerCallback(remoteEndpoint, defragData, recvDataLen);
                OICFree(remoteEndpoint);
                OICFree(defragData);
                recvDataLen = 0;
                totalDataLen = 0;
                isHeaderAvailable = false;
                u_mutex_unlock(gBleReqRespCbMutex);
            }

        }

        if (false == gDataReceiverHandlerState)
        {
            break;
        }


    }
    u_mutex_unlock(gBleServerReceiveDataMutex);
    OIC_LOG_V(DEBUG, TZ_BLE_SERVER_TAG, "OUT");
}

CAResult_t CAStartBleGattServer()
{
    ///TODO:: Check BT adapter is enabled or not ..if its not enabled, enable bt adapter.

    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "IN");

    CAResult_t ret = CAInitGattServerMutexVaraibles();

    if (CA_STATUS_OK != ret )
    {
        OIC_LOG(ERROR, TZ_BLE_SERVER_TAG, "CAInitGattServerMutexVaraibles failed!");
        CATerminateGattServerMutexVaraibles();
        return CA_SERVER_NOT_STARTED;
    }


    u_mutex_lock(gBleServerThreadPoolMutex);
    if (NULL == gBleServerThreadPool)
    {
        OIC_LOG(ERROR, TZ_BLE_SERVER_TAG, "gBleServerThreadPool is NULL");
        u_mutex_unlock(gBleServerThreadPoolMutex);
        return CA_STATUS_FAILED;
    }

    ret = u_thread_pool_add_task(gBleServerThreadPool, (void *) CAStartBleGattServerThread,
                                 (void *) NULL);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(ERROR, TZ_BLE_SERVER_TAG, "u_thread_pool_add_task failed with ret [%d]", ret);
        u_mutex_unlock(gBleServerThreadPoolMutex);
        return CA_STATUS_FAILED;
    }

    ret = CAAdapterInitializeMessageQueue(&gCABleServerSenderQueue);
    if (CA_STATUS_OK != ret )
    {
        OIC_LOG(ERROR, TZ_BLE_SERVER_TAG, "CAAdapterInitializeMessageQueue failed!");
        CATerminateBleGattServer();
        u_mutex_unlock(gBleServerThreadPoolMutex);
        return CA_STATUS_FAILED;
    }

    ret = CAAdapterInitializeMessageQueue(&gCABleServerReceiverQueue);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(ERROR, TZ_BLE_SERVER_TAG, "CAAdapterInitializeMessageQueue failed!");
        CATerminateBleGattServer();
        u_mutex_unlock(gBleServerThreadPoolMutex);
        return CA_STATUS_FAILED;
    }
    gServerUp = CA_TRUE;
    ret = u_thread_pool_add_task(gBleServerThreadPool, (void *) CABleServerSenderQueueProcessor,
                                 (void *) NULL);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(ERROR, TZ_BLE_SERVER_TAG, "u_thread_pool_add_task failed with ret [%d]", ret);

        u_mutex_unlock(gBleServerThreadPoolMutex);
        return CA_STATUS_FAILED;
    }
    gDataReceiverHandlerState = true;
    if (CA_STATUS_OK != u_thread_pool_add_task(gBleServerThreadPool, CABLEDataReceiverHandler, NULL))
    {
        OIC_LOG_V(ERROR, TZ_BLE_SERVER_TAG, "Failed to start data send handler!");
        return ;
    }
    u_mutex_unlock(gBleServerThreadPoolMutex);
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "OUT");
    return CA_STATUS_OK;
}

void *CAStartBleGattServerThread(void *data)
{
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "IN");
    u_mutex_lock(gBleServerStateMutex);
    if (CA_TRUE == gIsBleGattServerStarted)
    {
        OIC_LOG(ERROR, TZ_BLE_SERVER_TAG, "Gatt Server is already running");
        u_mutex_unlock(gBleServerStateMutex);
        CATerminateBleGattServer();
        return NULL;
    }

    CAResult_t ret  =  CAInitBleGattService();

    if (CA_STATUS_OK != ret )
    {
        OIC_LOG(ERROR, TZ_BLE_SERVER_TAG, "_bt_gatt_init_service failed");
        u_mutex_unlock(gBleServerStateMutex);
        CATerminateBleGattServer();
        return NULL;
    }

    sleep(5); // Sleep is must. otherwise its crashing

    char *serviceUUID = "713d0000-503e-4c75-ba94-3148f18d941e";

    ret  = CAAddNewBleServiceInGattServer(serviceUUID);

    if (CA_STATUS_OK != ret )
    {
        OIC_LOG(ERROR, TZ_BLE_SERVER_TAG, "CAAddNewBleServiceInGattServer failed");
        u_mutex_unlock(gBleServerStateMutex);
        CATerminateBleGattServer();
        return NULL;
    }

    char *charReadUUID = "00002af5-0000-1000-8000-00805f9b34fb";

    char charReadValue[4] = {33, 44, 55, 66};

    ret = CAAddNewCharacteristicsToGattServer(gGattSvcPath, charReadUUID, charReadValue, 512 ,
            0); // For Read Characteristics.

    if (CA_STATUS_OK != ret )
    {
        OIC_LOG(ERROR, TZ_BLE_SERVER_TAG, "CAAddNewCharacteristicsToGattServer failed");
        u_mutex_unlock(gBleServerStateMutex);
        CATerminateBleGattServer();
        return NULL;
    }

    char *charWriteUUID = "00002af6-0000-1000-8000-00805f9b34fb";

    char charWriteValue[4] = {33, 44, 55, 66};


    ret = CAAddNewCharacteristicsToGattServer(gGattSvcPath, charWriteUUID, charWriteValue, 512 ,
            1); // For Write Characteristics.
    if (CA_STATUS_OK != ret )
    {
        OIC_LOG(ERROR, TZ_BLE_SERVER_TAG, "CAAddNewCharacteristicsToGattServer failed");
        u_mutex_unlock(gBleServerStateMutex);
        CATerminateBleGattServer();
        return NULL;
    }

    ret = CARegisterBleServicewithGattServer(gGattSvcPath);

    if (CA_STATUS_OK != ret )
    {
        OIC_LOG(ERROR, TZ_BLE_SERVER_TAG, "CARegisterBleServicewithGattServer failed");
        u_mutex_unlock(gBleServerStateMutex);
        CATerminateBleGattServer();
        return NULL;
    }

    bt_adapter_le_create_advertiser(&hAdvertiser);

    if (NULL == hAdvertiser)
    {
        OIC_LOG(ERROR, TZ_BLE_SERVER_TAG, "hAdvertiser is NULL");
        u_mutex_unlock(gBleServerStateMutex);
        CATerminateBleGattServer();
        return NULL;
    }

    int32_t retVal = bt_adapter_le_start_advertising(hAdvertiser, NULL, NULL, NULL);
    if (BT_ERROR_NONE != retVal)
    {
        OIC_LOG_V(DEBUG, TZ_BLE_SERVER_TAG, "bt_adapter_le_start_advertising failed with ret [%d] ",
                  retVal);
        u_mutex_unlock(gBleServerStateMutex);
        CATerminateBleGattServer();
        return NULL;
    }

    gIsBleGattServerStarted = CA_TRUE;

    u_mutex_unlock(gBleServerStateMutex);

    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG,
            "LE Server initialization complete. Enabling gServerUp state to TRUE ");
    gServerUp = CA_TRUE;

    GMainContext *thread_context = NULL;

    thread_context = g_main_context_new();

    g_event_loop = g_main_loop_new(thread_context, FALSE);

    g_main_context_push_thread_default(thread_context);

    g_main_loop_run(g_event_loop);

    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "OUT");

    return NULL;

}

CAResult_t CAStopBleGattServer()
{
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "IN");

    u_mutex_lock(gBleServerStateMutex);

    if (CA_FALSE == gIsBleGattServerStarted)
    {
        OIC_LOG(ERROR, TZ_BLE_SERVER_TAG, "Gatt Server is not running to stop");

        u_mutex_unlock(gBleServerStateMutex);
        return CA_STATUS_OK;
    }
    u_mutex_unlock(gBleServerStateMutex);

    //Stop data send and receive handlers
    if (gBleServerReceiveDataMutex && gReceiverDataCond && gDataReceiverHandlerState)
    {
        u_mutex_lock(gBleServerReceiveDataMutex);
        gDataReceiverHandlerState = CA_FALSE;
        u_cond_signal(gReceiverDataCond);
        u_mutex_unlock(gBleServerReceiveDataMutex);
    }

    CATerminateBleGattServer();

    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "OUT");

    return CA_STATUS_OK;
}

void CATerminateBleGattServer()
{
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "IN");

    gIsBleGattServerStarted = CA_FALSE;
    gServerUp = CA_FALSE;
    u_cond_signal(gBleServerSendCondWait);

    u_mutex_lock(gBleServerStateMutex);
    // Required for waking up the thread which is running in gmain loop
    GMainContext  *context_event_loop = g_main_loop_get_context(g_event_loop);

    if (context_event_loop)
    {
        OIC_LOG_V(DEBUG,  TZ_BLE_SERVER_TAG, "g_event_loop context %x", context_event_loop);
        g_main_context_wakeup(context_event_loop);
    }
    else
    {
        OIC_LOG(ERROR, TZ_BLE_SERVER_TAG, "g_event_loop context is NULL");
    }

    // Kill g main loops and kill threads
    g_main_loop_quit(g_event_loop);

    int32_t ret = 0;

    if (NULL != hAdvertiser )
    {
        ret  = bt_adapter_le_stop_advertising(hAdvertiser);
        if (ret != 0)
        {
            OIC_LOG_V(ERROR, TZ_BLE_SERVER_TAG, "bt_adapter_le_stop_advertising failed with ret [%d]", ret);
        }

        ret = bt_adapter_le_destroy_advertiser(hAdvertiser);
        if (ret != 0)
        {
            OIC_LOG_V(ERROR, TZ_BLE_SERVER_TAG, "bt_adapter_le_destroy_advertiser failed with ret [%d]", ret);
        }
        hAdvertiser = NULL;
    }

    ret = CARemoveAllBleServicesFromGattServer();
    if (ret != 0)
    {
        OIC_LOG(ERROR, TZ_BLE_SERVER_TAG, "removeAllBleServicesFromGattServer failed");
    }

    ret =  CADeInitBleGattService();
    if (ret != 0)
    {
        OIC_LOG_V(ERROR, TZ_BLE_SERVER_TAG, "_bt_gatt_deinit_service failed with ret [%d]", ret);
    }

    // free service Path(unique identifier for ble service)
    u_mutex_lock(gBleServiceMutex);
    OICFree(gGattSvcPath);
    gGattSvcPath = NULL;
    u_mutex_unlock(gBleServiceMutex);

    // freeing characteristics
    u_mutex_lock(gBleCharacteristicMutex);
    OICFree(gGattReadCharPath);
    gGattReadCharPath = NULL;
    OICFree(gGattWriteCharPath);
    gGattWriteCharPath = NULL;
    u_mutex_unlock(gBleCharacteristicMutex);

    u_mutex_unlock(gBleServerStateMutex);

    // Terminating gCABleServerSenderQueue
    u_mutex_lock(gBleServerSendDataMutex);
    if (NULL != gCABleServerSenderQueue)
    {
        CAAdapterTerminateMessageQueue(gCABleServerSenderQueue);
        gCABleServerSenderQueue = NULL;
    }
    u_mutex_unlock(gBleServerSendDataMutex);

    // Terminating gCABleServerReceiverQueue
    u_mutex_lock(gBleServerReceiveDataMutex);
    if (NULL != gCABleServerReceiverQueue)
    {
        CAAdapterTerminateMessageQueue(gCABleServerReceiverQueue);
        gCABleServerReceiverQueue = NULL;
    }
    u_mutex_unlock(gBleServerReceiveDataMutex);

    // Terminating all mutex variables.
    CATerminateGattServerMutexVaraibles();
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "OUT");
}

CAResult_t CAInitGattServerMutexVaraibles()
{
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "IN");
    u_mutex_init();
    if (NULL == gBleServerStateMutex)
    {
        gBleServerStateMutex = u_mutex_new();
        if (NULL == gBleServerStateMutex)
        {
            OIC_LOG(ERROR, TZ_BLE_SERVER_TAG, "u_mutex_new failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == gBleServiceMutex)
    {
        gBleServiceMutex = u_mutex_new();
        if (NULL == gBleServiceMutex)
        {
            OIC_LOG(ERROR, TZ_BLE_SERVER_TAG, "u_mutex_new failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == gBleCharacteristicMutex)
    {
        gBleCharacteristicMutex = u_mutex_new();
        if (NULL == gBleCharacteristicMutex)
        {
            OIC_LOG(ERROR, TZ_BLE_SERVER_TAG, "u_mutex_new failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == gBleReqRespCbMutex)
    {
        gBleReqRespCbMutex = u_mutex_new();
        if (NULL == gBleReqRespCbMutex)
        {
            OIC_LOG(ERROR, TZ_BLE_SERVER_TAG, "u_mutex_new failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == gBleServerSendDataMutex)
    {
        gBleServerSendDataMutex = u_mutex_new();
        if (NULL == gBleServerSendDataMutex)
        {
            OIC_LOG(ERROR, TZ_BLE_SERVER_TAG, "u_mutex_new failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == gBleServerReceiveDataMutex)
    {
        gBleServerReceiveDataMutex = u_mutex_new();
        if (NULL == gBleServerReceiveDataMutex)
        {
            OIC_LOG(ERROR, TZ_BLE_SERVER_TAG, "u_mutex_new failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == gBleServerSendCondWait)
    {
        gBleServerSendCondWait = u_cond_new();
        if (NULL == gBleServerSendCondWait)
        {
            OIC_LOG(ERROR, TZ_BLE_SERVER_TAG, "u_mutex_new failed");
            return CA_STATUS_FAILED;
        }
    }

    if (!gReceiverDataCond)
    {
        gReceiverDataCond = u_cond_new();
        if (NULL == gReceiverDataCond)
        {
            OIC_LOG(ERROR, TZ_BLE_SERVER_TAG, "u_cond_new failed");
            return CA_STATUS_FAILED;
        }
    }

    return CA_STATUS_OK;
}
void CATerminateGattServerMutexVaraibles()
{
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "IN");
    u_mutex_free(gBleServerStateMutex);
    gBleServerStateMutex = NULL;

    u_mutex_free(gBleServerReceiveDataMutex);
    gBleServerReceiveDataMutex = NULL;
    u_mutex_free(gBleServerSendDataMutex);
    gBleServerSendDataMutex = NULL;

    gBleServerStateMutex = NULL;
    u_mutex_free(gBleServiceMutex);
    gBleServiceMutex = NULL;
    u_mutex_free(gBleCharacteristicMutex);
    gBleCharacteristicMutex = NULL;
    u_mutex_free(gBleReqRespCbMutex);
    gBleReqRespCbMutex = NULL;

    u_cond_free(gBleServerSendCondWait);
    gBleServerSendCondWait = NULL;

    if (gReceiverDataCond)
    {
        u_cond_free(gReceiverDataCond);
        gReceiverDataCond = NULL;
    }

    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "OUT");
}
CAResult_t CAInitBleGattService()
{
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "IN");
    int32_t ret =  _bt_gatt_init_service();

    if (ret != 0)
    {
        OIC_LOG_V(ERROR, TZ_BLE_SERVER_TAG, "_bt_gatt_deinit_service failed with ret [%d]", ret);
        return CA_STATUS_FAILED;
    }
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "OUT");
    return CA_STATUS_OK;
}


CAResult_t CADeInitBleGattService()
{
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "IN");
    int32_t ret =  _bt_gatt_deinit_service();

    if (ret != 0)
    {
        OIC_LOG_V(ERROR, TZ_BLE_SERVER_TAG, "_bt_gatt_deinit_service failed with ret [%d]", ret);
        return CA_STATUS_FAILED;
    }
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CASetBleServerThreadPoolHandle(u_thread_pool_t handle)
{
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "IN");
    u_mutex_lock(gBleServerThreadPoolMutex);
    gBleServerThreadPool = handle;
    u_mutex_unlock(gBleServerThreadPoolMutex);
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "OUT");
}


CAResult_t CALEReadDataFromLEServer()
{
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "OUT ");

    return CA_STATUS_OK;
}

CAResult_t CAAddNewBleServiceInGattServer(const char *serviceUUID)
{
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "IN");

    VERIFY_NON_NULL(serviceUUID, NULL, "Param serviceUUID is NULL");

    OIC_LOG_V(DEBUG, TZ_BLE_SERVER_TAG, "service uuid %s", serviceUUID);

    char *svcPath = NULL;

    int32_t ret = bt_gatt_add_service(serviceUUID, &svcPath);

    if (ret != 0)
    {
        OIC_LOG_V(ERROR, TZ_BLE_SERVER_TAG, "bt_gatt_add_service failed with ret [%d]", ret);
        return CA_STATUS_FAILED;
    }

    if (NULL != svcPath)
    {
        OIC_LOG_V(ERROR, TZ_BLE_SERVER_TAG, "AddNewBleServiceInGattServer ServicePath obtained is %s",
                  svcPath);

        u_mutex_lock(gBleServiceMutex);

        if (NULL != gGattSvcPath)
        {
            OICFree(gGattSvcPath);
            gGattSvcPath = NULL;
        }
        gGattSvcPath = svcPath;

        u_mutex_unlock(gBleServiceMutex);
    }

    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "OUT");

    return CA_STATUS_OK;
}

CAResult_t CARemoveBleServiceFromGattServer(const char *svcPath)
{
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "IN");

    VERIFY_NON_NULL(svcPath, NULL, "Param svcPath is NULL");

    int32_t ret = bt_gatt_remove_service(svcPath);

    if (ret != 0)
    {
        OIC_LOG_V(ERROR, TZ_BLE_SERVER_TAG, "bt_gatt_remove_service failed [%d]", ret);
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "OUT");

    return CA_STATUS_OK;
}

CAResult_t CARemoveAllBleServicesFromGattServer()
{
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "IN");
    int32_t ret = bt_gatt_delete_services();

    if (ret != 0)
    {
        OIC_LOG_V(ERROR, TZ_BLE_SERVER_TAG, "bt_gatt_delete_services  failed with ret [%d]", ret);
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "OUT");

    return CA_STATUS_OK;
}

void CABleGattRemoteCharacteristicWriteCb(char *charPath,
        unsigned char *charValue,
        int32_t charValueLen, void *userData)
{
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "IN");

    if (NULL == charPath || NULL == charValue)
    {
        OIC_LOG(ERROR, TZ_BLE_SERVER_TAG, "Param callback values are NULL");
        return;
    }

    OIC_LOG_V(DEBUG, TZ_BLE_SERVER_TAG, "charPath = [%s] charValue = [%s] len [%d]", charPath,
              charValue, charValueLen);

    char *data = (char *)OICMalloc(sizeof(char) * charValueLen + 1);
    if (NULL == data)
    {
        OIC_LOG(ERROR, TZ_BLE_SERVER_TAG, "Malloc failed!");
        //    OICFree(remoteEndPoint);
        return;
    }

    memset(data, 0x0, charValueLen + 1);

    strncpy(data, charValue, charValueLen);

    uint32_t sentLength = 0;

    CAResult_t res = CABLEPushDataToServerReceiverQueue(gRemoteClientAddress, OIC_BLE_SERVICE_ID,
                     data, strlen(data), &sentLength);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG_V(ERROR, TZ_BLE_SERVER_TAG, "CABLEPushDataToReceiverQueue failed");
        OICFree(data);
        return ;
    }

    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "OUT");
}

CAResult_t CABleServerSenderQueueEnqueueMessage(const CARemoteEndpoint_t *remoteEndpoint,
        void *data, uint32_t dataLen)
{
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "IN ");

    VERIFY_NON_NULL(data, NULL, "Param data is NULL");

    VERIFY_NON_NULL_RET(gCABleServerSenderQueue, TZ_BLE_SERVER_TAG, "BleServerReceiverQueue is  NULL",
                        CA_STATUS_FAILED);
    VERIFY_NON_NULL_RET(gBleServerSendDataMutex, TZ_BLE_SERVER_TAG, "BleServerSendDataMutex is NULL",
                        CA_STATUS_FAILED);
    VERIFY_NON_NULL_RET(gBleServerSendCondWait, TZ_BLE_SERVER_TAG, "BleServerSendCondWait is NULL",
                        CA_STATUS_FAILED);

    u_mutex_lock(gBleServerSendDataMutex);
    CAResult_t retVal = CAAdapterEnqueueMessage(gCABleServerSenderQueue, remoteEndpoint, data, dataLen);
    if (CA_STATUS_OK != retVal )
    {
        OIC_LOG(ERROR, TZ_BLE_SERVER_TAG, "CAAdapterEnqueueMessage failed!");
        u_mutex_unlock(gBleServerSendDataMutex);
        return CA_STATUS_FAILED;
    }
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "Sending signal for the sender processor ");
    u_mutex_unlock(gBleServerSendDataMutex);
    u_cond_signal(gBleServerSendCondWait);

    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "OUT ");

    return CA_STATUS_OK;
}

CAResult_t CARegisterBleServicewithGattServer(const char *svcPath)
{
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "IN");

    VERIFY_NON_NULL(svcPath, NULL, "Param svcPath is NULL");

    OIC_LOG_V(DEBUG, TZ_BLE_SERVER_TAG, "svcPath:%s", svcPath);

    int32_t ret = bt_gatt_register_service(svcPath, CABleGattRemoteCharacteristicWriteCb, NULL);

    if (ret != 0)
    {
        OIC_LOG_V(ERROR, TZ_BLE_SERVER_TAG, "bt_gatt_register_service failed with ret [%d]", ret);
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "OUT");

    return CA_STATUS_OK;
}

CAResult_t CAAddNewCharacteristicsToGattServer(const char *svcPath, char *charUUID, char *charValue,
        int32_t charValueLen, int32_t read)
{

    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "IN");

    const char *charFlags[4] = {"read", "write", "notify", "indicate"};

    char *charPath = NULL;

    int32_t flagLen = 4;

    int32_t ret = bt_gatt_add_characteristic(charUUID, charValue, charValueLen, charFlags, flagLen,
                  svcPath, &charPath);

    if (ret != 0 || charPath == NULL)
    {
        OIC_LOG_V(ERROR, TZ_BLE_SERVER_TAG, "bt_gatt_add_characteristic  failed with ret [%d]", ret);
        return CA_STATUS_FAILED;
    }

    OIC_LOG_V(DEBUG, TZ_BLE_SERVER_TAG, "bt_gatt_add_characteristic charPath obtained: %s", charPath);

    u_mutex_lock(gBleCharacteristicMutex);

    if (read == 0)
    {
        if (NULL != gGattReadCharPath)
        {
            OICFree(gGattReadCharPath);
            gGattReadCharPath = NULL;
        }
        gGattReadCharPath = charPath;

    }
    else
    {
        if (NULL != gGattWriteCharPath)
        {
            OICFree(gGattWriteCharPath);
            gGattWriteCharPath = NULL;
        }
        gGattWriteCharPath = charPath;
    }

    u_mutex_unlock(gBleCharacteristicMutex);

    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "OUT");

    return CA_STATUS_OK;
}

CAResult_t CARemoveCharacteristicsFromGattServer(const char *charPath)
{
    ///TODO: There is no api provided in bluetooth.h for removing characteristics.
    return CA_STATUS_OK;
}

CAResult_t CAUpdateCharacteristicsInGattServer(const char *charValue, int32_t charValueLen)
{
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "IN");

    VERIFY_NON_NULL(charValue, NULL, "Param charValue is NULL");

    u_mutex_lock(gBleCharacteristicMutex);

    if (NULL  == gGattWriteCharPath)
    {
        OIC_LOG(ERROR, TZ_BLE_SERVER_TAG, "gGattWriteCharPath is NULL");
        u_mutex_unlock(gBleCharacteristicMutex);
        return CA_STATUS_FAILED;
    }

    char *data = (char *) OICMalloc(sizeof(char) * (charValueLen + 1));

    if (NULL == data)
    {
        OIC_LOG(ERROR, TZ_BLE_SERVER_TAG, "malloc failed!");
        u_mutex_unlock(gBleCharacteristicMutex);
        return CA_STATUS_FAILED;
    }
    memset(data, 0x0, (charValueLen + 1));

    strncpy(data, charValue, charValueLen);

    OIC_LOG_V(DEBUG, TZ_BLE_SERVER_TAG, "updating characteristics char [%s] data [%s] dataLen [%d]",
              (const char *)gGattWriteCharPath, data, charValueLen);

    int32_t ret =  bt_gatt_update_characteristic(gGattWriteCharPath, data, charValueLen);
    if (0 != ret)
    {
        OIC_LOG_V(ERROR, TZ_BLE_SERVER_TAG, "bt_gatt_update_characteristic failed with return [%d]", ret);
        OICFree(data);
        data = NULL;
        u_mutex_unlock(gBleCharacteristicMutex);
        return CA_STATUS_FAILED;
    }
    OICFree(data);
    data = NULL;

    u_mutex_unlock(gBleCharacteristicMutex);

    OIC_LOG(ERROR, TZ_BLE_SERVER_TAG, "OUT");

    return CA_STATUS_OK;
}

void CASetBLEReqRespServerCallback(CANetworkPacketReceivedCallback callback)
{
    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "IN");

    u_mutex_lock(gBleReqRespCbMutex);

    gNetworkPacketReceivedServerCallback = callback;

    u_mutex_unlock(gBleReqRespCbMutex);

    OIC_LOG(DEBUG, TZ_BLE_SERVER_TAG, "OUT");
}


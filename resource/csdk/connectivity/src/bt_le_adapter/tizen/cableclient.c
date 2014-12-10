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

#include "cableclient.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include <pthread.h>
#include <gio/gio.h>

#include "umutex.h"
#include "uarraylist.h"
#include "camessagequeue.h"
#include "caadapterutils.h"
#include "camsgparser.h"

/**
 * @def TZ_BLE_CLIENT_TAG
 * @brief Logging tag for module name
 */
#define TZ_BLE_CLIENT_TAG "TZ_BLE_GATT_CLIENT"

/**
 * @var BLE_UUID_DESCRIPTOR_CLIENT_CHAR_CONFIG
 * @brief Its the constant value for characteristic descriptor from spec.
 */
#define BLE_UUID_DESCRIPTOR_CLIENT_CHAR_CONFIG "2902"

/**
 * @var gBLEServiceList
 * @brief This contains the list of OIC services a client connect tot.
 */
static BLEServiceList *gBLEServiceList = NULL;

/**
 * @var gIsBleGattClientStarted
 * @brief Boolean variable to keep the state of the GATT Client.
 */
static CABool_t gIsBleGattClientStarted = CA_FALSE;

/**
 * @var gCABleClientSenderQueue
 * @brief Queue to process the outgoing packets from GATTClient.
 */
static CAAdapterMessageQueue_t *gCABleClientSenderQueue = NULL;

/**
 * @var gCABleClientReceiverQueue
 * @brief Queue to process the incoming packets to GATT Client.
 */
static CAAdapterMessageQueue_t *gCABleClientReceiverQueue = NULL;

/**
 * @var gBleServiceListMutex
 * @brief Mutex to synchronize access to BleServiceList.
 */
static u_mutex gBleServiceListMutex = NULL;

/**
 * @var gBleReqRespClientCbMutex
 * @brief Mutex to synchronize access to the requestResponse callback to be called
 *           when the data needs to be sent from GATTClient.
 */
static u_mutex gBleReqRespClientCbMutex = NULL;

/**
 * @var gBleClientStateMutex
 * @brief Mutex to synchronize the calls to be done to the platform from GATTClient
 *           interfaces from different threads.
 */
static u_mutex gBleClientStateMutex = NULL;

/**
 * @var gBleClientSendDataMutex
 * @brief Mutex to synchronize the queing of the data from SenderQueue.
 */
static u_mutex gBleClientSendDataMutex = NULL;

/**
 * @var gBleClientSendCondWait
 * @brief Condition used for notifying handler the presence of data in send queue.
 */
static u_cond gBleClientSendCondWait = NULL;

/**
 * @var gBleClientReceiveDataMutex
 * @brief Mutex to synchronize the queing of the data from ReceiverQueue.
 */
static u_mutex gBleClientReceiveDataMutex = NULL;

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
 * @var gBleClientThreadPoolMutex
 * @brief Mutex to synchronize the task to be pushed to thread pool.
 */
static u_mutex gBleClientThreadPoolMutex = NULL;

/**
 * @var gNetworkPacketReceivedClientCallback
 * @brief Maintains the callback to be notified on receival of network packets from other
 *           BLE devices
 */
static CANetworkPacketReceivedCallback gNetworkPacketReceivedClientCallback = NULL;

/**
 * @var gClientUp
 * @brief Boolean variable to maintain the GATT Client state.
 */
static CABool_t gClientUp = CA_FALSE;

/**
 * @var g_event_loop
 * @brief gmainLoop to manage the threads to receive the callback from the platfrom.
 */
static GMainLoop *g_event_loop = NULL;

/**
 * @var gBleClientThreadPool
 * @brief reference to threadpool
 */
static u_thread_pool_t gBleClientThreadPool = NULL;

/**
 * @struct stGattServiceInfo_t
 * @brief structure to map the service attribute to BD Address.
 *
 */
typedef struct gattService
{
    bt_gatt_attribute_h serviceInfo;         /**< bluetoth attribute for oic service*/
    char *address;                                  /**< BD Address of */
} stGattServiceInfo_t;

///TODO: gRemoteAddress is defined here temporarily.
///TODO: It will be removed once UUID based service discovery Implemented in tizen BLE platform
/**
 * @struct gRemoteAddress
 * @brief Remote address of Gatt Server
 *
 */
static const char *gRemoteAddress = "E4:12:1D:99:F3:57";
//static const char *gRemoteAddress = "DB:F7:EB:B5:0F:07";

static u_arraylist_t *gNonOicDeviceList = NULL;

static CABool_t gIsOicDeviceFound = CA_FALSE;
/**
 * @fn CABLEDataReceiverHandler
 * @brief This function handles data from recv message queue.
 */
static void CABLEDataReceiverHandler(void *context);

CAResult_t CABLEPushDataToReceiverQueue(const char *remoteAddress, const char *serviceUUID,
                                        void *data, uint32_t dataLength, uint32_t *sentLength)
{
    OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "IN");

    //Input validation
    VERIFY_NON_NULL(serviceUUID, TZ_BLE_CLIENT_TAG, "service UUID is null");
    VERIFY_NON_NULL(data, TZ_BLE_CLIENT_TAG, "Data is null");
    VERIFY_NON_NULL(sentLength, TZ_BLE_CLIENT_TAG, "Sent data length holder is null");

    //Add message to data queue
    CARemoteEndpoint_t *remoteEndpoint = CAAdapterCreateRemoteEndpoint(CA_LE, remoteAddress,
                                         serviceUUID);
    if (NULL == remoteEndpoint)
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG, "Failed to create remote endpoint !");
        return CA_STATUS_FAILED;
    }

    if (CA_STATUS_OK != CAAdapterEnqueueMessage(gCABleClientReceiverQueue, remoteEndpoint, data,
            dataLength))
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG, "Failed to add message to queue !");

        CAAdapterFreeRemoteEndpoint(remoteEndpoint);
        return CA_STATUS_FAILED;
    }

    *sentLength = dataLength;

    OICFree(data);
    data = NULL;

    //Signal message handler for processing data for sending
    OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "Signalling message send handler");
    u_mutex_lock(gBleClientReceiveDataMutex);
    u_cond_signal(gReceiverDataCond);
    u_mutex_unlock(gBleClientReceiveDataMutex);

    OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");
    return CA_STATUS_OK;
}

void CABleGattCharacteristicChangedCb(bt_gatt_attribute_h characteristic,
                                      unsigned char *value,
                                      int32_t valueLen, void *userData)
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN");

    OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "Changed characteristic is  [%s]", (char *)characteristic);
    OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "Changed characteristic value length [%d]", valueLen);
    OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "Changed characteristic value [%s]", value);

    char *data = (char *)OICMalloc(sizeof(char) * valueLen + 1);
    if (NULL == data)
    {
        OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG, "Malloc failed!");
        return;
    }

    memset(data, 0x0, valueLen + 1);
    strncpy(data, value, valueLen);

    uint32_t sentLength = 0;

    CAResult_t res = CABLEPushDataToReceiverQueue(gRemoteAddress, OIC_BLE_SERVICE_ID,
                     data, strlen(data), &sentLength);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG, "CABLEPushDataToReceiverQueue failed");
        OICFree(data);
        return ;
    }

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");
    return;
}

void CABleGattCharacteristicWriteCb(bt_gatt_attribute_h handle)
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN ");


    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT ");
}

void CABleGattDescriptorDiscoveredCb(int32_t result, unsigned char format, int32_t total,
                                     bt_gatt_attribute_h descriptor,
                                     bt_gatt_attribute_h characteristic, void *userData)
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN");

    stGattCharDescriptor_t *stTemp = (stGattCharDescriptor_t *)OICMalloc(sizeof(
                                         stGattCharDescriptor_t));

    VERIFY_NON_NULL_VOID(stTemp, TZ_BLE_CLIENT_TAG, "malloc failed!");

    memset(stTemp, 0x0, sizeof(stGattCharDescriptor_t));

    stTemp->desc = (uint8_t *)OICMalloc(total + 1);
    if (NULL == stTemp->desc)
    {
        OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG, "malloc failed");
        OICFree(stTemp);
        return;
    }

    memset(stTemp->desc, 0x0, total + 1);

    memcpy(stTemp->desc, (uint8_t *)descriptor, total);

    OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG, "result [%d] format [%d] total [%d]", result, format, total);
    OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG, "characteristic [%s]", (const char *) characteristic);


    bt_gatt_clone_attribute_handle(&(stTemp->characteristic), characteristic);
    stTemp->total = total;

    u_mutex_lock(gBleClientThreadPoolMutex);
    if (NULL == gBleClientThreadPool)
    {
        OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG, "gBleClientThreadPool is NULL");
        bt_gatt_destroy_attribute_handle(stTemp->characteristic);
        OICFree(stTemp->desc);
        OICFree(stTemp);
        u_mutex_unlock(gBleClientThreadPoolMutex);
        return ;
    }

    CAResult_t ret = u_thread_pool_add_task(gBleClientThreadPool,
                                            (void *) CASetCharacteristicDescriptorValueThread,
                                            (void *) stTemp);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG, "u_thread_pool_add_task failed");
        bt_gatt_destroy_attribute_handle(stTemp->characteristic);
        OICFree(stTemp->desc);
        OICFree(stTemp);
        u_mutex_unlock(gBleClientThreadPoolMutex);
        return ;
    }

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG,
            "LE Client initialization flow complete. Enabling gClientUp state to TRUE ");
    gClientUp = CA_TRUE;

    ret = u_thread_pool_add_task(gBleClientThreadPool,
                                 (void *) CABleClientSenderQueueProcessor,
                                 (void *) NULL);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG, "u_thread_pool_add_task failed with ret [%d]", ret);
        u_mutex_unlock(gBleClientThreadPoolMutex);
        return ;
    }

    gDataReceiverHandlerState = true;
    if (CA_STATUS_OK != u_thread_pool_add_task(gBleClientThreadPool, CABLEDataReceiverHandler, NULL))
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG, "Failed to start data send handler!");
        return ;
    }

    u_mutex_unlock(gBleClientThreadPoolMutex);

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");
}

CABool_t CABleGattCharacteristicsDiscoveredCb(int32_t result,
        int32_t inputIndex, int32_t total,
        bt_gatt_attribute_h characteristic, void *userData)
{

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN");

    VERIFY_NON_NULL_RET(characteristic, TZ_BLE_CLIENT_TAG, "Param characteristic is NULL", false);

    VERIFY_NON_NULL_RET(userData, TZ_BLE_CLIENT_TAG, "Param userData is NULL", false);

    OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG,
              "result [%d] input_index [%d] total [%d]",
              result, inputIndex, total);

    char *bdAddress = (char *) userData;

    BLEServiceInfo *bleServiceInfo = NULL;

    u_mutex_lock(gBleServiceListMutex);

    CAGetBLEServiceInfo(gBLEServiceList, bdAddress, &bleServiceInfo);

    u_mutex_unlock(gBleServiceListMutex);


    if ( READ_CHAR_INDEX == inputIndex) // Server will read on this characteristics.
    {
        OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "new read Characteristics is obtained [%s]",
                  (char *)characteristic);

        CAResult_t retVal = CAAppendBLECharInfo(characteristic, READ_CHAR, bleServiceInfo);
        if (CA_STATUS_OK != retVal)
        {
            OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG , "CAAppendBLECharInfo failed ");
            return false;
        }
        char *uuid = NULL;

        bt_gatt_get_service_uuid(characteristic, &uuid);
        OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "new read Characteristics uuid is obtained [%s]",
                  uuid);
    }
    else if ( WRITE_CHAR_INDEX == inputIndex) // Server will write on this characterisctics
    {
        OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "new write Characteristics is obtained [%s]",
                  (char *)characteristic);

        CAResult_t retVal = CAAppendBLECharInfo(characteristic, WRITE_CHAR, bleServiceInfo);
        if (CA_STATUS_OK != retVal)
        {
            OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG , "CAAppendBLECharInfo failed! ");
            return false;
        }
        char *uuid = NULL;

        bt_gatt_get_service_uuid(characteristic, &uuid);
        OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "new write Characteristics uuid is obtained [%s]",
                  uuid);

        stGattServiceInfo_t *stTemp = (stGattServiceInfo_t *)OICMalloc(sizeof(stGattServiceInfo_t));

        VERIFY_NON_NULL_RET(stTemp, TZ_BLE_CLIENT_TAG, "malloc failed!", false);

        memset(stTemp, 0x0, sizeof(stGattServiceInfo_t));

        int32_t len = strlen(bdAddress);

        stTemp->address = (char *)OICMalloc(sizeof(char) * len + 1);
        if (NULL == stTemp->address)
        {
            OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG , "Malloc  failed! ");
            OICFree(stTemp);
            return false;
        }
        memset(stTemp->address, 0x0, len + 1);
        strncpy(stTemp->address, bdAddress, len);

        bt_gatt_clone_attribute_handle(&(stTemp->serviceInfo), characteristic);

        u_mutex_lock(gBleClientThreadPoolMutex);
        if (NULL == gBleClientThreadPool)
        {
            OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG, "gBleClientThreadPool is NULL");
            bt_gatt_destroy_attribute_handle(stTemp->serviceInfo);
            OICFree(stTemp->address);
            OICFree(stTemp);
            u_mutex_unlock(gBleClientThreadPoolMutex);
            return false;
        }

        retVal = u_thread_pool_add_task(gBleClientThreadPool,
                                        (void *) CADiscoverDescriptorThread,
                                        (void *) stTemp);
        if (CA_STATUS_OK != retVal)
        {
            OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG, "u_thread_pool_add_task failed with ret [%d]", retVal);
            bt_gatt_destroy_attribute_handle(stTemp->serviceInfo);
            OICFree(stTemp->address);
            OICFree(stTemp);
            u_mutex_unlock(gBleClientThreadPoolMutex);
            return false;
        }
        u_mutex_unlock(gBleClientThreadPoolMutex);
    }
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");
    return true;

}

void CABtGattBondCreatedCb(int32_t result, bt_device_info_s *device_info, void *user_data)
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN");
    if (BT_ERROR_NONE != result)
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG,
                  " create_bond Failed as [%s ]", CABTGetErrorMsg(result));
    }
    else
    {
        OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "bond with remote device is created.");
        OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "Callback: The number of service : %d.",
                  device_info->service_count);

        VERIFY_NON_NULL_VOID(device_info, TZ_BLE_CLIENT_TAG,
                             "device_info is NULL");
        VERIFY_NON_NULL_VOID(device_info->remote_address, TZ_BLE_CLIENT_TAG,
                             "device_info->remote_address is NULL");

        char *bdAddress = device_info->remote_address;

        BLEServiceInfo *bleServiceInfo = NULL;

        u_mutex_lock(gBleServiceListMutex);
        CAResult_t retVal = CAGetBLEServiceInfo(gBLEServiceList, bdAddress, &bleServiceInfo);
        if (CA_STATUS_OK != retVal)
        {
            OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG , "CAGetBLEServiceInfo failed! ");
            bleServiceInfo = NULL;
            return;
        }
        u_mutex_unlock(gBleServiceListMutex);

        VERIFY_NON_NULL_VOID(bleServiceInfo, TZ_BLE_CLIENT_TAG,
                             "bleServiceInfo is NULL");

        int32_t ret = CAVerifyOICService(bleServiceInfo->service_clone);
        if (CA_STATUS_OK != ret)
        {
            OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG,
                    "We dont need to interact with this generic BLE device since it deos not have OIC service");
        }
        else
        {
            OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "Its OIC service");

            OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG ,
                      " serviceInfo remote address [%s]", bleServiceInfo->bdAddress);

            stGattServiceInfo_t *stTemp = (stGattServiceInfo_t *)OICMalloc(sizeof(stGattServiceInfo_t));

            VERIFY_NON_NULL_VOID(stTemp, TZ_BLE_CLIENT_TAG, "malloc failed");

            bt_gatt_clone_attribute_handle(&(stTemp->serviceInfo), bleServiceInfo->service_clone);

            int32_t len = strlen(bleServiceInfo->bdAddress);

            stTemp->address = (char *)OICMalloc(sizeof(char) * len + 1);
            if (NULL == stTemp->address)
            {
                OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG , "Malloc  failed! ");
                bt_gatt_destroy_attribute_handle(stTemp->serviceInfo);
                OICFree(stTemp);
                return;
            }
            memset(stTemp->address, 0x0, len + 1);

            strncpy(stTemp->address, bleServiceInfo->bdAddress, len);

            u_mutex_lock(gBleClientThreadPoolMutex);
            if (NULL == gBleClientThreadPool)
            {
                OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG, "gBleClientThreadPool is NULL");
                bt_gatt_destroy_attribute_handle(stTemp->serviceInfo);
                OICFree(stTemp->address);
                OICFree(stTemp);
                u_mutex_unlock(gBleClientThreadPoolMutex);
                return;
            }

            CAResult_t ret = u_thread_pool_add_task(gBleClientThreadPool,
                                                    (void *) CADiscoverCharThread, (void *) stTemp);
            if (CA_STATUS_OK != ret)
            {
                OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG,
                          "u_thread_pool_add_task failed with ret [%d]", ret);
                bt_gatt_destroy_attribute_handle(stTemp->serviceInfo);
                OICFree(stTemp->address);
                OICFree(stTemp);
                u_mutex_unlock(gBleClientThreadPoolMutex);
                return;
            }
            u_mutex_unlock(gBleClientThreadPoolMutex);
        }

        OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG,
                  "Callback: is_bonded - %d.", device_info->is_bonded);
        OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG,
                  "Callback: is_connected - %d.", device_info->is_connected);
    }
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");
}

CABool_t CABleGattPrimaryServiceCb(bt_gatt_attribute_h service, int32_t index, int32_t count,
                                   void *userData)
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN");

    VERIFY_NON_NULL_RET(service, TZ_BLE_CLIENT_TAG, "Param service is NULL", false);

    VERIFY_NON_NULL_RET(userData, TZ_BLE_CLIENT_TAG, "Param userData is NULL", false);

    OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "Service info [%s] index [%d] count [%d]", (char *)service,
              index, count);

    CAResult_t result = CAVerifyOICService(service);

    if (CA_STATUS_OK != result)
    {
        OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "No need to process the service ");

        if ((index + 1) == count && CA_FALSE == gIsOicDeviceFound)
        {
            char *bdAddress = (char *)userData;
            OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "OIC service is not available in remote address [%s]",
                      bdAddress);
            int32_t len = strlen(bdAddress);

            char *addr = (char *)OICMalloc(sizeof(char) * len + 1);
            VERIFY_NON_NULL_RET(userData, TZ_BLE_CLIENT_TAG, "malloc failed!", false);
            memset(addr, 0x0, len + 1);
            strncpy(addr, bdAddress, len);

            uint32_t listLen = u_arraylist_length(gNonOicDeviceList);
            int32_t indexValue = 0;
            CABool_t found = CA_FALSE;
            for (indexValue = 0; indexValue < listLen; indexValue++)
            {
                char *storedAddr = u_arraylist_get(gNonOicDeviceList, indexValue);
                if (strcmp(storedAddr, addr) == 0)
                {
                    found = CA_TRUE;
                    break;
                }
            }
            if (found == CA_FALSE)
            {
                OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "Adding device [%s] to gNonOicDeviceList", addr);
                u_arraylist_add(gNonOicDeviceList, addr);
            }

            CAResult_t result = CABleGattDisConnect(bdAddress);
            if (CA_STATUS_OK != result)
            {
                OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG,
                          "CABleGattDisConnect failed!");
                return false;
            }
        }
    }
    else
    {
        OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "Its OIC service");

        gIsOicDeviceFound = CA_TRUE;

        OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG , "Registering to watch characteristics changes  \n");

        result = CABleGattWatchCharacteristicChanges(service);
        if (CA_STATUS_OK != result)
        {
            OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG,
                      "CABleGattWatchCharacteristicChanges failed!");
            return false;
        }

        char *bdAddress = (char *)userData;

        stGattServiceInfo_t *stTemp = (stGattServiceInfo_t *)OICMalloc(sizeof(stGattServiceInfo_t));

        VERIFY_NON_NULL_RET(stTemp, TZ_BLE_CLIENT_TAG, "Malloc Failed", false);

        memset(stTemp, 0x0, sizeof(stGattServiceInfo_t));

        int32_t len = strlen(bdAddress);

        stTemp->address = (char *)OICMalloc(sizeof(char) * len + 1);

        if (NULL == stTemp->address)
        {
            OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG , "Malloc  failed! ");
            OICFree(stTemp);
            return false;
        }
        memset(stTemp->address, 0x0, len + 1);
        strncpy(stTemp->address, bdAddress, len);

        BLEServiceInfo *bleServiceInfo = NULL;

        CAResult_t result = CACreateBLEServiceInfo(bdAddress, service, &bleServiceInfo);
        if (CA_STATUS_OK != result)
        {
            OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG , "CACreateBLEServiceInfo failed! ");
            OICFree(stTemp);
            OICFree(stTemp->address);
            bleServiceInfo = NULL;
            return false;
        }

        OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG ,
                  " serviceInfo remote address [%s]", bleServiceInfo->bdAddress);

        u_mutex_lock(gBleServiceListMutex);
        result = CAAddBLEServiceInfoToList(&gBLEServiceList, bleServiceInfo);
        if (CA_STATUS_OK != result)
        {
            OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG , "CAAddBLEServiceInfoToList failed!");
            OICFree(stTemp);
            OICFree(stTemp->address);
            CAFreeBLEServiceInfo(bleServiceInfo);
            bleServiceInfo = NULL;
            u_mutex_unlock(gBleServiceListMutex);
            return false;
        }
        u_mutex_unlock(gBleServiceListMutex);

        u_mutex_lock(gBleClientThreadPoolMutex);
        if (NULL == gBleClientThreadPool)
        {
            OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG, "gBleClientThreadPool is NULL");
            OICFree(stTemp->address);
            OICFree(stTemp);
            u_mutex_lock(gBleServiceListMutex);
            CARemoveBLEServiceInfoToList(&gBLEServiceList, bleServiceInfo,
                                         bleServiceInfo->bdAddress);
            u_mutex_unlock(gBleServiceListMutex);
            u_mutex_unlock(gBleClientThreadPoolMutex);
            return false;
        }

        result = u_thread_pool_add_task(gBleClientThreadPool,
                                        (void *) CAGATTCreateBondThread,
                                        (void *) stTemp);
        if (CA_STATUS_OK != result)
        {
            OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG,
                      "u_thread_pool_add_task failed with ret [%d]", result);
            OICFree(stTemp->address);
            OICFree(stTemp);
            u_mutex_lock(gBleServiceListMutex);
            CARemoveBLEServiceInfoToList(&gBLEServiceList, bleServiceInfo,
                                         bleServiceInfo->bdAddress);
            u_mutex_unlock(gBleServiceListMutex);
            u_mutex_unlock(gBleClientThreadPoolMutex);
            return false;
        }
        u_mutex_unlock(gBleClientThreadPoolMutex);
    }

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT ");
    return true;;
}

void CABleGattConnectionStateChangedCb(int32_t result, bool connected, const char *remoteAddress,
                                       void *userData)
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN ");

    OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "CABleGattConnectionStateChangedCb result[%d] ", result);

    VERIFY_NON_NULL_VOID(remoteAddress, TZ_BLE_CLIENT_TAG, "remote address is NULL");

    if (!connected)
    {
        OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "DisConnected from [%s] ", remoteAddress);

        CAResult_t result = CABleGattStartDeviceDiscovery();
        if (CA_STATUS_OK != result)
        {
            OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG, "CABleGattStartDeviceDiscovery failed");
            return;
        }
    }
    else
    {
        OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "Connected to [%s] ", remoteAddress);

        int32_t len = strlen(remoteAddress);

        char *addr = (char *)OICMalloc(sizeof(char) * len + 1);

        VERIFY_NON_NULL_VOID(addr, TZ_BLE_CLIENT_TAG, "Malloc failed");

        memset(addr, 0x0, len + 1);

        strncpy(addr, remoteAddress, len);

        u_mutex_lock(gBleClientThreadPoolMutex);
        if (NULL == gBleClientThreadPool)
        {
            OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG, "gBleClientThreadPool is NULL");
            OICFree(addr);
            u_mutex_unlock(gBleClientThreadPoolMutex);
            return;
        }

        CAResult_t ret = u_thread_pool_add_task(gBleClientThreadPool, (void *) CADiscoverBLEServicesThread,
                                                (void *) addr);
        if (CA_STATUS_OK != ret)
        {
            OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG, "u_thread_pool_add_task failed with ret [%d]", ret);
            OICFree(addr);
            u_mutex_unlock(gBleClientThreadPoolMutex);
            return;
        }
        u_mutex_unlock(gBleClientThreadPoolMutex);
    }

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");
}

void CABtAdapterLeDeviceDiscoveryStateChangedCb(int32_t result,
        bt_adapter_le_device_discovery_state_e discoveryState,
        bt_adapter_le_device_discovery_info_s *discoveryInfo,
        void *userData)
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN");

    if (NULL  == discoveryInfo && BT_ADAPTER_LE_DEVICE_DISCOVERY_FOUND == discoveryState)
    {
        OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "discoveryInfo is NULL");
        return;
    }

    if (BT_ADAPTER_LE_DEVICE_DISCOVERY_FOUND != discoveryState)
    {
        OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG,
                  " LE Discovery state is [%s]",
                  discoveryState == BT_ADAPTER_LE_DEVICE_DISCOVERY_STARTED ? "Started" : "Finished" );
    }
    else
    {
        CAPrintDiscoveryInformation(discoveryInfo);

        VERIFY_NON_NULL_VOID(discoveryInfo->remote_address, TZ_BLE_CLIENT_TAG,
                             "discoveryInfo->remote_address is NULL");
        ///TODO: Below check is for filtering to make connection with our intended device(hardcoded one)
        ///TODO: Later it will be removed once Service UUID based implementation is done.

        if (NULL != gNonOicDeviceList)
        {
            uint32_t listLen = u_arraylist_length(gNonOicDeviceList);
            int32_t index = 0;
            for (index = 0; index < listLen; index++)
            {
                char *storedAddr = u_arraylist_get(gNonOicDeviceList, index);
                if (strcmp(storedAddr, discoveryInfo->remote_address) == 0)
                {
                    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "Discovered Device is NON OIC Device. Ignore.");
                    return;
                }
            }
        }

        int32_t len = strlen(discoveryInfo->remote_address);

        char *addr = (char *)OICMalloc(sizeof(char) * len + 1);

        VERIFY_NON_NULL_VOID(addr, TZ_BLE_CLIENT_TAG, "Malloc failed");

        memset(addr, 0x0, len + 1);
        strncpy(addr, discoveryInfo->remote_address, len);

        OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG,
                  "Trying to do Gatt connection to [%s] ", addr);

        u_mutex_lock(gBleClientThreadPoolMutex);
        if (NULL == gBleClientThreadPool)
        {
            OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG, "gBleClientThreadPool is NULL");
            OICFree(addr);
            u_mutex_unlock(gBleClientThreadPoolMutex);
            return;
        }

        CAResult_t ret = u_thread_pool_add_task(gBleClientThreadPool, (void *) CAGattConnectThread,
                                                (void *) addr);
        if (CA_STATUS_OK != ret)
        {
            OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG, "u_thread_pool_add_task failed with ret [%d]", ret);
            OICFree(addr);
            u_mutex_unlock(gBleClientThreadPoolMutex);
            return;
        }
        u_mutex_unlock(gBleClientThreadPoolMutex);
        if (discoveryInfo->adv_data_len > 31 || discoveryInfo->scan_data_len > 31)
        {
            bt_adapter_le_stop_device_discovery();
            return;
        }

    }
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");
}

void CABLEDataReceiverHandler(void *context)
{
    OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "IN");

    CAAdapterMessage_t *message = NULL;
    const char *remoteAddress = NULL;
    const char *serviceUUID = NULL;
    uint32_t recvDataLen = 0;
    uint32_t totalDataLen = 0;
    char *dataSegment = NULL;
    char *defragData = NULL;
    CARemoteEndpoint_t *remoteEndpoint = NULL;

    u_mutex_lock(gBleClientReceiveDataMutex);

    while (gDataReceiverHandlerState)
    {

        OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, " waiting for the data");

        u_cond_wait(gReceiverDataCond, gBleClientReceiveDataMutex);

        OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "wait unlocked");

        //Extract the message from queue and send to remote ble device
        while (CA_STATUS_OK == CAAdapterDequeueMessage(gCABleClientReceiverQueue, &message))
        {
            OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "checking for DE Fragmentation");

            if (!isHeaderAvailable)
            {
                char *header = (char *) OICMalloc(sizeof(char) * CA_HEADER_LENGTH);
                memcpy(header, message->data, CA_HEADER_LENGTH);
                totalDataLen = CAParseHeader(header);
                OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "Total data to be accumulated [%d] bytes", totalDataLen);
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
            CAAdapterFreeMessage(message);
            if (totalDataLen == recvDataLen)
            {
                u_mutex_lock(gBleReqRespClientCbMutex);
                if (NULL == gNetworkPacketReceivedClientCallback)
                {
                    OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG, "gReqRespCallback is NULL!");
                    u_mutex_unlock(gBleReqRespClientCbMutex);
                    return;
                }
                OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "Sending data up !");
                gNetworkPacketReceivedClientCallback(remoteEndpoint, defragData, recvDataLen);
                OICFree(remoteEndpoint);
                OICFree(defragData);
                recvDataLen = 0;
                totalDataLen = 0;
                isHeaderAvailable = false;
                u_mutex_unlock(gBleReqRespClientCbMutex);
            }

        }

        if (false == gDataReceiverHandlerState)
        {
            break;
        }


    }
    u_mutex_unlock(gBleClientReceiveDataMutex);
    OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");
}

void CAPrintDiscoveryInformation(bt_adapter_le_device_discovery_info_s *discoveryInfo)
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN");

    if (NULL == discoveryInfo)
    {
        OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "discoveryInfo is NULL ");
        return;
    }

    if (discoveryInfo->remote_address)
    {
        OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "Remote Address [%s]", discoveryInfo->remote_address);
    }

    OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG,
              " Adv data len [%d] Scan data len[%d]RSSI [%d] Addr_type [%d] ",
              discoveryInfo->adv_data_len, discoveryInfo->scan_data_len, discoveryInfo->rssi,
              discoveryInfo->address_type);
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");
}

void CASetBleClientThreadPoolHandle(u_thread_pool_t handle)
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN");

    u_mutex_unlock(gBleClientThreadPoolMutex);
    gBleClientThreadPool = handle;
    u_mutex_unlock(gBleClientThreadPoolMutex);

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");
}

void CASetBLEReqRespClientCallback(CANetworkPacketReceivedCallback callback)
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN");

    u_mutex_lock(gBleReqRespClientCbMutex);

    gNetworkPacketReceivedClientCallback = callback;

    u_mutex_unlock(gBleReqRespClientCbMutex);

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");
}

CAResult_t CAStartBLEGattClient()
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN");

    CAResult_t retVal = CAInitGattClientMutexVaraibles();

    if (CA_STATUS_OK != retVal)
    {
        OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG, "CAInitGattClientMutexVaraibles failed!");
        CATerminateGattClientMutexVariables();
        return CA_STATUS_FAILED;
    }
    gNonOicDeviceList = u_arraylist_create();
    u_mutex_unlock(gBleClientThreadPoolMutex);
    if (NULL == gBleClientThreadPool)
    {
        OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG, "gBleServerThreadPool is NULL");
        CATerminateGattClientMutexVariables();
        u_mutex_unlock(gBleClientThreadPoolMutex);
        return CA_STATUS_FAILED;
    }

    retVal = u_thread_pool_add_task(gBleClientThreadPool, (void *) CAStartBleGattClientThread,
                                    (void *) NULL);
    if (CA_STATUS_OK != retVal)
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG, "u_thread_pool_add_task failed");
        CATerminateGattClientMutexVariables();
        u_mutex_unlock(gBleClientThreadPoolMutex);
        return CA_STATUS_FAILED;
    }
    u_mutex_unlock(gBleClientThreadPoolMutex);

    return CA_STATUS_OK;
}

void *CAStartBleGattClientThread(void *data)
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN");

    u_mutex_lock(gBleClientStateMutex);

    if (CA_TRUE  == gIsBleGattClientStarted)
    {
        OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG, "Gatt Client is already running!!");
        u_mutex_unlock(gBleClientStateMutex);
        return NULL;
    }

    CAResult_t  ret = CAInitBleQueues();
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG, "CAInitBleQueues failed");
        CATerminateBleQueues();
        u_mutex_unlock(gBleClientStateMutex);
        return NULL;
    }

    ret = CABleGattSetScanParameter();
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG, "CABleSetScanParameter Failed");
        u_mutex_unlock(gBleClientStateMutex);
        CATerminateBLEGattClient();
        return NULL;
    }

    ret = CABleGattSetCallbacks();
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG, "CABleGattSetCallbacks Failed");
        u_mutex_unlock(gBleClientStateMutex);
        CATerminateBLEGattClient();
        return NULL;
    }

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "Starting LE device discovery");

    ret = CABleGattStartDeviceDiscovery();
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG, "bt_adapter_le_start_device_discovery Failed");
        u_mutex_unlock(gBleClientStateMutex);
        CATerminateBLEGattClient();
        return NULL;
    }

    gIsBleGattClientStarted = CA_TRUE;

    u_mutex_unlock(gBleClientStateMutex);

    GMainContext *thread_context = NULL;

    thread_context = g_main_context_new();

    g_event_loop = g_main_loop_new(thread_context, FALSE);

    g_main_context_push_thread_default(thread_context);

    g_main_loop_run(g_event_loop);

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");

    return NULL;
}

void CAStopBLEGattClient()
{
    OIC_LOG(DEBUG,  TZ_BLE_CLIENT_TAG, "IN");

    u_mutex_lock(gBleClientStateMutex);

    if (CA_FALSE == gIsBleGattClientStarted)
    {
        OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG, "Gatt Client is not running to stop");
        u_mutex_unlock(gBleClientStateMutex);
        return;
    }
    u_mutex_unlock(gBleClientStateMutex);

    //Stop data send and receive handlers
    if (gBleClientReceiveDataMutex && gReceiverDataCond && gDataReceiverHandlerState)
    {
        u_mutex_lock(gBleClientReceiveDataMutex);
        gDataReceiverHandlerState = CA_FALSE;
        u_cond_signal(gReceiverDataCond);
        u_mutex_unlock(gBleClientReceiveDataMutex);
    }

    CATerminateBLEGattClient();

    OIC_LOG(DEBUG,  TZ_BLE_CLIENT_TAG, "OUT");
}

void CATerminateBLEGattClient()
{
    OIC_LOG(DEBUG,  TZ_BLE_CLIENT_TAG, "IN");

    gClientUp = CA_FALSE;
    gIsOicDeviceFound = CA_FALSE;

    u_cond_signal(gBleClientSendCondWait);

    u_mutex_lock(gBleClientStateMutex);
    // Required for waking up the thread which is running in gmain loop
    GMainContext  *context_event_loop = g_main_loop_get_context(g_event_loop);

    if (context_event_loop)
    {
        OIC_LOG_V(DEBUG,  TZ_BLE_CLIENT_TAG, "g_event_loop context %x", context_event_loop);
        g_main_context_wakeup(context_event_loop);
    }
    else
    {
        OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG, "g_event_loop context is NULL");
    }

    // Kill g main loops and kill threads
    g_main_loop_quit(g_event_loop);

    CABleGattUnWatchCharacteristicChanges();

    CABleGattUnSetCallbacks();

    CABleGattStopDeviceDiscovery();

    CAClearNonOICDeviceList();

    u_mutex_lock(gBleServiceListMutex);
    CAFreeBLEServiceList(gBLEServiceList);
    gBLEServiceList = NULL;
    u_mutex_unlock(gBleServiceListMutex);

    CAResetRegisteredServiceCount();

    CATerminateBleQueues();

    gIsBleGattClientStarted = CA_FALSE;
    u_mutex_unlock(gBleClientStateMutex);

    CATerminateGattClientMutexVariables();

    OIC_LOG(DEBUG,  TZ_BLE_CLIENT_TAG, "OUT");
}


CAResult_t CAInitGattClientMutexVaraibles()
{
    OIC_LOG(DEBUG,  TZ_BLE_CLIENT_TAG, "IN");
    u_mutex_init();
    if (NULL == gBleClientStateMutex)
    {
        gBleClientStateMutex = u_mutex_new();
        if (NULL == gBleClientStateMutex)
        {
            OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG, "u_mutex_new failed");
            return CA_STATUS_FAILED;
        }
    }

    if (!gReceiverDataCond)
    {
        gReceiverDataCond = u_cond_new();
        if (NULL == gReceiverDataCond)
        {
            OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG, "u_cond_new failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == gBleServiceListMutex)
    {
        gBleServiceListMutex = u_mutex_new();
        if (NULL == gBleServiceListMutex)
        {
            OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG, "u_mutex_new failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == gBleReqRespClientCbMutex)
    {
        gBleReqRespClientCbMutex = u_mutex_new();
        if (NULL == gBleReqRespClientCbMutex)
        {
            OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG, "u_mutex_new failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == gBleClientSendDataMutex)
    {
        gBleClientSendDataMutex = u_mutex_new();
        if (NULL == gBleClientSendDataMutex)
        {
            OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG, "u_mutex_new failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == gBleClientReceiveDataMutex)
    {
        gBleClientReceiveDataMutex = u_mutex_new();
        if (NULL == gBleClientReceiveDataMutex)
        {
            OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG, "u_mutex_new failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == gBleClientThreadPoolMutex)
    {
        gBleClientThreadPoolMutex = u_mutex_new();
        if (NULL == gBleClientThreadPoolMutex)
        {
            OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG, "u_mutex_new failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == gBleClientSendCondWait)
    {
        gBleClientSendCondWait = u_cond_new();
        if (NULL == gBleClientSendCondWait)
        {
            OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG, "u_cond_new failed");
            return CA_STATUS_FAILED;
        }
    }

    OIC_LOG(DEBUG,  TZ_BLE_CLIENT_TAG, "OUT");
    return CA_STATUS_OK;
}

void CATerminateGattClientMutexVariables()
{
    OIC_LOG(DEBUG,  TZ_BLE_CLIENT_TAG, "IN");

    u_mutex_free(gBleClientStateMutex);
    gBleClientStateMutex = NULL;

    u_mutex_free(gBleServiceListMutex);
    gBleServiceListMutex = NULL;

    u_mutex_free(gBleReqRespClientCbMutex);
    gBleReqRespClientCbMutex = NULL;

    u_mutex_free(gBleClientSendDataMutex);
    gBleClientSendDataMutex = NULL;

    u_mutex_free(gBleClientReceiveDataMutex);
    gBleClientReceiveDataMutex = NULL;

    u_mutex_free(gBleClientThreadPoolMutex);
    gBleClientThreadPoolMutex = NULL;

    u_cond_free(gBleClientSendCondWait);
    gBleClientSendCondWait = NULL;

    if (gReceiverDataCond)
    {
        u_cond_free(gReceiverDataCond);
        gReceiverDataCond = NULL;
    }

    OIC_LOG(DEBUG,  TZ_BLE_CLIENT_TAG, "OUT");
}

CAResult_t CAInitBleQueues()
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN");

    u_mutex_lock(gBleClientSendDataMutex);
    CAResult_t retVal = CAAdapterInitializeMessageQueue(&gCABleClientSenderQueue);
    if (CA_STATUS_OK != retVal )
    {
        OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG, "CAAdapterInitializeMessageQueue failed!");
        u_mutex_unlock(gBleClientSendDataMutex);
        return CA_STATUS_FAILED;
    }
    u_mutex_unlock(gBleClientSendDataMutex);

    u_mutex_lock(gBleClientReceiveDataMutex);
    retVal = CAAdapterInitializeMessageQueue(&gCABleClientReceiverQueue);
    if (CA_STATUS_OK != retVal)
    {
        OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG, "CAAdapterInitializeMessageQueue failed!");
        u_mutex_unlock(gBleClientReceiveDataMutex);
        return CA_STATUS_FAILED;
    }
    u_mutex_unlock(gBleClientReceiveDataMutex);

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");
    return CA_STATUS_OK;
}

void CATerminateBleQueues()
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN");

    u_mutex_lock(gBleClientSendDataMutex);
    if (NULL != gCABleClientSenderQueue)
    {
        CAAdapterTerminateMessageQueue(gCABleClientSenderQueue);
        gCABleClientSenderQueue = NULL;
    }
    u_mutex_unlock(gBleClientSendDataMutex);

    u_mutex_lock(gBleClientReceiveDataMutex);
    if (NULL != gCABleClientReceiverQueue)
    {
        CAAdapterTerminateMessageQueue(gCABleClientReceiverQueue);
        gCABleClientReceiverQueue = NULL;
    }
    u_mutex_unlock(gBleClientReceiveDataMutex);

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");
}

void CAClearNonOICDeviceList()
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN");

    if (NULL != gNonOicDeviceList)
    {
        uint32_t listLen = u_arraylist_length(gNonOicDeviceList);
        int32_t index = 0;
        for (index = 0; index < listLen; index++)
        {
            char *storedAddr = u_arraylist_remove(gNonOicDeviceList, index);
            free(storedAddr);
        }
        u_arraylist_free(gNonOicDeviceList);
        gNonOicDeviceList = NULL;
    }

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");
}

CAResult_t CABleGattSetScanParameter()
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN");

    bt_adapter_le_scan_params_s scan_param = { 0, };
    scan_param.type = BT_ADAPTER_LE_PASSIVE_SCAN;
    scan_param.interval = 1560;
    scan_param.window = 160;

    int32_t  ret = bt_adapter_le_set_scan_parameter(&scan_param);
    if (BT_ERROR_NONE != ret)
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG, "bt_adapter_le_set_scan_parameter Failed with ret [%d]", ret);
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CABleGattSetCallbacks()
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN");

    int32_t ret = bt_gatt_set_connection_state_changed_cb(CABleGattConnectionStateChangedCb, NULL);
    if (BT_ERROR_NONE != ret)
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG,
                  "bt_gatt_set_connection_state_changed_cb Failed with return as [%s ]",
                  CABTGetErrorMsg(ret));
        return CA_STATUS_FAILED;
    }

    ret = bt_adapter_le_set_device_discovery_state_changed_cb(
              CABtAdapterLeDeviceDiscoveryStateChangedCb, NULL);
    if (BT_ERROR_NONE != ret)
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG,
                  "bt_adapter_le_set_device_discovery_state_changed_cb Failed with return as [%s ]",
                  CABTGetErrorMsg(ret));;
        return CA_STATUS_FAILED;
    }

    ret = bt_gatt_set_characteristic_changed_cb(CABleGattCharacteristicChangedCb, NULL);
    if (BT_ERROR_NONE != ret)
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG, "bt_gatt_set_characteristic_changed_cb Failed as [%s ]",
                  CABTGetErrorMsg(ret));
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "Setting bt_device_set_bond_created_cb \n");

    ret = bt_device_set_bond_created_cb(CABtGattBondCreatedCb, NULL);
    if (BT_ERROR_NONE != ret)
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG, " bt_device_set_bond_created_cb Failed as [%s ]",
                  CABTGetErrorMsg(ret));
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");
    return CA_STATUS_OK;
}

void CABleGattUnSetCallbacks()
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN");

    bt_gatt_unset_characteristic_changed_cb();

    bt_device_unset_bond_created_cb();

    bt_gatt_unset_connection_state_changed_cb();

    bt_adapter_le_unset_device_discovery_state_changed_cb();

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");
}

CAResult_t CABleGattWatchCharacteristicChanges(bt_gatt_attribute_h service)
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN");


    int32_t ret = bt_gatt_watch_characteristic_changes(service);
    if (BT_ERROR_NONE != ret)
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG,
                  "bt_gatt_watch_characteristic_changes failed  with [%s] \n", CABTGetErrorMsg(ret));

        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");
    return CA_STATUS_OK;
}

void CABleGattUnWatchCharacteristicChanges()
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN");

    int32_t count = CAGetRegisteredServiceCount();

    int32_t index = 0;

    for (index = 0; index < count; index++)
    {
        BLEServiceInfo *bleServiceInfo = NULL;

        u_mutex_lock(gBleServiceListMutex);

        CAResult_t  result = CAGetBLEServiceInfoByPosition(gBLEServiceList, index, &bleServiceInfo);
        if (CA_STATUS_OK == result && NULL != bleServiceInfo && NULL != bleServiceInfo->service_clone)
        {
            bt_gatt_unwatch_characteristic_changes(bleServiceInfo->service_clone);
            OIC_LOG(INFO, TZ_BLE_CLIENT_TAG, "bt_gatt_unwatch_characteristic_changes done");
        }

        u_mutex_unlock(gBleServiceListMutex);
    }

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");
}

CAResult_t CABleGattStartDeviceDiscovery()
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN");

    int32_t ret = bt_adapter_le_start_device_discovery();
    if (BT_ERROR_NONE != ret)
    {
        OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG, "bt_adapter_le_start_device_discovery Failed");
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");
    return CA_STATUS_OK;
}

void CABleGattStopDeviceDiscovery()
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN");

    bt_adapter_le_stop_device_discovery();

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");
}

void *CAGattConnectThread (void *remoteAddress)
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN ");

    VERIFY_NON_NULL_RET(remoteAddress, TZ_BLE_CLIENT_TAG, "remote address is NULL", NULL);

    char *address  = (char *)remoteAddress;

    OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "remote address is [%s]", address);

    CAResult_t result = CABleGattConnect(address);

    if (CA_STATUS_OK != result)
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG, "bt_gatt_connect failed for [%s]", address);
        OICFree(address);
        address = NULL;
        return NULL;
    }

    OICFree(address);
    address = NULL;

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");
    return NULL;
}

CAResult_t CABleGattConnect(const char *remoteAddress)
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN");

    VERIFY_NON_NULL_RET(remoteAddress, TZ_BLE_CLIENT_TAG, "remote address is NULL", CA_STATUS_FAILED);

    int32_t ret = bt_gatt_connect(remoteAddress, true);

    if (BT_ERROR_NONE != ret)
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG, "bt_gatt_connect Failed with ret value [%s] ",
                  CABTGetErrorMsg(ret));
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CABleGattDisConnect(const char *remoteAddress)
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN");

    VERIFY_NON_NULL_RET(remoteAddress, TZ_BLE_CLIENT_TAG, "remote address is NULL", CA_STATUS_FAILED);

    int32_t ret = bt_gatt_disconnect(remoteAddress);

    if (BT_ERROR_NONE != ret)
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG, "bt_gatt_disconnect Failed with ret value [%d] ",
                  ret);
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");
    return CA_STATUS_OK;
}

void *CADiscoverBLEServicesThread (void *remoteAddress)
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN");

    VERIFY_NON_NULL_RET(remoteAddress, TZ_BLE_CLIENT_TAG, "remote address is NULL", NULL);

    char *address  = (char *)remoteAddress;

    CAResult_t result = CABleGattDiscoverServices(address);
    if (CA_STATUS_OK != result)
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG, "CABleGattDiscoverServices failed");
        OICFree(address);
        address = NULL;
        return NULL;
    }

    OICFree(address);
    address = NULL;

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT ");
    return NULL;
}

CAResult_t CABleGattDiscoverServices(const char *remoteAddress)
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN");

    VERIFY_NON_NULL_RET(remoteAddress, TZ_BLE_CLIENT_TAG,
                        "remote address is NULL", CA_STATUS_FAILED);

    int32_t len = strlen(remoteAddress);

    char *addr = (char *)OICMalloc(sizeof(char) * len + 1);

    VERIFY_NON_NULL_RET(addr, TZ_BLE_CLIENT_TAG,
                        "Malloc failed", CA_STATUS_FAILED);

    memset(addr, 0x0, len + 1);
    strncpy(addr, remoteAddress, len);

    int32_t ret = bt_gatt_foreach_primary_services(remoteAddress, CABleGattPrimaryServiceCb,
                  (void *)addr); //addr memory will be free in callback.
    if (BT_ERROR_NONE != ret)
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG, "bt_gatt_foreach_primary_services Failed with ret value [%d] ",
                  ret);
        OICFree(addr);
        return CA_STATUS_FAILED;
    }
    else
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG,
                  "bt_gatt_foreach_primary_services success for address [%s]", remoteAddress);
    }

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");
    return CA_STATUS_OK;
}

void *CADiscoverCharThread(void *stServiceInfo)
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN");

    VERIFY_NON_NULL_RET(stServiceInfo, TZ_BLE_CLIENT_TAG, "stServiceInfo is NULL", NULL);

    stGattServiceInfo_t *stTemp  = (stGattServiceInfo_t *)stServiceInfo;

    VERIFY_NON_NULL_RET(stTemp->address, TZ_BLE_CLIENT_TAG, "stTemp->address is NULL", NULL);

    OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "remote address [%s]", stTemp->address);

    CAResult_t  result = CABleGattDiscoverCharacteristics(stTemp->serviceInfo, stTemp->address);
    if (CA_STATUS_OK != result)
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG , "CABleGattDiscoverCharacteristics failed!");
        bt_gatt_destroy_attribute_handle(stTemp->serviceInfo);
        OICFree(stTemp->address);
        OICFree(stTemp);
        return NULL;
    }
    bt_gatt_destroy_attribute_handle(stTemp->serviceInfo);
    OICFree(stTemp->address);
    OICFree(stTemp);

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");
    return NULL;
}

CAResult_t CABleGattDiscoverCharacteristics(bt_gatt_attribute_h service, const char *remoteAddress)
{
    OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "IN");

    VERIFY_NON_NULL_RET(service, NULL, "service is NULL", CA_STATUS_FAILED);

    VERIFY_NON_NULL_RET(remoteAddress, NULL, "remoteAddress is NULL", CA_STATUS_FAILED);

    int32_t len = strlen(remoteAddress);

    char *addr = (char *)OICMalloc(sizeof(char) * len + 1);
    VERIFY_NON_NULL_RET(addr, TZ_BLE_CLIENT_TAG,
                        "Malloc failed", CA_STATUS_FAILED);

    memset(addr, 0x0, len + 1);
    strncpy(addr, remoteAddress, len);

    int32_t ret = bt_gatt_discover_characteristics(service, CABleGattCharacteristicsDiscoveredCb,
                  (void *)addr); // addr will be freed in callback.
    if (BT_ERROR_NONE != ret)
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG,
                  "bt_gatt_discover_characteristics failed with error [%d]", ret);
        OICFree(addr);
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");
    return CA_STATUS_OK;
}

void *CADiscoverDescriptorThread(void *stServiceInfo)
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, " IN");

    VERIFY_NON_NULL_RET(stServiceInfo, TZ_BLE_CLIENT_TAG, "stServiceInfo is NULL", NULL);

    stGattServiceInfo_t *stTemp  = (stGattServiceInfo_t *)stServiceInfo;

    CAResult_t result = CABleGattDiscoverDescriptor(stTemp->serviceInfo, NULL);
    if (CA_STATUS_OK != result)
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG,
                  "bt_gatt_discover_characteristic_descriptor failed");
        bt_gatt_destroy_attribute_handle(stTemp->serviceInfo);
        OICFree(stTemp->address);
        OICFree(stTemp);
        return NULL;
    }

    bt_gatt_destroy_attribute_handle(stTemp->serviceInfo);
    OICFree(stTemp->address);
    OICFree(stTemp);

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");
    return NULL;
}

CAResult_t CABleGattDiscoverDescriptor(bt_gatt_attribute_h service, const char *remoteAddress)
{
    OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "IN");

    VERIFY_NON_NULL_RET(service, NULL, "service is NULL", CA_STATUS_FAILED);

    int32_t ret = bt_gatt_discover_characteristic_descriptor(service,
                  CABleGattDescriptorDiscoveredCb, NULL);
    if (BT_ERROR_NONE != ret)
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG,
                  "bt_gatt_discover_characteristic_descriptor failed with returns[%s] \n",
                  CABTGetErrorMsg(ret));
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");
    return CA_STATUS_OK;
}

void *CASetCharacteristicDescriptorValueThread(void *stServiceInfo)
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN");

    VERIFY_NON_NULL_RET(stServiceInfo, TZ_BLE_CLIENT_TAG, "stServiceInfo is NULL", NULL);

    stGattCharDescriptor_t *stTemp  = (stGattCharDescriptor_t *)stServiceInfo;

    CAResult_t  result = CASetCharacteristicDescriptorValue(stTemp);
    if (CA_STATUS_OK != result)
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG , "CASetCharacteristicDescriptorValue failed!");
        bt_gatt_destroy_attribute_handle(stTemp->characteristic);
        OICFree(stTemp->desc);
        OICFree(stTemp);
        return NULL;
    }
    bt_gatt_destroy_attribute_handle(stTemp->characteristic);
    OICFree(stTemp->desc);
    OICFree(stTemp);

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");
    return NULL;
}

CAResult_t CASetCharacteristicDescriptorValue(stGattCharDescriptor_t *stGattCharDescriptorInfo)
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN");

    unsigned char noti[4] = {0,};
    char *strUUID = NULL;

    strUUID =  (char *)OICMalloc(sizeof(char) * 5);

    VERIFY_NON_NULL_RET(strUUID, TZ_BLE_CLIENT_TAG, "malloc failed", CA_STATUS_FAILED);

    memset(strUUID, 0x0, sizeof(char) * 5);
    snprintf(strUUID, 4, "%x%x", stGattCharDescriptorInfo->desc[3], stGattCharDescriptorInfo->desc[2]);
    noti[0] = stGattCharDescriptorInfo->desc[0];
    noti[1] = stGattCharDescriptorInfo->desc[1];
    noti[2] = 0x01;
    noti[3] = 0x00;

    OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "desc x0  [%x]", stGattCharDescriptorInfo->desc[0]);
    OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "desc x1 [%x]", stGattCharDescriptorInfo->desc[1]);
    OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "desc x2 [%x]", stGattCharDescriptorInfo->desc[2]);
    OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "desc x3 [%x]", stGattCharDescriptorInfo->desc[3]);


    OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "BLE_UUID_DESCRIPTOR_CLIENT_CHAR_CONFIG strUUID is [%s]",
              strUUID);
    //if (!strncmp(strUUID, BLE_UUID_DESCRIPTOR_CLIENT_CHAR_CONFIG, 2))
    {
        OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "setting notification/indication for descriptor");

        int32_t ret =  bt_gatt_set_characteristic_desc_value_request(
                           stGattCharDescriptorInfo->characteristic,
                           noti,  4, CABleGattCharacteristicWriteCb);
        if (BT_ERROR_NONE != ret)
        {
            OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG,
                      "bt_gatt_set_characteristic_desc_value_request failed with return[%s] \n",
                      CABTGetErrorMsg(ret));
            OICFree(strUUID);
            return CA_STATUS_FAILED;
        }
    }
    OICFree(strUUID);

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");
    return CA_STATUS_OK;
}

void *CAGATTCreateBondThread(void *stServiceInfo)
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN");

    VERIFY_NON_NULL_RET(stServiceInfo, TZ_BLE_CLIENT_TAG, "stServiceInfo is NULL", NULL);

    stGattServiceInfo_t *stTemp  = (stGattServiceInfo_t *)stServiceInfo;

    VERIFY_NON_NULL_RET(stTemp, TZ_BLE_CLIENT_TAG, "stTemp is NULL", NULL);

    OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "remote address [%s]",
              stTemp->address);

    CAResult_t  result = CABleGATTCreateBond(stTemp->address);
    if (CA_STATUS_OK != result)
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG ,
                  "CABleGattDiscoverCharacteristics failed!");
        OICFree(stTemp->address);
        OICFree(stTemp);
        return NULL;
    }
    OICFree(stTemp->address);
    OICFree(stTemp);

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");
    return NULL;
}

CAResult_t CABleGATTCreateBond(const char *remoteAddress)
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN");

    VERIFY_NON_NULL_RET(remoteAddress, TZ_BLE_CLIENT_TAG,
                        "remote address is NULL", CA_STATUS_FAILED);

    int32_t ret = bt_device_create_bond(remoteAddress);

    if (BT_ERROR_NONE != ret)
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG, "bt_device_create_bond Failed with ret value [%d] ", ret);
        return CA_STATUS_FAILED;
    }
    else
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG,
                  " bt_device_create_bond query success for address [%s]", remoteAddress);
    }
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");

    return CA_STATUS_OK;
}

CAResult_t CABleClientSenderQueueEnqueueMessage(const CARemoteEndpoint_t *remoteEndpoint,
        void *data,
        uint32_t dataLen)
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN ");

    VERIFY_NON_NULL(data, NULL, "Param data is NULL");

    VERIFY_NON_NULL_RET(gCABleClientSenderQueue, TZ_BLE_CLIENT_TAG,
                        "BleClientReceiverQueue is  NULL",
                        CA_STATUS_FAILED);
    VERIFY_NON_NULL_RET(gBleClientSendDataMutex, TZ_BLE_CLIENT_TAG,
                        "BleClientSendDataMutex is NULL",
                        CA_STATUS_FAILED);
    VERIFY_NON_NULL_RET(gBleClientSendCondWait, TZ_BLE_CLIENT_TAG,
                        "BleClientSendCondWait is NULL",
                        CA_STATUS_FAILED);

    u_mutex_lock(gBleClientSendDataMutex);
    CAResult_t retVal = CAAdapterEnqueueMessage(gCABleClientSenderQueue,
                        remoteEndpoint, data, dataLen);
    if (CA_STATUS_OK != retVal )
    {
        OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG, "CAAdapterEnqueueMessage failed!");
        u_mutex_unlock(gBleClientSendDataMutex);
        return CA_STATUS_FAILED;
    }
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "Sending signal for the sender processor ");
    u_mutex_unlock(gBleClientSendDataMutex);
    u_cond_signal(gBleClientSendCondWait);

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT ");
    return CA_STATUS_OK;
}

void *CABleClientSenderQueueProcessor()
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN ");

    while (gClientUp)
    {
        CAAdapterMessage_t *message = NULL;
        const char *remoteAddress = NULL;
        const char *serviceUUID = NULL;
        uint32_t sentLength = 0;
        int32_t headerAdded = 0;


        OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, " Waiting for the data ... ");

        u_mutex_lock(gBleClientSendDataMutex);
        u_cond_wait(gBleClientSendCondWait, gBleClientSendDataMutex);

        OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "Data got pushed to the queue...");

        CAResult_t result = CA_STATUS_FAILED;

        while (CA_STATUS_OK == CAAdapterDequeueMessage(gCABleClientSenderQueue,
                &message))
        {
            if (NULL == message)
            {
                OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "senderData is NULL");
                continue;
            }
            char *dataSegment = NULL;
            uint32_t offset = 0, ret = 1;
            int datalen = message->dataLen;
            while ( 1)
            {
                OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "fragmenting the data DataLen [%d] Offset [%d]", datalen,
                          offset);
                ret = CAFragmentData(message->data, &dataSegment, datalen, offset);
                sleep(1);
                if (0 == ret)
                {
                    break;
                }
                if (NULL != message->remoteEndpoint)
                {
                    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "Sending Unicast data");
                    const char *bdAddress = message->remoteEndpoint->addressInfo.LE.leMacAddress;

                    VERIFY_NON_NULL_RET(bdAddress, TZ_BLE_CLIENT_TAG, "bdAddress is NULL", NULL);

                    result = CAUpdateCharacteristicsToGattServer(bdAddress, dataSegment,
                             ret,
                             UNICAST, 0);
                    if (CA_STATUS_OK != result)
                    {
                        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG,
                                  "Failed to UpdateCharacteristicsToGattServer [%s]", bdAddress);
                    }
                }
                else
                {
                    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "Sending Multicast data");
                    result = CAUpdateCharacteristicsToAllGattServers(dataSegment, ret);
                    if (CA_STATUS_OK != result)
                    {
                        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG,
                                  "Failed to UpdateCharacteristicsToAllGattServers !");
                    }
                }
                OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "freeing dataSegment");

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
                    OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "All data has been fragmented and sent");
                    break;
                }
            }

            CAAdapterFreeMessage(message);
        }
        u_mutex_unlock(gBleClientSendDataMutex);
    }

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT ");
    return NULL;
}

CAResult_t  CAUpdateCharacteristicsToGattServer(const char *remoteAddress, const char  *data,
        int32_t dataLen, TRANSFER_TYPE type, int32_t position)
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN");

    VERIFY_NON_NULL(data, NULL, "data is NULL");

    if (0 >= dataLen)
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG, "dataLen is less than or equal zero. Invalid input!");
        return CA_STATUS_INVALID_PARAM;
    }

    BLEServiceInfo *bleServiceInfo = NULL;

    u_mutex_lock(gBleServiceListMutex);
    CAResult_t ret =  CA_STATUS_FAILED;
    if ( UNICAST == type)
    {
        VERIFY_NON_NULL(remoteAddress, NULL, "remoteAddress is NULL");

        ret = CAGetBLEServiceInfo(gBLEServiceList, remoteAddress, &bleServiceInfo);
    }
    else if ( MULTICAST == type)
    {
        ret = CAGetBLEServiceInfoByPosition(gBLEServiceList, position, &bleServiceInfo);
    }

    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG, "CAGetBLEServiceInfoByPosition is failed");
        u_mutex_unlock(gBleServiceListMutex);
        return CA_STATUS_FAILED;
    }
    u_mutex_unlock(gBleServiceListMutex);

    VERIFY_NON_NULL(bleServiceInfo, NULL, "bleServiceInfo is NULL");

    OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "Updating the data of length [%d] to [%s] ", dataLen,
              bleServiceInfo->bdAddress);

    OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "Updating to write char [%s]",
              bleServiceInfo->read_char);

    unsigned char *value = (unsigned char *) OICMalloc(sizeof(char) * (dataLen + 1));
    VERIFY_NON_NULL_RET(value, TZ_BLE_CLIENT_TAG, "malloc failed", CA_STATUS_FAILED);

    memset(value, 0x0, (dataLen + 1));
    strncpy(value, data, dataLen);

    OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "Updating the data of length [%d] to [%s] ", dataLen,
              bleServiceInfo->bdAddress);

    int32_t result = bt_gatt_set_characteristic_value_request(bleServiceInfo->read_char, value,
                     dataLen,
                     CABleGattCharacteristicWriteCb);
    if (BT_ERROR_NONE != result)
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG,
                  "bt_gatt_set_characteristic_value_request Failed with return val [%d]",
                  result);
        OICFree(value);
        return CA_STATUS_FAILED;
    }
    OICFree(value);

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t  CAUpdateCharacteristicsToAllGattServers(const char  *data, int32_t dataLen)
{
    OIC_LOG(DEBUG,  TZ_BLE_CLIENT_TAG, "IN");

    VERIFY_NON_NULL(data, NULL, "data is NULL");

    if (0 >= dataLen)
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG, "dataLen is less than or equal zero. Invalid input !");
        return CA_STATUS_INVALID_PARAM;
    }

    int32_t pos = 0;

    int32_t NumberOfServersConnected = CAGetRegisteredServiceCount();

    for (pos = 0; pos < NumberOfServersConnected ; pos++)
    {
        /*remoteAddress will be NULL.
            Since we have to send to all the address. pos will be used for getting remote address.
          */
        int32_t ret = CAUpdateCharacteristicsToGattServer(NULL, data, dataLen, MULTICAST, pos);

        if (CA_STATUS_OK != ret)
        {
            OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG,
                      "CAUpdateCharacteristicsToGattServer Failed with return val [%d] ", ret);
        }
    }

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT ");
    return CA_STATUS_OK;
}

CAResult_t CALEReadDataFromLEClient()
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN ");

    ///TODO: If CA layer request for the data, this function ll be implemented.

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT ");
    return CA_STATUS_OK;
}


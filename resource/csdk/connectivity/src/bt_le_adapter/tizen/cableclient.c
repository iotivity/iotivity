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
#include "camessagequeue.h"
#include "caadapterutils.h"

/**
 * @def TZ_BLE_CLIENT_TAG
 * @brief Logging tag for module name
 */
#define TZ_BLE_CLIENT_TAG "TZ_BLE_GATT_CLIENT"

/**
 * @var BLE_UUID_DESCRIPTOR_CLIENT_CHAR_CONFIG
 * @brief Its the constant value for characteristic descriptor from spec.
 */
#define BLE_UUID_DESCRIPTOR_CLIENT_CHAR_CONFIG 0x2902

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

void CABleGattCharacteristicWriteCb(int result, void *user_data)
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN ");


    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT ");
}

void *CABleClientSenderQueueProcessor()
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN ");

    while (gClientUp)
    {
        CAAdapterMessage_t *senderData = NULL;

        OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, " Waiting for the data ... ");

        u_mutex_lock(gBleClientSendDataMutex);
        u_cond_wait(gBleClientSendCondWait, gBleClientSendDataMutex);

        OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "Data got pushed to the queue...");

        CAResult_t result = CA_STATUS_FAILED;

        while (CA_STATUS_OK == CAAdapterDequeueMessage(gCABleClientSenderQueue,
                &senderData))
        {
            if (NULL == senderData)
            {
                OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "senderData is NULL");
                continue;
            }
            if (NULL != senderData->remoteEndpoint)
            {
                OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "Sending Unicast data");
                const char *bdAddress = senderData->remoteEndpoint->addressInfo.LE.leMacAddress;

                VERIFY_NON_NULL_RET(bdAddress, TZ_BLE_CLIENT_TAG, "bdAddress is NULL", NULL);

                result = CAUpdateCharacteristicsToGattServer(bdAddress, senderData->data,
                         senderData->dataLen,
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
                result = CAUpdateCharacteristicsToAllGattServers(senderData->data,
                         senderData->dataLen);
                if (CA_STATUS_OK != result)
                {
                    OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG,
                              "Failed to UpdateCharacteristicsToAllGattServers !");
                }
            }

            CAAdapterFreeMessage(senderData);
        }

        u_mutex_unlock(gBleClientSendDataMutex);
    }

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT ");
    return NULL;
}


CAResult_t CASetCharacteristicDescriptorValue(stGattCharDescriptor_t *stGattCharDescriptorInfo)
{

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN");

    uint8_t desc[stGattCharDescriptorInfo->total];
    unsigned char noti[4] = {0,};
    char *strUUID = NULL;

    memcpy(desc, (uint8_t *)stGattCharDescriptorInfo->descriptor,
           stGattCharDescriptorInfo->total);

    strUUID =  (char *)OICMalloc(sizeof(char) * 5);

    VERIFY_NON_NULL_RET(strUUID, TZ_BLE_CLIENT_TAG, "malloc failed", CA_STATUS_FAILED);

    memset(strUUID, 0x0, sizeof(char) * 5);
    snprintf(strUUID, 4, "%x%x", desc[3], desc[2]);
    noti[0] = desc[0];
    noti[1] = desc[1];
    noti[2] = 0x01;
    noti[3] = 0x00;

    if (!strncmp(strUUID, BLE_UUID_DESCRIPTOR_CLIENT_CHAR_CONFIG, 2))
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
    bt_gatt_destroy_attribute_handle(stGattCharDescriptorInfo->characteristic);
    bt_gatt_destroy_attribute_handle(stGattCharDescriptorInfo->descriptor);
    OICFree(stGattCharDescriptorInfo);
    OICFree(strUUID);

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");
    return CA_STATUS_OK;
}

void *CASetCharacteristicDescriptorValueThread(void *stServiceInfo)
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN");

    VERIFY_NON_NULL_RET(stServiceInfo, TZ_BLE_CLIENT_TAG, "stServiceInfo is NULL", NULL);

    stGattCharDescriptor_t *stTemp  = (stGattCharDescriptor_t *)stServiceInfo;

    VERIFY_NON_NULL_RET(stTemp, TZ_BLE_CLIENT_TAG, "stTemp is NULL", NULL);

    CAResult_t  result = CASetCharacteristicDescriptorValue(stTemp);

    if (CA_STATUS_OK != result)
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG , "CASetCharacteristicDescriptorValue failed!");
        return NULL;
    }

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");
    return NULL;
}


void CABleGattDescriptorDiscoveredCb(int32_t result, unsigned char format, int32_t total,
                                     bt_gatt_attribute_h descriptor,
                                     bt_gatt_attribute_h characteristic, void *userData)
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN");

    stGattCharDescriptor_t *stTemp = (stGattCharDescriptor_t *)OICMalloc(sizeof(
                                         stGattCharDescriptor_t));

    VERIFY_NON_NULL_VOID(stTemp, TZ_BLE_CLIENT_TAG, "stTemp is NULL");

    bt_gatt_clone_attribute_handle(&(stTemp->descriptor), descriptor);

    bt_gatt_clone_attribute_handle(&(stTemp->characteristic), characteristic);

    stTemp->total = total;
    u_mutex_lock(gBleClientThreadPoolMutex);
    if (NULL == gBleClientThreadPool)
    {
        OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG, "gBleClientThreadPool is NULL");
        OICFree(stTemp);
        u_mutex_unlock(gBleClientThreadPoolMutex);
        return ;
    }

    CAResult_t ret = u_thread_pool_add_task(gBleClientThreadPool,
                                            (void *) CASetCharacteristicDescriptorValueThread,
                                            (void *) stTemp);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG, "u_thread_pool_add_task failed with ret [%d]", ret);
        OICFree(stTemp);
        u_mutex_unlock(gBleClientThreadPoolMutex);
        return ;
    }

    ret = u_thread_pool_add_task(gBleClientThreadPool,
                                 (void *) CABleClientSenderQueueProcessor,
                                 (void *) NULL);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG, "u_thread_pool_add_task failed with ret [%d]", ret);
        u_mutex_unlock(gBleClientThreadPoolMutex);
        return ;
    }
    u_mutex_unlock(gBleClientThreadPoolMutex);

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG,
            "LE Client initialization complete. Enabling gClientUp state to TRUE ");
    gClientUp = CA_TRUE;

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");
    return;
}

void *CADiscoverDescriptorThread(void *stServiceInfo)
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, " IN");

    VERIFY_NON_NULL_RET(stServiceInfo, TZ_BLE_CLIENT_TAG, "stServiceInfo is NULL", NULL);

    stGattServiceInfo_t *stTemp  = (stGattServiceInfo_t *)stServiceInfo;

    int32_t ret = bt_gatt_discover_characteristic_descriptor(stTemp->serviceInfo,
                  CABleGattDescriptorDiscoveredCb, NULL);

    if (BT_ERROR_NONE != ret)
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG,
                  "bt_gatt_discover_characteristic_descriptor failed with returns[%s] \n",
                  CABTGetErrorMsg(ret));
        return NULL;
    }

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");
    return NULL;
}

void *CADiscoverCharThread(void *stServiceInfo)
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN");

    VERIFY_NON_NULL_RET(stServiceInfo, TZ_BLE_CLIENT_TAG, "stServiceInfo is NULL", NULL);

    stGattServiceInfo_t *stTemp  = (stGattServiceInfo_t *)stServiceInfo;

    VERIFY_NON_NULL_RET(stTemp, TZ_BLE_CLIENT_TAG, "stTemp is NULL", NULL);

    OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "remote address [%s]", stTemp->address);

    CAResult_t  result = CABleDiscoverCharacteristics(stTemp->serviceInfo, stTemp->address);
    bt_gatt_destroy_attribute_handle(stTemp->serviceInfo);

    OICFree(stTemp->address);

    OICFree(stTemp);

    if (CA_STATUS_OK != result)
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG , "CABleDiscoverCharacteristics failed!");
        return NULL;
    }
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");
    return NULL;
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
        OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "A bond with chat_server is created.");
        OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "Callback: The number of service : %d.",
                  device_info->service_count);

        char *bdAddress = device_info->remote_address;

        int32_t len = strlen(bdAddress);

        char *addr = (char *)OICMalloc(sizeof(char) * len + 1);

        VERIFY_NON_NULL_VOID(addr, TZ_BLE_CLIENT_TAG, "Malloc  failed!");

        memset(addr, 0x0, len + 1);

        strncpy(addr, bdAddress, len);

        BLEServiceInfo *bleServiceInfo = NULL;

        u_mutex_lock(gBleServiceListMutex);
        CAResult_t retVal = CAGetBLEServiceInfo(gBLEServiceList, addr, &bleServiceInfo);

        u_mutex_unlock(gBleServiceListMutex);
        OICFree(addr);

        if (CA_STATUS_OK != retVal)
        {
            OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG , "CAGetBLEServiceInfo failed! ");
            bleServiceInfo = NULL;
            return ;
        }

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

            len = strlen(bleServiceInfo->bdAddress);

            stTemp->address = (char *)OICMalloc(sizeof(char) * len + 1);
            if (NULL == stTemp->address)
            {
                OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG , "Malloc  failed! ");
                bt_gatt_destroy_attribute_handle(stTemp->serviceInfo);
                OICFree(stTemp);
                return ;
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
                return ;
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
                return ;
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

void *CAGATTCreateBondThread(void *stServiceInfo)
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN");

    VERIFY_NON_NULL_RET(stServiceInfo, TZ_BLE_CLIENT_TAG, "stServiceInfo is NULL", NULL);

    stGattServiceInfo_t *stTemp  = (stGattServiceInfo_t *)stServiceInfo;

    VERIFY_NON_NULL_RET(stTemp, TZ_BLE_CLIENT_TAG, "stTemp is NULL", NULL);

    OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "remote address [%s]",
              stTemp->address);

    CAResult_t  result = CABleGATTCreateBond(stTemp->address);

    OICFree(stTemp->address);
    OICFree(stTemp);

    if (CA_STATUS_OK != result)
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG ,
                  "CABleDiscoverCharacteristics failed!");
        return NULL;
    }

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");
    return NULL;
}

void CABleGattCharacteristicChangedCb(bt_gatt_attribute_h characteristic,
                                      unsigned char *value,
                                      int32_t valueLen, void *userData)
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN");

    int32_t i = 0;

    OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "Changed characteristic is  [%s]", (char *)characteristic);

    OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "Changed characteristic value length [%d]", valueLen);

    for (i = 0; i < valueLen; i++)
    {
        OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "Changed characteristic  value %c", value[i]);
    }

    CARemoteEndpoint_t  *remoteEndPoint = (CARemoteEndpoint_t *) OICMalloc(sizeof(CARemoteEndpoint_t));

    VERIFY_NON_NULL_VOID(remoteEndPoint, TZ_BLE_CLIENT_TAG, "malloc failed");

    memset(remoteEndPoint, 0x0, sizeof(CARemoteEndpoint_t));

    ///TODO: Currently Empty endpoint is being sent.
    ///TODO:Later proper remote address has to be added when tizen team changed their code

    char *data = (char *)OICMalloc(sizeof(char) * valueLen + 1);
    if (NULL == data)
    {
        OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG, "Malloc failed!");
        OICFree(remoteEndPoint);
        return;
    }

    memset(data, 0x0, valueLen + 1);
    strncpy(data, value, valueLen);

    u_mutex_lock(gBleReqRespClientCbMutex);
    if (NULL == gNetworkPacketReceivedClientCallback)
    {
        OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG, "gReqRespCallback is NULL!");
        OICFree(value);
        OICFree(remoteEndPoint);
        OICFree(data);
        u_mutex_unlock(gBleReqRespClientCbMutex);
        return;
    }

    gNetworkPacketReceivedClientCallback(remoteEndPoint, data, valueLen);

    u_mutex_unlock(gBleReqRespClientCbMutex);
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");
    return;
}

CABool_t CABleGattPrimaryServiceCb(bt_gatt_attribute_h service, void *userData)
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN");

    if (NULL == service || NULL == userData)
    {
        OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "Param service or userData is NULL");
        return false;
    }

    OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "Service info [%s]", (char *)service);

    int32_t ret = CAVerifyOICService(service);

    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "No need to process the service ");
    }
    else
    {
        OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "Its OIC service");

        char *bdAddress = (char *)userData;

        int32_t len = strlen(bdAddress);

        char *addr = (char *)OICMalloc(sizeof(char) * len + 1);
        if (NULL == addr)
        {
            OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG , "malloc failed! ");
            return false;
        }
        memset(addr, 0x0, len + 1);

        strncpy(addr, bdAddress, len);

        BLEServiceInfo *bleServiceInfo = NULL;

        CAResult_t result = CACreateBLEServiceInfo(addr, service, &bleServiceInfo);

        OICFree(addr);

        if (CA_STATUS_OK != result)
        {
            OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG , "CACreateBLEServiceInfo failed! ");
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
            bleServiceInfo = NULL;
            u_mutex_unlock(gBleServiceListMutex);
            return false;
        }
        u_mutex_unlock(gBleServiceListMutex);

        OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG , "Registering to watch characteristics changes  \n");

        int32_t ret = bt_gatt_watch_characteristic_changes(bleServiceInfo->service_clone);

        if (BT_ERROR_NONE != ret)
        {
            OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG,
                      "bt_gatt_watch_characteristic_changes failed  with [%s] \n", CABTGetErrorMsg(ret));
            u_mutex_lock(gBleServiceListMutex);
            CARemoveBLEServiceInfoToList(&gBLEServiceList,
                                         bleServiceInfo, bleServiceInfo->bdAddress);
            u_mutex_unlock(gBleServiceListMutex);
            return false;
        }

        stGattServiceInfo_t *stTemp = (stGattServiceInfo_t *)OICMalloc(sizeof(stGattServiceInfo_t));

        if (NULL == stTemp)
        {
            OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG , "Malloc  failed! ");
            u_mutex_lock(gBleServiceListMutex);
            CARemoveBLEServiceInfoToList(&gBLEServiceList,
                                         bleServiceInfo, bleServiceInfo->bdAddress);
            u_mutex_unlock(gBleServiceListMutex);
            return false;
        }

        memset(stTemp, 0x0, sizeof(stGattServiceInfo_t));

        len = strlen(bleServiceInfo->bdAddress);

        stTemp->address = (char *)OICMalloc(sizeof(char) * len + 1);

        if (NULL == stTemp->address)
        {
            OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG , "Malloc  failed! ");
            u_mutex_lock(gBleServiceListMutex);
            CARemoveBLEServiceInfoToList(&gBLEServiceList, bleServiceInfo,
                                         bleServiceInfo->bdAddress);
            u_mutex_unlock(gBleServiceListMutex);
            OICFree(stTemp);
            return false;
        }

        memset(stTemp->address, 0x0, len + 1);

        strncpy(stTemp->address, bleServiceInfo->bdAddress, len);

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


CABool_t CABleGattCharacteristicsDiscoveredCb(int32_t result,
        int32_t inputIndex, int32_t total,
        bt_gatt_attribute_h characteristic, void *userData)
{

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN");

    if (NULL == characteristic || NULL == userData)
    {
        OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "Param characteristic and userData is NULL");
        return false;
    }

    OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG,
              "result [%d] input_index [%d] total [%d]",
              result, inputIndex, total);

    OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG,
              "new characteristic found is [%s]", (char *)characteristic);

    char *bdAddress = (char *) userData;

    BLEServiceInfo *bleServiceInfo = NULL;

    u_mutex_lock(gBleServiceListMutex);

    CAGetBLEServiceInfo(gBLEServiceList, bdAddress, &bleServiceInfo);

    u_mutex_unlock(gBleServiceListMutex);

    if ( READ_CHAR_INDEX == inputIndex)
    {
        CAResult_t retVal = CAAppendBLECharInfo(characteristic, READ_CHAR, bleServiceInfo);
        if (CA_STATUS_OK != retVal)
        {
            OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG , "CAAppendBLECharInfo failed ");
            return false;
        }
    }
    else if ( WRITE_CHAR_INDEX == inputIndex)
    {
        CAResult_t retVal = CAAppendBLECharInfo(characteristic, WRITE_CHAR, bleServiceInfo);
        if (CA_STATUS_OK != retVal)
        {
            OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG , "CAAppendBLECharInfo failed! ");
            return false;
        }

    }

    stGattServiceInfo_t *stTemp = (stGattServiceInfo_t *)OICMalloc(sizeof(stGattServiceInfo_t));

    if (NULL == stTemp)
    {
        OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG , "Malloc  failed! ");
        return false;
    }

    memset(stTemp, 0x0, sizeof(stGattServiceInfo_t));

    bt_gatt_clone_attribute_handle(&(stTemp->serviceInfo), characteristic);

    int32_t len = strlen(bleServiceInfo->bdAddress);

    stTemp->address = (char *)OICMalloc(sizeof(char) * len + 1);
    if (NULL == stTemp->address)
    {
        OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG , "Malloc  failed! ");
        bt_gatt_destroy_attribute_handle(stTemp->serviceInfo);
        OICFree(stTemp);
        return false;
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
        //CARemoveBLEServiceInfoToList(&gBLEServiceList, bleServiceInfo, bleServiceInfo->bdAddress);
        return false;
    }

    CAResult_t ret = u_thread_pool_add_task(gBleClientThreadPool,
                                            (void *) CADiscoverDescriptorThread,
                                            (void *) stTemp);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG, "u_thread_pool_add_task failed with ret [%d]", ret);
        bt_gatt_destroy_attribute_handle(stTemp->serviceInfo);
        OICFree(stTemp->address);
        OICFree(stTemp);
        u_mutex_unlock(gBleClientThreadPoolMutex);
        //CARemoveBLEServiceInfoToList(&gBLEServiceList, bleServiceInfo, bleServiceInfo->bdAddress);
        return false;
    }
    u_mutex_unlock(gBleClientThreadPoolMutex);
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");
    return true;

}

CAResult_t CABleClientSenderQueueEnqueueMessage(const CARemoteEndpoint_t *remoteEndpoint, void *data,
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


CAResult_t CALEReadDataFromLEClient()
{

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN ");

    ///TODO: If CA layer request for the data, this function ll be implemented.

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT ");

    return CA_STATUS_OK;
}


void *CAGattConnectThread (void *remoteAddress)
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN ");

    if (NULL == remoteAddress)
    {
        OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "Param remoteAddress is NULL ");
        return NULL;
    }

    char *address  = (char *)remoteAddress;

    OIC_LOG_V(DEBUG, TZ_BLE_CLIENT_TAG, "remote address is [%s]", address);

    CAResult_t result = bt_gatt_connect(address, true);

    if (CA_STATUS_OK != result)
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG, "bt_gatt_connect failed for [%s]", address);
        return NULL;
    }

    OICFree(address);
    address = NULL;

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");
    return NULL;
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


void *CADiscoverBLEServicesThread (void *remoteAddress)
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN");

    VERIFY_NON_NULL_RET(remoteAddress, TZ_BLE_CLIENT_TAG, "remote address is NULL", NULL);

    char *address  = (char *)remoteAddress;

    CAResult_t result = CABleDiscoverServices(remoteAddress);

    OICFree(address);
    address = NULL;

    if (CA_STATUS_OK != result)
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG, "CABleDiscoverServices Failed");
        return NULL;
    }

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT ");
    return NULL;
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

CAResult_t CABleGATTCreateBond(const char *remoteAddress)
{

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN");

    VERIFY_NON_NULL_RET(remoteAddress, TZ_BLE_CLIENT_TAG,
                        "remote address is NULL", CA_STATUS_FAILED);

    sleep(2);

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

CAResult_t CABleDiscoverServices(const char *remoteAddress)
{

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN");

    VERIFY_NON_NULL_RET(remoteAddress, TZ_BLE_CLIENT_TAG,
                        "remote address is NULL", CA_STATUS_FAILED);
    sleep(2);

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

CAResult_t CABleDiscoverCharacteristics(bt_gatt_attribute_h service, const char *remoteAddress)
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

    u_mutex_unlock(gBleClientThreadPoolMutex);
    if (NULL == gBleClientThreadPool)
    {
        OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG, "gBleServerThreadPool is NULL");
        u_mutex_unlock(gBleClientThreadPoolMutex);
        return CA_STATUS_FAILED;
    }

    CAResult_t ret = u_thread_pool_add_task(gBleClientThreadPool, (void *) CAStartBleGattClientThread,
                                            (void *) NULL);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG, "u_thread_pool_add_task failed with ret [%d]", ret);
        u_mutex_unlock(gBleClientThreadPoolMutex);
        return CA_STATUS_FAILED;
    }
    u_mutex_unlock(gBleClientThreadPoolMutex);
    return CA_STATUS_OK;
}

void *CAStartBleGattClientThread(void *data)
{
    u_mutex_lock(gBleClientStateMutex);

    if (CA_TRUE  == gIsBleGattClientStarted)
    {
        OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG, "Gatt Client is already running");
        u_mutex_unlock(gBleClientStateMutex);
        CATerminateBLEGattClient();
        return NULL;
    }

    bt_adapter_le_scan_params_s scan_param = { 0, };
    scan_param.type = BT_ADAPTER_LE_PASSIVE_SCAN;
    scan_param.interval = 1560;
    scan_param.window = 160;

    int32_t  ret = bt_adapter_le_set_scan_parameter(&scan_param);
    if (BT_ERROR_NONE != ret)
    {
        OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG, "bt_adapter_le_set_scan_parameter Failed");
        u_mutex_unlock(gBleClientStateMutex);
        CATerminateBLEGattClient();
        return NULL;
    }

    ret = bt_gatt_set_connection_state_changed_cb(CABleGattConnectionStateChangedCb, NULL);
    if (BT_ERROR_NONE != ret)
    {
        OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG,
                "bt_gatt_set_connection_state_changed_cb Failed");
        u_mutex_unlock(gBleClientStateMutex);
        CATerminateBLEGattClient();
        return NULL;
    }

    ret = bt_adapter_le_set_device_discovery_state_changed_cb(
              CABtAdapterLeDeviceDiscoveryStateChangedCb, NULL);
    if (BT_ERROR_NONE != ret)
    {
        OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG,
                "bt_adapter_le_set_device_discovery_state_changed_cb Failed");
        u_mutex_unlock(gBleClientStateMutex);
        CATerminateBLEGattClient();
        return NULL;
    }

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "Setting characteristic_changed cb \n");

    ret = bt_gatt_set_characteristic_changed_cb(CABleGattCharacteristicChangedCb, NULL);
    if (BT_ERROR_NONE != ret)
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG, "bt_gatt_set_characteristic_changed_cb Failed as [%s ]",
                  CABTGetErrorMsg(ret));
        u_mutex_unlock(gBleClientStateMutex);
        CATerminateBLEGattClient();
        return NULL;
    }

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "Setting bt_device_set_bond_created_cb \n");

    ret = bt_device_set_bond_created_cb(CABtGattBondCreatedCb, NULL);
    if (BT_ERROR_NONE != ret)
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG, " bt_device_set_bond_created_cb Failed as [%s ]",
                  CABTGetErrorMsg(ret));
        return NULL;
    }

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "Starting LE device discovery");

    ret = bt_adapter_le_start_device_discovery();
    if (BT_ERROR_NONE != ret)
    {
        OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG, "bt_adapter_le_start_device_discovery Failed");
        u_mutex_unlock(gBleClientStateMutex);
        CATerminateBLEGattClient();
        return NULL;
    }
    else
    {
        OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG,
                "bt_adapter_le_start_device_discovery Success");
    }

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");

    u_mutex_unlock(gBleClientStateMutex);

    CAResult_t retVal = CAAdapterInitializeMessageQueue(&gCABleClientSenderQueue);
    if (CA_STATUS_OK != retVal )
    {
        OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG, "CAAdapterInitializeMessageQueue failed!");
        CATerminateBLEGattClient();
        return NULL;
    }

    retVal = CAAdapterInitializeMessageQueue(&gCABleClientReceiverQueue);
    if (CA_STATUS_OK != retVal)
    {
        OIC_LOG(ERROR, TZ_BLE_CLIENT_TAG, "CAAdapterInitializeMessageQueue failed!");
        CATerminateBLEGattClient();
        return NULL;
    }
    gIsBleGattClientStarted = CA_TRUE;
    GMainContext *thread_context = NULL;

    thread_context = g_main_context_new();

    g_event_loop = g_main_loop_new(thread_context, FALSE);

    g_main_context_push_thread_default(thread_context);

    g_main_loop_run(g_event_loop);

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

    CATerminateBLEGattClient();

    OIC_LOG(DEBUG,  TZ_BLE_CLIENT_TAG, "OUT");
}

void CATerminateBLEGattClient()
{
    OIC_LOG(DEBUG,  TZ_BLE_CLIENT_TAG, "IN");

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

    gClientUp = CA_FALSE;

    bt_gatt_unset_connection_state_changed_cb();

    bt_adapter_le_stop_device_discovery();

    bt_gatt_unset_characteristic_changed_cb();

    bt_adapter_le_unset_device_discovery_state_changed_cb();

    bt_gatt_unset_connection_state_changed_cb();

    bt_adapter_le_disable();

    bt_device_unset_bond_created_cb();
    bt_gatt_unset_characteristic_changed_cb();

    u_mutex_lock(gBleServiceListMutex);
    CAFreeBLEServiceList(gBLEServiceList);
    gBLEServiceList = NULL;
    u_mutex_unlock(gBleServiceListMutex);

    gIsBleGattClientStarted = CA_FALSE;

    u_mutex_unlock(gBleClientStateMutex);

    CAResetRegisteredServiceCount();

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
    u_mutex_free(gBleClientReceiveDataMutex);
    gBleClientReceiveDataMutex = NULL;

    u_mutex_free(gBleClientStateMutex);
    gBleClientStateMutex = NULL;

    u_mutex_free(gBleServiceListMutex);
    gBleServiceListMutex = NULL;

    u_mutex_free(gBleReqRespClientCbMutex);
    gBleReqRespClientCbMutex = NULL;

    u_mutex_free(gBleClientSendDataMutex);
    gBleClientSendDataMutex = NULL;

    u_mutex_free(gBleClientThreadPoolMutex);
    gBleClientThreadPoolMutex = NULL;

    u_mutex_free(gBleClientSendCondWait);
    gBleClientSendCondWait = NULL;

    OIC_LOG(DEBUG,  TZ_BLE_CLIENT_TAG, "OUT");
}

void CASetBleClientThreadPoolHandle(u_thread_pool_t handle)
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN");
    u_mutex_unlock(gBleClientThreadPoolMutex);
    gBleClientThreadPool = handle;
    u_mutex_unlock(gBleClientThreadPoolMutex);
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");
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

    unsigned char *value = (unsigned char *) OICMalloc(sizeof(char) * (dataLen + 1));
    VERIFY_NON_NULL_RET(value, TZ_BLE_CLIENT_TAG, "malloc failed", CA_STATUS_FAILED);

    memset(value, 0x0, (dataLen + 1));
    strncpy(value, data, dataLen);

    int32_t result = bt_gatt_set_characteristic_value_request(bleServiceInfo->write_char, value,
                     dataLen,
                     CABleGattCharacteristicWriteCb);

    OICFree(value);

    if (BT_ERROR_NONE != result)
    {
        OIC_LOG_V(ERROR, TZ_BLE_CLIENT_TAG,
                  "bt_gatt_set_characteristic_value_request Failed with return val [%d]",
                  result);
        return CA_STATUS_FAILED;
    }

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
void CASetBLEReqRespClientCallback(CANetworkPacketReceivedCallback callback)
{
    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "IN");

    u_mutex_lock(gBleReqRespClientCbMutex);

    gNetworkPacketReceivedClientCallback = callback;

    u_mutex_unlock(gBleReqRespClientCbMutex);

    OIC_LOG(DEBUG, TZ_BLE_CLIENT_TAG, "OUT");
}


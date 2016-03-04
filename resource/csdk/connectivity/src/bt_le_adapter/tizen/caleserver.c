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

#include "caleserver.h"
#include "cacommon.h"
#include "cacommonutil.h"
#include "camutex.h"
#include "caqueueingthread.h"
#include "cagattservice.h"
#include "oic_malloc.h"
#include "caleutil.h"

/**
 * Logging tag for module name
 */
#define TAG "OIC_CA_LE_SERVER"

/**
 * Initial buffer size for Gatt Server.
 */
#define CA_LE_INITIAL_BUF_SIZE 512

/**
 * The handle of the OIC server.
 */
static bt_gatt_server_h g_gattServer = NULL;

/**
 * The handle of the OIC service.
 */
static bt_gatt_h g_gattSvcPath = NULL;

/**
 * The handle of the OIC read characteristics.
 */
static bt_gatt_h g_gattReadCharPath = NULL;

/**
 * The handle of the OIC write characteristics.
 */
static bt_gatt_h g_gattWriteCharPath = NULL;

/**
 * The handle to control Bluetooth LE advertising.
 */
static bt_advertiser_h g_hAdvertiser = NULL;

/**
 * Callback register with LE adapter.  This callback is called on reception of any
 * data from the remote device.
 */
static CABLEDataReceivedCallback g_leServerDataReceivedCallback = NULL;

/**
 * Callback to notify any error in LE adapter.
 */
static CABLEErrorHandleCallback g_serverErrorCallback;

/**
 * To keep the state of GATT server if started or not.
 */
static bool g_isLEGattServerStarted = false;

/**
 * Mutex to synchronize the calls to start and stop server.
 */
static ca_mutex g_leServerStateMutex = NULL;

/**
 * Mutex to synchronize writing operations on the characteristics.
 */
static  ca_mutex g_leCharacteristicMutex = NULL;

/**
 * Mutex to synchronize to creation of OIC service.
 */
static  ca_mutex g_leServiceMutex = NULL;

/**
 * Mutex to synchronize access to the requestResponse callback to be called
 * when the data needs to be sent from GATTClient.
 */
static  ca_mutex g_leReqRespCbMutex = NULL;

/**
 * Mutex to synchronize the task to be pushed to thread pool.
 */
static ca_mutex g_leServerThreadPoolMutex = NULL;

/**
 * Reference to threadpool.
 */
static ca_thread_pool_t g_leServerThreadPool = NULL;

/**
 * GmainLoop to manage the threads to receive the callback from the platfrom.
 */
static GMainLoop *g_eventLoop = NULL;

static CALEConnectionStateChangedCallback g_connStateCb = NULL;

void CASetLEConnectionStateChangedCallback(CALEConnectionStateChangedCallback connStateCb)
{
    g_connStateCb = connStateCb;
}

void CALEGattServerConnectionStateChanged(bool connected, const char *remoteAddress)
{
    VERIFY_NON_NULL_VOID(remoteAddress, TAG, "remote address");

    if (connected)
    {
        OIC_LOG_V(DEBUG, TAG, "Connected to [%s]", remoteAddress);
        if (g_connStateCb)
        {
            g_connStateCb(CA_ADAPTER_GATT_BTLE, remoteAddress, true);
        }
    }
    else
    {
        OIC_LOG_V(DEBUG, TAG, "Disconnected from [%s]", remoteAddress);
        if (g_connStateCb)
        {
            g_connStateCb(CA_ADAPTER_GATT_BTLE, remoteAddress, false);
        }
    }
}

void CALEServerNotificationSentCB(int result, char *remote_address, bt_gatt_server_h server,
                                  bt_gatt_h characteristic, bool completed, void *user_data)
{
    OIC_LOG_V(DEBUG, TAG, "Notification to the device[%s] result[%d]", remote_address, result);
}

CAResult_t CAStartLEGattServer()
{
    OIC_LOG(DEBUG, TAG, "IN");

    ca_mutex_lock(g_leServerThreadPoolMutex);
    if (NULL == g_leServerThreadPool)
    {
        OIC_LOG(ERROR, TAG, "g_leServerThreadPool is NULL");
        ca_mutex_unlock(g_leServerThreadPoolMutex);
        return CA_STATUS_FAILED;
    }

    CAResult_t ret = ca_thread_pool_add_task(g_leServerThreadPool, CAStartLEGattServerThread,
                                             NULL);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(ERROR, TAG, "ca_thread_pool_add_task failed with ret [%d]", ret);
        ca_mutex_unlock(g_leServerThreadPoolMutex);
        return CA_STATUS_FAILED;
    }

    ca_mutex_unlock(g_leServerThreadPoolMutex);
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

void CALENotificationCb(bool notify, bt_gatt_server_h server, bt_gatt_h gatt_handle,
                        void *user_data)
{
    OIC_LOG(DEBUG, TAG, "IN");
    if(notify)
    {
        OIC_LOG(DEBUG, TAG, "Notification is subscribed by the client");
    }
    OIC_LOG(DEBUG, TAG, "OUT");
}

CAResult_t CALEStartAdvertise(const char *serviceUUID)
{
    OIC_LOG(DEBUG, TAG, "IN");

    int res = bt_adapter_le_create_advertiser(&g_hAdvertiser);
    if (NULL == g_hAdvertiser || BT_ERROR_NONE != res)
    {
        OIC_LOG_V(ERROR, TAG, "g_hAdvertiser is NULL/ Result is %d", res);
        return CA_STATUS_FAILED;
    }

    res = bt_adapter_le_add_advertising_service_uuid(g_hAdvertiser,
                                                     BT_ADAPTER_LE_PACKET_ADVERTISING,
                                                     serviceUUID);
    if (BT_ERROR_NONE != res)
    {
        OIC_LOG_V(ERROR, TAG, "bt_adapter_le_add_advertising_service_uuid failed with ret[%s]",
                  CALEGetErrorMsg(res));
        return CA_STATUS_FAILED;
    }

    res = bt_adapter_le_start_advertising(g_hAdvertiser, NULL, NULL, NULL);
    if (BT_ERROR_NONE != res)
    {
        OIC_LOG_V(ERROR, TAG, "bt_adapter_le_start_advertising failed with ret[%s]",
                  CALEGetErrorMsg(res));
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CALEStopAdvertise()
{
    OIC_LOG(DEBUG, TAG, "IN");
    if (NULL != g_hAdvertiser)
    {
        int ret  = bt_adapter_le_stop_advertising(g_hAdvertiser);
        if (0 != ret)
        {
            OIC_LOG_V(ERROR, TAG,
                      "bt_adapter_le_stop_advertising failed with ret[%s]", CALEGetErrorMsg(ret));
        }

        ret = bt_adapter_le_destroy_advertiser(g_hAdvertiser);
        if (0 != ret)
        {
            OIC_LOG_V(ERROR, TAG,
                      "bt_adapter_le_destroy_advertiser failed with ret[%s]", CALEGetErrorMsg(ret));
        }
        g_hAdvertiser = NULL;
    }
    else
    {
        OIC_LOG(ERROR, TAG, "Advertising is not running");
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

void CAStartLEGattServerThread(void *data)
{
    OIC_LOG(DEBUG, TAG, "IN");
    ca_mutex_lock(g_leServerStateMutex);
    if (true == g_isLEGattServerStarted)
    {
        OIC_LOG(ERROR, TAG, "Gatt Server is already running");
        ca_mutex_unlock(g_leServerStateMutex);
        CATerminateLEGattServer();
        return;
    }

    CAResult_t ret  =  CAInitLEGattServer();
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(ERROR, TAG, "CAInitLEGattService failed[%d]", ret);
        ca_mutex_unlock(g_leServerStateMutex);
        CATerminateLEGattServer();
        return;
    }

    char *serviceUUID = CA_GATT_SERVICE_UUID;

    ret  = CAAddNewLEServiceInGattServer(serviceUUID);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(ERROR, TAG, "CAAddNewLEServiceInGattServer failed[%d]", ret);
        ca_mutex_unlock(g_leServerStateMutex);
        CATerminateLEGattServer();
        return;
    }

    static const char charReadUUID[] = CA_GATT_RESPONSE_CHRC_UUID;
    char charReadValue[] = {33, 44, 55, 66}; // These are initial random values

    // For Read Characteristics.
    ret = CAAddNewCharacteristicsToGattServer(g_gattSvcPath, charReadUUID, charReadValue,
                                              CA_LE_INITIAL_BUF_SIZE, true);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(ERROR, TAG, "CAAddNewCharacteristicsToGattServer failed[%d]", ret);
        ca_mutex_unlock(g_leServerStateMutex);
        CATerminateLEGattServer();
        return;
    }

    static const char charWriteUUID[] = CA_GATT_REQUEST_CHRC_UUID;
    char charWriteValue[] = {33, 44, 55, 66}; // These are initial random values


    ret = CAAddNewCharacteristicsToGattServer(g_gattSvcPath, charWriteUUID, charWriteValue,
            CA_LE_INITIAL_BUF_SIZE, false); // For Write Characteristics.
    if (CA_STATUS_OK != ret )
    {
        OIC_LOG_V(ERROR, TAG, "CAAddNewCharacteristicsToGattServer failed[%d]", ret);
        ca_mutex_unlock(g_leServerStateMutex);
        CATerminateLEGattServer();
        return;
    }

    ret = CARegisterLEServicewithGattServer(g_gattSvcPath);
    if (CA_STATUS_OK != ret )
    {
        OIC_LOG_V(ERROR, TAG, "CARegisterLEServicewithGattServer failed[%d]", ret);
        ca_mutex_unlock(g_leServerStateMutex);
        CATerminateLEGattServer();
        return;
    }

    ret = CALEStartAdvertise(serviceUUID);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(ERROR, TAG, "CALEStartAdvertise failed[%d]", ret);
        ca_mutex_unlock(g_leServerStateMutex);
        CATerminateLEGattServer();
        return;
    }

    g_isLEGattServerStarted = true;

    ca_mutex_unlock(g_leServerStateMutex);

    OIC_LOG(DEBUG, TAG, "LE Server initialization complete.");

    GMainContext *thread_context = NULL;

    thread_context = g_main_context_new();

    g_eventLoop = g_main_loop_new(thread_context, FALSE);

    g_main_context_push_thread_default(thread_context);

    g_main_loop_run(g_eventLoop);

    OIC_LOG(DEBUG, TAG, "OUT");
}

CAResult_t CAStopLEGattServer()
{
    OIC_LOG(DEBUG, TAG, "IN");

    ca_mutex_lock(g_leServerStateMutex);

    if (false == g_isLEGattServerStarted)
    {
        OIC_LOG(ERROR, TAG, "Gatt Server is not running to stop");
        ca_mutex_unlock(g_leServerStateMutex);
        return CA_STATUS_OK;
    }

    g_isLEGattServerStarted = false;

    CAResult_t res = CALEStopAdvertise();
    {
        OIC_LOG_V(ERROR, TAG, "CALEStopAdvertise failed with ret[%d]", res);
    }

    res = CADeInitLEGattServer();
    if (CA_STATUS_OK != res)
    {
        OIC_LOG_V(ERROR, TAG, "CADeInitLEGattService failed with ret[%d]", res);
    }

    GMainContext  *context_event_loop = NULL;
    // Required for waking up the thread which is running in gmain loop
    if (NULL != g_eventLoop)
    {
        context_event_loop = g_main_loop_get_context(g_eventLoop);

        if (context_event_loop)
        {
            OIC_LOG_V(DEBUG,  TAG, "g_eventLoop context %x", context_event_loop);
            g_main_context_wakeup(context_event_loop);

            // Kill g main loops and kill threads
            g_main_loop_quit(g_eventLoop);
        }
    }
    else
    {
        OIC_LOG(ERROR, TAG, "g_eventLoop context is NULL");
    }

    ca_mutex_unlock(g_leServerStateMutex);

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAInitializeLEGattServer()
{
    OIC_LOG(DEBUG, TAG, "IN");

    CAResult_t ret = CAInitGattServerMutexVariables();
    if (CA_STATUS_OK != ret )
    {
        OIC_LOG(ERROR, TAG, "CAInitGattServerMutexVariables failed!");
        CATerminateGattServerMutexVariables();
        return CA_SERVER_NOT_STARTED;
    }
    OIC_LOG(DEBUG, TAG, "OUT");
    return ret;
}

void CATerminateLEGattServer()
{
    OIC_LOG(DEBUG, TAG, "IN");

    // Service and characteristics path will be freed by the platform.
    ca_mutex_lock(g_leServiceMutex);
    g_gattSvcPath = NULL;
    ca_mutex_unlock(g_leServiceMutex);

    ca_mutex_lock(g_leCharacteristicMutex);
    g_gattReadCharPath = NULL;
    g_gattWriteCharPath = NULL;
    ca_mutex_unlock(g_leCharacteristicMutex);

    ca_mutex_lock(g_leServerThreadPoolMutex);
    g_leServerThreadPool = NULL;
    ca_mutex_unlock(g_leServerThreadPoolMutex);

    // Terminating all mutex variables.
    CATerminateGattServerMutexVariables();
    OIC_LOG(DEBUG, TAG, "OUT");
}

CAResult_t CAInitGattServerMutexVariables()
{
    OIC_LOG(DEBUG, TAG, "IN");
    if (NULL == g_leServerStateMutex)
    {
        g_leServerStateMutex = ca_mutex_new();
        if (NULL == g_leServerStateMutex)
        {
            OIC_LOG(ERROR, TAG, "ca_mutex_new failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == g_leServiceMutex)
    {
        g_leServiceMutex = ca_mutex_new();
        if (NULL == g_leServiceMutex)
        {
            OIC_LOG(ERROR, TAG, "ca_mutex_new failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == g_leCharacteristicMutex)
    {
        g_leCharacteristicMutex = ca_mutex_new();
        if (NULL == g_leCharacteristicMutex)
        {
            OIC_LOG(ERROR, TAG, "ca_mutex_new failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == g_leReqRespCbMutex)
    {
        g_leReqRespCbMutex = ca_mutex_new();
        if (NULL == g_leReqRespCbMutex)
        {
            OIC_LOG(ERROR, TAG, "ca_mutex_new failed");
            return CA_STATUS_FAILED;
        }
    }
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

void CATerminateGattServerMutexVariables()
{
    OIC_LOG(DEBUG, TAG, "IN");
    ca_mutex_free(g_leServerStateMutex);
    g_leServerStateMutex = NULL;

    ca_mutex_free(g_leServiceMutex);
    g_leServiceMutex = NULL;

    ca_mutex_free(g_leCharacteristicMutex);
    g_leCharacteristicMutex = NULL;

    ca_mutex_free(g_leReqRespCbMutex);
    g_leReqRespCbMutex = NULL;

    OIC_LOG(DEBUG, TAG, "OUT");
}

CAResult_t CAInitLEGattServer()
{
    OIC_LOG(DEBUG, TAG, "IN");

    int ret =  bt_gatt_server_initialize();
    if (0 != ret)
    {
        OIC_LOG_V(ERROR, TAG, "bt_gatt_server_initialize failed with ret[%s]",
                  CALEGetErrorMsg(ret));
        return CA_STATUS_FAILED;
    }

    bt_gatt_server_h server;

    ret = bt_gatt_server_create(&server);
    if (0 != ret)
    {
        OIC_LOG_V(ERROR, TAG, "bt_gatt_server_create failed with ret[%s]",
                  CALEGetErrorMsg(ret));
        return CA_STATUS_FAILED;
    }

    if (NULL != g_gattServer)
    {
        OICFree(g_gattServer);
        g_gattServer = NULL;
    }
    g_gattServer = server;

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CADeInitLEGattServer()
{
    OIC_LOG(DEBUG, TAG, "IN");

    int ret = bt_gatt_server_unregister_all_services(g_gattServer);
    if (0 != ret)
    {
        OIC_LOG_V(ERROR, TAG, "bt_gatt_server_unregister_all_services failed with ret[%s]",
                  CALEGetErrorMsg(ret));
        return CA_STATUS_FAILED;
    }

    ret = bt_gatt_server_destroy(g_gattServer);
    if (0 != ret)
    {
        OIC_LOG_V(ERROR, TAG, "bt_gatt_server_destroy failed with ret[%s]",
                  CALEGetErrorMsg(ret));
        return CA_STATUS_FAILED;
    }

    ret =  bt_gatt_server_deinitialize();
    if (0 != ret)
    {
        OIC_LOG_V(ERROR, TAG, "bt_gatt_server_deinitialize failed with ret[%s]",
                  CALEGetErrorMsg(ret));
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

void CASetLEServerThreadPoolHandle(ca_thread_pool_t handle)
{
    OIC_LOG(DEBUG, TAG, "IN");
    ca_mutex_lock(g_leServerThreadPoolMutex);
    g_leServerThreadPool = handle;
    ca_mutex_unlock(g_leServerThreadPoolMutex);
    OIC_LOG(DEBUG, TAG, "OUT");
}

CAResult_t CAAddNewLEServiceInGattServer(const char *serviceUUID)
{
    OIC_LOG(DEBUG, TAG, "IN");

    VERIFY_NON_NULL(serviceUUID, TAG, "serviceUUID");

    OIC_LOG_V(DEBUG, TAG, "service uuid %s", serviceUUID);

    bt_gatt_h service = NULL;
    bt_gatt_service_type_e type = BT_GATT_SERVICE_TYPE_PRIMARY;

    int ret = bt_gatt_service_create(serviceUUID, type, &service);
    if (0 != ret)
    {
        OIC_LOG_V(ERROR, TAG, "bt_gatt_service_create failed with ret [%s]",
                  CALEGetErrorMsg(ret));
        return CA_STATUS_FAILED;
    }

    if (NULL != service)
    {
        OIC_LOG_V(DEBUG, TAG, "ServicePath obtained is %s", (char *)service);

        ca_mutex_lock(g_leServiceMutex);

        if (NULL != g_gattSvcPath)
        {
            OICFree(g_gattSvcPath);
            g_gattSvcPath = NULL;
        }
        g_gattSvcPath = service;

        ca_mutex_unlock(g_leServiceMutex);
    }

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

void CALEGattRemoteCharacteristicWriteCb(char *remoteAddress, bt_gatt_server_h server,
                                         bt_gatt_h charPath, int offset, char *charValue,
                                         int charValueLen, void *userData)
{
    OIC_LOG(DEBUG, TAG, "IN");

    if (NULL == charValue || NULL == remoteAddress)
    {
        OIC_LOG(ERROR, TAG, "Param callback values are NULL");
        return;
    }

    OIC_LOG_V(DEBUG, TAG, "charPath = [%s] charValue = [%s] len [%d]", (char *)charPath,
              charValue, charValueLen);

    uint8_t *data = OICMalloc(charValueLen);
    if (NULL == data)
    {
        OIC_LOG(ERROR, TAG, "Malloc failed!");
        return;
    }

    memcpy(data, charValue, charValueLen);

    ca_mutex_lock(g_leReqRespCbMutex);
    if (NULL == g_leServerDataReceivedCallback)
    {
        OIC_LOG(ERROR, TAG, "gReqRespCallback is NULL!");
        ca_mutex_unlock(g_leReqRespCbMutex);
        OICFree(data);
        return;
    }

    OIC_LOG(DEBUG, TAG, "Sending data up !");
    uint32_t sentLength = 0;
    g_leServerDataReceivedCallback(remoteAddress, data, charValueLen,
                                    &sentLength);
    ca_mutex_unlock(g_leReqRespCbMutex);
    OICFree(data);
    OIC_LOG(DEBUG, TAG, "OUT");
}

CAResult_t CARegisterLEServicewithGattServer(const bt_gatt_h svcPath)
{
    OIC_LOG(DEBUG, TAG, "IN");

    VERIFY_NON_NULL(svcPath, TAG, "svcPath");

    OIC_LOG_V(DEBUG, TAG, "svcPath:%s", svcPath);

    int ret = bt_gatt_server_register_service(g_gattServer, svcPath);
    if (0 != ret)
    {
        OIC_LOG_V(ERROR, TAG, "bt_gatt_server_register_service failed with ret[%s]",
                  CALEGetErrorMsg(ret));
        return CA_STATUS_FAILED;
    }

    ret = bt_gatt_server_set_value_changed_cb(g_gattWriteCharPath,
                                              CALEGattRemoteCharacteristicWriteCb, NULL);

    if (0 != ret)
    {
        OIC_LOG_V(ERROR, TAG, "bt_gatt_server_set_value_changed_cb failed with ret[%s]",
                  CALEGetErrorMsg(ret));
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAAddNewCharacteristicsToGattServer(const bt_gatt_h svcPath, const char *charUUID,
                                               const char *charValue, int charValueLen, bool read)
{

    OIC_LOG(DEBUG, TAG, "IN");

    int permissions = BT_GATT_PERMISSION_READ | BT_GATT_PERMISSION_WRITE;
    int properties = BT_GATT_PROPERTY_WRITE | BT_GATT_PROPERTY_NOTIFY;
    if(read)
    {
        properties = BT_GATT_PROPERTY_NOTIFY | BT_GATT_PROPERTY_READ;
    }
    else
    {
        properties = BT_GATT_PROPERTY_WRITE | BT_GATT_PROPERTY_READ;
    }

    bt_gatt_h charPath;

    int ret = bt_gatt_characteristic_create(charUUID, permissions, properties, charValue,
                                            charValueLen, &charPath);

    if (0 != ret || NULL == charPath)
    {
        OIC_LOG_V(ERROR, TAG,
                  "bt_gatt_add_characteristic  failed with ret [%s]", CALEGetErrorMsg(ret));
        return CA_STATUS_FAILED;
    }

    OIC_LOG_V(DEBUG, TAG,
              "bt_gatt_characteristic_create charPath obtained: %s", (char *)charPath);

    if (read)
    {
        ret = bt_gatt_server_set_notification_state_change_cb(charPath, CALENotificationCb, NULL);
        if (0 != ret)
        {
            OIC_LOG_V(ERROR, TAG,
                      "bt_gatt_server_set_notification_state_change_cb  failed with ret[%s]",
                      CALEGetErrorMsg(ret));
            return CA_STATUS_FAILED;
        }
    }

    ret =  bt_gatt_service_add_characteristic(svcPath, charPath);
    if (0 != ret)
    {
        OIC_LOG_V(ERROR, TAG,
                  "bt_gatt_service_add_characteristic  failed with ret[%s]",
                  CALEGetErrorMsg(ret));
        return CA_STATUS_FAILED;
    }

    ca_mutex_lock(g_leCharacteristicMutex);

    if (read)
    {
        char desc_value[2] = {1, 0};  // Notification enabled.
        bt_gatt_h descriptor = NULL;
        permissions = BT_GATT_PERMISSION_READ | BT_GATT_PERMISSION_WRITE;
        ret = bt_gatt_descriptor_create(CA_GATT_CONFIGURATION_DESC_UUID,
                                        permissions, desc_value, sizeof(desc_value), &descriptor);
        if (0 != ret)
        {
            OIC_LOG_V(ERROR, TAG,
                      "bt_gatt_descriptor_create  failed with ret[%s]",
                      CALEGetErrorMsg(ret));
            return CA_STATUS_FAILED;
        }

        ret = bt_gatt_characteristic_add_descriptor(charPath, descriptor);
        if (0 != ret)
        {
            OIC_LOG_V(ERROR, TAG,
                      "bt_gatt_characteristic_add_descriptor  failed with ret[%s]",
                      CALEGetErrorMsg(ret));
            return CA_STATUS_FAILED;
        }

        if (NULL != g_gattReadCharPath)
        {
            OICFree(g_gattReadCharPath);
            g_gattReadCharPath = NULL;
        }
        g_gattReadCharPath = charPath;

    }
    else
    {
        if (NULL != g_gattWriteCharPath)
        {
            OICFree(g_gattWriteCharPath);
            g_gattWriteCharPath = NULL;
        }
        g_gattWriteCharPath = charPath;
    }

    ca_mutex_unlock(g_leCharacteristicMutex);

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAUpdateCharacteristicsToGattClient(const char *address, const uint8_t *charValue,
                                               uint32_t charValueLen)
{
    OIC_LOG(DEBUG, TAG, "IN");

    VERIFY_NON_NULL(charValue, TAG, "charValue");
    VERIFY_NON_NULL(address, TAG, "address");

    OIC_LOG_V(DEBUG, TAG, "Client's Unicast address for sending data [%s]", address);

    ca_mutex_lock(g_leCharacteristicMutex);

    if (NULL  == g_gattReadCharPath)
    {
        OIC_LOG(ERROR, TAG, "g_gattReadCharPath is NULL");
        ca_mutex_unlock(g_leCharacteristicMutex);
        return CA_STATUS_FAILED;
    }

    int ret = bt_gatt_set_value(g_gattReadCharPath, (char *)charValue, charValueLen);
    if (0 != ret)
    {
        OIC_LOG_V(ERROR, TAG,
                  "bt_gatt_set_value failed with return [%s]", CALEGetErrorMsg(ret));
        ca_mutex_unlock(g_leCharacteristicMutex);
        return CA_STATUS_FAILED;
    }

    ret = bt_gatt_server_notify(g_gattReadCharPath, false, CALEServerNotificationSentCB,
                                address, NULL);
    if (0 != ret)
    {
        OIC_LOG_V(ERROR, TAG,
                  "bt_gatt_server_notify failed with return [%s]", CALEGetErrorMsg(ret));
        ca_mutex_unlock(g_leCharacteristicMutex);
        return CA_STATUS_FAILED;
    }

    ca_mutex_unlock(g_leCharacteristicMutex);

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAUpdateCharacteristicsToAllGattClients(const uint8_t *charValue, uint32_t charValueLen)
{
    OIC_LOG(DEBUG, TAG, "IN");

    VERIFY_NON_NULL(charValue, TAG, "charValue");

    ca_mutex_lock(g_leCharacteristicMutex);

    if (NULL  == g_gattReadCharPath)
    {
        OIC_LOG(ERROR, TAG, "g_gattReadCharPath is NULL");
        ca_mutex_unlock(g_leCharacteristicMutex);
        return CA_STATUS_FAILED;
    }

    int ret = bt_gatt_set_value(g_gattReadCharPath, (char *)charValue, charValueLen);
    if (0 != ret)
    {
        OIC_LOG_V(ERROR, TAG, "bt_gatt_set_value failed with return[%s]", CALEGetErrorMsg(ret));
        ca_mutex_unlock(g_leCharacteristicMutex);
        return CA_STATUS_FAILED;
    }

    ret = bt_gatt_server_notify(g_gattReadCharPath, false, CALEServerNotificationSentCB,
                                NULL, NULL);
    if (0 != ret)
    {
        OIC_LOG_V(ERROR, TAG,
                  "bt_gatt_server_notify failed with return[%s]", CALEGetErrorMsg(ret));
        ca_mutex_unlock(g_leCharacteristicMutex);
        return CA_STATUS_FAILED;
    }

    ca_mutex_unlock(g_leCharacteristicMutex);

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

void CASetLEReqRespServerCallback(CABLEDataReceivedCallback callback)
{
    OIC_LOG(DEBUG, TAG, "IN");

    ca_mutex_lock(g_leReqRespCbMutex);
    g_leServerDataReceivedCallback = callback;
    ca_mutex_unlock(g_leReqRespCbMutex);

    OIC_LOG(DEBUG, TAG, "OUT");
}

void CASetBLEServerErrorHandleCallback(CABLEErrorHandleCallback callback)
{
    g_serverErrorCallback = callback;
}

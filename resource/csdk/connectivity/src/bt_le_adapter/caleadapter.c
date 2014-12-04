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
#define CALEADAPTER_TAG "CA_BLE_ADAPTER"

static CANetworkChangeCallback gNetworkCallback = NULL;

static char gLocalBLEAddress[16] = { 0, };

static CABool_t gIsServer = CA_FALSE;

static u_mutex gBleIsServerMutex = NULL;

static u_mutex gBleNetworkCbMutex = NULL;

static u_mutex gBleLocalAddressMutex = NULL;

int32_t CALERegisterNetworkNotifications(CANetworkChangeCallback netCallback);

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
    CASetBLEReqRespServerCallback(reqRespCallback);
    CASetBLEReqRespClientCallback(reqRespCallback);
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

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return;
}

CAResult_t CAStartLEListeningServer()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

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
        result = CABleServerSenderQueueEnqueueMessage(endpoint, data, dataLen);
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

        result = CABleClientSenderQueueEnqueueMessage(endpoint, data, dataLen);
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
        result = CABleServerSenderQueueEnqueueMessage(NULL, data, dataLen);
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
        result = CABleClientSenderQueueEnqueueMessage(NULL, data, dataLen);
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
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}
#endif //#ifdef OIC_TIZEN

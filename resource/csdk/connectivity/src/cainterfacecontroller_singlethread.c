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

#include "cainterfacecontroller_singlethread.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "caipadapter_singlethread.h"
#include "caedradapter_singlethread.h"
#include "caleadapter_singlethread.h"
#include "caadapterutils.h"

#include "canetworkconfigurator.h"
#include "oic_malloc.h"
#include "logger.h"

#define TAG "CAIFCNT_ST"

#define CA_MEMORY_ALLOC_CHECK(arg) { if (arg == NULL) {OIC_LOG(ERROR, TAG, "Out of memory");\
    goto memory_error_exit;} }

#define CA_CONNECTIVITY_TYPE_NUM   4

static CAConnectivityHandler_t g_adapterHandler[CA_CONNECTIVITY_TYPE_NUM];

static CANetworkPacketReceivedCallback g_networkPacketReceivedCallback = NULL;

static CANetworkChangeCallback g_networkChangeCallback = NULL;

static int CAGetAdapterIndex(CATransportType_t cType)
{
    switch (cType)
    {
        case CA_IPV4:
            return 0;
        case CA_IPV6:
            return 1;
        case CA_EDR:
            return 2;
        case CA_LE:
            return 3;
    }

    OIC_LOG(DEBUG, TAG, "CA_CONNECTIVITY_TYPE_NUM is not 4");

    return -1;
}

static void CARegisterCallback(CAConnectivityHandler_t handler, CATransportType_t cType)
{
    OIC_LOG(DEBUG, TAG, "IN");

    if(handler.startAdapter == NULL ||
        handler.startListenServer == NULL ||
        handler.startDiscoveryServer == NULL ||
        handler.sendData == NULL ||
        handler.sendDataToAll == NULL ||
        handler.GetnetInfo == NULL ||
        handler.readData == NULL ||
        handler.stopAdapter == NULL ||
        handler.terminate == NULL)
    {
        OIC_LOG(ERROR, TAG, "connectivity handler is not enough to be used!");
        return;
    }

    int index = CAGetAdapterIndex(cType);

    if (index == -1)
    {
        OIC_LOG(DEBUG, TAG, "unknown connectivity type!");
        return;
    }

    memcpy(&g_adapterHandler[index], &handler, sizeof(CAConnectivityHandler_t));

    OIC_LOG_V(DEBUG, TAG, "%d type adapter", cType);
    OIC_LOG(DEBUG, TAG, "OUT");
}

static void CAReceivedPacketCallback(CARemoteEndpoint_t *endpoint, void *data,
    uint32_t dataLen)
{
    OIC_LOG(DEBUG, TAG, "IN");

    // Call the callback.
    if (g_networkPacketReceivedCallback != NULL)
    {
        g_networkPacketReceivedCallback(endpoint, data, dataLen);
    }
    OIC_LOG(DEBUG, TAG, "OUT");
}

static void CANetworkChangedCallback(CALocalConnectivity_t *info, CANetworkStatus_t status)
{
    OIC_LOG(DEBUG, TAG, "IN");

    // Call the callback.
    if (g_networkChangeCallback != NULL)
    {
        g_networkChangeCallback(info, status);
    }
    OIC_LOG(DEBUG, TAG, "OUT");
}

void CAInitializeAdapters()
{
    OIC_LOG(DEBUG, TAG, "IN");

    memset(g_adapterHandler, 0, sizeof(CAConnectivityHandler_t) * CA_CONNECTIVITY_TYPE_NUM);

    // Initialize adapters and register callback.
#ifdef IP_ADAPTER
    CAInitializeIP(CARegisterCallback, CAReceivedPacketCallback, CANetworkChangedCallback);
#endif /* IP_ADAPTER */

#ifdef EDR_ADAPTER
    CAInitializeEDR(CARegisterCallback, CAReceivedPacketCallback, CANetworkChangedCallback);
#endif /* EDR_ADAPTER */

#ifdef LE_ADAPTER
    CAInitializeLE(CARegisterCallback, CAReceivedPacketCallback, CANetworkChangedCallback);
#endif /* LE_ADAPTER */

    OIC_LOG(DEBUG, TAG, "OUT");
}

void CASetPacketReceivedCallback(CANetworkPacketReceivedCallback callback)
{
    OIC_LOG(DEBUG, TAG, "IN");

    g_networkPacketReceivedCallback = callback;
    OIC_LOG(DEBUG, TAG, "OUT");
}

void CASetNetworkChangeCallback(CANetworkChangeCallback callback)
{
    OIC_LOG(DEBUG, TAG, "IN");

    g_networkChangeCallback = callback;
    OIC_LOG(DEBUG, TAG, "OUT");
}

CAResult_t CAStartAdapter(CATransportType_t transportType)
{
    OIC_LOG_V(DEBUG, TAG, "transportType[%d]", transportType);

    int index = CAGetAdapterIndex(transportType);

    if (index == -1)
    {
        OIC_LOG(DEBUG, TAG, "unknown connectivity type!");
        return CA_STATUS_FAILED;
    }

    if (g_adapterHandler[index].startAdapter != NULL)
    {
        g_adapterHandler[index].startAdapter();
    }
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

void CAStopAdapter(CATransportType_t transportType)
{
    OIC_LOG_V(DEBUG, TAG, "transportType[%d]", transportType);

    int index = CAGetAdapterIndex(transportType);

    if (index == -1)
    {
        OIC_LOG(DEBUG, TAG, "unknown connectivity type!");
        return;
    }

    if (g_adapterHandler[index].stopAdapter != NULL)
    {
        g_adapterHandler[index].stopAdapter();
    }
    OIC_LOG(DEBUG, TAG, "OUT");
}

CAResult_t CAGetNetworkInfo(CALocalConnectivity_t **info, uint32_t *size)
{
    OIC_LOG(DEBUG, TAG, "IN");
    VERIFY_NON_NULL(info, TAG, "info");
    VERIFY_NON_NULL(size, TAG, "size");

    CALocalConnectivity_t *tempInfo[CA_CONNECTIVITY_TYPE_NUM] = { 0 };
    uint32_t tempSize[CA_CONNECTIVITY_TYPE_NUM] = { 0 };

    CAResult_t res = CA_STATUS_FAILED;
    // #1. get information each adapter
    for (uint8_t index = 0; index < CA_CONNECTIVITY_TYPE_NUM; index++)
    {
        if (g_adapterHandler[index].GetnetInfo != NULL)
        {
            res = g_adapterHandler[index].GetnetInfo(&tempInfo[index], &tempSize[index]);

            OIC_LOG_V (DEBUG, TAG, "%d adapter network info size is %d", index, tempSize[index]);
        }
    }

    uint32_t resSize = 0;
    for (uint8_t index = 0; index < CA_CONNECTIVITY_TYPE_NUM; index++)
    {
        // check information
        if (tempInfo[index] == NULL || tempSize[index] <= 0)
        {
            continue;
        }

        // #2. total size
        resSize += tempSize[index];
    }

    OIC_LOG_V(DEBUG, TAG, "network info total size is %d!", resSize);

    if (resSize <= 0)
    {
        if (CA_ADAPTER_NOT_ENABLED == res || CA_NOT_SUPPORTED == res)
        {
            return res;
        }
        return CA_STATUS_FAILED;
    }

    // #3. add data into result
    // memory allocation
    CALocalConnectivity_t *resInfo =
        (CALocalConnectivity_t *) OICCalloc(resSize, sizeof(CALocalConnectivity_t));
    CA_MEMORY_ALLOC_CHECK(resInfo);

    uint8_t i = 0;
    for (uint8_t index = 0; index < CA_CONNECTIVITY_TYPE_NUM; index++)
    {
        // check information
        if (tempInfo[index] == NULL || tempSize[index] <= 0)
        {
            continue;
        }

        memcpy(resInfo + i, tempInfo[index], sizeof(CALocalConnectivity_t) * tempSize[index]);

        i += tempSize[index];

        // free adapter data
        OICFree(tempInfo[index]);
    }

    // #5. save data
    *info = resInfo;
    *size = resSize;

    OIC_LOG(DEBUG, TAG, "OUT");

    return res;

    // memory error label.
memory_error_exit:

    for (uint8_t index = 0; index < CA_CONNECTIVITY_TYPE_NUM; index++)
    {

        OICFree(tempInfo[index]);
    }

    return CA_MEMORY_ALLOC_FAILED;
}

CAResult_t CASendUnicastData(const CARemoteEndpoint_t *endpoint, const void *data, uint32_t length)
{
    OIC_LOG(DEBUG, TAG, "IN");

    CAResult_t res = CA_STATUS_FAILED;

    if (endpoint == NULL)
    {
        OIC_LOG(DEBUG, TAG, "RemoteEndpoint is NULL");
        return CA_STATUS_INVALID_PARAM;
    }

    CATransportType_t type = endpoint->transportType;

    int index = CAGetAdapterIndex(type);

    if (index == -1)
    {
        OIC_LOG(DEBUG, TAG, "unknown connectivity type!");
        return CA_STATUS_INVALID_PARAM;
    }

    uint32_t sentDataLen = 0;
    if (g_adapterHandler[index].sendData != NULL)
    {
        sentDataLen = g_adapterHandler[index].sendData(endpoint, data, length);
    }

    if (sentDataLen == length)
    {
        res = CA_STATUS_OK;
    }

    OIC_LOG(DEBUG, TAG, "OUT");
    return res;
}

CAResult_t CASendMulticastData(const void *data, uint32_t length)
{
    OIC_LOG(DEBUG, TAG, "IN");

    CAResult_t res = CA_STATUS_FAILED;
    u_arraylist_t *list = CAGetSelectedNetworkList();

    if (!list)
    {
        OIC_LOG(DEBUG, TAG, "No selected network");
        return res;
    }

    for (uint8_t i = 0; i < u_arraylist_length(list); i++)
    {
        void* ptrType = u_arraylist_get(list, i);
        if (NULL == ptrType)
        {
            continue;
        }

        CATransportType_t connType = *(CATransportType_t *) ptrType;

        int index = CAGetAdapterIndex(connType);

        if (index == -1)
        {
            OIC_LOG(DEBUG, TAG, "unknown connectivity type!");
            continue;
        }

        uint32_t sentDataLen = 0;
        if (g_adapterHandler[index].sendDataToAll != NULL)
        {
            sentDataLen = g_adapterHandler[index].sendDataToAll(data, length);
        }

        if (sentDataLen == length)
        {
            res = CA_STATUS_OK;
        }
    }
    OIC_LOG(DEBUG, TAG, "OUT");
    return res;
}

CAResult_t CAStartListeningServerAdapters()
{
    OIC_LOG(DEBUG, TAG, "IN");

    u_arraylist_t *list = CAGetSelectedNetworkList();

    if (!list)
    {
        OIC_LOG(DEBUG, TAG, "No selected network");
        return CA_STATUS_FAILED;
    }

    for (uint8_t i = 0; i < u_arraylist_length(list); i++)
    {
        void* ptrType = u_arraylist_get(list, i);
        if (NULL == ptrType)
        {
            OIC_LOG(ERROR, TAG, "Invalid conn type");
            continue;
        }

        CATransportType_t connType = *(CATransportType_t *) ptrType;

        int index = CAGetAdapterIndex(connType);

        if (index == -1)
        {
            OIC_LOG(DEBUG, TAG, "unknown connectivity type!");
            continue;
        }

        if (g_adapterHandler[index].startListenServer != NULL)
        {
            g_adapterHandler[index].startListenServer();
        }
    }
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAStartDiscoveryServerAdapters()
{
    OIC_LOG(DEBUG, TAG, "IN");

    u_arraylist_t *list = CAGetSelectedNetworkList();

    if (!list)
    {
        OIC_LOG(DEBUG, TAG, "No selected network");
        return CA_STATUS_FAILED;
    }

    for (uint8_t i = 0; i < u_arraylist_length(list); i++)
    {
        void* ptrType = u_arraylist_get(list, i);
        if (NULL == ptrType)
        {
            continue;
        }

        CATransportType_t connType = *(CATransportType_t *) ptrType;

        int index = CAGetAdapterIndex(connType);

        if (index == -1)
        {
            OIC_LOG(DEBUG, TAG, "unknown connectivity type!");
            continue;
        }

        if (g_adapterHandler[index].startDiscoveryServer != NULL)
        {
            g_adapterHandler[index].startDiscoveryServer();
        }
    }
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

void CATerminateAdapters()
{
    OIC_LOG(DEBUG, TAG, "IN");

    uint8_t index;

    for (index = 0; index < CA_CONNECTIVITY_TYPE_NUM; index++)
    {
        if (g_adapterHandler[index].stopAdapter != NULL)
        {
            g_adapterHandler[index].stopAdapter();
        }
        if (g_adapterHandler[index].terminate != NULL)
        {
            g_adapterHandler[index].terminate();
        }
    }

    OIC_LOG(DEBUG, TAG, "OUT");
}

CAResult_t CAReadData()
{
    OIC_LOG(DEBUG, TAG, "IN");
    u_arraylist_t *list = CAGetSelectedNetworkList();

    if (!list)
    {
        return CA_STATUS_FAILED;
    }

    for (uint8_t i = 0; i < u_arraylist_length(list); i++)
    {
        void *ptrType = u_arraylist_get(list, i);
        if (NULL == ptrType)
        {
            OIC_LOG(ERROR, TAG, "get list fail");
            return CA_STATUS_FAILED;
        }

        CATransportType_t connType = *(CATransportType_t *) ptrType;

        int index = CAGetAdapterIndex(connType);

        if (-1 == index)
        {
            OIC_LOG(DEBUG, TAG, "unknown connectivity type!");
            continue;
        }

        if (g_adapterHandler[index].readData != NULL)
        {
            g_adapterHandler[index].readData();
        }
    }

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}


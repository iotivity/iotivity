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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "cainterfacecontroller.h"
#include "caipadapter.h"
#include "caedradapter.h"
#include "caleadapter.h"
#include "canetworkconfigurator.h"
#include "caremotehandler.h"
#include "oic_malloc.h"
#include "logger.h"
#include "cathreadpool.h"

#define TAG PCF("CA")

#define CA_MEMORY_ALLOC_CHECK(arg) {if (arg == NULL) \
    {OIC_LOG(ERROR, TAG, "memory error");goto memory_error_exit;} }

#define CA_TRANSPORT_TYPE_NUM   4

static CAConnectivityHandler_t g_adapterHandler[CA_TRANSPORT_TYPE_NUM];

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

    OIC_LOG(DEBUG, TAG, "CA_TRANSPORT_TYPE_NUM is not 4");

    return -1;
}

static void CARegisterCallback(CAConnectivityHandler_t handler, CATransportType_t cType)
{
    OIC_LOG(DEBUG, TAG, "CARegisterCallback - Entry");

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
        OIC_LOG(ERROR, TAG, "unknown connectivity type!");
        return;
    }

    memcpy(&g_adapterHandler[index], &handler, sizeof(CAConnectivityHandler_t));

    OIC_LOG_V(DEBUG, TAG, "%d type adapter, register complete!", cType);
}

static void CAReceivedPacketCallback(CARemoteEndpoint_t *endpoint, void *data,
                                     uint32_t dataLen)
{
    OIC_LOG(DEBUG, TAG, "receivedPacketCallback in interface controller");

    // Call the callback.
    if (g_networkPacketReceivedCallback != NULL)
    {
        g_networkPacketReceivedCallback(endpoint, data, dataLen);
    }
    else
    {
        OICFree(endpoint);
        endpoint = NULL;
        OICFree(data);
        data = NULL;

        OIC_LOG(ERROR, TAG, "network packet received callback is NULL!");
    }
}

static void CANetworkChangedCallback(CALocalConnectivity_t *info,
                                     CANetworkStatus_t status)
{
    OIC_LOG(DEBUG, TAG, "Network Changed callback");

    // Call the callback.
    if (g_networkChangeCallback != NULL)
    {
        g_networkChangeCallback(info, status);
    }
}

void CAInitializeAdapters(ca_thread_pool_t handle)
{
    OIC_LOG(DEBUG, TAG, "initialize adapters..");

    memset(g_adapterHandler, 0, sizeof(CAConnectivityHandler_t) * CA_TRANSPORT_TYPE_NUM);

    // Initialize adapters and register callback.
#ifdef IP_ADAPTER
    CAInitializeIP(CARegisterCallback, CAReceivedPacketCallback, CANetworkChangedCallback,
                         handle);
#endif /* IP_ADAPTER */

#ifdef EDR_ADAPTER
    CAInitializeEDR(CARegisterCallback, CAReceivedPacketCallback, CANetworkChangedCallback,
                    handle);
#endif /* EDR_ADAPTER */

#ifdef LE_ADAPTER
    CAInitializeLE(CARegisterCallback, CAReceivedPacketCallback, CANetworkChangedCallback,
                   handle);
#endif /* LE_ADAPTER */

}

void CASetPacketReceivedCallback(CANetworkPacketReceivedCallback callback)
{
    OIC_LOG(DEBUG, TAG, "Set packet received callback");

    g_networkPacketReceivedCallback = callback;
}

void CASetNetworkChangeCallback(CANetworkChangeCallback callback)
{
    OIC_LOG(DEBUG, TAG, "Set network change callback");

    g_networkChangeCallback = callback;
}

CAResult_t CAStartAdapter(CATransportType_t transportType)
{
    OIC_LOG_V(DEBUG, TAG, "Start the adapter of CAConnectivityType[%d]", transportType);

    int index = CAGetAdapterIndex(transportType);

    if (index == -1)
    {
        OIC_LOG(ERROR, TAG, "unknown connectivity type!");
        return CA_STATUS_FAILED;
    }

    if (g_adapterHandler[index].startAdapter != NULL)
    {
        g_adapterHandler[index].startAdapter();
    }

    return CA_STATUS_OK;
}

void CAStopAdapter(CATransportType_t transportType)
{
    OIC_LOG_V(DEBUG, TAG, "Stop the adapter of CATransportType[%d]", transportType);

    int index = CAGetAdapterIndex(transportType);

    if (index == -1)
    {
        OIC_LOG(ERROR, TAG, "unknown transport type!");
        return;
    }

    if (g_adapterHandler[index].stopAdapter != NULL)
    {
        g_adapterHandler[index].stopAdapter();
    }
}

CAResult_t CAGetNetworkInfo(CALocalConnectivity_t **info, uint32_t *size)
{
    if (info == NULL || size == NULL)
    {
        return CA_STATUS_INVALID_PARAM;
    }

    CALocalConnectivity_t *tempInfo[CA_TRANSPORT_TYPE_NUM] = { 0 };
    uint32_t tempSize[CA_TRANSPORT_TYPE_NUM] = { 0 };

    CAResult_t res = CA_STATUS_FAILED;
    uint32_t resSize = 0;
    for (int index = 0; index < CA_TRANSPORT_TYPE_NUM; index++)
    {
        if (g_adapterHandler[index].GetnetInfo != NULL)
        {
            // #1. get information for each adapter
            res = g_adapterHandler[index].GetnetInfo(&tempInfo[index],
                                                     &tempSize[index]);

            // #2. total size
            if (res == CA_STATUS_OK)
            {
                resSize += tempSize[index];
            }

            OIC_LOG_V(DEBUG,
                      TAG,
                      "%d adapter network info size is %" PRIu32 " res:%d",
                      index,
                      tempSize[index],
                      res);
        }
    }

    OIC_LOG_V(DEBUG, TAG, "network info total size is %d!", resSize);

    if (resSize == 0)
    {
        if (res == CA_ADAPTER_NOT_ENABLED || res == CA_NOT_SUPPORTED)
        {
            return res;
        }
        return CA_STATUS_FAILED;
    }

    // #3. add data into result
    // memory allocation

    CALocalConnectivity_t * resInfo = OICCalloc(resSize, sizeof(*resInfo));
    CA_MEMORY_ALLOC_CHECK(resInfo);

    // #4. save data
    *info = resInfo;
    *size = resSize;

    for (int index = 0; index < CA_TRANSPORT_TYPE_NUM; index++)
    {
        // check information
        if (tempSize[index] == 0)
        {
            continue;
        }

        memcpy(resInfo,
               tempInfo[index],
               sizeof(*resInfo) * tempSize[index]);

        resInfo += tempSize[index];

        // free adapter data
        OICFree(tempInfo[index]);
        tempInfo[index] = NULL;
    }

    OIC_LOG(DEBUG, TAG, "each network info save success!");
    return CA_STATUS_OK;

    // memory error label.
memory_error_exit:

    for (int index = 0; index < CA_TRANSPORT_TYPE_NUM; index++)
    {

        OICFree(tempInfo[index]);
        tempInfo[index] = NULL;
    }

    return CA_MEMORY_ALLOC_FAILED;
}

CAResult_t CASendUnicastData(const CARemoteEndpoint_t *endpoint, const void *data, uint32_t length)
{
    OIC_LOG(DEBUG, TAG, "Send unicast data to enabled interface..");

    CAResult_t res = CA_STATUS_FAILED;

    if (endpoint == NULL)
    {
        OIC_LOG(DEBUG, TAG, "Invalid endpoint");
        return CA_STATUS_INVALID_PARAM;
    }

    CATransportType_t type = endpoint->transportType;

    int index = CAGetAdapterIndex(type);

    if (index == -1)
    {
        OIC_LOG(ERROR, TAG, "unknown transport type!");
        return CA_STATUS_INVALID_PARAM;
    }

    uint32_t sentDataLen = 0;

    if (g_adapterHandler[index].sendData != NULL)
    {
        sentDataLen = g_adapterHandler[index].sendData(endpoint, data, length);
    }

    if (sentDataLen != -1)
    {
        res = CA_STATUS_OK;
    }
    return res;
}

CAResult_t CASendMulticastData(const void *data, uint32_t length)
{
    OIC_LOG(DEBUG, TAG, "Send multicast data to enabled interface..");

    CAResult_t res = CA_STATUS_FAILED;
    u_arraylist_t *list = CAGetSelectedNetworkList();

    if (!list)
    {
        OIC_LOG(DEBUG, TAG, "No selected network");
        return CA_STATUS_FAILED;
    }

    int i = 0;
    for (i = 0; i < u_arraylist_length(list); i++)
    {
        void* ptrType = u_arraylist_get(list, i);

        if(ptrType == NULL)
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
            void *payload = (void *) OICMalloc(length);
            if (!payload)
            {
                OIC_LOG(ERROR, TAG, "Out of memory!");
                return CA_MEMORY_ALLOC_FAILED;
            }
            memcpy(payload, data, length);
            sentDataLen = g_adapterHandler[index].sendDataToAll(payload, length);
            OICFree(payload);
        }

        if (sentDataLen == length)
        {
           res = CA_STATUS_OK;
        }
        else
        {
            OIC_LOG(ERROR, TAG, "sendDataToAll failed!");
        }
    }

    return res;
}

CAResult_t CAStartListeningServerAdapters()
{
    OIC_LOG(DEBUG, TAG, "Start listening server from adapters..");

    u_arraylist_t *list = CAGetSelectedNetworkList();
    if (!list)
    {
        OIC_LOG(ERROR, TAG, "No selected network");
        return CA_STATUS_FAILED;
    }

    int i = 0;
    for (i = 0; i < u_arraylist_length(list); i++)
    {
        void* ptrType = u_arraylist_get(list, i);

        if(ptrType == NULL)
        {
            continue;
        }

        CATransportType_t connType = *(CATransportType_t *) ptrType;

        int index = CAGetAdapterIndex(connType);
        if (index == -1)
        {
            OIC_LOG(ERROR, TAG, "unknown connectivity type!");
            continue;
        }

        if (g_adapterHandler[index].startListenServer != NULL)
        {
            g_adapterHandler[index].startListenServer();
        }
    }

    return CA_STATUS_OK;
}

CAResult_t CAStartDiscoveryServerAdapters()
{
    OIC_LOG(DEBUG, TAG, "Start discovery server from adapters..");

    u_arraylist_t *list = CAGetSelectedNetworkList();

    if (!list)
    {
        OIC_LOG(ERROR, TAG, "No selected network");
        return CA_STATUS_FAILED;
    }

    int i = 0;
    for (i = 0; i < u_arraylist_length(list); i++)
    {
        void* ptrType = u_arraylist_get(list, i);

        if(ptrType == NULL)
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

    return CA_STATUS_OK;
}

void CATerminateAdapters()
{
    OIC_LOG(DEBUG, TAG, "terminate all adapters..");

    uint32_t index;
    for (index = 0; index < CA_TRANSPORT_TYPE_NUM; index++)
    {
        if (g_adapterHandler[index].terminate != NULL)
        {
            g_adapterHandler[index].terminate();
        }
    }
}

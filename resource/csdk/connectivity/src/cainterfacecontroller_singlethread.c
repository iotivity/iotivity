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

#include "cawifiadapter_singlethread.h"
#include "caethernetadapter_singlethread.h"
#include "caedradapter_singlethread.h"
#include "caleadapter_singlethread.h"

#include "canetworkconfigurator.h"
#include "oic_malloc.h"
#include "logger.h"

#define TAG "CAIFCNT_ST"

#define MEMORY_ALLOC_CHECK(arg) { if (arg == NULL) {OIC_LOG_V(DEBUG, TAG, "Out of memory");\
    goto memory_error_exit;} }

#define CA_CONNECTIVITY_TYPE_NUM   4

static CAConnectivityHandler_t gAdapterHandler[CA_CONNECTIVITY_TYPE_NUM];

static CANetworkPacketReceivedCallback gNetworkPacketReceivedCallback = NULL;

static CANetworkChangeCallback gNetworkChangeCallback = NULL;

static int8_t CAGetAdapterIndex(CAConnectivityType_t cType)
{
    switch (cType)
    {
        case CA_ETHERNET:
            return 0;
        case CA_WIFI:
            return 1;
        case CA_EDR:
            return 2;
        case CA_LE:
            return 3;
    }
    return -1;
}

static void CARegisterCallback(CAConnectivityHandler_t handler, CAConnectivityType_t cType)
{
    OIC_LOG(DEBUG, TAG, "IN");
    int8_t index = -1;

    index = CAGetAdapterIndex(cType);

    if (index == -1)
    {
        OIC_LOG(DEBUG, TAG, "unknown connectivity type!");
        return;
    }

    memcpy(&gAdapterHandler[index], &handler, sizeof(CAConnectivityHandler_t));

    OIC_LOG_V(DEBUG, TAG, "%d type adapter", cType);
    OIC_LOG(DEBUG, TAG, "OUT");
}

static void CAReceivedPacketCallback(CARemoteEndpoint_t *endpoint, void *data, 
    uint32_t dataLen)
{
    OIC_LOG(DEBUG, TAG, "IN");

    // Call the callback.
    if (gNetworkPacketReceivedCallback != NULL)
    {
        gNetworkPacketReceivedCallback(endpoint, data, dataLen);
    }
    OIC_LOG(DEBUG, TAG, "OUT");
}

static void CANetworkChangedCallback(CALocalConnectivity_t *info, CANetworkStatus_t status)
{
    OIC_LOG(DEBUG, TAG, "IN");

    // Call the callback.
    if (gNetworkChangeCallback != NULL)
    {
        gNetworkChangeCallback(info, status);
    }
    OIC_LOG(DEBUG, TAG, "OUT");
}

void CAInitializeAdapters()
{
    OIC_LOG(DEBUG, TAG, "IN");

    memset(gAdapterHandler, 0, sizeof(CAConnectivityHandler_t) * CA_CONNECTIVITY_TYPE_NUM);


    // Initialize adapters and register callback.
#ifdef ETHERNET_ADAPTER
    CAInitializeEthernet(CARegisterCallback, CAReceivedPacketCallback, CANetworkChangedCallback);
#endif /* ETHERNET_ADAPTER */

#ifdef WIFI_ADAPTER
    CAInitializeWifi(CARegisterCallback, CAReceivedPacketCallback, CANetworkChangedCallback);
#endif /* WIFI_ADAPTER */

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

    gNetworkPacketReceivedCallback = callback;
    OIC_LOG(DEBUG, TAG, "OUT");
}

void CASetNetworkChangeCallback(CANetworkChangeCallback callback)
{
    OIC_LOG(DEBUG, TAG, "IN");

    gNetworkChangeCallback = callback;
    OIC_LOG(DEBUG, TAG, "OUT");
}

void CAStartAdapter(CAConnectivityType_t cType)
{
    OIC_LOG_V(DEBUG, TAG, "cType[%d]", cType);

    int8_t index = -1;

    index = CAGetAdapterIndex(cType);

    if (index == -1)
    {
        OIC_LOG(DEBUG, TAG, "unknown connectivity type!");
        return;
    }

    if (gAdapterHandler[index].startAdapter != NULL)
    {
        gAdapterHandler[index].startAdapter();
    }
    OIC_LOG(DEBUG, TAG, "OUT");
}

void CAStopAdapter(CAConnectivityType_t cType)
{
    OIC_LOG_V(DEBUG, TAG, "cType[%d]", cType);

    int8_t index = -1;

    index = CAGetAdapterIndex(cType);

    if (index == -1)
    {
        OIC_LOG(DEBUG, TAG, "unknown connectivity type!");
        return;
    }

    if (gAdapterHandler[index].stopAdapter != NULL)
    {
        gAdapterHandler[index].stopAdapter();
    }
    OIC_LOG(DEBUG, TAG, "OUT");
}

CAResult_t CAGetNetworkInfo(CALocalConnectivity_t **info, uint32_t *size)
{
    OIC_LOG(DEBUG, TAG, "IN");
    CAResult_t res = CA_STATUS_FAILED;
    int8_t index = 0;
    int8_t i = 0;

    CALocalConnectivity_t *resInfo = NULL;
    uint32_t resSize = 0;

    CALocalConnectivity_t *tempInfo[CA_CONNECTIVITY_TYPE_NUM];
    uint32_t tempSize[CA_CONNECTIVITY_TYPE_NUM];

    memset(tempInfo, 0, sizeof(CALocalConnectivity_t *) * CA_CONNECTIVITY_TYPE_NUM);
    memset(tempSize, 0, sizeof(int8_t) * CA_CONNECTIVITY_TYPE_NUM);

    // #1. get information each adapter
    for (index = 0; index < CA_CONNECTIVITY_TYPE_NUM; index++)
    {
        if (gAdapterHandler[index].GetnetInfo != NULL)
        {
            res = gAdapterHandler[index].GetnetInfo(&tempInfo[index], &tempSize[index]);

            OIC_LOG_V (DEBUG, TAG, "%d adapter network info size is %d", index, tempSize[index]);
        }
    }

    resSize = 0;
    for (index = 0; index < CA_CONNECTIVITY_TYPE_NUM; index++)
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
        return res;
    }

    // #3. add data into result
    // memory allocation
    resInfo = (CALocalConnectivity_t *) OICMalloc(sizeof(CALocalConnectivity_t) * resSize);
    MEMORY_ALLOC_CHECK(resInfo);
    memset(resInfo, 0, sizeof(CALocalConnectivity_t) * resSize);

    i = 0;
    for (index = 0; index < CA_CONNECTIVITY_TYPE_NUM; index++)
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

    OIC_LOG_V(DEBUG, TAG, "OUT");

    return res;

    // memory error label.
memory_error_exit:

    return CA_MEMORY_ALLOC_FAILED;
}

CAResult_t CASendUnicastData(const CARemoteEndpoint_t *endpoint, void *data, uint32_t length)
{
    OIC_LOG(DEBUG, TAG, "IN");

    int8_t index = -1;
    uint32_t sentDataLen = 0;
    CAResult_t res = CA_STATUS_FAILED;

    if (endpoint == NULL)
    {
        OIC_LOG_V(DEBUG, TAG, "RemoteEndpoint is NULL");
        return CA_STATUS_INVALID_PARAM;
    }

    CAConnectivityType_t type = endpoint->connectivityType;

    index = CAGetAdapterIndex(type);

    if (index == -1)
    {
        OIC_LOG(DEBUG, TAG, "unknown connectivity type!");
        return CA_STATUS_INVALID_PARAM;
    }

    if (gAdapterHandler[index].sendData != NULL)
    {
        sentDataLen = gAdapterHandler[index].sendData(endpoint, data, length);
    }

    if (sentDataLen == length)
    {
        res = CA_STATUS_OK;
    }

    OIC_LOG(DEBUG, TAG, "OUT");
    return res;
}

CAResult_t CASendMulticastData(void *data, uint32_t length)
{
    OIC_LOG(DEBUG, TAG, "IN");

    uint8_t i = 0;
    CAConnectivityType_t connType;
    int8_t index = -1;
    uint32_t sentDataLen = 0;
    CAResult_t res = CA_STATUS_FAILED;
    u_arraylist_t *list = CAGetSelectedNetworkList();

    if (!list)
    {
        OIC_LOG(DEBUG, TAG, "No selected network");
        return CA_STATUS_FAILED;
    }

    void *ptrType = NULL;
    for (i = 0; i < u_arraylist_length(list); i++)
    {
        ptrType = u_arraylist_get(list, i);
        if (NULL == ptrType)
        {
            OIC_LOG(ERROR, TAG, "error");
            return CA_STATUS_FAILED;
        }
        connType = *(CAConnectivityType_t *) ptrType;

        index = CAGetAdapterIndex(connType);

        if (index == -1)
        {
            OIC_LOG(DEBUG, TAG, "unknown connectivity type!");
            continue;
        }

        if (gAdapterHandler[index].sendDataToAll != NULL)
        {
            sentDataLen = gAdapterHandler[index].sendDataToAll(data, length);
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

    uint8_t i = 0;
    CAConnectivityType_t connType;
    int8_t index = -1;
    u_arraylist_t *list = CAGetSelectedNetworkList();

    if (!list)
    {
        OIC_LOG(DEBUG, TAG, "No selected network");
        return CA_STATUS_FAILED;
    }

    void *ptrType = NULL;
    for (i = 0; i < u_arraylist_length(list); i++)
    {
        ptrType = u_arraylist_get(list, i);
        if (NULL == ptrType)
        {
            OIC_LOG(ERROR, TAG, "error");
            return CA_STATUS_FAILED;
        }
        connType = *(CAConnectivityType_t *) ptrType;

        index = CAGetAdapterIndex(connType);

        if (index == -1)
        {
            OIC_LOG(DEBUG, TAG, "unknown connectivity type!");
            continue;
        }

        if (gAdapterHandler[index].startListenServer != NULL)
        {
            gAdapterHandler[index].startListenServer();
        }
    }
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAStartDiscoveryServerAdapters()
{
    OIC_LOG(DEBUG, TAG, "IN");

    uint8_t i = 0;
    CAConnectivityType_t connType;
    int8_t index = -1;
    u_arraylist_t *list = CAGetSelectedNetworkList();

    if (!list)
    {
        OIC_LOG(DEBUG, TAG, "No selected network");
        return CA_STATUS_FAILED;
    }

    void *ptrType = NULL;
    for (i = 0; i < u_arraylist_length(list); i++)
    {
        ptrType = u_arraylist_get(list, i);
        if (NULL == ptrType)
        {
            OIC_LOG(ERROR, TAG, "error");
            return CA_STATUS_FAILED;
        }
        connType = *(CAConnectivityType_t *) ptrType;

        index = CAGetAdapterIndex(connType);

        if (index == -1)
        {
            OIC_LOG_V(DEBUG, TAG, "unknown connectivity type!");
            continue;
        }

        if (gAdapterHandler[index].startDiscoverServer != NULL)
        {
            gAdapterHandler[index].startDiscoverServer();
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
        if (gAdapterHandler[index].terminate != NULL)
        {
            gAdapterHandler[index].terminate();
        }
    }
    OIC_LOG(DEBUG, TAG, "OUT");
}

CAResult_t CAReadData()
{

    uint8_t i = 0;
    CAConnectivityType_t connType;
    int8_t index = -1;

    u_arraylist_t *list = CAGetSelectedNetworkList();

    if (!list)
    {
        return CA_STATUS_FAILED;
    }
    void *ptrType = NULL;
    for (i = 0; i < u_arraylist_length(list); i++)
    {
        ptrType = u_arraylist_get(list, i);
        if (NULL == ptrType)
        {
            OIC_LOG(ERROR, TAG, "error");
            return CA_STATUS_FAILED;
        }
        connType = *(CAConnectivityType_t *) ptrType;

        index = CAGetAdapterIndex(connType);

        if (-1 == index)
        {
            OIC_LOG_V(DEBUG, TAG, "unknown connectivity type!");
            continue;
        }

        if (gAdapterHandler[index].readData != NULL)
        {
            gAdapterHandler[index].readData();
        }
    }

    return CA_STATUS_OK;
}

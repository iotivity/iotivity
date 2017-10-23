/* ****************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
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

#include "iotivity_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>

#include "cainterface.h"
#include "caipnwmonitor.h"
#include "cawsinterface.h"
#include "cawsadapter.h"
#include "caqueueingthread.h"
#include "caadapterutils.h"
#include "octhread.h"
#include "uarraylist.h"
#include "caremotehandler.h"
#include "experimental/logger.h"
#include "oic_malloc.h"

#define TAG "OIC_CA_WS_ADAP"

static void *g_threadPool = NULL;
static CAQueueingThread_t *g_sendQueueHandle = NULL;
static CAQueueingThread_t *g_receiveQueueHandle = NULL;
static CANetworkPacketReceivedCallback g_networkPacketCallback = NULL;
static CAAdapterChangeCallback g_networkChangeCallback = NULL;
static CAConnectionChangeCallback g_connectionChangeCallback = NULL;
static CAErrorHandleCallback g_errorCallback = NULL;

typedef struct
{
    CASecureEndpoint_t sep;
    void *data;
    uint32_t dataLength;
} CAWSData_t;

static void CAFreeWSData(CAWSData_t *wsData)
{
    if (wsData)
    {
        if (wsData->data)
        {
            OICFree(wsData->data);
        }
        OICFree(wsData);
    }
}

static CAWSData_t *CACreateWSData(const CASecureEndpoint_t *sep,
                                  const void *data, uint32_t dataLength)
{
    VERIFY_NON_NULL_RET(sep, TAG, "endpoint is NULL", NULL);
    VERIFY_NON_NULL_RET(data, TAG, "WSData is NULL", NULL);

    CAWSData_t *wsData = (CAWSData_t *) OICMalloc(sizeof(CAWSData_t));
    if (!wsData)
    {
        OIC_LOG(ERROR, TAG, "Memory allocation failed!");
        return NULL;
    }

    wsData->sep = *sep;
    wsData->data = (void *) OICMalloc(dataLength);
    if (!wsData->data)
    {
        OIC_LOG(ERROR, TAG, "Memory allocation failed!");
        CAFreeWSData(wsData);
        return NULL;
    }

    memcpy(wsData->data, data, dataLength);
    wsData->dataLength = dataLength;

    return wsData;
}

static int32_t CAQueueWSData(CAQueueingThread_t *threadHandle,
                             const CASecureEndpoint_t *sep,
                             const void *data, uint32_t dataLength)
{
    OIC_LOG_V(DEBUG, TAG, "IN: %s", __func__);
    VERIFY_NON_NULL_RET(sep, TAG, "endpoint", -1);
    VERIFY_NON_NULL_RET(data, TAG, "data", -1);
    VERIFY_NON_NULL_RET(g_sendQueueHandle, TAG, "sendQueueHandle", -1);

    if (0 == dataLength)
    {
        OIC_LOG(ERROR, TAG, "Invalid Data Length");
        return -1;
    }

    CAWSData_t *wsData = CACreateWSData(sep, data, dataLength);
    if (!wsData)
    {
        OIC_LOG(ERROR, TAG, "Failed to create wsData");
        return -1;
    }
    CAQueueingThreadAddData(threadHandle, wsData, sizeof(CAWSData_t));
    return dataLength;
}

static void CAWSDataDestroyer(void *data, uint32_t size)
{
    if (size < sizeof(CAWSData_t))
    {
        OIC_LOG_V(ERROR, TAG, "Destroy data too small %p %d", data, size);
    }
    CAWSData_t *wsData = (CAWSData_t *) data;
    CAFreeWSData(wsData);
}

static void CAWSErrorHandler(const CAEndpoint_t *endpoint, const void *data,
                             size_t dataLength, CAResult_t result)
{
    VERIFY_NON_NULL_VOID(endpoint, TAG, "endpoint is NULL");
    VERIFY_NON_NULL_VOID(data, TAG, "data is NULL");

    if (g_errorCallback)
    {
        g_errorCallback(endpoint, data, dataLength, result);
    }
}

static void CAWSSendDataThread(void *threadData)
{
    OIC_LOG_V(DEBUG, TAG, "IN: %s", __func__);
    CAWSData_t *wsData = (CAWSData_t *) threadData;
    if (!wsData)
    {
        OIC_LOG(DEBUG, TAG, "Invalid ws data!");
        return;
    }

    int ret = CAWSSendData(&(wsData->sep.endpoint), wsData->data, wsData->dataLength);
    if (CA_STATUS_OK != ret)
    {
        CAWSErrorHandler(&(wsData->sep.endpoint), wsData->data, wsData->dataLength, ret);
    }
}

static void CAWSReceiveDataThread(void *threadData)
{
    OIC_LOG_V(DEBUG, TAG, "IN: %s", __func__);
    CAWSData_t *wsData = (CAWSData_t *) threadData;
    if (!wsData)
    {
        OIC_LOG(DEBUG, TAG, "Invalid ws data!");
        return;
    }
    if (g_networkPacketCallback)
    {
        g_networkPacketCallback(&(wsData->sep), wsData->data, wsData->dataLength);
    }
}

void CAWSPacketReceivedCallback(const CASecureEndpoint_t *sep, const void *data,
                                size_t dataLength)
{
    VERIFY_NON_NULL_VOID(sep, TAG, "sep is NULL");
    VERIFY_NON_NULL_VOID(data, TAG, "data is NULL");

    if (-1 == CAQueueWSData(g_receiveQueueHandle, sep, data, dataLength))
    {
        OIC_LOG(ERROR, TAG, "could not add to receive queue");
    }
}

static CAResult_t CAWSInitializeQueueHandles()
{
    if (g_sendQueueHandle)
    {
        OIC_LOG(DEBUG, TAG, "send queue handle is already initialized!");
        return CA_STATUS_OK;
    }

    // Create send message queue
    g_sendQueueHandle = OICMalloc(sizeof(CAQueueingThread_t));
    if (!g_sendQueueHandle)
    {
        OIC_LOG(ERROR, TAG, "Memory allocation failed! (g_sendQueueHandle)");
        return CA_MEMORY_ALLOC_FAILED;
    }

    if (CA_STATUS_OK != CAQueueingThreadInitialize(g_sendQueueHandle,
            (const ca_thread_pool_t)g_threadPool,
            CAWSSendDataThread, CAWSDataDestroyer))
    {
        OIC_LOG(ERROR, TAG, "Failed to Initialize send queue thread");
        OICFree(g_sendQueueHandle);
        g_sendQueueHandle = NULL;
        return CA_STATUS_FAILED;
    }

    // Create receive message queue
    g_receiveQueueHandle = OICMalloc(sizeof(CAQueueingThread_t));
    if (!g_receiveQueueHandle)
    {
        OIC_LOG(ERROR, TAG, "Memory allocation failed! (g_receiveQueueHandle)");
        CAQueueingThreadDestroy(g_sendQueueHandle);
        OICFree(g_sendQueueHandle);
        g_sendQueueHandle = NULL;
        return CA_MEMORY_ALLOC_FAILED;
    }

    if (CA_STATUS_OK != CAQueueingThreadInitialize(g_receiveQueueHandle,
            (const ca_thread_pool_t)g_threadPool,
            CAWSReceiveDataThread, CAWSDataDestroyer))
    {
        OIC_LOG(ERROR, TAG, "Failed to Initialize receive queue thread");
        CAQueueingThreadDestroy(g_sendQueueHandle);
        OICFree(g_sendQueueHandle);
        OICFree(g_receiveQueueHandle);
        g_sendQueueHandle = NULL;
        g_receiveQueueHandle = NULL;
        return CA_STATUS_FAILED;
    }

    return CA_STATUS_OK;
}

static void CAWSDeinitializeQueueHandles()
{
    if (g_sendQueueHandle)
    {
        CAQueueingThreadDestroy(g_sendQueueHandle);
        OICFree(g_sendQueueHandle);
        g_sendQueueHandle = NULL;
    }

    if (g_receiveQueueHandle)
    {
        CAQueueingThreadDestroy(g_receiveQueueHandle);
        OICFree(g_receiveQueueHandle);
        g_receiveQueueHandle = NULL;
    }
}

static void CAInitializeWSGlobals()
{
    caglobals.ws.u4 = caglobals.ports.ws.u4;
    caglobals.ws.u6 = caglobals.ports.ws.u6;
    caglobals.ws.u4s = caglobals.ports.ws.u4s;
    caglobals.ws.u6s = caglobals.ports.ws.u6s;

    CATransportFlags_t flags = 0;
    if (caglobals.client)
    {
        flags |= caglobals.clientFlags;
    }
    if (caglobals.server)
    {
        flags |= caglobals.serverFlags;
    }

    caglobals.ws.ipv4wsenabled = flags & CA_IPV4;
    caglobals.ws.ipv6wsenabled = flags & CA_IPV6;
}

CAResult_t CAInitializeWS(CARegisterConnectivityCallback registerCallback,
                          CANetworkPacketReceivedCallback networkPacketCallback,
                          CAAdapterChangeCallback netCallback,
                          CAConnectionChangeCallback connCallback,
                          CAErrorHandleCallback errorCallback, ca_thread_pool_t handle)
{
    OIC_LOG(DEBUG, TAG, "IN");
    VERIFY_NON_NULL(registerCallback, TAG, "registerCallback");
    VERIFY_NON_NULL(networkPacketCallback, TAG, "networkPacketCallback");
    VERIFY_NON_NULL(netCallback, TAG, "netCallback");
    VERIFY_NON_NULL(connCallback, TAG, "connCallback");
    VERIFY_NON_NULL(errorCallback, TAG, "errorCallback");
    VERIFY_NON_NULL(handle, TAG, "thread pool handle");

    g_networkChangeCallback = netCallback;
    g_connectionChangeCallback = connCallback;
    g_networkPacketCallback = networkPacketCallback;
    g_errorCallback = errorCallback;

    CAInitializeWSGlobals();
    CAWSSetPacketReceivedCallback(CAWSPacketReceivedCallback);
    CAWSSetConnectionChangeCallback(g_connectionChangeCallback);
    CAWSSetErrorHandleCallback(CAWSErrorHandler);

    g_threadPool = handle;

    CAConnectivityHandler_t wsHandler =
    {
        .startAdapter = CAStartWS,
        .startListenServer = CAStartWSListeningServer,
        .stopListenServer = CAStopWSListeningServer,
        .startDiscoveryServer = CAStartWSDiscoveryServer,
        .sendData = CASendWSUnicastData,
        .sendDataToAll = CASendWSMulticastData,
        .GetnetInfo = CAGetWSInterfaceInformation,
        .readData = CAReadWSData,
        .stopAdapter = CAStopWS,
        .terminate = CATerminateWS,
        .cType = CA_ADAPTER_WS
    };

    registerCallback(wsHandler);

    OIC_LOG(INFO, TAG, "OUT InitializeWS is Success");
    return CA_STATUS_OK;
}

CAResult_t CAStartWS()
{
    OIC_LOG_V(DEBUG, TAG, "IN: %s", __func__);

    if (CA_STATUS_OK != CAWSInitializeQueueHandles())
    {
        OIC_LOG(ERROR, TAG, "Failed to Initialize Queue Handle");
        return CA_STATUS_FAILED;
    }

    if (CA_STATUS_OK != CAQueueingThreadStart(g_sendQueueHandle))
    {
        OIC_LOG(ERROR, TAG, "Failed to Start Send Data Thread");
        CAWSDeinitializeQueueHandles();
        return CA_STATUS_FAILED;
    }

    if (CA_STATUS_OK != CAQueueingThreadStart(g_receiveQueueHandle))
    {
        OIC_LOG(ERROR, TAG, "Failed to Start Receive Data Thread");
        CAQueueingThreadStop(g_sendQueueHandle);
        CAWSDeinitializeQueueHandles();
        return CA_STATUS_FAILED;
    }

    CAResult_t ret = CAWSStart(g_threadPool);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(ERROR, TAG, "Failed to start ws");
        CAQueueingThreadStop(g_sendQueueHandle);
        CAQueueingThreadStop(g_receiveQueueHandle);
        CAWSDeinitializeQueueHandles();
        return ret;
    }

    return CA_STATUS_OK;
}

CAResult_t CAStartWSListeningServer()
{
    OIC_LOG_V(DEBUG, TAG, "IN: %s", __func__);
    return CA_STATUS_OK;
}

CAResult_t CAStopWSListeningServer()
{
    OIC_LOG_V(DEBUG, TAG, "IN: %s", __func__);
    return CA_STATUS_OK;
}

CAResult_t CAStartWSDiscoveryServer()
{
    OIC_LOG_V(DEBUG, TAG, "IN: %s", __func__);
    return CA_STATUS_OK;
}

int32_t CASendWSUnicastData(const CAEndpoint_t *endpoint,
                            const void *data, uint32_t dataLength,
                            CADataType_t dataType)
{
    OIC_LOG_V(DEBUG, TAG, "IN: %s", __func__);
    OC_UNUSED(dataType);

    OIC_LOG(DEBUG, TAG, "Need to send:");
    OIC_LOG_BUFFER(DEBUG, TAG, data, dataLength);

    CASecureEndpoint_t sep;
    sep.endpoint = *endpoint;
    return CAQueueWSData(g_sendQueueHandle, &sep, data, dataLength);
}

int32_t CASendWSMulticastData(const CAEndpoint_t* endpoint,
                              const void* data, uint32_t dataLength,
                              CADataType_t dataType)
{
    OIC_LOG_V(DEBUG, TAG, "IN: %s", __func__);
    OC_UNUSED(endpoint);
    OC_UNUSED(data);
    OC_UNUSED(dataLength);
    OC_UNUSED(dataType);

    return 0;
}

CAResult_t CAGetWSInterfaceInformation(CAEndpoint_t **info, size_t *size)
{
    OIC_LOG_V(DEBUG, TAG, "IN: %s", __func__);

    return CAWSGetInterfaceInformation(info, size);
}

CAResult_t CAReadWSData()
{
    OIC_LOG_V(DEBUG, TAG, "IN: %s", __func__);
    return CA_STATUS_OK;
}

CAResult_t CAStopWS()
{
    OIC_LOG_V(DEBUG, TAG, "IN: %s", __func__);

    if (g_sendQueueHandle && g_sendQueueHandle->threadMutex)
    {
        CAQueueingThreadStop(g_sendQueueHandle);
    }

    if (g_receiveQueueHandle && g_receiveQueueHandle->threadMutex)
    {
        CAQueueingThreadStop(g_receiveQueueHandle);
    }
    CAWSDeinitializeQueueHandles();

    CAWSStop();

    return CA_STATUS_OK;
}

void CATerminateWS()
{
    OIC_LOG_V(DEBUG, TAG, "IN: %s", __func__);
}

CANetworkPacketReceivedCallback CAGetWSNetworkPacketReceivedCallback()
{
    return g_networkPacketCallback;
}

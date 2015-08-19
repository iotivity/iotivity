/* ****************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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
#include <stdint.h>

#include "caciadapter.h"
#include "caciinterface.h"
#include "caqueueingthread.h"
#include "caadapterutils.h"
#include "camutex.h"
#include "uarraylist.h"
#include "caremotehandler.h"
#include "logger.h"
#include "oic_malloc.h"
#include "oic_string.h"

/**
 * Logging tag for module name.
 */
#define TAG "CI_ADAP"

/**
 * Holds internal thread CI data information.
 */
typedef struct
{
    CAEndpoint_t *remoteEndpoint;
    void *data;
    uint32_t dataLen;
    bool isMulticast;
} CACIData;

/**
 * Queue handle for Send Data.
 */
static CAQueueingThread_t *g_sendQueueHandle = NULL;

/**
 * Network Packet Received Callback to CA.
 */
static CANetworkPacketReceivedCallback g_networkPacketCallback = NULL;

/**
 * Network Changed Callback to CA.
 */
static CANetworkChangeCallback g_networkChangeCallback = NULL;

/**
 * error Callback to CA adapter.
 */
static CAErrorHandleCallback g_errorCallback = NULL;

static void CACIPacketReceivedCB(const CAEndpoint_t *endpoint,
                                 const void *data, uint32_t dataLength);

static CAResult_t CACIInitializeQueueHandles();

static void CACIDeinitializeQueueHandles();

static void CACISendDataThread(void *threadData);

static CACIData *CACreateCIData(const CAEndpoint_t *remoteEndpoint,
                                const void *data, uint32_t dataLength,
                                bool isMulticast);
void CAFreeCIData(CACIData *ipData);

static void CADataDestroyer(void *data, uint32_t size);

CAResult_t CACIInitializeQueueHandles()
{
    OIC_LOG(DEBUG, TAG, "IN");

    // Check if the message queue is already initialized
    if (g_sendQueueHandle)
    {
        OIC_LOG(DEBUG, TAG, "send queue handle is already initialized!");
        return CA_STATUS_OK;
    }

    // Create send message queue
    g_sendQueueHandle = OICMalloc(sizeof(CAQueueingThread_t));
    if (!g_sendQueueHandle)
    {
        OIC_LOG(ERROR, TAG, "Memory allocation failed!");
        return CA_MEMORY_ALLOC_FAILED;
    }

    if (CA_STATUS_OK != CAQueueingThreadInitialize(g_sendQueueHandle,
                                (const ca_thread_pool_t)caglobals.ci.threadpool,
                                CACISendDataThread, CADataDestroyer))
    {
        OIC_LOG(ERROR, TAG, "Failed to Initialize send queue thread");
        OICFree(g_sendQueueHandle);
        g_sendQueueHandle = NULL;
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

void CACIDeinitializeQueueHandles()
{
    OIC_LOG(DEBUG, TAG, "IN");

    CAQueueingThreadDestroy(g_sendQueueHandle);
    OICFree(g_sendQueueHandle);
    g_sendQueueHandle = NULL;

    OIC_LOG(DEBUG, TAG, "OUT");
}

void CACIConnectionStateCB(const char *ipAddress, CANetworkStatus_t status)
{
    (void)ipAddress;
    (void)status;
    OIC_LOG(DEBUG, TAG, "IN");
}

void CACIPacketReceivedCB(const CAEndpoint_t *endpoint, const void *data,
                          uint32_t dataLength)
{
    OIC_LOG(DEBUG, TAG, "IN");

    VERIFY_NON_NULL_VOID(endpoint, TAG, "ipAddress is NULL");
    VERIFY_NON_NULL_VOID(data, TAG, "data is NULL");

    OIC_LOG_V(DEBUG, TAG, "Address: %s, port:%d", endpoint->addr, endpoint->port);

    if (g_networkPacketCallback)
    {
        g_networkPacketCallback(endpoint, data, dataLength);
    }
    OIC_LOG(DEBUG, TAG, "OUT");
}

void CACIErrorHandler (const CAEndpoint_t *endpoint, const void *data,
                       uint32_t dataLength, CAResult_t result)
{
    OIC_LOG(DEBUG, TAG, "IN");

    VERIFY_NON_NULL_VOID(endpoint, TAG, "endpoint is NULL");

    VERIFY_NON_NULL_VOID(data, TAG, "data is NULL");

    void *buf = (void*)OICMalloc(sizeof(char) * dataLength);
    if (!buf)
    {
        OIC_LOG(ERROR, TAG, "Memory Allocation failed!");
        return;
    }
    memcpy(buf, data, dataLength);
    if (g_errorCallback)
    {
        g_errorCallback(endpoint, buf, dataLength, result);
    }
    else
    {
        OICFree(buf);
    }

    OIC_LOG(DEBUG, TAG, "OUT");
}

static void CAInitializeCIGlobals()
{
    caglobals.ci.selectTimeout = 5000;
    caglobals.ci.svrlist = NULL;

    CATransportFlags_t flags = 0;
    if (caglobals.client)
    {
        flags |= caglobals.clientFlags;
    }

    caglobals.ci.ipv4tcpenabled = flags & CA_IPV4_TCP;
}

CAResult_t CAInitializeCI(CARegisterConnectivityCallback registerCallback,
                          CANetworkPacketReceivedCallback networkPacketCallback,
                          CANetworkChangeCallback netCallback,
                          CAErrorHandleCallback errorCallback, ca_thread_pool_t handle)
{
    OIC_LOG(DEBUG, TAG, "IN");
    VERIFY_NON_NULL(registerCallback, TAG, "registerCallback");
    VERIFY_NON_NULL(networkPacketCallback, TAG, "networkPacketCallback");
    VERIFY_NON_NULL(netCallback, TAG, "netCallback");
    VERIFY_NON_NULL(handle, TAG, "thread pool handle");

    g_networkChangeCallback = netCallback;
    g_networkPacketCallback = networkPacketCallback;
    g_errorCallback = errorCallback;

    CAInitializeCIGlobals();
    caglobals.ci.threadpool = handle;

    CACISetPacketReceiveCallback(CACIPacketReceivedCB);
    CACISetErrorHandler(CACIErrorHandler);

    CAConnectivityHandler_t ciHandler;
    ciHandler.startAdapter = CAStartCI;
    ciHandler.startListenServer = CAStartCIListeningServer;
    ciHandler.startDiscoveryServer = CAStartCIDiscoveryServer;
    ciHandler.sendData = CASendCIUnicastData;
    ciHandler.sendDataToAll = CASendCIMulticastData;
    ciHandler.GetnetInfo = CAGetCIInterfaceInformation;
    ciHandler.readData = CAReadCIData;
    ciHandler.stopAdapter = CAStopCI;
    ciHandler.terminate = CATerminateCI;
    registerCallback(ciHandler, CA_ADAPTER_CLOUD_INTERFACE);

    OIC_LOG(INFO, TAG, "OUT IntializeCI is Success");
    return CA_STATUS_OK;
}

CAResult_t CAStartCI()
{
    OIC_LOG(DEBUG, TAG, "IN");

    if (CA_STATUS_OK != CACIInitializeQueueHandles())
    {
        OIC_LOG(ERROR, TAG, "Failed to Initialize Queue Handle");
        CATerminateCI();
        return CA_STATUS_FAILED;
    }

    // Start send queue thread
    if (CA_STATUS_OK != CAQueueingThreadStart(g_sendQueueHandle))
    {
        OIC_LOG(ERROR, TAG, "Failed to Start Send Data Thread");
        return CA_STATUS_FAILED;
    }

    CAResult_t ret = CACIStartServer((const ca_thread_pool_t)caglobals.ci.threadpool);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to start server![%d]", ret);
        return ret;
    }

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAStartCIListeningServer()
{
    OIC_LOG(DEBUG, TAG, "IN");

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAStartCIDiscoveryServer()
{
    OIC_LOG(DEBUG, TAG, "IN");

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

static int32_t CAQueueCIData(bool isMulticast, const CAEndpoint_t *endpoint,
                            const void *data, uint32_t dataLength)
{
    OIC_LOG(DEBUG, TAG, "IN");

    VERIFY_NON_NULL_RET(endpoint, TAG, "remoteEndpoint", -1);
    VERIFY_NON_NULL_RET(data, TAG, "data", -1);

    if (0 == dataLength)
    {
        OIC_LOG(ERROR, TAG, "Invalid Data Length");
        return -1;
    }

    VERIFY_NON_NULL_RET(g_sendQueueHandle, TAG, "sendQueueHandle", -1);

    // Create CIData to add to queue
    CACIData *ciData = CACreateCIData(endpoint, data, dataLength, isMulticast);
    if (!ciData)
    {
        OIC_LOG(ERROR, TAG, "Failed to create ipData!");
        return -1;
    }
    // Add message to send queue
    CAQueueingThreadAddData(g_sendQueueHandle, ciData, sizeof(CACIData));

    OIC_LOG(DEBUG, TAG, "OUT");
    return dataLength;
}

int32_t CASendCIUnicastData(const CAEndpoint_t *endpoint,
                            const void *data, uint32_t dataLength)
{
    OIC_LOG(DEBUG, TAG, "IN");
    return CAQueueCIData(false, endpoint, data, dataLength);
}

int32_t CASendCIMulticastData(const CAEndpoint_t *endpoint, const void *data, uint32_t dataLength)
{
    OIC_LOG(DEBUG, TAG, "IN");
    return CAQueueCIData(true, endpoint, data, dataLength);
}

CAResult_t CAReadCIData()
{
    OIC_LOG(DEBUG, TAG, "IN");
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAStopCI()
{
    OIC_LOG(DEBUG, TAG, "IN");

    if (g_sendQueueHandle && g_sendQueueHandle->threadMutex)
    {
        CAQueueingThreadStop(g_sendQueueHandle);
    }

    CACIDeinitializeQueueHandles();

    CACIStopServer();

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

void CATerminateCI()
{
    OIC_LOG(DEBUG, TAG, "IN");

    CACISetPacketReceiveCallback(NULL);

    CACIDeinitializeQueueHandles();

    OIC_LOG(DEBUG, TAG, "OUT");
}

void CACISendDataThread(void *threadData)
{
    OIC_LOG(DEBUG, TAG, "IN");

    CACIData *ciData = (CACIData *) threadData;
    if (!ciData)
    {
        OIC_LOG(DEBUG, TAG, "Invalid CI data!");
        return;
    }

    if (ciData->isMulticast)
    {
        return;
    }
    else
    {
        CACISendData(ciData->remoteEndpoint, ciData->data, ciData->dataLen, false);
    }

    OIC_LOG(DEBUG, TAG, "OUT");
}

CACIData *CACreateCIData(const CAEndpoint_t *remoteEndpoint, const void *data,
                         uint32_t dataLength, bool isMulticast)
{
    VERIFY_NON_NULL_RET(data, TAG, "CIData is NULL", NULL);

    CACIData *ciData = (CACIData *) OICMalloc(sizeof(CACIData));
    if (!ciData)
    {
        OIC_LOG(ERROR, TAG, "Memory allocation failed!");
        return NULL;
    }

    ciData->remoteEndpoint = CACloneEndpoint(remoteEndpoint);
    ciData->data = (void *) OICMalloc(dataLength);
    if (!ciData->data)
    {
        OIC_LOG(ERROR, TAG, "Memory allocation failed!");
        CAFreeCIData(ciData);
        return NULL;
    }

    memcpy(ciData->data, data, dataLength);
    ciData->dataLen = dataLength;

    ciData->isMulticast = isMulticast;

    return ciData;
}

void CAFreeCIData(CACIData *ciData)
{
    VERIFY_NON_NULL_VOID(ciData, TAG, "ciData is NULL");

    CAFreeEndpoint(ciData->remoteEndpoint);
    OICFree(ciData->data);
    OICFree(ciData);
}

void CADataDestroyer(void *data, uint32_t size)
{
    if (size < sizeof(CACIData))
    {
        OIC_LOG_V(ERROR, TAG, "Destroy data too small %p %d", data, size);
    }
    CACIData *ciData = (CACIData *) data;

    CAFreeCIData(ciData);
}

CAResult_t CACreateCIClient(const CACIServerInfo_t *ciServerInfo)
{
    VERIFY_NON_NULL(ciServerInfo, TAG, "CI server info is NULL");

    // #1. create CI server object
    CACIServerInfo_t *svritem = (CACIServerInfo_t *) OICMalloc(sizeof (CACIServerInfo_t));
    if (!svritem)
    {
        OIC_LOG(ERROR, TAG, "Out of memory");
        return CA_MEMORY_ALLOC_FAILED;
    }
    memcpy(svritem, ciServerInfo, sizeof (CACIServerInfo_t));

    // #2. create socket and connect to CI server
    CAResult_t res = CAConnectToCIServer(svritem);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "Failed to start CI Client");
        OICFree(svritem);
        return res;
    }

    return CA_STATUS_OK;
}

CAResult_t CADestroyCIClient(const CACIServerInfo_t *ciServerInfo)
{
    VERIFY_NON_NULL(ciServerInfo, TAG, "CI server info is NULL");

    CAResult_t res = CADisconnectFromCIServer(ciServerInfo);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "Failed to stop CI Client");
        return res;
    }

    return CA_STATUS_OK;
}

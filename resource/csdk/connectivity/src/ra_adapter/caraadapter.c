/******************************************************************
//
// Copyright 2015 Intel Mobile Communications GmbH All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
******************************************************************/

#include "caraadapter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "carainterface.h"
#include "caqueueingthread.h"
#include "caadapterutils.h"
#include "camutex.h"
#include "uarraylist.h"
#include "logger.h"
#include "oic_malloc.h"

/**
 * @def RA_ADAPTER_TAG
 * @brief Logging tag for module name
 */
#define RA_ADAPTER_TAG "RA_ADAP"

/**
 * @var CARAData
 * @brief Holds RA data information.
 */
typedef struct
{
    CARemoteEndpoint_t *remoteEndpoint;
    void *data;
    uint32_t dataLen;
} CARAData;

/**
 * @var g_networkPacketCallback
 * @brief Network Packet Received Callback to CA
 */
static CANetworkPacketReceivedCallback g_networkPacketCallback = NULL;

/**
 * @var g_networkChangeCallback
 * @brief Network Changed Callback to CA
 */
static CANetworkChangeCallback g_networkChangeCallback = NULL;

/**
 * @var g_sendQueueHandle
 * @brief Queue handle for Send Data
 */
static CAQueueingThread_t *g_sendQueueHandle = NULL;

/**
 * @var g_threadPool
 * @brief ThreadPool for storing ca_thread_pool_t handle passed from CA
 */
static ca_thread_pool_t g_threadPool = NULL;

static CAResult_t CARAInitializeQueueHandles();

static void CARADeinitializeQueueHandles();

static void CARANotifyNetworkChange(const char *address, CANetworkStatus_t status);

static void CARAPacketReceivedCB(const char *ipAddress, uint16_t port, const void *data,
                                       uint32_t dataLength, bool isSecured);
static CAResult_t CARAStopServers();

static void CARASendDataThread(void *threadData);

static CARAData *CACreateRAData(const CARemoteEndpoint_t *remoteEndpoint,
                                            const void *data, uint32_t dataLength);
static void CAFreeRAData(CARAData *raData);

static void CADataDestroyer(void *data, uint32_t size);

static CAResult_t CARADummy();

CAResult_t CARADummy()
{
    return CA_STATUS_OK;
}

CAResult_t CARAInitializeQueueHandles()
{
    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CARAInitializeQueueHandles IN");

    // Check if the message queue is already initialized
    if (g_sendQueueHandle)
    {
        OIC_LOG(DEBUG, RA_ADAPTER_TAG, "send queue handle is already initialized!");
        return CA_STATUS_OK;
    }

    // Create send message queue
    g_sendQueueHandle = OICMalloc(sizeof(CAQueueingThread_t));
    if (!g_sendQueueHandle)
    {
        OIC_LOG(ERROR, RA_ADAPTER_TAG, "Memory allocation failed!");
        return CA_MEMORY_ALLOC_FAILED;
    }

    if (CA_STATUS_OK != CAQueueingThreadInitialize(g_sendQueueHandle, g_threadPool,
                                                   CARASendDataThread, CADataDestroyer))
    {
        OIC_LOG(ERROR, RA_ADAPTER_TAG, "Failed to Initialize send queue thread");
        OICFree(g_sendQueueHandle);
        g_sendQueueHandle = NULL;
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CARAInitializeQueueHandles OUT");
    return CA_STATUS_OK;
}

void CARADeinitializeQueueHandles()
{
    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CARADeinitializeQueueHandles IN");

    CAQueueingThreadDestroy(g_sendQueueHandle);
    OICFree(g_sendQueueHandle);
    g_sendQueueHandle = NULL;

    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CARADeinitializeQueueHandles OUT");
}

void CARANotifyNetworkChange(const char *address, CANetworkStatus_t status)
{
    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CARANotifyNetworkChange IN");

    CALocalConnectivity_t *localEndpoint = CAAdapterCreateLocalEndpoint(CA_RA, address);
    if (!localEndpoint)
    {
        OIC_LOG(ERROR, RA_ADAPTER_TAG, "localEndpoint creation failed!");
        return;
    }

    if (g_networkChangeCallback)
    {
        g_networkChangeCallback(localEndpoint, status);
    }
    else
    {
        OIC_LOG(ERROR, RA_ADAPTER_TAG, "g_networkChangeCallback is NULL");
    }

    CAAdapterFreeLocalEndpoint(localEndpoint);

    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CARANotifyNetworkChange OUT");
}

void CARAPacketReceivedCB(const char *ipAddress, uint16_t port, const void *data,
                                uint32_t dataLength, bool isSecured)
{
    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CARAPacketReceivedCB IN");

    //TODO: logic needs to be added

    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CARAPacketReceivedCB OUT");
}

CAResult_t CAInitializeRA(CARegisterConnectivityCallback registerCallback,
                                CANetworkPacketReceivedCallback networkPacketCallback,
                                CANetworkChangeCallback netCallback, ca_thread_pool_t handle)
{
    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CAInitializeRA IN");
    VERIFY_NON_NULL(registerCallback, RA_ADAPTER_TAG, "registerCallback");
    VERIFY_NON_NULL(networkPacketCallback, RA_ADAPTER_TAG, "networkPacketCallback");
    VERIFY_NON_NULL(netCallback, RA_ADAPTER_TAG, "netCallback");
    VERIFY_NON_NULL(handle, RA_ADAPTER_TAG, "thread pool handle");

    g_threadPool = handle;
    g_networkChangeCallback = netCallback;
    g_networkPacketCallback = networkPacketCallback;

    CAResult_t ret = CARAInitializeServer(g_threadPool);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG_V(ERROR, RA_ADAPTER_TAG, "Failed to initialize server![%d]", ret);
        CATerminateRA();
        return ret;
    }

    CARASetPacketReceiveCallback(CARAPacketReceivedCB);

    CAConnectivityHandler_t raHandler = {};
    raHandler.startAdapter = CAStartRA;
    raHandler.startListenServer = CARADummy;
    raHandler.startDiscoveryServer = CARADummy;
    raHandler.sendData = CASendRAUnicastData;
    raHandler.sendDataToAll = CARADummy;
    raHandler.GetnetInfo = CARADummy;
    raHandler.readData = CARADummy;
    raHandler.stopAdapter = CAStopRA;
    raHandler.terminate = CATerminateRA;
    registerCallback(raHandler, CA_RA);

    if (CA_STATUS_OK != CARAInitializeQueueHandles())
    {
        OIC_LOG(ERROR, RA_ADAPTER_TAG, "Failed to Initialize Queue Handle");
        CATerminateRA();
        return CA_STATUS_FAILED;
    }

    OIC_LOG(INFO, RA_ADAPTER_TAG, "CAInitializeRA OUT");
    return CA_STATUS_OK;
}

CAResult_t CAStartRA()
{
    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CAStartRA IN");

    //TODO: logic needs to be added

    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CAStartRA OUT");
    return CA_STATUS_OK;
}

int32_t CASendRAUnicastData(const CARemoteEndpoint_t *remoteEndpoint, const void *data,
                                  uint32_t dataLength)
{
    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CASendRAUnicastData IN");

    //TODO: logic needs to be added

    return 0;
}

CAResult_t CARAStopServers()
{
    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CARAStopServers IN");

    // Stop all unicast and multicast servers.
    if (CA_STATUS_OK == CARAStopAllServers())
    {
        OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CARAStopAllServers success");
    }

    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CARAStopServers OUT");
    return CA_STATUS_OK;
}

CAResult_t CAStopRA()
{
    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CAStopRA IN");

    // Stop RA network monitor
    CARAStopNetworkMonitor();

    // Stop send queue thread
    if (g_sendQueueHandle)
    {
        CAQueueingThreadStop(g_sendQueueHandle);
    }

    // Stop Unicast, Secured unicast and Multicast servers running
    CARAStopServers();

    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CAStopRA OUT");
    return CA_STATUS_OK;
}

void CATerminateRA()
{
    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CATerminateRA IN");

    CARASetPacketReceiveCallback(NULL);

    CAStopRA();

    CARATerminateServer();

    CARADeinitializeQueueHandles();

    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CATerminateRA OUT");
}

void CARASendDataThread(void *threadData)
{
    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CARASendDataThread IN");

    //TODO: add in the logic

    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CARASendDataThread OUT");
}

CARAData *CACreateRAData(const CARemoteEndpoint_t *remoteEndpoint, const void *data,
                                     uint32_t dataLength)
{
    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CACreateRAData IN");

    VERIFY_NON_NULL_RET(data, RA_ADAPTER_TAG, "RAData is NULL", NULL);

    CARAData *raData = (CARAData *) OICMalloc(sizeof(CARAData));
    if (!raData)
    {
        OIC_LOG(ERROR, RA_ADAPTER_TAG, "Memory allocation failed!");
        return NULL;
    }

    raData->remoteEndpoint = CAAdapterCopyRemoteEndpoint(remoteEndpoint);
    raData->data = (void *) OICMalloc(dataLength);
    if (!raData->data)
    {
        OIC_LOG(ERROR, RA_ADAPTER_TAG, "Memory allocation failed!");
        CAFreeRAData(raData);
        return NULL;
    }

    memcpy(raData->data, data, dataLength);
    raData->dataLen = dataLength;

    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CACreateRAData OUT");
    return raData;
}

void CAFreeRAData(CARAData *raData)
{
    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CAFreeRAData IN");
    VERIFY_NON_NULL_VOID(raData, RA_ADAPTER_TAG, "raData is NULL");

    CAAdapterFreeRemoteEndpoint(raData->remoteEndpoint);
    OICFree(raData->data);
    OICFree(raData);

    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CAFreeRAData OUT");
}

void CADataDestroyer(void *data, uint32_t size)
{
    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CADataDestroyer IN");
    CARAData *etdata = (CARAData *) data;

    CAFreeRAData(etdata);
    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CADataDestroyer OUT");
}


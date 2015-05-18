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

#include "carainterface.h"

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>

#include "pdu.h"
#include "caadapterutils.h"
#include "camutex.h"
#include "oic_malloc.h"

/**
 * @def RA_SERVER_TAG
 * @brief Logging tag for module name
 */
#define RA_SERVER_TAG "RA_SERVER"

/**
 * @var g_packetHandlerStopFlag
 * @brief Flag for stopping packet handler thread.
 */
static bool g_packetHandlerStopFlag = false;

/**
 * @var CAAdapterRAServerContext_tf
 * @brief Thread context information for callbacks and threadpool.
 */
typedef struct
{
    ca_thread_pool_t threadPool;
    CARAPacketReceivedCallback packetReceivedCallback;
    CARAExceptionCallback exceptionCallback;
} CAAdapterRAServerContext_t;

/**
 * @var g_adapterServerContext
 * @brief Global RA adapter context.
 */
static CAAdapterRAServerContext_t *g_adapterServerContext = NULL;

/**
 * @var g_mutexAdapterServerContext
 * @brief Mutex to synchronize adapter context
 */
static ca_mutex g_mutexAdapterServerContext = NULL;

/**
 * @var g_serverInfoList
 * @brief Array to hold started server information.
 */
static u_arraylist_t *g_serverInfoList = NULL;

/**
 * @var g_mutexServerInfoList
 * @brief Mutex to synchronize Server Information.
 */
static ca_mutex g_mutexServerInfoList = NULL;

static void CAReceiveHandler(void *data)
{
    OIC_LOG(DEBUG, RA_SERVER_TAG, "IN");

    while (true != g_packetHandlerStopFlag)
    {
        // TODO: implement RA receive functionality

        ca_mutex_lock(g_mutexAdapterServerContext);
        if (g_adapterServerContext->packetReceivedCallback)
        {
            //g_adapterServerContext->packetReceivedCallback(srcIPAddress, srcPort, recvBuffer, recvLen, false);
        }
        ca_mutex_unlock(g_mutexAdapterServerContext);
    }

    OIC_LOG(DEBUG, RA_SERVER_TAG, "OUT");
}

static CAResult_t CAStartUnicastServer(const char *localAddress, uint16_t *port,
                                       bool forceBindStart, bool isSecured, int *serverFD)
{
    OIC_LOG(DEBUG, RA_SERVER_TAG, "CAStartUnicastServer IN");

    // TODO: implement

    OIC_LOG(DEBUG, RA_SERVER_TAG, "CAStartUnicastServer OUT");
    return 0;
}

static CAResult_t CARAStartPacketReceiverHandler()
{
    OIC_LOG(DEBUG, RA_SERVER_TAG, "CARAStartPacketReceiverHandler IN");

    ca_mutex_lock(g_mutexServerInfoList);

    uint32_t listLength = u_arraylist_length(g_serverInfoList);

    ca_mutex_unlock(g_mutexServerInfoList);

    ca_mutex_lock(g_mutexAdapterServerContext);

    if (!g_adapterServerContext)
    {
        OIC_LOG(ERROR, RA_SERVER_TAG, "g_adapterEthServerContext NULL");
        ca_mutex_unlock(g_mutexAdapterServerContext);
        return CA_STATUS_FAILED;
    }

    if (1 == listLength) //Its first time.
    {
        if (CA_STATUS_OK != ca_thread_pool_add_task(g_adapterServerContext->threadPool,
                                                   CAReceiveHandler, NULL ))
        {
            OIC_LOG(ERROR, RA_SERVER_TAG, "thread_pool_add_task failed!");
            ca_mutex_unlock(g_mutexAdapterServerContext);
            return CA_STATUS_FAILED;
        }
        OIC_LOG(DEBUG, RA_SERVER_TAG, "CAReceiveHandler thread started successfully.");
    }
    else
    {
        OIC_LOG(DEBUG, RA_SERVER_TAG, "CAReceiveHandler thread already is running");
    }
    ca_mutex_unlock(g_mutexAdapterServerContext);

    OIC_LOG(DEBUG, RA_SERVER_TAG, "OUT");

    return CA_STATUS_OK;
}

static void CARAServerDestroyMutex(void)
{
    OIC_LOG(DEBUG, RA_SERVER_TAG, "CARAServerDestroyMutex IN");

    if (g_mutexServerInfoList)
    {
        ca_mutex_free(g_mutexServerInfoList);
        g_mutexServerInfoList = NULL;
    }

    if (g_mutexAdapterServerContext)
    {
        ca_mutex_free(g_mutexAdapterServerContext);
        g_mutexAdapterServerContext = NULL;
    }

    OIC_LOG(DEBUG, RA_SERVER_TAG, "CARAServerDestroyMutex OUT");
}

static CAResult_t CARAServerCreateMutex(void)
{
    OIC_LOG(DEBUG, RA_SERVER_TAG, "CARAServerCreateMutex IN");

    g_mutexServerInfoList = ca_mutex_new();
    if (!g_mutexServerInfoList)
    {
        OIC_LOG(DEBUG, RA_SERVER_TAG, "OUT");
        return CA_MEMORY_ALLOC_FAILED;
    }

    g_mutexAdapterServerContext = ca_mutex_new();
    if (!g_mutexAdapterServerContext)
    {
        OIC_LOG(ERROR, RA_SERVER_TAG, "Failed to created mutex!");
        ca_mutex_free(g_mutexServerInfoList);
        g_mutexServerInfoList = NULL;
        return CA_MEMORY_ALLOC_FAILED;
    }

    OIC_LOG(DEBUG, RA_SERVER_TAG, "CARAServerCreateMutex OUT");
    return CA_STATUS_OK;
}

CAResult_t CARAInitializeServer(const ca_thread_pool_t threadPool)
{
    OIC_LOG(DEBUG, RA_SERVER_TAG, "CARAInitializeServer IN");

    // Input validation
    VERIFY_NON_NULL(threadPool, RA_SERVER_TAG, "Thread pool handle is NULL");

    // Initialize mutex
    if (CA_STATUS_OK != CARAServerCreateMutex())
    {
        OIC_LOG(ERROR, RA_SERVER_TAG, "Failed to create mutex!");
        return CA_STATUS_FAILED;
    }

    ca_mutex_lock(g_mutexAdapterServerContext);
    g_adapterServerContext = (CAAdapterRAServerContext_t *) OICCalloc(1,
                                 sizeof(CAAdapterRAServerContext_t));

    if (!g_adapterServerContext)
    {
        OIC_LOG(ERROR, RA_SERVER_TAG, "Malloc failed");
        ca_mutex_unlock(g_mutexAdapterServerContext);
        return CA_MEMORY_ALLOC_FAILED;
    }

    g_adapterServerContext->threadPool = threadPool;

    ca_mutex_unlock(g_mutexAdapterServerContext);

    ca_mutex_lock(g_mutexServerInfoList);

    g_serverInfoList = u_arraylist_create();
    if (!g_serverInfoList)
    {
        OIC_LOG(ERROR, RA_SERVER_TAG, "u_arraylist_create failed");
        ca_mutex_unlock(g_mutexServerInfoList);
        return CA_MEMORY_ALLOC_FAILED;
    }
    ca_mutex_unlock(g_mutexServerInfoList);
    OIC_LOG(DEBUG, RA_SERVER_TAG, "CARAInitializeServer OUT");
    return CA_STATUS_OK;
}

void CARATerminateServer()
{
    OIC_LOG(DEBUG, RA_SERVER_TAG, "CARATerminateServer IN");
    ca_mutex_lock(g_mutexAdapterServerContext);
    if (!g_adapterServerContext)
    {
        OIC_LOG(ERROR, RA_SERVER_TAG, "g_adapterServerContext NULL");
        ca_mutex_unlock(g_mutexAdapterServerContext);
        return;
    }

    OICFree(g_adapterServerContext);
    g_adapterServerContext = NULL;

    ca_mutex_unlock(g_mutexAdapterServerContext);

    ca_mutex_lock(g_mutexServerInfoList);

    CAClearServerInfoList(g_serverInfoList);
    g_serverInfoList = NULL;

    ca_mutex_unlock(g_mutexServerInfoList);
    // Destroy mutex
    CARAServerDestroyMutex();

    OIC_LOG(DEBUG, RA_SERVER_TAG, "CARATerminateServer OUT");

}

CAResult_t CARAStartUnicastServer(const char *localAddress, uint16_t *port,
                                        bool forceBindStart, bool isSecured)
{
    OIC_LOG(DEBUG, RA_SERVER_TAG, "CARAStartUnicastServer IN");

    // TODO: implement if needed

    OIC_LOG(DEBUG, RA_SERVER_TAG, "CARAStartUnicastServer OUT");
    return CA_STATUS_OK;
}

CAResult_t CARAStartMulticastServer(const char *localAddress, const char *multicastAddress,
                                          uint16_t multicastPort)
{
    OIC_LOG(DEBUG, RA_SERVER_TAG, "CARAStartMulticastServer IN");

    // TODO: implement if needed

    OIC_LOG(DEBUG, RA_SERVER_TAG, "CARAStartMulticastServer OUT");
    return CA_STATUS_OK;
}

CAResult_t CARAStopServer(const char *interfaceAddress)
{
    OIC_LOG(DEBUG, RA_SERVER_TAG, "CARAStopServer IN");

    VERIFY_NON_NULL(interfaceAddress, RA_SERVER_TAG, "interfaceAddress is NULL");

    ca_mutex_lock(g_mutexServerInfoList);

    // TODO: implement

    OIC_LOG(DEBUG, RA_SERVER_TAG, "CARAStopServer OUT");
    ca_mutex_unlock(g_mutexServerInfoList);
    return CA_STATUS_OK;
}

CAResult_t CARAStopAllServers()
{
    OIC_LOG(DEBUG, RA_SERVER_TAG, "CARAStopAllServers IN");

    g_packetHandlerStopFlag = true;

    ca_mutex_lock(g_mutexServerInfoList);

    // TODO: implement

    ca_mutex_unlock(g_mutexServerInfoList);

    OIC_LOG(DEBUG, RA_SERVER_TAG, "CARAStopAllServers OUT");
    return CA_STATUS_OK;
}

CAResult_t CAGetRAServerInfoList(u_arraylist_t **serverInfoList)
{
    OIC_LOG(DEBUG, RA_SERVER_TAG, "CAGetRAServerInfoList IN");
    ca_mutex_lock(g_mutexServerInfoList);

    uint32_t list_index = 0;
    uint32_t list_length = u_arraylist_length(g_serverInfoList);
    for (list_index = 0; list_index < list_length; list_index++)
    {
        CAServerInfo_t *info = (CAServerInfo_t *) u_arraylist_get(g_serverInfoList, list_index);
        if (!info)
        {
            continue;
        }

        CAServerInfo_t *newNetinfo = (CAServerInfo_t *) OICMalloc(sizeof(CAServerInfo_t));
        if (!newNetinfo)
        {
            OIC_LOG(ERROR, RA_SERVER_TAG, "Malloc failed!");
            ca_mutex_unlock(g_mutexServerInfoList);
            return CA_MEMORY_ALLOC_FAILED;
        }

        memcpy(newNetinfo, info, sizeof(*info));

        CAResult_t result = u_arraylist_add(*serverInfoList, (void *) newNetinfo);
        if (CA_STATUS_OK != result)
        {
            OIC_LOG(ERROR, RA_SERVER_TAG, "u_arraylist_add failed!");
            ca_mutex_unlock(g_mutexServerInfoList);
            return CA_STATUS_FAILED;
        }
    }
    ca_mutex_unlock(g_mutexServerInfoList);
    OIC_LOG(DEBUG, RA_SERVER_TAG, "CAGetRAServerInfoList OUT");
    return CA_STATUS_OK;
}

void CARASetPacketReceiveCallback(CARAPacketReceivedCallback callback)
{
    OIC_LOG(DEBUG, RA_SERVER_TAG, "CARASetPacketReceiveCallback IN");

    ca_mutex_lock(g_mutexAdapterServerContext);

    if (!g_adapterServerContext)
    {
        OIC_LOG(ERROR, RA_SERVER_TAG, "g_adapterEthServerContext NULL");
        ca_mutex_unlock(g_mutexAdapterServerContext);
        return;
    }
    g_adapterServerContext->packetReceivedCallback = callback;

    ca_mutex_unlock(g_mutexAdapterServerContext);

    OIC_LOG(DEBUG, RA_SERVER_TAG, "CARASetPacketReceiveCallback OUT");
}

void CARASetExceptionCallback(CARAExceptionCallback callback)
{
    OIC_LOG(DEBUG, RA_SERVER_TAG, "CARASetExceptionCallback IN");
    ca_mutex_lock(g_mutexAdapterServerContext);

    if (!g_adapterServerContext)
    {
        OIC_LOG(ERROR, RA_SERVER_TAG, "g_adapterEthServerContext NULL");
        ca_mutex_unlock(g_mutexAdapterServerContext);
        return;
    }
    g_adapterServerContext->exceptionCallback = callback;

    ca_mutex_unlock(g_mutexAdapterServerContext);

    OIC_LOG(DEBUG, RA_SERVER_TAG, "CARASetExceptionCallback OUT");
}


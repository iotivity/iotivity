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
#include <ifaddrs.h>
#include <net/if.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "caadapterutils.h"
#include "camutex.h"
#include "logger.h"
#include "oic_malloc.h"
#include "oic_string.h"

#define RA_MONITOR_TAG "RA_MONITOR"

/**
 * @struct CARANetworkMonitorContext
 * @brief  Used for storing network monitor context information.
 */
typedef struct
{
    ca_thread_pool_t threadPool;
    CANetworkStatus_t nwConnectivityStatus;
    CARAConnectionStateChangeCallback networkChangeCb;
} CARANetworkMonitorContext;

/**
 * @var g_networkMonitorContext
 * @brief  network monitor context.
 */
static CARANetworkMonitorContext *g_networkMonitorContext = NULL;

/**
 * @var g_networkMonitorContextMutex
 * @brief  Mutex for synchronizing access to cached interface and IP address information.
 */
static ca_mutex g_networkMonitorContextMutex = NULL;

/**
 * @var g_stopNetworkMonitor
 * @brief  Used to stop the network monitor thread.
 */
static bool g_stopNetworkMonitor = false;

/**
 * @var g_stopNetworkMonitorMutex
 * @brief  Mutex for synchronizing access to g_stopNetworkMonitor flag.
 */
static ca_mutex g_stopNetworkMonitorMutex = NULL;

/**
 * @var g_isRAConnected
 * @brief RA connection status to the XMPP server
 */
static bool g_isRAConnected = false;

// Main network listening thread
static void CANetworkMonitorThread(void *threadData)
{
    OIC_LOG(DEBUG, RA_MONITOR_TAG, "CANetworkMonitorThread IN");

    while (!g_stopNetworkMonitor)
    {
        ca_mutex_lock(g_networkMonitorContextMutex);

        // TODO: listen for XMPP callback on network status events
        if (g_networkMonitorContext->networkChangeCb)
        {
            g_networkMonitorContext->networkChangeCb(
                    g_isRAConnected ? CA_INTERFACE_UP : CA_INTERFACE_DOWN);
        }

        ca_mutex_unlock(g_networkMonitorContextMutex);

        sleep(2); // To avoid maximum cpu usage.
    }

    OIC_LOG(DEBUG, RA_MONITOR_TAG, "CANetworkMonitorThread OUT");
}

static CAResult_t CAInitializeNetworkMonitorMutexes()
{
    OIC_LOG(DEBUG, RA_MONITOR_TAG, "CAInitializeNetworkMonitorMutexes IN");

    if (!g_networkMonitorContextMutex)
    {
        g_networkMonitorContextMutex = ca_mutex_new();
        if (!g_networkMonitorContextMutex)
        {
            OIC_LOG(ERROR, RA_MONITOR_TAG, "g_networkMonitorContextMutex Malloc  failed");
            return CA_MEMORY_ALLOC_FAILED;
        }
    }

    if (!g_stopNetworkMonitorMutex)
    {
        g_stopNetworkMonitorMutex = ca_mutex_new();
        if (!g_stopNetworkMonitorMutex)
        {
            OIC_LOG(ERROR, RA_MONITOR_TAG, "g_stopNetworkMonitorMutex Malloc  failed");
            ca_mutex_free(g_networkMonitorContextMutex);
            return CA_MEMORY_ALLOC_FAILED;
        }
    }
    OIC_LOG(DEBUG, RA_MONITOR_TAG, "CAInitializeNetworkMonitorMutexes OUT");

    return CA_STATUS_OK;
}

static void CADestroyNetworkMonitorMutexes()
{
    OIC_LOG(DEBUG, RA_MONITOR_TAG, "CADestroyNetworkMonitorMutexes IN");
    ca_mutex_free(g_networkMonitorContextMutex);
    g_networkMonitorContextMutex = NULL;

    ca_mutex_free(g_stopNetworkMonitorMutex);
    g_stopNetworkMonitorMutex = NULL;
    OIC_LOG(DEBUG, RA_MONITOR_TAG, "CADestroyNetworkMonitorMutexes OUT");
}

CAResult_t CARAInitializeNetworkMonitor(const ca_thread_pool_t threadPool,
                                        CARAConnectionStateChangeCallback callback)
{
    OIC_LOG(DEBUG, RA_MONITOR_TAG, "CARAInitializeNetworkMonitor IN");

    VERIFY_NON_NULL(threadPool, RA_MONITOR_TAG, "threadPool is null");

    CAResult_t ret = CAInitializeNetworkMonitorMutexes();

    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(ERROR, RA_MONITOR_TAG, "CAInitializeNetworkMonitorMutexes failed");
        return CA_STATUS_FAILED;
    }

    ca_mutex_lock(g_networkMonitorContextMutex);

    g_networkMonitorContext = (CARANetworkMonitorContext *)
            OICCalloc(1, sizeof(*g_networkMonitorContext));
    if (!g_networkMonitorContext)
    {
        OIC_LOG(ERROR, RA_MONITOR_TAG, "g_networkMonitorContext Malloc  failed");
        ca_mutex_unlock(g_networkMonitorContextMutex);
        CADestroyNetworkMonitorMutexes();
        return CA_MEMORY_ALLOC_FAILED;
    }
    g_networkMonitorContext->threadPool = threadPool;
    g_networkMonitorContext->networkChangeCb = callback;

    g_networkMonitorContext->nwConnectivityStatus =
            g_isRAConnected ? CA_INTERFACE_UP : CA_INTERFACE_DOWN;

    ca_mutex_unlock(g_networkMonitorContextMutex);

    OIC_LOG(DEBUG, RA_MONITOR_TAG, "CARAInitializeNetworkMonitor OUT");
    return CA_STATUS_OK;
}

void CARATerminateNetworkMonitor()
{
    OIC_LOG(DEBUG, RA_MONITOR_TAG, "CARATerminateNetworkMonitor IN");

    ca_mutex_lock(g_networkMonitorContextMutex);
    g_networkMonitorContext->nwConnectivityStatus = CA_INTERFACE_DOWN;
    g_networkMonitorContext->networkChangeCb = NULL;
    g_networkMonitorContext->threadPool = NULL;
    OICFree(g_networkMonitorContext);
    g_networkMonitorContext = NULL;
    ca_mutex_unlock(g_networkMonitorContextMutex);

    ca_mutex_lock(g_stopNetworkMonitorMutex);
    g_stopNetworkMonitor = true;
    ca_mutex_unlock(g_stopNetworkMonitorMutex);

    CADestroyNetworkMonitorMutexes();

    OIC_LOG(DEBUG, RA_MONITOR_TAG, "CARATerminateNetworkMonitor OUT");
}

CAResult_t CARAStartNetworkMonitor()
{
    OIC_LOG(DEBUG, RA_MONITOR_TAG, "CARAStartNetworkMonitor IN");

    ca_mutex_lock(g_stopNetworkMonitorMutex);
    g_stopNetworkMonitor = false;
    ca_mutex_unlock(g_stopNetworkMonitorMutex);

    ca_mutex_lock(g_networkMonitorContextMutex);
    if (!g_networkMonitorContext)
    {
        OIC_LOG(ERROR, RA_MONITOR_TAG, "g_networkMonitorContext is null");
        ca_mutex_unlock(g_networkMonitorContextMutex);
        return CA_STATUS_FAILED;
    }

    if (CA_STATUS_OK != ca_thread_pool_add_task(g_networkMonitorContext->threadPool,
            CANetworkMonitorThread, NULL))
    {
        OIC_LOG(ERROR, RA_MONITOR_TAG, "[ThreadPool] thread_pool_add_task failed!");
        ca_mutex_unlock(g_networkMonitorContextMutex);
        return CA_STATUS_FAILED;
    }
    // TODO: Register some sort of callback with XMPP lib
    ca_mutex_unlock(g_networkMonitorContextMutex);

    OIC_LOG(DEBUG, RA_MONITOR_TAG, "CARAStartNetworkMonitor OUT");
    return CA_STATUS_OK;
}

CAResult_t CARAStopNetworkMonitor()
{
    OIC_LOG(DEBUG, RA_MONITOR_TAG, "CARAStopNetworkMonitor IN");

    ca_mutex_lock(g_networkMonitorContextMutex);
    if (!g_networkMonitorContext)
    {
        OIC_LOG(ERROR, RA_MONITOR_TAG, "g_networkMonitorContext is null");
        ca_mutex_unlock(g_networkMonitorContextMutex);
        return CA_STATUS_FAILED;
    }

    ca_mutex_unlock(g_networkMonitorContextMutex);

    ca_mutex_lock(g_stopNetworkMonitorMutex);
    if (!g_stopNetworkMonitor)
    {
        g_stopNetworkMonitor = true;
    }
    else
    {
        OIC_LOG(DEBUG, RA_MONITOR_TAG, "CARAStopNetworkMonitor, already stopped!");
    }
    ca_mutex_unlock(g_stopNetworkMonitorMutex);

    OIC_LOG(DEBUG, RA_MONITOR_TAG, "CARAStopNetworkMonitor OUT");
    return CA_STATUS_OK;
}

bool CARAIsConnected()
{
    return g_isRAConnected;
}

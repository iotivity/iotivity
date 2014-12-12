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

#include "caethernetinterface.h"

#include <string.h>
#include <ifaddrs.h>
#include <netdb.h>

#include "caadapterutils.h"
#include "umutex.h"
#include "logger.h"
#include "oic_malloc.h"

#define ETHERNET_MONITOR_TAG "ETHERNET_MONITOR"

/**
 * @var nwConnectivityStatus
 * @brief  Maintains network status.
 */
static CANetworkStatus_t nwConnectivityStatus;

/**
 * @var gEthernetNetInfoMutex
 * @brief  Mutex for synchronizing access to cached interface and IP address information.
 */
static u_mutex gEthernetNetInfoMutex = NULL;

/**
 * @var gEthernetInterfaceName
 * @brief  Maintains interface name.
 */
static char *gEthernetInterfaceName = NULL;

/**
 * @var gEthernetIPAddress
 * @brief  Maintains interface IP address.
 */
static char *gEthernetIPAddress = NULL;

/**
 * @var gThreadPool
 * @brief ThreadPool for storing u_thread_pool_t handle passed from adapter
 */
static u_thread_pool_t gThreadPool = NULL;

/**
 * @var gStopNetworkMonitor
 * @brief Flag to control the Network Monitor Thread
 */
static bool gStopNetworkMonitor = false;

/**
 * @var gNetworkChangeCb
 * @brief  Maintains network connection state change callback.
 */
static CAEthernetConnectionStateChangeCallback gNetworkChangeCb = NULL;

/**
 * @fn CAEthernetGetInterfaceInformation
 * @brief This methods gets local interface name and IP address information.
 */
static void CAEthernetGetInterfaceInformation(char **interfaceName, char **ipAddress);

static void CANetworkMonitorThread(void *threadData);

CAResult_t CAEthernetInitializeNetworkMonitor(const u_thread_pool_t threadPool)
{
    OIC_LOG_V(DEBUG, ETHERNET_MONITOR_TAG, "IN");

    gThreadPool = threadPool;

    if (!gEthernetNetInfoMutex)
    {
        gEthernetNetInfoMutex = u_mutex_new();
    }

    u_mutex_lock(gEthernetNetInfoMutex);
    CAEthernetGetInterfaceInformation(&gEthernetInterfaceName, &gEthernetIPAddress);
    u_mutex_unlock(gEthernetNetInfoMutex);

    nwConnectivityStatus = (gEthernetIPAddress) ? CA_INTERFACE_UP : CA_INTERFACE_DOWN;

    OIC_LOG_V(DEBUG, ETHERNET_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

void CAEthernetTerminateNetworkMonitor(void)
{
    OIC_LOG_V(DEBUG, ETHERNET_MONITOR_TAG, "IN");

    gThreadPool = NULL;

    if (gEthernetInterfaceName)
    {
        OICFree(gEthernetInterfaceName);
        gEthernetInterfaceName = NULL;
    }

    if (gEthernetIPAddress)
    {
        OICFree(gEthernetIPAddress);
        gEthernetIPAddress = NULL;
    }

    if (gEthernetNetInfoMutex)
    {
        u_mutex_free(gEthernetNetInfoMutex);
        gEthernetNetInfoMutex = NULL;
    }

    OIC_LOG_V(DEBUG, ETHERNET_MONITOR_TAG, "OUT");
}

CAResult_t CAEthernetStartNetworkMonitor(void)
{
    OIC_LOG_V(DEBUG, ETHERNET_MONITOR_TAG, "IN");

    u_mutex_lock(gEthernetNetInfoMutex);
    gStopNetworkMonitor = false;
    u_mutex_unlock(gEthernetNetInfoMutex);

    if (gStopNetworkMonitor)
    {
        OIC_LOG_V(ERROR, ETHERNET_MONITOR_TAG, "Network Monitor Thread is already running!");
        return CA_SERVER_STARTED_ALREADY;
    }

    if (CA_STATUS_OK != u_thread_pool_add_task(gThreadPool, (void *) CANetworkMonitorThread,
            (void *)NULL))
    {
        OIC_LOG(ERROR, ETHERNET_MONITOR_TAG, "[ThreadPool] thread_pool_add_task failed!");
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, ETHERNET_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAEthernetStopNetworkMonitor(void)
{
    OIC_LOG_V(DEBUG, ETHERNET_MONITOR_TAG, "IN");

    if (gStopNetworkMonitor)
    {
        OIC_LOG(DEBUG, ETHERNET_MONITOR_TAG, "CAEthernetStopNetworkMonitor, already stopped");

        return CA_STATUS_OK;
    }

    u_mutex_lock(gEthernetNetInfoMutex);
    gStopNetworkMonitor = true;
    u_mutex_unlock(gEthernetNetInfoMutex);

    OIC_LOG_V(DEBUG, ETHERNET_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAEthernetGetInterfaceInfo(char **interfaceName, char **ipAddress)
{
    OIC_LOG_V(DEBUG, ETHERNET_MONITOR_TAG, "IN");

    u_mutex_lock(gEthernetNetInfoMutex);

    if (gEthernetInterfaceName && strlen(gEthernetInterfaceName))
    {
        *interfaceName = (gEthernetInterfaceName) ? strndup(gEthernetInterfaceName,
                         strlen(gEthernetInterfaceName)) :
                         NULL;
    }

    if (gEthernetIPAddress && strlen(gEthernetIPAddress))
    {
        *ipAddress = (gEthernetIPAddress) ? strndup(gEthernetIPAddress, strlen(gEthernetIPAddress)) :
                     NULL;
    }

    u_mutex_unlock(gEthernetNetInfoMutex);

    OIC_LOG_V(DEBUG, ETHERNET_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

bool CAEthernetIsConnected(void)
{
    OIC_LOG_V(DEBUG, ETHERNET_MONITOR_TAG, "IN");

    if (nwConnectivityStatus == CA_INTERFACE_DOWN)
        return false;

    return true;
}

void CAEthernetSetConnectionStateChangeCallback(CAEthernetConnectionStateChangeCallback callback)
{
    OIC_LOG_V(DEBUG, ETHERNET_MONITOR_TAG, "IN");

    gNetworkChangeCb = callback;
}

void CAEthernetGetInterfaceInformation(char **interfaceName, char **ipAddress)
{
    struct ifaddrs *ifa = NULL;
    struct ifaddrs *ifp = NULL;

    if (getifaddrs(&ifp) < 0)
    {
        OIC_LOG_V(DEBUG, ETHERNET_MONITOR_TAG, "Get network interface list error");
    }

    for (ifa = ifp; ifa; ifa = ifa->ifa_next)
    {
        char localIPAddress[CA_IPADDR_SIZE];
        socklen_t len;

        if (ifa->ifa_addr == NULL)
            continue;

        if (ifa->ifa_addr->sa_family == AF_INET)
            len = sizeof(struct sockaddr_in);
        else if (ifa->ifa_addr->sa_family == AF_INET6)
            continue;
        else
            continue;

        if (getnameinfo(ifa->ifa_addr, len, localIPAddress,
                        sizeof(localIPAddress), NULL, 0, NI_NUMERICHOST) < 0)
        {
            OIC_LOG_V(DEBUG, ETHERNET_MONITOR_TAG, "Get IPAddress fail");
        }

        // except loopback address
        if (strcmp(localIPAddress, "127.0.0.1") == 0)
            continue;

        // set interface name
        *interfaceName = strndup(ifa->ifa_name, strlen(ifa->ifa_name));

        // set local ip address
        *ipAddress = strndup(localIPAddress, strlen(localIPAddress));
    }

    freeifaddrs(ifp);
}

void CANetworkMonitorThread(void *threadData)
{
    while (!gStopNetworkMonitor)
    {
        if (gStopNetworkMonitor)
        {
            OIC_LOG(DEBUG, ETHERNET_MONITOR_TAG, "Stop Network Monitor Thread is called");
            break;
        }

        // Get network information
        CANetworkStatus_t currNetworkStatus;
        char *interfaceName = NULL;
        char *ipAddress = NULL;
        CAEthernetGetInterfaceInformation(&interfaceName, &ipAddress);

        // check current network status
        currNetworkStatus = (ipAddress) ? CA_INTERFACE_UP : CA_INTERFACE_DOWN;

        // if network status is changed
        if (currNetworkStatus != nwConnectivityStatus)
        {
            // set current network information
            u_mutex_lock(gEthernetNetInfoMutex);

            nwConnectivityStatus = currNetworkStatus;

            OICFree(gEthernetInterfaceName);
            OICFree(gEthernetIPAddress);
            gEthernetInterfaceName = (interfaceName) ? strndup(interfaceName, strlen(interfaceName)) : NULL;
            gEthernetIPAddress = (ipAddress) ? strndup(ipAddress, strlen(ipAddress)) : NULL;

            u_mutex_unlock(gEthernetNetInfoMutex);

            if (gNetworkChangeCb)
            {
                gNetworkChangeCb(gEthernetIPAddress, nwConnectivityStatus);
            }
        }
        OICFree(interfaceName);
        OICFree(ipAddress);
    }
}

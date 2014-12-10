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

#include "cawifiinterface.h"

#include <string.h>
#include <ifaddrs.h>
#include <netdb.h>

#include "caadapterutils.h"
#include "umutex.h"
#include "logger.h"
#include "oic_malloc.h"

#define WIFI_MONITOR_TAG "WIFI_MONITOR"

/**
 * @var nwConnectivityStatus
 * @brief  Maintains network status.
 */
static CANetworkStatus_t nwConnectivityStatus;

/**
 * @var gWifiNetInfoMutex
 * @brief  Mutex for synchronizing access to cached interface and IP address information.
 */
static u_mutex gWifiNetInfoMutex = NULL;

/**
 * @var gWifiInterfaceName
 * @brief  Maintains interface name.
 */
static char *gWifiInterfaceName = NULL;

/**
 * @var gWifiIPAddress
 * @brief  Maintains interface IP address.
 */
static char *gWifiIPAddress = NULL;

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
static CAWiFiConnectionStateChangeCallback gNetworkChangeCb = NULL;

/**
 * @fn CAWiFiGetInterfaceInformation
 * @brief This methods gets local interface name and IP address information.
 */
static void CAWiFiGetInterfaceInformation(char **interfaceName, char **ipAddress);

static void CANetworkMonitorThread(void* threadData);

CAResult_t CAWiFiInitializeNetworkMonitor(const u_thread_pool_t threadPool)
{
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "IN");

    gThreadPool = threadPool;

    if (!gWifiNetInfoMutex)
    {
        gWifiNetInfoMutex = u_mutex_new();
    }

    u_mutex_lock(gWifiNetInfoMutex);
    CAWiFiGetInterfaceInformation(&gWifiInterfaceName, &gWifiIPAddress);
    u_mutex_unlock(gWifiNetInfoMutex);

    nwConnectivityStatus = (gWifiIPAddress) ? CA_INTERFACE_UP : CA_INTERFACE_DOWN;

    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

void CAWiFiTerminateNetworkMonitor(void)
{
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "IN");

    gThreadPool = NULL;

    if (gWifiInterfaceName)
    {
        OICFree(gWifiInterfaceName);
        gWifiInterfaceName = NULL;
    }

    if (gWifiIPAddress)
    {
        OICFree(gWifiIPAddress);
        gWifiIPAddress = NULL;
    }

    if (gWifiNetInfoMutex)
    {
        u_mutex_free(gWifiNetInfoMutex);
        gWifiNetInfoMutex = NULL;
    }

    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "OUT");
}

CAResult_t CAWiFiStartNetworkMonitor(void)
{
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "IN");

    u_mutex_lock(gWifiNetInfoMutex);
    gStopNetworkMonitor = false;
    u_mutex_unlock(gWifiNetInfoMutex);

    if (gStopNetworkMonitor)
    {
        OIC_LOG_V(ERROR, WIFI_MONITOR_TAG, "Network Monitor Thread is already running!");
        return CA_SERVER_STARTED_ALREADY;
    }

    if (CA_STATUS_OK != u_thread_pool_add_task(gThreadPool, (void *) CANetworkMonitorThread,
            (void *)NULL))
    {
        OIC_LOG(ERROR, WIFI_MONITOR_TAG, "[ThreadPool] thread_pool_add_task failed!");
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAWiFiStopNetworkMonitor(void)
{
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "IN");

    if (gStopNetworkMonitor)
    {
        OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "CAWiFiStopNetworkMonitor, already stopped");

        return CA_STATUS_OK;
    }

    u_mutex_lock(gWifiNetInfoMutex);
    gStopNetworkMonitor = true;
    u_mutex_unlock(gWifiNetInfoMutex);

    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAWiFiGetInterfaceInfo(char **interfaceName, char **ipAddress)
{
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "IN");

    VERIFY_NON_NULL(interfaceName, WIFI_MONITOR_TAG, "interface name holder is NULL");
    VERIFY_NON_NULL(ipAddress, WIFI_MONITOR_TAG, "IP address holder is NULL");

    u_mutex_lock(gWifiNetInfoMutex);

    if (gWifiInterfaceName && strlen(gWifiInterfaceName))
    {
        *interfaceName = (gWifiInterfaceName) ? strndup(gWifiInterfaceName, strlen(gWifiInterfaceName)) :
                                NULL;
    }

    if (gWifiIPAddress && strlen(gWifiIPAddress))
    {
        *ipAddress = (gWifiIPAddress) ? strndup(gWifiIPAddress, strlen(gWifiIPAddress)) :
                                NULL;
    }

    u_mutex_unlock(gWifiNetInfoMutex);

    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

bool CAWiFiIsConnected(void)
{
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "IN");

    if (nwConnectivityStatus == CA_INTERFACE_DOWN)
        return false;

    return true;
}

void CAWiFiSetConnectionStateChangeCallback(CAWiFiConnectionStateChangeCallback callback)
{
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "IN");

    gNetworkChangeCb = callback;
}

void CAWiFiGetInterfaceInformation(char **interfaceName, char **ipAddress)
{
    struct ifaddrs *ifa = NULL;
    struct ifaddrs *ifp = NULL;

    if (getifaddrs(&ifp) < 0)
    {
        OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "Get network interface list error");
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
            OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "Get IPAddress fail");
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

void CANetworkMonitorThread(void* threadData)
{
    while (!gStopNetworkMonitor)
    {
        if (gStopNetworkMonitor)
        {
            OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "Stop Network Monitor Thread is called");
            break;
        }

        // Get network information
        CANetworkStatus_t currNetworkStatus;
        char *interfaceName = NULL;
        char *ipAddress = NULL;
        CAWiFiGetInterfaceInformation(&interfaceName, &ipAddress);

        // check current network status
        currNetworkStatus = (ipAddress) ? CA_INTERFACE_UP : CA_INTERFACE_DOWN;

        // if network status is changed
        if (currNetworkStatus != nwConnectivityStatus)
        {
            // set current network information
            u_mutex_lock(gWifiNetInfoMutex);

            nwConnectivityStatus = currNetworkStatus;

            OICFree(gWifiInterfaceName);
            OICFree(gWifiIPAddress);
            gWifiInterfaceName = (interfaceName) ? strndup(interfaceName, strlen(interfaceName)) : NULL;
            gWifiIPAddress = (ipAddress) ? strndup(ipAddress, strlen(ipAddress)) : NULL;

            u_mutex_unlock(gWifiNetInfoMutex);

            if (gNetworkChangeCb)
            {
                gNetworkChangeCb(gWifiIPAddress, nwConnectivityStatus);
            }
        }
        OICFree(interfaceName);
        OICFree(ipAddress);
    }
}

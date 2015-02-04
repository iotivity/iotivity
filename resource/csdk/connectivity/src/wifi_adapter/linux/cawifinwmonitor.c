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

#include <sys/types.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include "logger.h"
#include "oic_malloc.h"
#include "umutex.h"
#include "caadapterutils.h"

#define WIFI_MONITOR_TAG "WIFI_MONITOR"

/**
 * @var g_nwConnectivityStatus
 * @brief  Maintains network status.
 */
static CANetworkStatus_t g_nwConnectivityStatus;

/**
 * @var g_wifiNetInfoMutex
 * @brief  Mutex for synchronizing access to cached interface and IP address information.
 */
static u_mutex g_wifiNetInfoMutex = NULL;

/**
 * @var g_wifiInterfaceName
 * @brief  Maintains interface name.
 */
static char *g_wifiInterfaceName = NULL;

/**
 * @var g_wifiIPAddress
 * @brief  Maintains interface IP address.
 */
static char *g_wifiIPAddress = NULL;

/**
 * @var g_wifiSubnetMask
 * @brief  Maintains interface subnetmask.
 */
static char *g_wifiSubnetMask = NULL;

/**
 * @var g_threadPool
 * @brief ThreadPool for storing u_thread_pool_t handle passed from adapter
 */
static u_thread_pool_t g_threadPool = NULL;

/**
 * @var g_stopNetworkMonitor
 * @brief Flag to control the Network Monitor Thread
 */
static bool g_stopNetworkMonitor = false;

/**
 * @var g_networkChangeCb
 * @brief  Maintains network connection state change callback.
 */
static CAWiFiConnectionStateChangeCallback g_networkChangeCb = NULL;

/**
 * @fn CAWiFiGetInterfaceInformation
 * @brief This methods gets local interface name and IP address information.
 */
static void CAWiFiGetInterfaceInformation(char **interfaceName, char **ipAddress,
        char **subnetMask);

static void CANetworkMonitorThread(void *threadData);

CAResult_t CAWiFiInitializeNetworkMonitor(const u_thread_pool_t threadPool)
{
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "IN");

    g_threadPool = threadPool;

    if (!g_wifiNetInfoMutex)
    {
        g_wifiNetInfoMutex = u_mutex_new();
    }

    u_mutex_lock(g_wifiNetInfoMutex);
    CAWiFiGetInterfaceInformation(&g_wifiInterfaceName, &g_wifiIPAddress, &g_wifiSubnetMask);
    u_mutex_unlock(g_wifiNetInfoMutex);

    g_nwConnectivityStatus = (g_wifiIPAddress) ? CA_INTERFACE_UP : CA_INTERFACE_DOWN;

    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

void CAWiFiTerminateNetworkMonitor(void)
{
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "IN");

    g_threadPool = NULL;

    if (g_wifiInterfaceName)
    {
        OICFree(g_wifiInterfaceName);
        g_wifiInterfaceName = NULL;
    }

    if (g_wifiIPAddress)
    {
        OICFree(g_wifiIPAddress);
        g_wifiIPAddress = NULL;
    }

    if (g_wifiSubnetMask)
    {
        OICFree(g_wifiSubnetMask);
        g_wifiSubnetMask = NULL;
    }

    if (g_wifiNetInfoMutex)
    {
        u_mutex_free(g_wifiNetInfoMutex);
        g_wifiNetInfoMutex = NULL;
    }

    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "OUT");
}

CAResult_t CAWiFiStartNetworkMonitor(void)
{
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "IN");

    u_mutex_lock(g_wifiNetInfoMutex);
    g_stopNetworkMonitor = false;
    u_mutex_unlock(g_wifiNetInfoMutex);

    if (g_stopNetworkMonitor)
    {
        OIC_LOG_V(ERROR, WIFI_MONITOR_TAG, "Stop network monitor requested!");
        return CA_STATUS_FAILED;
    }

    if (CA_STATUS_OK != u_thread_pool_add_task(g_threadPool,  CANetworkMonitorThread,
            NULL))
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

    if (g_stopNetworkMonitor)
    {
        OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "CAWiFiStopNetworkMonitor, already stopped");

        return CA_STATUS_OK;
    }

    u_mutex_lock(g_wifiNetInfoMutex);
    g_stopNetworkMonitor = true;
    u_mutex_unlock(g_wifiNetInfoMutex);

    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAWiFiGetInterfaceInfo(char **interfaceName, char **ipAddress)
{
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "IN");

    VERIFY_NON_NULL(interfaceName, WIFI_MONITOR_TAG, "interface name");
    VERIFY_NON_NULL(ipAddress, WIFI_MONITOR_TAG, "ip address");

    // Get the interface and ipaddress information from cache
    u_mutex_lock(g_wifiNetInfoMutex);

    if (g_wifiInterfaceName == NULL || g_wifiIPAddress == NULL)
    {
        OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "Network not enabled");
        return CA_ADAPTER_NOT_ENABLED;

    }
    *interfaceName = (g_wifiInterfaceName) ? strndup(g_wifiInterfaceName, strlen(g_wifiInterfaceName))
                     : NULL;
    *ipAddress = (g_wifiIPAddress) ? strndup(g_wifiIPAddress, strlen(g_wifiIPAddress))
                 : NULL;

    u_mutex_unlock(g_wifiNetInfoMutex);

    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAWiFiGetInterfaceSubnetMask(char **subnetMask)
{
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "IN");

    VERIFY_NON_NULL(subnetMask, WIFI_MONITOR_TAG, "subnet mask");

    u_mutex_lock(g_wifiNetInfoMutex);
    if (NULL == g_wifiSubnetMask)
    {
        OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "There is no subnet mask information!");
        return CA_STATUS_FAILED;
    }

    *subnetMask = (g_wifiSubnetMask) ? strndup(g_wifiSubnetMask, strlen(g_wifiSubnetMask))
                  : NULL;
    u_mutex_unlock(g_wifiNetInfoMutex);

    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

bool CAWiFiIsConnected(void)
{
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "IN");

    if (g_nwConnectivityStatus == CA_INTERFACE_DOWN)
        return false;

    return true;
}

void CAWiFiSetConnectionStateChangeCallback(
    CAWiFiConnectionStateChangeCallback callback)
{
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "IN");

    g_networkChangeCb = callback;
}

void CAWiFiGetInterfaceInformation(char **interfaceName, char **ipAddress, char **subnetMask)
{
    struct ifaddrs *ifa = NULL;
    struct ifaddrs *ifp = NULL;
    const char *matchName = "wlan";

    if (!interfaceName || !ipAddress || !subnetMask)
    {
        OIC_LOG(ERROR, WIFI_MONITOR_TAG, "Invalid input: interface/ipaddress holder is NULL!");
        return;
    }

    if (-1 == getifaddrs(&ifp))
    {
        OIC_LOG_V(ERROR, WIFI_MONITOR_TAG, "Failed to get interface list!, Error code: %s",
                  strerror(errno));
        return;
    }

    for (ifa = ifp; ifa; ifa = ifa->ifa_next)
    {
        char interfaceAddress[CA_IPADDR_SIZE] = {0};
        char interfaceSubnetMask[CA_IPADDR_SIZE] = {0};
        socklen_t len = sizeof(struct sockaddr_in);

        if (NULL == ifa->ifa_addr)
        {
            continue;
        }

        int type = ifa->ifa_addr->sa_family;
        if (ifa->ifa_flags & IFF_LOOPBACK
            || !((ifa->ifa_flags & IFF_UP) && (ifa->ifa_flags & IFF_RUNNING)))
        {
            continue;
        }

        if (AF_INET != type)
        {
            continue;
        }

        if (!strncasecmp(ifa->ifa_name, matchName, strlen(matchName)))
        {
            // get the interface ip address
            if (0 != getnameinfo(ifa->ifa_addr, len, interfaceAddress,
                                 sizeof(interfaceAddress), NULL, 0, NI_NUMERICHOST))
            {
                OIC_LOG_V(ERROR, WIFI_MONITOR_TAG, "Failed to get IPAddress, Error code: %s",
                          strerror(errno));
                break;
            }

            // get the interface subnet mask
            if (0 != getnameinfo(ifa->ifa_netmask, len, interfaceSubnetMask,
                                 sizeof(interfaceSubnetMask), NULL, 0, NI_NUMERICHOST))
            {
                OIC_LOG_V(ERROR, WIFI_MONITOR_TAG, "Failed to get subnet mask, Error code: %s",
                          strerror(errno));
                break;
            }

            // set interface name
            *interfaceName = strndup(ifa->ifa_name, strlen(ifa->ifa_name));

            // set local ip address
            *ipAddress = strndup(interfaceAddress, strlen(interfaceAddress));

            // set subnet mask
            *subnetMask = strndup(interfaceSubnetMask, strlen(interfaceSubnetMask));
            break;
        }
    }

    freeifaddrs(ifp);
}
void CANetworkMonitorThread(void *threadData)
{
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "IN");

    while (!g_stopNetworkMonitor)
    {
        // Get network information
        CANetworkStatus_t currNetworkStatus;
        char *interfaceName = NULL;
        char *ipAddress = NULL;
        char *subnetMask = NULL;
        CAWiFiGetInterfaceInformation(&interfaceName, &ipAddress, &subnetMask);

        // check current network status
        currNetworkStatus = (ipAddress) ? CA_INTERFACE_UP : CA_INTERFACE_DOWN;

        // if network status is changed
        if (currNetworkStatus != g_nwConnectivityStatus)
        {
            // set current network information
            u_mutex_lock(g_wifiNetInfoMutex);

            g_nwConnectivityStatus = currNetworkStatus;

            OICFree(g_wifiInterfaceName);
            OICFree(g_wifiIPAddress);
            OICFree(g_wifiSubnetMask);
            g_wifiInterfaceName = (interfaceName) ? strndup(interfaceName, strlen(interfaceName)) : NULL;
            g_wifiIPAddress = (ipAddress) ? strndup(ipAddress, strlen(ipAddress)) : NULL;
            g_wifiSubnetMask = (subnetMask) ? strndup(subnetMask, strlen(subnetMask)) : NULL;

            u_mutex_unlock(g_wifiNetInfoMutex);

            if (g_networkChangeCb)
            {
                g_networkChangeCb(g_wifiIPAddress, g_nwConnectivityStatus);
            }
        }
        OICFree(interfaceName);
        OICFree(ipAddress);
        OICFree(subnetMask);
    }

    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "OUT");
}


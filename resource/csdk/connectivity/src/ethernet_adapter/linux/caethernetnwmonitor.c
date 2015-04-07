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

// Defining _POSIX_C_SOURCE macro with 200809L (or greater) as value
// causes header files to expose definitions
// corresponding to the POSIX.1-2008 base
// specification (excluding the XSI extension).
// For POSIX.1-2008 base specification,
// Refer http://pubs.opengroup.org/stage7tc1/
//
// For this specific file, see use of strndup,
// Refer http://man7.org/linux/man-pages/man3/strdup.3.html
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

// Defining _BSD_SOURCE or _DEFAULT_SOURCE causes header files to expose
// definitions that may otherwise be skipped. Skipping can cause implicit
// declaration warnings and/or bugs and subtle problems in code execution.
// For glibc information on feature test macros,
// Refer http://www.gnu.org/software/libc/manual/html_node/Feature-Test-Macros.html
//
// This file requires #define use due to IFF_LOOPBACK
#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#include <net/if.h>

#include "caethernetinterface.h"

#include <sys/types.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <strings.h>
#include <errno.h>

#include "caadapterutils.h"
#include "umutex.h"
#include "logger.h"
#include "oic_malloc.h"
#include "oic_string.h"

#define ETHERNET_MONITOR_TAG "ETHERNET_MONITOR"

/**
 * @var nwConnectivityStatus
 * @brief  Maintains network status.
 */
static CANetworkStatus_t nwConnectivityStatus;

/**
 * @var g_ethernetNetInfoMutex
 * @brief  Mutex for synchronizing access to cached interface and IP address information.
 */
static u_mutex g_ethernetNetInfoMutex = NULL;

/**
 * @var g_ethernetInterfaceName
 * @brief  Maintains interface name.
 */
static char *g_ethernetInterfaceName = NULL;

/**
 * @var g_ethernetIPAddress
 * @brief  Maintains interface IP address.
 */
static char *g_ethernetIPAddress = NULL;

/**
 * @var g_ethernetSubnetMask
 * @brief  Maintains interface subnetmask.
 */
static char *g_ethernetSubnetMask = NULL;

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
static CAEthernetConnectionStateChangeCallback g_networkChangeCb = NULL;

/**
 * @fn CAEthernetGetInterfaceInformation
 * @brief This methods gets local interface name and IP address information.
 */
static void CAEthernetGetInterfaceInformation(char **interfaceName, char **ipAddress,
        char **subnetMask);

static void CANetworkMonitorThread(void *threadData);

CAResult_t CAEthernetInitializeNetworkMonitor(const u_thread_pool_t threadPool)
{
    OIC_LOG(DEBUG, ETHERNET_MONITOR_TAG, "IN");

    g_threadPool = threadPool;

    if (!g_ethernetNetInfoMutex)
    {
        g_ethernetNetInfoMutex = u_mutex_new();
    }

    u_mutex_lock(g_ethernetNetInfoMutex);
    CAEthernetGetInterfaceInformation(&g_ethernetInterfaceName, &g_ethernetIPAddress,
                                      &g_ethernetSubnetMask);
    u_mutex_unlock(g_ethernetNetInfoMutex);

    nwConnectivityStatus = (g_ethernetIPAddress) ? CA_INTERFACE_UP : CA_INTERFACE_DOWN;

    OIC_LOG(DEBUG, ETHERNET_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

void CAEthernetTerminateNetworkMonitor(void)
{
    OIC_LOG(DEBUG, ETHERNET_MONITOR_TAG, "IN");

    g_threadPool = NULL;

    if (g_ethernetInterfaceName)
    {
        OICFree(g_ethernetInterfaceName);
        g_ethernetInterfaceName = NULL;
    }

    if (g_ethernetIPAddress)
    {
        OICFree(g_ethernetIPAddress);
        g_ethernetIPAddress = NULL;
    }

    if (g_ethernetSubnetMask)
    {
        OICFree(g_ethernetSubnetMask);
        g_ethernetSubnetMask = NULL;
    }

    if (g_ethernetNetInfoMutex)
    {
        u_mutex_free(g_ethernetNetInfoMutex);
        g_ethernetNetInfoMutex = NULL;
    }

    OIC_LOG(DEBUG, ETHERNET_MONITOR_TAG, "OUT");
}

CAResult_t CAEthernetStartNetworkMonitor(void)
{
    OIC_LOG(DEBUG, ETHERNET_MONITOR_TAG, "IN");

    g_stopNetworkMonitor = false;

    if (g_stopNetworkMonitor)
    {
        OIC_LOG(ERROR, ETHERNET_MONITOR_TAG, "Stop network monitor requested");
        return CA_STATUS_FAILED;
    }

    if (CA_STATUS_OK != u_thread_pool_add_task(g_threadPool, (void *) CANetworkMonitorThread,
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
    OIC_LOG(DEBUG, ETHERNET_MONITOR_TAG, "IN");

    if (g_stopNetworkMonitor)
    {
        OIC_LOG(DEBUG, ETHERNET_MONITOR_TAG, "CAEthernetStopNetworkMonitor, already stopped!");
        return CA_STATUS_OK;
    }

    g_stopNetworkMonitor = true;

    OIC_LOG(DEBUG, ETHERNET_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAEthernetGetInterfaceInfo(char **interfaceName, char **ipAddress)
{
    OIC_LOG(DEBUG, ETHERNET_MONITOR_TAG, "IN");

    VERIFY_NON_NULL(interfaceName, ETHERNET_MONITOR_TAG, "interface name");
    VERIFY_NON_NULL(ipAddress, ETHERNET_MONITOR_TAG, "ip address");

    // Get the interface and ipaddress information from cache
    u_mutex_lock(g_ethernetNetInfoMutex);
    if (g_ethernetInterfaceName == NULL || g_ethernetIPAddress == NULL)
    {
        OIC_LOG(DEBUG, ETHERNET_MONITOR_TAG, "Network not enabled");

        u_mutex_unlock(g_ethernetNetInfoMutex);
        return CA_ADAPTER_NOT_ENABLED;
    }

    *interfaceName = (g_ethernetInterfaceName) ? strndup(g_ethernetInterfaceName,
                     strlen(g_ethernetInterfaceName)) : NULL;
    *ipAddress = (g_ethernetIPAddress) ? OICStrdup((const char *)g_ethernetIPAddress)
                 : NULL;

    u_mutex_unlock(g_ethernetNetInfoMutex);

    OIC_LOG(DEBUG, ETHERNET_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAEthernetGetInterfaceSubnetMask(char **subnetMask)
{
    OIC_LOG(DEBUG, ETHERNET_MONITOR_TAG, "IN");

    VERIFY_NON_NULL(subnetMask, ETHERNET_MONITOR_TAG, "subnet mask");

    u_mutex_lock(g_ethernetNetInfoMutex);
    if (NULL == g_ethernetSubnetMask)
    {
        OIC_LOG(DEBUG, ETHERNET_MONITOR_TAG, "There is no subnet mask information!");

        u_mutex_unlock(g_ethernetNetInfoMutex);
        return CA_STATUS_FAILED;
    }

    *subnetMask = (g_ethernetSubnetMask) ?
                  strndup(g_ethernetSubnetMask, strlen(g_ethernetSubnetMask))
                  : NULL;
    u_mutex_unlock(g_ethernetNetInfoMutex);

    OIC_LOG(DEBUG, ETHERNET_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

bool CAEthernetIsConnected(void)
{
    OIC_LOG(DEBUG, ETHERNET_MONITOR_TAG, "IN");

    if (CA_INTERFACE_DOWN == nwConnectivityStatus) {
        OIC_LOG(DEBUG, ETHERNET_MONITOR_TAG, "OUT");

        return false;
    }

    OIC_LOG(DEBUG, ETHERNET_MONITOR_TAG, "OUT");

    return true;
}

void CAEthernetSetConnectionStateChangeCallback
    (CAEthernetConnectionStateChangeCallback callback)
{
    OIC_LOG(DEBUG, ETHERNET_MONITOR_TAG, "IN");
    g_networkChangeCb = callback;

    OIC_LOG(DEBUG, ETHERNET_MONITOR_TAG, "OUT");
}

void CAEthernetGetInterfaceInformation(char **interfaceName,
    char **ipAddress, char **subnetMask)
{
    if (!interfaceName || !ipAddress || !subnetMask)
    {
        OIC_LOG(ERROR, ETHERNET_MONITOR_TAG, "Invalid input: interface/ipaddress/subnet mask holder is NULL!");
        return;
    }

    struct ifaddrs *ifp = NULL;
    if (-1 == getifaddrs(&ifp))
    {
        OIC_LOG_V(ERROR, ETHERNET_MONITOR_TAG, "Failed to get interface list!, Error code: %s",
                  strerror(errno));
        return;
    }

    struct ifaddrs *ifa = NULL;
    for (ifa = ifp; ifa; ifa = ifa->ifa_next)
    {
        char interfaceAddress[CA_IPADDR_SIZE];
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

        const char matchName[] = "eth";
        int matchNameLen = strlen(matchName);
        if (!strncasecmp(ifa->ifa_name, matchName, matchNameLen))
        {
            // get the interface ip address
            if (0 != getnameinfo(ifa->ifa_addr, len, interfaceAddress,
                                 sizeof(interfaceAddress), NULL, 0, NI_NUMERICHOST))
            {
                OIC_LOG_V(ERROR, ETHERNET_MONITOR_TAG, "Failed to get IPAddress, Error code: %s",
                          strerror(errno));
                break;
            }

            // get the interface subnet mask
            if (0 != getnameinfo(ifa->ifa_netmask, len, interfaceSubnetMask,
                                 sizeof(interfaceSubnetMask), NULL, 0, NI_NUMERICHOST))
            {
                OIC_LOG_V(ERROR, ETHERNET_MONITOR_TAG, "Failed to get subnet mask, Error code: %s",
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
    OIC_LOG(DEBUG, ETHERNET_MONITOR_TAG, "IN");

    while (!g_stopNetworkMonitor)
    {
        // Get network information
        char *interfaceName = NULL;
        char *ipAddress = NULL;
        char *subnetMask = NULL;
        CAEthernetGetInterfaceInformation(&interfaceName, &ipAddress, &subnetMask);

        // check current network status
        CANetworkStatus_t currNetworkStatus;
        currNetworkStatus = (ipAddress) ? CA_INTERFACE_UP : CA_INTERFACE_DOWN;

        // if network status is changed
        if (currNetworkStatus != nwConnectivityStatus)
        {
            // set current network information
            u_mutex_lock(g_ethernetNetInfoMutex);

            nwConnectivityStatus = currNetworkStatus;

            OICFree(g_ethernetInterfaceName);
            OICFree(g_ethernetIPAddress);
            OICFree(g_ethernetSubnetMask);
            g_ethernetInterfaceName =
                (interfaceName) ? strndup(interfaceName, strlen(interfaceName)) : NULL;
            g_ethernetIPAddress = (ipAddress) ? strndup(ipAddress, strlen(ipAddress)) : NULL;
            g_ethernetSubnetMask = (subnetMask) ? strndup(subnetMask, strlen(subnetMask)) : NULL;

            u_mutex_unlock(g_ethernetNetInfoMutex);

            if (g_networkChangeCb)
            {
                g_networkChangeCb(g_ethernetIPAddress, nwConnectivityStatus);
            }
        }
        OICFree(interfaceName);
        OICFree(ipAddress);
        OICFree(subnetMask);
    }

    OIC_LOG(DEBUG, ETHERNET_MONITOR_TAG, "OUT");
}


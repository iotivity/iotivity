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

#include <sys/types.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>

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
 * @var gEthernetSubnetMask
 * @brief  Maintains interface subnetmask.
 */
static char *gEthernetSubnetMask = NULL;

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
static void CAEthernetGetInterfaceInformation(char **interfaceName, char **ipAddress,
                                              char **subnetMask);

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
    CAEthernetGetInterfaceInformation(&gEthernetInterfaceName, &gEthernetIPAddress, &gEthernetSubnetMask);
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
        OIC_LOG_V(ERROR, ETHERNET_MONITOR_TAG, "Stop network monitor requested");
        return CA_STATUS_FAILED;
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
        OIC_LOG(DEBUG, ETHERNET_MONITOR_TAG, "CAEthernetStopNetworkMonitor, already stopped!");
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

    VERIFY_NON_NULL(interfaceName, ETHERNET_MONITOR_TAG, "interface name");
    VERIFY_NON_NULL(ipAddress, ETHERNET_MONITOR_TAG, "ip address");

    // Get the interface and ipaddress information from cache
    u_mutex_lock(gEthernetNetInfoMutex);
    if(gEthernetInterfaceName == NULL || gEthernetIPAddress == NULL)
    {
        OIC_LOG_V(DEBUG, ETHERNET_MONITOR_TAG, "Network not enabled");
        return CA_ADAPTER_NOT_ENABLED;
    }

    *interfaceName = (gEthernetInterfaceName) ? strndup(gEthernetInterfaceName,
                               strlen(gEthernetInterfaceName)) : NULL;
    *ipAddress = (gEthernetIPAddress) ? strndup(gEthernetIPAddress,strlen(gEthernetIPAddress))
                               : NULL;

    u_mutex_unlock(gEthernetNetInfoMutex);

    OIC_LOG_V(DEBUG, ETHERNET_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAEthernetGetInterfaceSubnetMask(char **subnetMask)
{
    OIC_LOG_V(DEBUG, ETHERNET_MONITOR_TAG, "IN");

    VERIFY_NON_NULL(subnetMask, ETHERNET_MONITOR_TAG, "subnet mask");

    u_mutex_lock(gEthernetNetInfoMutex);
    if(NULL == gEthernetSubnetMask)
    {
        OIC_LOG_V(DEBUG, ETHERNET_MONITOR_TAG, "There is no subnet mask information!");
        return CA_STATUS_FAILED;
    }

    *subnetMask = (gEthernetSubnetMask) ? strndup(gEthernetSubnetMask,strlen(gEthernetSubnetMask))
                               : NULL;
    u_mutex_unlock(gEthernetNetInfoMutex);

    OIC_LOG_V(DEBUG, ETHERNET_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

bool CAEthernetIsConnected(void)
{
    OIC_LOG_V(DEBUG, ETHERNET_MONITOR_TAG, "IN");

    if (CA_INTERFACE_DOWN == nwConnectivityStatus)
        return false;

    return true;
}

void CAEthernetSetConnectionStateChangeCallback(CAEthernetConnectionStateChangeCallback callback)
{
    OIC_LOG_V(DEBUG, ETHERNET_MONITOR_TAG, "IN");
    gNetworkChangeCb = callback;
}

void CAEthernetGetInterfaceInformation(char **interfaceName, char **ipAddress, char ** subnetMask)
{
    struct ifaddrs *ifa = NULL;
    struct ifaddrs *ifp = NULL;
    const char *matchName = "eth";

    if (!interfaceName || !ipAddress || !subnetMask)
    {
        OIC_LOG(ERROR, ETHERNET_MONITOR_TAG, "Invalid input: interface/ipaddress holder is NULL!");
        return;
    }

    if (-1 == getifaddrs(&ifp))
    {
        OIC_LOG_V(ERROR, ETHERNET_MONITOR_TAG, "Failed to get interface list!, Error code: %s",
                          strerror(errno));
        return;
    }

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

        if (!strncasecmp(ifa->ifa_name,matchName,strlen(matchName)))
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
    OIC_LOG_V(DEBUG, ETHERNET_MONITOR_TAG, "IN");

    while (!gStopNetworkMonitor)
    {
        // Get network information
        CANetworkStatus_t currNetworkStatus;
        char *interfaceName = NULL;
        char *ipAddress = NULL;
        char *subnetMask = NULL;
        CAEthernetGetInterfaceInformation(&interfaceName, &ipAddress, &subnetMask);

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
            OICFree(gEthernetSubnetMask);
            gEthernetInterfaceName = (interfaceName) ? strndup(interfaceName, strlen(interfaceName)) : NULL;
            gEthernetIPAddress = (ipAddress) ? strndup(ipAddress, strlen(ipAddress)) : NULL;
            gEthernetSubnetMask = (subnetMask) ? strndup(subnetMask, strlen(subnetMask)) : NULL;

            u_mutex_unlock(gEthernetNetInfoMutex);

            if (gNetworkChangeCb)
            {
                gNetworkChangeCb(gEthernetIPAddress, nwConnectivityStatus);
            }
        }
        OICFree(interfaceName);
        OICFree(ipAddress);
        OICFree(subnetMask);
    }

    OIC_LOG_V(DEBUG, ETHERNET_MONITOR_TAG, "OUT");
}

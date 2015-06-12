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

#include "caipinterface.h"

#include <wifi.h>

#include "caadapterutils.h"
#include "camutex.h"
#include "logger.h"
#include "oic_malloc.h"
#include "oic_string.h"

#define IP_MONITOR_TAG "IP_MONITOR"

/**
 * @var g_networkMonitorContextMutex
 * @brief  Mutex for synchronizing access to cached interface and IP address information.
 */
static ca_mutex g_networkMonitorContextMutex = NULL;

/**
 * @struct CAIPNwMonitorContext
 * @brief  Used for storing network monitor context information.
 */
typedef struct
{
    u_arraylist_t  *netInterfaceList;
    ca_thread_pool_t threadPool;
    CANetworkStatus_t nwConnectivityStatus;
    CAIPConnectionStateChangeCallback networkChangeCb;
} CAIPNetworkMonitorContext;

/**
 * @var g_networkMonitorContext
 * @brief  network monitor context.
 */
static CAIPNetworkMonitorContext *g_networkMonitorContext = NULL;

static void CARemoveInterfaceInfo()
{
    ca_mutex_lock(g_networkMonitorContextMutex);
    if (!g_networkMonitorContext->netInterfaceList)
    {
        OIC_LOG(ERROR, IP_MONITOR_TAG,
                "netInterfaceList is empty");
        ca_mutex_unlock(g_networkMonitorContextMutex);
        return;
    }

    uint32_t list_index = 0;
    uint32_t list_length = u_arraylist_length(g_networkMonitorContext->netInterfaceList);
    for (list_index = 0; list_index < list_length; list_index++)
    {
        CANetInfo_t *netInfo = (CANetInfo_t *)u_arraylist_get(
                               g_networkMonitorContext->netInterfaceList, list_index);
        if (!netInfo)
        {
            continue;
        }

        if (u_arraylist_remove(g_networkMonitorContext->netInterfaceList, list_index))
        {
            if (g_networkMonitorContext->networkChangeCb)
            {
                g_networkMonitorContext->networkChangeCb(netInfo->ipAddress,
                                                         CA_INTERFACE_DOWN);
            }
        }
        else
        {
            OIC_LOG(ERROR, IP_MONITOR_TAG, "u_arraylist_remove failed");
        }

        OICFree(netInfo);
    }

    u_arraylist_free(&g_networkMonitorContext->netInterfaceList);

    ca_mutex_unlock(g_networkMonitorContextMutex);
}

static bool CACheckIsInterfaceInfoChanged(const CANetInfo_t *info)
{
    VERIFY_NON_NULL_RET(info, IP_MONITOR_TAG, "info is null", false);

    ca_mutex_lock(g_networkMonitorContextMutex);
    uint32_t list_index = 0;
    uint32_t list_length = u_arraylist_length(g_networkMonitorContext->netInterfaceList);
    for (list_index = 0; list_index < list_length; list_index++)
    {
        CANetInfo_t *netInfo = (CANetInfo_t *)u_arraylist_get(
                               g_networkMonitorContext->netInterfaceList, list_index);
        if (!netInfo)
        {
            continue;
        }

        if (strncmp(netInfo->interfaceName, info->interfaceName, strlen(info->interfaceName)) == 0)
        {
            if (strncmp(netInfo->ipAddress, info->ipAddress, strlen(info->ipAddress)) == 0)
            {
                ca_mutex_unlock(g_networkMonitorContextMutex);
                return false;
            }
            else
            {
                OIC_LOG(DEBUG, IP_MONITOR_TAG, "Network interface info changed");
                if (u_arraylist_remove(g_networkMonitorContext->netInterfaceList, list_index))
                {
                    if (g_networkMonitorContext->networkChangeCb)
                    {
                        g_networkMonitorContext->networkChangeCb(netInfo->ipAddress,
                                                                 CA_INTERFACE_DOWN);
                    }
                    OICFree(netInfo);
                }
                else
                {
                    OIC_LOG(ERROR, IP_MONITOR_TAG, "u_arraylist_remove failed");
                }
                break;
            }
        }
    }

    CANetInfo_t *newNetInfo = (CANetInfo_t *)OICMalloc(sizeof(CANetInfo_t));
    if (!newNetInfo)
    {
        OIC_LOG(ERROR, IP_MONITOR_TAG, "newNetInfo malloc failed");
        ca_mutex_unlock(g_networkMonitorContextMutex);
        return false;
    }
    memcpy(newNetInfo, info, sizeof(*newNetInfo));

    OIC_LOG(DEBUG, IP_MONITOR_TAG, "New Interface found");

    CAResult_t result = u_arraylist_add(g_networkMonitorContext->netInterfaceList,
                                        (void *)newNetInfo);
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, IP_MONITOR_TAG, "u_arraylist_add failed!");
        OICFree(newNetInfo);
        ca_mutex_unlock(g_networkMonitorContextMutex);
        return false;
    }
    ca_mutex_unlock(g_networkMonitorContextMutex);

    /*Callback will be unset only at the time of termination. By that time, all the threads will be
      stopped gracefully. This callback is properly protected*/
    if (g_networkMonitorContext->networkChangeCb)
    {
        g_networkMonitorContext->networkChangeCb(newNetInfo->ipAddress, CA_INTERFACE_UP);
    }

    return true;
}

void CAWiFiGetInterfaceInformation(char **interfaceName, char **ipAddress, char **subnetMask)
{
    OIC_LOG(DEBUG, IP_MONITOR_TAG, "IN");

    int ret = WIFI_ERROR_NONE;

    if (!interfaceName || !ipAddress || !subnetMask)
    {
        OIC_LOG(ERROR, IP_MONITOR_TAG, "Invalid input: interface/ipaddress holder is NULL!");
        return;
    }

    // Get wifi interface name
    if (WIFI_ERROR_NONE != (ret = wifi_get_network_interface_name(interfaceName)))
    {
        OIC_LOG_V(ERROR, IP_MONITOR_TAG, "Failed to get interface name! error num [%d]", ret);
        return;
    }

    // Get wifi connected IP address
    wifi_ap_h accessPoint = NULL;
    if (WIFI_ERROR_NONE != (ret = wifi_get_connected_ap(&accessPoint)))
    {
        OIC_LOG_V(ERROR, IP_MONITOR_TAG, "Failed to get access point! error num [%d]",
                  ret);

        OICFree(*interfaceName);
        *interfaceName = NULL;
        return;
    }

    if (WIFI_ERROR_NONE != (ret = wifi_ap_get_ip_address(accessPoint, WIFI_ADDRESS_FAMILY_IPV4,
                                  ipAddress)))
    {
        OIC_LOG_V(ERROR, IP_MONITOR_TAG, "Failed to get interface address! error num [%d]",
                  ret);
        OICFree(*interfaceName);
        *interfaceName = NULL;
        return;
    }

    if (WIFI_ERROR_NONE != (ret = wifi_ap_get_subnet_mask(accessPoint, WIFI_ADDRESS_FAMILY_IPV4,
                                  subnetMask)))
    {
        OIC_LOG_V(ERROR, IP_MONITOR_TAG, "Failed to get interface address! error num [%d]",
                  ret);
        OICFree(*ipAddress);
        OICFree(*interfaceName);
        *ipAddress = NULL;
        *interfaceName = NULL;
        return;
    }

    OIC_LOG(DEBUG, IP_MONITOR_TAG, "OUT");
}

static void CAIPGetInterfaceInformation(u_arraylist_t **netInterfaceList)
{
    VERIFY_NON_NULL_VOID(netInterfaceList, IP_MONITOR_TAG, "netInterfaceList is null");

    // Get wifi network information
    char *interfaceName = NULL;
    char *ipAddress = NULL;
    char *subnetMask = NULL;
    ///TODO: currently we are filling single interface. Once found the proper tizen apis for
    // getting multiple interfaces, then this function will be updated.
    CAWiFiGetInterfaceInformation(&interfaceName, &ipAddress, &subnetMask);

    if (!interfaceName || !ipAddress || !subnetMask)
    {
        OIC_LOG(ERROR, IP_MONITOR_TAG, "interface/ipaddress/subnetmask is NULL!");
        return;
    }

    CANetInfo_t *netInfo = (CANetInfo_t *)OICCalloc(1, sizeof(CANetInfo_t));
    if (!netInfo)
    {
        OIC_LOG(ERROR, IP_MONITOR_TAG, "Malloc failed");
        OICFree(interfaceName);
        OICFree(ipAddress);
        OICFree(subnetMask);
        return;
    }

    // set interface name
    strncpy(netInfo->interfaceName, interfaceName, strlen(interfaceName));

    // set local ip address
    strncpy(netInfo->ipAddress, ipAddress, strlen(ipAddress));

    // set subnet mask
    strncpy(netInfo->subnetMask, subnetMask, strlen(subnetMask));

    CAResult_t result = u_arraylist_add(*netInterfaceList, (void *)netInfo);
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, IP_MONITOR_TAG, "u_arraylist_add failed!");
    }
    OICFree(interfaceName);
    OICFree(ipAddress);
    OICFree(subnetMask);
}


void CAWIFIConnectionStateChangedCb(wifi_connection_state_e state, wifi_ap_h ap,
                                    void *userData)
{
    OIC_LOG(DEBUG, IP_MONITOR_TAG, "IN");

    if (WIFI_CONNECTION_STATE_ASSOCIATION == state
        || WIFI_CONNECTION_STATE_CONFIGURATION == state)
    {
        OIC_LOG(DEBUG, IP_MONITOR_TAG, "Connection is in Association State");
        return;
    }

    // If Wifi is connected, then get the latest IP from the WIFI Interface
    if (WIFI_CONNECTION_STATE_CONNECTED == state)
    {
        // Get network information
        char *interfaceName = NULL;
        char *ipAddress = NULL;
        char *subnetMask = NULL;
        CAWiFiGetInterfaceInformation(&interfaceName, &ipAddress, &subnetMask);

        CANetInfo_t *netInfo = (CANetInfo_t *)OICCalloc(1, sizeof(CANetInfo_t));
        if (!netInfo)
        {
            OIC_LOG(ERROR, IP_MONITOR_TAG, "Malloc failed");
            OICFree(interfaceName);
            OICFree(ipAddress);
            OICFree(subnetMask);
            return;
        }

        // set interface name
        strncpy(netInfo->interfaceName, interfaceName, strlen(interfaceName));

        // set local ip address
        strncpy(netInfo->ipAddress, ipAddress, strlen(ipAddress));

        // set subnet mask
        strncpy(netInfo->subnetMask, subnetMask, strlen(subnetMask));

        bool ret = CACheckIsInterfaceInfoChanged(netInfo);
        if (ret)
        {
            OIC_LOG(DEBUG, IP_MONITOR_TAG, "CACheckIsInterfaceInfoChanged true");
        }

        OICFree(interfaceName);
        OICFree(ipAddress);
        OICFree(subnetMask);
    }
    else
    {
        CARemoveInterfaceInfo();
    }

    OIC_LOG(DEBUG, IP_MONITOR_TAG, "OUT");
}

void CAWIFIDeviceStateChangedCb(wifi_device_state_e state, void *userData)
{
    OIC_LOG(DEBUG, IP_MONITOR_TAG, "IN");

    if (WIFI_DEVICE_STATE_ACTIVATED == state)
    {
        OIC_LOG(DEBUG, IP_MONITOR_TAG, "Wifi is in Activated State");
    }
    else
    {
        CAWIFIConnectionStateChangedCb(WIFI_CONNECTION_STATE_DISCONNECTED, NULL, NULL);
        OIC_LOG(DEBUG, IP_MONITOR_TAG, "Wifi is in Deactivated State");
    }

    OIC_LOG(DEBUG, IP_MONITOR_TAG, "OUT");
}

static CAResult_t CAInitializeNetworkMonitorMutexes()
{
    if (!g_networkMonitorContextMutex)
    {
        g_networkMonitorContextMutex = ca_mutex_new();
        if (!g_networkMonitorContextMutex)
        {
            OIC_LOG(ERROR, IP_MONITOR_TAG, "g_networkMonitorContextMutex Malloc  failed");
            return CA_MEMORY_ALLOC_FAILED;
        }
    }

    return CA_STATUS_OK;
}

static void CADestroyNetworkMonitorMutexes()
{
    ca_mutex_free(g_networkMonitorContextMutex);
    g_networkMonitorContextMutex = NULL;
}

CAResult_t CAIPInitializeNetworkMonitor(const ca_thread_pool_t threadPool)
{
    OIC_LOG(DEBUG, IP_MONITOR_TAG, "IN");

    VERIFY_NON_NULL(threadPool, IP_MONITOR_TAG, "threadPool is null");

    CAResult_t ret = CAInitializeNetworkMonitorMutexes();

    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(ERROR, IP_MONITOR_TAG, "CAInitializeNetworkMonitorMutexes failed");
        return CA_STATUS_FAILED;
    }
    ca_mutex_lock(g_networkMonitorContextMutex);

     // Initialize Wifi service
    wifi_error_e retValue = wifi_initialize();
    if (WIFI_ERROR_NONE != retValue)
    {
        OIC_LOG(ERROR, IP_MONITOR_TAG, "wifi_initialize failed");
        return CA_STATUS_FAILED;
    }

    g_networkMonitorContext = (CAIPNetworkMonitorContext *)OICCalloc(1,
                              sizeof(*g_networkMonitorContext));
    if (!g_networkMonitorContext)
    {
        OIC_LOG(ERROR, IP_MONITOR_TAG, "g_networkMonitorContext Malloc  failed");
        ca_mutex_unlock(g_networkMonitorContextMutex);
        CADestroyNetworkMonitorMutexes();
        return CA_MEMORY_ALLOC_FAILED;
    }

    g_networkMonitorContext->netInterfaceList = u_arraylist_create();
    if (!g_networkMonitorContext->netInterfaceList)
    {
        OIC_LOG(ERROR, IP_MONITOR_TAG, "u_arraylist_create failed");
        OICFree(g_networkMonitorContext);
        ca_mutex_unlock(g_networkMonitorContextMutex);
        CADestroyNetworkMonitorMutexes();
        return CA_MEMORY_ALLOC_FAILED;
    }

    CAIPGetInterfaceInformation(&g_networkMonitorContext->netInterfaceList);

    ca_mutex_unlock(g_networkMonitorContextMutex);

    OIC_LOG(DEBUG, IP_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

void CAIPTerminateNetworkMonitor()
{
    OIC_LOG(DEBUG, IP_MONITOR_TAG, "IN");

    ca_mutex_lock(g_networkMonitorContextMutex);

     // Deinitialize Wifi service
    wifi_error_e ret = wifi_deinitialize();
    if (WIFI_ERROR_NONE != ret)
    {
        OIC_LOG(ERROR, IP_MONITOR_TAG, "wifi_deinitialize failed");
    }

    CAClearNetInterfaceInfoList(g_networkMonitorContext->netInterfaceList);

    g_networkMonitorContext->netInterfaceList = NULL;
    g_networkMonitorContext->nwConnectivityStatus = CA_INTERFACE_DOWN;
    g_networkMonitorContext->networkChangeCb = NULL;

    OICFree(g_networkMonitorContext);
    g_networkMonitorContext = NULL;

    ca_mutex_unlock(g_networkMonitorContextMutex);

    CADestroyNetworkMonitorMutexes();

    OIC_LOG(DEBUG, IP_MONITOR_TAG, "OUT");
}

CAResult_t CAIPStartNetworkMonitor()
{
    OIC_LOG(DEBUG, IP_MONITOR_TAG, "IN");

     // Set callback for receiving state changes
    wifi_error_e ret = wifi_set_device_state_changed_cb(CAWIFIDeviceStateChangedCb, NULL);
    if (WIFI_ERROR_NONE != ret)
    {
        OIC_LOG(ERROR, IP_MONITOR_TAG, "wifi_set_device_state_changed_cb failed");
        return CA_STATUS_FAILED;
    }

    // Set callback for receiving connection state changes
    ret = wifi_set_connection_state_changed_cb(CAWIFIConnectionStateChangedCb, NULL);
    if (WIFI_ERROR_NONE != ret)
    {
        OIC_LOG(ERROR, IP_MONITOR_TAG, "wifi_set_connection_state_changed_cb failed");
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, IP_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAIPStopNetworkMonitor()
{
    OIC_LOG(DEBUG, IP_MONITOR_TAG, "IN");

     // Reset callback for receiving state changes
    wifi_error_e ret = wifi_unset_device_state_changed_cb();
    if (WIFI_ERROR_NONE != ret)
    {
        OIC_LOG(ERROR, IP_MONITOR_TAG, "wifi_unset_device_state_changed_cb failed");
    }

    // Reset callback for receiving connection state changes
    ret = wifi_unset_connection_state_changed_cb();
    if (WIFI_ERROR_NONE != ret)
    {
        OIC_LOG(ERROR, IP_MONITOR_TAG, "wifi_unset_connection_state_changed_cb failed");
    }

    OIC_LOG(DEBUG, IP_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAIPGetInterfaceInfo(u_arraylist_t **netInterfaceList)
{
    OIC_LOG(DEBUG, IP_MONITOR_TAG, "IN");

    VERIFY_NON_NULL(netInterfaceList, IP_MONITOR_TAG, "u_array_list is null");
    VERIFY_NON_NULL(g_networkMonitorContext, IP_MONITOR_TAG,
                    "g_networkMonitorContext is null");
    VERIFY_NON_NULL(g_networkMonitorContextMutex, IP_MONITOR_TAG,
                    "g_networkMonitorContextMutex is null");

    // Get the interface and ipaddress information from cache
    ca_mutex_lock(g_networkMonitorContextMutex);
    if (!g_networkMonitorContext->netInterfaceList
        || !(u_arraylist_length(g_networkMonitorContext->netInterfaceList)))
    {
        OIC_LOG(ERROR, IP_MONITOR_TAG, "Network not enabled");
        ca_mutex_unlock(g_networkMonitorContextMutex);
        return CA_ADAPTER_NOT_ENABLED;
    }

    uint32_t list_index = 0;
    uint32_t list_length = u_arraylist_length(g_networkMonitorContext->netInterfaceList);
    OIC_LOG_V(DEBUG, IP_MONITOR_TAG, "CAIPGetInterfaceInfo list length [%d]",
              list_length);
    for (list_index = 0; list_index < list_length; list_index++)
    {
        CANetInfo_t *info = (CANetInfo_t *)u_arraylist_get(
                            g_networkMonitorContext->netInterfaceList, list_index);
        if (!info)
        {
            continue;
        }
        OIC_LOG_V(DEBUG, IP_MONITOR_TAG, "CAIPGetInterfaceInfo ip [%s]",
                  info->ipAddress);
        CANetInfo_t *newNetinfo = (CANetInfo_t *) OICMalloc(sizeof(CANetInfo_t));
        if (!newNetinfo)
        {
            OIC_LOG(ERROR, IP_MONITOR_TAG, "Malloc failed!");
            ca_mutex_unlock(g_networkMonitorContextMutex);
            return CA_MEMORY_ALLOC_FAILED;
        }

        memcpy(newNetinfo, info, sizeof(*info));

        CAResult_t result = u_arraylist_add(*netInterfaceList, (void *)newNetinfo);
        if (CA_STATUS_OK != result)
        {
            OIC_LOG(ERROR, IP_MONITOR_TAG, "u_arraylist_add failed!");
            ca_mutex_unlock(g_networkMonitorContextMutex);
            return CA_STATUS_FAILED;
        }
    }

    ca_mutex_unlock(g_networkMonitorContextMutex);

    OIC_LOG(DEBUG, IP_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAIPGetInterfaceSubnetMask(const char *ipAddress, char **subnetMask)
{
    OIC_LOG(DEBUG, IP_MONITOR_TAG, "IN");

    VERIFY_NON_NULL(subnetMask, IP_MONITOR_TAG, "subnet mask");
    VERIFY_NON_NULL(ipAddress, IP_MONITOR_TAG, "ipAddress is null");
    VERIFY_NON_NULL(g_networkMonitorContext, IP_MONITOR_TAG,
                    "g_networkMonitorContext is null");
    VERIFY_NON_NULL(g_networkMonitorContextMutex, IP_MONITOR_TAG,
                    "g_networkMonitorContextMutex is null");

    // Get the interface and ipaddress information from cache
    ca_mutex_lock(g_networkMonitorContextMutex);
    if (!g_networkMonitorContext->netInterfaceList
        || !(u_arraylist_length(g_networkMonitorContext->netInterfaceList)))
    {
        OIC_LOG(DEBUG, IP_MONITOR_TAG, "Network not enabled");
        ca_mutex_unlock(g_networkMonitorContextMutex);
        return CA_ADAPTER_NOT_ENABLED;
    }

    uint32_t list_index = 0;
    uint32_t list_length = u_arraylist_length(g_networkMonitorContext->netInterfaceList);
    OIC_LOG_V(DEBUG, IP_MONITOR_TAG, "list lenght [%d]", list_length);
    for (list_index = 0; list_index < list_length; list_index++)
    {
        CANetInfo_t *info = (CANetInfo_t *)u_arraylist_get(
                            g_networkMonitorContext->netInterfaceList, list_index);
        if (!info)
        {
            continue;
        }

        if (strncmp(info->ipAddress, ipAddress, strlen(ipAddress)) == 0)
        {
            OIC_LOG_V(DEBUG, IP_MONITOR_TAG,
                      "CAIPGetInterfaceSubnetMask subnetmask is %s", info->subnetMask);
            *subnetMask = OICStrdup(info->subnetMask);
            break;
        }
    }
    ca_mutex_unlock(g_networkMonitorContextMutex);

    OIC_LOG(DEBUG, IP_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

bool CAIPIsConnected()
{
    OIC_LOG(DEBUG, IP_MONITOR_TAG, "IN");

    wifi_connection_state_e connection_state;
    wifi_error_e ret = wifi_get_connection_state(&connection_state);
    if (WIFI_ERROR_NONE != ret)
    {
        OIC_LOG(ERROR, IP_MONITOR_TAG, "Failed to get the Connection State");
        return false;
    }

    if (WIFI_CONNECTION_STATE_DISCONNECTED == connection_state)
    {
        OIC_LOG(DEBUG, IP_MONITOR_TAG, "WIFI is not Connected");
        return false;
    }

    OIC_LOG(DEBUG, IP_MONITOR_TAG, "OUT");
    return true;
}

void CAIPSetConnectionStateChangeCallback(CAIPConnectionStateChangeCallback callback)
{
    OIC_LOG(DEBUG, IP_MONITOR_TAG, "IN");
    if (!g_networkMonitorContextMutex || !g_networkMonitorContext)
    {
        OIC_LOG(ERROR, IP_MONITOR_TAG, "CAIPSetConnectionStateChangeCallback failed");
        return;
    }
    ca_mutex_lock(g_networkMonitorContextMutex);

    g_networkMonitorContext->networkChangeCb = callback;

    ca_mutex_unlock(g_networkMonitorContextMutex);

    OIC_LOG(DEBUG, IP_MONITOR_TAG, "OUT");
}

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

#include <wifi.h>
#include "logger.h"
#include "caadapterutils.h"
#include "umutex.h"
#include "oic_malloc.h"

#define WIFI_MONITOR_TAG "WIFI_MONITOR"


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
 * @var gWifiSubnetMask
 * @brief  Maintains interface subnetmask.
 */
static char *gWifiSubnetMask = NULL;

/**
 * @var gNetworkChangeCb
 * @brief  Maintains network connection state change callback.
 */
static CAWiFiConnectionStateChangeCallback gNetworkChangeCb = NULL;

/**
 * @fn CAWIFIConnectionStateChangedCb
 * @brief  This callback is registered to receive wifi network connection state changes.
 */
static void CAWIFIConnectionStateChangedCb(wifi_connection_state_e state, wifi_ap_h ap,
        void *userData);

/**
 * @fn CAWIFIDeviceStateChangedCb
 * @brief This callback is registered to receive wifi device state changes.
 */
static void CAWIFIDeviceStateChangedCb(wifi_device_state_e state, void *userData);

/**
 * @fn CAWiFiGetInterfaceInformation
 * @brief This methods gets local interface name and IP address information.
 */
static void CAWiFiGetInterfaceInformation(char **interfaceName, char **ipAddress,
        char **subnetMask);

CAResult_t CAWiFiInitializeNetworkMonitor(const u_thread_pool_t threadPool)
{
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "IN");

    // Initialize Wifi service
    wifi_error_e ret = wifi_initialize();
    if (WIFI_ERROR_NONE != ret)
    {
        OIC_LOG_V(ERROR, WIFI_MONITOR_TAG, "wifi_initialize failed");
        return CA_STATUS_FAILED;
    }

    if (!gWifiNetInfoMutex)
    {
        gWifiNetInfoMutex = u_mutex_new();
    }

    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

void CAWiFiTerminateNetworkMonitor(void)
{
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "IN");

    // Deinitialize Wifi service
    wifi_error_e ret = wifi_deinitialize();
    if (WIFI_ERROR_NONE != ret)
    {
        OIC_LOG_V(INFO, WIFI_MONITOR_TAG, "wifi_deinitialize failed");
    }

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

    // Set callback for receiving state changes
    wifi_error_e ret = wifi_set_device_state_changed_cb(CAWIFIDeviceStateChangedCb, NULL);
    if (WIFI_ERROR_NONE != ret)
    {
        OIC_LOG_V(ERROR, WIFI_MONITOR_TAG, "wifi_set_device_state_changed_cb failed");
        return CA_STATUS_FAILED;
    }

    // Set callback for receiving connection state changes
    ret = wifi_set_connection_state_changed_cb(CAWIFIConnectionStateChangedCb, NULL);
    if (WIFI_ERROR_NONE != ret)
    {
        OIC_LOG_V(ERROR, WIFI_MONITOR_TAG, "wifi_set_connection_state_changed_cb failed");
        return CA_STATUS_FAILED;
    }

    CAWiFiGetInterfaceInformation(&gWifiInterfaceName, &gWifiIPAddress, &gWifiSubnetMask);

    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAWiFiStopNetworkMonitor(void)
{
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "IN");

    // Reset callback for receiving state changes
    wifi_error_e ret = wifi_unset_device_state_changed_cb();
    if (WIFI_ERROR_NONE != ret)
    {
        OIC_LOG_V(INFO, WIFI_MONITOR_TAG, "wifi_unset_device_state_changed_cb failed");
    }

    // Reset callback for receiving connection state changes
    ret = wifi_unset_connection_state_changed_cb();
    if (WIFI_ERROR_NONE != ret)
    {
        OIC_LOG_V(INFO, WIFI_MONITOR_TAG, "wifi_unset_connection_state_changed_cb failed");
    }

    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAWiFiGetInterfaceInfo(char **interfaceName, char **ipAddress)
{
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "IN");

    VERIFY_NON_NULL(interfaceName, WIFI_MONITOR_TAG, "interface name holder is NULL");
    VERIFY_NON_NULL(ipAddress, WIFI_MONITOR_TAG, "IP address holder is NULL");

    u_mutex_lock(gWifiNetInfoMutex);

    if (NULL == gWifiInterfaceName || NULL == gWifiIPAddress)
    {
        OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "Network not enabled");
        u_mutex_unlock(gWifiNetInfoMutex);
        return CA_ADAPTER_NOT_ENABLED;
    }

    if (gWifiInterfaceName && strlen(gWifiInterfaceName))
    {
        *interfaceName = strndup(gWifiInterfaceName, strlen(gWifiInterfaceName));
    }

    if (gWifiIPAddress && strlen(gWifiIPAddress))
    {
        *ipAddress = strndup(gWifiIPAddress, strlen(gWifiIPAddress));
    }

    u_mutex_unlock(gWifiNetInfoMutex);

    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAWiFiGetInterfaceSubnetMask(char **subnetMask)
{
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "IN");

    VERIFY_NON_NULL(subnetMask, WIFI_MONITOR_TAG, "subnet mask");

    u_mutex_lock(gWifiNetInfoMutex);
    if (NULL == gWifiSubnetMask)
    {
        OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "There is no subnet mask information!");
        return CA_STATUS_FAILED;
    }

    *subnetMask = (gWifiSubnetMask) ? strndup(gWifiSubnetMask, strlen(gWifiSubnetMask))
                  : NULL;
    u_mutex_unlock(gWifiNetInfoMutex);

    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

bool CAWiFiIsConnected(void)
{
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "IN");

    wifi_connection_state_e connection_state;
    wifi_error_e ret = wifi_get_connection_state(&connection_state);
    if (WIFI_ERROR_NONE != ret)
    {
        OIC_LOG_V(ERROR, WIFI_MONITOR_TAG, "Failed to get the Connection State");
        return false;
    }

    if (WIFI_CONNECTION_STATE_DISCONNECTED == connection_state)
    {
        OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "WIFI is not Connected");
        return false;
    }

    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "OUT");
    return true;
}

void CAWiFiSetConnectionStateChangeCallback(
    CAWiFiConnectionStateChangeCallback callback)
{
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "IN");

    gNetworkChangeCb = callback;
}

void CAWIFIConnectionStateChangedCb(wifi_connection_state_e state, wifi_ap_h ap,
                                    void *userData)
{
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "IN");

    if (WIFI_CONNECTION_STATE_ASSOCIATION == state
        || WIFI_CONNECTION_STATE_CONFIGURATION == state)
    {
        OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "Connection is in Association State");
        return;
    }

    CANetworkStatus_t nwStatus = CA_INTERFACE_DOWN;

    // If Wifi is connected, then get the latest IP from the WIFI Interface
    if (WIFI_CONNECTION_STATE_CONNECTED == state)
    {
        nwStatus = CA_INTERFACE_UP;

        // Get network information
        char *interfaceName = NULL;
        char *ipAddress = NULL;
        char *subnetMask = NULL;
        CAWiFiGetInterfaceInformation(&interfaceName, &ipAddress, &subnetMask);

        // Update the cached network information
        u_mutex_lock(gWifiNetInfoMutex);

        OICFree(gWifiInterfaceName);
        OICFree(gWifiIPAddress);
        OICFree(gWifiSubnetMask);
        gWifiInterfaceName = interfaceName;
        gWifiIPAddress = ipAddress;
        gWifiSubnetMask = subnetMask;

        u_mutex_unlock(gWifiNetInfoMutex);
    }
    else
    {
        nwStatus = CA_INTERFACE_DOWN;
    }

    if (gNetworkChangeCb)
    {
        gNetworkChangeCb(gWifiIPAddress, nwStatus);
    }

    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "OUT");
    return;
}

void CAWIFIDeviceStateChangedCb(wifi_device_state_e state, void *userData)
{
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "IN");

    if (WIFI_DEVICE_STATE_ACTIVATED == state)
    {
        OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "Wifi is in Activated State");
    }
    else
    {
        CAWIFIConnectionStateChangedCb(WIFI_CONNECTION_STATE_DISCONNECTED, NULL, NULL);
        OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "Wifi is in Deactivated State");
    }

    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "OUT");
    return;
}

void CAWiFiGetInterfaceInformation(char **interfaceName, char **ipAddress, char **subnetMask)
{
    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "IN");

    int ret = WIFI_ERROR_NONE;

    if (!interfaceName || !ipAddress || !subnetMask)
    {
        OIC_LOG(ERROR, WIFI_MONITOR_TAG, "Invalid input: interface/ipaddress holder is NULL!");
        return;
    }

    u_mutex_lock(gWifiNetInfoMutex);
    // Get wifi interface name
    if (WIFI_ERROR_NONE != (ret = wifi_get_network_interface_name(interfaceName)))
    {
        OIC_LOG_V(ERROR, WIFI_MONITOR_TAG, "Failed to get interface name! error num [%d]", ret);

        u_mutex_unlock(gWifiNetInfoMutex);
        return;
    }

    // Get wifi connected IP address
    wifi_ap_h accessPoint = NULL;
    if (WIFI_ERROR_NONE != (ret = wifi_get_connected_ap(&accessPoint)))
    {
        OIC_LOG_V(ERROR, WIFI_MONITOR_TAG, "Failed to get access point! error num [%d]",
                  ret);

        OICFree(*interfaceName);
        *interfaceName = NULL;
        u_mutex_unlock(gWifiNetInfoMutex);
        return;
    }

    if (WIFI_ERROR_NONE != (ret = wifi_ap_get_ip_address(accessPoint, WIFI_ADDRESS_FAMILY_IPV4,
                                  ipAddress)))
    {
        OIC_LOG_V(ERROR, WIFI_MONITOR_TAG, "Failed to get interface address! error num [%d]",
                  ret);
        OICFree(*interfaceName);
        *interfaceName = NULL;
        u_mutex_unlock(gWifiNetInfoMutex);
        return;
    }

    if (WIFI_ERROR_NONE != (ret = wifi_ap_get_subnet_mask(accessPoint, WIFI_ADDRESS_FAMILY_IPV4,
                                  subnetMask)))
    {
        OIC_LOG_V(ERROR, WIFI_MONITOR_TAG, "Failed to get interface address! error num [%d]",
                  ret);
        OICFree(*ipAddress);
        OICFree(*interfaceName);
        *ipAddress = NULL;
        *interfaceName = NULL;
        u_mutex_unlock(gWifiNetInfoMutex);
        return;
    }

    u_mutex_unlock(gWifiNetInfoMutex);

    OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "OUT");
}

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

#include "cawifimonitor.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <unistd.h>
#include <wifi.h>
#include "logger.h"
#include "caadapterutils.h"
#include "cawifiadapter.h"
#include "cawifiserver.h"
#include "umutex.h"

#define WIFI_MONITOR_TAG "WIFI_MONITOR"

static int gIsStartServerCalled = 0;
static u_mutex gMutexIsStartServerCalled = NULL;
CANetworkChangeCallback gNetworkChangeCb = NULL;

CAResult_t CAGetInterfaceAddress(char *address, int32_t addrLen)
{
    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "IN");
    VERIFY_NON_NULL(address, WIFI_MONITOR_TAG, "Invalid argument : address is NULL");
    if (addrLen < CA_IPADDR_SIZE)
    {
        OIC_LOG_V(ERROR, WIFI_MONITOR_TAG, "addrLen MUST be atleast %d", CA_IPADDR_SIZE);
        return CA_STATUS_INVALID_PARAM;
    }

    struct ifaddrs *interfaceAddr, *ifAddr;
    int interfaceFamily, name;

    /* Getting All Interfaces*/
    if (getifaddrs(&interfaceAddr) == -1)
    {
        OIC_LOG(ERROR, WIFI_MONITOR_TAG, "Failed to get interface list");
        return CA_STATUS_FAILED;
    }

    for (ifAddr = interfaceAddr; ifAddr != NULL; ifAddr = ifAddr->ifa_next)
    {
        if (ifAddr->ifa_addr == NULL)
            continue;

        /* Currently, check for wifi interface and get the IP Address */
        OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "interface name = %s", ifAddr->ifa_name);
        if (0 == strncmp(ifAddr->ifa_name, "wlan0", strlen("wlan0")))
        {
            interfaceFamily = ifAddr->ifa_addr->sa_family;
            OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "interfaceFamily = %d", interfaceFamily);

            if (interfaceFamily == AF_INET || interfaceFamily == AF_INET6)
            {
                name = getnameinfo(ifAddr->ifa_addr,
                                   (interfaceFamily == AF_INET) ? sizeof(struct sockaddr_in) :
                                   sizeof(struct sockaddr_in6),
                                   address, CA_IPADDR_SIZE,
                                   NULL, 0, NI_NUMERICHOST);
                if (name != 0)
                {
                    OIC_LOG_V(ERROR, WIFI_MONITOR_TAG, "Failed to get IPAddress, Error code: %s",
                              strerror(errno));
                    return CA_STATUS_FAILED;
                }
                OIC_LOG_V(DEBUG, WIFI_MONITOR_TAG, "interface name = %s", address);
                break;
            }
        }
    }
    freeifaddrs(interfaceAddr);
    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAInitializeWIFIAdapter()
{
    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "IN");
    wifi_initialize();
    gMutexIsStartServerCalled = u_mutex_new();
    wifi_error_e ret = WIFI_ERROR_INVALID_PARAMETER;
    wifi_set_device_state_changed_cb(CAWIFIDeviceStateChangedCb, NULL);
    ret = wifi_set_connection_state_changed_cb(CAWIFIConnectionStateChangedCb, NULL);
    if (ret != WIFI_ERROR_NONE)
    {
        OIC_LOG(ERROR, WIFI_MONITOR_TAG, "wifi_set_connection_state_changed_cb failed");
        return CA_STATUS_FAILED;
    }
    else
    {
        OIC_LOG(INFO, WIFI_MONITOR_TAG, "wifi_set_connection_state_changed_cb sucess");
    }
    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CADeinitializeWIFIAdapter()
{
    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "IN");
    wifi_error_e ret = WIFI_ERROR_INVALID_PARAMETER;
    wifi_unset_device_state_changed_cb();
    ret = wifi_unset_connection_state_changed_cb();
    if (ret != WIFI_ERROR_NONE)
    {
        OIC_LOG(INFO, WIFI_MONITOR_TAG, "wifi_unset_device_state_changed_cb failed");
    }
    else
    {
        OIC_LOG(INFO, WIFI_MONITOR_TAG, "wifi_unset_device_state_changed_cb success");
    }
    wifi_deinitialize();
    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "OUT");
    return CA_STATUS_OK;
}

CABool_t CAIsWIFIConnected()
{
    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "IN");
    wifi_connection_state_e connection_state;
    int retVal = wifi_get_connection_state(&connection_state);
    if (retVal != WIFI_ERROR_NONE)
    {
        OIC_LOG(ERROR, WIFI_MONITOR_TAG, "Failed to get the Connection State");
        return CA_FALSE;
    }
    if (connection_state == WIFI_CONNECTION_STATE_DISCONNECTED)
    {
        OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "WIFI is not Connected");
        return CA_FALSE;
    }
    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "WIFI is Connected");
    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "OUT");
    return CA_TRUE;
}

void CAWIFIConnectionStateChangedCb(wifi_connection_state_e state, wifi_ap_h ap, void *userData)
{
    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "IN");
    if (state == WIFI_CONNECTION_STATE_ASSOCIATION
        || WIFI_CONNECTION_STATE_CONFIGURATION == state)
    {
        OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "Connection is in Association State");
        return;
    }
    CANetworkStatus_t nwConnectivityStatus = CA_INTERFACE_DOWN;
    CALocalConnectivity_t *localEndpoint = NULL;
    uint32_t size = 0;
    /* If Wifi is connected, then get the latest IP from the WIFI Interface */
    if (WIFI_CONNECTION_STATE_CONNECTED == state)
    {
        if (gIsStartServerCalled)
        {
            CAStartWIFI();
        }
        nwConnectivityStatus = CA_INTERFACE_UP;
        CAGetWIFIInterfaceInformation(&localEndpoint, &size);
    }
    else
    {
        localEndpoint = CAAdapterCreateLocalEndpoint(CA_WIFI, "", NULL);
        CAStopUnicastServer();
    }

    gNetworkChangeCb(localEndpoint, nwConnectivityStatus);

    CAAdapterFreeLocalEndpoint(localEndpoint);

    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "OUT");
    return;
}

void CAWIFIDeviceStateChangedCb(wifi_device_state_e state, void *userData)
{
    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "IN");

    if (WIFI_DEVICE_STATE_ACTIVATED == state)
    {
        OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "Wifi is in Activated State");
    }
    else
    {
        CAWIFIConnectionStateChangedCb(WIFI_CONNECTION_STATE_DISCONNECTED, NULL, NULL);
        OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "Wifi is in Deactivated State");
    }
    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "OUT");
    return;
}

void CASetIsStartServerInvoked()
{
    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "IN");
    u_mutex_lock(gMutexIsStartServerCalled);
    gIsStartServerCalled = 1;
    u_mutex_unlock(gMutexIsStartServerCalled);
    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "OUT");
}

void CAUnsetIsStartServerInvoked()
{
    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "IN");
    u_mutex_lock(gMutexIsStartServerCalled);
    gIsStartServerCalled = 0;
    u_mutex_unlock(gMutexIsStartServerCalled);
    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "OUT");
}

void CASetWIFINetworkChangeCallback(CANetworkChangeCallback netCallback)
{
    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "IN");
    gNetworkChangeCb = netCallback;
    OIC_LOG(DEBUG, WIFI_MONITOR_TAG, "OUT");
}
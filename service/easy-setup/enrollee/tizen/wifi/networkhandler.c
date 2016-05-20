/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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

#include "networkhandler.h"

#include <wifi.h>
#include <unistd.h>

#include "logger.h"
#include "easysetup.h"
#include "oic_string.h"

#define LOG_TAG "TIZEN ES"

const char *gSsid = "DLNA_LISMORE1";
const char *gPass = "dlna@010203";
char *gIpAddress = NULL;
wifi_ap_h connectedWifi;
ESEnrolleeNetworkEventCallback gNetworkEventCb;
static void ESActivateWifi();

static const char*
print_state(wifi_connection_state_e state)
{
    switch (state)
    {
        case WIFI_CONNECTION_STATE_DISCONNECTED:
            return "Disconnected";
        case WIFI_CONNECTION_STATE_ASSOCIATION:
            return "Association";
        case WIFI_CONNECTION_STATE_CONNECTED:
            return "Connected";
        case WIFI_CONNECTION_STATE_CONFIGURATION:
            return "Configuration";
    }
}

void __wifi_connected_cb(wifi_error_e error_code, void *user_data)
{
    OIC_LOG(INFO,LOG_TAG,"#### __connected ");

    if (gIpAddress)
        free(gIpAddress);

    wifi_ap_get_ip_address(connectedWifi, WIFI_ADDRESS_FAMILY_IPV4, &gIpAddress);
    OIC_LOG_V(INFO,LOG_TAG,"#### __connected, Ipaddress=%s", gIpAddress);
    gNetworkEventCb(ES_OK);

}

bool __wifi_found_ap_cb(wifi_ap_h ap, void *user_data)
{
    OIC_LOG(INFO,LOG_TAG,"#### __wifi_found_ap_cb received ");

    int error_code = 0;
    char *ap_name = NULL;
    wifi_connection_state_e state;

    error_code = wifi_ap_get_essid(ap, &ap_name);
    if (error_code != WIFI_ERROR_NONE)
    {
        OIC_LOG(ERROR,LOG_TAG,"#### Fail to get AP name.");

        return false;
    }
    error_code = wifi_ap_get_connection_state(ap, &state);
    if (error_code != WIFI_ERROR_NONE)
    {
        OIC_LOG(ERROR,LOG_TAG,"#### Fail to get state.");
        free(ap_name);

        return false;
    }
    OIC_LOG_V(INFO,LOG_TAG,"#### AP name : %s, state : %s", ap_name, print_state(state));

    if (strcmp(ap_name, gSsid) == 0)
    {
        OIC_LOG(INFO,LOG_TAG,"#### network found");
        wifi_ap_set_passphrase(ap, gPass);
        connectedWifi = ap;
        error_code = wifi_connect(ap, __wifi_connected_cb, NULL);
        OIC_LOG_V(INFO,LOG_TAG,"Code=%d", error_code);
    }
    OIC_LOG(INFO,LOG_TAG,"#### __wifi_found_ap_cb received ");
    free(ap_name);
    return true;
}
void __scan_request_cb(wifi_error_e error_code, void *user_data)
{
    OIC_LOG(INFO, LOG_TAG, "__scan_request_cb");
    int error_code1;
    error_code1 = wifi_foreach_found_aps(__wifi_found_ap_cb, NULL);
    if (error_code1 != WIFI_ERROR_NONE)
        OIC_LOG(INFO,LOG_TAG,"#### Fail to scan");

    OIC_LOG(INFO, LOG_TAG,"#### __scan_request_cb exit ");
}

static void __wifi_activated_cb(wifi_error_e result, void *user_data)
{
    OIC_LOG(INFO, LOG_TAG, "__wifi_activated_cb");
    if (result == WIFI_ERROR_NONE)
    {
        OIC_LOG(INFO,LOG_TAG,"#### Success to activate Wi-Fi device!");
    }
    wifi_scan(__scan_request_cb, NULL);

}
static void ESActivateWifi()
{
    int error_code;
    error_code = wifi_initialize();
    OIC_LOG_V(INFO,LOG_TAG,"#### WIFI INITIALIZED WITH STATUS :%d", error_code);

    error_code = wifi_activate(__wifi_activated_cb, NULL);
    OIC_LOG_V(INFO,LOG_TAG,"#### WIFI ACTIVATED WITH STATUS :%d", error_code);

    bool wifi_activated = false;
    wifi_is_activated(&wifi_activated);
    if (wifi_activated)
    {
        OIC_LOG(INFO,LOG_TAG,"#### Success to get Wi-Fi device state.");
        int scan_result = wifi_scan(__scan_request_cb, NULL);
        OIC_LOG_V(INFO,LOG_TAG,"#### Wifi scan result:%d", scan_result);
    }
    else
    {
        OIC_LOG(ERROR,LOG_TAG, "#### Fail to get Wi-Fi device state.");
    }
}

static void start()
{
    OIC_LOG(INFO, LOG_TAG, "START");
    ESActivateWifi();
}

void ConnectToWiFiNetwork(const char *ssid, const char *pass,
                                                            ESEnrolleeNetworkEventCallback cb)
{
    OIC_LOG_V(INFO, LOG_TAG, "ConnectToWiFiNetwork %s %s",ssid,pass);
    gPass = pass;
    gSsid = ssid;
    gNetworkEventCb = cb;
    start();
}

ESResult getCurrentNetworkInfo(OCConnectivityType targetType, NetworkInfo *info)
{
    if (targetType == CT_ADAPTER_IP)
    {
        info->type = CT_ADAPTER_IP;
        info->ipaddr = gIpAddress;
        if (strlen(gSsid) <= MAXSSIDLEN)
        {
            OICStrcpy(info->ssid, sizeof(info->ssid), gSsid);
            return ES_OK;
        }
        else
        {
            return ES_ERROR;
        }
    }

    return ES_ERROR;
}


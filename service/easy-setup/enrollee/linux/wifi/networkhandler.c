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

#include <unistd.h>

#include "logger.h"
#include "easysetup.h"
#include "oic_string.h"

#define LOG_TAG "LINUX ES"

const char *gSsid = "DLNA_LISMORE1";
const char *gPass = "dlna@010203";
char *gIpAddress;
ESEnrolleeNetworkEventCallback gNetworkEventCb;

/*
 * All the functions defined in this file are stub functions to be implemented by the end user of
 * Easysetup Enrollee applications.
 */
static void ESActivateWifi()
{

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


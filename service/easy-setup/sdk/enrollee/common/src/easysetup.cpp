//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include "easysetup.h"

/// WiFi network info and credentials
char defaultSsid[] = "EasyConnect";
char defaultPass[] = "EasyConnect";

int g_eventflag = 0;
int g_cnt = 0;
char *targetSsid;
char *targetPass;

EventCallback g_cbForProvisioning = NULL;
EventCallback g_cbForOnboarding = NULL;

void EventCallbackInOnboarding(ES_RESULT event);
void EventCallbackInProvisioning(ES_RESULT event);
void EventCallbackAfterProvisioning(ES_RESULT event);

void EventCallbackInOnboarding(ES_RESULT event)
{
    if (event == ES_NETWORKFOUND || event == ES_NETWORKCONNECTED)
    {
        if (g_cbForOnboarding != NULL)
        {
            g_cbForOnboarding(event);
        }
    }
}

void EventCallbackInProvisioning(ES_RESULT event)
{
    ES_RESULT res = ES_OK;

    if (event == ES_RECVTRIGGEROFPROVRES)
    {
        targetSsid = (char *) malloc(MAXSSIDLEN);
        targetPass = (char *) malloc(MAXNETCREDLEN);

        GetTargetNetworkInfoFromProvResource(targetSsid, targetPass);

        res = ConnectToWiFiNetwork(targetSsid, targetPass, EventCallbackAfterProvisioning);

        if (g_cbForProvisioning != NULL)
        {
            g_cbForProvisioning(res);
        }
    }
}

void EventCallbackAfterProvisioning(ES_RESULT event)
{
    if (event == ES_NETWORKFOUND || event == ES_NETWORKCONNECTED)
    {
        if (g_cbForProvisioning != NULL)
        {
            g_cbForProvisioning(event);
        }
    }
}

ES_RESULT FindNetworkForOnboarding(NetworkType networkType, EventCallback cb)
{
    if (networkType == ES_WIFI)
    {
        if (g_cbForOnboarding == NULL)
        {
            g_cbForOnboarding = cb;
        }

        return ConnectToWiFiNetwork(defaultSsid, defaultPass, EventCallbackInOnboarding);
    }
}

ES_RESULT FindNetworkForOnboarding(NetworkType networkType, const char *ssid, const char *passwd,
        EventCallback cb)
{
    if (!ssid || !passwd)
    {
        return ES_ERROR;
    }

    if (networkType == ES_WIFI)
    {
        if (g_cbForOnboarding == NULL)
        {
            g_cbForOnboarding = cb;
        }

        return ConnectToWiFiNetwork(ssid, passwd, EventCallbackInOnboarding);
    }
}

ES_RESULT InitializeProvisioning(EventCallback cb)
{
    if (cb == NULL)
    {
        return ES_ERROR;
    }
    else
    {
        g_cbForProvisioning = cb;
    }

    if (CreateProvisioningResource() != OC_STACK_OK)
    {
        return ES_ERROR;
    }

    if (CreateNetworkResource() != OC_STACK_OK)
    {
        return ES_ERROR;
    }

    RegisterResourceEventCallBack(EventCallbackInProvisioning);

    return ES_RESOURCECREATED;
}


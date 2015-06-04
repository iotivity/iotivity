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

PROGMEM const char TAG[] = "ArduinoServer";

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
    Serial.println("[Callback] in onboarding process");

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

    Serial.println("[Callback] in provisioning process");

    if (event == ES_RECVTRIGGEROFPROVRES)
    {
        targetSsid = (char *) malloc(MAXSSIDLEN);
        targetPass = (char *) malloc(MAXNETCREDLEN);

        getTargetNetworkInfoFromProvResource(targetSsid, targetPass);

        res = ConnectToWiFiNetworkForOnboarding(targetSsid, targetPass,
                EventCallbackAfterProvisioning);

        if (g_cbForProvisioning != NULL)
        {
            g_cbForProvisioning(res);
        }
    }
}

void EventCallbackAfterProvisioning(ES_RESULT event)
{
    Serial.println("[Callback] after provisioning process");

    if (event == ES_NETWORKFOUND || event == ES_NETWORKCONNECTED)
    {
        if (g_cbForProvisioning != NULL)
        {
            g_cbForProvisioning(event);
        }
    }
}

ES_RESULT WaitingForOnboarding(NetworkType networkType, EventCallback cb)
{
    if (networkType == ES_WIFI)
    {
        if (g_cbForOnboarding == NULL)
        {
            g_cbForOnboarding = cb;
        }

        return ConnectToWiFiNetworkForOnboarding(defaultSsid, defaultPass,
                EventCallbackInOnboarding);
    }
}

ES_RESULT PrepareToProvisioning(EventCallback cb)
{
    if (cb == NULL)
    {
        return ES_ERROR;
    }
    else
    {
        g_cbForProvisioning = cb;
    }

    if (createProvisioningResource() != OC_STACK_OK)
    {
        return ES_ERROR;
    }

    if (createNetworkResource() != OC_STACK_OK)
    {
        return ES_ERROR;
    }

    RegisterResourceEventCallBack(EventCallbackInProvisioning);

    return ES_RESOURCECREATED;
}


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

#include "logger.h"
#include "resourceHandler.h"

#define TAG "ES"

int g_eventflag = 0;
int g_cnt = 0;
char *targetSsid;
char *targetPass;

EventCallback g_cbForProvisioning = NULL;
EventCallback g_cbForOnboarding = NULL;

void EventCallbackInOnboarding(ESResult event);
void EventCallbackInProvisioning(ESResult event);
void EventCallbackAfterProvisioning(ESResult event);

void EventCallbackInOnboarding(ESResult event)
{
    if (event == ES_NETWORKFOUND || event == ES_NETWORKCONNECTED)
    {
        if (g_cbForOnboarding != NULL)
        {
            g_cbForOnboarding(event);
        }
    }
}

void EventCallbackInProvisioning(ESResult event)
{
    ESResult res = ES_OK;

    if (event == ES_RECVTRIGGEROFPROVRES)
    {
        targetSsid = (char *) malloc(MAXSSIDLEN);
        targetPass = (char *) malloc(MAXNETCREDLEN);

        if(TerminateEasySetup() != OC_STACK_OK)
        {
            OC_LOG(ERROR, TAG, "Terminating stack failed");
            return;
        }

        GetTargetNetworkInfoFromProvResource(targetSsid, targetPass);

        res = ConnectToWiFiNetwork(targetSsid, targetPass, EventCallbackAfterProvisioning);

        if (g_cbForProvisioning != NULL)
        {
            g_cbForProvisioning(res);
        }
    }
}

void EventCallbackAfterProvisioning(ESResult event)
{
    if (event == ES_NETWORKFOUND || event == ES_NETWORKCONNECTED)
    {
        if (g_cbForProvisioning != NULL)
        {
            g_cbForProvisioning(event);
        }
    }
}

ESResult FindNetworkForOnboarding(NetworkType networkType,
                                           const char *ssid,
                                           const char *passwd,
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

        if(ConnectToWiFiNetwork(ssid, passwd, EventCallbackInOnboarding) != ES_NETWORKCONNECTED)
        {
            OC_LOG(ERROR, TAG, "ConnectToWiFiNetwork Failed");
            return ES_ERROR;
        }
        else{
            OC_LOG(INFO, TAG, "ConnectToWiFiNetwork Success");
            return ES_OK;
        }
    }
}


ESResult InitEasySetup(NetworkType networkType, const char *ssid, const char *passwd,
              EventCallback cb)
{
    if(FindNetworkForOnboarding(networkType, ssid, passwd, cb) != ES_OK)
    {
        OC_LOG(ERROR, TAG, "OnBoarding Failed");
        return ES_ERROR;
    }

    // Initialize the OC Stack in Server mode
    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, "OCStack init error");
        return ES_ERROR;
    }
    else
    {
        OC_LOG(DEBUG, TAG, "OCStack init success");
        return ES_OK;
    }
}

ESResult TerminateEasySetup()
{
    if(OCStop() != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, "OCStack stop failed");
        return ES_ERROR;
    }
    else
    {
        OC_LOG(ERROR, TAG, "OCStack stop success");
        return ES_OK;
    }
}

ESResult InitProvisioning(EventCallback cb)
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


//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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

        if (TerminateEasySetup() != OC_STACK_OK)
        {
            OC_LOG(ERROR, TAG, "Terminating stack failed");
            return;
        }

        GetTargetNetworkInfoFromProvResource(targetSsid, targetPass);

        OC_LOG(INFO, TAG, targetSsid);
        OC_LOG(INFO, TAG, targetPass);
        OC_LOG(INFO, TAG, "Device Provisioned");

        // cannot connect to target network, as currently arduino supports only one transport at a time
        //res = ConnectToWiFiNetwork(targetSsid, targetPass, EventCallbackAfterProvisioning);

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

ESResult InitEasySetup(NetworkType networkType, EventCallback cb)
{

    // Initialize the OC Stack in Server mode
    //if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    if (OCInit1(OC_SERVER, OC_DEFAULT_FLAGS, OC_DEFAULT_FLAGS) != OC_STACK_OK)
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
    if (OCStop() != OC_STACK_OK)
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

    RegisterResourceEventCallBack( EventCallbackInProvisioning);

    return ES_RESOURCECREATED;
}


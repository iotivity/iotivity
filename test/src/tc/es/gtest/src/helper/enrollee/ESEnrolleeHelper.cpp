/******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
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
 *
 ******************************************************************/

#include "ESEnrolleeHelper.h"
#include <iostream>
#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif
#include <stdio.h>
#include <stdlib.h>

bool ESEnrolleeHelper::isCallbackInvoked = false;
const int g_timeout = 30;
ESEnrolleeHelper m_esEnrolleeHelper;
ESEnrolleeHelper::ESEnrolleeHelper()
{
}


void ESEnrolleeHelper::waitForResponse()
{
    void* ret = NULL;
    pthread_t callbackListiningThread;
    pthread_create(&callbackListiningThread, NULL, listeningFunc, NULL);
    pthread_join(callbackListiningThread, &ret);
}

void* ESEnrolleeHelper::listeningFunc(void *arg)
{
    int second = 0;
    OCStackResult result = OC_STACK_ERROR;
    IOTIVITYTEST_LOG(INFO,"\nWaiting For the Response...........................\n");

    while (!ESEnrolleeHelper::isCallbackInvoked)
    {
        result = OCProcess();
        if (result != OC_STACK_OK)
        {
            IOTIVITYTEST_LOG(INFO,"OCStack stop error\n");
        }
        CommonUtil::waitInSecond(1);
        if (++second == g_timeout)
        {
            second = 0;
            IOTIVITYTEST_LOG(INFO,"\nTimeout For Response!Please Try Again\n\n");
            break;
        }
    }

    IOTIVITYTEST_LOG(INFO,"Callback is successfully called\n");
    pthread_exit (NULL);
}

void ESEnrolleeHelper::wiFiProvCbInApp(ESWiFiConfData *eventData)
{
    IOTIVITYTEST_LOG(INFO,"wiFiProvCbInApp IN\n");
    ESEnrolleeHelper::isCallbackInvoked = true;

    if (eventData->ssid != NULL)
    {
        IOTIVITYTEST_LOG(INFO,"SSID : %s\n", eventData->ssid);
    }
    else
    {
        IOTIVITYTEST_LOG(INFO,"ERROR! SSID IS NULL\n");
    }

    if (eventData->pwd != NULL)
    {
        IOTIVITYTEST_LOG(INFO,"Password : %s\n", eventData->pwd);
    }
    else
    {
        IOTIVITYTEST_LOG(INFO,"ERROR! Password IS NULL\n");
    }

    if (eventData->authtype == NONE_AUTH || eventData->authtype == WEP
            || eventData->authtype == WPA_PSK || eventData->authtype == WPA2_PSK)
    {
        IOTIVITYTEST_LOG(INFO,"AuthType : %d\n", eventData->authtype);
    }
    else
    {
        IOTIVITYTEST_LOG(INFO,"ERROR! AuthType IS NULL\n");
    }

    if (eventData->enctype == NONE_ENC || eventData->enctype == WEP_64
            || eventData->enctype == WEP_128 || eventData->enctype == TKIP
            || eventData->enctype == AES || eventData->enctype == TKIP_AES)
    {
        IOTIVITYTEST_LOG(INFO,"EncType : %d\n", eventData->enctype);
    }
    else
    {
        IOTIVITYTEST_LOG(INFO,"ERROR! EncType IS NULL\n");
    }

    if (eventData->userdata != NULL)
    {
    }
    IOTIVITYTEST_LOG(INFO,"enven data User", eventData->userdata);

    IOTIVITYTEST_LOG(INFO,"wiFiProvCbInApp OUT\n");
}

void ESEnrolleeHelper::devConfProvCbInApp(ESDevConfData *eventData)
{
    IOTIVITYTEST_LOG(INFO,"devConfProvCbInApp IN\n");
    ESEnrolleeHelper::isCallbackInvoked = true;

    IOTIVITYTEST_LOG(INFO,"enven data User", eventData->userdata);

    IOTIVITYTEST_LOG(INFO,"devConfProvCbInApp OUT\n");
}

void ESEnrolleeHelper::cloudDataProvCbInApp(ESCoapCloudConfData *eventData)
{
    IOTIVITYTEST_LOG(INFO,"cloudDataProvCbInApp IN\n");
    ESEnrolleeHelper::isCallbackInvoked = true;
    if (eventData->authProvider != NULL)
    {
        IOTIVITYTEST_LOG(INFO,"AuthProvider : %s\n", eventData->authProvider);
    }
    else
    {
        IOTIVITYTEST_LOG(INFO,"ERROR! AuthProvider IS NULL\n");
    }

    if (eventData->ciServer != NULL)
    {
        IOTIVITYTEST_LOG(INFO,"CI Server : %s\n", eventData->ciServer);
    }
    else
    {
        IOTIVITYTEST_LOG(INFO,"ERROR! CI Server IS NULL\n");
    }

    IOTIVITYTEST_LOG(INFO,"cloudDataProvCbInApp OUT\n");
}

ESResult ESEnrolleeHelper::setDeviceProperty()
{
    ESDeviceProperty deviceProperty =
    {
    {
    { WIFI_11G, WIFI_11N, WIFI_11AC }, 3,
    { WIFI_24G, WIFI_5G }, 2,
    { WPA_PSK, WPA2_PSK }, 2,
    { AES, TKIP_AES }, 2
    },
    { "Test Device" } };

    return ESSetDeviceProperty(&deviceProperty);
}
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
 *      LICENSE-2.0" target="_blank">http://www.apache.org/licenses/LICENSE-2.0
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
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

bool ESEnrolleeHelper::isCallbackInvoked = false;
const int g_timeout = 15;

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
            IOTIVITYTEST_LOG(INFO,"\nTimeout For Response!Please Try Again\n\n");
            break;
        }
    }

    IOTIVITYTEST_LOG(INFO,"Callback is successfully called\n");
    pthread_exit (NULL);
}

void ESEnrolleeHelper::wiFiProvCbInApp(ESWiFiProvData *eventData)
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

void ESEnrolleeHelper::devConfProvCbInApp(ESDevConfProvData *eventData)
{
    IOTIVITYTEST_LOG(INFO,"devConfProvCbInApp IN\n");
    ESEnrolleeHelper::isCallbackInvoked = true;

    if (eventData->language != NULL)
    {
        IOTIVITYTEST_LOG(INFO,"Language : %s\n", eventData->language);
    }
    else
    {
        IOTIVITYTEST_LOG(INFO,"ERROR! Language IS NULL\n");
    }

    if (eventData->country != NULL)
    {
        IOTIVITYTEST_LOG(INFO,"Country : %s\n", eventData->country);
    }
    else
    {
        IOTIVITYTEST_LOG(INFO,"ERROR! Country IS NULL\n");
    }
    IOTIVITYTEST_LOG(INFO,"enven data User", eventData->userdata);

    IOTIVITYTEST_LOG(INFO,"devConfProvCbInApp OUT\n");
}

void ESEnrolleeHelper::cloudDataProvCbInApp(ESCloudProvData *eventData)
{
    IOTIVITYTEST_LOG(INFO,"cloudDataProvCbInApp IN\n");
    ESEnrolleeHelper::isCallbackInvoked = true;
    if (eventData->authCode != NULL)
    {
        IOTIVITYTEST_LOG(INFO,"AuthCode : %s\n", eventData->authCode);
    }
    else
    {
        IOTIVITYTEST_LOG(INFO,"ERROR! AuthCode IS NULL\n");
    }

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
    { WIFI_11G, WiFi_EOF }, WIFI_5G },
    { "Test Device" } };

    return ESSetDeviceProperty(&deviceProperty);
}


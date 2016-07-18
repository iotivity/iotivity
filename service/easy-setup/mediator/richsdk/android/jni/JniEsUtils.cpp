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
 ******************************************************************/

#include "JniEsUtils.h"

void throwESException(JNIEnv *env, std::string reason)
{
    jobject ex = env->NewObject(g_cls_ESException, g_mid_ESException_ctor,
                                env->NewStringUTF(reason.c_str()));
    if (!ex)
    {
        LOGI("throwException : jobject is NULL");
    }
    env->Throw((jthrowable)ex);
}

OCConnectivityType  getOCConnectivityTypeFromInt(int connectivityType)
{
    switch (connectivityType)
    {
        case CT_DEFAULT:
            return CT_DEFAULT;
        case CT_ADAPTER_IP:
            return CT_ADAPTER_IP;

            //May be need to add more types, if easy setup service support more transport.
    }
    return CT_DEFAULT;
}

WIFI_AUTHTYPE getWifiAuthTypeFromInt(int authType)
{
    switch (authType)
    {
        case 0:
            return WIFI_AUTHTYPE::NONE_AUTH;
        case 1:
            return WIFI_AUTHTYPE::WEP;
        case 2:
            return WIFI_AUTHTYPE::WPA_PSK;
        case 3:
            return WIFI_AUTHTYPE::WPA2_PSK;
    }
    return NONE_AUTH;
}

WIFI_ENCTYPE getWifiEncTypeFromInt(int encType)
{
    switch (encType)
    {
        case 0:
            return WIFI_ENCTYPE::NONE_ENC;
        case 1:
            return WIFI_ENCTYPE::WEP_64;
        case 2:
            return WIFI_ENCTYPE::WEP_128;
        case 3:
            return WIFI_ENCTYPE::TKIP;
        case 4:
            return WIFI_ENCTYPE::AES;
        case 5:
            return WIFI_ENCTYPE::TKIP_AES;
    }
    return NONE_ENC;
}

int convertNativeWifiFreqToInt(WIFI_FREQ wifiFreq)
{
    switch (wifiFreq)
    {
        case WIFI_FREQ::WIFI_24G:
            return 0;
        case WIFI_FREQ::WIFI_5G:
            return 1;
        case WIFI_FREQ::WIFI_BOTH:
            return 2;
    }
}

int convertNativeWifiModeToInt(WIFI_MODE wifiMode)
{
    switch (wifiMode)
    {
        case WIFI_MODE::WIFI_11A:
            return 0;
        case WIFI_MODE::WIFI_11B:
            return 1;
        case WIFI_MODE::WIFI_11G:
            return 2;
        case WIFI_MODE::WIFI_11N:
            return 3;
        case WIFI_MODE::WIFI_11AC:
            return 4;
    }
}

int convertNativeDeviceProvStateToInt(ESDeviceProvState nativeState)
{
    switch (nativeState)
    {
        case ESDeviceProvState::ES_PROVISIONING_ERROR:
            return -1;
        case ESDeviceProvState::ES_NEED_PROVISIONING:
            return 0;
        case ESDeviceProvState::ES_PROVISIONED_ALREADY:
            return 1;
        case ESDeviceProvState::ES_PROVISIONING_SUCCESS:
            return 2;
    }
}

int convertNativeDeviceProvResultToInt(ESResult nativeResult)
{
    switch (nativeResult)
    {
        case ESResult::ES_ERROR:
            return -1;
        case ESResult::ES_OK:
            return 0;
        case ESResult::ES_NETWORKFOUND:
            return 1;
        case ESResult::ES_NETWORKCONNECTED:
            return 2;
        case ESResult::ES_NETWORKNOTCONNECTED:
            return 3;
        case ESResult::ES_RESOURCECREATED:
            return 11;
        case ESResult::ES_RECVREQOFPROVRES:
            return 21;
        case ESResult::ES_RECVREQOFNETRES:
            return 22;
        case ESResult::ES_RECVUPDATEOFPROVRES:
            return 23;
        case ESResult::ES_RECVTRIGGEROFPROVRES:
            return 24;
        case ESResult::ES_UNAUTHORIZED:
            return 31;
    }
}
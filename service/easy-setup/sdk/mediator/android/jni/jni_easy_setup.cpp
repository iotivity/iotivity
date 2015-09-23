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

#include "jni_easy_setup.h"

#include "jni_easy_setup_jvm.h"
#include "prov_adapter.h"

void JNIProvisioningStatusCallback(ProvisioningInfo * provInfo)
{
    JNIEnv *env = EasySetupJVM::getEnv();
    if (env == NULL)
    {
        LOGE("JNIProvisioningStatusCallback : Getting JNIEnv failed");
        return;
    }

    // Get EasySetupHandler class reference
    jclass easysetupCallbacks = GetJClass(EASY_SETUP_SERVICE_CALLBACK_NATIVE_API_CLASS_PATH);
    if (NULL == easysetupCallbacks)
    {
        LOGE("JNIProvisioningStatusCallback : GetJClass easysetupCallbacks failed");
        EasySetupJVM::releaseEnv();
        return;
    }

    // Get the easysetupCallback class instance
    jobject jobjectCallback = GetJObjectInstance(EASY_SETUP_SERVICE_CALLBACK_NATIVE_API_CLASS_PATH);
    if (NULL == jobjectCallback)
    {
        LOGE("getInstance( %s) failed!", EASY_SETUP_SERVICE_CALLBACK_NATIVE_API_CLASS_PATH);
        EasySetupJVM::releaseEnv();
        return;
    }

    // Get onResourceCallback method reference
    jmethodID method_id = env->GetMethodID(easysetupCallbacks, "ProvisioningStatusCallBack",
            METHOD_PROVISIONING_STATUS_INTEGER_CALLBACK);
    if (NULL == method_id)
    {
        LOGE("JNIProvisioningStatusCallback: onResourceCallback : GetMethodID failed");
        EasySetupJVM::releaseEnv();
        return;
    }

    if ((env)->ExceptionCheck())
    {
        LOGE("JNIProvisioningStatusCallback : ExceptionCheck failed");
        EasySetupJVM::releaseEnv();
        return;
    }

    if (NULL == method_id)
    {
        LOGI("JNI method_id is NULL");
    }
    else
    {
        LOGI("JNI method_id is VALID");

        jint result;
        if (provInfo->provStatus == DEVICE_PROVISIONED)
        {
            result = 0;
        }
        else
        {
            result = -1;
        }

        env->CallVoidMethod(jobjectCallback, method_id, (jint) result);
    }

    EasySetupJVM::releaseEnv();
}

JNIEXPORT void JNICALL
JNIInitEasySetup(JNIEnv
        *env,
        jobject thisObj
)
{
    LOGI("JNI JNIInitEasySetup: Enter");

    InitEasySetupManager();

    RegisterProvisioningStausCallback(JNIProvisioningStatusCallback);
}

JNIEXPORT void JNICALL
JNITerminateEasySetup(JNIEnv
        *env,
        jobject thisObj
)
{
    LOGI("JNI JNITerminateEasySetup: Enter");

    TerminateEasySetupManager();

}

JNIEXPORT void JNICALL
JNIProvisionEnrollee(JNIEnv
        *env,
        jobject thisObj,
        jstring
        jIPAddress,
        jstring jNetSSID,
        jstring
        jNetPWD,
        jint jConnectivityType
)
{
    LOGI("JNI JNIProvisionEnrollee: Enter");

    if (!jIPAddress)
    {
        LOGE("JNI JNIProvisionEnrollee : jIPAddress is NULL!");
        return;
    }

    const char *ipAddress = env->GetStringUTFChars(jIPAddress, NULL);
    if (NULL == ipAddress)
    {
        LOGE("JNI JNIProvisionEnrollee : Failed to convert jstring to char string!");
    }

    LOGI("JNI JNIProvisionEnrollee : ipAddress is : %s",ipAddress);

    const char *netSSID = env->GetStringUTFChars(jNetSSID, NULL);
    if (NULL == netSSID)
    {
        LOGE("JNI JNIProvisionEnrollee : Failed to convert jstring to char string!");
    }

    LOGI("JNI JNIProvisionEnrollee : netSSID is : %s",netSSID);

    const char *netPWD = env->GetStringUTFChars(jNetPWD, NULL);
    if (NULL == netPWD)
    {
        LOGE("JNI JNIProvisionEnrollee : Failed to convert jstring to char string!");
    }

    LOGI("JNI JNIProvisionEnrollee : netPWD is : %s",netPWD);

    OCConnectivityType connecitivityType;
    EnrolleeNWProvInfo_t netInfo =
    {   0};

    netInfo.
    connType = (OCConnectivityType) jConnectivityType;

    if(netInfo.connType==CT_ADAPTER_GATT_BTLE)
    {

        strncpy(netInfo
                .netAddressInfo.LE.leMacAddress,ipAddress,NET_MACADDR_SIZE-1);
        LOGI("MAC set=%s",netInfo.netAddressInfo.LE.leMacAddress);

    }
    else strncpy(netInfo.netAddressInfo.WIFI.ipAddress, ipAddress, IPV4_ADDR_SIZE-1);

    strncpy(netInfo
            .netAddressInfo.WIFI.ssid, netSSID, NET_WIFI_SSID_SIZE-1);
    strncpy(netInfo
            .netAddressInfo.WIFI.pwd, netPWD, NET_WIFI_PWD_SIZE-1);

    netInfo.
    isSecured = true;

    ProvisionEnrollee(&netInfo);

    return;
}

JNIEXPORT void JNICALL
JNIStopEnrolleeProvisioning(JNIEnv
        *env,
        jobject thisObj,
        jint
        jConnectivityType)
{
    LOGI("JNI Stop Easy Setup: Entering");

    OCConnectivityType connecitivityType = OCConnectivityType::CT_DEFAULT;

    if(jConnectivityType == 0)
    {
        connecitivityType = OCConnectivityType::CT_ADAPTER_IP;
    }

    StopEnrolleeProvisioning(connecitivityType);

    return;
}


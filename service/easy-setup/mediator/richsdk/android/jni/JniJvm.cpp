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
#include "JniJvm.h"

JavaVM *g_jvm = NULL;

jclass g_cls_RemoteEnrollee = NULL;
jclass g_cls_PropertyData = NULL;
jclass g_cls_DeviceConfig = NULL;
jclass g_cls_NetworkInfo = NULL;
jclass g_cls_RequestPropertyDataStatus = NULL;
jclass g_cls_SecurityProvisioningStatus = NULL;
jclass g_cls_DataProvisioningStatus = NULL;
jclass g_cls_ESException = NULL;
jclass g_cls_CloudProvisioningStatus = NULL;
jclass g_cls_Integer = NULL;

jmethodID g_mid_RemoteEnrollee_ctor = NULL;
jmethodID g_mid_ESException_ctor = NULL;
jmethodID g_mid_PropertyData_ctor = NULL;
jmethodID g_mid_DeviceConfig_ctor = NULL;
jmethodID g_mid_NetworkInfo_ctor = NULL;
jmethodID g_mid_RequestPropertyDataStatus_ctor = NULL;
jmethodID g_mid_SecurityProvisioningStatus_ctor = NULL;
jmethodID g_mid_DataProvisioningStatus_ctor = NULL;
jmethodID g_mid_CloudProvisioningStatus_ctor = NULL;
jmethodID g_mid_Integer_ctor = NULL;

// JNI OnLoad
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
    LOGI("JNI_OnLoad");
    JNIEnv *env;
    g_jvm = vm;

    if (g_jvm->GetEnv((void **)&env, JNI_CURRENT_VERSION) != JNI_OK)
    {
        LOGE("Failed to get the environment using GetEnv()");
        return JNI_ERR;
    }

    jclass clazz = nullptr;

    // Remote Enrollee
    clazz = env->FindClass("org/iotivity/service/easysetup/mediator/RemoteEnrollee");
    if (!clazz) return JNI_ERR;
    g_cls_RemoteEnrollee = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_RemoteEnrollee_ctor = env->GetMethodID(g_cls_RemoteEnrollee, "<init>",
                                "(J)V");
    if (!g_mid_RemoteEnrollee_ctor) return JNI_ERR;

    // ESException
    clazz = env->FindClass("org/iotivity/service/easysetup/mediator/ESException");
    if (!clazz) return JNI_ERR;
    g_cls_ESException = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_ESException_ctor = env->GetMethodID(g_cls_ESException, "<init>", "(Ljava/lang/String;)V");
    if (!g_mid_ESException_ctor) return JNI_ERR;

   // DeviceConfig
    clazz = env->FindClass("org/iotivity/service/easysetup/mediator/DeviceConfig");
    if (!clazz) return JNI_ERR;

    g_cls_DeviceConfig = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_DeviceConfig_ctor = env->GetMethodID(g_cls_DeviceConfig, "<init>",
                                "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
    if (!g_mid_DeviceConfig_ctor) return JNI_ERR;

    // NetworkInfo
    clazz = env->FindClass("org/iotivity/service/easysetup/mediator/NetworkInfo");
    if (!clazz) return JNI_ERR;

    g_cls_NetworkInfo = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_NetworkInfo_ctor = env->GetMethodID(g_cls_NetworkInfo, "<init>",
                                "(Ljava/util/ArrayList;I)V");
    if (!g_mid_NetworkInfo_ctor) return JNI_ERR;

    // PropertyData
    clazz = env->FindClass("org/iotivity/service/easysetup/mediator/PropertyData");
    if (!clazz) return JNI_ERR;

    g_cls_PropertyData = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_PropertyData_ctor = env->GetMethodID(g_cls_PropertyData, "<init>",
                                "(Lorg/iotivity/service/easysetup/mediator/DeviceConfig;Lorg/iotivity/service/easysetup/mediator/NetworkInfo;Z)V");
    if (!g_mid_PropertyData_ctor) return JNI_ERR;

    // RequestPropertyDataStatus
    clazz = env->FindClass("org/iotivity/service/easysetup/mediator/RequestPropertyDataStatus");
    if (!clazz) return JNI_ERR;

    g_cls_RequestPropertyDataStatus = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_RequestPropertyDataStatus_ctor = env->GetMethodID(g_cls_RequestPropertyDataStatus, "<init>",
                                "(ILorg/iotivity/service/easysetup/mediator/PropertyData;)V");
    if (!g_mid_RequestPropertyDataStatus_ctor) return JNI_ERR;

    // SecurityProvisioningStatus
    clazz = env->FindClass("org/iotivity/service/easysetup/mediator/SecurityProvisioningStatus");
    if (!clazz) return JNI_ERR;

    g_cls_SecurityProvisioningStatus = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_SecurityProvisioningStatus_ctor = env->GetMethodID(g_cls_SecurityProvisioningStatus, "<init>",
                                "(ILjava/lang/String;)V");
    if (!g_mid_SecurityProvisioningStatus_ctor) return JNI_ERR;

    // DataProvisioningStatus
    clazz = env->FindClass("org/iotivity/service/easysetup/mediator/DataProvisioningStatus");
    if (!clazz) return JNI_ERR;

    g_cls_DataProvisioningStatus = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_DataProvisioningStatus_ctor = env->GetMethodID(g_cls_DataProvisioningStatus, "<init>",
                                "(II)V");
    if (!g_mid_DataProvisioningStatus_ctor) return JNI_ERR;

    // CloudProvisioningStatus
    clazz = env->FindClass("org/iotivity/service/easysetup/mediator/CloudProvisioningStatus");
    if (!clazz) return JNI_ERR;
    g_cls_CloudProvisioningStatus = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_CloudProvisioningStatus_ctor = env->GetMethodID(g_cls_CloudProvisioningStatus, "<init>",
                                "(II)V");
    if (!g_mid_CloudProvisioningStatus_ctor) return JNI_ERR;

    // Integer
    clazz = env->FindClass("java/lang/Integer");
    if (!clazz) return JNI_ERR;
    g_cls_Integer = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_Integer_ctor = env->GetMethodID(g_cls_Integer, "<init>",
                                "(I)V");
    if (!g_mid_Integer_ctor) return JNI_ERR;

    return JNI_CURRENT_VERSION;
}

//JNI OnUnload
JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved)
{
    LOGI("JNI_OnUnload");
    JNIEnv *env;

    if (g_jvm->GetEnv((void **)&env, JNI_CURRENT_VERSION) != JNI_OK)
    {
        LOGE("Failed to get the environment using GetEnv()");
        return;
    }
    env->DeleteGlobalRef(g_cls_RemoteEnrollee);
    env->DeleteGlobalRef(g_cls_ESException);
}

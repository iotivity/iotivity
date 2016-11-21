/*
* //******************************************************************
* //
* // Copyright 2015 Intel Corporation.
* //
* //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
* //
* // Licensed under the Apache License, Version 2.0 (the "License");
* // you may not use this file except in compliance with the License.
* // You may obtain a copy of the License at
* //
* //      http://www.apache.org/licenses/LICENSE-2.0
* //
* // Unless required by applicable law or agreed to in writing, software
* // distributed under the License is distributed on an "AS IS" BASIS,
* // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* // See the License for the specific language governing permissions and
* // limitations under the License.
* //
* //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/

#include <jni.h>
#include <android/log.h>
#include <stdio.h>
#include "cainterface.h"
#include "JniCaInterface.h"
#include "cautilinterface.h"
#include "cacommon.h"

#define  LOG_TAG   "JNI_CA_INTERFACE"
#define  LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define  LOGE(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

static jobject g_foundDeviceListenerObject = NULL;
static jobject g_listenerObject = NULL;
static JavaVM *g_jvm = NULL;
static jclass g_jni_cls_enum = NULL;
static jmethodID g_jni_mid_enum = NULL;

JNIEXPORT jint JNI_OnLoad(JavaVM *jvm, void *reserved)
{
    LOGI("CaInterface_initialize");
    g_jvm = jvm;
    CANativeJNISetJavaVM(jvm);

    return JNI_VERSION_1_6;
}

void JNI_OnUnload(JavaVM *jvm, void *reserved)
{
    return;
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_CaInterface_initialize
(JNIEnv *env, jclass clazz, jobject activity, jobject context)
{
    LOGI("CaInterface_initialize");

    CANativeSetActivity(env, activity);
    CANativeJNISetContext(env, context);
}

void CAManagerConnectionStateChangedCB(const CAEndpoint_t *info,
                                       bool connected)
{
    if (!info)
    {
        LOGE("info is NULL");
        return;
    }

    if (!g_listenerObject)
    {
        LOGE("g_listener is NULL, cannot have callback");
        return;
    }

    LOGI("Callback - CAManagerConnectionStateChangedCB : type(%d), address(%s), connected(%d)",
         info->adapter, info->addr, connected);

    bool isAttached = false;
    JNIEnv* env = NULL;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**) &env, JNI_VERSION_1_6);
    if (JNI_OK != res)
    {
        LOGI("AttachCurrentThread will be called for JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);

        if (JNI_OK != res)
        {
            LOGE("AttachCurrentThread has failed");
            return;
        }
        isAttached = true;
    }

    jclass jni_cls_listener = (*env)->GetObjectClass(env, g_listenerObject);
    if (!jni_cls_listener)
    {
        LOGE("could not get jni_cls_listener");
        goto exit_error;
    }

    jmethodID jni_mid_listener = (*env)->GetMethodID(env, jni_cls_listener,
                                                     "onConnectionStateChanged",
                                                     "(Lorg/iotivity/base/OcConnectivityType;"
                                                     "Ljava/lang/String;Z)V");
    if (!jni_mid_listener)
    {
        LOGE("could not get Method ID");
        goto exit_error;
    }

    jstring jni_address = (*env)->NewStringUTF(env, info->addr);
    if (!jni_address)
    {
        LOGE("jni_address is null");
        goto exit_error;
    }

    if (g_jni_cls_enum && g_jni_mid_enum)
    {
        jobject jni_adaptertype = (*env)->CallStaticObjectMethod(env, g_jni_cls_enum,
                                                                 g_jni_mid_enum, info->adapter);
        (*env)->CallVoidMethod(env, g_listenerObject, jni_mid_listener,
                               jni_adaptertype, jni_address,
                               (jboolean)connected);
    }

exit_error:
    if (isAttached)
    {
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }

    LOGI("OUT - CAManagerConnectionStateChangedCB");
}

void CAManagerAdapterStateChangedCB(CATransportAdapter_t adapter, bool enabled)
{
    LOGI("Callback - CAManagerAdapterStateChangedCB : type(%d), enabled(%d)",
         adapter, enabled);

    if (!g_listenerObject)
    {
        LOGE("g_listener is NULL, cannot have callback");
        return;
    }

    bool isAttached = false;
    JNIEnv* env = NULL;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**) &env, JNI_VERSION_1_6);
    if (JNI_OK != res)
    {
        LOGI("AttachCurrentThread will be called for JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);

        if (JNI_OK != res)
        {
            LOGE("AttachCurrentThread has failed");
            return;
        }
        isAttached = true;
    }

    jclass jni_cls_listener = (*env)->GetObjectClass(env, g_listenerObject);
    if (!jni_cls_listener)
    {
        LOGE("could not get jni_cls_listener");
        goto exit_error;
    }

    jmethodID jni_mid_listener = (*env)->GetMethodID(env, jni_cls_listener,
                                                     "onAdapterStateChanged",
                                                     "(Lorg/iotivity/base/OcConnectivityType;Z)V");
    if (!jni_mid_listener)
    {
        LOGE("could not get Method ID");
        goto exit_error;
    }

    if (g_jni_cls_enum && g_jni_mid_enum)
    {
        jobject jni_adaptertype = (*env)->CallStaticObjectMethod(env, g_jni_cls_enum,
                                                                 g_jni_mid_enum, adapter);

        (*env)->CallVoidMethod(env, g_listenerObject, jni_mid_listener,
                               jni_adaptertype, (jboolean)enabled);
    }

exit_error:
    if (isAttached)
    {
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }
    LOGI("OUT -  CAManagerAdapterStateChangedCB");
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_CaInterface_caManagerInitialize(JNIEnv *env, jclass clazz,
                                                     jobject context, jobject listener)
{
    LOGI("CaManagere_initialize");

    CAUtilClientInitialize(env, g_jvm, context);

    if (listener)
    {
        g_listenerObject = (*env)->NewGlobalRef(env, listener);
    }

    if (g_listenerObject)
    {
        jclass cls = (*env)->FindClass(env, "org/iotivity/base/OcConnectivityType");
        if (cls)
        {
            g_jni_cls_enum = (jclass)(*env)->NewGlobalRef(env, cls);
        }

        if (g_jni_cls_enum)
        {
            g_jni_mid_enum = (*env)->GetStaticMethodID(env, g_jni_cls_enum, "getInstance",
                                                   "(I)Lorg/iotivity/base/OcConnectivityType;");
        }
    }
    CARegisterNetworkMonitorHandler(CAManagerAdapterStateChangedCB,
                                    CAManagerConnectionStateChangedCB);
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_CaInterface_caManagerTerminate(JNIEnv *env, jclass clazz)
{
    LOGI("CaManager_terminate");

    CAUtilClientTerminate(env);

    if (g_listenerObject)
    {
        (*env)->DeleteGlobalRef(env, g_listenerObject);
        g_listenerObject = NULL;
    }

    if (g_jni_cls_enum)
    {
        (*env)->DeleteGlobalRef(env, g_jni_cls_enum);
        g_jni_cls_enum = NULL;
    }
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_CaInterface_caManagerSetAutoConnectionDeviceInfo(JNIEnv *env,
                                                                      jclass clazz,
                                                                      jstring jaddress)
{
    LOGI("CaManager_setAutoConnectionDeviceInfo");
    if (!jaddress)
    {
        LOGE("jaddress is null");
        return;
    }

    const char* address = (*env)->GetStringUTFChars(env, jaddress, NULL);
    if (!address)
    {
        LOGE("address is null");
        return;
    }

    CASetAutoConnectionDeviceInfo(address);

    (*env)->ReleaseStringUTFChars(env, jaddress, address);
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_CaInterface_caManagerUnsetAutoConnectionDeviceInfo(JNIEnv *env,
                                                                        jclass clazz,
                                                                        jstring jaddress)
{
    LOGI("CaManager_unsetAutoConnectionDeviceInfo");
    if (!jaddress)
    {
        LOGE("jaddress is null");
        return;
    }

    const char* address = (*env)->GetStringUTFChars(env, jaddress, NULL);
    if (!address)
    {
        LOGE("address is null");
        return;
    }

    CAUnsetAutoConnectionDeviceInfo(address);

    (*env)->ReleaseStringUTFChars(env, jaddress, address);
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_CaInterface_caBtPairingInitialize(JNIEnv *env, jclass clazz,
                                                       jobject context, jobject listener)
{
    LOGI("caBtPairingInitialize");
    (void)clazz;

    CAUtilClientInitialize(env, g_jvm, context);

    g_foundDeviceListenerObject = (*env)->NewGlobalRef(env, listener);
    CAUtilSetFoundDeviceListener(g_foundDeviceListenerObject);
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_CaInterface_caBtPairingTerminate(JNIEnv *env, jclass clazz)
{
    LOGI("caBtPairingTerminate");
    (void)clazz;

    if (g_foundDeviceListenerObject)
    {
        (*env)->DeleteGlobalRef(env, g_foundDeviceListenerObject);
    }
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_CaInterface_caBtPairingStartScan(JNIEnv *env, jclass clazz)
{
    LOGI("caBtPairingStartScan");
    (void)clazz;
    CAUtilStartScan(env);
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_CaInterface_caBtPairingStopScan(JNIEnv *env, jclass clazz)
{
    LOGI("caBtPairingStopScan");
    (void)clazz;
    CAUtilStopScan(env);
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_CaInterface_caBtPairingCreateBond(JNIEnv *env, jclass clazz, jobject device)
{
    LOGI("caBtPairingCreateBond");
    (void)clazz;
    CAUtilCreateBond(env, device);
}

JNIEXPORT void JNICALL
Java_org_iotivity_ca_CaInterface_setLeScanIntervalTimeImpl(JNIEnv *env, jclass clazz,
                                                           jint intervalTime, jint workignCount)
{
    LOGI("setLeScanIntervalTimeImpl");
    (void)env;
    (void)clazz;
    CAUtilSetLEScanInterval(intervalTime, workignCount);
}

JNIEXPORT jint JNICALL Java_org_iotivity_ca_CaInterface_setCipherSuiteImpl
  (JNIEnv *env, jclass clazz, jint cipherSuite, jint adapter)
{
    LOGI("setCipherSuiteImpl");
    (void)env;
    (void)clazz;
    CAResult_t ret = CASelectCipherSuite(cipherSuite, (CATransportAdapter_t) adapter);
    if (CA_STATUS_OK != ret)
    {
        LOGE("CASelectCipherSuite has failed");
    }
    return ret;
}


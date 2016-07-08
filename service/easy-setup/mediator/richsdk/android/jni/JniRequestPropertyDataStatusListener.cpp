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

#include "JniRequestPropertyDataStatusListener.h"
#include "JniRemoteEnrollee.h"

using namespace OIC::Service;

JniRequestPropertyDataStatusListener::JniRequestPropertyDataStatusListener(JNIEnv *env, jobject jListener,
        JniRemoteEnrollee *owner)
    : m_ownerResource(owner)
{
    m_jwListener = env->NewWeakGlobalRef(jListener);
}

JniRequestPropertyDataStatusListener::~JniRequestPropertyDataStatusListener()
{
    LOGI("~JniRequestPropertyDataStatusListener()");
    if (m_jwListener)
    {
        jint ret;
        JNIEnv *env = GetESJNIEnv(ret);
        if (NULL == env) return;
        env->DeleteWeakGlobalRef(m_jwListener);
        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
    }
}

void JniRequestPropertyDataStatusListener::requestPropertyDataStatusCallback (
    std::shared_ptr<RequestPropertyDataStatus> requestPropertyDataStatusCb)
{
    LOGI("JniRequestPropertyDataStatusListener::provisioiningStatusCallback enter");

    jint ret;
    JNIEnv *env = GetESJNIEnv(ret);
    if (NULL == env) return;
    jobject jListener = env->NewLocalRef(m_jwListener);
    if (!jListener)
    {
        checkExAndRemoveListener(env);
        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
        return;
    }

    jclass clsL = env->GetObjectClass(jListener);
    if (!clsL)
    {
        checkExAndRemoveListener(env);
        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
        return;
    }

    jmethodID midL = env->GetMethodID(clsL, "onProgress",
                                      "(Lorg/iotivity/service/easysetup/mediator/"
                                      "RequestPropertyDataStatus;"
                                      ")V");

    if (!midL)
    {
        checkExAndRemoveListener(env);
        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
        return;
    }

    const PropertyData propertyData = requestPropertyDataStatusCb->getPropertyData();
    DeviceConfig devConf = propertyData.getDevConf();
    NetworkInfo netInfo = propertyData.getNetInfo();

    jobject jDevConf = NULL;
    jDevConf = env->NewObject(g_cls_DeviceConfig,
                                                g_mid_DeviceConfig_ctor,
                                                (jstring)env->NewStringUTF(devConf.id.c_str()),
                                                (jstring)env->NewStringUTF(devConf.name.c_str()),
                                                (jstring)env->NewStringUTF(devConf.language.c_str()),
                                                (jstring)env->NewStringUTF(devConf.country.c_str()));
    if (!jDevConf) {
        LOGE("JniRequestPropertyDataStatusListener::requestPropertyDataStatusCallback Unable to create the jDevConf");
        return ;
    }

    jclass clazz = env->FindClass("java/util/ArrayList");
    jobject wifiModeTypes = env->NewObject(clazz, env->GetMethodID(clazz, "<init>", "()V"));
    jmethodID arraylist_add = env->GetMethodID(clazz, "add", "(Ljava/lang/Object;)Z");

    for (int n=0; n<netInfo.types.size(); n++)
    {
        jobject value = env->NewObject(g_cls_Integer,
                                                            g_mid_Integer_ctor,
                                                            convertNativeWifiModeToInt(static_cast<WIFI_MODE>(netInfo.types[n])));
       env->CallBooleanMethod(wifiModeTypes, arraylist_add, value);
    }
    if (!wifiModeTypes) {
        LOGE("JniRequestPropertyDataStatusListener::requestPropertyDataStatusCallback Unable to create the wifiModeTypes");
        return ;
    }


    jobject jNetInfo = NULL;
    jNetInfo = env->NewObject(g_cls_NetworkInfo,
                                                g_mid_NetworkInfo_ctor,
                                                (jobject)wifiModeTypes,
                                                (jint)convertNativeWifiFreqToInt(netInfo.freq));
    if (!jNetInfo) {
        LOGE("JniRequestPropertyDataStatusListener::requestPropertyDataStatusCallback Unable to create the jNetInfo");
        return ;
    }

    jobject jPropertyData = NULL;
    jPropertyData = env->NewObject(g_cls_PropertyData,
                                                g_mid_PropertyData_ctor,
                                                (jobject)jDevConf,
                                                (jobject)jNetInfo,
                                                (jboolean)propertyData.isCloudable());
    if (!jPropertyData) {
        LOGE("JniRequestPropertyDataStatusListener::requestPropertyDataStatusCallback Unable to create the jPropertyData");
        return ;
    }

    ESResult esResult = requestPropertyDataStatusCb->getESResult();
    jobject jRequestPropertyDataStatus = NULL;
    jRequestPropertyDataStatus = env->NewObject(g_cls_RequestPropertyDataStatus,
                                                g_mid_RequestPropertyDataStatus_ctor,
                                                (jint)esResult,
                                                (jobject)jPropertyData);
LOGI("4");
    if (!jRequestPropertyDataStatus)
    {
        LOGE("JniRequestPropertyDataStatusListener::requestPropertyDataStatusCallback Unable to create the java object");
        return ;
    }

    env->CallVoidMethod(jListener, midL, jRequestPropertyDataStatus);

    if (env->ExceptionCheck())
    {
        LOGE("Java exception is thrown");
        checkExAndRemoveListener(env);
        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
        return;
    }

    if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
}

void JniRequestPropertyDataStatusListener::checkExAndRemoveListener(JNIEnv *env)
{
    if (env->ExceptionCheck())
    {
        jthrowable ex = env->ExceptionOccurred();
        env->ExceptionClear();
        m_ownerResource->removeStatusListener<JniRequestPropertyDataStatusListener>(env, m_jwListener);

        env->Throw((jthrowable)ex);
    }
    else
    {
        m_ownerResource->removeStatusListener<JniRequestPropertyDataStatusListener>(env, m_jwListener);
    }
}

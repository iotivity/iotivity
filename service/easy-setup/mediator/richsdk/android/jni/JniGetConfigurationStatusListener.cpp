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

#include "JniGetConfigurationStatusListener.h"
#include "JniRemoteEnrollee.h"

using namespace OIC::Service;

JniGetConfigurationStatusListener::JniGetConfigurationStatusListener(JNIEnv *env, jobject jListener,
        JniRemoteEnrollee *owner)
    : m_ownerResource(owner)
{
    m_jwListener = env->NewWeakGlobalRef(jListener);
}

JniGetConfigurationStatusListener::~JniGetConfigurationStatusListener()
{
    LOGI("~JniGetConfigurationStatusListener()");
    if (m_jwListener)
    {
        jint ret;
        JNIEnv *env = GetESJNIEnv(ret);
        if (NULL == env) return;
        env->DeleteWeakGlobalRef(m_jwListener);
        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
    }
}

void JniGetConfigurationStatusListener::getConfigurationStatusCallback (
    std::shared_ptr<GetConfigurationStatus> getConfigurationStatusCb)
{
    LOGI("JniGetConfigurationStatusListener::provisioiningStatusCallback enter");

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
                                      "GetConfigurationStatus;"
                                      ")V");

    if (!midL)
    {
        checkExAndRemoveListener(env);
        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
        return;
    }

    const EnrolleeConf enrolleeConf = getConfigurationStatusCb->getEnrolleeConf();
    DeviceConfig devConf = enrolleeConf.getDevConf();
    WiFiConfig wifiConf = enrolleeConf.getWiFiConf();

    jobject jDevConf = NULL;
    jDevConf = env->NewObject(g_cls_DeviceConfig,
                                                g_mid_DeviceConfig_ctor,
                                                (jstring)env->NewStringUTF(devConf.name.c_str()),
                                                (jstring)env->NewStringUTF(devConf.language.c_str()),
                                                (jstring)env->NewStringUTF(devConf.country.c_str()));
    if (!jDevConf) {
        LOGE("JniGetConfigurationStatusListener::getConfigurationStatusCallback Unable to create the jDevConf");
        return ;
    }

    jclass clazz = env->FindClass("java/util/ArrayList");
    jobject wifiModes = env->NewObject(clazz, env->GetMethodID(clazz, "<init>", "()V"));
    jmethodID arraylist_add = env->GetMethodID(clazz, "add", "(Ljava/lang/Object;)Z");

    for (int n=0; n<wifiConf.modes.size(); n++)
    {
        jobject value = env->NewObject(g_cls_Integer,
                                        g_mid_Integer_ctor,
                                        convertNativeWifiModeToInt(static_cast<WIFI_MODE>(wifiConf.modes[n])));
       env->CallBooleanMethod(wifiModes, arraylist_add, value);
    }
    if (!wifiModes) {
        LOGE("JniGetConfigurationStatusListener::getConfigurationStatusCallback Unable to create the wifiModes");
        return ;
    }


    jobject jWiFiConf = NULL;
    jWiFiConf = env->NewObject(g_cls_WiFiConfig,
                                                g_mid_WiFiConfig_ctor,
                                                (jobject)wifiModes,
                                                (jint)convertNativeWifiFreqToInt(wifiConf.freq));
    if (!jWiFiConf) {
        LOGE("JniGetConfigurationStatusListener::getConfigurationStatusCallback Unable to create the jWiFiConf");
        return ;
    }

    jobject jEnrolleeConf = NULL;
    jEnrolleeConf = env->NewObject(g_cls_EnrolleeConf,
                                                g_mid_EnrolleeConf_ctor,
                                                (jobject)jDevConf,
                                                (jobject)jWiFiConf,
                                                (jboolean)enrolleeConf.isCloudable());
    if (!jEnrolleeConf) {
        LOGE("JniGetConfigurationStatusListener::getConfigurationStatusCallback Unable to create the jEnrolleeConf");
        return ;
    }

    ESResult esResult = getConfigurationStatusCb->getESResult();
    jobject jgetConfigurationStatus = NULL;
    jgetConfigurationStatus = env->NewObject(g_cls_getConfigurationStatus,
                                                g_mid_getConfigurationStatus_ctor,
                                                (jint)esResult,
                                                (jobject)jEnrolleeConf);

    if (!jgetConfigurationStatus)
    {
        LOGE("JniGetConfigurationStatusListener::getConfigurationStatusCallback Unable to create the java object");
        return ;
    }

    env->CallVoidMethod(jListener, midL, jgetConfigurationStatus);

    if (env->ExceptionCheck())
    {
        LOGE("Java exception is thrown");
        checkExAndRemoveListener(env);
        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
        return;
    }

    if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
}

void JniGetConfigurationStatusListener::checkExAndRemoveListener(JNIEnv *env)
{
    if (env->ExceptionCheck())
    {
        jthrowable ex = env->ExceptionOccurred();
        env->ExceptionClear();
        m_ownerResource->removeStatusListener<JniGetConfigurationStatusListener>(env, m_jwListener);

        env->Throw((jthrowable)ex);
    }
    else
    {
        m_ownerResource->removeStatusListener<JniGetConfigurationStatusListener>(env, m_jwListener);
    }
}

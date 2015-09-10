/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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

#include "jni_re_resource_statechange_listener.h"
#include "jni_re_rcs_remoteresourceobject.h"

JniResourceStateChangeListener::JniResourceStateChangeListener(JNIEnv *env, jobject jListener,
        JniRCSRemoteResourceObject *owner)
    : m_ownerResource(owner)
{
    m_jwListener = env->NewWeakGlobalRef(jListener);
}

JniResourceStateChangeListener::~JniResourceStateChangeListener()
{
    LOGI("~JniResourceStateChangeListener()");
    if (m_jwListener)
    {
        jint ret;
        JNIEnv *env = GetREJNIEnv(ret);
        if (NULL == env) return;
        env->DeleteWeakGlobalRef(m_jwListener);
        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
    }
}

void JniResourceStateChangeListener::stateChangeCallback(OIC::Service::ResourceState state)
{

    LOGI("JniResourceStateChangeListener::stateChangeCallback enter");

    jint ret;
    JNIEnv *env = GetREJNIEnv(ret);
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
    jmethodID midL = env->GetMethodID(clsL, "onStateChanged",
            "(Lorg/iotivity/ResourceEncapsulation/client/RCSRemoteResourceObject$ResourceState;)V");
    if (!midL)
    {
        checkExAndRemoveListener(env);
        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
        return;
    }
    jobject resourcestate = convertNativeResourceStateToJavaResourceState(state, env);
    env->CallVoidMethod(jListener, midL, resourcestate);

    if (env->ExceptionCheck())
    {
        LOGE("Java exception is thrown");
        checkExAndRemoveListener(env);
        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
        return;
    }

    if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
}

void JniResourceStateChangeListener::checkExAndRemoveListener(JNIEnv *env)
{
    if (env->ExceptionCheck())
    {
        jthrowable ex = env->ExceptionOccurred();
        env->ExceptionClear();
        m_ownerResource->removeOnStateChangeListener(env, m_jwListener);
        env->Throw((jthrowable)ex);
    }
    else
    {
        m_ownerResource->removeOnStateChangeListener(env, m_jwListener);
    }
}

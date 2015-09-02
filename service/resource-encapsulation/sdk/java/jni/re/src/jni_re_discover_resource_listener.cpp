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

#include "RCSRemoteResourceObject.h"

#include "jni_re_discover_resource_listener.h"

JniDiscoverResourceListener::JniDiscoverResourceListener(JNIEnv *env, jobject jListener,
        RemoveListenerCallback removeListenerCallback)
{
    m_jwListener = env->NewWeakGlobalRef(jListener);
    m_removeListenerCallback = removeListenerCallback;
}

JniDiscoverResourceListener::~JniDiscoverResourceListener()
{
    LOGI("JniDiscoverResourceListener::~jni_DiscoverResourceListener()");
    if (m_jwListener)
    {
        jint ret;
        JNIEnv *env = GetREJNIEnv(ret);
        if (NULL == env) return;
        env->DeleteWeakGlobalRef(m_jwListener);
        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
    }
}

void JniDiscoverResourceListener::discoverResourceCallback(
    std::shared_ptr<OIC::Service::RCSRemoteResourceObject> resource)
{
    LOGI("JniDiscoverResourceListener::discoverResourceCallback enter");

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

    jobject jResource = env->NewObject(g_cls_RCSRemoteResourceObject,
                                       g_mid_RCSRemoteResourceObject_ctor);
    if (!jResource)
    {
        checkExAndRemoveListener(env);
        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
        return;
    }

    JniRCSRemoteResourceObject *jniRCSRemoteResourceObject = new JniRCSRemoteResourceObject(resource);
    RESetHandle<JniRCSRemoteResourceObject>(env, jResource, jniRCSRemoteResourceObject);
    if (env->ExceptionCheck())
    {
        delete jniRCSRemoteResourceObject;
        checkExAndRemoveListener(env);
        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
        return;
    }

    jclass clsL = env->GetObjectClass(jListener);
    if (!clsL)
    {
        delete jniRCSRemoteResourceObject;
        checkExAndRemoveListener(env);
        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
        return;
    }
    jmethodID midL = env->GetMethodID(clsL, "onResourceDiscovered",
                                      "(Lorg/iotivity/ResourceEncapsulation/client/RCSRemoteResourceObject;)V");
    if (!midL)
    {
        delete jniRCSRemoteResourceObject;
        checkExAndRemoveListener(env);
        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
        return;
    }
    env->CallVoidMethod(jListener, midL, jResource);
    if (env->ExceptionCheck())
    {
        LOGE("Java exception is thrown");
        delete jniRCSRemoteResourceObject;
        checkExAndRemoveListener(env);
        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
        return;
    }

    if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
    LOGI("JniDiscoverResourceListener::discoverResourceCallback exit");
}

void JniDiscoverResourceListener::checkExAndRemoveListener(JNIEnv *env)
{
    if (env->ExceptionCheck())
    {
        jthrowable ex = env->ExceptionOccurred();
        env->ExceptionClear();
        m_removeListenerCallback(env, m_jwListener);
        env->Throw((jthrowable)ex);
    }
    else
    {
        m_removeListenerCallback(env, m_jwListener);
    }
}

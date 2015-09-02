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

#include "jni_re_discover_resource_listener.h"
#include "RCSRemoteResourceObject.h"
#include "jni_re_cache_update_listener.h"

JniCacheUpdateListener::JniCacheUpdateListener(JNIEnv *env, jobject jListener,
        JniRCSRemoteResourceObject *owner)
    :  m_ownerResource(owner)
{
    m_jwListener = env->NewWeakGlobalRef(jListener);
}

JniCacheUpdateListener::~JniCacheUpdateListener()
{
    LOGI("JniCacheUpdateListener::~JniCacheUpdateListener");
    if (m_jwListener)
    {
        jint ret;
        JNIEnv *env = GetREJNIEnv(ret);
        if (NULL == env) return;
        env->DeleteWeakGlobalRef(m_jwListener);
        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
    }
}

void JniCacheUpdateListener::cacheUpdatedCallback(OIC::Service::RCSResourceAttributes attributes)
{
    LOGI("JniDiscoverResourceListener::cacheUpdatedCallback enter");

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

    RCSResourceAttributes *attributesObj = new RCSResourceAttributes(attributes);
    jlong handle = reinterpret_cast<jlong>(attributesObj);
    jobject jAttributes = env->NewObject(g_cls_RCSRemoteResourceAttributesObject,
                                         g_mid_RCSRemoteResourceAttributesObject_ctor,
                                         handle, true);
    if (!jAttributes)
    {
        delete attributesObj;
        checkExAndRemoveListener(env);
        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
        return;
    }

    jmethodID midL = env->GetMethodID(clsL, "onCacheUpdatedCallback",
                                      "(Lorg/iotivity/ResourceEncapsulation/common/RCSResourceAttributes;)V");

    if (!midL)
    {
        delete attributesObj;
        checkExAndRemoveListener(env);
        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
        return;
    }
    env->CallVoidMethod(jListener, midL, jAttributes);
    if (env->ExceptionCheck())
    {
        LOGE("Java exception is thrown");
        delete attributesObj;
    }

    if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
    LOGI("JniDiscoverResourceListener::cacheUpdatedCallback exit");
}

void JniCacheUpdateListener::checkExAndRemoveListener(JNIEnv *env)
{
    if (env->ExceptionCheck())
    {
        jthrowable ex = env->ExceptionOccurred();
        env->ExceptionClear();
        m_ownerResource->removeCacheUpdateListener(env, m_jwListener);
        env->Throw((jthrowable)ex);
    }
    else
    {
        m_ownerResource->removeCacheUpdateListener(env, m_jwListener);
    }
}

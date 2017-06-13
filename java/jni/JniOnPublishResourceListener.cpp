/* ****************************************************************
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
#include "JniOnPublishResourceListener.h"

using namespace OC;

JniOnPublishResourceListener::JniOnPublishResourceListener(JNIEnv *env, jobject jListener,
    RemoveListenerCallback removeListenerCallback)
{
    m_jwListener = env->NewWeakGlobalRef(jListener);
    m_removeListenerCallback = removeListenerCallback;
}

JniOnPublishResourceListener::~JniOnPublishResourceListener()
{
    LOGI("~JniOnPublishResourceListener()");
    if (m_jwListener)
    {
        jint ret = JNI_ERR;
        JNIEnv *env = GetJNIEnv(ret);
        if (nullptr == env)
        {
            return;
        }
        env->DeleteWeakGlobalRef(m_jwListener);
        m_jwListener = nullptr;
        if (JNI_EDETACHED == ret)
        {
            g_jvm->DetachCurrentThread();
        }
    }
}

void JniOnPublishResourceListener::onPublishResourceCallback(
        const OCRepresentation& ocRepresentation,
        const int eCode)
{
    jint envRet = JNI_ERR;
    JNIEnv *env = GetJNIEnv(envRet);
    if (nullptr == env)
    {
        return;
    }

    if (nullptr == m_jwListener)
    {
        LOGE("listener is not available");
        if (JNI_EDETACHED == envRet)
        {
            g_jvm->DetachCurrentThread();
        }
        return;
    }

    jobject jListener = env->NewLocalRef(m_jwListener);
    if (!jListener)
    {
        checkExAndRemoveListener(env);
        if (JNI_EDETACHED == envRet)
        {
            g_jvm->DetachCurrentThread();
        }
        return;
    }
    jclass clsL = env->GetObjectClass(jListener);

    if (!clsL)
    {
        checkExAndRemoveListener(env);
        if (JNI_EDETACHED == envRet)
        {
            g_jvm->DetachCurrentThread();
        }
        return;
    }

    if (OC_STACK_OK != eCode && OC_STACK_RESOURCE_CREATED != eCode
            && OC_STACK_RESOURCE_DELETED != eCode && OC_STACK_CONTINUE != eCode
            && OC_STACK_RESOURCE_CHANGED != eCode)
    {
        jobject ex = GetOcException(eCode, "stack error in onPublishResourceCallback");
        if (!ex)
        {
            goto CLEANUP;
        }
        jmethodID midL = env->GetMethodID(clsL, "onPublishResourceFailed",
            "(Ljava/lang/Throwable;)V");
        if (!midL)
        {
            goto CLEANUP;
        }
        env->CallVoidMethod(jListener, midL, ex);
    }
    else
    {
        OCRepresentation* rep = new OCRepresentation(ocRepresentation);
        jlong handle = reinterpret_cast<jlong>(rep);
        jobject jRepresentation = env->NewObject(g_cls_OcRepresentation,
                                                 g_mid_OcRepresentation_N_ctor_bool,
                                                 handle, true);
        if (!jRepresentation)
        {
            delete rep;
            goto CLEANUP;
        }

        jmethodID midL = env->GetMethodID(clsL, "onPublishResourceCompleted",
            "(Lorg/iotivity/base/OcRepresentation;)V");
        if (!midL)
        {
            delete rep;
            goto CLEANUP;
        }
        env->CallVoidMethod(jListener, midL, jRepresentation);
        if (env->ExceptionCheck())
        {
            LOGE("Java exception is thrown");
            delete rep;
        }
    }

CLEANUP:
    checkExAndRemoveListener(env);
    if (JNI_EDETACHED == envRet)
    {
        g_jvm->DetachCurrentThread();
    }
}

void JniOnPublishResourceListener::checkExAndRemoveListener(JNIEnv* env)
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

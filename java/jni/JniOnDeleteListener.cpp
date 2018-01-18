/*
* ******************************************************************
*
*  Copyright 2015 Intel Corporation.
*
* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*       http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*
* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/

#include "JniOnDeleteListener.h"
#include "JniOcResource.h"
#include "JniUtils.h"
#ifdef WITH_CLOUD
#include "JniOcAccountManager.h"
#endif

using namespace OC;

JniOnDeleteListener::JniOnDeleteListener(JNIEnv *env, jobject jListener, JniOcResource* owner)
    : m_ownerResource(owner)
{
    m_jwListener = env->NewWeakGlobalRef(jListener);
#ifdef WITH_CLOUD
    m_ownerAccountManager = nullptr;
#endif
}

#ifdef WITH_CLOUD
JniOnDeleteListener::JniOnDeleteListener(JNIEnv *env, jobject jListener, JniOcAccountManager* owner)
    : m_ownerAccountManager(owner)
{
    m_jwListener = env->NewWeakGlobalRef(jListener);
    m_ownerResource = nullptr;
}
#endif

JniOnDeleteListener::~JniOnDeleteListener()
{
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

void JniOnDeleteListener::onDeleteCallback(const HeaderOptions& headerOptions, const int eCode)
{
    jint envRet = JNI_ERR;
    JNIEnv *env = GetJNIEnv(envRet);
    if (nullptr == env)
    {
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

    if (OC_STACK_RESOURCE_DELETED != eCode)
    {
        jobject ex = GetOcException(eCode, "stack error in onDeleteCallback");
        if (!ex)
        {
            checkExAndRemoveListener(env);
            if (JNI_EDETACHED == envRet)
            {
                g_jvm->DetachCurrentThread();
            }
            return;
        }
        jmethodID midL = env->GetMethodID(clsL, "onDeleteFailed", "(Ljava/lang/Throwable;)V");
        if (!midL)
        {
            checkExAndRemoveListener(env);
            if (JNI_EDETACHED == envRet)
            {
                g_jvm->DetachCurrentThread();
            }
            return;
        }
        env->CallVoidMethod(jListener, midL, ex);
    }
    else
    {
        jobject jHeaderOptionList = JniUtils::convertHeaderOptionsVectorToJavaList(env, headerOptions);
        if (!jHeaderOptionList)
        {
            checkExAndRemoveListener(env);
            if (JNI_EDETACHED == envRet)
            {
                g_jvm->DetachCurrentThread();
            }
            return;
        }

        jmethodID midL = env->GetMethodID(clsL, "onDeleteCompleted", "(Ljava/util/List;)V");
        if (!midL)
        {
            checkExAndRemoveListener(env);
            if (JNI_EDETACHED == envRet)
            {
                g_jvm->DetachCurrentThread();
            }
            return;
        }
        env->CallVoidMethod(jListener, midL, jHeaderOptionList);
    }

    checkExAndRemoveListener(env);
    if (JNI_EDETACHED == envRet)
    {
        g_jvm->DetachCurrentThread();
    }
}

void JniOnDeleteListener::checkExAndRemoveListener(JNIEnv* env)
{
    if (env->ExceptionCheck())
    {
        jthrowable ex = env->ExceptionOccurred();
        env->ExceptionClear();
#ifndef WITH_CLOUD
        m_ownerResource->removeOnDeleteListener(env, m_jwListener);
#else
        if (nullptr != m_ownerAccountManager)
        {
            m_ownerAccountManager->removeOnDeleteListener(env, m_jwListener);
        }
        if (nullptr != m_ownerResource)
        {
            m_ownerResource->removeOnDeleteListener(env, m_jwListener);
        }
#endif
        env->Throw((jthrowable)ex);
    }
    else
    {
#ifndef WITH_CLOUD
        m_ownerResource->removeOnDeleteListener(env, m_jwListener);
#else
        if (nullptr != m_ownerAccountManager)
        {
            m_ownerAccountManager->removeOnDeleteListener(env, m_jwListener);
        }
        if (nullptr != m_ownerResource)
        {
            m_ownerResource->removeOnDeleteListener(env, m_jwListener);
        }
#endif
    }
}

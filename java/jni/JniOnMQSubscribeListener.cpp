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

#include "JniOnMQSubscribeListener.h"
#include "JniOcResource.h"
#include "JniOcRepresentation.h"
#include "JniUtils.h"

using namespace OC;

JniOnMQSubscribeListener::JniOnMQSubscribeListener(JNIEnv *env,
                                                   jobject jListener,
                                                   JniOcResource* owner)
    : m_ownerResource(owner)
{
    m_jwListener = env->NewWeakGlobalRef(jListener);
}

JniOnMQSubscribeListener::~JniOnMQSubscribeListener()
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

void JniOnMQSubscribeListener::onSubscribeCallback(const HeaderOptions headerOptions,
    const OCRepresentation& ocRepresentation, const int& eCode, const int& sequenceNumber)
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
        env->DeleteLocalRef(jListener);
        checkExAndRemoveListener(env);
        if (JNI_EDETACHED == envRet)
        {
            g_jvm->DetachCurrentThread();
        }
        return;
    }

    if (OC_STACK_OK != eCode && OC_STACK_RESOURCE_CREATED != eCode &&
            OC_STACK_RESOURCE_DELETED != eCode && OC_STACK_RESOURCE_CHANGED != eCode)
    {
        jobject ex = GetOcException(eCode, "stack error in onSubScribeCallback");
        if (!ex)
        {
            goto JNI_EXIT;
        }
        jmethodID midL = env->GetMethodID(clsL, "onSubScribeFailed", "(Ljava/lang/Throwable;)V");
        if (!midL)
        {
            goto JNI_EXIT;
        }

        env->CallVoidMethod(jListener, midL, ex);
    }
    else
    {
        OCRepresentation * rep = new OCRepresentation(ocRepresentation);
        jlong handle = reinterpret_cast<jlong>(rep);
        jobject jRepresentation = env->NewObject(g_cls_OcRepresentation,
            g_mid_OcRepresentation_N_ctor_bool, handle, true);
        if (!jRepresentation)
        {
            delete rep;
            goto JNI_EXIT;
        }

        if (sequenceNumber != 1 + MAX_SEQUENCE_NUMBER)
        {
            jmethodID midL = env->GetMethodID(clsL, "onSubScribeCompleted",
                        "(Ljava/util/List;Lorg/iotivity/base/OcRepresentation;I)V");
            if (!midL)
            {
                delete rep;
                env->DeleteLocalRef(jRepresentation);
                goto JNI_EXIT;
            }

            jobject jHeaderOptionList = JniUtils::convertHeaderOptionsVectorToJavaList(env,
                                                                                       headerOptions);
            if (!jHeaderOptionList)
            {
                delete rep;
                env->DeleteLocalRef(jRepresentation);
                goto JNI_EXIT;
            }

            env->CallVoidMethod(jListener, midL, jHeaderOptionList, jRepresentation,
                static_cast<jint>(sequenceNumber));
            if (env->ExceptionCheck())
            {
                LOGE("Java exception is thrown");
                delete rep;
                env->DeleteLocalRef(jRepresentation);
                env->DeleteLocalRef(jHeaderOptionList);
                jthrowable ex = env->ExceptionOccurred();
                env->ExceptionClear();
                m_ownerResource->removeOnMQTopicSubscribeListener(env, m_jwListener);
                env->Throw((jthrowable)ex);
            }
        }
        else
        {
            jmethodID midL = env->GetMethodID(clsL, "onUnsubScribeCompleted",
                        "(Lorg/iotivity/base/OcRepresentation;I)V");
            if (!midL)
            {
                delete rep;
                env->DeleteLocalRef(jRepresentation);
                goto JNI_EXIT;
            }

            env->CallVoidMethod(jListener, midL, jRepresentation,
                        static_cast<jint>(sequenceNumber));
            if (env->ExceptionCheck())
            {
                LOGE("Java exception is thrown");
                delete rep;
                env->DeleteLocalRef(jRepresentation);
                jthrowable ex = env->ExceptionOccurred();
                env->ExceptionClear();
                m_ownerResource->removeOnMQTopicSubscribeListener(env, m_jwListener);
                env->Throw((jthrowable)ex);
            }

            checkExAndRemoveListener(env);
        }
    }

    env->DeleteLocalRef(clsL);
    env->DeleteLocalRef(jListener);
    if (JNI_EDETACHED == envRet)
    {
        g_jvm->DetachCurrentThread();
    }
    return;

JNI_EXIT:
    env->DeleteLocalRef(clsL);
    env->DeleteLocalRef(jListener);
    checkExAndRemoveListener(env);
    if (JNI_EDETACHED == envRet)
    {
        g_jvm->DetachCurrentThread();
    }
}

void JniOnMQSubscribeListener::checkExAndRemoveListener(JNIEnv* env)
{
    if (env->ExceptionCheck())
    {
        jthrowable ex = env->ExceptionOccurred();
        env->ExceptionClear();
        m_ownerResource->removeOnMQTopicSubscribeListener(env, m_jwListener);
        env->Throw((jthrowable)ex);
    }
    else
    {
        m_ownerResource->removeOnMQTopicSubscribeListener(env, m_jwListener);
    }
}

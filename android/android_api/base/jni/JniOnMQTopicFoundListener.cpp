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

#include "JniOnMQTopicFoundListener.h"
#include "JniOcResource.h"
#include "JniOcRepresentation.h"
#include "JniUtils.h"

JniOnMQTopicFoundListener::JniOnMQTopicFoundListener(JNIEnv *env, jobject jListener,
                                                     JniOcResource* owner)
    : m_ownerResource(owner)
{
    m_jwListener = env->NewWeakGlobalRef(jListener);
}

JniOnMQTopicFoundListener::~JniOnMQTopicFoundListener()
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

void JniOnMQTopicFoundListener::foundTopicCallback(const int eCode, const std::string& uri,
                                                   std::shared_ptr<OC::OCResource> resource)
{
    jint ret = JNI_ERR;
    JNIEnv *env = GetJNIEnv(ret);
    if (nullptr == env)
    {
        return;
    }

    jobject jListener = env->NewLocalRef(m_jwListener);
    if (!jListener)
    {
        checkExAndRemoveListener(env);
        if (JNI_EDETACHED == ret)
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
        if (JNI_EDETACHED == ret)
        {
            g_jvm->DetachCurrentThread();
        }
        return;
    }

    if (OC_STACK_OK != eCode)
    {
        jobject ex = GetOcException(eCode, "stack error");
        if (!ex)
        {
            goto JNI_EXIT;
        }
        jmethodID midL = env->GetMethodID(clsL, "onDiscoveryTopicFailed",
                                          "(Ljava/lang/Throwable;Ljava/lang/String;)V");
        if (!midL)
        {
            goto JNI_EXIT;
        }

        env->CallVoidMethod(jListener, midL, ex, env->NewStringUTF(uri.c_str()));
    }
    else
    {

        jobject jResource = env->NewObject(g_cls_OcResource, g_mid_OcResource_ctor);
        if (!jResource)
        {
            goto JNI_EXIT;
        }

        JniOcResource *jniOcResource = new JniOcResource(resource);
        SetHandle<JniOcResource>(env, jResource, jniOcResource);
        if (env->ExceptionCheck())
        {
            delete jniOcResource;
            env->DeleteLocalRef(jResource);
            goto JNI_EXIT;
        }

        jmethodID midL = env->GetMethodID(clsL, "onTopicDiscoveried", "(Lorg/iotivity/base/OcResource;)V");
        if (!midL)
        {
            delete jniOcResource;
            env->DeleteLocalRef(jResource);
            goto JNI_EXIT;
        }
        env->CallVoidMethod(jListener, midL, jResource);
        if (env->ExceptionCheck())
        {
            LOGE("Java exception is thrown");
            delete jniOcResource;
            env->DeleteLocalRef(jResource);
            goto JNI_EXIT;
        }
    }

    env->DeleteLocalRef(clsL);
    env->DeleteLocalRef(jListener);
    if (JNI_EDETACHED == ret)
    {
        g_jvm->DetachCurrentThread();
    }
    return;

JNI_EXIT:
    env->DeleteLocalRef(clsL);
    env->DeleteLocalRef(jListener);
    checkExAndRemoveListener(env);
    if (JNI_EDETACHED == ret)
    {
        g_jvm->DetachCurrentThread();
    }
}

void JniOnMQTopicFoundListener::createdTopicCallback(const int eCode, const std::string& uri,
                                                     std::shared_ptr<OC::OCResource> resource)
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
            OC_STACK_RESOURCE_CHANGED != eCode)
    {
        jobject ex = GetOcException(eCode, "stack error");
        if (!ex)
        {
            goto JNI_EXIT;
        }
        jmethodID midL = env->GetMethodID(clsL, "onCreateTopicFailed",
                                          "(Ljava/lang/Throwable;Ljava/lang/String;)V");
        if (!midL)
        {
            goto JNI_EXIT;
        }

        env->CallVoidMethod(jListener, midL, ex, env->NewStringUTF(uri.c_str()));
    }
    else
    {
        // create Resource object
        jobject jResource = env->NewObject(g_cls_OcResource, g_mid_OcResource_ctor);
        if (!jResource)
        {
            goto JNI_EXIT;
        }

        JniOcResource *jniOcResource = new JniOcResource(resource);
        SetHandle<JniOcResource>(env, jResource, jniOcResource);
        if (env->ExceptionCheck())
        {
            delete jniOcResource;
            env->DeleteLocalRef(jResource);
            goto JNI_EXIT;
        }

        jmethodID midL = env->GetMethodID(clsL, "onTopicResourceCreated",
            "(Lorg/iotivity/base/OcResource;)V");
        if (!midL)
        {
            delete jniOcResource;
            env->DeleteLocalRef(jResource);
            goto JNI_EXIT;
        }

        env->CallVoidMethod(jListener, midL, jResource);
        if (env->ExceptionCheck())
        {
            LOGE("Java exception is thrown");
            env->DeleteLocalRef(jResource);
            delete jniOcResource;
        }
    }

JNI_EXIT:
    env->DeleteLocalRef(clsL);
    env->DeleteLocalRef(jListener);
    checkExAndRemoveListener(env);
    if (JNI_EDETACHED == envRet)
    {
        g_jvm->DetachCurrentThread();
    }
}

void JniOnMQTopicFoundListener::checkExAndRemoveListener(JNIEnv* env)
{
    if (env->ExceptionCheck())
    {
        jthrowable ex = env->ExceptionOccurred();
        env->ExceptionClear();
        m_ownerResource->removeOnTopicFoundListener(env, m_jwListener);
        env->Throw((jthrowable)ex);
    }
    else
    {
        m_ownerResource->removeOnTopicFoundListener(env, m_jwListener);
    }
}

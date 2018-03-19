/*
* ******************************************************************
*
*  Copyright 2016 Samsung Electronics All Rights Reserved.
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
#include "JniOcCloudResultListener.h"

JniOcCloudResultListener::JniOcCloudResultListener(JNIEnv *env, jobject jListener,
    RemoveCallback removeCloudResultListener)
{
    m_jwListener = env->NewWeakGlobalRef(jListener);
    m_removeCloudResultListener = removeCloudResultListener;
}

JniOcCloudResultListener::~JniOcCloudResultListener()
{
    LOGI("~JniOcCloudResultListener()");
    if (m_jwListener)
    {
        jint ret = JNI_ERR;
        JNIEnv *env = GetJNIEnv(ret);
        if (nullptr == env)
        {
            return;
        }
        env->DeleteWeakGlobalRef(m_jwListener);
        if (JNI_EDETACHED == ret)
        {
            g_jvm->DetachCurrentThread();
        }
    }
}

void JniOcCloudResultListener::CloudResultListenerCB(int result, void *data,
      ListenerFunc func)
{
    OC_UNUSED(data);
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
        checkExAndRemoveListener(env);
        if (JNI_EDETACHED == ret)
        {
            g_jvm->DetachCurrentThread();
        }
        return;
    }

    std::string calledFunc;
    switch (func)
    {
        case ListenerFunc::REQUEST_CERTIFICATE:
            {
                calledFunc = "requestCertificateListener";
            }
            break;
        case ListenerFunc::GET_ACLINFO:
            {
                calledFunc = "getIndividualAclInfoListener";
            }
            break;
        case ListenerFunc::GET_CRL:
            {
                calledFunc = "getCRLListener";
            }
            break;
        case ListenerFunc::POST_CRL:
            {
                calledFunc = "postCRLListener";
            }
            break;
        case ListenerFunc::UPDATE_IND_ACL:
            {
                calledFunc = "updateIndividualACLListener";
            }
            break;
        default:
            {
                checkExAndRemoveListener(env);
                if (JNI_EDETACHED == ret)
                {
                    g_jvm->DetachCurrentThread();
                }
                return;
            }
    }

    jmethodID midL = env->GetMethodID(clsL, calledFunc.c_str(), "(I)V");
    if (!midL)
    {
        checkExAndRemoveListener(env);
        if (JNI_EDETACHED == ret)
        {
            g_jvm->DetachCurrentThread();
        }
        return;
    }

    env->CallVoidMethod(jListener, midL, result);
    if (env->ExceptionCheck())
    {
        LOGE("Java exception is thrown");
    }

    checkExAndRemoveListener(env);
    if (JNI_EDETACHED == ret)
    {
        g_jvm->DetachCurrentThread();
    }
}

void JniOcCloudResultListener::checkExAndRemoveListener(JNIEnv* env)
{
    if (env->ExceptionCheck())
    {
        jthrowable ex = env->ExceptionOccurred();
        env->ExceptionClear();
        m_removeCloudResultListener(env, m_jwListener);
        env->Throw((jthrowable)ex);
    }
    else
    {
        m_removeCloudResultListener(env, m_jwListener);
    }
}

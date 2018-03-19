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
#include "JniConfirmNumListener.h"

JniConfirmNumListener::JniConfirmNumListener(JNIEnv *env, jobject jListener)
{
    m_jgListener = env->NewGlobalRef(jListener);
}

JniConfirmNumListener::~JniConfirmNumListener()
{
    LOGI("~JniConfirmNumListener()");
    if (m_jgListener)
    {
        jint ret = JNI_ERR;
        JNIEnv *env = GetJNIEnv(ret);
        if (NULL == env) return;
        env->DeleteGlobalRef(m_jgListener);
        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
    }
}

OCStackResult JniConfirmNumListener::confirmMutualVerifNumCallback()
{
    jint ret = JNI_ERR;
    JNIEnv *env = GetJNIEnv(ret);
    if (NULL == env)
    {
        return OC_STACK_ERROR;
    }

    jclass clsL = env->GetObjectClass(m_jgListener);

    if (!clsL)
    {
        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
        return OC_STACK_ERROR;
    }

    jmethodID midL = env->GetMethodID(clsL, "confirmNumListener", "()I");
    if (!midL)
    {
        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
        return OC_STACK_ERROR;
    }

    OCStackResult result = (OCStackResult) env->CallIntMethod(m_jgListener, midL);

    if (env->ExceptionCheck())
    {
        LOGE("Java exception is thrown");
        env->ExceptionClear();
    }

    if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
    return result;
}

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
#include "JniDisplayVerifyNumListener.h"
#include "oic_malloc.h"

JniDisplayVerifyNumListener::JniDisplayVerifyNumListener(JNIEnv *env, jobject jListener)
{
    m_jgListener = env->NewGlobalRef(jListener);
}

JniDisplayVerifyNumListener::~JniDisplayVerifyNumListener()
{
    LOGI("~JniDisplayVerifyNumListener()");
    if (m_jgListener)
    {
        jint ret = JNI_ERR;
        JNIEnv *env = GetJNIEnv(ret);
        if (NULL == env) return;
        env->DeleteGlobalRef(m_jgListener);
        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
    }
}

OCStackResult JniDisplayVerifyNumListener::displayMutualVerifNumCallback(uint8_t verifNum[3])
{
    jint ret = JNI_ERR;
    JNIEnv *env = GetJNIEnv(ret);
    char *byteArray;

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

    jmethodID midL = env->GetMethodID(clsL, "displayNumListener", "(Ljava/lang/String;)I");
    if (!midL)
    {
        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
        return OC_STACK_ERROR;
    }

    byteArray = (char*)OICCalloc(20, sizeof(char));

    if (NULL == byteArray)
    {
        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
        return OC_STACK_NO_MEMORY;
    }

    sprintf(byteArray, "%02X%02X%02X",  verifNum[0], verifNum[1], verifNum[2]);

    jstring jStr = env->NewStringUTF(byteArray);
    if (!jStr)
    {
        if (JNI_EDETACHED == ret)
        {
            g_jvm->DetachCurrentThread();
        }
        return OC_STACK_ERROR;
    }
    OCStackResult result = (OCStackResult)env->CallIntMethod(m_jgListener, midL, jStr);

    if (env->ExceptionCheck())
    {
        LOGE("Java exception is thrown");
        env->ExceptionClear();
    }

    if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
    free(byteArray);
    return result;
}

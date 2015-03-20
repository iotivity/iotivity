/*
* //******************************************************************
* //
* // Copyright 2015 Intel Corporation.
* //
* //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
* //
* // Licensed under the Apache License, Version 2.0 (the "License");
* // you may not use this file except in compliance with the License.
* // You may obtain a copy of the License at
* //
* //      http://www.apache.org/licenses/LICENSE-2.0
* //
* // Unless required by applicable law or agreed to in writing, software
* // distributed under the License is distributed on an "AS IS" BASIS,
* // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* // See the License for the specific language governing permissions and
* // limitations under the License.
* //
* //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/

#include "JniOnDeleteListener.h"
#include "JniOcResource.h"

JniOnDeleteListener::JniOnDeleteListener(JNIEnv *env, jobject jListener, JniOcResource* owner)
    : m_ownerResource(owner)
{
    m_jwListener = env->NewWeakGlobalRef(jListener);
}

JniOnDeleteListener::~JniOnDeleteListener()
{
    LOGD("~JniOnDeleteListener()");

    if (m_jwListener)
    {
        jint ret;
        JNIEnv *env = GetJNIEnv(ret);
        if (NULL == env) return;

        env->DeleteWeakGlobalRef(m_jwListener);

        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
    }
}

void JniOnDeleteListener::onDeleteCallback(const HeaderOptions& headerOptions, const int eCode)
{
    jint envRet;
    JNIEnv *env = GetJNIEnv(envRet);
    if (NULL == env) return;

    if (OC_STACK_OK != eCode)
    {
        ThrowOcException(eCode, "DeleteCallback has failed");
        m_ownerResource->removeOnDeleteListener(env, m_jwListener);

        if (JNI_EDETACHED == envRet) g_jvm->DetachCurrentThread();
        return;
    }

    jobject jHeaderOptionList = env->NewObject(g_cls_LinkedList, g_mid_LinkedList_ctor);
    for (int i = 0; i < headerOptions.size(); i++)
    {
        jobject jHeaderOption = env->NewObject(
            g_cls_OcHeaderOption,
            g_mid_OcHeaderOption_ctor,
            (jint)headerOptions[i].getOptionID(),
            env->NewStringUTF(headerOptions[i].getOptionData().c_str())
            );

        env->CallObjectMethod(jHeaderOptionList, g_mid_LinkedList_add_object, jHeaderOption);
        env->DeleteLocalRef(jHeaderOption);
    }

    jobject jListener = env->NewLocalRef(m_jwListener);
    if (!jListener)
    {
        m_ownerResource->removeOnDeleteListener(env, m_jwListener);

        if (JNI_EDETACHED == envRet) g_jvm->DetachCurrentThread();
        return;
    }

    jclass clsL = env->GetObjectClass(jListener);
    jmethodID midL = env->GetMethodID(clsL, "onDeleteCompleted",
        "(Ljava/util/List;)V");

    env->CallVoidMethod(jListener, midL, jHeaderOptionList);

    if (env->ExceptionCheck())
    {
        env->ExceptionClear();
        LOGE("Exception is thrown in Java onDeleteCompleted handler");
    }

    m_ownerResource->removeOnDeleteListener(env, m_jwListener);

    if (JNI_EDETACHED == envRet) g_jvm->DetachCurrentThread();
}
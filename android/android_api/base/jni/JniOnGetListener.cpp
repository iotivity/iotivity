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
#include "JniOnGetListener.h"
#include "JniOcResource.h"
#include "JniOcRepresentation.h"
#include "JniUtils.h"

JniOnGetListener::JniOnGetListener(JNIEnv *env, jobject jListener, JniOcResource* owner) 
    : m_ownerResource(owner)
{
    m_jwListener = env->NewWeakGlobalRef(jListener);
}

JniOnGetListener::~JniOnGetListener()
{
    LOGD("~JniOnGetListener()");

    if (m_jwListener)
    {
        jint ret;
        JNIEnv *env = GetJNIEnv(ret);
        if (NULL == env) return;

        env->DeleteWeakGlobalRef(m_jwListener);

        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
    }
}

void JniOnGetListener::onGetCallback(const HeaderOptions& headerOptions,
    const OCRepresentation& ocRepresentation, const int eCode)
{
    jint envRet;
    JNIEnv *env = GetJNIEnv(envRet);
    if (NULL == env) return;

    if (OC_STACK_OK != eCode)
    {
        ThrowOcException(eCode, "GetCallback has failed");
        m_ownerResource->removeOnGetListener(env, m_jwListener);

        if (JNI_EDETACHED == envRet) g_jvm->DetachCurrentThread();
        return;
    }

    jobject jHeaderOptionList = JniUtils::convertHeaderOptionsVectorToJavaList(env, headerOptions);

    OCRepresentation * rep= new OCRepresentation(ocRepresentation);
    jlong handle = reinterpret_cast<jlong>(rep);
    jobject jRepresentation = env->NewObject(g_cls_OcRepresentation, g_mid_OcRepresentation_N_ctor_bool, handle, true);

    jobject jListener = env->NewLocalRef(m_jwListener);
    if (!jListener)
    {
        m_ownerResource->removeOnGetListener(env, m_jwListener);

        if (JNI_EDETACHED == envRet) g_jvm->DetachCurrentThread();
        return;
    }

    jclass clsL = env->GetObjectClass(jListener);
    jmethodID midL = env->GetMethodID(clsL, "onGetCompleted", 
        "(Ljava/util/List;Lorg/iotivity/base/OcRepresentation;)V");

    env->CallVoidMethod(jListener, midL, jHeaderOptionList, jRepresentation);

    if (env->ExceptionCheck())
    {
        env->ExceptionClear();
        LOGE("Exception is thrown in Java onGetCompleted handler");
    }

    m_ownerResource->removeOnGetListener(env, m_jwListener);

    if (JNI_EDETACHED == envRet) g_jvm->DetachCurrentThread();
}
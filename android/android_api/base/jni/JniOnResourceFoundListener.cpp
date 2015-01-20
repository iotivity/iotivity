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
#include "JniOnResourceFoundListener.h"
#include "JniOcResource.h"

JniOnResourceFoundListener::JniOnResourceFoundListener(JNIEnv *env, jobject jListener)
{
    m_jwListener = env->NewWeakGlobalRef(jListener);
}

JniOnResourceFoundListener::~JniOnResourceFoundListener()
{
    LOGI("~JniOnResourceFoundListener()");

    if (m_jwListener)
    {
        jint ret;
        JNIEnv *env = GetJNIEnv(ret);
        if (NULL == env) return;

        env->DeleteWeakGlobalRef(m_jwListener);
        m_jwListener = NULL;

        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
    }
}

void JniOnResourceFoundListener::foundResourceCallback(std::shared_ptr<OC::OCResource> resource)
{
    jint ret;
    JNIEnv *env = GetJNIEnv(ret);
    if (NULL == env) return;

    jobject jListener = env->NewLocalRef(m_jwListener);
    if (!jListener)
    {
        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
        return;
    }

    jclass clsL = env->GetObjectClass(jListener);
    jmethodID midL = env->GetMethodID(clsL, "onResourceFound", "(Lorg/iotivity/base/OcResource;)V");

    jobject jResource = env->NewObject(g_cls_OcResource, g_mid_OcResource_ctor);

    JniOcResource *jniOcResource = new JniOcResource(resource);

    SetHandle<JniOcResource>(env, jResource, jniOcResource);

    env->CallVoidMethod(jListener, midL, jResource);

    if (env->ExceptionCheck())
    {
        env->ExceptionClear();
        LOGE("Exception is thrown in Java onResourceFound handler");
    }

    env->DeleteLocalRef(jListener);
    env->DeleteLocalRef(jResource);

    if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
}

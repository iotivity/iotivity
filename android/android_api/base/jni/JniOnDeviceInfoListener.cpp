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
#include "JniOnDeviceInfoListener.h"
#include "JniOcRepresentation.h"

JniOnDeviceInfoListener::JniOnDeviceInfoListener(JNIEnv *env, jobject jListener)
{
    m_jwListener = env->NewWeakGlobalRef(jListener);
}

JniOnDeviceInfoListener::~JniOnDeviceInfoListener()
{
    LOGI("JniOnDeviceInfoListener - destr.");

    if (m_jwListener)
    {
        jint ret;
        JNIEnv *env = GetJNIEnv(ret);
        if (NULL == env) return;

        env->DeleteWeakGlobalRef(m_jwListener);

        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
    }
}

void JniOnDeviceInfoListener::foundDeviceCallback(const OC::OCRepresentation& ocRepresentation)
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

    jlong handle = reinterpret_cast<jlong>(&ocRepresentation);

    jobject jRepresentation = env->NewObject(g_cls_OcRepresentation, g_mid_OcRepresentation_N_ctor, handle);
    if (env->ExceptionCheck())
    {
        LOGE("Failed to create OcRepresentation");
        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
        return;
    }

    jclass clsL = env->GetObjectClass(jListener);
    jmethodID midL = env->GetMethodID(clsL, "onDeviceFound", "(Lorg/iotivity/base/OcRepresentation;)V");

    env->CallVoidMethod(jListener, midL, jRepresentation);

    env->DeleteLocalRef(jListener);
    env->DeleteLocalRef(jRepresentation);

    if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
}
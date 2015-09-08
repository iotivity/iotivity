/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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

#include "jni_re_attribute_updated_listener.h"
#include "jni_re_resource_object.h"

JniAttributeUpdatedListener::JniAttributeUpdatedListener(JNIEnv *env, jobject jListener,
        JniRCSResourceObject *owner)
    : m_ownerResource(owner)
{
    m_jwListener = env->NewWeakGlobalRef(jListener);
}

JniAttributeUpdatedListener::~JniAttributeUpdatedListener()
{
    LOGI("JniAttributeUpdatedListener::~JniAttributeUpdatedListener()");
    if (m_jwListener)
    {
        jint ret;
        JNIEnv *env = GetREJNIEnv(ret);
        if (NULL == env) return;
        env->DeleteWeakGlobalRef(m_jwListener);
        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
    }
}

void JniAttributeUpdatedListener::onAttributeUpdatedCallback(
    OIC::Service::RCSResourceAttributes::Value oldValue,
    OIC::Service::RCSResourceAttributes::Value newValue)
{
    LOGI("JniAttributeUpdatedListener::onAttributeUpdatedCallback got a callback in JNI");

    jint ret;
    JNIEnv *env = GetREJNIEnv(ret);
    if (NULL == env) return;

    jobject jListener = env->NewLocalRef(m_jwListener);
    if (!jListener)
    {
        checkExAndRemoveListener(env);
        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
        return;
    }

    RCSResourceAttributes::TypeId oldValueTypeId = oldValue.getType().getId();
    RCSResourceAttributes::TypeId newValueTypeId = newValue.getType().getId();
    jstring javaOldValue;
    jstring javaNewValue;
    char buf[64]; // assumed large enough to cope with result
    char bufValue[64];

    if (oldValueTypeId == RCSResourceAttributes::TypeId::INT)
    {
        int oldVal = oldValue.get<int>();
        sprintf(buf, "%d", oldVal);
        javaOldValue = (env)->NewStringUTF(buf);

        int newVal = newValue.get<int>();
        sprintf(bufValue, "%d", newVal);
        javaNewValue = (env)->NewStringUTF(bufValue);

    }
    else if (oldValueTypeId == RCSResourceAttributes::TypeId::DOUBLE)
    {
        double oldVal = oldValue.get<double>();
        sprintf(buf, "%f", oldVal);
        javaOldValue = (env)->NewStringUTF(buf);

        double newVal = newValue.get<double>();
        sprintf(bufValue, "%f", newVal);
        javaNewValue = (env)->NewStringUTF(bufValue);

    }
    else if (oldValueTypeId == RCSResourceAttributes::TypeId::BOOL)
    {
        bool oldVal = oldValue.get<bool>();
        sprintf(buf, "%d", oldVal);
        javaOldValue = (env)->NewStringUTF(buf);

        bool newVal = newValue.get<bool>();
        sprintf(bufValue, "%d", newVal);
        javaNewValue = (env)->NewStringUTF(bufValue);
    }
    else if (oldValueTypeId == RCSResourceAttributes::TypeId::STRING)
    {
        std::string oldVal = oldValue.get<std::string>();
        javaOldValue = env->NewStringUTF(oldVal.c_str());

        std::string newVal = newValue.get<std::string>();
        javaNewValue = env->NewStringUTF(newVal.c_str());
    }

    //sending callback to application
    jclass clsL = env->GetObjectClass(jListener);

    if (!clsL)
    {
        checkExAndRemoveListener(env);
        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
        return;
    }

    jmethodID midL = env->GetMethodID(clsL, "onAttributeUpdate",
                                      "(Ljava/lang/String;Ljava/lang/String;)V");
    if (!midL)
    {
        checkExAndRemoveListener(env);
        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
        return;
    }
    env->CallVoidMethod(jListener, midL, javaOldValue, javaNewValue);
    if (env->ExceptionCheck())
    {
        LOGE("Java exception is thrown");
    }

    if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
    LOGI("JniDiscoverResourceListener::cacheUpdatedCallback exit");

}

void JniAttributeUpdatedListener::checkExAndRemoveListener(JNIEnv *env)
{
    if (env->ExceptionCheck())
    {
        jthrowable ex = env->ExceptionOccurred();
        env->ExceptionClear();
        m_ownerResource->removeOnSetRequestListener(env, m_jwListener);
        env->Throw((jthrowable)ex);
    }
    else
    {
        m_ownerResource->removeOnSetRequestListener(env, m_jwListener);
    }
}

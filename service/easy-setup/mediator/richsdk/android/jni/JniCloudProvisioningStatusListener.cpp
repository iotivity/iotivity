/******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
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

#include "JniCloudProvisioningStatusListener.h"
#include "JniRemoteEnrollee.h"

using namespace OIC::Service;

JniCloudProvisioningStatusListener::JniCloudProvisioningStatusListener(JNIEnv *env, jobject jListener,
        JniRemoteEnrollee *owner)
    : m_ownerResource(owner)
{
    m_jwListener = env->NewWeakGlobalRef(jListener);
}

JniCloudProvisioningStatusListener::~JniCloudProvisioningStatusListener()
{
    LOGI("~JniCloudProvisioningStatusListener()");
    if (m_jwListener)
    {
        jint ret;
        JNIEnv *env = GetESJNIEnv(ret);
        if (NULL == env) return;
        env->DeleteWeakGlobalRef(m_jwListener);
        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
    }
}

void JniCloudProvisioningStatusListener::onCloudProvisioningStatus(std::shared_ptr<CloudProvisioningStatus>
        cloudProvisioningStatus)
{

    LOGI("JniCloudProvisioningStatusListener::onCloudProvisioningStatus enter");

    jint ret;
    JNIEnv *env = GetESJNIEnv(ret);
    if (NULL == env) return;
    jobject jListener = env->NewLocalRef(m_jwListener);
    if (!jListener)
    {
        checkExAndRemoveListener(env);
        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
        return;
    }

    jclass clsL = env->GetObjectClass(jListener);
    if (!clsL)
    {
        checkExAndRemoveListener(env);
        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
        return;
    }

    jmethodID midL = env->GetMethodID(clsL, "onProgress",
                                      "(Lorg/iotivity/service/easysetup/mediator/"
                                      "CloudProvisioningStatus;"
                                      ")V");
    if (!midL)
    {
        checkExAndRemoveListener(env);
        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
        return;
    }

    ESResult esResult = cloudProvisioningStatus->getESResult();
    ESCloudProvState cloudProvisionState = cloudProvisioningStatus->getESCloudState();

    //create the java object
    jobject jCloudProvisioningStatus = NULL;
    jCloudProvisioningStatus = env->NewObject(g_cls_CloudProvisioningStatus,
                                                g_mid_CloudProvisioningStatus_ctor,
                                                (jint)esResult,
                                                (jint)cloudProvisionState);

    LOGI("JniCloudProvisioningStatus::onCloudProvisioningStatus - %d, %d", esResult, cloudProvisionState);
    if (!jCloudProvisioningStatus)
    {
        LOGE("JniCloudProvisioningStatus::onCloudProvisioningStatus Unable to create the java object");
        return ;
    }

    env->CallVoidMethod(jListener, midL, jCloudProvisioningStatus);

    bool needRemoveListener = false;

    if(cloudProvisionState == ES_CLOUD_PROVISIONING_ERROR ||
            cloudProvisionState == ES_CLOUD_PROVISIONING_SUCCESS )
    {
        needRemoveListener = true;
    }

    if (env->ExceptionCheck())
    {
        LOGE("Java exception is thrown");
        if(needRemoveListener)
            checkExAndRemoveListener(env);
        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
        return;
    }

    if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
}

void JniCloudProvisioningStatusListener::checkExAndRemoveListener(JNIEnv *env)
{
    if (env->ExceptionCheck())
    {
        jthrowable ex = env->ExceptionOccurred();
        env->ExceptionClear();
        m_ownerResource->removeCloudProvisioningStatusListener(env, m_jwListener);
        env->Throw((jthrowable)ex);
    }
    else
    {
        m_ownerResource->removeCloudProvisioningStatusListener(env, m_jwListener);
    }
}

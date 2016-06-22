/******************************************************************
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

#include "JniRemoteEnrollee.h"

using namespace OIC::Service;

JniRemoteEnrollee::JniRemoteEnrollee(std::shared_ptr<RemoteEnrollee> remoteEnrollee)
    : m_sharedResource(remoteEnrollee) {}

JniRemoteEnrollee::~JniRemoteEnrollee()
{
    LOGD("JniRemoteEnrollee::~JniRemoteEnrollee()");
    m_sharedResource = NULL;

    jint envRet;
    JNIEnv *env = GetESJNIEnv(envRet);
    if (NULL == env) return;

    if (JNI_EDETACHED == envRet) g_jvm->DetachCurrentThread();
}

JniRemoteEnrollee *JniRemoteEnrollee::getJniRemoteEnrollee(JNIEnv *env, jobject thiz)
{
    JniRemoteEnrollee *remoteEnrollee = ESGetHandle<JniRemoteEnrollee>(env, thiz);
    if (env->ExceptionCheck())
    {
        LOGE("getJniRemoteEnrollee :: Failed to get native handle from RemoteEnrollee object");
    }
    if (!remoteEnrollee)
    {
        LOGE("getJniRemoteEnrollee :: no resource");
    }
    return remoteEnrollee;
}

JNIEXPORT void JNICALL
Java_org_iotivity_service_easysetup_mediator_RemoteEnrollee_nativeSetCloudProvInfo
(JNIEnv *env, jobject jClass, jstring jauthCode, jstring jauthProvider, jstring jciServer)
{
    LOGD("nativeSetCloudProvInfo Enter");

    // TODO
    JniRemoteEnrollee *remoteEnrollee = JniRemoteEnrollee::getJniRemoteEnrollee(env, jClass);
    remoteEnrollee->setCloudProvInfo(env, jauthCode, jauthProvider, jciServer);

    LOGD("nativeSetCloudProvInfo Exit");
}

JNIEXPORT void JNICALL
Java_org_iotivity_service_easysetup_mediator_RemoteEnrollee_nativeStartCloudProvisioning
(JNIEnv *env, jobject jClass, jobject jListener)
{
    LOGD("nativeStartCloudProvisioning Enter");

    JniRemoteEnrollee *remoteEnrollee = JniRemoteEnrollee::getJniRemoteEnrollee(env, jClass);
    remoteEnrollee->startCloudProvisioning(env, jListener);

    LOGD("nativeStartCloudProvisioning Exit");
}

void JniRemoteEnrollee::setCloudProvInfo(JNIEnv *env, jstring jauthCode, jstring jauthProvider, jstring jciServer)
{
    CloudProvInfo info;
    info.authCode = env->GetStringUTFChars(jauthCode, NULL);
    info.authProvider = env->GetStringUTFChars(jauthProvider, NULL);
    info.ciServer = env->GetStringUTFChars(jciServer, NULL);

    m_sharedResource->setCloudProvInfo(info);
}

void JniRemoteEnrollee::startCloudProvisioning(JNIEnv *env, jobject jListener)
{
    JniCloudProvisioningStatusListener *onCloudProvisioningStatusReceived = addCloudProvisioningStatusListener(env,
            jListener);

    CloudProvStatusCb provisionStatusCallback = [onCloudProvisioningStatusReceived]
            (std::shared_ptr< OIC::Service::CloudProvisioningStatus > cloudProvisioningStatus)

    {
        onCloudProvisioningStatusReceived->onCloudProvisioningStatus(cloudProvisioningStatus);
    };

    try
    {
        m_sharedResource->startCloudProvisioning(provisionStatusCallback);
    }
    catch (ESBadRequestException exception)
    {
        LOGE("JNI startProvisioning :: Exception occured");
        //throw the exception to java
        throwESException(env, exception.what());
    }
}

JniCloudProvisioningStatusListener *JniRemoteEnrollee::addCloudProvisioningStatusListener(JNIEnv *env,
        jobject jListener)
{
    return this->m_cloudProvisioningStatus.addListener(env, jListener, this);
}

void JniRemoteEnrollee::removeCloudProvisioningStatusListener(JNIEnv *env, jobject jListener)
{
    this->m_cloudProvisioningStatus.removeListener(env, jListener);
}


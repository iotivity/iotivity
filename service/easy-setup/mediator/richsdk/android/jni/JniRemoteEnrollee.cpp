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

void JniRemoteEnrollee::startProvisioning(JNIEnv *env)
{
    try
    {
        m_sharedResource->startProvisioning();
    }
    catch (ESBadRequestException exception)
    {
        LOGE("JNI startProvisioning :: Exception occured");
        //throw the exception to java
        throwESException( env,  exception.what());
    }
}
void JniRemoteEnrollee::stopProvisioning(JNIEnv *env)
{
    try
    {
        m_sharedResource->stopProvisioning();
    }
    catch (ESBadRequestException exception)
    {
        LOGE("JNI stopProvisioning :: Exception occured");
        //throw the exception to java
        throwESException( env,  exception.what());
    }
}

void JniRemoteEnrollee::registerProvisioningHandler(JNIEnv *env, jobject jListener)
{
    JniProvisioningStatusListener *onProvisioningStatusReceived = addProvisioningStatusListener(env,
            jListener);

    RemoteEnrollee::EasySetupStatusCB provisionStatusCallback = [onProvisioningStatusReceived]
            (std::shared_ptr< OIC::Service::EasySetupStatus > easySetupStatus)

    {
        onProvisioningStatusReceived->provisionStatusCallback(easySetupStatus);
    };

    try
    {
        m_sharedResource->registerEasySetupStatusHandler(provisionStatusCallback);
    }
    catch (ESException exception)
    {
        LOGE("JNI stopProvisioning :: Exception occured");
        //throw the exception to java
        throwESException( env,  exception.what());
    }

}

JniProvisioningStatusListener *JniRemoteEnrollee::addProvisioningStatusListener(JNIEnv *env,
        jobject jListener)
{
    return this->m_provisioingStatus.addListener(env, jListener, this);
}

void JniRemoteEnrollee::removeProvisioningStatusListener(JNIEnv *env, jobject jListener)
{
    this->m_provisioingStatus.removeListener(env, jListener);
}

JNIEXPORT void JNICALL
Java_org_iotivity_service_easysetup_mediator_RemoteEnrollee_nativeStartProvision
(JNIEnv *env, jobject jClass)
{
    LOGD("nativeStartProvision Enter");

    JniRemoteEnrollee *remoteEnrollee = JniRemoteEnrollee::getJniRemoteEnrollee(env, jClass);
    remoteEnrollee->startProvisioning(env);

    LOGD("nativeStartProvision Exit");
}

JNIEXPORT void JNICALL
Java_org_iotivity_service_easysetup_mediator_RemoteEnrollee_nativeStopProvision
(JNIEnv *env, jobject jClass)
{

    LOGD("nativeStopProvision Enter");

    JniRemoteEnrollee *remoteEnrollee = JniRemoteEnrollee::getJniRemoteEnrollee(env, jClass);
    remoteEnrollee->stopProvisioning(env);

    LOGD("nativeStopProvision Exit");
}

JNIEXPORT void JNICALL
Java_org_iotivity_service_easysetup_mediator_RemoteEnrollee_nativeRegisterProvisioningHandler
(JNIEnv *env, jobject jClass, jobject provisiongListener)
{
    LOGD("nativeRegisterProvisioningHandler Enter");

    if (!provisiongListener)
    {
        LOGE("nativeRegisterProvisioningHandler : listener is NULL");
        return;
    }
    JniRemoteEnrollee *remoteEnrollee = JniRemoteEnrollee::getJniRemoteEnrollee(env, jClass);
    remoteEnrollee->registerProvisioningHandler(env, provisiongListener);

    LOGD("nativeRegisterProvisioningHandler Exit");
}


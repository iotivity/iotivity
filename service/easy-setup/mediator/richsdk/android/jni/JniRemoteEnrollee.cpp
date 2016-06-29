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

void JniRemoteEnrollee::initRemoteEnrollee(JNIEnv *env)
{
    try
    {
        m_sharedResource->initRemoteEnrollee();
    }
    catch (ESBadRequestException exception)
    {
        LOGE("JNI initRemoteEnrollee :: Exception occured");
        //throw the exception to java
        throwESException( env,  exception.what());
    }
}

void JniRemoteEnrollee::requestPropertyData(JNIEnv *env, jobject jListener)
{
    JniRequestPropertyDataStatusListener *onRequestPropertyDataStatusReceived =
                    addStatusListener<JniRequestPropertyDataStatusListener>(env, jListener);

    RequestPropertyDataStatusCb requestPropertyDataStatusCallback = [onRequestPropertyDataStatusReceived]
            (std::shared_ptr<OIC::Service::RequestPropertyDataStatus > requestPropertyDataStatus)
    {
        onRequestPropertyDataStatusReceived->requestPropertyDataStatusCallback(requestPropertyDataStatus);
    };

    try
    {
        m_sharedResource->requestPropertyData(requestPropertyDataStatusCallback);
    }
    catch (ESBadRequestException exception)
    {
        LOGE("JNI requestPropertyData :: Exception occured");
        //throw the exception to java
        throwESException( env,  exception.what());
    }
}

void JniRemoteEnrollee::startSecurityProvisioning(JNIEnv *env, jobject jListener)
{
    JniSecurityStatusListener *onSecurityProvStatusReceived =
                    addStatusListener<JniSecurityStatusListener>(env, jListener);

    SecurityProvStatusCb secProvStatusCallback = [onSecurityProvStatusReceived]
            (std::shared_ptr<OIC::Service::SecProvisioningStatus > SecProvisioningStatus)
    {
        onSecurityProvStatusReceived->secProvisionStatusCallback(SecProvisioningStatus);
    };

    try
    {
        m_sharedResource->startSecurityProvisioning(secProvStatusCallback);
    }
    catch (ESBadRequestException exception)
    {
        LOGE("JNI startSecurityProvisioning :: Exception occured");
        //throw the exception to java
        throwESException( env,  exception.what());
    }
}

void JniRemoteEnrollee::startDataProvisioning(JNIEnv *env, jobject jListener)
{
    JniDataProvisioningStatusListener *onDataProvStatusReceived =
                    addStatusListener<JniDataProvisioningStatusListener>(env, jListener);

    DataProvStatusCb dataProvStatusCallback = [onDataProvStatusReceived]
            (std::shared_ptr<OIC::Service::DataProvisioningStatus > dataProvisioningStatus)
    {
        onDataProvStatusReceived->dataProvisionStatusCallback(dataProvisioningStatus);
    };

    try
    {
        m_sharedResource->startDataProvisioning(dataProvStatusCallback);
    }
    catch (ESBadRequestException exception)
    {
        LOGE("JNI startDataProvisioning :: Exception occured");
        //throw the exception to java
        throwESException( env,  exception.what());
    }
}

void JniRemoteEnrollee::setDataProvInfo(JNIEnv *env, jstring jssid, jstring jpwd, jint jauthType,
    jint jencType, jstring jlanguage, jstring jcountry)
{
    DataProvInfo info;

    info.WIFI.ssid = env->GetStringUTFChars(jssid, NULL);
    info.WIFI.pwd = env->GetStringUTFChars(jpwd, NULL);
    info.WIFI.authtype = getWifiAuthTypeFromInt(jauthType);
    info.WIFI.enctype = getWifiEncTypeFromInt(jencType);
    info.Device.language = env->GetStringUTFChars(jlanguage, NULL);
    info.Device.country = env->GetStringUTFChars(jcountry, NULL);

    m_sharedResource->setDataProvInfo(info);
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
    JniCloudProvisioningStatusListener *onCloudProvisioningStatusReceived =
                    addStatusListener<JniCloudProvisioningStatusListener>(env, jListener);

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

//JNI
JNIEXPORT void JNICALL
Java_org_iotivity_service_easysetup_mediator_RemoteEnrollee_nativeInitRemoteEnrollee
(JNIEnv *env, jobject jClass)
{
    LOGD("nativeInitRemoteEnrollee Enter");

    JniRemoteEnrollee *remoteEnrollee = JniRemoteEnrollee::getJniRemoteEnrollee(env, jClass);
    remoteEnrollee->initRemoteEnrollee(env);

    LOGD("nativeInitRemoteEnrollee Exit");
}

JNIEXPORT void JNICALL
Java_org_iotivity_service_easysetup_mediator_RemoteEnrollee_nativeRequestPropertyData
(JNIEnv *env, jobject jClass, jobject jListener)
{
    LOGD("nativeRequestPropertyData Enter");

    JniRemoteEnrollee *remoteEnrollee = JniRemoteEnrollee::getJniRemoteEnrollee(env, jClass);
    remoteEnrollee->requestPropertyData(env, jListener);

    LOGD("nativeRequestPropertyData Exit");
}

JNIEXPORT void JNICALL
Java_org_iotivity_service_easysetup_mediator_RemoteEnrollee_nativeStartSecurityProvision
(JNIEnv *env, jobject jClass, jobject jListener)
{
    LOGD("nativeStartSecurityProvision Enter");

    JniRemoteEnrollee *remoteEnrollee = JniRemoteEnrollee::getJniRemoteEnrollee(env, jClass);
    remoteEnrollee->startSecurityProvisioning(env, jListener);

    LOGD("nativeStartSecurityProvision Exit");
}

JNIEXPORT void JNICALL
Java_org_iotivity_service_easysetup_mediator_RemoteEnrollee_nativeStartDataProvision
(JNIEnv *env, jobject jClass, jobject jListener)
{
    LOGD("nativeStartDataProvision Enter");

    JniRemoteEnrollee *remoteEnrollee = JniRemoteEnrollee::getJniRemoteEnrollee(env, jClass);
    remoteEnrollee->startDataProvisioning(env, jListener);

    LOGD("nativeStartDataProvision Exit");
}

JNIEXPORT void JNICALL
Java_org_iotivity_service_easysetup_mediator_RemoteEnrollee_nativeSetDataProvInfo
(JNIEnv *env, jobject jClass, jstring jssid, jstring jpwd, jint jauthType, jint jencType,
    jstring jlanguage, jstring jcountry)
{
    LOGD("nativeSetDataProvInfo Enter");

    JniRemoteEnrollee *remoteEnrollee = JniRemoteEnrollee::getJniRemoteEnrollee(env, jClass);
    remoteEnrollee->setDataProvInfo(env, jssid, jpwd, jauthType, jencType, jlanguage, jcountry);

    LOGD("nativeSetDataProvInfo Exit");
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
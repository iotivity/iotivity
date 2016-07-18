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

void JniRemoteEnrollee::getConfiguration(JNIEnv *env, jobject jListener)
{
    JniGetConfigurationStatusListener *onGetConfigurationStatusReceived =
                    addStatusListener<JniGetConfigurationStatusListener>(env, jListener);

    GetConfigurationStatusCb getConfigurationStatusCallback = [onGetConfigurationStatusReceived]
            (std::shared_ptr<OIC::Service::GetConfigurationStatus > getConfigurationStatus)
    {
        onGetConfigurationStatusReceived->getConfigurationStatusCallback(getConfigurationStatus);
    };

    try
    {
        m_sharedResource->getConfiguration(getConfigurationStatusCallback);
    }
    catch (ESBadRequestException exception)
    {
        LOGE("JNI getConfiguration :: Exception occured");
        //throw the exception to java
        throwESException( env,  exception.what());
    }
}

void JniRemoteEnrollee::configureSecurity(JNIEnv *env, jobject jListener)
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
        m_sharedResource->configureSecurity(secProvStatusCallback);
    }
    catch (ESBadRequestException exception)
    {
        LOGE("JNI configureSecurity :: Exception occured");
        //throw the exception to java
        throwESException( env,  exception.what());
    }
}

void JniRemoteEnrollee::provisionDeviceProperties(JNIEnv *env, jstring jssid, jstring jpwd, jint jauthType,
    jint jencType, jstring jlanguage, jstring jcountry, jobject jListener)
{
    DeviceProp prop;

    prop.WIFI.ssid = env->GetStringUTFChars(jssid, NULL);
    prop.WIFI.pwd = env->GetStringUTFChars(jpwd, NULL);
    prop.WIFI.authtype = getWifiAuthTypeFromInt(jauthType);
    prop.WIFI.enctype = getWifiEncTypeFromInt(jencType);
    prop.Device.language = env->GetStringUTFChars(jlanguage, NULL);
    prop.Device.country = env->GetStringUTFChars(jcountry, NULL);

    JniDevicePropProvisioningStatusListener *onDevicePropProvStatusReceived =
                    addStatusListener<JniDevicePropProvisioningStatusListener>(env, jListener);

    DevicePropProvStatusCb devicePropProvStatusCallback = [onDevicePropProvStatusReceived]
            (std::shared_ptr<OIC::Service::DevicePropProvisioningStatus > devicePropProvisioningStatus)
    {
        onDevicePropProvStatusReceived->onDevicePropProvisioningStatusCallback(devicePropProvisioningStatus);
    };

    try
    {
        m_sharedResource->provisionDeviceProperties(prop, devicePropProvStatusCallback);
    }
    catch (ESBadRequestException exception)
    {
        LOGE("JNI provisionDeviceProperties :: Exception occured");
        //throw the exception to java
        throwESException( env,  exception.what());
    }
}

void JniRemoteEnrollee::provisionCloudProperties(JNIEnv *env, jstring jauthCode, jstring jauthProvider,
    jstring jciServer, jobject jListener)
{
    CloudProp prop;

    prop.authCode = env->GetStringUTFChars(jauthCode, NULL);
    prop.authProvider = env->GetStringUTFChars(jauthProvider, NULL);
    prop.ciServer = env->GetStringUTFChars(jciServer, NULL);

    JniCloudPropProvisioningStatusListener *onCloudPropProvisioningStatusReceived =
                    addStatusListener<JniCloudPropProvisioningStatusListener>(env, jListener);

    CloudPropProvStatusCb cloudPropProvStatusCallback = [onCloudPropProvisioningStatusReceived]
            (std::shared_ptr< OIC::Service::CloudPropProvisioningStatus > cloudPropProvisioningStatus)

    {
        onCloudPropProvisioningStatusReceived->onCloudPropProvisioningStatus(cloudPropProvisioningStatus);
    };

    try
    {
        m_sharedResource->provisionCloudProperties(prop, cloudPropProvStatusCallback);
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
Java_org_iotivity_service_easysetup_mediator_RemoteEnrollee_nativeGetConfiguration
(JNIEnv *env, jobject jClass, jobject jListener)
{
    LOGD("nativegetConfiguration Enter");

    JniRemoteEnrollee *remoteEnrollee = JniRemoteEnrollee::getJniRemoteEnrollee(env, jClass);
    remoteEnrollee->getConfiguration(env, jListener);

    LOGD("nativegetConfiguration Exit");
}

JNIEXPORT void JNICALL
Java_org_iotivity_service_easysetup_mediator_RemoteEnrollee_nativeConfigureSecurity
(JNIEnv *env, jobject jClass, jobject jListener)
{
    LOGD("nativeStartSecurityProvision Enter");

    JniRemoteEnrollee *remoteEnrollee = JniRemoteEnrollee::getJniRemoteEnrollee(env, jClass);
    remoteEnrollee->configureSecurity(env, jListener);

    LOGD("nativeStartSecurityProvision Exit");
}

JNIEXPORT void JNICALL
Java_org_iotivity_service_easysetup_mediator_RemoteEnrollee_nativeProvisionDeviceProperties
(JNIEnv *env, jobject jClass, jstring jssid, jstring jpwd, jint jauthType,
    jint jencType, jstring jlanguage, jstring jcountry, jobject jListener)
{
    LOGD("nativeProvisionDeviceProperties Enter");

    JniRemoteEnrollee *remoteEnrollee = JniRemoteEnrollee::getJniRemoteEnrollee(env, jClass);
    remoteEnrollee->provisionDeviceProperties(env, jssid, jpwd, jauthType, jencType,
                                                                            jlanguage, jcountry, jListener);

    LOGD("nativeProvisionDeviceProperties Exit");
}

JNIEXPORT void JNICALL
Java_org_iotivity_service_easysetup_mediator_RemoteEnrollee_nativeProvisionCloudProperties
(JNIEnv *env, jobject jClass, jstring authCode, jstring authProvider,
    jstring ciServer, jobject jListener)
{
    LOGD("nativeprovisionCloudProperties Enter");

    JniRemoteEnrollee *remoteEnrollee = JniRemoteEnrollee::getJniRemoteEnrollee(env, jClass);
    remoteEnrollee->provisionCloudProperties(env, authCode, authProvider, ciServer, jListener);

    LOGD("nativeprovisionCloudProperties Exit");
}
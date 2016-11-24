//******************************************************************
//
// Copyright 2015 Intel Corporation.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include "JniOcPlatform.h"
#include "OCPlatform.h"
#include "JniOcResource.h"
#include "JniOcResourceHandle.h"
#include "JniOcPresenceHandle.h"
#include "JniOcResourceResponse.h"
#include "JniOcSecurity.h"
#include "JniOcDirectPairDevice.h"
#include "JniUtils.h"
#include "ocpayload.h"
#include "RDClient.h"

#ifdef WITH_CLOUD
#include "JniOcAccountManager.h"
#endif

using namespace OC;

JniOnResourceFoundListener* AddOnResourceFoundListener(JNIEnv* env, jobject jListener)
{
    JniOnResourceFoundListener *onResourceFoundListener = nullptr;

    resourceFoundMapLock.lock();

    for (auto it = onResourceFoundListenerMap.begin();
         it != onResourceFoundListenerMap.end();
         ++it)
    {
        if (env->IsSameObject(jListener, it->first))
        {
            auto refPair = it->second;
            onResourceFoundListener = refPair.first;
            refPair.second++;
            it->second = refPair;
            onResourceFoundListenerMap.insert(*it);
            LOGD("OnResourceFoundListener: ref. count incremented");
            break;
        }
    }

    if (!onResourceFoundListener)
    {
        onResourceFoundListener =
            new JniOnResourceFoundListener(env, jListener, RemoveOnResourceFoundListener);
        jobject jgListener = env->NewGlobalRef(jListener);

        onResourceFoundListenerMap.insert(
            std::pair<jobject,
                      std::pair<JniOnResourceFoundListener*, int >>(
                          jgListener,
                          std::pair<JniOnResourceFoundListener*, int>(onResourceFoundListener,
                                                                      1)));
        LOGD("OnResourceFoundListener: new listener");
    }
    resourceFoundMapLock.unlock();
    return onResourceFoundListener;
}

void RemoveOnResourceFoundListener(JNIEnv* env, jobject jListener)
{
    resourceFoundMapLock.lock();

    for (auto it = onResourceFoundListenerMap.begin();
         it != onResourceFoundListenerMap.end();
         ++it)
    {
        if (env->IsSameObject(jListener, it->first))
        {
            auto refPair = it->second;
            if (refPair.second > 1)
            {
                refPair.second--;
                it->second = refPair;
                onResourceFoundListenerMap.insert(*it);
                LOGI("OnResourceFoundListener: ref. count decremented");
            }
            else
            {
                env->DeleteGlobalRef(it->first);
                JniOnResourceFoundListener* listener = refPair.first;
                delete listener;
                onResourceFoundListenerMap.erase(it);
                LOGI("OnResourceFoundListener removed");
            }
            break;
        }
    }
    resourceFoundMapLock.unlock();
}

JniOnDeviceInfoListener* AddOnDeviceInfoListener(JNIEnv* env, jobject jListener)
{
    JniOnDeviceInfoListener *onDeviceInfoListener = nullptr;

    deviceInfoMapLock.lock();

    for (auto it = onDeviceInfoListenerMap.begin(); it != onDeviceInfoListenerMap.end(); ++it)
    {
        if (env->IsSameObject(jListener, it->first))
        {
            auto refPair = it->second;
            onDeviceInfoListener = refPair.first;
            refPair.second++;
            it->second = refPair;
            onDeviceInfoListenerMap.insert(*it);
            LOGD("OnDeviceInfoListener: ref. count incremented");
            break;
        }
    }

    if (!onDeviceInfoListener)
    {
        onDeviceInfoListener =
            new JniOnDeviceInfoListener(env, jListener, RemoveOnDeviceInfoListener);
        jobject jgListener = env->NewGlobalRef(jListener);

        onDeviceInfoListenerMap.insert(
            std::pair<jobject,
                      std::pair<JniOnDeviceInfoListener*, int>>(
                          jgListener,
                          std::pair<JniOnDeviceInfoListener*, int>(onDeviceInfoListener, 1)));
        LOGI("OnDeviceInfoListener: new listener");
    }

    deviceInfoMapLock.unlock();
    return onDeviceInfoListener;
}

void RemoveOnDeviceInfoListener(JNIEnv* env, jobject jListener)
{
    deviceInfoMapLock.lock();
    bool isFound = false;
    for (auto it = onDeviceInfoListenerMap.begin(); it != onDeviceInfoListenerMap.end(); ++it)
    {
        if (env->IsSameObject(jListener, it->first))
        {
            auto refPair = it->second;
            if (refPair.second > 1)
            {
                refPair.second--;
                it->second = refPair;
                onDeviceInfoListenerMap.insert(*it);
                LOGI("OnDeviceInfoListener: ref. count decremented");
            }
            else
            {
                env->DeleteGlobalRef(it->first);
                JniOnDeviceInfoListener* listener = refPair.first;
                delete listener;
                onDeviceInfoListenerMap.erase(it);

                LOGI("OnDeviceInfoListener removed");
            }

            isFound = true;
            break;
        }
    }

    if (!isFound)
    {
        ThrowOcException(JNI_EXCEPTION, "OnDeviceInfoListenet not found");
    }
    deviceInfoMapLock.unlock();
}

JniOnPlatformInfoListener* AddOnPlatformInfoListener(JNIEnv* env, jobject jListener)
{
    JniOnPlatformInfoListener *onPlatformInfoListener = nullptr;

    platformInfoMapLock.lock();

    for (auto it = onPlatformInfoListenerMap.begin(); it != onPlatformInfoListenerMap.end(); ++it)
    {
        if (env->IsSameObject(jListener, it->first))
        {
            auto refPair = it->second;
            onPlatformInfoListener = refPair.first;
            refPair.second++;
            it->second = refPair;
            onPlatformInfoListenerMap.insert(*it);
            LOGD("OnPlatformInfoListener: ref. count incremented");
            break;
        }
    }

    if (!onPlatformInfoListener)
    {
        onPlatformInfoListener =
            new JniOnPlatformInfoListener(env, jListener, RemoveOnPlatformInfoListener);
        jobject jgListener = env->NewGlobalRef(jListener);

        onPlatformInfoListenerMap.insert(
            std::pair<jobject, std::pair<JniOnPlatformInfoListener*, int>>(
                jgListener,
                std::pair<JniOnPlatformInfoListener*, int>(onPlatformInfoListener, 1)));
        LOGI("OnPlatformInfoListener: new listener");
    }

    platformInfoMapLock.unlock();
    return onPlatformInfoListener;
}

void RemoveOnPlatformInfoListener(JNIEnv* env, jobject jListener)
{
    platformInfoMapLock.lock();
    bool isFound = false;
    for (auto it = onPlatformInfoListenerMap.begin(); it != onPlatformInfoListenerMap.end(); ++it)
    {
        if (env->IsSameObject(jListener, it->first))
        {
            auto refPair = it->second;
            if (refPair.second > 1)
            {
                refPair.second--;
                it->second = refPair;
                onPlatformInfoListenerMap.insert(*it);
                LOGI("OnPlatformInfoListener: ref. count decremented");
            }
            else
            {
                env->DeleteGlobalRef(it->first);
                JniOnPlatformInfoListener* listener = refPair.first;
                delete listener;
                onPlatformInfoListenerMap.erase(it);

                LOGI("OnPlatformInfoListener removed");
            }

            isFound = true;
            break;
        }
    }

    if (!isFound)
    {
        ThrowOcException(JNI_EXCEPTION, "OnPlatformInfoListenet not found");
    }
    platformInfoMapLock.unlock();
}

JniOnPresenceListener* AddOnPresenceListener(JNIEnv* env, jobject jListener)
{
    JniOnPresenceListener *onPresenceListener = nullptr;

    presenceMapLock.lock();

    for (auto it = onPresenceListenerMap.begin(); it != onPresenceListenerMap.end(); ++it)
    {
        if (env->IsSameObject(jListener, it->first))
        {
            auto refPair = it->second;
            onPresenceListener = refPair.first;
            refPair.second++;
            it->second = refPair;
            onPresenceListenerMap.insert(*it);
            LOGD("OnPresenceListener: ref. count incremented");
            break;
        }
    }
    if (!onPresenceListener)
    {
        onPresenceListener = new JniOnPresenceListener(env, jListener, RemoveOnPresenceListener);
        jobject jgListener = env->NewGlobalRef(jListener);
        onPresenceListenerMap.insert(
            std::pair<jobject, std::pair<JniOnPresenceListener*, int>>(
                jgListener,
                std::pair<JniOnPresenceListener*, int>(onPresenceListener, 1)));
        LOGI("OnPresenceListener: new listener");
    }
    presenceMapLock.unlock();
    return onPresenceListener;
}

void RemoveOnPresenceListener(JNIEnv* env, jobject jListener)
{
    presenceMapLock.lock();
    bool isFound = false;
    for (auto it = onPresenceListenerMap.begin(); it != onPresenceListenerMap.end(); ++it)
    {
        if (env->IsSameObject(jListener, it->first))
        {
            auto refPair = it->second;
            if (refPair.second > 1)
            {
                refPair.second--;
                it->second = refPair;
                onPresenceListenerMap.insert(*it);
                LOGI("OnPresenceListener: ref. count decremented");
            }
            else
            {
                env->DeleteGlobalRef(it->first);
                JniOnPresenceListener* listener = refPair.first;
                delete listener;
                onPresenceListenerMap.erase(it);
                LOGI("OnPresenceListener is removed");
            }
            isFound = true;
            break;
        }
    }
    if (!isFound)
    {
        ThrowOcException(JNI_EXCEPTION, "OnPresenceListener not found");
    }
    presenceMapLock.unlock();
}

JniOnObserveListener* AddOnObserveListener(JNIEnv* env, jobject jListener)
{
    JniOnObserveListener *onObserveListener = nullptr;

    observeMapLock.lock();

    for (auto it = onObserveListenerMap.begin(); it != onObserveListenerMap.end(); ++it)
    {
        if (env->IsSameObject(jListener, it->first))
        {
            auto refPair = it->second;
            onObserveListener = refPair.first;
            refPair.second++;
            it->second = refPair;
            onObserveListenerMap.insert(*it);
            LOGD("OnObserveListener: ref. count incremented");
            break;
        }
    }
    if (!onObserveListener)
    {
        onObserveListener = new JniOnObserveListener(env, jListener, (JniOcResource*)nullptr);
        jobject jgListener = env->NewGlobalRef(jListener);
        onObserveListenerMap.insert(
            std::pair<jobject, std::pair<JniOnObserveListener*, int>>(
                jgListener,
                std::pair<JniOnObserveListener*, int>(onObserveListener, 1)));
        LOGI("OnObserveListener: new listener");
    }
    observeMapLock.unlock();
    return onObserveListener;
}

void RemoveOnObserveListener(JNIEnv* env, jobject jListener)
{
    observeMapLock.lock();
    bool isFound = false;
    for (auto it = onObserveListenerMap.begin(); it != onObserveListenerMap.end(); ++it)
    {
        if (env->IsSameObject(jListener, it->first))
        {
            auto refPair = it->second;
            if (refPair.second > 1)
            {
                refPair.second--;
                it->second = refPair;
                onObserveListenerMap.insert(*it);
                LOGI("OnObserveListener: ref. count decremented");
            }
            else
            {
                env->DeleteGlobalRef(it->first);
                JniOnObserveListener* listener = refPair.first;
                delete listener;
                onObserveListenerMap.erase(it);
                LOGI("OnObserveListener is removed");
            }
            isFound = true;
            break;
        }
    }
    if (!isFound)
    {
        ThrowOcException(JNI_EXCEPTION, "OnObserveListener not found");
    }
    observeMapLock.unlock();
}

JniOnDPDevicesFoundListener* AddOnDPDevicesFoundListener(JNIEnv* env, jobject jListener)
{
    JniOnDPDevicesFoundListener *onDPDeviceListener = nullptr;

    dpDevicesFoundListenerMapLock.lock();

    for (auto it = onDPDevicesFoundListenerMap.begin(); it !=
            onDPDevicesFoundListenerMap.end(); ++it)
    {
        if (env->IsSameObject(jListener, it->first))
        {
            auto refPair = it->second;
            onDPDeviceListener = refPair.first;
            refPair.second++;
            it->second = refPair;
            onDPDevicesFoundListenerMap.insert(*it);
            LOGD("onDPDeviceListener: ref. count incremented");
            break;
        }
    }
    if (!onDPDeviceListener)
    {
        onDPDeviceListener = new JniOnDPDevicesFoundListener(env, jListener,
                RemoveOnDPDevicesFoundListener);
        jobject jgListener = env->NewGlobalRef(jListener);
        onDPDevicesFoundListenerMap.insert(
                std::pair<jobject, std::pair<JniOnDPDevicesFoundListener*, int>>(
                    jgListener,
                    std::pair<JniOnDPDevicesFoundListener*, int>(onDPDeviceListener, 1)));
        LOGI("onDPDeviceListener: new listener");
    }
    dpDevicesFoundListenerMapLock.unlock();
    return onDPDeviceListener;
}

void RemoveOnDPDevicesFoundListener(JNIEnv* env, jobject jListener)
{
    dpDevicesFoundListenerMapLock.lock();
    bool isFound = false;
    for (auto it = onDPDevicesFoundListenerMap.begin(); it !=
            onDPDevicesFoundListenerMap.end(); ++it)
    {
        if (env->IsSameObject(jListener, it->first))
        {
            auto refPair = it->second;
            if (refPair.second > 1)
            {
                refPair.second--;
                it->second = refPair;
                onDPDevicesFoundListenerMap.insert(*it);
                LOGI("onDPDeviceListener: ref. count decremented");
            }
            else
            {
                env->DeleteGlobalRef(it->first);
                JniOnDPDevicesFoundListener* listener = refPair.first;
                delete listener;
                onDPDevicesFoundListenerMap.erase(it);
                LOGI("onDPDeviceListener is removed");
            }
            isFound = true;
            break;
        }
    }
    if (!isFound)
    {
        ThrowOcException(JNI_EXCEPTION, "onDPDeviceListener not found");
    }
    dpDevicesFoundListenerMapLock.unlock();
}

JniOnDirectPairingListener* AddOnDirectPairingListener(JNIEnv* env, jobject jListener)
{
    JniOnDirectPairingListener *onDirectPairingListener = nullptr;

    directPairingListenerMapLock.lock();

    for (auto it = directPairingListenerMap.begin(); it !=
            directPairingListenerMap.end(); ++it)
    {
        if (env->IsSameObject(jListener, it->first))
        {
            auto refPair = it->second;
            onDirectPairingListener = refPair.first;
            refPair.second++;
            it->second = refPair;
            directPairingListenerMap.insert(*it);
            LOGD("onDirectPairingListener: ref. count incremented");
            break;
        }
    }
    if (!onDirectPairingListener)
    {
        onDirectPairingListener = new JniOnDirectPairingListener(env, jListener,
                RemoveOnDirectPairingListener);
        jobject jgListener = env->NewGlobalRef(jListener);
        directPairingListenerMap.insert(
                std::pair<jobject, std::pair<JniOnDirectPairingListener*, int>>(
                    jgListener,
                    std::pair<JniOnDirectPairingListener*, int>(onDirectPairingListener, 1)));
        LOGI("onDirectPairingListener: new listener");
    }
    directPairingListenerMapLock.unlock();
    return onDirectPairingListener;
}

void RemoveOnDirectPairingListener(JNIEnv* env, jobject jListener)
{
    directPairingListenerMapLock.lock();
    bool isFound = false;
    for (auto it = directPairingListenerMap.begin(); it !=
            directPairingListenerMap.end(); ++it)
    {
        if (env->IsSameObject(jListener, it->first))
        {
            auto refPair = it->second;
            if (refPair.second > 1)
            {
                refPair.second--;
                it->second = refPair;
                directPairingListenerMap.insert(*it);
                LOGI("onDirectPairingListener: ref. count decremented");
            }
            else
            {
                env->DeleteGlobalRef(it->first);
                JniOnDirectPairingListener* listener = refPair.first;
                delete listener;
                directPairingListenerMap.erase(it);
                LOGI("onDirectPairingListener is removed");
            }
            isFound = true;
            break;
        }
    }
    if (!isFound)
    {
        ThrowOcException(JNI_EXCEPTION, "onDirectPairingListener not found");
    }
    directPairingListenerMapLock.unlock();
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    configure
* Signature: (IILjava/lang/String;II)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_configure
(JNIEnv *env, jclass clazz, jint jServiceType, jint jModeType, jstring jIpAddress, jint jPort,
                                                                 jint jQOS, jstring jDbPath)
{
    LOGI("OcPlatform_configure");

    std::string ipAddress;
    std::string dbfile;
    if (jIpAddress)
    {
        ipAddress = env->GetStringUTFChars(jIpAddress, nullptr);
    }
    if (jDbPath)
    {
        dbfile = env->GetStringUTFChars(jDbPath, nullptr);
        JniOcSecurity::StoreDbPath(dbfile);
    }
    uint16_t port = 0;
    if (jPort > 0)
    {
        port = static_cast<uint16_t>(jPort);
    }

    PlatformConfig cfg{
        JniUtils::getServiceType(env, jServiceType),
        JniUtils::getModeType(env, jModeType),
        ipAddress,
        port,
        JniUtils::getQOS(env, static_cast<int>(jQOS)),
        JniOcSecurity::getOCPersistentStorage()
    };
    OCPlatform::Configure(cfg);
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    notifyAllObservers0
* Signature: (Lorg/iotivity/base/OcResourceHandle;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_notifyAllObservers0
(JNIEnv *env, jclass clazz, jobject jResourceHandle)
{
    LOGI("OcPlatform_notifyAllObservers");
    if (!jResourceHandle)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "resourceHandle cannot be null");
        return;
    }

    JniOcResourceHandle* jniOcResourceHandle = JniOcResourceHandle::getJniOcResourceHandlePtr(
        env, jResourceHandle);
    if (!jniOcResourceHandle)
    {
        return;
    }

    try
    {
        OCStackResult result =
            OCPlatform::notifyAllObservers(jniOcResourceHandle->getOCResourceHandle());

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Failed to notify all observers");
            return;
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    notifyAllObservers1
* Signature: (Lorg/iotivity/base/OcResourceHandle;I)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_notifyAllObservers1
(JNIEnv *env, jclass clazz, jobject jResourceHandle, jint jQoS)
{
    LOGI("OcPlatform_notifyAllObservers1");

    if (!jResourceHandle)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "resourceHandle cannot be null");
        return;
    }

    JniOcResourceHandle* jniOcResourceHandle =
        JniOcResourceHandle::getJniOcResourceHandlePtr(env, jResourceHandle);
    if (!jniOcResourceHandle)
    {
        return;
    }

    try {
        OCStackResult result = OCPlatform::notifyAllObservers(
            jniOcResourceHandle->getOCResourceHandle(),
            JniUtils::getQOS(env, static_cast<int>(jQoS)));

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Failed to notify all observers");
            return;
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    notifyListOfObservers2
* Signature: (Lorg/iotivity/base/OcResourceHandle;[Ljava/lang/Byte;Lorg/iotivity/base/OcResourceResponse;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_notifyListOfObservers2(
    JNIEnv *env,
    jclass clazz,
    jobject jResourceHandle,
    jbyteArray jObservationIdArr,
    jobject jResourceResponse)
{
    LOGD("OcPlatform_notifyListOfObservers2");
    if (!jResourceHandle)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "resourceHandle cannot be null");
        return;
    }
    if (!jObservationIdArr)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "observationIdList cannot be null");
        return;
    }
    if (!jResourceResponse)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "resourceResponse cannot be null");
        return;
    }

    JniOcResourceHandle* jniOcResourceHandle =
        JniOcResourceHandle::getJniOcResourceHandlePtr(env, jResourceHandle);
    if (!jniOcResourceHandle)
    {
        return;
    }

    JniOcResourceResponse* jniOcResourceResponse =
        JniOcResourceResponse::getJniOcResourceResponsePtr(env, jResourceResponse);
    if (!jniOcResourceResponse)
    {
        return;
    }

    int len = env->GetArrayLength(jObservationIdArr);
    uint8_t* bArr = (uint8_t*)env->GetByteArrayElements(jObservationIdArr, 0);

    ObservationIds observationIds;
    for (int i = 0; i < len; ++i)
    {
        observationIds.push_back(bArr[i]);
    }

    env->ReleaseByteArrayElements(jObservationIdArr, (jbyte*)bArr, 0);

    try
    {
        OCStackResult result = OCPlatform::notifyListOfObservers(
            jniOcResourceHandle->getOCResourceHandle(),
            observationIds,
            jniOcResourceResponse->getOCResourceResponse());

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Failed to notify all observers");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    notifyListOfObservers3
* Signature: (Lorg/iotivity/base/OcResourceHandle;[Ljava/lang/Byte;Lorg/iotivity/base/OcResourceResponse;I)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_notifyListOfObservers3(
    JNIEnv *env,
    jclass clazz,
    jobject jResourceHandle,
    jbyteArray jObservationIdArr,
    jobject jResourceResponse,
    jint jQoS)
{
    LOGD("OcPlatform_notifyListOfObservers3");
    if (!jResourceHandle)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "resourceHandle cannot be null");
        return;
    }
    if (!jObservationIdArr)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "observationIdList cannot be null");
        return;
    }
    if (!jResourceResponse)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "resourceResponse cannot be null");
        return;
    }

    JniOcResourceHandle* jniOcResourceHandle =
        JniOcResourceHandle::getJniOcResourceHandlePtr(env, jResourceHandle);
    if (!jniOcResourceHandle)
    {
        return;
    }

    JniOcResourceResponse* jniOcResourceResponse =
        JniOcResourceResponse::getJniOcResourceResponsePtr(env, jResourceResponse);
    if (!jniOcResourceResponse)
    {
        return;
    }

    int len = env->GetArrayLength(jObservationIdArr);
    uint8_t* bArr = (uint8_t*)env->GetByteArrayElements(jObservationIdArr, 0);

    ObservationIds observationIds;
    for (int i = 0; i < len; ++i)
    {
        observationIds.push_back(bArr[i]);
    }

    env->ReleaseByteArrayElements(jObservationIdArr, (jbyte*)bArr, 0);

    try
    {
        OCStackResult result = OCPlatform::notifyListOfObservers(
            jniOcResourceHandle->getOCResourceHandle(),
            observationIds,
            jniOcResourceResponse->getOCResourceResponse(),
            JniUtils::getQOS(env, static_cast<int>(jQoS)));

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Failed to notify all observers");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    findResource0
* Signature: (Ljava/lang/String;Ljava/lang/String;ILorg/iotivity/base/OcPlatform/OnResourceFoundListener;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_findResource0(
    JNIEnv *env,
    jclass clazz,
    jstring jHost,
    jstring jResourceUri,
    jint jConnectivityType,
    jobject jListener)
{
    LOGD("OcPlatform_findResource");
    std::string host;
    if (jHost)
    {
        host = env->GetStringUTFChars(jHost, nullptr);
    }
    std::string resourceUri;
    if (jResourceUri)
    {
        resourceUri = env->GetStringUTFChars(jResourceUri, nullptr);
    }
    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "onResourceFoundListener cannot be null");
        return;
    }

    JniOnResourceFoundListener *onResFoundListener = AddOnResourceFoundListener(env, jListener);

    FindCallback findCallback = [onResFoundListener](std::shared_ptr<OCResource> resource)
    {
        onResFoundListener->foundResourceCallback(resource);
    };

    FindErrorCallback findErrorCallback = [onResFoundListener](const std::string& uri, const int eCode)
    {
        onResFoundListener->findResourceErrorCallback(uri, eCode);
    };

    try
    {
        OCStackResult result = OCPlatform::findResource(
            host,
            resourceUri,
            static_cast<OCConnectivityType>(jConnectivityType),
            findCallback,
            findErrorCallback);

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Find resource has failed");
            return;
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    findResource1
* Signature: (Ljava/lang/String;Ljava/lang/String;ILorg/iotivity/base/OcPlatform/OnResourceFoundListener;I)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_findResource1(
    JNIEnv *env,
    jclass clazz,
    jstring jHost,
    jstring jResourceUri,
    jint jConnectivityType,
    jobject jListener,
    jint jQoS)
{
    LOGD("OcPlatform_findResource");
    std::string host;
    if (jHost)
    {
        host = env->GetStringUTFChars(jHost, nullptr);
    }
    std::string resourceUri;
    if (jResourceUri)
    {
        resourceUri = env->GetStringUTFChars(jResourceUri, nullptr);
    }
    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "onResourceFoundListener cannot be null");
        return;
    }
    JniOnResourceFoundListener *onResFoundListener = AddOnResourceFoundListener(env, jListener);

    FindCallback findCallback = [onResFoundListener](std::shared_ptr<OCResource> resource)
    {
        onResFoundListener->foundResourceCallback(resource);
    };

    FindErrorCallback findErrorCallback = [onResFoundListener](const std::string& uri, const int eCode)
    {
        onResFoundListener->findResourceErrorCallback(uri, eCode);
    };

    try
    {
        OCStackResult result = OCPlatform::findResource(
            host,
            resourceUri,
            static_cast<OCConnectivityType>(jConnectivityType),
            findCallback,
            findErrorCallback,
            JniUtils::getQOS(env, static_cast<int>(jQoS)));

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Find resource has failed");
            return;
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
 * Class:     org_iotivity_base_OcPlatform
 * Method:    findDirectPairingDevices
 * Signature: (ILorg/iotivity/base/OcPlatform/FindDirectPairingListener;)V
 */
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_findDirectPairingDevices
  (JNIEnv * env, jclass clazz, jint jTimeout, jobject jListener)
{
    LOGD("OcPlatform_findDirectPairingDevices");

    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "onDPDevicesFoundListener cannot be null");
        return;
    }
    JniOnDPDevicesFoundListener *onDPDevsFoundListener = AddOnDPDevicesFoundListener(env,
            jListener);

    GetDirectPairedCallback getDirectPairedCallback =
        [onDPDevsFoundListener](PairedDevices pairingDevList)
        {
            onDPDevsFoundListener->directPairingDevicesCallback(pairingDevList,
                    DPFunc::FIND_DIRECT_PAIRED_DEV_LIST);
        };

    try
    {
        OCStackResult result = OCPlatform::findDirectPairingDevices(jTimeout,
                getDirectPairedCallback);
        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "OCPlatform::findDirectPairingDevices has failed");
            return;
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
 * Class:     org_iotivity_base_OcPlatform
 * Method:    getDirectPairedDevices
 * Signature: (Lorg/iotivity/base/OcDirectPairDevice/GetDirectPairedListener;)V
 */
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_getDirectPairedDevices
(JNIEnv *env, jclass jclazz, jobject jListener)
{
    LOGD("OcPlatform_getDirectPairedDevices");

    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "getPairedDevList Callback cannot be null");
        return;
    }
    JniOnDPDevicesFoundListener *onGetPairedDevicesListener = AddOnDPDevicesFoundListener(env,
            jListener);

    GetDirectPairedCallback getDirectPairedCallback =
        [onGetPairedDevicesListener](PairedDevices pairedDevList)
        {
            onGetPairedDevicesListener->directPairingDevicesCallback(pairedDevList,
                    DPFunc::GET_PAIRED_DEV_LIST);
        };

    try
    {
        OCStackResult result = OCPlatform::getDirectPairedDevices(getDirectPairedCallback);
        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "OcDirectPairDevice_getDirectPairedDevices");
            return;
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
 * Class:     org_iotivity_base_OcPlatform
 * Method:    doDirectPairing
 * Signature: (Lorg/iotivity/base/OcDirectPairDevice;Lorg/iotivity/base/OcPrmType;
 *           Ljava/lang/String;Lorg/iotivity/base/OcDirectPairDevice/DirectPairingListener;)V
 */
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_doDirectPairing0
(JNIEnv *env, jclass clazz, jobject jpeer, jint jprmType, jstring jpin, jobject jListener)
{
    LOGD("OcPlatform_doDirectPairing");

    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "doDirectPairing Callback cannot be null");
        return;
    }
    if (!jpeer)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "Peer cannot be null");
        return;
    }

    JniOnDirectPairingListener *onDirectPairingListener = AddOnDirectPairingListener(env,
            jListener);

    DirectPairingCallback DirectPairingCB =
        [onDirectPairingListener](std::shared_ptr<OCDirectPairing> dpDev, OCStackResult result)
        {
            onDirectPairingListener->doDirectPairingCB(dpDev, result);
        };

    JniOcDirectPairDevice *dev = JniOcDirectPairDevice::getJniOcDirectPairDevicePtr(env, jpeer);

    if (!dev)
    {
        return ;
    }
    std::string pin = env->GetStringUTFChars(jpin, 0);

    try
    {
        OCStackResult result = OCPlatform::doDirectPairing(dev->getPtr(), (OCPrm_t)jprmType,
                pin, DirectPairingCB);
        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "OcPlatform_oDirectPairing");
            return;
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    getDeviceInfo0
* Signature: (Ljava/lang/String;Ljava/lang/String;ILorg/iotivity/base/OcPlatform/OnDeviceFoundListener;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_getDeviceInfo0(
    JNIEnv *env,
    jclass clazz,
    jstring jHost,
    jstring jResourceUri,
    jint jConnectivityType,
    jobject jListener)
{
    LOGD("OcPlatform_getDeviceInfo0");
    std::string host;
    if (jHost)
    {
        host = env->GetStringUTFChars(jHost, nullptr);
    }
    std::string resourceUri;
    if (jResourceUri)
    {
        resourceUri = env->GetStringUTFChars(jResourceUri, nullptr);
    }
    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "onDeviceFoundListener cannot be null");
        return;
    }
    JniOnDeviceInfoListener *onDeviceInfoListener = AddOnDeviceInfoListener(env, jListener);

    FindDeviceCallback findDeviceCallback =
        [onDeviceInfoListener](const OCRepresentation& ocRepresentation)
        {
            onDeviceInfoListener->foundDeviceCallback(ocRepresentation);
        };

    try
    {
        OCStackResult result = OCPlatform::getDeviceInfo(
            host,
            resourceUri,
            static_cast<OCConnectivityType>(jConnectivityType),
            findDeviceCallback);

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Find device has failed");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    getDeviceInfo1
* Signature: (Ljava/lang/String;Ljava/lang/String;ILorg/iotivity/base/OcPlatform/OnDeviceFoundListener;I)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_getDeviceInfo1(
    JNIEnv *env,
    jclass clazz,
    jstring jHost,
    jstring jResourceUri,
    jint jConnectivityType,
    jobject jListener,
    jint jQoS)
{
    LOGD("OcPlatform_getDeviceInfo1");
    std::string host;
    if (jHost)
    {
        host = env->GetStringUTFChars(jHost, nullptr);
    }
    std::string resourceUri;
    if (jResourceUri)
    {
        resourceUri = env->GetStringUTFChars(jResourceUri, nullptr);
    }
    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "onDeviceFoundListener cannot be null");
        return;
    }
    JniOnDeviceInfoListener *onDeviceInfoListener = AddOnDeviceInfoListener(env, jListener);

    FindDeviceCallback findDeviceCallback =
        [onDeviceInfoListener](const OCRepresentation& ocRepresentation)
        {
            onDeviceInfoListener->foundDeviceCallback(ocRepresentation);
        };

    try
    {
        OCStackResult result = OCPlatform::getDeviceInfo(
            host,
            resourceUri,
            static_cast<OCConnectivityType>(jConnectivityType),
            findDeviceCallback,
            JniUtils::getQOS(env, static_cast<int>(jQoS)));

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Find device has failed");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    getPlatformInfo0
* Signature: (Ljava/lang/String;Ljava/lang/String;ILorg/iotivity/base/OcPlatform/OnPlatformFoundListener;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_getPlatformInfo0(
    JNIEnv *env,
    jclass clazz,
    jstring jHost,
    jstring jResourceUri,
    jint jConnectivityType,
    jobject jListener)
{
    LOGD("OcPlatform_getPlatformInfo0");
    std::string host;
    if (jHost)
    {
        host = env->GetStringUTFChars(jHost, nullptr);
    }
    std::string resourceUri;
    if (jResourceUri)
    {
        resourceUri = env->GetStringUTFChars(jResourceUri, nullptr);
    }
    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "onPlatformFoundListener cannot be null");
        return;
    }
    JniOnPlatformInfoListener *onPlatformInfoListener = AddOnPlatformInfoListener(env, jListener);

    FindPlatformCallback findPlatformCallback =
        [onPlatformInfoListener](const OCRepresentation& ocRepresentation)
        {
            onPlatformInfoListener->foundPlatformCallback(ocRepresentation);
        };

    try
    {
        OCStackResult result = OCPlatform::getPlatformInfo(
            host,
            resourceUri,
            static_cast<OCConnectivityType>(jConnectivityType),
            findPlatformCallback);

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Find platform has failed");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    getPlatformInfo1
* Signature: (Ljava/lang/String;Ljava/lang/String;ILorg/iotivity/base/OcPlatform/OnPlatformFoundListener;I)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_getPlatformInfo1(
    JNIEnv *env,
    jclass clazz,
    jstring jHost,
    jstring jResourceUri,
    jint jConnectivityType,
    jobject jListener,
    jint jQoS)
{
    LOGD("OcPlatform_getPlatformInfo1");
    std::string host;
    if (jHost)
    {
        host = env->GetStringUTFChars(jHost, nullptr);
    }
    std::string resourceUri;
    if (jResourceUri)
    {
        resourceUri = env->GetStringUTFChars(jResourceUri, nullptr);
    }
    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "onPlatformFoundListener cannot be null");
        return;
    }
    JniOnPlatformInfoListener *onPlatformInfoListener = AddOnPlatformInfoListener(env, jListener);

    FindPlatformCallback findPlatformCallback =
        [onPlatformInfoListener](const OCRepresentation& ocRepresentation)
        {
            onPlatformInfoListener->foundPlatformCallback(ocRepresentation);
        };

    try
    {
        OCStackResult result = OCPlatform::getPlatformInfo(
            host,
            resourceUri,
            static_cast<OCConnectivityType>(jConnectivityType),
            findPlatformCallback,
            JniUtils::getQOS(env, static_cast<int>(jQoS)));

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Find platform has failed");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    registerResource0
* Signature: (Lorg/iotivity/base/OcResource;)Lorg/iotivity/base/OcResourceHandle;
*/
JNIEXPORT jobject JNICALL Java_org_iotivity_base_OcPlatform_registerResource0(
    JNIEnv *env, jclass clazz, jobject jResource)
{
    LOGD("OcPlatform_registerResource");
    if (!jResource)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "Resource cannot be null");
        return nullptr;
    }
    JniOcResource *resource = JniOcResource::getJniOcResourcePtr(env, jResource);
    if (!resource)
    {
        return nullptr;
    }

    OCResourceHandle resourceHandle;
    try
    {
        OCStackResult result = OCPlatform::registerResource(
            resourceHandle,
            resource->getOCResource());

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "register resource");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
        return nullptr;
    }
    JniOcResourceHandle* jniHandle = new JniOcResourceHandle(resourceHandle);
    jlong handle = reinterpret_cast<jlong>(jniHandle);
    jobject jResourceHandle =
        env->NewObject(g_cls_OcResourceHandle, g_mid_OcResourceHandle_N_ctor, handle);

    if (!jResourceHandle)
    {
        LOGE("Failed to create OcResourceHandle");
        delete jniHandle;
    }
    return jResourceHandle;
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    registerResource1
* Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Lorg/iotivity/base/OcPlatform/EntityHandler;I)Lorg/iotivity/base/OcResourceHandle;
*/
JNIEXPORT jobject JNICALL Java_org_iotivity_base_OcPlatform_registerResource1(
    JNIEnv *env,
    jclass clazz,
    jstring jResourceUri,
    jstring jResourceTypeName,
    jstring jResourceInterface,
jobject jListener, jint jResourceProperty)
{
    LOGI("OcPlatform_registerResource1");
    std::string resourceUri;
    if (jResourceUri)
    {
        resourceUri = env->GetStringUTFChars(jResourceUri, nullptr);
    }
    std::string resourceTypeName;
    if (jResourceTypeName)
    {
        resourceTypeName = env->GetStringUTFChars(jResourceTypeName, nullptr);
    }
    std::string resourceInterface;
    if (jResourceInterface)
    {
        resourceInterface = env->GetStringUTFChars(jResourceInterface, nullptr);
    }
    JniEntityHandler* entityHandler = NULL;
    EntityHandler handleEntityCallback = NULL;
    if (jListener)
    {
        entityHandler = new JniEntityHandler(env, jListener);
        handleEntityCallback =
            [entityHandler](const std::shared_ptr<OCResourceRequest> request) -> OCEntityHandlerResult
            {
                return entityHandler->handleEntity(request);
            };
    }

    OCResourceHandle resourceHandle;
    try
    {
        OCStackResult result = OCPlatform::registerResource(
            resourceHandle,
            resourceUri,
            resourceTypeName,
            resourceInterface,
            handleEntityCallback,
            static_cast<int>(jResourceProperty));

        if (OC_STACK_OK != result)
        {
            delete entityHandler;
            ThrowOcException(result, "register resource");
            return nullptr;
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        delete entityHandler;
        ThrowOcException(e.code(), e.reason().c_str());
        return nullptr;
    }

    JniOcResourceHandle* jniHandle = new JniOcResourceHandle(resourceHandle);
    jlong handle = reinterpret_cast<jlong>(jniHandle);
    jobject jResourceHandle =
        env->NewObject(g_cls_OcResourceHandle, g_mid_OcResourceHandle_N_ctor, handle);
    if (!jResourceHandle)
    {
        LOGE("Failed to create OcResourceHandle");
        delete jniHandle;
    }

    return jResourceHandle;
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    registerDeviceInfo0
* Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_registerDeviceInfo0(
    JNIEnv *env,
    jclass clazz,
    jstring jDeviceName,
    jobjectArray jDeviceTypes)
{
    LOGI("OcPlatform_registerDeviceInfo");

    if (!jDeviceName)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "deviceName cannot be null");
        return;
    }

    if (!jDeviceTypes)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "deviceTypes cannot be null");
        return;
    }

    OCDeviceInfo deviceInfo;
    memset(&deviceInfo, 0, sizeof(deviceInfo));
    try
    {
        DuplicateString(&deviceInfo.deviceName, env->GetStringUTFChars(jDeviceName, nullptr));

        jsize len = env->GetArrayLength(jDeviceTypes);
        for (jsize i = 0; i < len; ++i)
        {
            jstring jStr = (jstring)env->GetObjectArrayElement(jDeviceTypes, i);
            if (!jStr)
            {
                delete deviceInfo.deviceName;
                ThrowOcException(OC_STACK_INVALID_PARAM, "device type cannot be null");
                return;
            }

            OCResourcePayloadAddStringLL(&deviceInfo.types, env->GetStringUTFChars(jStr, nullptr));
            if (env->ExceptionCheck())
            {
                delete deviceInfo.deviceName;
                return;
            }

            env->DeleteLocalRef(jStr);
        }
    }
    catch (std::exception &e)
    {
        ThrowOcException(JNI_EXCEPTION, "Failed to construct device info");
        return;
    }

    try
    {
        OCStackResult result = OCPlatform::registerDeviceInfo(deviceInfo);

        delete deviceInfo.deviceName;

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Failed to register device info");
            return;
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    registerPlatformInfo0
* Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_registerPlatformInfo0(
    JNIEnv *env,
    jclass clazz,
    jstring jPlatformID,
    jstring jManufacturerName,
    jstring jManufacturerUrl,
    jstring jModelNumber,
    jstring jDateOfManufacture,
    jstring jPlatformVersion,
    jstring jOperatingSystemVersion,
    jstring jHardwareVersion,
    jstring jFirmwareVersion,
    jstring jSupportUrl,
    jstring jSystemTime)
{
    LOGI("OcPlatform_registerPlatformInfo");


    std::string platformID;
        std::string manufacturerName;
        std::string manufacturerUrl;
        std::string modelNumber;
        std::string dateOfManufacture;
        std::string platformVersion;
        std::string operatingSystemVersion;
        std::string hardwareVersion;
        std::string firmwareVersion;
        std::string supportUrl;
        std::string systemTime;

        if (jPlatformID)
        {
            platformID = env->GetStringUTFChars(jPlatformID, nullptr);
        }
        if (jManufacturerName)
        {
            manufacturerName = env->GetStringUTFChars(jManufacturerName, nullptr);
        }
        if (jManufacturerUrl)
        {
            manufacturerUrl = env->GetStringUTFChars(jManufacturerUrl, nullptr);
        }
        if (jModelNumber)
        {
            modelNumber = env->GetStringUTFChars(jModelNumber, nullptr);
        }
        if (jDateOfManufacture)
        {
            dateOfManufacture = env->GetStringUTFChars(jDateOfManufacture, nullptr);
        }
        if (jPlatformVersion)
        {
            platformVersion = env->GetStringUTFChars(jPlatformVersion, nullptr);
        }
        if (jOperatingSystemVersion)
        {
            operatingSystemVersion = env->GetStringUTFChars(jOperatingSystemVersion, nullptr);
        }
        if (jHardwareVersion)
        {
            hardwareVersion = env->GetStringUTFChars(jHardwareVersion, nullptr);
        }
        if (jFirmwareVersion)
        {
            firmwareVersion = env->GetStringUTFChars(jFirmwareVersion, nullptr);
        }
        if (jSupportUrl)
        {
            supportUrl = env->GetStringUTFChars(jSupportUrl, nullptr);
        }
        if (jSystemTime)
        {
            systemTime = env->GetStringUTFChars(jSystemTime, nullptr);
        }

        OCPlatformInfo platformInfo;
        try
        {
            DuplicateString(&platformInfo.platformID, platformID);
            DuplicateString(&platformInfo.manufacturerName, manufacturerName);
            DuplicateString(&platformInfo.manufacturerUrl, manufacturerUrl);
            DuplicateString(&platformInfo.modelNumber, modelNumber);
            DuplicateString(&platformInfo.dateOfManufacture, dateOfManufacture);
            DuplicateString(&platformInfo.platformVersion, platformVersion);
            DuplicateString(&platformInfo.operatingSystemVersion, operatingSystemVersion);
            DuplicateString(&platformInfo.hardwareVersion, hardwareVersion);
            DuplicateString(&platformInfo.firmwareVersion, firmwareVersion);
            DuplicateString(&platformInfo.supportUrl, supportUrl);
            DuplicateString(&platformInfo.systemTime, systemTime);
        }
        catch (std::exception &e)
        {
            ThrowOcException(JNI_EXCEPTION, "Failed to construct platform info");
            return;
        }

       // __android_log_print(ANDROID_LOG_INFO, "Rahul", "platformID  = %s", platformID);
        try
        {
            OCStackResult result = OCPlatform::registerPlatformInfo(platformInfo);

            delete platformInfo.platformID;
            delete platformInfo.manufacturerName;
            delete platformInfo.manufacturerUrl;
            delete platformInfo.modelNumber;
            delete platformInfo.dateOfManufacture;
            delete platformInfo.platformVersion;
            delete platformInfo.operatingSystemVersion;
            delete platformInfo.hardwareVersion;
            delete platformInfo.firmwareVersion;
            delete platformInfo.supportUrl;
            delete platformInfo.systemTime;

            if (OC_STACK_OK != result)
            {
                ThrowOcException(result, "Failed to register platform info");
                return;
            }
        }
        catch (OCException& e)
        {
            LOGE("Error is due to %s", e.reason().c_str());
            ThrowOcException(e.code(), e.reason().c_str());
        }
}

JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_setPropertyValue0
    (JNIEnv *env, jclass clazz, jint jType, jstring jPropName, jobjectArray jPropValue)
{
    try
    {
        OCPayloadType type = (OCPayloadType)jType;
        std::string propName;
        std::vector<std::string> propValue;
        if (jPropName)
        {
            propName = env->GetStringUTFChars(jPropName, nullptr);
        }
        if (jPropValue)
        {
            JniUtils::convertJavaStrArrToStrVector(env, jPropValue, propValue);
        }
        OCStackResult result = OCPlatform::setPropertyValue(type, propName, propValue);
        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Failed to set property");
            return;
        }
    }
    catch (OCException& e)
    {
        LOGE("Error is due to %s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }

}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    setPropertyValue0
* Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_setPropertyValue1
    (JNIEnv *env, jclass clazz, jint jType, jstring jPropName, jstring jPropValue)
{
    try
    {
        OCPayloadType type = (OCPayloadType)jType;
        std::string propName;
        std::string propValue;
        if (jPropName)
        {
            propName = env->GetStringUTFChars(jPropName, nullptr);
        }
        if (jPropValue)
        {
            propValue = env->GetStringUTFChars(jPropValue, nullptr);
        }
        OCStackResult result = OCPlatform::setPropertyValue(type, propName, propValue);
        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Failed to set property");
            return;
        }
    }
    catch (OCException& e)
    {
        LOGE("Error is due to %s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    getPropertyValue
* Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_getPropertyValue0
    (JNIEnv *env, jclass clazz, jint jType, jstring jPropName, jstring jPropValue)
{
    try
    {
        OCPayloadType type = (OCPayloadType) jType;
        std::string propName;
        std::string propValue;

        if (jPropName)
        {
            propName = env->GetStringUTFChars(jPropName, nullptr);
        }
        OCStackResult result = OCPlatform::getPropertyValue(type, propName, propValue);
        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Failed to get property value.");
            return;
        }
    }
    catch (OCException& e)
    {
        LOGE("Error is due to %s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    unregisterResource0
* Signature: (Lorg/iotivity/base/OcResourceHandle;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_unregisterResource0(
    JNIEnv *env, jclass clazz, jobject jResourceHandle)
{
    LOGI("OcPlatform_unregisterResource");
    if (!jResourceHandle)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "resourceHandle cannot be null");
        return;
    }
    JniOcResourceHandle* jniOcResourceHandle = JniOcResourceHandle::getJniOcResourceHandlePtr(
        env, jResourceHandle);
    if (!jniOcResourceHandle)
    {
        return;
    }

    try
    {
        OCResourceHandle resHandle = jniOcResourceHandle->getOCResourceHandle();
        OCStackResult result = OCPlatform::unregisterResource(resHandle);
        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Failed to unregister resource");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    bindResource0
* Signature: (Lorg/iotivity/base/OcResourceHandle;Lorg/iotivity/base/OcResourceHandle;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_bindResource0
(JNIEnv *env, jclass clazz, jobject jResourceCollectionHandle, jobject jResourceHandle)
{
    LOGI("OcPlatform_bindResource");
    if (!jResourceCollectionHandle)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "resourceCollectionHandle cannot be null");
        return;
    }
    if (!jResourceHandle)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "resourceHandle cannot be null");
        return;
    }
    JniOcResourceHandle* jniOcResourceCollectionHandle =
        JniOcResourceHandle::getJniOcResourceHandlePtr(env, jResourceCollectionHandle);
    if (!jniOcResourceCollectionHandle)
    {
        return;
    }

    JniOcResourceHandle* jniOcResourceHandle = JniOcResourceHandle::getJniOcResourceHandlePtr(
        env, jResourceHandle);
    if (!jniOcResourceHandle)
    {
        return;
    }

    try
    {
        OCStackResult result = OCPlatform::bindResource(
            jniOcResourceCollectionHandle->getOCResourceHandle(),
            jniOcResourceHandle->getOCResourceHandle()
            );

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Failed to bind resource");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    bindResources0
* Signature: (Lorg/iotivity/base/OcResourceHandle;[Lorg/iotivity/base/OcResourceHandle;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_bindResources0(
    JNIEnv *env,
    jclass clazz,
    jobject jResourceCollectionHandle,
    jobjectArray jResourceHandleArray)
{
    LOGI("OcPlatform_bindResources");

    if (!jResourceCollectionHandle)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "resourceCollectionHandle cannot be null");
        return;
    }
    if (!jResourceHandleArray)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "resourceHandleList cannot be null");
        return;
    }

    JniOcResourceHandle* jniOcResourceCollectionHandle =
        JniOcResourceHandle::getJniOcResourceHandlePtr(env, jResourceCollectionHandle);
    if (!jniOcResourceCollectionHandle)
    {
        return;
    }

    std::vector<OCResourceHandle> resourceHandleList;
    int len = env->GetArrayLength(jResourceHandleArray);
    for (int i = 0; i < len; ++i)
    {
        jobject jResourceHandle = env->GetObjectArrayElement(jResourceHandleArray, i);
        if (!jResourceHandle)
        {
            ThrowOcException(JNI_EXCEPTION, "resource handle cannot be null");
            return;
        }

        JniOcResourceHandle* jniOcResourceHandle =
            JniOcResourceHandle::getJniOcResourceHandlePtr(env, jResourceHandle);
        if (!jniOcResourceHandle)
        {
            return;
        }

        resourceHandleList.push_back(
            jniOcResourceHandle->getOCResourceHandle());
    }

    try
    {
        OCStackResult result = OCPlatform::bindResources(
            jniOcResourceCollectionHandle->getOCResourceHandle(),
            resourceHandleList
            );

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Failed to bind resources");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    unbindResource0
* Signature: (Lorg/iotivity/base/OcResourceHandle;Lorg/iotivity/base/OcResourceHandle;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_unbindResource0(
    JNIEnv *env,
    jclass clazz,
    jobject jResourceCollectionHandle,
    jobject jResourceHandle)
{
    LOGI("OcPlatform_unbindResource");
    if (!jResourceCollectionHandle)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "resourceCollectionHandle cannot be null");
        return;
    }
    if (!jResourceHandle)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "resourceHandle cannot be null");
        return;
    }

    JniOcResourceHandle* jniOcResourceCollectionHandle =
        JniOcResourceHandle::getJniOcResourceHandlePtr(env, jResourceCollectionHandle);
    if (!jniOcResourceCollectionHandle)
    {
        return;
    }

    JniOcResourceHandle* jniOcResourceHandle =
        JniOcResourceHandle::getJniOcResourceHandlePtr(env, jResourceHandle);
    if (!jniOcResourceHandle)
    {
        return;
    }

    try
    {
        OCStackResult result = OCPlatform::unbindResource(
            jniOcResourceCollectionHandle->getOCResourceHandle(),
            jniOcResourceHandle->getOCResourceHandle());

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Failed to unbind resource");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    unbindResources0
* Signature: (Lorg/iotivity/base/OcResourceHandle;[Lorg/iotivity/base/OcResourceHandle;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_unbindResources0(
    JNIEnv *env,
    jclass clazz,
    jobject jResourceCollectionHandle,
    jobjectArray jResourceHandleArray)
{
    LOGI("OcPlatform_unbindResources");
    if (!jResourceCollectionHandle)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "resourceCollectionHandle cannot be null");
        return;
    }
    if (!jResourceHandleArray)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "resourceHandleList cannot be null");
        return;
    }

    JniOcResourceHandle* jniOcResourceCollectionHandle =
        JniOcResourceHandle::getJniOcResourceHandlePtr(env, jResourceCollectionHandle);
    if (!jniOcResourceCollectionHandle)
    {
        return;
    }

    std::vector<OCResourceHandle> resourceHandleList;
    int len = env->GetArrayLength(jResourceHandleArray);
    for (int i = 0; i < len; ++i)
    {
        jobject jResourceHandle = env->GetObjectArrayElement(jResourceHandleArray, i);
        if (!jResourceHandle)
        {
            ThrowOcException(JNI_EXCEPTION, "resource handle cannot be null");
            return;
        }

        JniOcResourceHandle* jniOcResourceHandle =
            JniOcResourceHandle::getJniOcResourceHandlePtr(env, jResourceHandle);
        if (!jniOcResourceHandle)
        {
            return;
        }

        resourceHandleList.push_back(
            jniOcResourceHandle->getOCResourceHandle());
    }

    try
    {
        OCStackResult result = OCPlatform::unbindResources(
            jniOcResourceCollectionHandle->getOCResourceHandle(),
            resourceHandleList
            );

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Failed to unbind resources");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    bindTypeToResource0
* Signature: (Lorg/iotivity/base/OcResourceHandle;Ljava/lang/String;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_bindTypeToResource0(
    JNIEnv *env,
    jclass clazz,
    jobject jResourceHandle,
    jstring jResourceTypeName)
{
    LOGI("OcPlatform_bindTypeToResource");
    if (!jResourceHandle)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "resourceHandle cannot be null");
        return;
    }
    std::string typeName;
    if (jResourceTypeName)
    {
        typeName = env->GetStringUTFChars(jResourceTypeName, nullptr);
    }

    JniOcResourceHandle* jniOcResourceHandle =
        JniOcResourceHandle::getJniOcResourceHandlePtr(env, jResourceHandle);
    if (!jniOcResourceHandle)
    {
        return;
    }

    try
    {
        OCStackResult result = OCPlatform::bindTypeToResource(
            jniOcResourceHandle->getOCResourceHandle(),
            typeName
            );

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Failed to bind type to resource");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    bindInterfaceToResource0
* Signature: (Lorg/iotivity/base/OcResourceHandle;Ljava/lang/String;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_bindInterfaceToResource0
(JNIEnv *env, jclass clazz, jobject jResourceHandle, jstring jResourceInterfaceName)
{
    LOGI("OcPlatform_bindInterfaceToResource");
    if (!jResourceHandle)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "resourceHandle cannot be null");
        return;
    }
    std::string interfaceName;
    if (jResourceInterfaceName)
    {
        interfaceName = env->GetStringUTFChars(jResourceInterfaceName, nullptr);
    }

    JniOcResourceHandle* jniOcResourceHandle =
        JniOcResourceHandle::getJniOcResourceHandlePtr(env, jResourceHandle);
    if (!jniOcResourceHandle)
    {
        return;
    }

    try
    {
        OCStackResult result = OCPlatform::bindInterfaceToResource(
            jniOcResourceHandle->getOCResourceHandle(),
            interfaceName
            );

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Failed to bind interface to resource");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    startPresence0
* Signature: (I)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_startPresence0(
    JNIEnv *env, jclass clazz, jint ttl)
{
    LOGI("OcPlatform_startPresence");

    try
    {
        OCStackResult result = OCPlatform::startPresence((unsigned int)ttl);

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Failed to start presence");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    stopPresence0
* Signature: ()V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_stopPresence0(
    JNIEnv *env, jclass clazz)
{
    LOGI("OcPlatform_stopPresence");

    try
    {
        OCStackResult result = OCPlatform::stopPresence();

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Failed to stop presence");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    subscribePresence0
* Signature: (Ljava/lang/String;ILorg/iotivity/base/OcPlatform/OnPresenceListener;)Lorg/iotivity/base/OcPresenceHandle;
*/
JNIEXPORT jobject JNICALL Java_org_iotivity_base_OcPlatform_subscribePresence0(
    JNIEnv *env,
    jclass clazz,
    jstring jHost,
    jint jConnectivityType,
    jobject jListener)
{
    LOGD("OcPlatform_subscribePresence");
    std::string host;
    if (jHost)
    {
        host = env->GetStringUTFChars(jHost, nullptr);
    }
    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "onPresenceListener cannot be null");
        return nullptr;
    }

    JniOnPresenceListener *onPresenceListener = AddOnPresenceListener(env, jListener);

    SubscribeCallback subscribeCallback =
        [onPresenceListener](OCStackResult result,
                             const unsigned int nonce,
                             const std::string& hostAddress)
        {
            onPresenceListener->onPresenceCallback(result, nonce, hostAddress);
        };

    OCPlatform::OCPresenceHandle presenceHandle;
    try
    {
        OCStackResult result = OCPlatform::subscribePresence(
            presenceHandle,
            host,
            static_cast<OCConnectivityType>(jConnectivityType),
            subscribeCallback);

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "subscribe presence has failed");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
        return nullptr;
    }

    JniOcPresenceHandle* jniPresenceHandle =
        new JniOcPresenceHandle(onPresenceListener, presenceHandle);
    jlong jhandle = reinterpret_cast<jlong>(jniPresenceHandle);
    jobject jPresenceHandle =
        env->NewObject(g_cls_OcPresenceHandle, g_mid_OcPresenceHandle_N_ctor, jhandle);
    if (!jPresenceHandle)
    {
        LOGE("Failed to create OcPresenceHandle");
        delete jniPresenceHandle;
    }
    return jPresenceHandle;
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    subscribePresence1
* Signature: (Ljava/lang/String;Ljava/lang/String;I
Lorg/iotivity/base/OcPlatform/OnPresenceListener;)Lorg/iotivity/base/OcPresenceHandle;
*/
JNIEXPORT jobject JNICALL Java_org_iotivity_base_OcPlatform_subscribePresence1(
    JNIEnv *env,
    jclass clazz,
    jstring jHost,
    jstring jResourceType,
    jint jConnectivityType,
    jobject jListener)
{
    LOGD("OcPlatform_subscribePresence1");
    std::string host;
    if (jHost)
    {
        host = env->GetStringUTFChars(jHost, nullptr);
    }
    std::string resourceType;
    if (jResourceType)
    {
        resourceType = env->GetStringUTFChars(jResourceType, nullptr);
    }
    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "onPresenceListener cannot be null");
        return nullptr;
    }

    JniOnPresenceListener *onPresenceListener = AddOnPresenceListener(env, jListener);

    SubscribeCallback subscribeCallback = [onPresenceListener](OCStackResult result,
        const unsigned int nonce, const std::string& hostAddress)
    {
        onPresenceListener->onPresenceCallback(result, nonce, hostAddress);
    };

    OCPlatform::OCPresenceHandle presenceHandle;
    try
    {
        OCStackResult result = OCPlatform::subscribePresence(
            presenceHandle,
            host,
            resourceType,
            static_cast<OCConnectivityType>(jConnectivityType),
            subscribeCallback);

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "subscribe presence has failed");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
        return nullptr;
    }

    JniOcPresenceHandle* jniPresenceHandle =
        new JniOcPresenceHandle(onPresenceListener, presenceHandle);
    jlong jhandle = reinterpret_cast<jlong>(jniPresenceHandle);
    jobject jPresenceHandle =
        env->NewObject(g_cls_OcPresenceHandle, g_mid_OcPresenceHandle_N_ctor, jhandle);
    if (!jPresenceHandle)
    {
        LOGE("Failed to create OcPresenceHandle");
        delete jniPresenceHandle;
    }
    return jPresenceHandle;
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    unsubscribePresence0
* Signature: (Lorg/iotivity/base/OcPresenceHandle;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_unsubscribePresence0(
    JNIEnv *env, jclass clazz, jobject jPresenceHandle)
{
    LOGD("OcPlatform_unsubscribePresence");
    if (!jPresenceHandle)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "presenceHandle cannot be null");
        return;
    }
    JniOcPresenceHandle* jniPresenceHandle =
        JniOcPresenceHandle::getJniOcPresenceHandlePtr(env, jPresenceHandle);
    if (!jniPresenceHandle)
    {
        return;
    }

    OCPresenceHandle presenceHandle = jniPresenceHandle->getOCPresenceHandle();

    try
    {
        OCStackResult result = OCPlatform::unsubscribePresence(presenceHandle);

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "unsubscribe presence has failed");
            return;
        }

        JniOnPresenceListener* jniPresenceListener = jniPresenceHandle->getJniOnPresenceListener();
        if (jniPresenceListener)
        {
            jweak jwOnPresenceListener = jniPresenceListener->getJWListener();
            if (jwOnPresenceListener)
            {
                RemoveOnPresenceListener(env, jwOnPresenceListener);
            }
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    subscribeDevicePresence0
* Signature: (Ljava/lang/String;[Ljava/lang/String;I
Lorg/iotivity/base/OcResource/OnObserveListener;)Lorg/iotivity/base/OcPresenceHandle;
*/
JNIEXPORT jobject JNICALL Java_org_iotivity_base_OcPlatform_subscribeDevicePresence0(
    JNIEnv *env,
    jclass clazz,
    jstring jHost,
    jobjectArray jDiArray,
    jint jConnectivityType,
    jobject jListener)
{
    LOGD("OcPlatform_subscribeDevicePresence0");
#ifdef WITH_CLOUD
    std::string host;
    if (jHost)
    {
        host = env->GetStringUTFChars(jHost, nullptr);
    }

    if (!jDiArray)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "device id List cannot be null");
        return nullptr;
    }

    std::vector<std::string> di;
    JniUtils::convertJavaStrArrToStrVector(env, jDiArray, di);

    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "onObserveListener cannot be null");
        return nullptr;
    }

    JniOnObserveListener *onObserveListener = AddOnObserveListener(env, jListener);

    ObserveCallback observeCallback = [onObserveListener](const HeaderOptions& opts,
        const OCRepresentation& rep, const int& eCode, const int& sequenceNumber)
    {
        onObserveListener->onObserveCallback(opts, rep, eCode, sequenceNumber);
    };

    OCPlatform::OCPresenceHandle presenceHandle;
    try
    {
        OCStackResult result = OCPlatform::subscribeDevicePresence(
            presenceHandle,
            host,
            di,
            static_cast<OCConnectivityType>(jConnectivityType),
            observeCallback);

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "subscribe device presence has failed");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
        return nullptr;
    }

    JniOcPresenceHandle* jniPresenceHandle =
        new JniOcPresenceHandle(onObserveListener, presenceHandle);
    jlong jhandle = reinterpret_cast<jlong>(jniPresenceHandle);
    jobject jPresenceHandle =
        env->NewObject(g_cls_OcPresenceHandle, g_mid_OcPresenceHandle_N_ctor, jhandle);
    if (!jPresenceHandle)
    {
        LOGE("Failed to create OcPresenceHandle");
        delete jniPresenceHandle;
    }
    return jPresenceHandle;
#else
    ThrowOcException(JNI_NO_SUPPORT, "Not supported");
    return nullptr;
#endif
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    constructResourceObject0
* Signature: (Ljava/lang/String;Ljava/lang/String;IZ[Ljava/lang/String;[Ljava/lang/String;)
Lorg/iotivity/base/OcResource;
*/
JNIEXPORT jobject JNICALL Java_org_iotivity_base_OcPlatform_constructResourceObject0(
    JNIEnv *env,
    jclass clazz,
    jstring jHost,
    jstring jUri,
    jint jConnectivityType,
    jboolean jIsObservable,
    jobjectArray jResourceTypeArray,
    jobjectArray jInterfaceArray)
{
    LOGD("OcPlatform_constructResourceObject");
    std::string host;
    if (jHost)
    {
        host = env->GetStringUTFChars(jHost, nullptr);
    }
    std::string uri;
    if (jUri)
    {
        uri = env->GetStringUTFChars(jUri, nullptr);
    }
    if (!jResourceTypeArray)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "resourceTypeList cannot be null");
        return nullptr;
    }
    if (!jInterfaceArray)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "interfaceList cannot be null");
        return nullptr;
    }

    std::vector<std::string> resourceTypes;
    JniUtils::convertJavaStrArrToStrVector(env, jResourceTypeArray, resourceTypes);

    std::vector<std::string> interfaces;
    JniUtils::convertJavaStrArrToStrVector(env, jInterfaceArray, interfaces);

    std::shared_ptr<OCResource> resource = OCPlatform::constructResourceObject(
        host,
        uri,
        static_cast<OCConnectivityType>(jConnectivityType),
        static_cast<bool>(jIsObservable),
        resourceTypes,
        interfaces);

    if (!resource)
    {
        ThrowOcException(OC_STACK_ERROR, "Failed to create OCResource");
        return nullptr;
    }

    JniOcResource *jniOcResource = new JniOcResource(resource);

    jobject jResource = env->NewObject(g_cls_OcResource, g_mid_OcResource_ctor);
    if (!jResource)
    {
        delete jniOcResource;
        return nullptr;
    }
    SetHandle<JniOcResource>(env, jResource, jniOcResource);
    if (env->ExceptionCheck())
    {
        delete jniOcResource;
        return nullptr;
    }
    return jResource;
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    sendResponse0
* Signature: (Lorg/iotivity/base/OcResourceResponse;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_sendResponse0(
    JNIEnv *env, jclass clazz, jobject jResourceResponse)
{
    LOGD("OcPlatform_sendResponse");
    if (!jResourceResponse)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "resourceResponse cannot be null");
        return;
    }

    JniOcResourceResponse *jniResponse =
        JniOcResourceResponse::getJniOcResourceResponsePtr(env, jResourceResponse);
    if (!jniResponse)
    {
        return;
    }

    try
    {
        OCStackResult result =
            OCPlatform::sendResponse(jniResponse->getOCResourceResponse());

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "failed to send response");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    constructAccountManagerObject0
* Signature: (Ljava/lang/String;I)Lorg/iotivity/base/OcAccountManager;
*/
JNIEXPORT jobject JNICALL Java_org_iotivity_base_OcPlatform_constructAccountManagerObject0(
    JNIEnv *env,
    jclass clazz,
    jstring jHost,
    jint jConnectivityType)
{
#ifndef WITH_CLOUD
    ThrowOcException(OC_STACK_ERROR,
                     "OCAccountManager is not supported. (Please build with WITH_CLOUD=1 option)");
    return nullptr;
#else
    LOGD("OcPlatform_constructAccountManagerObject");
    if (!jHost)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "host cannot be null");
        return nullptr;
    }

    const char* charHost = env->GetStringUTFChars(jHost, nullptr);
    if (!charHost)
    {
        ThrowOcException(JNI_EXCEPTION, "charHost is null");
        return nullptr;
    }
    std::string host(charHost);
    env->ReleaseStringUTFChars(jHost, charHost);

    std::shared_ptr<OCAccountManager> accountManager = OCPlatform::constructAccountManagerObject(
        host,
        static_cast<OCConnectivityType>(jConnectivityType));

    if (!accountManager)
    {
        ThrowOcException(OC_STACK_ERROR, "Failed to create OCAccountManager");
        return nullptr;
    }

    JniOcAccountManager *jniOcAccountManager = new JniOcAccountManager(accountManager);

    jobject jAccountManager = env->NewObject(g_cls_OcAccountManager, g_mid_OcAccountManager_ctor);
    if (!jAccountManager)
    {
        delete jniOcAccountManager;
        return nullptr;
    }
    SetHandle<JniOcAccountManager>(env, jAccountManager, jniOcAccountManager);
    if (env->ExceptionCheck())
    {
        delete jniOcAccountManager;
        return nullptr;
    }
    return jAccountManager;
#endif
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    getDeviceId
* Signature: (I)V
*/
JNIEXPORT jbyteArray JNICALL Java_org_iotivity_base_OcPlatform_getDeviceId
(JNIEnv *env, jobject thiz)
{
    LOGD("OcPlatform_getDeviceId");
    OCUUIdentity deviceId;

    jbyteArray ret = env->NewByteArray(UUID_IDENTITY_SIZE);
    jbyte uuid[UUID_IDENTITY_SIZE];
    try
    {

        OCStackResult result = OCPlatform::getDeviceId(&deviceId);
        LOGD("OcPlatform_getDeviceId return from CPP");
        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Error while getting my device Id");
        }
        else
        {
            for(int i=0;i < UUID_IDENTITY_SIZE; i++)
            {
                uuid[i] =(jbyte) deviceId.id[i];
            }
        }

    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }

    env->SetByteArrayRegion(ret, 0, UUID_IDENTITY_SIZE, uuid);

    return ret;
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    setDeviceId
* Signature: (Ljava/lang/byte;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_setDeviceId(
    JNIEnv *env, jobject thiz, jbyteArray data)
{
    LOGI("OcPlatform_setDeviceId");
    OCUUIdentity deviceId;
    try
    {
        OCStackResult result;
        jbyte* uuid = env->GetByteArrayElements(data, 0);
        jsize arrayLength = env->GetArrayLength(data);
        if(arrayLength!=UUID_IDENTITY_SIZE)
        {
            ThrowOcException(OC_STACK_INVALID_PARAM, "Byte length not equal to UUID_IDENTITY_SIZE");
        }
        else
        {
            for(int i=0;i < UUID_IDENTITY_SIZE; i++)
            {
                deviceId.id[i]=(jchar)uuid[i];
            }
            result = OCPlatform::setDeviceId(&deviceId);
            if (OC_STACK_OK != result)
            {
                ThrowOcException(result, "Failed to set DeviceId");
            }
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
}

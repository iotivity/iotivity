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
#include "JniOcPlatform.h"
#include "OCPlatform.h"
#include "JniOcResource.h"
#include "JniOcResourceHandle.h"
#include "JniOcPresenceHandle.h"
#include "JniOcResourceResponse.h"
#include "JniUtils.h"

using namespace OC;

JniOnResourceFoundListener* AddOnResourceFoundListener(JNIEnv* env, jobject jListener)
{
    JniOnResourceFoundListener *onResourceFoundListener = NULL;

    resourceFoundMapLock.lock();

    for (auto it = onResourceFoundListenerMap.begin(); it != onResourceFoundListenerMap.end(); ++it)
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
        onResourceFoundListener = new JniOnResourceFoundListener(env, jListener, RemoveOnResourceFoundListener);
        jobject jgListener = env->NewGlobalRef(jListener);

        onResourceFoundListenerMap.insert(std::pair < jobject, std::pair < JniOnResourceFoundListener*,
            int >> (jgListener, std::pair<JniOnResourceFoundListener*, int>(onResourceFoundListener, 1)));
        LOGD("OnResourceFoundListener: new listener");
    }
    resourceFoundMapLock.unlock();
    return onResourceFoundListener;
}

void RemoveOnResourceFoundListener(JNIEnv* env, jobject jListener)
{
    resourceFoundMapLock.lock();

    for (auto it = onResourceFoundListenerMap.begin(); it != onResourceFoundListenerMap.end(); ++it)
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
    JniOnDeviceInfoListener *onDeviceInfoListener = NULL;

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
        onDeviceInfoListener = new JniOnDeviceInfoListener(env, jListener, RemoveOnDeviceInfoListener);
        jobject jgListener = env->NewGlobalRef(jListener);

        onDeviceInfoListenerMap.insert(std::pair < jobject, std::pair < JniOnDeviceInfoListener*,
            int >> (jgListener, std::pair<JniOnDeviceInfoListener*, int>(onDeviceInfoListener, 1)));
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

JniOnPresenceListener* AddOnPresenceListener(JNIEnv* env, jobject jListener)
{
    JniOnPresenceListener *onPresenceListener = NULL;

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
        onPresenceListenerMap.insert(std::pair < jobject, std::pair < JniOnPresenceListener*,
            int >> (jgListener, std::pair<JniOnPresenceListener*, int>(onPresenceListener, 1)));
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

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    configure
* Signature: (IILjava/lang/String;II)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_configure
(JNIEnv *env, jclass clazz, jint jServiceType, jint jModeType, jstring jIpAddress, jint jPort, jint jQOS)
{
    LOGI("OcPlatform_configure");

    std::string ipAddress;
    if (jIpAddress)
    {
        ipAddress = env->GetStringUTFChars(jIpAddress, NULL);
    }
    uint16_t port;
    if (jPort > 0)
    {
        port = static_cast<uint16_t>(jPort);
    }
    PlatformConfig cfg{
        JniUtils::getServiceType(env, jServiceType),
        JniUtils::getModeType(env, jModeType),
        ipAddress,
        port,
        JniUtils::getQOS(env, static_cast<int>(jQOS))
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
    if (!jniOcResourceHandle) return;

    try
    {
        OCStackResult result = OCPlatform::notifyAllObservers(jniOcResourceHandle->getOCResourceHandle());

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Failed to notify all observers");
            return;
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
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

    JniOcResourceHandle* jniOcResourceHandle = JniOcResourceHandle::getJniOcResourceHandlePtr(
        env, jResourceHandle);
    if (!jniOcResourceHandle) return;

    try{
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
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    notifyListOfObservers2
* Signature: (Lorg/iotivity/base/OcResourceHandle;[Ljava/lang/Byte;Lorg/iotivity/base/OcResourceResponse;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_notifyListOfObservers2
(JNIEnv *env, jclass clazz, jobject jResourceHandle, jbyteArray jObservationIdArr, jobject jResourceResponse)
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

    JniOcResourceHandle* jniOcResourceHandle = JniOcResourceHandle::getJniOcResourceHandlePtr(
        env, jResourceHandle);
    if (!jniOcResourceHandle) return;

    JniOcResourceResponse* jniOcResourceResponse = JniOcResourceResponse::getJniOcResourceResponsePtr(
        env, jResourceResponse);
    if (!jniOcResourceResponse) return;

    int len = env->GetArrayLength(jObservationIdArr);
    uint8_t* bArr = (uint8_t*)env->GetByteArrayElements(jObservationIdArr, 0);

    ObservationIds observationIds;
    for (int i = 0; i < len; ++i)
    {
        observationIds.push_back(bArr[i]);
    }

    env->ReleaseByteArrayElements(jObservationIdArr, (jbyte*)bArr, 0);

    try{
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
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    notifyListOfObservers3
* Signature: (Lorg/iotivity/base/OcResourceHandle;[Ljava/lang/Byte;Lorg/iotivity/base/OcResourceResponse;I)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_notifyListOfObservers3
(JNIEnv *env, jclass clazz, jobject jResourceHandle, jbyteArray jObservationIdArr, jobject jResourceResponse, jint jQoS)
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

    JniOcResourceHandle* jniOcResourceHandle = JniOcResourceHandle::getJniOcResourceHandlePtr(
        env, jResourceHandle);
    if (!jniOcResourceHandle) return;

    JniOcResourceResponse* jniOcResourceResponse = JniOcResourceResponse::getJniOcResourceResponsePtr(
        env, jResourceResponse);
    if (!jniOcResourceResponse) return;

    int len = env->GetArrayLength(jObservationIdArr);
    uint8_t* bArr = (uint8_t*)env->GetByteArrayElements(jObservationIdArr, 0);

    ObservationIds observationIds;
    for (int i = 0; i < len; ++i)
    {
        observationIds.push_back(bArr[i]);
    }

    env->ReleaseByteArrayElements(jObservationIdArr, (jbyte*)bArr, 0);

    try{
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
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    findResource0
* Signature: (Ljava/lang/String;Ljava/lang/String;ILorg/iotivity/base/OcPlatform/OnResourceFoundListener;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_findResource0
(JNIEnv *env, jclass clazz, jstring jHost, jstring jResourceUri, jint jConnectivityType, jobject jListener)
{
    LOGD("OcPlatform_findResource");
    std::string host;
    if (jHost)
    {
        host = env->GetStringUTFChars(jHost, NULL);
    }
    std::string resourceUri;
    if (jResourceUri)
    {
        resourceUri = env->GetStringUTFChars(jResourceUri, NULL);
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

    try
    {
        OCStackResult result = OCPlatform::findResource(
            host,
            resourceUri,
            JniUtils::getConnectivityType(env, static_cast<int>(jConnectivityType)),
            findCallback);

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Find resource has failed");
            return;
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    findResource1
* Signature: (Ljava/lang/String;Ljava/lang/String;ILorg/iotivity/base/OcPlatform/OnResourceFoundListener;I)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_findResource1
(JNIEnv *env, jclass clazz, jstring jHost, jstring jResourceUri, jint jConnectivityType, jobject jListener, jint jQoS)
{
    LOGD("OcPlatform_findResource");
    std::string host;
    if (jHost)
    {
        host = env->GetStringUTFChars(jHost, NULL);
    }
    std::string resourceUri;
    if (jResourceUri)
    {
        resourceUri = env->GetStringUTFChars(jResourceUri, NULL);
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

    try
    {
        OCStackResult result = OCPlatform::findResource(
            host,
            resourceUri,
            JniUtils::getConnectivityType(env, static_cast<int>(jConnectivityType)),
            findCallback,
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
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    getDeviceInfo0
* Signature: (Ljava/lang/String;Ljava/lang/String;ILorg/iotivity/base/OcPlatform/OnDeviceFoundListener;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_getDeviceInfo0
(JNIEnv *env, jclass clazz, jstring jHost, jstring jResourceUri, jint jConnectivityType, jobject jListener)
{
    LOGD("OcPlatform_getDeviceInfo0");
    std::string host;
    if (jHost)
    {
        host = env->GetStringUTFChars(jHost, NULL);
    }
    std::string resourceUri;
    if (jResourceUri)
    {
        resourceUri = env->GetStringUTFChars(jResourceUri, NULL);
    }
    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "onDeviceFoundListener cannot be null");
        return;
    }
    JniOnDeviceInfoListener *onDeviceInfoListener = AddOnDeviceInfoListener(env, jListener);

    FindDeviceCallback findDeviceCallback = [onDeviceInfoListener](const OCRepresentation& ocRepresentation)
    {
        onDeviceInfoListener->foundDeviceCallback(ocRepresentation);
    };

    try
    {
        OCStackResult result = OCPlatform::getDeviceInfo(
            host,
            resourceUri,
            JniUtils::getConnectivityType(env, static_cast<int>(jConnectivityType)),
            findDeviceCallback);

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Find device has failed");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    getDeviceInfo1
* Signature: (Ljava/lang/String;Ljava/lang/String;ILorg/iotivity/base/OcPlatform/OnDeviceFoundListener;I)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_getDeviceInfo1
(JNIEnv *env, jclass clazz, jstring jHost, jstring jResourceUri, jint jConnectivityType, jobject jListener, jint jQoS)
{
    LOGD("OcPlatform_getDeviceInfo1");
    std::string host;
    if (jHost)
    {
        host = env->GetStringUTFChars(jHost, NULL);
    }
    std::string resourceUri;
    if (jResourceUri)
    {
        resourceUri = env->GetStringUTFChars(jResourceUri, NULL);
    }
    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "onDeviceFoundListener cannot be null");
        return;
    }
    JniOnDeviceInfoListener *onDeviceInfoListener = AddOnDeviceInfoListener(env, jListener);

    FindDeviceCallback findDeviceCallback = [onDeviceInfoListener](const OCRepresentation& ocRepresentation)
    {
        onDeviceInfoListener->foundDeviceCallback(ocRepresentation);
    };

    try
    {
        OCStackResult result = OCPlatform::getDeviceInfo(
            host,
            resourceUri,
            JniUtils::getConnectivityType(env, static_cast<int>(jConnectivityType)),
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
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    registerResource0
* Signature: (Lorg/iotivity/base/OcResource;)Lorg/iotivity/base/OcResourceHandle;
*/
JNIEXPORT jobject JNICALL Java_org_iotivity_base_OcPlatform_registerResource0
(JNIEnv *env, jclass clazz, jobject jResource)
{
    LOGD("OcPlatform_registerResource");
    if (!jResource)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "Resource cannot be null");
        return nullptr;
    }
    JniOcResource *resource = JniOcResource::getJniOcResourcePtr(env, jResource);
    if (!resource) return nullptr;

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
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
        return nullptr;
    }

    JniOcResourceHandle* jniHandle = new JniOcResourceHandle(resourceHandle);
    jlong handle = reinterpret_cast<jlong>(jniHandle);
    jobject jResourceHandle = env->NewObject(g_cls_OcResourceHandle, g_mid_OcResourceHandle_N_ctor, handle);
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
JNIEXPORT jobject JNICALL Java_org_iotivity_base_OcPlatform_registerResource1
(JNIEnv *env, jclass clazz, jstring jResourceUri, jstring jResourceTypeName, jstring jResourceInterface,
jobject jListener, jint jResourceProperty)
{
    LOGI("OcPlatform_registerResource1");
    std::string resourceUri;
    if (jResourceUri)
    {
        resourceUri = env->GetStringUTFChars(jResourceUri, NULL);
    }
    std::string resourceTypeName;
    if (jResourceTypeName)
    {
        resourceTypeName = env->GetStringUTFChars(jResourceTypeName, NULL);
    }
    std::string resourceInterface;
    if (jResourceInterface)
    {
        resourceInterface = env->GetStringUTFChars(jResourceInterface, NULL);
    }
    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "entityHandler cannot be null");
        return nullptr;
    }
    JniEntityHandler* entityHandler = new JniEntityHandler(env, jListener);
    EntityHandler handleEntityCallback = [entityHandler](const std::shared_ptr<OCResourceRequest> request) ->
        OCEntityHandlerResult{
        return entityHandler->handleEntity(request);
    };

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
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
        return nullptr;
    }

    JniOcResourceHandle* jniHandle = new JniOcResourceHandle(resourceHandle);
    jlong handle = reinterpret_cast<jlong>(jniHandle);
    jobject jResourceHandle = env->NewObject(g_cls_OcResourceHandle, g_mid_OcResourceHandle_N_ctor, handle);
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
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_registerDeviceInfo0
(JNIEnv *env,
jclass clazz,
jstring jDeviceName,
jstring jHostName,
jstring jDeviceUUID,
jstring jContentType,
jstring jVersion,
jstring jManufacturerName,
jstring jManufacturerUrl,
jstring jModelNumber,
jstring jDateOfManufacture,
jstring jPlatformVersion,
jstring jFirmwareVersion,
jstring jSupportUrl)
{
    LOGI("OcPlatform_registerDeviceInfo");

    std::string deviceName;
    if (jDeviceName)
    {
        deviceName = env->GetStringUTFChars(jDeviceName, NULL);
    }
    std::string hostName;
    if (jHostName)
    {
        hostName = env->GetStringUTFChars(jHostName, NULL);
    }
    std::string deviceUUID;
    if (jDeviceUUID)
    {
        deviceUUID = env->GetStringUTFChars(jDeviceUUID, NULL);
    }
    std::string contentType;
    if (jContentType)
    {
        contentType = env->GetStringUTFChars(jContentType, NULL);
    }
    std::string version;
    if (jVersion)
    {
        version = env->GetStringUTFChars(jVersion, NULL);
    }
    std::string manufacturerName;
    if (jManufacturerName)
    {
        manufacturerName = env->GetStringUTFChars(jManufacturerName, NULL);
    }
    std::string manufacturerUrl;
    if (jManufacturerUrl)
    {
        manufacturerUrl = env->GetStringUTFChars(jManufacturerUrl, NULL);
    }
    std::string modelNumber;
    if (jModelNumber)
    {
        modelNumber = env->GetStringUTFChars(jModelNumber, NULL);
    }
    std::string dateOfManufacture;
    if (jDateOfManufacture)
    {
        dateOfManufacture = env->GetStringUTFChars(jDateOfManufacture, NULL);
    }
    std::string platformVersion;
    if (jPlatformVersion)
    {
        platformVersion = env->GetStringUTFChars(jPlatformVersion, NULL);
    }
    std::string firmwareVersion;
    if (jFirmwareVersion)
    {
        firmwareVersion = env->GetStringUTFChars(jFirmwareVersion, NULL);
    }
    std::string supportUrl;
    if (jSupportUrl)
    {
        supportUrl = env->GetStringUTFChars(jSupportUrl, NULL);
    }

    OCDeviceInfo deviceInfo;
    try
    {
        DuplicateString(&deviceInfo.contentType, contentType);
        DuplicateString(&deviceInfo.dateOfManufacture, dateOfManufacture);
        DuplicateString(&deviceInfo.deviceName, deviceName);
        DuplicateString(&deviceInfo.deviceUUID, deviceUUID);
        DuplicateString(&deviceInfo.firmwareVersion, firmwareVersion);
        DuplicateString(&deviceInfo.hostName, hostName);
        DuplicateString(&deviceInfo.manufacturerName, manufacturerName);
        DuplicateString(&deviceInfo.manufacturerUrl, manufacturerUrl);
        DuplicateString(&deviceInfo.modelNumber, modelNumber);
        DuplicateString(&deviceInfo.platformVersion, platformVersion);
        DuplicateString(&deviceInfo.supportUrl, supportUrl);
        DuplicateString(&deviceInfo.version, version);
    }
    catch (std::exception &e)
    {
        ThrowOcException(JNI_EXCEPTION, "Failed to register device info");
        return;
    }

    try
    {
        OCStackResult result = OCPlatform::registerDeviceInfo(deviceInfo);

        delete deviceInfo.contentType;
        delete deviceInfo.dateOfManufacture;
        delete deviceInfo.deviceName;
        delete deviceInfo.deviceUUID;
        delete deviceInfo.firmwareVersion;
        delete deviceInfo.hostName;
        delete deviceInfo.manufacturerName;
        delete deviceInfo.manufacturerUrl;
        delete deviceInfo.modelNumber;
        delete deviceInfo.platformVersion;
        delete deviceInfo.supportUrl;
        delete deviceInfo.version;

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Failed to register device info");
            return;
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    unregisterResource0
* Signature: (Lorg/iotivity/base/OcResourceHandle;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_unregisterResource0
(JNIEnv *env, jclass clazz, jobject jResourceHandle)
{
    LOGI("OcPlatform_unregisterResource");
    if (!jResourceHandle)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "resourceHandle cannot be null");
        return;
    }
    JniOcResourceHandle* jniOcResourceHandle = JniOcResourceHandle::getJniOcResourceHandlePtr(
        env, jResourceHandle);
    if (!jniOcResourceHandle) return;

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
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
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
    JniOcResourceHandle* jniOcResourceCollectionHandle = JniOcResourceHandle::getJniOcResourceHandlePtr(
        env, jResourceCollectionHandle);
    if (!jniOcResourceCollectionHandle) return;

    JniOcResourceHandle* jniOcResourceHandle = JniOcResourceHandle::getJniOcResourceHandlePtr(
        env, jResourceHandle);
    if (!jniOcResourceHandle) return;

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
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    bindResources0
* Signature: (Lorg/iotivity/base/OcResourceHandle;[Lorg/iotivity/base/OcResourceHandle;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_bindResources0
(JNIEnv *env, jclass clazz, jobject jResourceCollectionHandle, jobjectArray jResourceHandleArray)
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

    JniOcResourceHandle* jniOcResourceCollectionHandle = JniOcResourceHandle::getJniOcResourceHandlePtr(
        env, jResourceCollectionHandle);
    if (!jniOcResourceCollectionHandle) return;

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

        JniOcResourceHandle* jniOcResourceHandle = JniOcResourceHandle::getJniOcResourceHandlePtr(
            env, jResourceHandle);
        if (!jniOcResourceHandle) return;

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
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    unbindResource0
* Signature: (Lorg/iotivity/base/OcResourceHandle;Lorg/iotivity/base/OcResourceHandle;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_unbindResource0
(JNIEnv *env, jclass clazz, jobject jResourceCollectionHandle, jobject jResourceHandle)
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

    JniOcResourceHandle* jniOcResourceCollectionHandle = JniOcResourceHandle::getJniOcResourceHandlePtr(
        env, jResourceCollectionHandle);
    if (!jniOcResourceCollectionHandle) return;

    JniOcResourceHandle* jniOcResourceHandle = JniOcResourceHandle::getJniOcResourceHandlePtr(
        env, jResourceHandle);
    if (!jniOcResourceHandle) return;

    try
    {
        OCStackResult result = OCPlatform::unbindResource(
            jniOcResourceCollectionHandle->getOCResourceHandle(),
            jniOcResourceHandle->getOCResourceHandle()
            );

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Failed to unbind resource");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    unbindResources0
* Signature: (Lorg/iotivity/base/OcResourceHandle;[Lorg/iotivity/base/OcResourceHandle;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_unbindResources0
(JNIEnv *env, jclass clazz, jobject jResourceCollectionHandle, jobjectArray jResourceHandleArray)
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

    JniOcResourceHandle* jniOcResourceCollectionHandle = JniOcResourceHandle::getJniOcResourceHandlePtr(
        env, jResourceCollectionHandle);
    if (!jniOcResourceCollectionHandle) return;

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

        JniOcResourceHandle* jniOcResourceHandle = JniOcResourceHandle::getJniOcResourceHandlePtr(
            env, jResourceHandle);
        if (!jniOcResourceHandle) return;

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
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    bindTypeToResource0
* Signature: (Lorg/iotivity/base/OcResourceHandle;Ljava/lang/String;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_bindTypeToResource0
(JNIEnv *env, jclass clazz, jobject jResourceHandle, jstring jResourceTypeName)
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
        typeName = env->GetStringUTFChars(jResourceTypeName, NULL);
    }

    JniOcResourceHandle* jniOcResourceHandle = JniOcResourceHandle::getJniOcResourceHandlePtr(
        env, jResourceHandle);
    if (!jniOcResourceHandle) return;

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
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
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
        interfaceName = env->GetStringUTFChars(jResourceInterfaceName, NULL);
    }

    JniOcResourceHandle* jniOcResourceHandle = JniOcResourceHandle::getJniOcResourceHandlePtr(
        env, jResourceHandle);
    if (!jniOcResourceHandle) return;

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
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    startPresence0
* Signature: (I)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_startPresence0
(JNIEnv *env, jclass clazz, jint ttl)
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
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    stopPresence0
* Signature: ()V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_stopPresence0
(JNIEnv *env, jclass clazz)
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
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    subscribePresence0
* Signature: (Ljava/lang/String;ILorg/iotivity/base/OcPlatform/OnPresenceListener;)Lorg/iotivity/base/OcPresenceHandle;
*/
JNIEXPORT jobject JNICALL Java_org_iotivity_base_OcPlatform_subscribePresence0
(JNIEnv *env, jclass clazz, jstring jHost, jint jConnectivityType, jobject jListener)
{
    LOGD("OcPlatform_subscribePresence");
    std::string host;
    if (jHost)
    {
        host = env->GetStringUTFChars(jHost, NULL);
    }
    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "onPresenceListener cannot be null");
        return nullptr;
    }

    JniOnPresenceListener *onPresenceListener = AddOnPresenceListener(env, jListener);

    SubscribeCallback subscribeCallback = [onPresenceListener](OCStackResult result, const unsigned int nonce,
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
            JniUtils::getConnectivityType(env, static_cast<int>(jConnectivityType)),
            subscribeCallback);

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "subscribe presence has failed");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
        return nullptr;
    }

    JniOcPresenceHandle* jniPresenceHandle = new JniOcPresenceHandle(onPresenceListener, presenceHandle);
    jlong jhandle = reinterpret_cast<jlong>(jniPresenceHandle);
    jobject jPresenceHandle = env->NewObject(g_cls_OcPresenceHandle, g_mid_OcPresenceHandle_N_ctor, jhandle);
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
JNIEXPORT jobject JNICALL Java_org_iotivity_base_OcPlatform_subscribePresence1
(JNIEnv *env, jclass clazz, jstring jHost, jstring jResourceType, jint jConnectivityType, jobject jListener)
{
    LOGD("OcPlatform_subscribePresence1");
    std::string host;
    if (jHost)
    {
        host = env->GetStringUTFChars(jHost, NULL);
    }
    std::string resourceType;
    if (jResourceType)
    {
        resourceType = env->GetStringUTFChars(jResourceType, NULL);
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
            JniUtils::getConnectivityType(env, static_cast<int>(jConnectivityType)),
            subscribeCallback);

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "subscribe presence has failed");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
        return nullptr;
    }

    JniOcPresenceHandle* jniPresenceHandle = new JniOcPresenceHandle(onPresenceListener, presenceHandle);
    jlong jhandle = reinterpret_cast<jlong>(jniPresenceHandle);
    jobject jPresenceHandle = env->NewObject(g_cls_OcPresenceHandle, g_mid_OcPresenceHandle_N_ctor, jhandle);
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
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_unsubscribePresence0
(JNIEnv *env, jclass clazz, jobject jPresenceHandle)
{
    LOGD("OcPlatform_unsubscribePresence");
    if (!jPresenceHandle)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "presenceHandle cannot be null");
        return;
    }
    JniOcPresenceHandle* jniPresenceHandle = JniOcPresenceHandle::getJniOcPresenceHandlePtr(env, jPresenceHandle);
    if (!jniPresenceHandle) return;

    OCPresenceHandle presenceHandle = jniPresenceHandle->getOCPresenceHandle();

    try
    {
        OCStackResult result = OCPlatform::unsubscribePresence(presenceHandle);

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "unsubscribe presence has failed");
            return;
        }
        jweak jwOnPresenceListener = jniPresenceHandle->getJniOnPresenceListener()->getJWListener();
        if (jwOnPresenceListener)
        {
            RemoveOnPresenceListener(env, jwOnPresenceListener);
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    constructResourceObject0
* Signature: (Ljava/lang/String;Ljava/lang/String;IZ[Ljava/lang/String;[Ljava/lang/String;)
Lorg/iotivity/base/OcResource;
*/
JNIEXPORT jobject JNICALL Java_org_iotivity_base_OcPlatform_constructResourceObject0
(JNIEnv *env, jclass clazz, jstring jHost, jstring jUri, jint jConnectivityType,
jboolean jIsObservable, jobjectArray jResourceTypeArray, jobjectArray jInterfaceArray)
{
    LOGD("OcPlatform_constructResourceObject");
    std::string host;
    if (jHost)
    {
        host = env->GetStringUTFChars(jHost, NULL);
    }
    std::string uri;
    if (jUri)
    {
        uri = env->GetStringUTFChars(jUri, NULL);
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
        JniUtils::getConnectivityType(env, static_cast<int>(jConnectivityType)),
        static_cast<bool>(jIsObservable),
        resourceTypes,
        interfaces);

    if (!resource)
    {
        ThrowOcException(OC_STACK_ERROR, "Failed to create OCResource");
        return nullptr;
    }

    JniOcResource *jniOcResource = new JniOcResource(resource);
    jlong handle = reinterpret_cast<jlong>(jniOcResource);

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
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_sendResponse0
(JNIEnv *env, jclass clazz, jobject jResourceResponse)
{
    LOGD("OcPlatform_sendResponse");
    if (!jResourceResponse)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "resourceResponse cannot be null");
        return;
    }

    JniOcResourceResponse *jniResponse = JniOcResourceResponse::getJniOcResourceResponsePtr(
        env, jResourceResponse);
    if (!jniResponse) return;

    try
    {
        OCStackResult result = OCPlatform::sendResponse(jniResponse->getOCResourceResponse());

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "failed to send response");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
    }
}
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

    resourceFoundMapMutex.lock();

    for (auto it = onResourceFoundListenerMap.begin(); it != onResourceFoundListenerMap.end(); ++it)
    {
        if (env->IsSameObject(jListener, it->first))
        {
            auto refPair = it->second;
            onResourceFoundListener = refPair.first;
            refPair.second++;
            it->second = refPair;
            onResourceFoundListenerMap.insert(*it);
            LOGD("OnResourceFoundListener: ref. count is incremented");
            break;
        }
    }

    if (!onResourceFoundListener)
    {
        onResourceFoundListener = new JniOnResourceFoundListener(env, jListener);
        jobject jgListener = env->NewGlobalRef(jListener);

        onResourceFoundListenerMap.insert(std::pair<jobject, std::pair<JniOnResourceFoundListener*, int>>(jgListener, std::pair<JniOnResourceFoundListener*, int>(onResourceFoundListener, 1)));
        LOGI("OnResourceFoundListener: new listener");
    }

    resourceFoundMapMutex.unlock();

    return onResourceFoundListener;
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
            LOGD("OnDeviceInfoListener: ref. count is incremented");
            break;
        }
    }

    if (!onDeviceInfoListener)
    {
        onDeviceInfoListener = new JniOnDeviceInfoListener(env, jListener);
        jobject jgListener = env->NewGlobalRef(jListener);

        onDeviceInfoListenerMap.insert(std::pair<jobject, std::pair<JniOnDeviceInfoListener*, int>>(jgListener, std::pair<JniOnDeviceInfoListener*, int>(onDeviceInfoListener, 1)));
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
                LOGI("OnDeviceInfoListener: ref. count is decremented");
            }
            else
            {
                env->DeleteGlobalRef(it->first);
                JniOnDeviceInfoListener* listener = refPair.first;
                delete listener;
                onDeviceInfoListenerMap.erase(it);

                LOGI("OnDeviceInfoListener is removed");
            }

            isFound = true;
            break;
        }
    }

    if (!isFound)
    {
        ThrowOcException(JNI_EXCEPTION, "OnDeviceInfoListenet isn't found");
    }

    deviceInfoMapLock.unlock();
}

JniOnPresenceListener* AddOnPresenceListener(JNIEnv* env, jobject jListener)
{
    JniOnPresenceListener *onPresenceListener = NULL;

    onPresenceMapLock.lock();

    for (auto it = onPresenceListenerMap.begin(); it != onPresenceListenerMap.end(); ++it)
    {
        if (env->IsSameObject(jListener, it->first))
        {
            auto refPair = it->second;
            onPresenceListener = refPair.first;
            refPair.second++;
            it->second = refPair;
            onPresenceListenerMap.insert(*it);
            LOGD("OnPresenceListener: ref. count is incremented");
            break;
        }
    }

    if (!onPresenceListener)
    {
        onPresenceListener = new JniOnPresenceListener(env, jListener);
        jobject jgListener = env->NewGlobalRef(jListener);

        onPresenceListenerMap.insert(std::pair<jobject, std::pair<JniOnPresenceListener*, int>>(jgListener, std::pair<JniOnPresenceListener*, int>(onPresenceListener, 1)));
        LOGI("OnPresenceListener: new listener");
    }

    onPresenceMapLock.unlock();

    return onPresenceListener;
}

void RemoveOnPresenceListener(JNIEnv* env, jobject jListener)
{
    onPresenceMapLock.lock();

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
                LOGI("OnPresenceListener: ref. count is decremented");
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
        ThrowOcException(JNI_EXCEPTION, "OnPresenceListenet isn't found");
    }

    onPresenceMapLock.unlock();
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

    string ipAddress;
    if (jIpAddress)
    {
        ipAddress = env->GetStringUTFChars(jIpAddress, NULL);
    }
    uint16_t port;
    if (jPort > 0)
    {
        port = (uint16_t)jPort;
    }
    PlatformConfig cfg{
        JniUtils::getServiceType(env, jServiceType),
        JniUtils::getModeType(env, jModeType),
        ipAddress,
        port,
        JniUtils::getQOS(env, jQOS)
    };
       
    OCPlatform::Configure(cfg);
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    notifyAllObservers
* Signature: (Lorg/iotivity/base/OcResourceHandle;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_notifyAllObservers
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
    try
    {
        OCStackResult result = OCPlatform::notifyAllObservers(jniOcResourceHandle->getOCResourceHandle());

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Failed to notify all observers");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
        if (jniOcResourceHandle)
        {
            delete jniOcResourceHandle;
            jniOcResourceHandle = NULL;
        }
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

    try{

        OCStackResult result = OCPlatform::notifyAllObservers(
            jniOcResourceHandle->getOCResourceHandle(),
            JniUtils::getQOS(env, jQoS));

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Failed to notify all observers");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
        if (jniOcResourceHandle)
        {
            delete jniOcResourceHandle;
            jniOcResourceHandle = NULL;
        }
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

    JniOcResourceResponse* jniOcResourceResponse = JniOcResourceResponse::getJniOcResourceResponsePtr(
        env, jResourceResponse);

    int len = env->GetArrayLength(jObservationIdArr);
    uint8_t* bArr = (uint8_t*)env->GetByteArrayElements(jObservationIdArr, 0);

    ObservationIds observationIds;
    for (int i = 0; i < len; i++)
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

    JniOcResourceResponse* jniOcResourceResponse = JniOcResourceResponse::getJniOcResourceResponsePtr(
        env, jResourceResponse);

    int len = env->GetArrayLength(jObservationIdArr);
    uint8_t* bArr = (uint8_t*)env->GetByteArrayElements(jObservationIdArr, 0);

    ObservationIds observationIds;
    for (int i = 0; i < len; i++)
    {
        observationIds.push_back(bArr[i]);
    }

    env->ReleaseByteArrayElements(jObservationIdArr, (jbyte*)bArr, 0);

    try{
        OCStackResult result = OCPlatform::notifyListOfObservers(
            jniOcResourceHandle->getOCResourceHandle(),
            observationIds,
            jniOcResourceResponse->getOCResourceResponse(),
            JniUtils::getQOS(env, (int)jQoS));

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
* Method:    findResource
* Signature: (Ljava/lang/String;Ljava/lang/String;Lorg/iotivity/base/OcPlatform/OnResourceFoundListener;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_findResource
    (JNIEnv *env, jclass clazz, jstring jHost, jstring jResourceUri, jobject jListener)
{
    LOGD("OcPlatform_findResource");
    string host;
    if (jHost)
    {
        host = env->GetStringUTFChars(jHost, NULL);
    }
    string resourceUri;
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
        OCStackResult result = OCPlatform::findResource(host, resourceUri, findCallback);

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Find resource has failed");

            if (onResFoundListener)
            {
                delete onResFoundListener;
                onResFoundListener = NULL;
            }
        }

    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
        if (onResFoundListener)
        {
            delete onResFoundListener;
            onResFoundListener = NULL;
        }
    }
}


/*
* Class:     org_iotivity_base_OcPlatform
* Method:    findResource1
* Signature: (Ljava/lang/String;Ljava/lang/String;Lorg/iotivity/base/OcPlatform/OnResourceFoundListener;I)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_findResource1
    (JNIEnv *env, jclass clazz, jstring jHost, jstring jResourceUri, jobject jListener, jint jQoS)
{
    LOGD("OcPlatform_findResource");
    string host;
    if (jHost)
    {
        host = env->GetStringUTFChars(jHost, NULL);
    }
    string resourceUri;
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
            findCallback,
            JniUtils::getQOS(env, (int)jQoS));

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Find resource has failed");

            if (onResFoundListener)
            {
                delete onResFoundListener;
                onResFoundListener = NULL;
            }
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
        if (onResFoundListener)
        {
            delete onResFoundListener;
            onResFoundListener = NULL;
        }
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    getDeviceInfo
* Signature: (Ljava/lang/String;Ljava/lang/String;Lorg/iotivity/base/OcPlatform/OnDeviceFoundListener;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_getDeviceInfo
    (JNIEnv *env, jclass clazz, jstring jHost, jstring jResourceUri, jobject jListener)
{
    LOGD("OcPlatform_getDeviceInfo");
    string host;
    if (jHost)
    {
        host = env->GetStringUTFChars(jHost, NULL);
    }
    string resourceUri;
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
            findDeviceCallback);

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Find device has failed");

            if (onDeviceInfoListener)
            {
                delete onDeviceInfoListener;
                onDeviceInfoListener = NULL;
            }
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
        if (onDeviceInfoListener)
        {
            delete onDeviceInfoListener;
            onDeviceInfoListener = NULL;
        }
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    getDeviceInfo1
* Signature: (Ljava/lang/String;Ljava/lang/String;Lorg/iotivity/base/OcPlatform/OnDeviceFoundListener;I)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_getDeviceInfo1
    (JNIEnv *env, jclass clazz, jstring jHost, jstring jResourceUri, jobject jListener, jint jQoS)
{
    LOGD("OcPlatform_getDeviceInfo1");
    string host;
    if (jHost)
    {
        host = env->GetStringUTFChars(jHost, NULL);
    }
    string resourceUri;
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
            findDeviceCallback,
            JniUtils::getQOS(env, jQoS));

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Find device has failed");

            if (onDeviceInfoListener)
            {
                delete onDeviceInfoListener;
                onDeviceInfoListener = NULL;
            }
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
        if (onDeviceInfoListener)
        {
            delete onDeviceInfoListener;
            onDeviceInfoListener = NULL;
        }
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    registerResource
* Signature: (Lorg/iotivity/base/OcResource;)Lorg/iotivity/base/OcResourceHandle;
*/
JNIEXPORT jobject JNICALL Java_org_iotivity_base_OcPlatform_registerResource
(JNIEnv *env, jclass clazz, jobject jResource)
{
    LOGD("OcPlatform_registerResource");
    if (!jResource)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "Resource cannot be null");
        return NULL;
    }
    JniOcResource *resource = JniOcResource::getJniOcResourcePtr(env, jResource);

    OCResourceHandle resourceHandle;
    try
    {
        OCStackResult result = OCPlatform::registerResource(
            resourceHandle,
            resource->getOCResource()
            );

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "register resource");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
        return NULL;
    }

    JniOcResourceHandle* jniHandle = new JniOcResourceHandle(resourceHandle);

    jlong handle = reinterpret_cast<jlong>(jniHandle);

    jobject jResourceHandle = env->NewObject(g_cls_OcResourceHandle, g_mid_OcResourceHandle_N_ctor, handle);
    if (env->ExceptionCheck())
    {
        LOGE("Failed to create OcResourceHandle");
        delete jniHandle;
        jniHandle = NULL;
        return NULL;
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
    string resourceUri;
    if (jResourceUri)
    {
        resourceUri = env->GetStringUTFChars(jResourceUri, NULL);
    }
    string resourceTypeName;
    if (jResourceTypeName)
    {
        resourceTypeName = env->GetStringUTFChars(jResourceTypeName, NULL);
    }
    string resourceInterface;
    if (jResourceInterface)
    {
        resourceInterface = env->GetStringUTFChars(jResourceInterface, NULL);
    }
    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "entityHandler cannot be null");
        return NULL;
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
            (int)jResourceProperty);

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "register resource");
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
        return NULL;
    }

    JniOcResourceHandle* jniHandle = new JniOcResourceHandle(resourceHandle);

    jlong handle = reinterpret_cast<jlong>(jniHandle);

    jobject jResourceHandle = env->NewObject(g_cls_OcResourceHandle, g_mid_OcResourceHandle_N_ctor, handle);
    if (env->ExceptionCheck())
    {
        LOGE("Failed to create OcResourceHandle");
        delete jniHandle;
        jniHandle = NULL;
        return NULL;
    }

    return jResourceHandle;
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    registerDeviceInfo
* Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_registerDeviceInfo
(	JNIEnv *env, 
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

    string deviceName;
    if (jDeviceName)
    {
        deviceName = env->GetStringUTFChars(jDeviceName, NULL);
    }
    string hostName;
    if (jHostName)
    {
        hostName = env->GetStringUTFChars(jHostName, NULL);
    }
    string deviceUUID;
    if (jDeviceUUID)
    {
        deviceUUID = env->GetStringUTFChars(jDeviceUUID, NULL);
    }
    string contentType;
    if (jContentType)
    {
        contentType = env->GetStringUTFChars(jContentType, NULL);
    }
    string version;
    if (jVersion)
    {
        version = env->GetStringUTFChars(jVersion, NULL);
    }
    string manufacturerName;
    if (jManufacturerName)
    {
        manufacturerName = env->GetStringUTFChars(jManufacturerName, NULL);
    }
    string manufacturerUrl;
    if (jManufacturerUrl)
    {
        manufacturerUrl = env->GetStringUTFChars(jManufacturerUrl, NULL);
    }
    string modelNumber;
    if (jModelNumber)
    {
        modelNumber = env->GetStringUTFChars(jModelNumber, NULL);
    }
    string dateOfManufacture;
    if (jDateOfManufacture)
    {
        dateOfManufacture = env->GetStringUTFChars(jDateOfManufacture, NULL);
    }
    string platformVersion;
    if (jPlatformVersion)
    {
        platformVersion = env->GetStringUTFChars(jPlatformVersion, NULL);
    }
    string firmwareVersion;
    if (jFirmwareVersion)
    {
        firmwareVersion = env->GetStringUTFChars(jFirmwareVersion, NULL);
    }
    string supportUrl;
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
    catch (exception &e)
    {
        ThrowOcException(JNI_EXCEPTION, "Failed to register device info");
    }

    try
    {
        OCStackResult result = OCPlatform::registerDeviceInfo(deviceInfo);

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Failed to register device info");
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
* Method:    unregisterResource
* Signature: (Lorg/iotivity/base/OcResourceHandle;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_unregisterResource
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

    try
    {
        OCResourceHandle resHandle = jniOcResourceHandle->getOCResourceHandle();
        OCStackResult result = OCPlatform::unregisterResource(resHandle);
        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Failed to unregister resource");
            if (jniOcResourceHandle)
            {
                delete jniOcResourceHandle;
                jniOcResourceHandle = NULL;
            }
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
        if (jniOcResourceHandle)
        {
            delete jniOcResourceHandle;
            jniOcResourceHandle = NULL;
        }
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    bindResource
* Signature: (Lorg/iotivity/base/OcResourceHandle;Lorg/iotivity/base/OcResourceHandle;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_bindResource
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

    JniOcResourceHandle* jniOcResourceHandle = JniOcResourceHandle::getJniOcResourceHandlePtr(
        env, jResourceHandle);

    try
    {
        OCStackResult result = OCPlatform::bindResource(
            jniOcResourceCollectionHandle->getOCResourceHandle(),
            jniOcResourceHandle->getOCResourceHandle()
            );

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Failed to bind resource");

            if (jniOcResourceHandle)
            {
                delete jniOcResourceHandle;
                jniOcResourceHandle = NULL;
            }
            if (jniOcResourceCollectionHandle)
            {
                delete jniOcResourceCollectionHandle;
                jniOcResourceCollectionHandle = NULL;
            }
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
        if (jniOcResourceHandle)
        {
            delete jniOcResourceHandle;
            jniOcResourceHandle = NULL;
        }
        if (jniOcResourceCollectionHandle)
        {
            delete jniOcResourceCollectionHandle;
            jniOcResourceCollectionHandle = NULL;
        }
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    bindResources
* Signature: (Lorg/iotivity/base/OcResourceHandle;[Lorg/iotivity/base/OcResourceHandle;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_bindResources
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

    std::vector<OCResourceHandle> resourceHandleList;
    int len = env->GetArrayLength(jResourceHandleArray);
    for (int i = 0; i < len; i++)
    {
        jobject jResourceHandle = env->GetObjectArrayElement(jResourceHandleArray, i);
        if (!jResourceHandle) continue;

        JniOcResourceHandle* jniOcResourceHandle = JniOcResourceHandle::getJniOcResourceHandlePtr(
            env, jResourceHandle);

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
* Method:    unbindResource
* Signature: (Lorg/iotivity/base/OcResourceHandle;Lorg/iotivity/base/OcResourceHandle;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_unbindResource
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

    JniOcResourceHandle* jniOcResourceHandle = JniOcResourceHandle::getJniOcResourceHandlePtr(
        env, jResourceHandle);

    try
    {
        OCStackResult result = OCPlatform::unbindResource(
            jniOcResourceCollectionHandle->getOCResourceHandle(),
            jniOcResourceHandle->getOCResourceHandle()
            );

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Failed to unbind resource");

            if (jniOcResourceHandle)
            {
                delete jniOcResourceHandle;
                jniOcResourceHandle = NULL;
            }

            if (jniOcResourceCollectionHandle)
            {
                delete jniOcResourceCollectionHandle;
                jniOcResourceCollectionHandle = NULL;
            }
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());

        if (jniOcResourceHandle)
        {
            delete jniOcResourceHandle;
            jniOcResourceHandle = NULL;
        }

        if (jniOcResourceCollectionHandle)
        {
            delete jniOcResourceCollectionHandle;
            jniOcResourceCollectionHandle = NULL;
        }
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    unbindResources
* Signature: (Lorg/iotivity/base/OcResourceHandle;[Lorg/iotivity/base/OcResourceHandle;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_unbindResources
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

    std::vector<OCResourceHandle> resourceHandleList;
    int len = env->GetArrayLength(jResourceHandleArray);
    for (int i = 0; i < len; i++)
    {
        jobject jResourceHandle = env->GetObjectArrayElement(jResourceHandleArray, i);
        if (!jResourceHandle) continue;

        JniOcResourceHandle* jniOcResourceHandle = JniOcResourceHandle::getJniOcResourceHandlePtr(
            env, jResourceHandle);

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
* Method:    bindTypeToResource
* Signature: (Lorg/iotivity/base/OcResourceHandle;Ljava/lang/String;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_bindTypeToResource
    (JNIEnv *env, jclass clazz, jobject jResourceHandle, jstring jResourceTypeName)
{
    LOGI("OcPlatform_bindTypeToResource");
    if (!jResourceHandle)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "resourceHandle cannot be null");
        return;
    }
    string typeName;
    if (jResourceTypeName)
    {
        typeName = env->GetStringUTFChars(jResourceTypeName, NULL);
    }

    JniOcResourceHandle* jniOcResourceHandle = JniOcResourceHandle::getJniOcResourceHandlePtr(
        env, jResourceHandle);

    try
    {
        OCStackResult result = OCPlatform::bindTypeToResource(
            jniOcResourceHandle->getOCResourceHandle(),
            typeName
            );

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Failed to bind type to resource");
            if (jniOcResourceHandle)
            {
                delete jniOcResourceHandle;
                jniOcResourceHandle = NULL;
            }
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
        if (jniOcResourceHandle)
        {
            delete jniOcResourceHandle;
            jniOcResourceHandle = NULL;
        }
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    bindInterfaceToResource
* Signature: (Lorg/iotivity/base/OcResourceHandle;Ljava/lang/String;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_bindInterfaceToResource
    (JNIEnv *env, jclass clazz, jobject jResourceHandle, jstring jResourceInterfaceName)
{
    LOGI("OcPlatform_bindInterfaceToResource");
    if (!jResourceHandle)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "resourceHandle cannot be null");
        return;
    }
    string interfaceName;
    if (jResourceInterfaceName)
    {
        interfaceName = env->GetStringUTFChars(jResourceInterfaceName, NULL);
    }

    JniOcResourceHandle* jniOcResourceHandle = JniOcResourceHandle::getJniOcResourceHandlePtr(
        env, jResourceHandle);

    try
    {
        OCStackResult result = OCPlatform::bindInterfaceToResource(
            jniOcResourceHandle->getOCResourceHandle(),
            interfaceName
            );

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Failed to bind interface to resource");

            if (jniOcResourceHandle)
            {
                delete jniOcResourceHandle;
                jniOcResourceHandle = NULL;
            }
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());

        if (jniOcResourceHandle)
        {
            delete jniOcResourceHandle;
            jniOcResourceHandle = NULL;
        }
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    startPresence
* Signature: (I)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_startPresence
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
* Method:    stopPresence
* Signature: ()V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_stopPresence
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
* Method:    subscribePresence
* Signature: (Ljava/lang/String;Lorg/iotivity/base/OcPlatform/OnPresenceListener;)Lorg/iotivity/base/OcPresenceHandle;
*/
JNIEXPORT jobject JNICALL Java_org_iotivity_base_OcPlatform_subscribePresence
    (JNIEnv *env, jclass clazz, jstring jHost, jobject jListener)
{
    LOGD("OcPlatform_subscribePresence");
    string host;
    if (jHost)
    {
        host = env->GetStringUTFChars(jHost, NULL);
    }
    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "onPresenceListener cannot be null");
        return NULL;
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
            subscribeCallback);

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "subscribe presence has failed");

            if (onPresenceListener)
            {
                delete onPresenceListener;
                onPresenceListener = NULL;
            }
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
        return NULL;
    }

    JniOcPresenceHandle* jniPresenceHandle = new JniOcPresenceHandle(onPresenceListener, presenceHandle);

    jlong jhandle = reinterpret_cast<jlong>(jniPresenceHandle);

    jobject jPresenceHandle = env->NewObject(g_cls_OcPresenceHandle, g_mid_OcPresenceHandle_N_ctor, jhandle);
    if (env->ExceptionCheck())
    {
        LOGE("Failed to create OcPresenceHandle");

        if (jniPresenceHandle)
        {
            delete jniPresenceHandle;
            jniPresenceHandle = NULL;
        }
        return NULL;
    }

    return jPresenceHandle;
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    subscribePresence1
* Signature: (Ljava/lang/String;Ljava/lang/String;
Lorg/iotivity/base/OcPlatform/OnPresenceListener;)Lorg/iotivity/base/OcPresenceHandle;
*/
JNIEXPORT jobject JNICALL Java_org_iotivity_base_OcPlatform_subscribePresence1
    (JNIEnv *env, jclass clazz, jstring jHost, jstring jResourceType, jobject jListener)
{
    LOGD("OcPlatform_subscribePresence1");
    string host;
    if (jHost)
    {
        host = env->GetStringUTFChars(jHost, NULL);
    }
    string resourceType;
    if (jResourceType)
    {
        resourceType = env->GetStringUTFChars(jResourceType, NULL);
    }
    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "onPresenceListener cannot be null");
        return NULL;
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
            subscribeCallback);

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "subscribe presence has failed");

            if (onPresenceListener)
            {
                delete onPresenceListener;
                onPresenceListener = NULL;
            }
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
        return NULL;
    }

    JniOcPresenceHandle* jniPresenceHandle = new JniOcPresenceHandle(onPresenceListener, presenceHandle);

    jlong jhandle = reinterpret_cast<jlong>(jniPresenceHandle);

    jobject jPresenceHandle = env->NewObject(g_cls_OcPresenceHandle, g_mid_OcPresenceHandle_N_ctor, jhandle);
    if (env->ExceptionCheck())
    {
        LOGE("Failed to create OcPresenceHandle");

        if (jniPresenceHandle)
        {
            delete jniPresenceHandle;
            jniPresenceHandle = NULL;
        }
        return NULL;
    }

    return jPresenceHandle;
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    unsubscribePresence
* Signature: (Lorg/iotivity/base/OcPresenceHandle;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_unsubscribePresence
    (JNIEnv *env, jclass clazz, jobject jPresenceHandle)
{
    LOGD("OcPlatform_unsubscribePresence");
    if (!jPresenceHandle)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "presenceHandle cannot be null");
        return;
    }
    JniOcPresenceHandle* jniPresenceHandle = JniOcPresenceHandle::getJniOcPresenceHandlePtr(env, jPresenceHandle);

    OCPresenceHandle presenceHandle = jniPresenceHandle->getOCPresenceHandle();

    try
    {
        OCStackResult result = OCPlatform::unsubscribePresence(presenceHandle);

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "unsubscribe presence has failed");
        }

        RemoveOnPresenceListener(env, jniPresenceHandle->getJniOnPresenceListener()->getJWListener());

        delete jniPresenceHandle;
        jniPresenceHandle = NULL;
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
        return;
    }
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    constructResourceObject
* Signature: (Ljava/lang/String;Ljava/lang/String;Z[Ljava/lang/String;[Ljava/lang/String;)
Lorg/iotivity/base/OcResource;
*/
JNIEXPORT jobject JNICALL Java_org_iotivity_base_OcPlatform_constructResourceObject
    (JNIEnv *env, jclass clazz, jstring jHost, jstring jUri, jboolean jIsObservable, 
    jobjectArray jResourceTypeArray, jobjectArray jInterfaceArray)
{
    LOGD("OcPlatform_constructResourceObject");
    string host;
    if (jHost)
    {
        host = env->GetStringUTFChars(jHost, NULL);
    }
    string uri;
    if (jUri)
    {
        uri = env->GetStringUTFChars(jUri, NULL);
    }
    if (!jResourceTypeArray)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "resourceTypeList cannot be null");
        return NULL;
    }
    if (!jInterfaceArray)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "interfaceList cannot be null");
        return NULL;
    }

    std::vector<std::string> resourceTypes;
    JniUtils::convertJavaStrArrToStrVector(env, jResourceTypeArray, resourceTypes);

    std::vector<std::string> interfaces;
    JniUtils::convertJavaStrArrToStrVector(env, jInterfaceArray, interfaces);

    std::shared_ptr<OCResource> resource = OCPlatform::constructResourceObject(
        host,
        uri,
        (bool)jIsObservable,
        resourceTypes,
        interfaces);

    if (!resource)
    {
        ThrowOcException(OC_STACK_ERROR, "Failed to create OCResource");
        return NULL; 
    }

    jobject jResource = env->NewObject(g_cls_OcResource, g_mid_OcResource_ctor);

    JniOcResource *jniOcResource = new JniOcResource(resource);
    jlong handle = reinterpret_cast<jlong>(jniOcResource);

    SetHandle<JniOcResource>(env, jResource, jniOcResource);

    return jResource;
}

/*
* Class:     org_iotivity_base_OcPlatform
* Method:    sendResponse
* Signature: (Lorg/iotivity/base/OcResourceResponse;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcPlatform_sendResponse
    (JNIEnv *env, jclass clazz, jobject jResourceResponse)
{
    LOGD("OcPlatform_sendResponse");
    if (!jResourceResponse)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "resourceResponse cannot be null");
        return;
    }

    JniOcResourceResponse *jniResponse = JniOcResourceResponse::getJniOcResourceResponsePtr(
        env, 
        jResourceResponse);
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
        return;
    }
}


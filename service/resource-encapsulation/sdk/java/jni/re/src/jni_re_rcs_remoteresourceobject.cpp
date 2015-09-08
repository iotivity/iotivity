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

#include "jni_re_rcs_remoteresourceobject.h"

using namespace OIC::Service;
JniRCSRemoteResourceObject::JniRCSRemoteResourceObject(std::shared_ptr< RCSRemoteResourceObject>
        resource)
    : m_sharedResource(resource)
{}

JniRCSRemoteResourceObject::~JniRCSRemoteResourceObject()
{
    LOGD("JniRCSRemoteResourceObject::~JniOcResource()");
    m_sharedResource = NULL;

    jint envRet;
    JNIEnv *env = GetREJNIEnv(envRet);
    if (NULL == env) return;

    if (JNI_EDETACHED == envRet) g_jvm->DetachCurrentThread();
}

JniRCSRemoteResourceObject *JniRCSRemoteResourceObject::getJniRCSRemoteResourceObject(JNIEnv *env,
        jobject thiz)
{
    JniRCSRemoteResourceObject *resource = REGetHandle<JniRCSRemoteResourceObject>(env, thiz);
    if (env->ExceptionCheck())
    {
        LOGE("Failed to get native handle from RCSRemoteResourceObject");
    }
    if (!resource)
    {
        LOGE("getJniRCSRemoteResourceObject :: no resource");
    }
    return resource;
}

std::string JniRCSRemoteResourceObject::getUri()
{
    return m_sharedResource->getUri();
}

std::string JniRCSRemoteResourceObject::getAddress()
{
    return m_sharedResource->getAddress();
}

jobject JniRCSRemoteResourceObject::getTypes(JNIEnv *env)
{
    return convertStrVectorToJavaStrList( env, m_sharedResource->getTypes());
}

jobject JniRCSRemoteResourceObject::getInterfaces(JNIEnv *env)
{
    return convertStrVectorToJavaStrList(env, m_sharedResource->getInterfaces());
}

bool JniRCSRemoteResourceObject::isMonitoring()
{
    return m_sharedResource->isMonitoring();
}

bool JniRCSRemoteResourceObject::isCaching()
{
    return m_sharedResource->isCaching();
}

bool JniRCSRemoteResourceObject::isObservable()
{
    return m_sharedResource->isObservable();
}

bool JniRCSRemoteResourceObject::isCachedAvailable()
{
    return m_sharedResource->isCachedAvailable();
}

int JniRCSRemoteResourceObject::getState()
{
    ResourceState state = m_sharedResource->getState();
    return covertResourceStateToInt(state);
}

int JniRCSRemoteResourceObject::getCacheState()
{
    CacheState state = m_sharedResource->getCacheState();
    return convertCacheStateToInt(state);
}

void JniRCSRemoteResourceObject::startMonitoring(JNIEnv *env, jobject jListener)
{

    try
    {
        JniResourceStateChangeListener *onResourceStateChanged = addOnStateChangeListener(env, jListener);
        RCSRemoteResourceObject::StateChangedCallback stateChangecb = [onResourceStateChanged](
                    OIC::Service::ResourceState state)
        {
            onResourceStateChanged->stateChangeCallback(state);
        };
        m_sharedResource->startMonitoring(stateChangecb);
    }
    catch (InvalidParameterException exception)
    {
        LOGE("native startMonitoring InvalidParameterException");
        throwRCSException(env,  exception.what());
    }
    catch (BadRequestException exception)
    {
        LOGE("native startMonitoring : BadRequestException ");
        throwRCSException(env,  exception.what());
    }

}

void  JniRCSRemoteResourceObject::stopMonitoring()
{
    m_sharedResource->stopMonitoring();
}

void JniRCSRemoteResourceObject::startCaching(JNIEnv *env, jobject jListener)
{
    JniCacheUpdateListener *onCacheUpdated = addOnCacheUpdateListener(env, jListener);
    RCSRemoteResourceObject::CacheUpdatedCallback cacheUpdatedCallback = [onCacheUpdated](
                OIC::Service::RCSResourceAttributes attributes)
    {
        onCacheUpdated->cacheUpdatedCallback(attributes);
    };
    try
    {
        m_sharedResource->startCaching(cacheUpdatedCallback);
    }
    catch (BadRequestException exception)
    {
        throwRCSException(env,  exception.what());
    }

}

void JniRCSRemoteResourceObject::startCaching(JNIEnv *env)
{
    try
    {
        m_sharedResource->startCaching();
    }
    catch (BadRequestException exception)
    {
        throwRCSException(env,  exception.what());
    }
}

void JniRCSRemoteResourceObject::stopCaching()
{
    m_sharedResource->stopCaching();
}

void JniRCSRemoteResourceObject::getRemoteAttributes(JNIEnv *env, jobject jListener)
{
    JniGetAttributesListener *onGetAttributes = addOngetAttributesListener(env, jListener);
    RCSRemoteResourceObject::RemoteAttributesGetCallback getArributescb = [onGetAttributes](
                OIC::Service::RCSResourceAttributes attributes)
    {
        onGetAttributes->onRemoteAttributesGetCallback(attributes);
    };
    m_sharedResource->getRemoteAttributes(getArributescb);
}

void JniRCSRemoteResourceObject::setRemoteAttributes(JNIEnv *env, jobject attributes,
        jobject jListener)
{
    JniSetAttributesListener *onSetAttributes = addOnsetAttributesListener(env, jListener);
    RCSRemoteResourceObject::RemoteAttributesSetCallback setArributescb = [onSetAttributes](
                OIC::Service::RCSResourceAttributes attribute)
    {
        onSetAttributes->onRemoteAttributesSetCallback(attribute);
    };
    RCSResourceAttributes *attributesObj =   JniResourceAttributes::getResourceAttributePtr(env,
            attributes);
    m_sharedResource->setRemoteAttributes(*attributesObj, setArributescb);
}


jobject JniRCSRemoteResourceObject::getCachedAttributes(JNIEnv *env)
{
    try
    {
        RCSResourceAttributes attributes = m_sharedResource->getCachedAttributes();
        RCSResourceAttributes *attributesObj = new RCSResourceAttributes(attributes);
        jlong handle = reinterpret_cast<jlong>(attributesObj);
        jobject jAttributes = env->NewObject(g_cls_RCSRemoteResourceAttributesObject,
                                             g_mid_RCSRemoteResourceAttributesObject_ctor,
                                             handle, true);
        return jAttributes;
    }
    catch (BadRequestException exception)
    {
        throwRCSException(env,  exception.what());
    }
}

jstring JniRCSRemoteResourceObject::getCachedAttribute(JNIEnv *env, jstring jkey,
        jobject interfaceObject )
{
    const char *str = env->GetStringUTFChars(jkey , NULL);
    std::string s(str);
    char buf[64]; // assumed large enough to cope with result
    env->ReleaseStringUTFChars(jkey, str);
    RCSResourceAttributes::Value value;
    RCSResourceAttributes::TypeId typeId;
    try
    {
        value =  m_sharedResource->getCachedAttribute(s);
        typeId = value.getType().getId();
    }
    catch (BadRequestException exception)
    {
        throwRCSException(env,  exception.what());
    }

    if (typeId == RCSResourceAttributes::TypeId::INT)
    {
        int val = value.get<int>();
        sprintf(buf, "%d", val);
        return (env)->NewStringUTF(buf);
    }
    else if (typeId == RCSResourceAttributes::TypeId::DOUBLE)
    {
        double val = value.get<double>();
        sprintf(buf, "%f", val);
        return (env)->NewStringUTF(buf);
    }
    else if (typeId == RCSResourceAttributes::TypeId::BOOL)
    {
        bool val = value.get<bool>();
        sprintf(buf, "%d", val);
        return (env)->NewStringUTF(buf);
    }
    else if (typeId == RCSResourceAttributes::TypeId::STRING)
    {
        std::string val = value.get<std::string>();
        jstring jstr = env->NewStringUTF(val.c_str());
        return jstr;
    }
}
JniResourceStateChangeListener *JniRCSRemoteResourceObject::addOnStateChangeListener(JNIEnv *env,
        jobject jListener)
{
    return this->m_onStateChange.addListener(env, jListener, this);
}

JniCacheUpdateListener *JniRCSRemoteResourceObject::addOnCacheUpdateListener(JNIEnv *env,
        jobject jListener)
{
    return this->m_onCacheUpdateChange.addListener(env, jListener, this);
}

JniGetAttributesListener *JniRCSRemoteResourceObject::addOngetAttributesListener(JNIEnv *env,
        jobject jListener)
{
    return this->m_ongetAttributes.addListener(env, jListener, this);
}

JniSetAttributesListener *JniRCSRemoteResourceObject::addOnsetAttributesListener(JNIEnv *env,
        jobject jListener)
{
    return this->m_onsetAttributes.addListener(env, jListener, this);
}

void JniRCSRemoteResourceObject::removeOnStateChangeListener(JNIEnv *env, jobject jListener)
{
    this->m_onStateChange.removeListener(env, jListener);
}

void JniRCSRemoteResourceObject::removeCacheUpdateListener(JNIEnv *env, jobject jListener)
{
    this->m_onCacheUpdateChange.removeListener(env, jListener);
}

void JniRCSRemoteResourceObject::removeOngetAttributesListener(JNIEnv *env, jobject jListener)
{
    this->m_ongetAttributes.removeListener(env, jListener);
}

void JniRCSRemoteResourceObject::removeOnsetAttributesListener(JNIEnv *env, jobject jListener)
{
    this->m_onsetAttributes.removeListener(env, jListener);
}

JNIEXPORT jboolean JNICALL
Java_org_iotivity_ResourceEncapsulation_client_RCSRemoteResourceObject_nativeIsMonitoring
(JNIEnv *env, jobject interfaceClass)
{
    LOGD("RCSRemoteResourceObject_nativeIsMonitoring");
    JniRCSRemoteResourceObject *resource = JniRCSRemoteResourceObject::getJniRCSRemoteResourceObject(
            env, interfaceClass);
    return (jboolean)resource->isMonitoring();
}

JNIEXPORT jboolean JNICALL
Java_org_iotivity_ResourceEncapsulation_client_RCSRemoteResourceObject_nativeIsCaching
(JNIEnv *env, jobject interfaceClass)
{

    LOGD("RCSRemoteResourceObject_nativeIsCaching");
    JniRCSRemoteResourceObject *resource = JniRCSRemoteResourceObject::getJniRCSRemoteResourceObject(
            env, interfaceClass);
    return (jboolean)resource->isCaching();
}

JNIEXPORT jboolean JNICALL
Java_org_iotivity_ResourceEncapsulation_client_RCSRemoteResourceObject_nativeIsObservable
(JNIEnv *env, jobject interfaceClass)
{
    LOGD("RCSRemoteResourceObject_nativeIsObservable");
    JniRCSRemoteResourceObject *resource = JniRCSRemoteResourceObject::getJniRCSRemoteResourceObject(
            env, interfaceClass);
    return (jboolean)resource->isObservable();
}

JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_client_RCSRemoteResourceObject_nativeStartMonitoring
(JNIEnv *env, jobject interfaceClass, jobject jStateChangedListener)
{

    if (!jStateChangedListener)
    {
        LOGE("RCSRemoteResourceObject_nativeStartMonitoring : listener is NULL");
        throwRCSException(env,  "Invalid parameter");
        return;
    }
    LOGD("RCSRemoteResourceObject_nativeStartMonitoring");
    JniRCSRemoteResourceObject *resource = JniRCSRemoteResourceObject::getJniRCSRemoteResourceObject(
            env, interfaceClass);
    if (!resource) return;

    resource->startMonitoring(env, jStateChangedListener);
}

JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_client_RCSRemoteResourceObject_nativeStopMonitoring
(JNIEnv *env, jobject interfaceClass)
{
    LOGD("RCSRemoteResourceObject_nativeStopMonitoring");
    JniRCSRemoteResourceObject *resource = JniRCSRemoteResourceObject::getJniRCSRemoteResourceObject(
            env, interfaceClass);
    resource->stopMonitoring();
}

JNIEXPORT jint JNICALL
Java_org_iotivity_ResourceEncapsulation_client_RCSRemoteResourceObject_nativeGetState
(JNIEnv *env, jobject interfaceClass)
{
    LOGD("RCSRemoteResourceObject_nativeGetState");
    JniRCSRemoteResourceObject *resource = JniRCSRemoteResourceObject::getJniRCSRemoteResourceObject(
            env, interfaceClass);
    return (jint)resource->getState();
}

JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_client_RCSRemoteResourceObject_nativeStartCaching
(JNIEnv *env, jobject interfaceClass, jobject jCacheUpdateListener)
{
    LOGD("RCSRemoteResourceObject_nativeStartCaching");
    JniRCSRemoteResourceObject *resource = JniRCSRemoteResourceObject::getJniRCSRemoteResourceObject(
            env, interfaceClass);
    if (!resource) return;
    if (NULL == jCacheUpdateListener)
    {
        resource->startCaching(env);
    }
    else
    {
        resource->startCaching(env, jCacheUpdateListener);
    }
}
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_client_RCSRemoteResourceObject_nativeStopCaching
(JNIEnv *env, jobject interfaceClass)
{
    LOGD("RCSRemoteResourceObject_nativeStopCaching");
    JniRCSRemoteResourceObject *resource = JniRCSRemoteResourceObject::getJniRCSRemoteResourceObject(
            env, interfaceClass);
    resource->stopCaching();
}

JNIEXPORT jint JNICALL
Java_org_iotivity_ResourceEncapsulation_client_RCSRemoteResourceObject_nativeGetCacheState
(JNIEnv *env, jobject interfaceClass)
{
    LOGD("RCSRemoteResourceObject_nativeGetCacheState");
    JniRCSRemoteResourceObject *resource = JniRCSRemoteResourceObject::getJniRCSRemoteResourceObject(
            env, interfaceClass);
    return (jint)resource->getCacheState();
}

JNIEXPORT jboolean JNICALL
Java_org_iotivity_ResourceEncapsulation_client_RCSRemoteResourceObject_nativeIsCachedAvailable
(JNIEnv *env, jobject interfaceClass)
{

    LOGD("RCSRemoteResourceObject_nativeIsMonitoring");
    JniRCSRemoteResourceObject *resource = JniRCSRemoteResourceObject::getJniRCSRemoteResourceObject(
            env, interfaceClass);
    return (jboolean)resource->isCachedAvailable();
}

JNIEXPORT jobject JNICALL
Java_org_iotivity_ResourceEncapsulation_client_RCSRemoteResourceObject_nativeGetCachedAttributes
(JNIEnv *env, jobject interfaceClass)
{
    LOGD("RCSRemoteResourceObject_nativeGetCachedAttributes");
    JniRCSRemoteResourceObject *resource = JniRCSRemoteResourceObject::getJniRCSRemoteResourceObject(
            env, interfaceClass);
    return resource->getCachedAttributes(env);
}

JNIEXPORT jstring JNICALL
Java_org_iotivity_ResourceEncapsulation_client_RCSRemoteResourceObject_nativeGetCachedAttribute
(JNIEnv *env, jobject interfaceClass, jstring jKey)
{
    if (!jKey)
    {
        LOGE("RCSRemoteResourceObject_nativeGetCachedAttributes : key is NULL");
        throwRCSException(env,  "Key is empty");
        return NULL;
    }
    LOGD("RCSRemoteResourceObject_nativeGetRemoteAttributes");
    JniRCSRemoteResourceObject *resource = JniRCSRemoteResourceObject::getJniRCSRemoteResourceObject(
            env, interfaceClass);
    return resource->getCachedAttribute(env, jKey, interfaceClass);
}

JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_client_RCSRemoteResourceObject_nativeGetRemoteAttributes
(JNIEnv *env, jobject interfaceClass, jobject jResourceAttributesListener)
{
    LOGD("RCSRemoteResourceObject_nativeGetRemoteAttributes");
    JniRCSRemoteResourceObject *resource = JniRCSRemoteResourceObject::getJniRCSRemoteResourceObject(
            env, interfaceClass);
    if (!resource) return;

    resource->getRemoteAttributes(env, jResourceAttributesListener);
}


JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_client_RCSRemoteResourceObject_nativeSetRemoteAttributes
(JNIEnv *env, jobject interfaceClass, jobject jresourceAttribute,
 jobject jResourceAttributesListener)
{
    LOGD("RCSRemoteResourceObject_nativeSetRemoteAttributes");
    JniRCSRemoteResourceObject *resource = JniRCSRemoteResourceObject::getJniRCSRemoteResourceObject(
            env, interfaceClass);
    if (!resource) return;

    resource->setRemoteAttributes(env, jresourceAttribute, jResourceAttributesListener);
}

JNIEXPORT jstring JNICALL
Java_org_iotivity_ResourceEncapsulation_client_RCSRemoteResourceObject_nativeGetUri
(JNIEnv *env, jobject interfaceClass)
{
    LOGD("JNI RCSRemoteResourceObjectNativeInterface_getUri");
    JniRCSRemoteResourceObject *resource = JniRCSRemoteResourceObject::getJniRCSRemoteResourceObject(
            env, interfaceClass);
    if (!resource) return nullptr;

    return env->NewStringUTF(resource->getUri().c_str());
}

JNIEXPORT jstring JNICALL
Java_org_iotivity_ResourceEncapsulation_client_RCSRemoteResourceObject_nativeGetAddress
(JNIEnv *env, jobject interfaceClass)
{
    LOGD("RCSRemoteResourceObject_nativeGetAddress");
    JniRCSRemoteResourceObject *resource = JniRCSRemoteResourceObject::getJniRCSRemoteResourceObject(
            env, interfaceClass);
    if (!resource) return nullptr;

    return env->NewStringUTF(resource->getAddress().c_str());
}

JNIEXPORT jobject JNICALL
Java_org_iotivity_ResourceEncapsulation_client_RCSRemoteResourceObject_nativeGetTypes
(JNIEnv *env, jobject interfaceClass)
{
    LOGD("RCSRemoteResourceObject_nativeGetTypes");
    JniRCSRemoteResourceObject *resource = JniRCSRemoteResourceObject::getJniRCSRemoteResourceObject(
            env, interfaceClass);
    if (!resource) return nullptr;
    return resource->getTypes(env);
}

JNIEXPORT jobject JNICALL
Java_org_iotivity_ResourceEncapsulation_client_RCSRemoteResourceObject_nativeGetInterFaces
(JNIEnv *env, jobject interfaceClass)
{

    LOGD("RCSRemoteResourceObject_nativeGetInterFaces");
    JniRCSRemoteResourceObject *resource = JniRCSRemoteResourceObject::getJniRCSRemoteResourceObject(
            env, interfaceClass);
    if (!resource) return nullptr;
    return resource->getInterfaces(env);
}


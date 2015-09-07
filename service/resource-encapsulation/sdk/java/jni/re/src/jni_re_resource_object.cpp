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

#include "jni_re_resource_object.h"
#include "jni_re_get_request_listener.h"
#include "RCSResourceObject.h"

JniRCSResourceObject::JniRCSResourceObject(std::shared_ptr< RCSResourceObject>
        resource)
    : m_sharedResource(resource)
{}

JniRCSResourceObject::~JniRCSResourceObject()
{
    LOGD("JniResourceObject::~JniResourceObject()");
    m_sharedResource = NULL;

    jint envRet;
    JNIEnv *env = GetREJNIEnv(envRet);
    if (NULL == env) return;

    if (JNI_EDETACHED == envRet) g_jvm->DetachCurrentThread();
}

JniRCSResourceObject *JniRCSResourceObject::getJniRCSResourceObject(JNIEnv *env,
        jobject thiz)
{
    JniRCSResourceObject *resource = REGetHandle<JniRCSResourceObject>(env, thiz);
    if (env->ExceptionCheck())
    {
        LOGE("Failed to get native handle from RCSResourceObject");
    }
    if (!resource)
    {
        LOGE("getJniRCSResourceObject :: no resource");
    }
    return resource;
}

void JniRCSResourceObject::setAttributeInteger(JNIEnv *env, jobject jObject, jstring jKey,
        jint jValue)
{
    LOGD("JniRCSResourceObject::setAttributeInteger");

    if (!jKey)
    {
        throwRCSException(env, "key cannot be null");
        return;
    }
    std::string key = env->GetStringUTFChars(jKey, NULL);
    RCSResourceAttributes::Value value ;
    RCSResourceAttributes::TypeId typeId;

    try
    {
        value =  m_sharedResource->getAttributeValue(key);
        if (value != NULL)
        {
            typeId = value.getType().getId();
            if (typeId == RCSResourceAttributes::TypeId::INT)
            {
                RCSResourceObject::LockGuard lock(*m_sharedResource);
                m_sharedResource->getAttributes()[key] = (int)jValue;
            }
            else
            {
                LOGD("JniRCSResourceObject::setAttributeInteger value is not interger type");
            }
        }
    }
    catch (BadRequestException exception)
    {
        throwRCSException(env,  exception.what());
    }
}

void JniRCSResourceObject::setAttributeDouble(JNIEnv *env, jobject jObject, jstring jKey,
        jdouble jValue)
{
    LOGD("JniRCSResourceObject::setAttributeDouble");

    if (!jKey)
    {
        throwRCSException(env, "key cannot be null");
        return;
    }
    std::string key = env->GetStringUTFChars(jKey, NULL);
    RCSResourceAttributes::Value value ;
    RCSResourceAttributes::TypeId typeId;

    try
    {
        value =  m_sharedResource->getAttributeValue(key);
        if (value != NULL)
        {
            typeId = value.getType().getId();
            if (typeId == RCSResourceAttributes::TypeId::DOUBLE)
            {
                RCSResourceObject::LockGuard lock(*m_sharedResource);
                m_sharedResource->getAttributes()[key] = (double)jValue;
            }
            else
            {
                LOGD("JniRCSResourceObject::setAttributeInteger value is not double type");
            }
        }
    }
    catch (BadRequestException exception)
    {
        throwRCSException(env,  exception.what());
    }

}


void JniRCSResourceObject::setAttributeBool(JNIEnv *env, jobject jObject, jstring jKey,
        jboolean jValue)
{
    LOGD("JniRCSResourceObject::setAttributeBool");

    if (!jKey)
    {
        throwRCSException(env, "key cannot be null");
        return;
    }
    std::string key = env->GetStringUTFChars(jKey, NULL);
    RCSResourceAttributes::Value value ;
    RCSResourceAttributes::TypeId typeId;

    try
    {
        value =  m_sharedResource->getAttributeValue(key);
        if (value != NULL)
        {
            typeId = value.getType().getId();
            if (typeId == RCSResourceAttributes::TypeId::BOOL)
            {
                RCSResourceObject::LockGuard lock(*m_sharedResource);
                m_sharedResource->getAttributes()[key] = (bool)jValue;
            }
            else
            {
                LOGD("JniRCSResourceObject::setAttributeInteger value is not bool type");
            }
        }
    }
    catch (BadRequestException exception)
    {
        throwRCSException(env,  exception.what());
    }
}

void JniRCSResourceObject::setAttributeString(JNIEnv *env, jobject jObject, jstring jKey,
        jstring jValue)
{
    LOGD("JniRCSResourceObject::setAttributeString");

    if (!jKey)
    {
        throwRCSException(env, "key cannot be null");
        return;
    }
    std::string key = env->GetStringUTFChars(jKey, NULL);
    RCSResourceAttributes::Value value ;
    RCSResourceAttributes::TypeId typeId;

    try
    {
        value =  m_sharedResource->getAttributeValue(key);
        if (value != NULL)
        {
            typeId = value.getType().getId();
            if (typeId == RCSResourceAttributes::TypeId::STRING)
            {
                RCSResourceObject::LockGuard lock(*m_sharedResource);
                std::string nativeValue = env->GetStringUTFChars(jValue, NULL);
                m_sharedResource->getAttributes()[key] = nativeValue;
            }
            else
            {
                LOGD("JniRCSResourceObject::setAttributeInteger value is not String type");
            }
        }
    }
    catch (BadRequestException exception)
    {
        throwRCSException(env,  exception.what());
    }
}

jstring JniRCSResourceObject::getAttributeValue(JNIEnv *env, jstring jKey)
{
    char buf[64];
    std::string key;
    if (jKey)
    {
        key = env->GetStringUTFChars(jKey, NULL);
    }
    else
    {
        LOGE("getAttributeValue Key is NULL");
        return NULL;
    }

    RCSResourceAttributes::Value value;
    RCSResourceAttributes::TypeId typeId;
    try
    {
        value =  m_sharedResource->getAttributeValue(key);
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

bool JniRCSResourceObject::removeAttribute(JNIEnv *env, jstring jKey)
{
    std::string key;
    if (jKey)
    {
        key = env->GetStringUTFChars(jKey, NULL);
    }
    return m_sharedResource->removeAttribute(key);
}
bool JniRCSResourceObject::containsAttribute(JNIEnv *env, jstring jKey)
{
    std::string key;
    if (jKey)
    {
        key = env->GetStringUTFChars(jKey, NULL);
    }
    return m_sharedResource->containsAttribute(key);
}

jobject JniRCSResourceObject::getAttributes(JNIEnv *env, jobject jObject)
{
    RCSResourceObject::LockGuard lock(*m_sharedResource);
    RCSResourceAttributes nativeAttributes = m_sharedResource->getAttributes();

    RCSResourceAttributes *attributesObj = new RCSResourceAttributes(nativeAttributes);

    jlong handle = reinterpret_cast<jlong>(attributesObj);
    jobject jAttributes = env->NewObject(g_cls_RCSRemoteResourceAttributesObject,
                                         g_mid_RCSRemoteResourceAttributesObject_ctor,
                                         handle, true);
    return jAttributes;
}

bool JniRCSResourceObject::isObservable()
{
    return m_sharedResource->isObservable();
}

bool JniRCSResourceObject::isDiscoverable()
{
    return m_sharedResource->isDiscoverable();
}

void JniRCSResourceObject::setGetRequestHandler(JNIEnv *env, jobject jGetRequestListener)
{

    JniGetRequestListener *onGetRequestListener = addOnGetRequestListener(env, jGetRequestListener);

    OIC::Service::RCSResourceObject::GetRequestHandler getRequestCallback = [onGetRequestListener](
                OIC::Service::RCSRequest request,
                OIC::Service::RCSResourceAttributes attributes)
    {
        return onGetRequestListener->onGetRequestCallback(request, attributes);
    };
    m_sharedResource->setGetRequestHandler(getRequestCallback);
}

void JniRCSResourceObject::setSetRequestHandler(JNIEnv *env, jobject jSetRequestListener)
{

    JniSetRequestListener *onSetRequestListener = addOnSetRequestListener(env, jSetRequestListener);

    OIC::Service::RCSResourceObject::SetRequestHandler setRequestCallback = [onSetRequestListener](
                OIC::Service::RCSRequest request,
                OIC::Service::RCSResourceAttributes attributes)
    {
        return onSetRequestListener->onSetRequestCallback(request, attributes);
    };
    m_sharedResource->setSetRequestHandler(setRequestCallback);
}

void JniRCSResourceObject::addAttributeUpdatedListener(JNIEnv *env, jstring jKey,
        jobject jAttributeUpdatedListener)
{
    std::string nativeKey;
    if (jKey)
    {
        nativeKey = env->GetStringUTFChars(jKey, NULL);
    }
    else
    {
        LOGE("addAttributeUpdatedListener is NULL");
        return;
    }
    JniAttributeUpdatedListener *onAttributeUpdatedListener = addOnAttributeUpdatedListener(env,
            jAttributeUpdatedListener);

    OIC::Service::RCSResourceObject::AttributeUpdatedListener attributeUpdatedCallback
        = [onAttributeUpdatedListener]( OIC::Service::RCSResourceAttributes::Value oldValue,
                                        OIC::Service::RCSResourceAttributes::Value newValue)
    {
        onAttributeUpdatedListener->onAttributeUpdatedCallback(oldValue, newValue);
    };
    m_sharedResource->addAttributeUpdatedListener(nativeKey, attributeUpdatedCallback);
}

bool JniRCSResourceObject::removeAttributeUpdatedListener(JNIEnv *env, jobject jObject,
        jstring jKey)
{
    std::string nativeKey;
    if (jKey)
    {
        nativeKey = env->GetStringUTFChars(jKey, NULL);
    }
    return m_sharedResource->removeAttributeUpdatedListener(nativeKey);
}

void JniRCSResourceObject::notify()
{
    m_sharedResource->notify();
}

void JniRCSResourceObject::setAutoNotifyPolicy(int policy)
{
    RCSResourceObject::AutoNotifyPolicy nativePolicy = convertJavaNotifyPolicyToNativePolicy(policy);
    m_sharedResource->setAutoNotifyPolicy(nativePolicy);
}

int JniRCSResourceObject::getAutoNotifyPolicy()
{
    return convertAutoNotifyPolicyToInt(m_sharedResource->getAutoNotifyPolicy());
}

void JniRCSResourceObject::setSetRequestHandlerPolicy(int policy)
{
    RCSResourceObject::SetRequestHandlerPolicy nativePolicy =
        convertJavasetRequestHandlerPolicyToNativePolicy(policy);
    m_sharedResource->setSetRequestHandlerPolicy(nativePolicy);
}

int JniRCSResourceObject::getSetRequestHandlerPolicy()
{
    return convertSetRequestHandlerPolicyToInt(m_sharedResource->getSetRequestHandlerPolicy());
}

JniGetRequestListener *JniRCSResourceObject::addOnGetRequestListener(JNIEnv *env,
        jobject jListener)
{
    return this->m_onGetRequest.addListener(env, jListener, this);
}

JniSetRequestListener *JniRCSResourceObject::addOnSetRequestListener(JNIEnv *env,
        jobject jListener)
{
    return this->m_onSetRequest.addListener(env, jListener, this);
}

JniAttributeUpdatedListener *JniRCSResourceObject::addOnAttributeUpdatedListener(JNIEnv *env,
        jobject jListener)
{
    return this->m_onAttributeUpdated.addListener(env, jListener, this);
}

void JniRCSResourceObject::removeOnGetRequestListener(JNIEnv *env, jobject jListener)
{
    this->m_onGetRequest.removeListener(env, jListener);
}

void JniRCSResourceObject::removeOnSetRequestListener(JNIEnv *env, jobject jListener)
{
    this->m_onSetRequest.removeListener(env, jListener);
}

void JniRCSResourceObject::removeOnAttributeUpdatedListener(JNIEnv *env, jobject jListener)
{
    this->m_onAttributeUpdated.removeListener(env, jListener);
}


JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceObject_nativeSetAttributeInteger
(JNIEnv *env, jobject jObject, jstring key, jint value)
{
    LOGD("RCSRemoteResourceObject_nativeSetAttributeInteger");
    JniRCSResourceObject *resource = JniRCSResourceObject::getJniRCSResourceObject(env, jObject);
    resource->setAttributeInteger(env, jObject, key, value);
}

JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceObject_nativeSetAttributeDouble
(JNIEnv *env, jobject jObject, jstring key, jdouble value)
{
    LOGD("RCSRemoteResourceObject_nativeSetAttributeFloat");
    JniRCSResourceObject *resource = JniRCSResourceObject::getJniRCSResourceObject(env, jObject);
    resource->setAttributeDouble(env, jObject, key, value);
}

JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceObject_nativeSetAttributeBool
(JNIEnv *env, jobject jObject, jstring key, jboolean value)
{
    LOGD("RCSRemoteResourceObject_nativeSetAttributeDouble");
    JniRCSResourceObject *resource = JniRCSResourceObject::getJniRCSResourceObject(env, jObject);
    resource->setAttributeBool(env, jObject, key, value);
}

JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceObject_nativeSetAttributeString
(JNIEnv *env, jobject jObject, jstring key, jstring value)
{
    LOGD("RCSRemoteResourceObject_nativeSetAttributeFloat");
    JniRCSResourceObject *resource = JniRCSResourceObject::getJniRCSResourceObject(env, jObject);
    resource->setAttributeString(env, jObject, key, value);
}

JNIEXPORT jstring JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceObject_nativeGetAttributeValue
(JNIEnv *env, jobject jObject, jstring key)
{
    LOGD("RCSRemoteResourceObject_nativeGetAttributeValue");
    JniRCSResourceObject *resource = JniRCSResourceObject::getJniRCSResourceObject(env, jObject);
    return resource->getAttributeValue(env, key);
}

JNIEXPORT jboolean JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceObject_nativeRemoveAttribute
(JNIEnv *env, jobject jObject, jstring key)
{
    LOGD("RCSRemoteResourceObject_nativeRemoveAttribute");
    JniRCSResourceObject *resource = JniRCSResourceObject::getJniRCSResourceObject(env, jObject);
    return (jboolean)resource->removeAttribute(env, key);
}

JNIEXPORT jboolean JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceObject_nativeContainsAttribute
(JNIEnv *env, jobject jObject, jstring key)
{
    LOGD("RCSRemoteResourceObject_nativeContainsAttribute");
    JniRCSResourceObject *resource = JniRCSResourceObject::getJniRCSResourceObject(env, jObject);
    return (jboolean)resource->containsAttribute(env, key);
}

JNIEXPORT jobject JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceObject_nativeGetAttributes
(JNIEnv *env, jobject jObject)
{
    LOGD("RCSRemoteResourceObject_nativeGetAttributes");
    JniRCSResourceObject *resource = JniRCSResourceObject::getJniRCSResourceObject(env, jObject);
    return resource->getAttributes(env, jObject);
}

JNIEXPORT jboolean JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceObject_nativeIsObservable
(JNIEnv *env, jobject jObject)
{
    LOGD("RCSRemoteResourceObject_nativeIsObservable");
    JniRCSResourceObject *resource = JniRCSResourceObject::getJniRCSResourceObject(env, jObject);
    return (jboolean)resource->isObservable();
}

JNIEXPORT jboolean JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceObject_nativeIsDiscoverable
(JNIEnv *env, jobject jObject)
{
    LOGD("RCSRemoteResourceObject_nativeIsDiscoverable");
    JniRCSResourceObject *resource = JniRCSResourceObject::getJniRCSResourceObject(env, jObject);
    return (jboolean)resource->isDiscoverable();
}

JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceObject_nativeSetGetRequestHandler
(JNIEnv *env, jobject jObject , jobject getRequestListener)
{
    LOGD("RCSRemoteResourceObject_nativeSetGetRequestHandler");
    JniRCSResourceObject *resource = JniRCSResourceObject::getJniRCSResourceObject(env, jObject);
    resource->setGetRequestHandler(env, getRequestListener);
}

JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceObject_nativeSetSetRequestHandler
(JNIEnv *env, jobject jObject , jobject setRequestListener)
{
    LOGD("RCSRemoteResourceObject_nativeSetSetRequestHandler");
    JniRCSResourceObject *resource = JniRCSResourceObject::getJniRCSResourceObject(env, jObject);
    resource->setSetRequestHandler(env, setRequestListener);
}

JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceObject_nativeAddAttributeUpdatedListener
(JNIEnv *env, jobject jObject, jstring key, jobject attributeUpdatedListener)
{
    LOGD("RCSRemoteResourceObject_nativeAddAttributeUpdatedListener");
    JniRCSResourceObject *resource = JniRCSResourceObject::getJniRCSResourceObject(env, jObject);
    resource->addAttributeUpdatedListener(env, key, attributeUpdatedListener);
}

JNIEXPORT jboolean JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceObject_nativeRemoveAttributeUpdatedListener
(JNIEnv *env, jobject jObject, jstring key)
{
    LOGD("RCSRemoteResourceObject_nativeAddAttributeUpdatedListener");
    JniRCSResourceObject *resource = JniRCSResourceObject::getJniRCSResourceObject(env, jObject);
    return (jboolean)resource->removeAttributeUpdatedListener(env, jObject, key);
}

JNIEXPORT void JNICALL Java_org_iotivity_ResourceEncapsulation_server_RCSResourceObject_nativeNotify
(JNIEnv *env, jobject jObject)
{
    LOGD("RCSRemoteResourceObject_nativeNotify");
    JniRCSResourceObject *resource = JniRCSResourceObject::getJniRCSResourceObject(env, jObject);
    resource->notify();
}

JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceObject_nativeSetAutoNotifyPolicy
(JNIEnv *env, jobject jObject, jint jPolicy)
{
    LOGD("RCSRemoteResourceObject_nativeSetAutoNotifyPolicy");
    JniRCSResourceObject *resource = JniRCSResourceObject::getJniRCSResourceObject(env, jObject);
    resource->setAutoNotifyPolicy(jPolicy);
}

JNIEXPORT jint JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceObject_nativeGetAutoNotifyPolicy
(JNIEnv *env, jobject jObject)
{
    LOGD("RCSRemoteResourceObject_nativeGetAutoNotifyPolicy");
    JniRCSResourceObject *resource = JniRCSResourceObject::getJniRCSResourceObject(env, jObject);
    return (jint)resource->getAutoNotifyPolicy();
}

JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceObject_nativeSetSetRequestHandlerPolicy
(JNIEnv *env, jobject jObject, jint jPolicy)
{
    LOGD("RCSRemoteResourceObject_nativeSetSetRequestHandlerPolicy");
    JniRCSResourceObject *resource = JniRCSResourceObject::getJniRCSResourceObject(env, jObject);
    resource->setSetRequestHandlerPolicy(jPolicy);
}

JNIEXPORT jint JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceObject_nativeGetSetRequestHandlerPolicy
(JNIEnv *env, jobject jObject)
{
    LOGD("RCSRemoteResourceObject_nativeGetSetRequestHandlerPolicy");
    JniRCSResourceObject *resource = JniRCSResourceObject::getJniRCSResourceObject(env, jObject);
    return (jint)resource->getSetRequestHandlerPolicy();
}



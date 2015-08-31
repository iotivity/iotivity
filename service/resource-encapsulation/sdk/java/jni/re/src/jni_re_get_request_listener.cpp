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

#include "jni_re_get_request_listener.h"
#include "jni_re_resource_object.h"

JniGetRequestListener::JniGetRequestListener(JNIEnv *env, jobject jListener,
        JniRCSResourceObject *owner)
    : m_ownerResource(owner)
{
    m_jwListener = env->NewWeakGlobalRef(jListener);
}

JniGetRequestListener::~JniGetRequestListener()
{
    LOGI("JniGetRequestListener::~JniGetRequestListener()");
    if (m_jwListener)
    {
        jint ret;
        JNIEnv *env = GetREJNIEnv(ret);
        if (NULL == env) return;
        env->DeleteWeakGlobalRef(m_jwListener);
        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
    }
}

OIC::Service::RCSGetResponse JniGetRequestListener::onGetRequestCallback(
    OIC::Service::RCSRequest request, OIC::Service::RCSResourceAttributes attributes)
{
    LOGI("JniGetRequestListener::onGetRequestCallback enter");

    jint ret;
    JNIEnv *env = GetREJNIEnv(ret);
    if (NULL == env)
        LOGE("env is NULL");

    jobject jListener = env->NewLocalRef(m_jwListener);
    if (!jListener)
    {
        checkExAndRemoveListener(env);
        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
        LOGE("jListener is NULL");
    }

    jclass clsL = env->GetObjectClass(jListener);

    if (!clsL)
    {
        checkExAndRemoveListener(env);
        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
        LOGE("clsL is NULL");
    }

    //constructing java attribute object
    RCSResourceAttributes *attributesObj = new RCSResourceAttributes(attributes);
    jlong handle = reinterpret_cast<jlong>(attributesObj);
    jobject jAttributes = env->NewObject(g_cls_RCSRemoteResourceAttributesObject,
                                         g_mid_RCSRemoteResourceAttributesObject_ctor,
                                         handle, true);
    if (!jAttributes)
    {
        delete attributesObj;
        checkExAndRemoveListener(env);
        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
        LOGI("onGetRequestCallback: jAttributes == null");
    }

    //constructing java RCSRequest object
    std::string resourceUri = request.getResourceUri();
    jstring javaUri = env->NewStringUTF(resourceUri.c_str());

    g_mid_RCSRequest_ctor = env->GetMethodID(g_cls_RCSRequest, "<init>", "(Ljava/lang/String;)V");
    jobject javaRCSRequestObject = env->NewObject( g_cls_RCSRequest, g_mid_RCSRequest_ctor, javaUri);

    //getting the method ID

    jmethodID midL = env->GetMethodID(clsL, "onGetRequest",
  "(Lorg/iotivity/ResourceEncapsulation/server/RCSRequest;Lorg/iotivity/ResourceEncapsulation/common/RCSResourceAttributes;)Lorg/iotivity/ResourceEncapsulation/server/RCSGetResponse;");

    if (!midL)
    {
        delete attributesObj;
        checkExAndRemoveListener(env);
        if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
        LOGE(" midL is null");
    }

    //calling the Java method
    jobject javaGetResponse = env->CallObjectMethod(jListener, midL, javaRCSRequestObject, jAttributes);

    //getting the information related to Response
    jclass GetResponseClass = env->GetObjectClass(javaGetResponse);

    jmethodID midGetTypeOfGetResponse = env->GetMethodID(GetResponseClass, "getTypeOfGetResponse",
                                        "()I");
    jint type = env->CallIntMethod(javaGetResponse, midGetTypeOfGetResponse);

    if (0 == type)
    {
        return OIC::Service::RCSGetResponse::defaultAction();
    }
    else if (1 == type)
    {
        LOGI("Type == 1");
        jmethodID midGetResult = env->GetMethodID(GetResponseClass, "getResult", "()I");
        jmethodID midGetErrorCode = env->GetMethodID(GetResponseClass, "getErrorCode", "()I");
        jint handlerResult = env->CallIntMethod(javaGetResponse, midGetResult);
        jint errorCode = env->CallIntMethod(javaGetResponse, midGetErrorCode);
        OCEntityHandlerResult nativeHandlerResult = convertIntToOCEntityHandlerResult(handlerResult);
        return OIC::Service::RCSGetResponse::create(nativeHandlerResult, errorCode);
    }
    else if (2 == type)
    {
        LOGI("Type == 2");
        jmethodID midGetAttributes = env->GetMethodID(GetResponseClass, "getAttributes",
                                     "()Lorg/iotivity/ResourceEncapsulation/common/RCSResourceAttributes;");
        jobject attributes = env->CallObjectMethod(javaGetResponse, midGetAttributes);
        RCSResourceAttributes *attributesObj =   JniResourceAttributes::getResourceAttributePtr(env,
                attributes);
        return OIC::Service::RCSGetResponse::create(*attributesObj);
    }
    else if (3 == type)
    {
        LOGI("Type == 3");
        jmethodID midGetResult = env->GetMethodID(GetResponseClass, "getResult", "()I");
        jmethodID midGetErrorCode = env->GetMethodID(GetResponseClass, "getErrorCode", "()I");
        jint handlerResult = env->CallIntMethod(javaGetResponse, midGetResult);
        jint errorCode = env->CallIntMethod(javaGetResponse, midGetErrorCode);
        OCEntityHandlerResult nativeHandlerResult = convertIntToOCEntityHandlerResult(handlerResult);
        jmethodID midGetAttributes = env->GetMethodID(GetResponseClass, "getAttributes",
                                     "()Lorg/iotivity/ResourceEncapsulation/common/RCSResourceAttributes;");
        jobject attributes = env->CallObjectMethod(javaGetResponse, midGetAttributes);
        RCSResourceAttributes *attributesObj =   JniResourceAttributes::getResourceAttributePtr(env,
                attributes);
        return OIC::Service::RCSGetResponse::create(*attributesObj, nativeHandlerResult, errorCode);
    }
    else
    {
        LOGI("defaultAction");
        return OIC::Service::RCSGetResponse::defaultAction();
    }

    if (env->ExceptionCheck())
    {
        LOGE("Java exception is thrown");
        delete attributesObj;
    }

    checkExAndRemoveListener(env);
    if (JNI_EDETACHED == ret) g_jvm->DetachCurrentThread();
    LOGI("JniGetRequestListener::onGetRequestCallback exit");
}

void JniGetRequestListener::checkExAndRemoveListener(JNIEnv *env)
{
    if (env->ExceptionCheck())
    {
        jthrowable ex = env->ExceptionOccurred();
        env->ExceptionClear();
        m_ownerResource->removeOnGetRequestListener(env, m_jwListener);
        env->Throw((jthrowable)ex);
    }
    else
    {
        m_ownerResource->removeOnGetRequestListener(env, m_jwListener);
    }
}

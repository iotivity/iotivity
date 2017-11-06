/* ****************************************************************
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
#include "JniOcRDClient.h"
#include "JniUtils.h"
#include "JniOcResourceHandle.h"

#include "RDClient.h"

void RemoveOnPublishResourceListener(JNIEnv* env, jobject jListener)
{
    if (!env)
    {
        LOGE("env is null");
        return;
    }

    publishResourceListenerMapLock.lock();
    bool isFound = false;
    for (auto it = onPublishResourceListenerMap.begin(); it != onPublishResourceListenerMap.end(); ++it)
    {
        if (env->IsSameObject(jListener, it->first))
        {
            auto refPair = it->second;
            if (refPair.second > 1)
            {
                refPair.second--;
                it->second = refPair;
                onPublishResourceListenerMap.insert(*it);
                LOGI("onPublishResourceListener: ref. count decremented");
            }
            else
            {
                env->DeleteGlobalRef(it->first);
                JniOnPublishResourceListener* listener = refPair.first;
                delete listener;
                onPublishResourceListenerMap.erase(it);
                LOGI("onPublishResourceListener is removed");
            }
            isFound = true;
            break;
        }
    }
    if (!isFound)
    {
        ThrowOcException(JNI_EXCEPTION, "onPublishResourceListener not found");
    }
    publishResourceListenerMapLock.unlock();
}

JniOnPublishResourceListener* AddOnPublishResourceListener(JNIEnv* env, jobject jListener)
{
    if (!env)
    {
        LOGD("env is null");
        return nullptr;
    }

    JniOnPublishResourceListener *onPublishResourceListener = nullptr;

    publishResourceListenerMapLock.lock();

    for (auto it = onPublishResourceListenerMap.begin(); it !=
            onPublishResourceListenerMap.end(); ++it)
    {
        if (env->IsSameObject(jListener, it->first))
        {
            auto refPair = it->second;
            onPublishResourceListener = refPair.first;
            refPair.second++;
            it->second = refPair;
            onPublishResourceListenerMap.insert(*it);
            LOGD("onPublishResourceListener: ref. count incremented");
            break;
        }
    }
    if (!onPublishResourceListener)
    {
        onPublishResourceListener = new JniOnPublishResourceListener(env, jListener,
                RemoveOnPublishResourceListener);

        jobject jgListener = env->NewGlobalRef(jListener);
        onPublishResourceListenerMap.insert(
                std::pair<jobject, std::pair<JniOnPublishResourceListener*, int>>(
                    jgListener,
                    std::pair<JniOnPublishResourceListener*, int>(onPublishResourceListener, 1)));
        LOGI("onPublishResourceListener: new listener");
    }
    publishResourceListenerMapLock.unlock();
    return onPublishResourceListener;
}

void RemoveOnDeleteResourceListener(JNIEnv* env, jobject jListener)
{
    if (!env)
    {
        LOGE("env is null");
        return;
    }

    deleteResourceListenerMapLock.lock();
    bool isFound = false;
    for (auto it = onDeleteResourceListenerMap.begin(); it !=
            onDeleteResourceListenerMap.end(); ++it)
    {
        if (env->IsSameObject(jListener, it->first))
        {
            auto refPair = it->second;
            if (refPair.second > 1)
            {
                refPair.second--;
                it->second = refPair;
                onDeleteResourceListenerMap.insert(*it);
                LOGI("onDeleteResourceListener: ref. count decremented");
            }
            else
            {
                env->DeleteGlobalRef(it->first);
                JniOnDeleteResourceListener* listener = refPair.first;
                delete listener;
                onDeleteResourceListenerMap.erase(it);
                LOGI("onDeleteResourceListener is removed");
            }
            isFound = true;
            break;
        }
    }
    if (!isFound)
    {
        ThrowOcException(JNI_EXCEPTION, "onDeleteResourceListener not found");
    }
    deleteResourceListenerMapLock.unlock();
}

JniOnDeleteResourceListener* AddOnDeleteResourceListener(JNIEnv* env, jobject jListener)
{
    if (!env)
    {
        LOGD("env is null");
        return nullptr;
    }

    JniOnDeleteResourceListener *onDeleteResourceListener = nullptr;

    deleteResourceListenerMapLock.lock();

    for (auto it = onDeleteResourceListenerMap.begin(); it !=
            onDeleteResourceListenerMap.end(); ++it)
    {
        if (env->IsSameObject(jListener, it->first))
        {
            auto refPair = it->second;
            onDeleteResourceListener = refPair.first;
            refPair.second++;
            it->second = refPair;
            onDeleteResourceListenerMap.insert(*it);
            LOGD("onDeleteResourceListener: ref. count incremented");
            break;
        }
    }
    if (!onDeleteResourceListener)
    {
        onDeleteResourceListener = new JniOnDeleteResourceListener(env, jListener,
                RemoveOnDeleteResourceListener);
        jobject jgListener = env->NewGlobalRef(jListener);
        onDeleteResourceListenerMap.insert(
                std::pair<jobject, std::pair<JniOnDeleteResourceListener*, int>>(
                    jgListener,
                    std::pair<JniOnDeleteResourceListener*, int>(onDeleteResourceListener, 1)));
        LOGI("onDeleteResourceListener: new listener");
    }
    deleteResourceListenerMapLock.unlock();
    return onDeleteResourceListener;
}

/*
 * Class:     org_iotivity_base_OcRDClient
 * Method:    publishResourceToRD0
 * Signature: (Ljava/lang/String;ILorg/iotivity/base/OcRDClient/OnPublishResourceListener;I)V
 */
JNIEXPORT void JNICALL Java_org_iotivity_base_OcRDClient_publishResourceToRD0(
        JNIEnv *env,
        jclass clazz,
        jstring jHost,
        jint jConnectivityType,
        jobject jListener,
        jint jQoS)
{
    OC_UNUSED(clazz);
    LOGD("OcRDClient_publishResourceToRD");
#ifdef RD_CLIENT
    std::string host;
    if (jHost)
    {
        host = env->GetStringUTFChars(jHost, nullptr);
    }
    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "onPublishResourceListener cannot be null");
        return;
    }
    JniOnPublishResourceListener *onPubResListener = AddOnPublishResourceListener(env, jListener);

    PublishResourceCallback pubResCallback = [onPubResListener](
            const OCRepresentation& ocRepresentation,
            const int eCode)
    {
        onPubResListener->onPublishResourceCallback(ocRepresentation, eCode);
    };

    try
    {
        OCStackResult result = RDClient::Instance().publishResourceToRD(
            host,
            static_cast<OCConnectivityType>(jConnectivityType),
            pubResCallback,
            JniUtils::getQOS(env, static_cast<int>(jQoS)));

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Publish resource has failed");
            return;
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
#else
    OC_UNUSED(env);
    OC_UNUSED(clazz);
    OC_UNUSED(jHost)
    OC_UNUSED(jConnectivityType);
    OC_UNUSED(jListener);
    OC_UNUSED(jQoS);
    ThrowOcException(JNI_NO_SUPPORT, "Not supported");
    return;
#endif
}

/*
 * Class:     org_iotivity_base_OcRDClient
 * Method:    publishResourceToRD1
 * Signature: (Ljava/lang/String;I[Lorg/iotivity/base/OcResourceHandle;Lorg/iotivity/base/OcRDClient/OnPublishResourceListener;I)V
 */
JNIEXPORT void JNICALL Java_org_iotivity_base_OcRDClient_publishResourceToRD1(
        JNIEnv *env,
        jclass clazz,
        jstring jHost,
        jint jConnectivityType,
        jobjectArray jResourceHandleArray,
        jobject jListener,
        jint jQoS)
{
    OC_UNUSED(clazz);
    LOGD("OcRDClient_publishResourceToRD");
#ifdef RD_CLIENT
    if (!env)
    {
        LOGE("env is null");
        return;
    }
    std::string host;
    if (jHost)
    {
        host = env->GetStringUTFChars(jHost, nullptr);
    }
    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "onPublishResourceListener cannot be null");
        return;
    }
    if (!jResourceHandleArray)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "resourceHandleList cannot be null");
        return;
    }
    JniOnPublishResourceListener *onPubResListener = AddOnPublishResourceListener(env, jListener);

    PublishResourceCallback pubResCallback = [onPubResListener](
            const OCRepresentation& ocRepresentation,
            const int eCode)
    {
        onPubResListener->onPublishResourceCallback(ocRepresentation, eCode);
    };

    std::vector<OCResourceHandle> resourceHandleList;
    jsize len = env->GetArrayLength(jResourceHandleArray);
    for (jsize i = 0; i < len; ++i)
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
            ThrowOcException(OC_STACK_INVALID_PARAM, "resource handle is invalid");
            return;
        }

        resourceHandleList.push_back(jniOcResourceHandle->getOCResourceHandle());
    }

    try
    {
        OCStackResult result = RDClient::Instance().publishResourceToRD(
            host,
            static_cast<OCConnectivityType>(jConnectivityType),
            resourceHandleList,
            pubResCallback,
            JniUtils::getQOS(env, static_cast<int>(jQoS)));

        if (OC_STACK_OK != result)
        {
            ThrowOcException(result, "Publish resource has failed");
            return;
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(e.code(), e.reason().c_str());
    }
#else
    OC_UNUSED(env);
    OC_UNUSED(jHost);
    OC_UNUSED(jConnectivityType);
    OC_UNUSED(jResourceHandleArray);
    OC_UNUSED(jListener);
    OC_UNUSED(jQoS);
    ThrowOcException(JNI_NO_SUPPORT, "Not supported");
    return;
#endif
}

/*
 * Class:     org_iotivity_base_OcRDClient
 * Method:    deleteResourceFromRD0
 * Signature: (Ljava/lang/String;ILorg/iotivity/base/OcRDClient/OnDeleteResourceListener;I)V
 */
JNIEXPORT void JNICALL Java_org_iotivity_base_OcRDClient_deleteResourceFromRD0(
        JNIEnv *env,
        jclass clazz,
        jstring jHost,
        jint jConnectivityType,
        jobject jListener,
        jint jQoS)
{
    OC_UNUSED(clazz);
    LOGD("OcRDClient_deleteResourceFromRD");
#ifdef RD_CLIENT
    std::string host;
    if (jHost)
    {
        host = env->GetStringUTFChars(jHost, nullptr);
    }
    if (!jListener)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "onDeleteResourceListener cannot be null");
        return;
    }
    JniOnDeleteResourceListener *onDelResListener = AddOnDeleteResourceListener(env, jListener);

    DeleteResourceCallback delResCallback = [onDelResListener](const int eCode)
    {
        onDelResListener->onDeleteResourceCallback(eCode);
    };
    try
     {
         OCStackResult result = RDClient::Instance().deleteResourceFromRD(
             host,
             static_cast<OCConnectivityType>(jConnectivityType),
             delResCallback,
             JniUtils::getQOS(env, static_cast<int>(jQoS)));

         if (OC_STACK_OK != result)
         {
             ThrowOcException(result, "Delete resource has failed");
             return;
         }
     }
     catch (OCException& e)
     {
         LOGE("%s", e.reason().c_str());
         ThrowOcException(e.code(), e.reason().c_str());
     }
 #else
     OC_UNUSED(env);
     OC_UNUSED(clazz);
     OC_UNUSED(jHost);
     OC_UNUSED(jConnectivityType);
     OC_UNUSED(jListener);
     OC_UNUSED(jQoS);
     ThrowOcException(JNI_NO_SUPPORT, "Not supported");
     return;
 #endif
 }

 /*
  * Class:     org_iotivity_base_OcRDClient
  * Method:    deleteResourceFromRD1
  * Signature: (Ljava/lang/String;I[Lorg/iotivity/base/OcResourceHandle;Lorg/iotivity/base/OcRDClient/OnDeleteResourceListener;I)V
  */
 JNIEXPORT void JNICALL Java_org_iotivity_base_OcRDClient_deleteResourceFromRD1(
         JNIEnv *env,
         jclass clazz,
         jstring jHost,
         jint jConnectivityType,
         jobjectArray jResourceHandleArray,
         jobject jListener,
         jint jQoS)
 {
     OC_UNUSED(clazz);
     LOGD("OcRDClient_deleteResourceFromRD");
 #ifdef RD_CLIENT
     if (!env)
     {
         LOGE("env is null");
         return;
     }
     std::string host;
     if (jHost)
     {
         host = env->GetStringUTFChars(jHost, nullptr);
     }
     if (!jListener)
     {
         ThrowOcException(OC_STACK_INVALID_PARAM, "onDeleteResourceListener cannot be null");
         return;
     }
     if (!jResourceHandleArray)
     {
         ThrowOcException(OC_STACK_INVALID_PARAM, "resourceHandleList cannot be null");
         return;
     }
     JniOnDeleteResourceListener *onDelResListener = AddOnDeleteResourceListener(env, jListener);

     DeleteResourceCallback delResCallback = [onDelResListener](const int eCode)
     {
         onDelResListener->onDeleteResourceCallback(eCode);
     };

     std::vector<OCResourceHandle> resourceHandleList;
     jsize len = env->GetArrayLength(jResourceHandleArray);
     for (jsize i = 0; i < len; ++i)
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
             ThrowOcException(OC_STACK_INVALID_PARAM, "resource handle is invalid");
             return;
         }

         resourceHandleList.push_back(jniOcResourceHandle->getOCResourceHandle());
     }

     try
     {
         OCStackResult result = RDClient::Instance().deleteResourceFromRD(
             host,
             static_cast<OCConnectivityType>(jConnectivityType),
             resourceHandleList,
             delResCallback,
             JniUtils::getQOS(env, static_cast<int>(jQoS)));

         if (OC_STACK_OK != result)
         {
             ThrowOcException(result, "Delete resource has failed");
             return;
         }
     }
     catch (OCException& e)
     {
         LOGE("%s", e.reason().c_str());
         ThrowOcException(e.code(), e.reason().c_str());
     }
 #else
     OC_UNUSED(env);
     OC_UNUSED(jHos);
     OC_UNUSED(jConnectivityType);
     OC_UNUSED(jResourceHandleArray);
     OC_UNUSED(jListener);
     OC_UNUSED(jQoS);
     ThrowOcException(JNI_NO_SUPPORT, "Not supported");
     return;
 #endif
 }

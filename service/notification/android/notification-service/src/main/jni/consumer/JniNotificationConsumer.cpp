//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
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

#include "JniNotificationConsumer.h"
#include "NSConsumerService.h"

static JavaVM *g_jvm = NULL;

static jobject g_obj_postListener = NULL;
static jobject g_obj_discoverListener = NULL;
static jobject g_obj_syncListener = NULL;
static jobject g_obj_acceptListener = NULL;

jclass g_cls_Message;
jclass g_cls_Provider;
jclass g_cls_SyncInfo;
jclass g_cls_MediaContents;

static JNIEnv *GetJNIEnv(jint *ret)
{
    JNIEnv *env = NULL;

    *ret = g_jvm->GetEnv((void **) &env, JNI_CURRENT_VERSION);
    switch (*ret)
    {
        case JNI_OK:
            return env;
        case JNI_EDETACHED:
            if (g_jvm->AttachCurrentThread(&env, NULL) != JNI_OK)
            {
                LOGE ("Failed to get the environment");
                return NULL;
            }
            else
            {
                return env;
            }
        case JNI_EVERSION:
            LOGE ("JNI version is not supported");
        default:
            LOGE ("Failed to get the environment");
            return NULL;
    }
}

void onDiscoverProvider(OIC::Service::NSProvider *provider)
{
    LOGD ("ConsumerService_onDiscoverProvider");

    jint envRet;
    JNIEnv *env = GetJNIEnv(&envRet);
    if (NULL == env) return ;

    jobject jDiscoverListener = (jobject) env->NewLocalRef(g_obj_discoverListener);
    if (!jDiscoverListener)
    {
        LOGE ("Failed to Get jDiscoverListener");
        if (JNI_EDETACHED == envRet) g_jvm->DetachCurrentThread();
        return ;
    }

    LOGD ("ProviderId : %s\n", provider->getProviderId().c_str());

    jstring jProviderId = env->NewStringUTF(provider->getProviderId().c_str());
    jlong pProvider = (long)provider;

    jclass cls_provider = (jclass) (env->NewLocalRef(g_cls_Provider));
    if (!cls_provider)
    {
        LOGE ("Failed to Get ObjectClass for Provider");
        if (JNI_EDETACHED == envRet) g_jvm->DetachCurrentThread();
        return ;
    }
    jmethodID mid_provider = env->GetMethodID(
                                 cls_provider, "<init>", "(Ljava/lang/String;)V");
    if (!mid_provider)
    {
        LOGE ("Failed to Get MethodID for Provider<init>");
        if (JNI_EDETACHED == envRet) g_jvm->DetachCurrentThread();
        return ;
    }
    jobject obj_provider = env->NewObject(cls_provider, mid_provider,
                                          jProviderId);

    jfieldID nativeHandle = env->GetFieldID(cls_provider, "mNativeHandle", "J");
    if (!nativeHandle)
    {
        LOGE("Failed to get nativeHandle for Provider");
        return;
    }
    env->SetLongField(obj_provider, nativeHandle, pProvider);

    jclass cls = env->GetObjectClass(jDiscoverListener);
    if (!cls)
    {
        LOGE ("Failed to Get ObjectClass for jDiscoverListener");
        if (JNI_EDETACHED == envRet) g_jvm->DetachCurrentThread();
        return ;
    }
    jmethodID mid = env->GetMethodID(
                        cls,
                        "onProviderDiscovered",
                        "(Lorg/iotivity/service/ns/consumer/Provider;)V");
    if (!mid)
    {
        LOGE ("Failed to Get MethodID for onProviderDiscovered");
        if (JNI_EDETACHED == envRet) g_jvm->DetachCurrentThread();
        return ;
    }

    env->CallVoidMethod(jDiscoverListener, mid, obj_provider);

    env->DeleteLocalRef(jDiscoverListener);
    env->DeleteLocalRef(cls_provider);
    if (JNI_EDETACHED == envRet) g_jvm->DetachCurrentThread();
    return ;
}

void onProviderChanged(OIC::Service::NSProvider *provider,OIC::Service::NSResponse response)
{
    LOGD ("ConsumerService_onSubscriptionAccepted");

    jint envRet;
    JNIEnv *env = GetJNIEnv(&envRet);
    if (NULL == env) return ;

    jobject jAcceptListener = (jobject) env->NewLocalRef(g_obj_acceptListener);
    if (!jAcceptListener)
    {
        LOGE ("Failed to Get jAcceptListener");
        if (JNI_EDETACHED == envRet) g_jvm->DetachCurrentThread();
        return ;
    }

    LOGD ("ProviderId : %s\n", provider->getProviderId().c_str());

    jstring jProviderId = env->NewStringUTF(provider->getProviderId().c_str());
    jlong pProvider = (long)provider;

    jclass cls_provider = (jclass) (env->NewLocalRef(g_cls_Provider));
    if (!cls_provider)
    {
        LOGE ("Failed to Get ObjectClass for Provider");
        if (JNI_EDETACHED == envRet) g_jvm->DetachCurrentThread();
        return ;
    }
    jmethodID mid_provider = env->GetMethodID(
                                 cls_provider, "<init>", "(Ljava/lang/String;)V");
    if (!mid_provider)
    {
        LOGE ("Failed to Get MethodID for Provider<init>");
        if (JNI_EDETACHED == envRet) g_jvm->DetachCurrentThread();
        return ;
    }
    jobject obj_provider = env->NewObject(cls_provider, mid_provider,
                                          jProviderId);

    jfieldID nativeHandle = env->GetFieldID(cls_provider, "mNativeHandle", "J");
    if (!nativeHandle)
    {
        LOGE("Failed to get nativeHandle for Provider");
        return;
    }
    env->SetLongField(obj_provider, nativeHandle, pProvider);

    jclass cls = env->GetObjectClass(jAcceptListener);
    if (!cls)
    {
        LOGE ("Failed to Get ObjectClass for jAcceptListener");
        if (JNI_EDETACHED == envRet) g_jvm->DetachCurrentThread();
        return ;
    }
    //TODO: Modify to call ProviderChanged Cb adding topic in Java
    jmethodID mid = env->GetMethodID(
                        cls,
                        "onSubscriptionAccepted",
                        "(Lorg/iotivity/service/ns/consumer/Provider;)V");
    if (!mid)
    {
        LOGE ("Failed to Get MethodID for onSubscriptionAccepted");
        if (JNI_EDETACHED == envRet) g_jvm->DetachCurrentThread();
        return ;
    }

    env->CallVoidMethod(jAcceptListener, mid, obj_provider);

    env->DeleteLocalRef(jAcceptListener);
    env->DeleteLocalRef(cls_provider);
    if (JNI_EDETACHED == envRet) g_jvm->DetachCurrentThread();
    return ;

}

jobject getJavaMessageType(JNIEnv *env, OIC::Service::NSMessage::NSMessageType type)
{
    switch (type)
    {
        case OIC::Service::NSMessage::NSMessageType::NS_MESSAGE_ALERT:
            {
                static jfieldID fieldID = env->GetStaticFieldID(g_cls_Message,
                                          "ALERT", "Lorg/iotivity/service/ns/common/Message$MessageType;");
                return env->GetStaticObjectField(g_cls_Message, fieldID);
            }
        case OIC::Service::NSMessage::NSMessageType::NS_MESSAGE_NOTICE:
            {
                static jfieldID fieldID = env->GetStaticFieldID(g_cls_Message,
                                          "NOTICE", "Lorg/iotivity/service/ns/common/Message$MessageType;");
                return env->GetStaticObjectField(g_cls_Message, fieldID);
            }
        case OIC::Service::NSMessage::NSMessageType::NS_MESSAGE_EVENT:
            {
                static jfieldID fieldID = env->GetStaticFieldID(g_cls_Message,
                                          "EVENT", "Lorg/iotivity/service/ns/common/Message$MessageType;");
                return env->GetStaticObjectField(g_cls_Message, fieldID);
            }
        case OIC::Service::NSMessage::NSMessageType::NS_MESSAGE_INFO:
            {
                static jfieldID fieldID = env->GetStaticFieldID(g_cls_Message,
                                          "INFO", "Lorg/iotivity/service/ns/common/Message$MessageType;");
                return env->GetStaticObjectField(g_cls_Message, fieldID);
            }
        default:
            return NULL;
    }
    return NULL;
}

void onMessagePosted(OIC::Service::NSMessage *message)
{
    LOGD ("ConsumerService_onMessagePosted");

    jint envRet;
    JNIEnv *env = GetJNIEnv(&envRet);
    if (NULL == env) return ;

    jobject jPostListener = (jobject) env->NewLocalRef(g_obj_postListener);
    if (!jPostListener)
    {
        LOGE ("Failed to Get jPostListener");
        if (JNI_EDETACHED == envRet) g_jvm->DetachCurrentThread();
        return ;
    }

    LOGD ("id : %llu\n", message->getMessageId());
    LOGD ("title : %s\n", message->getTitle().c_str());
    LOGD ("content : %s\n", message->getContentText().c_str());
    LOGD ("source : %s\n", message->getSourceName().c_str());

    jlong jMessageId = (jlong) message->getMessageId();
    jstring jProviderId = env->NewStringUTF(message->getProviderId().c_str());
    jstring jTitle = env->NewStringUTF(message->getTitle().c_str());
    jstring jContentText = env->NewStringUTF(message->getContentText().c_str());
    jstring jSourceName = env->NewStringUTF(message->getSourceName().c_str());

    jstring jTime = env->NewStringUTF(message->getTime().c_str());
    jlong jTTL = (jlong) message->getTTL();

    jlong pMessage = (long) message;

    jclass cls_message = (jclass) (env->NewLocalRef(g_cls_Message));
    if (!cls_message)
    {
        LOGE ("Failed to Get ObjectClass for Message");
        if (JNI_EDETACHED == envRet) g_jvm->DetachCurrentThread();
        return ;
    }
    jmethodID mid_message = env->GetMethodID(
                                cls_message, "<init>",
                                "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
    if (!mid_message)
    {
        LOGE ("Failed to Get MethodID for Message<init>");
        if (JNI_EDETACHED == envRet) g_jvm->DetachCurrentThread();
        return ;
    }
    jobject obj_message = env->NewObject(cls_message, mid_message,
                                         jTitle, jContentText, jSourceName);

    jfieldID fid_nativeHandle = env->GetFieldID(cls_message, "mNativeHandle", "J");
    if (!fid_nativeHandle)
    {
        LOGE("Failed to get nativeHandle for Message");
        return;
    }
    env->SetLongField(obj_message, fid_nativeHandle, pMessage);

    jfieldID fid_messageId = env->GetFieldID(cls_message, "mMessageId", "J");
    if (!fid_messageId)
    {
        LOGE("Failed to get field MessageID for Message");
        return;
    }
    env->SetLongField(obj_message, fid_messageId, jMessageId);

    jfieldID fid_providerId = env->GetFieldID(cls_message, "mProviderId", "Ljava/lang/String;");
    if (!fid_providerId)
    {
        LOGE("Failed to get field ProviderID for Message");
        return;
    }
    env->SetObjectField(obj_message, fid_providerId, jProviderId);

    jfieldID fid_time = env->GetFieldID(cls_message, "mTime", "Ljava/lang/String;");
    if (!fid_time)
    {
        LOGE("Failed to get field Time for Message");
        return;
    }
    env->SetObjectField(obj_message, fid_time, jTime);

    jfieldID fid_ttl = env->GetFieldID(cls_message, "mTTL", "J");
    if (!fid_ttl)
    {
        LOGE("Failed to get field TTL for Message");
        return;
    }
    env->SetLongField(obj_message, fid_ttl, jTTL);

    OIC::Service::NSMediaContents *mediaCont = message->getMediaContents();
    if (mediaCont != nullptr)
    {
        jstring jIconImage = env->NewStringUTF(mediaCont->getIconImage().c_str());
        jclass cls_mediaContents = (jclass) (env->NewLocalRef(g_cls_MediaContents));
        if (!cls_mediaContents)
        {
            LOGE ("Failed to Get ObjectClass for MediaContents");
            if (JNI_EDETACHED == envRet) g_jvm->DetachCurrentThread();
            return ;
        }
        jmethodID mid_mediaContents = env->GetMethodID(
                                          cls_mediaContents, "<init>", "(Ljava/lang/String;)V");
        if (!mid_mediaContents)
        {
            LOGE ("Failed to Get MethodID for MediaContents<init>");
            if (JNI_EDETACHED == envRet) g_jvm->DetachCurrentThread();
            return ;
        }
        jobject obj_mediaContents = env->NewObject(cls_mediaContents, mid_mediaContents,
                                    jIconImage);

        jfieldID fid_mediaContents = env->GetFieldID(cls_message, "mMediaContents",
                                     "Lorg/iotivity/service/ns/common/MediaContents;");
        if (!fid_mediaContents)
        {
            LOGE("Failed to get field mediaContents for Message");
            return;
        }
        env->SetObjectField(obj_message, fid_mediaContents, obj_mediaContents);

    }

    jobject jType = getJavaMessageType(env, message->getType());
    if (!jType)
    {
        LOGE("Failed to get Type value for Message");
        return;
    }
    jfieldID fid_type = env->GetFieldID(cls_message, "mType",
                                        "Lorg/iotivity/service/ns/common/Message$MessageType;");
    if (!fid_type)
    {
        LOGE("Failed to get field Type for Message");
        return;
    }
    env->SetObjectField(obj_message, fid_type, jType);


    jclass cls = env->GetObjectClass(jPostListener);
    if (!cls)
    {
        LOGE ("Failed to Get ObjectClass for jPostListener");
        if (JNI_EDETACHED == envRet) g_jvm->DetachCurrentThread();
        return ;
    }
    jmethodID mid = env->GetMethodID(
                        cls,
                        "onMessageReceived",
                        "(Lorg/iotivity/service/ns/common/Message;)V");
    if (!mid)
    {
        LOGE ("Failed to Get MethodID for onMessageReceived");
        if (JNI_EDETACHED == envRet) g_jvm->DetachCurrentThread();
        return ;
    }

    env->CallVoidMethod(jPostListener, mid, obj_message);

    env->DeleteLocalRef(jPostListener);
    env->DeleteLocalRef(cls_message);
    if (JNI_EDETACHED == envRet) g_jvm->DetachCurrentThread();
    return ;
}

void onSyncInfoReceived(OIC::Service::NSSyncInfo *sync)
{
    LOGD ("ConsumerService_onSyncInfoReceived");

    jint envRet;
    JNIEnv *env = GetJNIEnv(&envRet);
    if (NULL == env) return ;

    jobject jSyncListener = (jobject) env->NewLocalRef(g_obj_syncListener);
    if (!jSyncListener)
    {
        LOGE ("Failed to Get jSyncListener");
        if (JNI_EDETACHED == envRet) g_jvm->DetachCurrentThread();
        return ;
    }

    LOGD ("Sync ID : %llu\n", sync->getMessageId());
    LOGD ("Sync ProviderId : %s\n", sync->getProviderId().c_str());
    LOGD ("Sync STATE : %d\n", (int) sync->getState());

    jlong jMessageId = (jlong) sync->getMessageId();
    jstring jProviderId = env->NewStringUTF(sync->getProviderId().c_str());
    jint jState = (jint) sync->getState();

    jclass cls_sync = (jclass) (env->NewLocalRef(g_cls_SyncInfo));
    if (!cls_sync)
    {
        LOGE ("Failed to Get ObjectClass for SyncInfo");
        if (JNI_EDETACHED == envRet) g_jvm->DetachCurrentThread();
        return ;
    }
    jmethodID mid_sync = env->GetMethodID(
                             cls_sync, "<init>", "(JLjava/lang/String;I)V");
    if (!mid_sync)
    {
        LOGE ("Failed to Get MethodID for SyncInfo<init>");
        if (JNI_EDETACHED == envRet) g_jvm->DetachCurrentThread();
        return ;
    }
    jobject obj_sync = env->NewObject(cls_sync, mid_sync,
                                      jMessageId, jProviderId, jState);

    jclass cls = env->GetObjectClass(jSyncListener);
    if (!cls)
    {
        LOGE ("Failed to Get ObjectClass for jSyncListener");
        if (JNI_EDETACHED == envRet) g_jvm->DetachCurrentThread();
        return ;
    }
    jmethodID mid = env->GetMethodID(
                        cls,
                        "onSyncInfoReceived",
                        "(Lorg/iotivity/service/ns/common/SyncInfo;)V");
    if (!mid)
    {
        LOGE ("Failed to Get MethodID for onSyncInfoReceived");
        if (JNI_EDETACHED == envRet) g_jvm->DetachCurrentThread();
        return ;
    }

    env->CallVoidMethod(jSyncListener, mid, obj_sync);

    env->DeleteLocalRef(jSyncListener);
    env->DeleteLocalRef(cls_sync);
    if (JNI_EDETACHED == envRet) g_jvm->DetachCurrentThread();
    return ;
}

JNIEXPORT void JNICALL Java_org_iotivity_service_ns_consumer_ConsumerService_nativeStart
(JNIEnv *env, jobject jObj, jobject jDiscoverListener, jobject jAccpetListener)
{
    LOGD ("ConsumerService_StartConsumer");
    if (!jDiscoverListener || !jAccpetListener)
    {
        ThrowNSException(NS_ERROR, "Listener cannot be null");
        return ;
    }

    if (g_obj_discoverListener != NULL)
    {
        env->DeleteGlobalRef(g_obj_discoverListener);
    }
    if (g_obj_acceptListener != NULL)
    {
        env->DeleteGlobalRef(g_obj_acceptListener);
    }

    g_obj_discoverListener = (jobject) env->NewGlobalRef(jDiscoverListener);
    g_obj_acceptListener = (jobject) env->NewGlobalRef(jAccpetListener);

    OIC::Service::NSConsumerService::ConsumerConfig cfg;
    cfg.m_discoverCb = onDiscoverProvider;
    cfg.m_changedCb = onProviderChanged;

    OIC::Service::NSConsumerService::getInstance()->Start(cfg);

}

JNIEXPORT void JNICALL Java_org_iotivity_service_ns_consumer_ConsumerService_nativeStop
(JNIEnv *env, jobject jObj)
{
    LOGD ("ConsumerService_StopConsumer");

    env->DeleteGlobalRef(g_obj_postListener);
    env->DeleteGlobalRef(g_obj_discoverListener);
    env->DeleteGlobalRef(g_obj_syncListener);
    env->DeleteGlobalRef(g_obj_acceptListener);
    g_obj_postListener = NULL;
    g_obj_discoverListener = NULL;
    g_obj_syncListener = NULL;
    g_obj_acceptListener = NULL;
    OIC::Service::NSConsumerService::getInstance()->Stop();
}

JNIEXPORT jint JNICALL
Java_org_iotivity_service_ns_consumer_ConsumerService_nativeEnableRemoteService
(JNIEnv *env, jobject jObj, jstring jServerAddress)
{
    LOGD ("ConsumerService_EnableRemoteService");
    if (!jServerAddress)
    {
        ThrowNSException(NS_ERROR, "EnableRemoteService server address NULL");
        return (jint) OIC::Service::NSResult::ERROR;
    }
    const char *serverAddress = env->GetStringUTFChars(jServerAddress, 0);
    OIC::Service::NSResult res =
        OIC::Service::NSConsumerService::getInstance()->EnableRemoteService(std::string(serverAddress));
    env->ReleaseStringUTFChars(jServerAddress, serverAddress);
    return (jint) res;
}

JNIEXPORT void JNICALL Java_org_iotivity_service_ns_consumer_ConsumerService_nativeRescanProvider
(JNIEnv *env, jobject jObj)
{
    LOGD ("ConsumerService_RescanProvider");

    OIC::Service::NSConsumerService::getInstance()->RescanProvider();
}

JNIEXPORT jobject JNICALL Java_org_iotivity_service_ns_consumer_ConsumerService_nativeGetProvider
(JNIEnv *env, jobject jObj, jstring jProviderId)
{
    LOGD ("ConsumerService_GetProvider");
    if (!jProviderId)
    {
        ThrowNSException(NS_ERROR, "ProviderId cannot be null");
        return NULL;
    }

    const char *providerId = env->GetStringUTFChars(jProviderId, 0);

    OIC::Service::NSProvider *provider =
        OIC::Service::NSConsumerService::getInstance()->getProvider(std::string(providerId));
    if (provider == nullptr)
    {
        ThrowNSException(NS_ERROR, "Provider with Given Id doesn't exist");
        return NULL;
    }

    LOGD ("ProviderId : %s\n", provider->getProviderId().c_str());

    jProviderId = env->NewStringUTF(provider->getProviderId().c_str());
    jlong pProvider = (long)provider;

    jclass cls_provider = (jclass) (env->NewLocalRef(g_cls_Provider));
    if (!cls_provider)
    {
        LOGE ("Failed to Get ObjectClass for Provider");
        return NULL;
    }
    jmethodID mid_provider = env->GetMethodID(
                                 cls_provider, "<init>", "(Ljava/lang/String;)V");
    if (!mid_provider)
    {
        LOGE ("Failed to Get MethodID for Provider<init>");
        return NULL;
    }
    jobject obj_provider = env->NewObject(cls_provider, mid_provider,
                                          jProviderId);

    jfieldID nativeHandle = env->GetFieldID(cls_provider, "mNativeHandle", "J");
    if (!nativeHandle)
    {
        ThrowNSException(NS_ERROR, "Failed to get nativeHandle for Provider");
        return NULL;
    }
    env->SetLongField(obj_provider, nativeHandle, pProvider);

    env->DeleteLocalRef(cls_provider);
    env->ReleaseStringUTFChars(jProviderId, providerId);

    return obj_provider;
}

OIC::Service::NSProvider *getNativeProvider(JNIEnv *env, jobject jObj)
{
    jclass providerClass = env->GetObjectClass(jObj);
    if (!providerClass)
    {
        ThrowNSException(NS_ERROR, "Failed to Get ObjectClass for Provider");
        return NULL;
    }
    jfieldID jproviderId = env->GetFieldID(providerClass, "mProviderId", "Ljava/lang/String;");
    if (!jproviderId)
    {
        ThrowNSException(NS_ERROR, "Failed to get providerId for Provider");
        return NULL;
    }
    jstring jprovider_id = (jstring) env->GetObjectField(jObj, jproviderId);
    if (!jprovider_id)
    {
        ThrowNSException(NS_ERROR, "ProviderId cannot be null");
        return NULL;
    }

    const char *providerId = env->GetStringUTFChars(jprovider_id, 0);
    LOGD ("ProviderId : %s\n", providerId);

    OIC::Service::NSProvider *provider =
        OIC::Service::NSConsumerService::getInstance()->getProvider(std::string(providerId));
    if (provider == nullptr)
    {
        ThrowNSException(NS_ERROR, "Provider with Given Id doesn't exist");
        return NULL;
    }
    env->ReleaseStringUTFChars(jprovider_id, providerId);
    return provider;
}

JNIEXPORT void JNICALL Java_org_iotivity_service_ns_consumer_Provider_nativeSubscribe
(JNIEnv *env, jobject jObj)
{
    LOGD ("Provider_Subscribe");
    jclass providerClass = env->GetObjectClass(jObj);
    if (!providerClass)
    {
        ThrowNSException(NS_ERROR, "Failed to Get ObjectClass for Provider");
        return ;
    }

    jfieldID nativeHandle = env->GetFieldID(providerClass, "mNativeHandle", "J");
    if (!nativeHandle)
    {
        ThrowNSException(NS_ERROR, "Failed to get nativeHandle for Provider");
        return ;
    }
    jlong jProvider = env->GetLongField(jObj, nativeHandle);
    if (jProvider)
    {
        LOGD ("calling subscribe on mNativeHandle");
        OIC::Service::NSProvider *provider = (OIC::Service::NSProvider *) (jProvider);
        provider->subscribe();
    }
    else
    {
        OIC::Service::NSProvider *provider = getNativeProvider(env, jObj);
        if (provider == nullptr)
        {
            ThrowNSException(NS_ERROR, "Provider with Given Id doesn't exist");
            return;
        }
        LOGD ("calling subscribe on ProviderID");
        provider->subscribe();
    }
}

JNIEXPORT void JNICALL Java_org_iotivity_service_ns_consumer_Provider_nativeUnsubscribe
(JNIEnv *env, jobject jObj)
{
    LOGD ("Provider_Unsubscribe");

    jclass providerClass = env->GetObjectClass(jObj);
    if (!providerClass)
    {
        ThrowNSException(NS_ERROR, "Failed to Get ObjectClass for Provider");
        return ;
    }

    jfieldID nativeHandle = env->GetFieldID(providerClass, "mNativeHandle", "J");
    if (!nativeHandle)
    {
        ThrowNSException(NS_ERROR, "Failed to get nativeHandle for Provider");
        return ;
    }
    jlong jProvider = env->GetLongField(jObj, nativeHandle);
    if (jProvider)
    {
        LOGD ("calling unSubscribe on mNativeHandle");
        OIC::Service::NSProvider *provider = (OIC::Service::NSProvider *) (jProvider);
        provider->unSubscribe();
    }
    else
    {
        OIC::Service::NSProvider *provider = getNativeProvider(env, jObj);
        if (provider == nullptr)
        {
            ThrowNSException(NS_ERROR, "Provider with Given Id doesn't exist");
            return;
        }
        LOGD ("calling unSubscribe on ProviderID");
        provider->unSubscribe();
    }
}

JNIEXPORT void JNICALL Java_org_iotivity_service_ns_consumer_Provider_nativeSendSyncInfo
(JNIEnv *env, jobject jObj, jlong jMessageId, jint jSyncType)
{
    LOGD ("Provider_SendSyncInfo");
    if (!jMessageId)
    {
        ThrowNSException(NS_ERROR, "MessageId cannot be 0");
        return ;
    }

    jclass providerClass = env->GetObjectClass(jObj);
    if (!providerClass)
    {
        ThrowNSException(NS_ERROR, "Failed to Get ObjectClass for Provider");
        return ;
    }

    jfieldID nativeHandle = env->GetFieldID(providerClass, "mNativeHandle", "J");
    if (!nativeHandle)
    {
        ThrowNSException(NS_ERROR, "Failed to get nativeHandle for Provider");
        return ;
    }
    uint64_t messageId = (uint64_t) jMessageId;

    LOGD("!!!!!!jMessageId: %lld", jMessageId);
    LOGD("!!!!!!messageId: %lld", messageId);

    jlong jProvider = env->GetLongField(jObj, nativeHandle);
    if (jProvider)
    {
        LOGD ("calling SendSyncInfo on mNativeHandle");
        OIC::Service::NSProvider *provider = (OIC::Service::NSProvider *) (jProvider);
        provider->SendSyncInfo(messageId, (OIC::Service::NSSyncInfo::NSSyncType)jSyncType);
    }
    else
    {
        OIC::Service::NSProvider *provider = getNativeProvider(env, jObj);
        if (provider == nullptr)
        {
            ThrowNSException(NS_ERROR, "Provider with Given Id doesn't exist");
            return;
        }
        LOGD ("calling SendSyncInfo on ProviderID");
        provider->SendSyncInfo(messageId, (OIC::Service::NSSyncInfo::NSSyncType)jSyncType);
    }
}

JNIEXPORT void JNICALL Java_org_iotivity_service_ns_consumer_Provider_nativeSetListener
(JNIEnv *env, jobject jObj, jobject jPostListener, jobject jSyncListener)
{
    LOGD ("Provider_SetListener");
    if (!jPostListener || !jSyncListener)
    {
        ThrowNSException(NS_ERROR, "Listener cannot be null");
        return ;
    }

    jclass providerClass = env->GetObjectClass(jObj);
    if (!providerClass)
    {
        ThrowNSException(NS_ERROR, "Failed to Get ObjectClass for Provider");
        return ;
    }

    jfieldID nativeHandle = env->GetFieldID(providerClass, "mNativeHandle", "J");
    if (!nativeHandle)
    {
        ThrowNSException(NS_ERROR, "Failed to get nativeHandle for Provider");
        return ;
    }
    jlong jProvider = env->GetLongField(jObj, nativeHandle);
    if (jProvider)
    {
        LOGD ("calling SetListener on mNativeHandle");
        if (g_obj_postListener != NULL)
        {
            env->DeleteGlobalRef(g_obj_postListener);
        }
        if (g_obj_syncListener != NULL)
        {
            env->DeleteGlobalRef(g_obj_syncListener);
        }
        g_obj_syncListener = NULL;
        g_obj_acceptListener = NULL;
        g_obj_postListener = (jobject) env->NewGlobalRef(jPostListener);
        g_obj_syncListener = (jobject) env->NewGlobalRef(jSyncListener);

        OIC::Service::NSProvider *provider = (OIC::Service::NSProvider *) (jProvider);
        provider->setListener(onMessagePosted, onSyncInfoReceived);
    }
    else
    {
        OIC::Service::NSProvider *provider = getNativeProvider(env, jObj);
        if (provider == nullptr)
        {
            ThrowNSException(NS_ERROR, "Provider with Given Id doesn't exist");
            return;
        }
        LOGD ("calling SetListener on ProviderID");
        if (g_obj_postListener != NULL)
        {
            env->DeleteGlobalRef(g_obj_postListener);
        }
        if (g_obj_syncListener != NULL)
        {
            env->DeleteGlobalRef(g_obj_syncListener);
        }
        g_obj_syncListener = NULL;
        g_obj_acceptListener = NULL;
        g_obj_postListener = (jobject) env->NewGlobalRef(jPostListener);
        g_obj_syncListener = (jobject) env->NewGlobalRef(jSyncListener);

        provider->setListener(onMessagePosted, onSyncInfoReceived);
    }
}

// JNI OnLoad
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void *reserved)
{
    LOGD("ConsumerService_JNI_OnLoad");
    g_jvm = jvm;

    JNIEnv *env;
    if (jvm->GetEnv((void **)&env, JNI_CURRENT_VERSION) != JNI_OK)
    {
        LOGE("Failed to get the environment using GetEnv()");
        return JNI_ERR;
    }

    jclass localMessage = env->FindClass(
                              "org/iotivity/service/ns/common/Message");
    if (!localMessage)
    {
        LOGE("Failed to get local Message class");
        return JNI_ERR;
    }
    g_cls_Message = (jclass) (env->NewGlobalRef(localMessage));
    if (!g_cls_Message)
    {
        LOGE("Failed to set Global Message reference");
        return JNI_ERR;
    }

    jclass localProvider = env->FindClass(
                               "org/iotivity/service/ns/consumer/Provider");
    if (!localProvider)
    {
        LOGE("Failed to get local Provider class");
        return JNI_ERR;
    }
    g_cls_Provider = (jclass) (env->NewGlobalRef(localProvider));
    if (!g_cls_Provider)
    {
        LOGE("Failed to set Global Provider reference");
        return JNI_ERR;
    }

    jclass localSyncInfo = env->FindClass(
                               "org/iotivity/service/ns/common/SyncInfo");
    if (!localSyncInfo)
    {
        LOGE("Failed to get local SyncInfo class");
        return JNI_ERR;
    }
    g_cls_SyncInfo = (jclass) (env->NewGlobalRef(localSyncInfo));
    if (!g_cls_SyncInfo)
    {
        LOGE("Failed to set Global NSSyncInfo reference");
        return JNI_ERR;
    }

    jclass localMediaContents = env->FindClass(
                                    "org/iotivity/service/ns/common/MediaContents");
    if (!localMediaContents)
    {
        LOGE("Failed to get local MediaContents class");
        return JNI_ERR;
    }
    g_cls_MediaContents = (jclass) (env->NewGlobalRef(localMediaContents));
    if (!g_cls_MediaContents)
    {
        LOGE("Failed to set Global MediaContents reference");
        return JNI_ERR;
    }

    env->DeleteLocalRef(localMessage);
    env->DeleteLocalRef(localProvider);
    env->DeleteLocalRef(localSyncInfo);
    env->DeleteLocalRef(localMediaContents);

    return NSExceptionInit(env);
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *jvm, void *reserved)
{
    LOGI("ConsumerService_JNI_OnUnload");
    JNIEnv *env;

    if (jvm->GetEnv((void **)&env, JNI_CURRENT_VERSION) != JNI_OK)
    {
        LOGE("Failed to get the environment using GetEnv()");
        return ;
    }

    env->DeleteGlobalRef(g_cls_Message);
    env->DeleteGlobalRef(g_cls_Provider);
    env->DeleteGlobalRef(g_cls_SyncInfo);
    env->DeleteGlobalRef(g_cls_MediaContents);
}

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

#include "notificationConsumer.h"

static JavaVM* g_jvm = NULL;

static jobject g_obj_postListener = NULL;
static jobject g_obj_discoverListener = NULL;
static jobject g_obj_syncListener = NULL;
static jobject g_obj_acceptListener = NULL;

jclass g_cls_NSMessage;
jclass g_cls_NSProvider;
jclass g_cls_NSSyncInfo;

static JNIEnv * GetJNIEnv(jint * ret)
{
    JNIEnv * env = NULL;

    *ret = (*g_jvm)->GetEnv(g_jvm, (void**) &env, JNI_CURRENT_VERSION);
    switch (*ret) {
        case JNI_OK:
            return env;
        case JNI_EDETACHED:
            if ((*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL) != JNI_OK)
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

void onMessagePosted(NSMessage * message)
{
    LOGD ("ConsumerService_onMessagePosted");

    jint envRet;
    JNIEnv *env = GetJNIEnv(&envRet);
    if (NULL == env) return ;

    jobject jPostListener = (jobject) (*env)->NewLocalRef(env, g_obj_postListener);
    if (!jPostListener)
    {
        LOGE ("Failed to Get jPostListener");
        if (JNI_EDETACHED == envRet) (*g_jvm)->DetachCurrentThread(g_jvm);
        return ;
    }

    LOGD ("id : %llu\n", message->messageId);
    LOGD ("title : %s\n", message->title);
    LOGD ("content : %s\n", message->contentText);
    LOGD ("source : %s\n", message->sourceName);

    jlong jMessageId = (jlong) message->messageId;
    jstring jProviderId = (*env)->NewStringUTF(env, message->providerId);
    jstring jTitle = (*env)->NewStringUTF(env, message->title);
    jstring jContentText = (*env)->NewStringUTF(env, message->contentText);
    jstring jSourceName = (*env)->NewStringUTF(env, message->sourceName);
    jlong pMessage = (long) message;

    jclass cls_message = (jclass) ((*env)->NewLocalRef(env, g_cls_NSMessage));
    if (!cls_message)
    {
        LOGE ("Failed to Get ObjectClass for NSMessage");
        if (JNI_EDETACHED == envRet) (*g_jvm)->DetachCurrentThread(g_jvm);
        return ;
    }
    jmethodID mid_message = (*env)->GetMethodID(
            env, cls_message, "<init>",
            "(JLjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;J)V");
    if (!mid_message)
    {
        LOGE ("Failed to Get MethodID for NSMessage<init>");
        if (JNI_EDETACHED == envRet) (*g_jvm)->DetachCurrentThread(g_jvm);
        return ;
    }
    jobject obj_message = (*env)->NewObject(env, cls_message, mid_message,
            jMessageId, jProviderId, jTitle, jContentText, jSourceName, pMessage);

    jclass cls = (*env)->GetObjectClass(env, jPostListener);
    if (!cls)
    {
        LOGE ("Failed to Get ObjectClass for jPostListener");
        if (JNI_EDETACHED == envRet) (*g_jvm)->DetachCurrentThread(g_jvm);
        return ;
    }
    jmethodID mid = (*env)->GetMethodID(env,
            cls,
            "onMessageReceived",
            "(Lorg/iotivity/service/ns/consumer/NSMessage;)V");
    if (!mid)
    {
        LOGE ("Failed to Get MethodID for onMessageReceived");
        if (JNI_EDETACHED == envRet) (*g_jvm)->DetachCurrentThread(g_jvm);
        return ;
    }

    (*env)->CallVoidMethod(env, jPostListener, mid, obj_message);

    (*env)->DeleteLocalRef(env, jPostListener);
    (*env)->DeleteLocalRef(env, cls_message);
    if (JNI_EDETACHED == envRet) (*g_jvm)->DetachCurrentThread(g_jvm);
    return ;
}


void onDiscoverProvider(NSProvider * provider)
{
    LOGD ("ConsumerService_onDiscoverProvider");

    jint envRet;
    JNIEnv *env = GetJNIEnv(&envRet);
    if (NULL == env) return ;

    jobject jDiscoverListener = (jobject) (*env)->NewLocalRef(env, g_obj_discoverListener);
    if (!jDiscoverListener)
    {
        LOGE ("Failed to Get jDiscoverListener");
        if (JNI_EDETACHED == envRet) (*g_jvm)->DetachCurrentThread(g_jvm);
        return ;
    }

    LOGD ("ProviderId : %s\n", provider->providerId);

    jstring jProviderId = (*env)->NewStringUTF(env, provider->providerId);
    jlong pProvider = (long)provider;

    jclass cls_provider = (jclass) ((*env)->NewLocalRef(env, g_cls_NSProvider));
    if (!cls_provider)
    {
        LOGE ("Failed to Get ObjectClass for NSProvider");
        if (JNI_EDETACHED == envRet) (*g_jvm)->DetachCurrentThread(g_jvm);
        return ;
    }
    jmethodID mid_provider = (*env)->GetMethodID(
            env, cls_provider, "<init>", "(Ljava/lang/String;J)V");
    if (!mid_provider)
    {
        LOGE ("Failed to Get MethodID for NSProvider<init>");
        if (JNI_EDETACHED == envRet) (*g_jvm)->DetachCurrentThread(g_jvm);
        return ;
    }
    jobject obj_provider = (*env)->NewObject(env, cls_provider, mid_provider,
            jProviderId, pProvider);

    jclass cls = (*env)->GetObjectClass(env, jDiscoverListener);
    if (!cls)
    {
        LOGE ("Failed to Get ObjectClass for jDiscoverListener");
        if (JNI_EDETACHED == envRet) (*g_jvm)->DetachCurrentThread(g_jvm);
        return ;
    }
    jmethodID mid = (*env)->GetMethodID(env,
            cls,
            "onProviderDiscovered",
            "(Lorg/iotivity/service/ns/consumer/NSProvider;)V");
    if (!mid)
    {
        LOGE ("Failed to Get MethodID for onProviderDiscovered");
        if (JNI_EDETACHED == envRet) (*g_jvm)->DetachCurrentThread(g_jvm);
        return ;
    }

    (*env)->CallVoidMethod(env, jDiscoverListener, mid, obj_provider);

    (*env)->DeleteLocalRef(env, jDiscoverListener);
    (*env)->DeleteLocalRef(env, cls_provider);
    if (JNI_EDETACHED == envRet) (*g_jvm)->DetachCurrentThread(g_jvm);
    return ;
}


void onSyncInfoReceived(NSSyncInfo * sync)
{
    LOGD ("ConsumerService_onSyncInfoReceived");

    jint envRet;
    JNIEnv *env = GetJNIEnv(&envRet);
    if (NULL == env) return ;

    jobject jSyncListener = (jobject) (*env)->NewLocalRef(env, g_obj_syncListener);
    if (!jSyncListener)
    {
        LOGE ("Failed to Get jSyncListener");
        if (JNI_EDETACHED == envRet) (*g_jvm)->DetachCurrentThread(g_jvm);
        return ;
    }

    LOGD ("Sync ID : %llu\n", sync->messageId);
    LOGD ("Sync ProviderId : %s\n", sync->providerId);
    LOGD ("Sync STATE : %d\n", sync->state);

    jlong jMessageId = (jlong) sync->messageId;
    jstring jProviderId = (*env)->NewStringUTF(env, sync->providerId);
    jint jState = (jint) sync->state;

    jclass cls_sync = (jclass) ((*env)->NewLocalRef(env, g_cls_NSSyncInfo));
    if (!cls_sync)
    {
        LOGE ("Failed to Get ObjectClass for NSSyncInfo");
        if (JNI_EDETACHED == envRet) (*g_jvm)->DetachCurrentThread(g_jvm);
        return ;
    }
    jmethodID mid_sync = (*env)->GetMethodID(
            env, cls_sync, "<init>", "(JLjava/lang/String;I)V");
    if (!mid_sync)
    {
        LOGE ("Failed to Get MethodID for NSSyncInfo<init>");
        if (JNI_EDETACHED == envRet) (*g_jvm)->DetachCurrentThread(g_jvm);
        return ;
    }
    jobject obj_sync = (*env)->NewObject(env, cls_sync, mid_sync,
            jMessageId, jProviderId, jState);

    jclass cls = (*env)->GetObjectClass(env, jSyncListener);
    if (!cls)
    {
        LOGE ("Failed to Get ObjectClass for jSyncListener");
        if (JNI_EDETACHED == envRet) (*g_jvm)->DetachCurrentThread(g_jvm);
        return ;
    }
    jmethodID mid = (*env)->GetMethodID(env,
            cls,
            "onSyncInfoReceived",
            "(Lorg/iotivity/service/ns/consumer/NSSyncInfo;)V");
    if (!mid)
    {
        LOGE ("Failed to Get MethodID for onSyncInfoReceived");
        if (JNI_EDETACHED == envRet) (*g_jvm)->DetachCurrentThread(g_jvm);
        return ;
    }

    (*env)->CallVoidMethod(env, jSyncListener, mid, obj_sync);

    (*env)->DeleteLocalRef(env, jSyncListener);
    (*env)->DeleteLocalRef(env, cls_sync);
    if (JNI_EDETACHED == envRet) (*g_jvm)->DetachCurrentThread(g_jvm);
    return ;
}

void onSubscriptionAccepted(NSProvider * provider)
{
    LOGD ("ConsumerService_onSubscriptionAccepted");

    jint envRet;
    JNIEnv *env = GetJNIEnv(&envRet);
    if (NULL == env) return ;

    jobject jAcceptListener = (jobject) (*env)->NewLocalRef(env, g_obj_acceptListener);
    if (!jAcceptListener)
    {
        LOGE ("Failed to Get jAcceptListener");
        if (JNI_EDETACHED == envRet) (*g_jvm)->DetachCurrentThread(g_jvm);
        return ;
    }

    LOGD ("ProviderId : %s\n", provider->providerId);

    jstring jProviderId = (*env)->NewStringUTF(env, provider->providerId);
    jlong pProvider = (long)provider;

    jclass cls_provider = (jclass) ((*env)->NewLocalRef(env, g_cls_NSProvider));
    if (!cls_provider)
    {
        LOGE ("Failed to Get ObjectClass for NSProvider");
        if (JNI_EDETACHED == envRet) (*g_jvm)->DetachCurrentThread(g_jvm);
        return ;
    }
    jmethodID mid_provider = (*env)->GetMethodID(
            env, cls_provider, "<init>", "(Ljava/lang/String;J)V");
    if (!mid_provider)
    {
        LOGE ("Failed to Get MethodID for NSProvider<init>");
        if (JNI_EDETACHED == envRet) (*g_jvm)->DetachCurrentThread(g_jvm);
        return ;
    }
    jobject obj_provider = (*env)->NewObject(env, cls_provider, mid_provider,
            jProviderId, pProvider);

    jclass cls = (*env)->GetObjectClass(env, jAcceptListener);
    if (!cls)
    {
        LOGE ("Failed to Get ObjectClass for jAcceptListener");
        if (JNI_EDETACHED == envRet) (*g_jvm)->DetachCurrentThread(g_jvm);
        return ;
    }
    jmethodID mid = (*env)->GetMethodID(env,
            cls,
            "onSubscriptionAccepted",
            "(Lorg/iotivity/service/ns/consumer/NSProvider;)V");
    if (!mid)
    {
        LOGE ("Failed to Get MethodID for onSubscriptionAccepted");
        if (JNI_EDETACHED == envRet) (*g_jvm)->DetachCurrentThread(g_jvm);
        return ;
    }

    (*env)->CallVoidMethod(env, jAcceptListener, mid, obj_provider);

    (*env)->DeleteLocalRef(env, jAcceptListener);
    (*env)->DeleteLocalRef(env, cls_provider);
    if (JNI_EDETACHED == envRet) (*g_jvm)->DetachCurrentThread(g_jvm);
    return ;

}


JNIEXPORT void JNICALL Java_org_iotivity_service_ns_consumer_ConsumerService_Start
(JNIEnv * env, jobject jObj, jobject jDiscoverListener, jobject jAccpetListener)
{
    LOGD ("ConsumerService_StartConsumer");
    if (!jDiscoverListener || !jAccpetListener)
    {
        ThrowNSException(NS_ERROR, "Listener cannot be null");
        return ;
    }

    //g_obj_postListener = (jobject) (*env)->NewGlobalRef(env, jPostListener);
    g_obj_discoverListener = (jobject) (*env)->NewGlobalRef(env, jDiscoverListener);
    //g_obj_syncListener = (jobject) (*env)->NewGlobalRef(env, jSyncListener);
    g_obj_acceptListener = (jobject) (*env)->NewGlobalRef(env, jAccpetListener);

    NSConsumerConfig cfg;
    cfg.discoverCb = onDiscoverProvider;
    cfg.acceptedCb = onSubscriptionAccepted;
    cfg.messageCb = onMessagePosted;
    cfg.syncInfoCb = onSyncInfoReceived;

    NSResult result = NSStartConsumer(cfg);
    if (NS_OK != result)
    {
        ThrowNSException(result, "Failed to start Consumer");
    }
}

JNIEXPORT void JNICALL Java_org_iotivity_service_ns_consumer_ConsumerService_Stop
(JNIEnv * env, jobject jObj)
{
    LOGD ("ConsumerService_StopConsumer");

    (*env)->DeleteGlobalRef(env, g_obj_postListener);
    (*env)->DeleteGlobalRef(env, g_obj_discoverListener);
    (*env)->DeleteGlobalRef(env, g_obj_syncListener);
    (*env)->DeleteGlobalRef(env, g_obj_acceptListener);

    NSResult result = NSStopConsumer();
    if (NS_OK != result)
    {
        ThrowNSException(result, "Failed to stop Consumer");
    }
}

JNIEXPORT jobject JNICALL Java_org_iotivity_service_ns_consumer_ConsumerService_GetProvider
(JNIEnv * env, jobject jObj, jobject jProviderId)
{
    LOGD ("ConsumerService_GetProvider");
    if (!jProviderId)
    {
        ThrowNSException(NS_ERROR, "ProviderId cannot be null");
        return NULL;
    }

    const char * providerId = (*env)->GetStringUTFChars(env, jProviderId, 0);

    NSProvider * provider = NSConsumerGetProvider(providerId);
    LOGD ("ProviderId : %s\n", provider->providerId);

    jProviderId = (*env)->NewStringUTF(env, provider->providerId);
    jlong pProvider = (long)provider;

    jclass cls_provider = (jclass) ((*env)->NewLocalRef(env, g_cls_NSProvider));
    if (!cls_provider)
    {
        LOGE ("Failed to Get ObjectClass for NSProvider");
        return NULL;
    }
    jmethodID mid_provider = (*env)->GetMethodID(
            env, cls_provider, "<init>", "(Ljava/lang/String;J)V");
    if (!mid_provider)
    {
        LOGE ("Failed to Get MethodID for NSProvider<init>");
        return NULL;
    }
    jobject obj_provider = (*env)->NewObject(env, cls_provider, mid_provider,
            jProviderId, pProvider);

    (*env)->DeleteLocalRef(env, cls_provider);

    return obj_provider;
}

JNIEXPORT void JNICALL Java_org_iotivity_service_ns_consumer_NSProvider_Subscribe
(JNIEnv * env, jobject jObj, jlong jProvider)
{
    LOGD ("NSProvider_Subscribe");
    if (!jProvider)
    {
        ThrowNSException(NS_ERROR, "Provider object cannot be null");
        return ;
    }

    NSProvider * provider = (NSProvider *) (jProvider);

    NSResult result = NSSubscribe(provider);
    if (NS_OK != result)
    {
        ThrowNSException(result, "Failed to subscribe provider");
    }
}

JNIEXPORT void JNICALL Java_org_iotivity_service_ns_consumer_NSProvider_Unsubscribe
(JNIEnv * env, jobject jObj, jlong jProvider)
{
    LOGD ("NSProvider_Unsubscribe");
    if (!jProvider)
    {
        ThrowNSException(NS_ERROR, "Provider object cannot be null");
        return ;
    }

    NSProvider * provider = (NSProvider *) (jProvider);

    NSResult result = NSUnsubscribe(provider);
    if (NS_OK != result)
    {
        ThrowNSException(result, "Failed to unsubscribe provider");
    }
}

JNIEXPORT void JNICALL Java_org_iotivity_service_ns_consumer_NSProvider_SendSyncInfo
(JNIEnv * env, jobject jObj, jobject jProviderId, jlong jMessageId, jint jSyncType)
{
    LOGD ("NSProvider_Unsubscribe");
    if (!jProviderId)
    {
        ThrowNSException(NS_ERROR, "ProviderId cannot be null");
        return ;
    }
    if (!jMessageId)
    {
        ThrowNSException(NS_ERROR, "MessageId cannot be 0");
        return ;
    }

    const char * providerId = (*env)->GetStringUTFChars(env, jProviderId, 0);
    uint64_t messageId = (uint64_t) jMessageId;

    LOGD("!!!!!!jMessageId: %lld", jMessageId);
    LOGD("!!!!!!messageId: %lld", messageId);

    NSResult result = NSConsumerSendSyncInfo(providerId, messageId, (int)jSyncType);
    if (NS_OK != result)
    {
        ThrowNSException(result, "Failed to Send SyncInfo");
    }
}

JNIEXPORT void JNICALL Java_org_iotivity_service_ns_consumer_NSProvider_SetListener
(JNIEnv * env, jobject jObj, jobject jPostListener, jobject jSyncListener)
{
    LOGD ("NSProvider_SetListener");
    if (!jPostListener || !jSyncListener)
    {
        ThrowNSException(NS_ERROR, "Listener cannot be null");
        return ;
    }

    g_obj_postListener = (jobject) (*env)->NewGlobalRef(env, jPostListener);
    g_obj_syncListener = (jobject) (*env)->NewGlobalRef(env, jSyncListener);
}

JNIEXPORT void JNICALL Java_org_iotivity_service_ns_consumer_ConsumerService_RescanProvider
(JNIEnv * env, jobject jObj)
{
    LOGD ("ConsumerService_RescanProvider");

    NSResult result = NSRescanProvider();
    if (NS_OK != result)
    {
        ThrowNSException(result, "Failed to rescan provider");
    }
}

// JNI OnLoad
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM * jvm, void * reserved)
{
    LOGD("ConsumerService_JNI_OnLoad");
    g_jvm = jvm;

    JNIEnv * env;
    if ((*jvm)->GetEnv(jvm, (void **)&env, JNI_CURRENT_VERSION) != JNI_OK)
    {
        LOGE("Failed to get the environment using GetEnv()");
        return JNI_ERR;
    }

    jclass localNSMessage = (*env)->FindClass(env,
            "org/iotivity/service/ns/consumer/NSMessage");
    if (!localNSMessage)
    {
        LOGE("Failed to get local NSMessage");
        return JNI_ERR;
    }
    g_cls_NSMessage = (jclass) ((*env)->NewGlobalRef(env, localNSMessage));
    if (!g_cls_NSMessage)
    {
        LOGE("Failed to set Global NSMessage");
        return JNI_ERR;
    }

    jclass localNSProvider = (*env)->FindClass(env,
            "org/iotivity/service/ns/consumer/NSProvider");
    if (!localNSProvider)
    {
        LOGE("Failed to get local NSProvider");
        return JNI_ERR;
    }
    g_cls_NSProvider = (jclass) ((*env)->NewGlobalRef(env, localNSProvider));
    if (!g_cls_NSProvider)
    {
        LOGE("Failed to set Global NSProvider");
        return JNI_ERR;
    }

    jclass localNSSyncInfo = (*env)->FindClass(env,
            "org/iotivity/service/ns/consumer/NSSyncInfo");
    if (!localNSSyncInfo)
    {
        LOGE("Failed to get local NSSyncInfo");
        return JNI_ERR;
    }
    g_cls_NSSyncInfo = (jclass) ((*env)->NewGlobalRef(env, localNSSyncInfo));
    if (!g_cls_NSSyncInfo)
    {
        LOGE("Failed to set Global NSSyncInfo");
        return JNI_ERR;
    }

    (*env)->DeleteLocalRef(env, localNSMessage);
    (*env)->DeleteLocalRef(env, localNSProvider);
    (*env)->DeleteLocalRef(env, localNSSyncInfo);

    return NSExceptionInit(env);
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM * jvm, void * reserved)
{
    LOGI("ConsumerService_JNI_OnUnload");
    JNIEnv * env;

    if ((*jvm)->GetEnv(jvm, (void **)&env, JNI_CURRENT_VERSION) != JNI_OK)
    {
        LOGE("Failed to get the environment using GetEnv()");
        return ;
    }

    (*env)->DeleteGlobalRef(env, g_cls_NSMessage);
    (*env)->DeleteGlobalRef(env, g_cls_NSProvider);
    (*env)->DeleteGlobalRef(env, g_cls_NSSyncInfo);
}
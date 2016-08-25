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
jclass g_cls_TopicState;
jclass g_cls_Message_Type;
jclass g_cls_Response;
jclass g_cls_Topic;
jclass g_cls_TopicsList;

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

jobject getJavaMessageType(JNIEnv *env, OIC::Service::NSMessage::NSMessageType type)
{
    LOGD ("ConsumerService_getJavaMessageType - IN");
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
    LOGD ("ConsumerService_getJavaMessageType - OUT");
    return NULL;
}

jobject getJavaState(JNIEnv *env, OIC::Service::NSTopic::NSTopicState nsState)
{
    LOGD ("ConsumerService_getJavaState - IN");

    // TopicState
    jclass cls_topicState = (jclass) (env->NewLocalRef(g_cls_TopicState));
    if (!cls_topicState)
    {
        LOGE ("Failed to Get ObjectClass for TopicState Type");
        return nullptr;
    }

    jobject obj_topicState;
    switch (nsState)
    {
        case OIC::Service::NSTopic::NSTopicState::UNSUBSCRIBED:
            {
                static jfieldID fieldID = env->GetStaticFieldID(cls_topicState,
                                          "UNSUBSCRIBED", "Lorg/iotivity/service/ns/common/Topic$TopicState;");
                obj_topicState = env->GetStaticObjectField(cls_topicState, fieldID);
            }
        case OIC::Service::NSTopic::NSTopicState::SUBSCRIBED:
            {
                static jfieldID fieldID = env->GetStaticFieldID(cls_topicState,
                                          "SUBSCRIBED", "Lorg/iotivity/service/ns/common/Topic$TopicState;");
                obj_topicState = env->GetStaticObjectField(cls_topicState, fieldID);
            }

    }
    if (obj_topicState == NULL)
    {
        LOGE("Error: object of field  TopicState  is null");
        return NULL;
    }

    env->DeleteLocalRef(cls_topicState);
    LOGD ("ConsumerService_getJavaState - OUT");
    return obj_topicState;
}


jobject getJavaTopicsList(JNIEnv *env, OIC::Service::NSTopicsList *topicList)
{
    LOGD ("ConsumerService_getJavaTopicsList - IN");
    jclass cls_topicList = (jclass) (env->NewLocalRef(g_cls_TopicsList));
    if (!cls_topicList)
    {
        LOGE ("Failed to Get ObjectClass for TopicsList");
        return NULL;
    }
    jmethodID mid_topicList = env->GetMethodID(cls_topicList, "<init>", "()V");
    if (!mid_topicList)
    {
        LOGE ("Failed to Get MethodID for TopicsList<init>");
        return NULL;
    }
    jobject obj_topicList = env->NewObject(cls_topicList, mid_topicList);
    jmethodID mid_addTopic =
        env->GetMethodID(cls_topicList, "addTopic", "(Lorg/iotivity/service/ns/common/Topic;)V");
    for (auto it : topicList->getTopicsList())
    {
        jobject jState = getJavaState(env, it->getState());
        std::string topicName = it->getTopicName();
        jstring jTopicName = env->NewStringUTF(topicName.c_str());
        env->CallVoidMethod(obj_topicList, mid_addTopic, jTopicName, jState);
    }
    env->DeleteLocalRef(cls_topicList);
    LOGD ("ConsumerService_getJavaTopicsList - OUT");
    return obj_topicList;
}

OIC::Service::NSTopic::NSTopicState getNativeState(JNIEnv *env,  jobject jTopic)
{
    LOGD ("ConsumerService_getNativeState - IN");

    jclass cls_topic = env->GetObjectClass( jTopic);
    // TopicState
    jclass cls_TopicState = (jclass) (env->NewLocalRef(g_cls_TopicState));
    if (!cls_TopicState)
    {
        LOGE ("Failed to Get ObjectClass for cls_TopicState Type");
        ThrowNSException(NS_ERROR, "TopicState class  not found ");
        return OIC::Service::NSTopic::NSTopicState::UNSUBSCRIBED;
    }
    jmethodID mid = env->GetMethodID(cls_TopicState, "ordinal", "()I");
    jfieldID fid_state = env->GetFieldID( cls_TopicState, "mState",
                                          "Lorg/iotivity/service/ns/common/Topic$TopicState;");
    if (fid_state == NULL)
    {
        LOGE("Error: jfieldID for state type  is null");
        ThrowNSException(NS_ERROR, "TopicState fid  not found ");
        return OIC::Service::NSTopic::NSTopicState::UNSUBSCRIBED;
    }
    jobject jobj = env->GetObjectField( jTopic, fid_state);
    if (jobj == NULL)
    {
        LOGE("Error: object of field  state Type is null");
        ThrowNSException(NS_ERROR, "TopicState obj  is null ");
        return OIC::Service::NSTopic::NSTopicState::UNSUBSCRIBED;
    }
    jint jState = env->CallIntMethod(jobj, mid);
    OIC::Service::NSTopic::NSTopicState  state = (OIC::Service::NSTopic::NSTopicState) jState;
    LOGD ("ConsumerService_getNativeState - OUT");
    return state;

}

const char *getNativeTopicName(JNIEnv *env,  jobject jTopic)
{
    LOGD ("ConsumerService_getNativeTopicName - IN");
    jclass cls_topic = env->GetObjectClass( jTopic);
    jfieldID fid_name = env->GetFieldID( cls_topic, "mTopicname",
                                         "Ljava/lang/String;");
    if (fid_name == NULL)
    {
        LOGE("Error: jfieldID for Topic Name  is null");
        return nullptr;
    }
    jstring jTopicName = (jstring) env->GetObjectField( jTopic, fid_name);
    const char *topicName;
    if (jTopicName)
    {
        topicName = env->GetStringUTFChars( jTopicName, NULL);
    }
    else
    {
        LOGI("Info: topicName is null");
    }
    LOGD ("ConsumerService_getNativeTopicName - OUT");
    return topicName;

}

OIC::Service::NSTopicsList *getNativeTopicsList(JNIEnv *env, jobject jTopicList)
{
    LOGD ("ConsumerService_getNativeTopicsList - IN");

    jclass cls_topicList = env->GetObjectClass( jTopicList);
    jfieldID fid_list = env->GetFieldID( cls_topicList, "mTopicsList",
                                         "Ljava/util/Vector;");
    if (fid_list == NULL)
    {
        LOGE("Error: jfieldID for Topic List  is null");
        return nullptr;
    }
    jobject jobj = env->GetObjectField( jTopicList, fid_list);
    if (jobj == NULL)
    {
        LOGE("Error: object of field  Topic List is null");
        return nullptr;
    }
    jclass cls_vec = env->FindClass("java/util/Vector");
    jmethodID sizeMethod = env->GetMethodID(cls_vec,  "size", "()I;");
    int size = env->CallIntMethod(jobj, sizeMethod);
    jmethodID getMethod = env->GetMethodID(cls_vec,  "get", "(I)Ljava/lang/Object;");
    OIC::Service::NSTopicsList *nsTopicList = new OIC::Service::NSTopicsList();
    for (int index = 0; index < size; index++)
    {
        jobject topicObj = env->CallObjectMethod(jobj, getMethod, index);
        const char *name =  getNativeTopicName(env, topicObj);
        std::string topicName(name);
        OIC::Service::NSTopic::NSTopicState state = getNativeState(env, topicObj);
        nsTopicList->addTopic(topicName, state);
    }

    env->DeleteLocalRef(cls_vec);
    env->DeleteLocalRef(cls_topicList);
    LOGD ("ConsumerService_getNativeTopicsList - OUT");
    return nsTopicList;
}

jobject getJavaResponse(JNIEnv *env, OIC::Service::NSResponse response)
{
    LOGD ("ConsumerService_getJavaResponse - IN");
    switch (response)
    {
        case OIC::Service::NSResponse::ALLOW:
            {
                static jfieldID fieldID = env->GetStaticFieldID(g_cls_Response,
                                          "ALLOW", "Lorg/iotivity/service/ns/consumer/ConsumerService$Response;");
                return env->GetStaticObjectField(g_cls_Response, fieldID);
            }
        case OIC::Service::NSResponse::DENY:
            {
                static jfieldID fieldID = env->GetStaticFieldID(g_cls_Response,
                                          "DENY", "Lorg/iotivity/service/ns/consumer/ConsumerService$Response;");
                return env->GetStaticObjectField(g_cls_Response, fieldID);
            }
        case OIC::Service::NSResponse::TOPIC:
            {
                static jfieldID fieldID = env->GetStaticFieldID(g_cls_Response,
                                          "TOPIC", "Lorg/iotivity/service/ns/consumer/ConsumerService$Response;");
                return env->GetStaticObjectField(g_cls_Response, fieldID);
            }
        default:
            return NULL;
    }
    LOGD ("ConsumerService_getJavaResponse - OUT");
    return NULL;
}

OIC::Service::NSProvider *getNativeProvider(JNIEnv *env, jobject jObj)
{
    LOGD ("ConsumerService_getNativeProvider - IN");
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
    LOGD ("ConsumerService_getNativeProvider - OUT");
    return provider;
}

jobject getJavaProvider(JNIEnv *env, OIC::Service::NSProvider *provider)
{
    LOGD ("ConsumerService_getJavaProvider - IN");
    LOGD ("ProviderId : %s\n", provider->getProviderId().c_str());

    jstring jProviderId = env->NewStringUTF(provider->getProviderId().c_str());
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
    jobject obj_provider = env->NewObject(cls_provider, mid_provider, jProviderId);

    jfieldID nativeHandle = env->GetFieldID(cls_provider, "mNativeHandle", "J");
    if (!nativeHandle)
    {
        LOGE("Failed to get nativeHandle for Provider");
        return NULL;
    }
    env->SetLongField(obj_provider, nativeHandle, pProvider);

    jfieldID fid_topic = env->GetFieldID(cls_provider, "mTopicsList",
                                         "Lorg/iotivity/service/ns/common/TopicsList");
    if (!fid_topic)
    {
        LOGE("Failed to get nativeHandle for Provider");
        return NULL;
    }
    jobject topicList = getJavaTopicsList(env, provider->getTopicList());
    env->SetObjectField(obj_provider, fid_topic, topicList);
    env->DeleteLocalRef(cls_provider);
    LOGD ("ConsumerService_getJavaProvider - OUT");
    return obj_provider;
}


jobject getJavaMessage(JNIEnv *env, OIC::Service::NSMessage *message)
{
    LOGD ("ConsumerService_getJavaMessage - IN");

    LOGD ("id : %llu\n", message->getMessageId());
    LOGD ("title : %s\n", message->getTitle().c_str());
    LOGD ("content : %s\n", message->getContentText().c_str());
    LOGD ("source : %s\n", message->getSourceName().c_str());

    jlong jMessageId = (jlong) message->getMessageId();
    jstring jProviderId = env->NewStringUTF(message->getProviderId().c_str());
    jstring jTitle = env->NewStringUTF(message->getTitle().c_str());
    jstring jContentText = env->NewStringUTF(message->getContentText().c_str());
    jstring jSourceName = env->NewStringUTF(message->getSourceName().c_str());
    jstring jTopic = env->NewStringUTF(message->getTopic().c_str());

    jstring jTime = env->NewStringUTF(message->getTime().c_str());
    jlong jTTL = (jlong) message->getTTL();

    jlong pMessage = (long) message;

    jclass cls_message = (jclass) (env->NewLocalRef(g_cls_Message));
    if (!cls_message)
    {
        LOGE ("Failed to Get ObjectClass for Message");
        return NULL ;
    }
    jmethodID mid_message = env->GetMethodID(
                                cls_message, "<init>",
                                "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
    if (!mid_message)
    {
        LOGE ("Failed to Get MethodID for Message<init>");
        return NULL;
    }
    jobject obj_message = env->NewObject(cls_message, mid_message,
                                         jTitle, jContentText, jSourceName);

    jfieldID fid_nativeHandle = env->GetFieldID(cls_message, "mNativeHandle", "J");
    if (!fid_nativeHandle)
    {
        LOGE("Failed to get nativeHandle for Message");
        return NULL;
    }
    env->SetLongField(obj_message, fid_nativeHandle, pMessage);

    jfieldID fid_messageId = env->GetFieldID(cls_message, "mMessageId", "J");
    if (!fid_messageId)
    {
        LOGE("Failed to get field MessageID for Message");
        return NULL;
    }
    env->SetLongField(obj_message, fid_messageId, jMessageId);

    jfieldID fid_providerId = env->GetFieldID(cls_message, "mProviderId", "Ljava/lang/String;");
    if (!fid_providerId)
    {
        LOGE("Failed to get field ProviderID for Message");
        return NULL;
    }
    env->SetObjectField(obj_message, fid_providerId, jProviderId);

    jfieldID fid_time = env->GetFieldID(cls_message, "mTime", "Ljava/lang/String;");
    if (!fid_time)
    {
        LOGE("Failed to get field Time for Message");
        return NULL;
    }
    env->SetObjectField(obj_message, fid_time, jTime);

    jfieldID fid_ttl = env->GetFieldID(cls_message, "mTTL", "J");
    if (!fid_ttl)
    {
        LOGE("Failed to get field TTL for Message");
        return NULL;
    }
    env->SetLongField(obj_message, fid_ttl, jTTL);

    jfieldID fid_topic = env->GetFieldID(cls_message, "mTopic", "Ljava/lang/String;");
    if (!fid_topic)
    {
        LOGE("Failed to get mTopic for Message");
        return NULL;
    }
    env->SetObjectField(obj_message, fid_topic, jTopic);

    OIC::Service::NSMediaContents *mediaCont = message->getMediaContents();
    if (mediaCont != nullptr)
    {
        jstring jIconImage = env->NewStringUTF(mediaCont->getIconImage().c_str());
        jclass cls_mediaContents = (jclass) (env->NewLocalRef(g_cls_MediaContents));
        if (!cls_mediaContents)
        {
            LOGE ("Failed to Get ObjectClass for MediaContents");
            return NULL;
        }
        jmethodID mid_mediaContents = env->GetMethodID(
                                          cls_mediaContents, "<init>", "(Ljava/lang/String;)V");
        if (!mid_mediaContents)
        {
            LOGE ("Failed to Get MethodID for MediaContents<init>");
            return NULL;
        }
        jobject obj_mediaContents = env->NewObject(cls_mediaContents, mid_mediaContents,
                                    jIconImage);

        jfieldID fid_mediaContents = env->GetFieldID(cls_message, "mMediaContents",
                                     "Lorg/iotivity/service/ns/common/MediaContents;");
        if (!fid_mediaContents)
        {
            LOGE("Failed to get field mediaContents for Message");
            return NULL;
        }
        env->SetObjectField(obj_message, fid_mediaContents, obj_mediaContents);

    }

    jobject jType = getJavaMessageType(env, message->getType());
    if (!jType)
    {
        LOGE("Failed to get Type value for Message");
        return NULL;
    }
    jfieldID fid_type = env->GetFieldID(cls_message, "mType",
                                        "Lorg/iotivity/service/ns/common/Message$MessageType;");
    if (!fid_type)
    {
        LOGE("Failed to get field Type for Message");
        return NULL;
    }
    env->SetObjectField(obj_message, fid_type, jType);
    env->DeleteLocalRef(cls_message);
    LOGD ("ConsumerService_getJavaMessage - OUT");
    return obj_message;
}

void onDiscoverProvider(OIC::Service::NSProvider *provider)
{
    LOGD ("ConsumerService_onDiscoverProvider - IN");

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

    jobject obj_provider = getJavaProvider(env, provider);
    if (!obj_provider)
    {
        LOGE ("Failed to Get Provider Object");
        if (JNI_EDETACHED == envRet) g_jvm->DetachCurrentThread();
        return ;
    }

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
    if (JNI_EDETACHED == envRet) g_jvm->DetachCurrentThread();
    LOGD ("ConsumerService_onDiscoverProvider - OUT");
    return ;
}

void onProviderChanged(OIC::Service::NSProvider *provider, OIC::Service::NSResponse response)
{
    LOGD ("ConsumerService_onSubscriptionAccepted -IN");

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
    jobject obj_provider = getJavaProvider(env, provider);
    if (!obj_provider)
    {
        LOGE ("Failed to Get Provider Object");
        if (JNI_EDETACHED == envRet) g_jvm->DetachCurrentThread();
        return ;
    }

    jclass cls = env->GetObjectClass(jAcceptListener);
    if (!cls)
    {
        LOGE ("Failed to Get ObjectClass for jAcceptListener");
        if (JNI_EDETACHED == envRet) g_jvm->DetachCurrentThread();
        return ;
    }
    jmethodID mid = env->GetMethodID(
                        cls,
                        "onProviderChanged",
                        "(Lorg/iotivity/service/ns/consumer/Provider;Lorg/iotivity/service/ns/consumer/ConsumerService$Response;)V");
    if (!mid)
    {
        LOGE ("Failed to Get MethodID for onProviderChanged");
        if (JNI_EDETACHED == envRet) g_jvm->DetachCurrentThread();
        return ;
    }
    jobject obj_response = getJavaResponse(env, response);
    env->CallVoidMethod(jAcceptListener, mid, obj_provider, obj_response);

    env->DeleteLocalRef(jAcceptListener);
    if (JNI_EDETACHED == envRet) g_jvm->DetachCurrentThread();
    LOGD ("ConsumerService_onSubscriptionAccepted -OUT");
    return ;

}

void onMessagePosted(OIC::Service::NSMessage *message)
{
    LOGD ("ConsumerService_onMessagePosted -IN");

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

    jobject obj_message = getJavaMessage( env, message);
    if (!obj_message)
    {
        LOGE ("Failed to Get Message Object");
        if (JNI_EDETACHED == envRet) g_jvm->DetachCurrentThread();
        return ;
    }
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
    if (JNI_EDETACHED == envRet) g_jvm->DetachCurrentThread();
    LOGD ("ConsumerService_onMessagePosted -OUT");
    return ;
}

void onSyncInfoReceived(OIC::Service::NSSyncInfo *sync)
{
    LOGD ("ConsumerService_onSyncInfoReceived - IN");

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
    LOGD ("ConsumerService_onSyncInfoReceived - OUT");
    return ;
}

JNIEXPORT void JNICALL Java_org_iotivity_service_ns_consumer_ConsumerService_nativeStart
(JNIEnv *env, jobject jObj, jobject jDiscoverListener, jobject jAccpetListener)
{
    LOGD ("ConsumerService_StartConsumer - IN");
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
    LOGD ("ConsumerService_StartConsumer - OUT");
    return;

}

JNIEXPORT void JNICALL Java_org_iotivity_service_ns_consumer_ConsumerService_nativeStop
(JNIEnv *env, jobject jObj)
{
    LOGD ("ConsumerService_StopConsumer - IN");

    env->DeleteGlobalRef(g_obj_postListener);
    env->DeleteGlobalRef(g_obj_discoverListener);
    env->DeleteGlobalRef(g_obj_syncListener);
    env->DeleteGlobalRef(g_obj_acceptListener);
    g_obj_postListener = NULL;
    g_obj_discoverListener = NULL;
    g_obj_syncListener = NULL;
    g_obj_acceptListener = NULL;
    OIC::Service::NSConsumerService::getInstance()->Stop();
    LOGD ("ConsumerService_StopConsumer - OUT");
    return;
}

JNIEXPORT jint JNICALL
Java_org_iotivity_service_ns_consumer_ConsumerService_nativeEnableRemoteService
(JNIEnv *env, jobject jObj, jstring jServerAddress)
{
    LOGD ("ConsumerService_EnableRemoteService - IN");
    if (!jServerAddress)
    {
        ThrowNSException(NS_ERROR, "EnableRemoteService server address NULL");
        return (jint) OIC::Service::NSResult::ERROR;
    }
    const char *serverAddress = env->GetStringUTFChars(jServerAddress, 0);
    OIC::Service::NSResult res =
        OIC::Service::NSConsumerService::getInstance()->EnableRemoteService(std::string(serverAddress));
    env->ReleaseStringUTFChars(jServerAddress, serverAddress);
    LOGD ("ConsumerService_EnableRemoteService - OUT");
    return (jint) res;
}

JNIEXPORT void JNICALL Java_org_iotivity_service_ns_consumer_ConsumerService_nativeRescanProvider
(JNIEnv *env, jobject jObj)
{
    LOGD ("ConsumerService_RescanProvider - IN");
    OIC::Service::NSConsumerService::getInstance()->RescanProvider();
    LOGD ("ConsumerService_RescanProvider - OUT");
    return;

}

JNIEXPORT jobject JNICALL Java_org_iotivity_service_ns_consumer_ConsumerService_nativeGetProvider
(JNIEnv *env, jobject jObj, jstring jProviderId)
{
    LOGD ("ConsumerService_GetProvider - IN");
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
    LOGD ("ConsumerService_GetProvider - OUT");
    return obj_provider;
}

JNIEXPORT jobject JNICALL Java_org_iotivity_service_ns_consumer_ConsumerService_nativeGetMessage
(JNIEnv *env, jobject jObj, jlong jMsgId)
{
    LOGI("JNIConsumerService: nativeGetMessage - IN");

    OIC::Service::NSMessage *msg  =
        OIC::Service::NSConsumerService::getInstance()->getMessage((uint64_t)jMsgId);
    if (msg == nullptr)
    {
        ThrowNSException(NS_ERROR, "Message doesn't exist");
        return NULL;
    }

    jobject obj_msg = getJavaMessage(env, msg);

    LOGI("JNIConsumerService: nativeGetMessage - OUT");
    return obj_msg;
}
JNIEXPORT void JNICALL Java_org_iotivity_service_ns_consumer_Provider_nativeSubscribe
(JNIEnv *env, jobject jObj)
{
    LOGD ("Provider_Subscribe -IN");
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
    LOGD ("Provider_Subscribe -OUT");
    return;
}

JNIEXPORT void JNICALL Java_org_iotivity_service_ns_consumer_Provider_nativeUnsubscribe
(JNIEnv *env, jobject jObj)
{
    LOGD ("Provider_Unsubscribe - IN");

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
    LOGD ("Provider_Unsubscribe - IN");
    return;
}

JNIEXPORT void JNICALL Java_org_iotivity_service_ns_consumer_Provider_nativeSendSyncInfo
(JNIEnv *env, jobject jObj, jlong jMessageId, jint jSyncType)
{
    LOGD ("Provider_SendSyncInfo - IN");
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
    LOGD ("Provider_SendSyncInfo - OUT");
    return;
}


JNIEXPORT void JNICALL Java_org_iotivity_service_ns_consumer_Provider_nativeSetListener
(JNIEnv *env, jobject jObj, jobject jPostListener, jobject jSyncListener)
{
    LOGD ("Provider_SetListener - IN");
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
    LOGD ("Provider_SetListener - OUT");
    return;
}

JNIEXPORT jint JNICALL Java_org_iotivity_service_ns_consumer_Provider_nativeSelectInterestTopics
(JNIEnv *env, jobject jObj, jobject jTopicsList)
{
    LOGD("Provider_SelectInterestTopics -IN");
    if (!jTopicsList)
    {
        LOGI("Fail to select Interest Topics - Topic List is null");
        ThrowNSException(NS_ERROR, "TopicList cannot be null");
        return (jint) OIC::Service::NSResult::ERROR;
    }
    OIC::Service::NSTopicsList *nsTopicsList = getNativeTopicsList(env, jTopicsList);
    if (nsTopicsList == nullptr)
    {
        ThrowNSException(NS_ERROR, "NSTopicList cannot be created ");
        return (jint) OIC::Service::NSResult::ERROR;
    }

    jclass providerClass = env->GetObjectClass(jObj);
    if (!providerClass)
    {
        ThrowNSException(NS_ERROR, "Failed to Get ObjectClass for Provider");
        return  (jint) OIC::Service::NSResult::ERROR;
    }

    jfieldID nativeHandle = env->GetFieldID(providerClass, "mNativeHandle", "J");
    if (!nativeHandle)
    {
        ThrowNSException(NS_ERROR, "Failed to get nativeHandle for Provider");
        return  (jint) OIC::Service::NSResult::ERROR;
    }
    jlong jProvider = env->GetLongField(jObj, nativeHandle);
    OIC::Service::NSResult result;
    if (jProvider)
    {
        LOGD ("calling subscribe on mNativeHandle");
        OIC::Service::NSProvider *provider = (OIC::Service::NSProvider *) (jProvider);
        result = provider->selectInterestTopics(nsTopicsList);
    }
    else
    {
        OIC::Service::NSProvider *provider = getNativeProvider(env, jObj);
        if (provider == nullptr)
        {
            ThrowNSException(NS_ERROR, "Provider with Given Id doesn't exist");
            return (jint) OIC::Service::NSResult::ERROR;
        }
        LOGD ("calling subscribe on ProviderID");
        result = provider->selectInterestTopics(nsTopicsList);
    }
    if (result !=  OIC::Service::NSResult::OK)
    {
        LOGI("Fail to select Interest Topics");
    }
    LOGD("Provider_SelectInterestTopics -OUT");
    return (jint) result;
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
    jclass localTopicState = env->FindClass(
                                 "org/iotivity/service/ns/common/Topic$TopicState");
    if (!localTopicState)
    {
        LOGE("Failed to get local TopicState enum");
        return JNI_ERR;
    }
    g_cls_TopicState = (jclass) (env->NewGlobalRef(localTopicState));
    if (!g_cls_TopicState)
    {
        LOGE("Failed to set Global TopicState reference");
        return JNI_ERR;
    }

    jclass localTopic = env->FindClass(
                            "org/iotivity/service/ns/common/Topic");
    if (!localTopic)
    {
        LOGE("Failed to get local TopicState enum");
        return JNI_ERR;
    }
    g_cls_Topic = (jclass) (env->NewGlobalRef(localTopic));
    if (!g_cls_Topic)
    {
        LOGE("Failed to set Global Topic reference");
        return JNI_ERR;
    }

    jclass localMessageType = env->FindClass(
                                  "org/iotivity/service/ns/common/Message$MessageType");
    if (!localMessageType)
    {
        LOGE("Failed to get local Message Type class");
        return JNI_ERR;
    }
    g_cls_Message_Type = (jclass) (env->NewGlobalRef(localMessageType));
    if (!g_cls_Message_Type)
    {
        LOGE("Failed to set Global Message Type reference");
        return JNI_ERR;
    }

    jclass localResponse = env->FindClass(
                               "org/iotivity/service/ns/consumer/ConsumerService$Response");
    if (!localResponse)
    {
        LOGE("Failed to get localResponse  Type class");
        return JNI_ERR;
    }
    g_cls_Response = (jclass) (env->NewGlobalRef(localResponse));
    if (!g_cls_Response)
    {
        LOGE("Failed to set Global Response Type reference");
        return JNI_ERR;
    }

    jclass localTopicsList = env->FindClass(
                                 "org/iotivity/service/ns/common/TopicsList");
    if (!localTopicsList)
    {
        LOGE("Failed to get local Topic class");
        return JNI_ERR;
    }
    g_cls_TopicsList = (jclass) (env->NewGlobalRef(localTopicsList));
    if (!g_cls_TopicsList)
    {
        LOGE("Failed to set Global TopicsList reference");
        return JNI_ERR;
    }
    env->DeleteLocalRef(localMessage);
    env->DeleteLocalRef(localProvider);
    env->DeleteLocalRef(localSyncInfo);
    env->DeleteLocalRef(localMediaContents);
    env->DeleteLocalRef(localTopicState);
    env->DeleteLocalRef(localMessageType);
    env->DeleteLocalRef(localResponse);
    env->DeleteLocalRef(localTopic);
    env->DeleteLocalRef(localTopicsList);

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
    env->DeleteGlobalRef(g_cls_TopicState);
    env->DeleteGlobalRef(g_cls_Message_Type);
    env->DeleteGlobalRef(g_cls_Response);
    env->DeleteGlobalRef(g_cls_Topic);
    env->DeleteGlobalRef(g_cls_TopicsList);
}

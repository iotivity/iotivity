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

#include "JniNotificationProvider.h"
#include "NSProviderService.h"
#include "JniOcRepresentation.h"

static JavaVM *g_jvm_provider = NULL;

static jobject g_obj_subscriptionListener = NULL;
static jobject g_obj_syncListener = NULL;

jclass g_cls_Message;
jclass g_cls_Message_Type;
jclass g_cls_Consumer;
jclass g_cls_SyncInfo;
jclass g_cls_SyncType;
jclass g_cls_MediaContents;
jclass g_cls_Topic;
jclass g_cls_TopicsList;
jclass g_cls_TopicState;
jclass g_cls_OcRepresentation;
jmethodID g_mid_OcRepresentation_N_ctor_bool = NULL;

static JNIEnv *GetJNIEnv(jint *ret)
{
    JNIEnv *env = NULL;

    *ret = g_jvm_provider->GetEnv((void **) &env, JNI_CURRENT_VERSION);
    switch (*ret)
    {
        case JNI_OK:
            return env;
        case JNI_EDETACHED:
            if (g_jvm_provider->AttachCurrentThread(&env, NULL) != JNI_OK)
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

OIC::Service::NSMessage *getNativeMessage(JNIEnv *env, jobject jMsg)
{
    LOGD("JNIProviderService: getMessage - IN");

    jclass cls = env->GetObjectClass( jMsg);

    // Message type
    jclass cls_messageType = (jclass) (env->NewLocalRef(g_cls_Message_Type));
    if (!cls_messageType)
    {
        LOGE ("Failed to Get ObjectClass for Message Type");
        return nullptr;
    }
    jmethodID mid = env->GetMethodID(cls_messageType, "ordinal", "()I");
    jfieldID fid_type = env->GetFieldID( cls, "mType",
                                         "Lorg/iotivity/service/ns/common/Message$MessageType;");
    if (fid_type == NULL)
    {
        LOGE("Error: jfieldID for message type  is null");
        return nullptr;
    }
    jobject jobj = env->GetObjectField( jMsg, fid_type);
    if (jobj == NULL)
    {
        LOGE("Error: object of field  Message Type is null");
        return nullptr;
    }
    jint jtype = env->CallIntMethod(jobj, mid);
    OIC::Service::NSMessage::NSMessageType  type = (OIC::Service::NSMessage::NSMessageType) jtype;

    LOGD("Message Type: %ld\n", (long )type);

    // Message Time
    jfieldID fid_tm = env->GetFieldID( cls, "mTime", "Ljava/lang/String;");
    if (fid_tm == NULL)
    {
        LOGE("Error: jfieldID for message time is null");
        return nullptr;
    }
    jstring jtime = (jstring)env->GetObjectField( jMsg, fid_tm);
    const char *time = "";
    if (jtime)
    {
        time = env->GetStringUTFChars( jtime, NULL);
    }
    else
    {
        LOGD("Info: messageTitle is null");
    }
    LOGD("Message Time: %s\n", time);

    // Message TTL
    jfieldID fid_ttl = env->GetFieldID( cls, "mTTL", "J");
    if (fid_ttl == NULL)
    {
        LOGE("Error: jfieldID for message ttl is null");
        return nullptr;
    }

    jlong jttl = (jlong) env->GetLongField( jMsg, fid_ttl);
    uint64_t  ttl = jttl;
    LOGD("TTL: %d\n", ttl);

    // Message Title
    jfieldID fid_title = env->GetFieldID( cls, "mTitle", "Ljava/lang/String;");
    if (fid_title == NULL)
    {
        LOGE("Error: jfieldID for message title is null");
        return nullptr;
    }
    jstring jmsgTitle = (jstring)env->GetObjectField( jMsg, fid_title);
    const char *messageTitle = "";
    if (jmsgTitle)
    {
        messageTitle = env->GetStringUTFChars( jmsgTitle, NULL);
    }
    else
    {
        LOGD("Info: messageTitle is null");
    }
    LOGD("Message Title: %s\n", messageTitle);

    // Message Content Text
    jfieldID fid_body = env->GetFieldID( cls, "mContentText", "Ljava/lang/String;");
    if (fid_body == NULL)
    {
        LOGE("Error: jfieldID for message context Text is null");
        return nullptr;
    }
    jstring jmsgBody = (jstring)env->GetObjectField( jMsg, fid_body);
    const char *messageBody = "";
    if (jmsgBody)
    {
        messageBody = env->GetStringUTFChars( jmsgBody, NULL);
    }
    else
    {
        LOGD("Info: messageBody is null");
    }
    LOGD("Message Body: %s\n", messageBody);

    // Message Source
    jfieldID fid_source = env->GetFieldID( cls, "mSourceName", "Ljava/lang/String;");
    if (fid_source == NULL)
    {
        LOGE("Error: jfieldID for message source is null");
        return nullptr;
    }
    jstring jmsgSource = (jstring)env->GetObjectField( jMsg, fid_source);
    const char *messageSource = "";
    if (jmsgSource)
    {
        messageSource = env->GetStringUTFChars( jmsgSource, NULL);
    }
    else
    {
        LOGD("Info: messageSource is null");
    }
    LOGD("Message Source: %s\n", messageSource);

    // Message Topic
    jfieldID fid_topic = env->GetFieldID( cls, "mTopic", "Ljava/lang/String;");
    if (fid_topic == NULL)
    {
        LOGE("Error: jfieldID for topic  is null");
        return nullptr;
    }
    jstring jtopic = (jstring)env->GetObjectField( jMsg, fid_topic);
    const char *topic = "";
    if (jtopic)
    {
        topic = env->GetStringUTFChars( jtopic, NULL);
    }
    else
    {
        LOGD("Info: topic is null");
    }
    LOGD("Topic : %s\n", topic);

    // Message MediaContents
    jfieldID fid_media = env->GetFieldID( cls, "mMediaContents",
                                          "Lorg/iotivity/service/ns/common/MediaContents;");
    if (fid_media == NULL)
    {
        LOGE("Error: jfieldID for MediaContents is null");
        return nullptr;
    }
    jobject jmedia = env->GetObjectField( jMsg, fid_media);
    OIC::Service::NSMediaContents *media = nullptr;
    if (jmedia == NULL)
    {
        LOGD("Info: jmedia object of MediaContents inside Message is null");
    }
    else
    {
        jclass cls_MediaContents = (jclass) (env->NewLocalRef(g_cls_MediaContents));
        if (!cls_MediaContents)
        {
            LOGE ("Failed to Get ObjectClass for class MediaContents");
            return nullptr;
        }
        jfieldID fid_icon = env->GetFieldID( cls_MediaContents, "mIconImage", "Ljava/lang/String;");
        if (fid_icon == NULL)
        {
            LOGE("Error: jfieldID for iconImage is null");
            return nullptr;
        }
        jstring jiconImage = (jstring)env->GetObjectField( jmedia, fid_icon);
        const char *iconImage = "";
        if (jiconImage)
        {
            iconImage = env->GetStringUTFChars( jiconImage, NULL);
            media = new OIC::Service::NSMediaContents(std::string(iconImage));
            env->ReleaseStringUTFChars(jiconImage, iconImage);
        }
        else
        {
            LOGD("Info: iconImage is null");
        }
        env->DeleteLocalRef(cls_MediaContents);
        LOGD("iconImage: %s\n", iconImage);
    }

    // Message ExtraInfo
    jfieldID fid_extraInfo = env->GetFieldID( cls, "mExtraInfo",
                             "Lorg/iotivity/base/OcRepresentation;");
    if (fid_extraInfo == NULL)
    {
        LOGE("Error: jfieldID for mExtraInfo is null");
        return nullptr;
    }
    jobject jExtraInfo = env->GetObjectField( jMsg, fid_extraInfo);
    OC::OCRepresentation *representation = nullptr;
    if (jExtraInfo == NULL)
    {
        LOGE("Error: jExtraInfo object of Message is null");
    }
    else
    {
        representation = GetHandle<OC::OCRepresentation>(env, jExtraInfo);
        if (env->ExceptionCheck())
        {
            LOGE("Failed to get native handle from OcRepresentation");
        }
        if (!representation)
        {
            LOGE("Failed to get native object OcRepresentation");
        }
    }

    OIC::Service::NSMessage *nsMsg;
    jfieldID nativeHandle = env->GetFieldID(cls, "mNativeHandle", "J");
    if (!nativeHandle)
    {
        LOGE("Error: fieldID for mNativeHandle is null");
        return nullptr;
    }
    jlong jMessage = env->GetLongField(jMsg, nativeHandle);
    if (jMessage)
    {
        LOGD ("calling sendMessage on mNativeHandle");
        nsMsg = (OIC::Service::NSMessage *) (jMessage);
    }
    else
    {
        nsMsg = OIC::Service::NSProviderService::getInstance()->createMessage();
    }

    nsMsg->setType(type);
    nsMsg->setTime(std::string(time));
    nsMsg->setTTL(ttl);
    nsMsg->setTitle(std::string(messageTitle));
    nsMsg->setContentText(std::string(messageBody));
    nsMsg->setSourceName(std::string(messageSource));
    if(media != nullptr)
    {
        nsMsg->setMediaContents(media);
    }
    nsMsg->setTopic(std::string(topic));
    if (representation != nullptr)
    {
        nsMsg->setExtraInfo(*representation);
    }

    env->DeleteLocalRef(cls_messageType);

    if (jtime)
    {
        env->ReleaseStringUTFChars(jtime, time);
    }
    if (jmsgTitle)
    {
        env->ReleaseStringUTFChars(jmsgTitle, messageTitle);
    }
    if (jmsgBody)
    {
        env->ReleaseStringUTFChars(jmsgBody, messageBody);
    }
    if (jmsgSource)
    {
        env->ReleaseStringUTFChars(jmsgSource, messageSource);
    }
    if (jtopic)
    {
        env->ReleaseStringUTFChars(jtopic, topic);
    }

    LOGD("JNIProviderService: getMessage - OUT");
    return nsMsg;

}

jobject getJavaMessageType(JNIEnv *env, OIC::Service::NSMessage::NSMessageType type)
{
    LOGD ("JNIProviderService: getJavaMessageType - IN");
    switch (type)
    {
        case OIC::Service::NSMessage::NSMessageType::NS_MESSAGE_ALERT:
            {
                static jfieldID fieldID = env->GetStaticFieldID(g_cls_Message_Type,
                                          "ALERT", "Lorg/iotivity/service/ns/common/Message$MessageType;");
                return env->GetStaticObjectField(g_cls_Message_Type, fieldID);
            }
        case OIC::Service::NSMessage::NSMessageType::NS_MESSAGE_NOTICE:
            {
                static jfieldID fieldID = env->GetStaticFieldID(g_cls_Message_Type,
                                          "NOTICE", "Lorg/iotivity/service/ns/common/Message$MessageType;");
                return env->GetStaticObjectField(g_cls_Message_Type, fieldID);
            }
        case OIC::Service::NSMessage::NSMessageType::NS_MESSAGE_EVENT:
            {
                static jfieldID fieldID = env->GetStaticFieldID(g_cls_Message_Type,
                                          "EVENT", "Lorg/iotivity/service/ns/common/Message$MessageType;");
                return env->GetStaticObjectField(g_cls_Message_Type, fieldID);
            }
        case OIC::Service::NSMessage::NSMessageType::NS_MESSAGE_INFO:
            {
                static jfieldID fieldID = env->GetStaticFieldID(g_cls_Message_Type,
                                          "INFO", "Lorg/iotivity/service/ns/common/Message$MessageType;");
                return env->GetStaticObjectField(g_cls_Message_Type, fieldID);
            }
        default:
            return NULL;
    }
    LOGD ("JNIProviderService: getJavaMessageType - OUT");
    return NULL;
}

jobject getJavaMessage(JNIEnv *env, OIC::Service::NSMessage *message)
{
    LOGD ("JNIProviderService: getJavaMessage - IN");

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
    if (!obj_message)
    {
        LOGE ("Failed to Get Java Object for Message");
        return NULL;
    }
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
    if (jType)
    {
        jfieldID fid_type = env->GetFieldID(cls_message, "mType",
                                            "Lorg/iotivity/service/ns/common/Message$MessageType;");
        if (!fid_type)
        {
            LOGE("Failed to get field Type for Message");
            return NULL;
        }
        env->SetObjectField(obj_message, fid_type, jType);
    }

    LOGD("Reading OCRepresentation Object from Native");
    OC::OCRepresentation *ocRepresentation = new OC::OCRepresentation(message->getExtraInfo());
    jlong handle = reinterpret_cast<jlong>(ocRepresentation);
    jobject jRepresentation = env->NewObject(g_cls_OcRepresentation, g_mid_OcRepresentation_N_ctor_bool,
                              handle, true);
    if (!jRepresentation)
    {
        LOGE("Failed to create OcRepresentation");
        delete ocRepresentation;
    }
    else
    {
        LOGD("Created OCRepresentation Object from Native");
    }
    jfieldID fid_extraInfo = env->GetFieldID(cls_message, "mExtraInfo",
                             "Lorg/iotivity/base/OcRepresentation;");
    if (!fid_extraInfo)
    {
        LOGE("Failed to get mExtraInfo for Message");
        delete ocRepresentation;
        return NULL;
    }
    LOGD ("setting extraInfo field");
    env->SetObjectField(obj_message, fid_extraInfo, jRepresentation);

    env->DeleteLocalRef(cls_message);
    LOGD ("JNIProviderService: getJavaMessage - OUT");
    return obj_message;
}

jobject getJavaTopicState(JNIEnv *env, OIC::Service::NSTopic::NSTopicState nsState)
{
    LOGD("JNIProviderService: getJavaTopicState - IN");

    // TopicState
    jclass cls_topicState = (jclass) (env->NewLocalRef(g_cls_TopicState));
    if (!cls_topicState)
    {
        LOGE ("Failed to Get ObjectClass for TopicState Type");
        return NULL;
    }

    switch (nsState)
    {
        case OIC::Service::NSTopic::NSTopicState::UNSUBSCRIBED:
            {
                static jfieldID fieldID = env->GetStaticFieldID(cls_topicState,
                                          "UNSUBSCRIBED", "Lorg/iotivity/service/ns/common/Topic$TopicState;");
                return env->GetStaticObjectField(cls_topicState, fieldID);
            }
        case OIC::Service::NSTopic::NSTopicState::SUBSCRIBED:
            {
                static jfieldID fieldID = env->GetStaticFieldID(cls_topicState,
                                          "SUBSCRIBED", "Lorg/iotivity/service/ns/common/Topic$TopicState;");
                return env->GetStaticObjectField(cls_topicState, fieldID);
            }
        default:
            return NULL;

    }

    LOGD("JNIProviderService: getJavaTopicState - OUT");
    return NULL;
}

jobject getJavaTopicsList(JNIEnv *env, OIC::Service::NSTopicsList *topicList)
{
    LOGD("JNIProviderService: getJavaTopicsList - IN");
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
    if (!obj_topicList)
    {
        LOGE ("Failed to Get object for TopicsList");
        return NULL;
    }
    jmethodID mid_addTopic = env->GetMethodID(cls_topicList, "addTopic",
                             "(Ljava/lang/String;Lorg/iotivity/service/ns/common/Topic$TopicState;)V");
    if (!mid_addTopic)
    {
        LOGE ("Failed to Get MethodID for addTopic");
        return NULL;
    }
    for (auto it : topicList->getTopicsList())
    {
        jobject jState = getJavaTopicState(env, it->getState());
        std::string topicName = it->getTopicName();
        jstring jTopicName = env->NewStringUTF(topicName.c_str());
        env->CallVoidMethod(obj_topicList, mid_addTopic, jTopicName, jState);
    }
    env->DeleteLocalRef(cls_topicList);
    LOGD("JNIProviderService: getJavaTopicsList - OUT");
    return obj_topicList;
}

jobject getJavaSyncType(JNIEnv *env, OIC::Service::NSSyncInfo::NSSyncType nsType)
{
    LOGD ("JNIProviderService: getJavaSyncType - IN");

    // SyncType
    jclass cls_SyncType = (jclass) (env->NewLocalRef(g_cls_SyncType));
    if (!cls_SyncType)
    {
        LOGE ("Failed to Get ObjectClass for SyncType");
        return NULL;
    }
    switch (nsType)
    {
        case OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_UNREAD:
            {
                static jfieldID fieldID = env->GetStaticFieldID(cls_SyncType,
                                          "UNREAD", "Lorg/iotivity/service/ns/common/SyncInfo$SyncType;");
                return env->GetStaticObjectField(cls_SyncType, fieldID);
            }
        case OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_READ :
            {
                static jfieldID fieldID = env->GetStaticFieldID(cls_SyncType,
                                          "READ", "Lorg/iotivity/service/ns/common/SyncInfo$SyncType;");
                return env->GetStaticObjectField(cls_SyncType, fieldID);
            }
        case OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_DELETED :
            {
                static jfieldID fieldID = env->GetStaticFieldID(cls_SyncType,
                                          "DELETED", "Lorg/iotivity/service/ns/common/SyncInfo$SyncType;");
                return env->GetStaticObjectField(cls_SyncType, fieldID);
            }
        default:
            return NULL;
    }

    LOGD ("JNIProviderService: getJavaSyncType - OUT");
    return NULL;
}


void onSubscribeListenerCb(OIC::Service::NSConsumer *consumer)
{
    LOGD("JNIProviderService_onSubscribeListenerCb - IN");

    jint envRet;
    JNIEnv *env = GetJNIEnv(&envRet);
    if (NULL == env) return ;

    jobject jSubscriptionListener = (jobject) env->NewLocalRef(g_obj_subscriptionListener);
    if (!jSubscriptionListener)
    {
        LOGE ("Failed to Get jSubscriptionListener");
        if (JNI_EDETACHED == envRet) g_jvm_provider->DetachCurrentThread();
        return ;
    }

    LOGD("consumer ID : %s\n", consumer->getConsumerId().c_str());

    jstring jConsumerId = env->NewStringUTF( consumer->getConsumerId().c_str());

    jclass cls_consumer = (jclass) (env->NewLocalRef(g_cls_Consumer));
    if (!cls_consumer)
    {
        LOGE ("Failed to Get ObjectClass for Consumer");
        if (JNI_EDETACHED == envRet) g_jvm_provider->DetachCurrentThread();
        return ;
    }

    jmethodID mid_consumer = env->GetMethodID(
                                 cls_consumer,
                                 "<init>",
                                 "(Ljava/lang/String;)V");
    if (!mid_consumer)
    {
        LOGE ("Failed to Get MethodID for Consumer<init>");
        if (JNI_EDETACHED == envRet) g_jvm_provider->DetachCurrentThread();
        return ;
    }
    jobject obj_consumer = env->NewObject( cls_consumer, mid_consumer, jConsumerId);

    jclass cls = env->GetObjectClass( jSubscriptionListener);
    if (!cls)
    {
        LOGE("Failed to Get ObjectClass of jSubscriptionListener");
        if (JNI_EDETACHED == envRet) g_jvm_provider->DetachCurrentThread();
        return;
    }
    jmethodID mid = env->GetMethodID(
                        cls,
                        "onConsumerSubscribed",
                        "(Lorg/iotivity/service/ns/provider/Consumer;)V");
    if (!mid)
    {
        LOGE("Failed to Get MethodID of onConsumerSubscribed");
        if (JNI_EDETACHED == envRet) g_jvm_provider->DetachCurrentThread();
        return;
    }

    env->CallVoidMethod( jSubscriptionListener, mid, obj_consumer);
    env->DeleteLocalRef(jSubscriptionListener);
    env->DeleteLocalRef(cls_consumer);
    if (JNI_EDETACHED == envRet) g_jvm_provider->DetachCurrentThread();
    LOGD("JNIProviderService_onSubscribeListenerCb - OUT");
    return;
}

void onSyncInfoListenerCb(OIC::Service::NSSyncInfo *sync)
{
    LOGD("JNIProviderService_onSyncInfoListenerCb - IN");

    jint envRet;
    JNIEnv *env = GetJNIEnv(&envRet);
    if (NULL == env) return ;

    jobject jSyncListener = (jobject) env->NewLocalRef(g_obj_syncListener);
    if (!jSyncListener)
    {
        LOGE ("Failed to Get jSyncListener");
        if (JNI_EDETACHED == envRet) g_jvm_provider->DetachCurrentThread();
        return ;
    }

    LOGD("Sync ID : %ld\n", (long) sync->getMessageId());
    LOGD("Sync STATE : %d\n", (int) sync->getState());

    jlong jMessageId = (long)  sync->getMessageId();
    jstring jProviderId = env->NewStringUTF(sync->getProviderId().c_str());
    jobject syncType = getJavaSyncType(env, sync->getState());
    if (!syncType)
    {
        LOGE ("Failed to Get syncType for SyncInfo");
        if (JNI_EDETACHED == envRet) g_jvm_provider->DetachCurrentThread();
        return ;
    }
    jclass cls_SyncInfo = (jclass) (env->NewLocalRef(g_cls_SyncInfo));
    if (!cls_SyncInfo)
    {
        LOGE ("Failed to Get ObjectClass for SyncInfo");
        if (JNI_EDETACHED == envRet) g_jvm_provider->DetachCurrentThread();
        return ;
    }
    jmethodID mid_syncInfo = env->GetMethodID(
                                 cls_SyncInfo,
                                 "<init>",
                                 "(JLjava/lang/String;Lorg/iotivity/service/ns/common/SyncInfo$SyncType;)V");
    if (!mid_syncInfo)
    {
        LOGE ("Failed to Get MethodID for SyncInfo");
        if (JNI_EDETACHED == envRet) g_jvm_provider->DetachCurrentThread();
        return ;
    }

    jobject obj_syncInfo = env->NewObject( cls_SyncInfo, mid_syncInfo,
                                           jMessageId, jProviderId, syncType);
    if (!obj_syncInfo)
    {
        LOGE ("Failed to Get Object for SyncInfo");
        if (JNI_EDETACHED == envRet) g_jvm_provider->DetachCurrentThread();
        return ;
    }

    jclass cls = env->GetObjectClass( jSyncListener);
    if (!cls)
    {
        LOGE("Failed to Get ObjectClass");
        if (JNI_EDETACHED == envRet) g_jvm_provider->DetachCurrentThread();
        return;
    }
    jmethodID mid = env->GetMethodID(
                        cls,
                        "onMessageSynchronized",
                        "(Lorg/iotivity/service/ns/common/SyncInfo;)V");
    if (!mid)
    {
        LOGE("Failed to Get MethodID");
        if (JNI_EDETACHED == envRet) g_jvm_provider->DetachCurrentThread();
        return;
    }
    env->CallVoidMethod( jSyncListener, mid, obj_syncInfo);

    env->DeleteLocalRef(jSyncListener);
    env->DeleteLocalRef(cls_SyncInfo);
    if (JNI_EDETACHED == envRet) g_jvm_provider->DetachCurrentThread();

    LOGD("JNIProviderService: OnSyncInfoListenerCb - OUT");
    return;

}

JNIEXPORT jint JNICALL Java_org_iotivity_service_ns_provider_ProviderService_nativeStart
(JNIEnv *env, jobject jObj, jobject jSubscriptionListener, jobject jSyncListener,
 jboolean jPolicy, jstring jUserInfo, jboolean jResourceSecurity)
{
    LOGD("JNIProviderService: nativeStart - IN");
    if (!jSubscriptionListener || !jSyncListener)
    {
        LOGE("Fail to set listeners");
        ThrowNSException(NS_ERROR, "Listener cannot be null");
        return (jint) OIC::Service::NSResult::ERROR;
    }

    if (g_obj_subscriptionListener != NULL)
    {
        env->DeleteGlobalRef(g_obj_subscriptionListener);
    }
    if (g_obj_syncListener != NULL)
    {
        env->DeleteGlobalRef(g_obj_syncListener);
    }

    g_obj_subscriptionListener = (jobject)  env->NewGlobalRef(jSubscriptionListener);
    g_obj_syncListener = (jobject)  env->NewGlobalRef(jSyncListener);

    // check access policy

    OIC::Service::NSProviderService::ProviderConfig cfg;
    cfg.m_subscribeRequestCb =  onSubscribeListenerCb;
    cfg.m_syncInfoCb =  onSyncInfoListenerCb;
    cfg.subControllability = (bool) jPolicy;
    if (!jUserInfo)
    {
        const char *info = env->GetStringUTFChars( jUserInfo, NULL);
        std::string userInfo(info);
        cfg.userInfo = userInfo;
    }
    cfg.resourceSecurity = (bool) jResourceSecurity;

    OIC::Service::NSResult result = OIC::Service::NSProviderService::getInstance()->start(cfg);
    if (result != OIC::Service::NSResult::OK)
    {
        LOGE("Fail to start NSProviderService");

    }

    LOGD("JNIProviderService: nativeStart - OUT");
    return (jint) result;
}

JNIEXPORT jint JNICALL Java_org_iotivity_service_ns_provider_ProviderService_nativeStop
(JNIEnv *env, jobject jObj)
{
    LOGD("JNIProviderService: nativeStop - IN");

    OIC::Service::NSResult result = OIC::Service::NSProviderService::getInstance()->stop();
    if (result !=  OIC::Service::NSResult::OK)
    {
        LOGD("Fail to stop NSProvider service");
        return (jint) result;
    }

    env->DeleteGlobalRef( g_obj_subscriptionListener);
    env->DeleteGlobalRef( g_obj_syncListener);
    g_obj_subscriptionListener = NULL;
    g_obj_syncListener = NULL;

    LOGD("JNIProviderService: nativeStop - OUT");
    return (jint) result;
}

JNIEXPORT jint JNICALL Java_org_iotivity_service_ns_provider_ProviderService_nativeSendMessage
(JNIEnv *env, jobject jObj, jobject jMsg)
{
    LOGD("JNIProviderService: nativeSendMessage - IN");
    if (!jMsg)
    {
        LOGD("Fail to send notification - Message is null");
        ThrowNSException(NS_ERROR, "Message cannot be null");
        return (jint) OIC::Service::NSResult::ERROR;
    }
    OIC::Service::NSMessage *nsMsg = getNativeMessage(env, jMsg);
    if (nsMsg == nullptr)
    {
        ThrowNSException(NS_ERROR, "Message didn't have a field ID ");
        return (jint) OIC::Service::NSResult::ERROR;
    }

    OIC::Service::NSResult result = OIC::Service::NSProviderService::getInstance()->sendMessage(nsMsg);
    if (result !=  OIC::Service::NSResult::OK)
    {
        LOGD("Fail to send NSProvider Message");
    }
    LOGD("JNIProviderService: nativeSendMessage - OUT");
    return (jint) result;
}

JNIEXPORT void JNICALL Java_org_iotivity_service_ns_provider_ProviderService_nativeSendSyncInfo
(JNIEnv *env, jobject jObj, jlong messageId , jint syncState)
{
    LOGD("JNIProviderService: nativeSendSyncInfo - IN");
    OIC::Service::NSProviderService::getInstance()->sendSyncInfo( messageId,
            (OIC::Service::NSSyncInfo::NSSyncType) syncState);
    LOGD("JNIProviderService: nativeSendSyncInfo - OUT");
    return;
}

JNIEXPORT jobject JNICALL Java_org_iotivity_service_ns_provider_ProviderService_nativeCreateMessage
(JNIEnv *env, jobject jObj)
{
    LOGD("JNIProviderService: nativeCreateMessage - IN");
    OIC::Service::NSMessage *message =
        OIC::Service::NSProviderService::getInstance()->createMessage();
    if (message == nullptr)
    {
        ThrowNSException(NS_ERROR, "Couldn't get Native Message");
        return NULL;
    }
    jobject jMsg =  getJavaMessage(env, message);
    if (!jMsg)
    {
        ThrowNSException(NS_ERROR, "Couldn't create Java Message");
        return NULL;
    }
    LOGD("JNIProviderService: nativeCreateMessage - OUT");
    return jMsg;
}

JNIEXPORT jint JNICALL
Java_org_iotivity_service_ns_provider_ProviderService_nativeEnableRemoteService
(JNIEnv *env, jobject jObj, jstring jstr)
{
    LOGD("JNIProviderService: nativeEnableRemoteService - IN");
    if (!jstr)
    {
        ThrowNSException(NS_ERROR, "Server Address Can't be NULL");
        return (jint) OIC::Service::NSResult::ERROR;
    }

    const char *address = env->GetStringUTFChars( jstr, NULL);
    std::string servAddress(address);
    OIC::Service::NSResult result  =
        OIC::Service::NSProviderService::getInstance()->enableRemoteService(
            servAddress);
    if (result !=  OIC::Service::NSResult::OK)
    {
        LOGE("Fail to Enable Remote Service");
    }
    env->ReleaseStringUTFChars(jstr, address);
    LOGD("JNIProviderService: nativeEnableRemoteService - OUT");
    return (jint) result;
}

JNIEXPORT jint JNICALL
Java_org_iotivity_service_ns_provider_ProviderService_nativeDisableRemoteService
(JNIEnv *env, jobject jObj, jstring jstr)
{
    LOGD("JNIProviderService: nativeDisableRemoteService - IN");
    if (!jstr)
    {
        ThrowNSException(NS_ERROR, "Server Address Can't be NULL");
        return (jint) OIC::Service::NSResult::ERROR;
    }

    const char *address = env->GetStringUTFChars( jstr, NULL);
    std::string servAddress(address);
    OIC::Service::NSResult result  =
        OIC::Service::NSProviderService::getInstance()->disableRemoteService(
            servAddress);
    if (result !=  OIC::Service::NSResult::OK)
    {
        LOGE("Fail to Disable Remote Service");
    }
    env->ReleaseStringUTFChars(jstr, address);
    LOGD("JNIProviderService: nativeDisableRemoteService - OUT");
    return (jint) result;
}

JNIEXPORT jint JNICALL Java_org_iotivity_service_ns_provider_ProviderService_nativeRegisterTopic
(JNIEnv *env, jobject jObj, jstring jTopicName)
{
    LOGD("JNIProviderService: nativeRegisterTopic - IN");
    if (!jTopicName)
    {
        ThrowNSException(NS_ERROR, "Topic Name Can't be NULL");
        return (jint) OIC::Service::NSResult::ERROR;
    }
    const char *name = env->GetStringUTFChars( jTopicName, NULL);
    std::string topicName(name);
    OIC::Service::NSResult result  = OIC::Service::NSProviderService::getInstance()->registerTopic(
                                         topicName);
    if (result !=  OIC::Service::NSResult::OK)
    {
        LOGE("Fail to Register Topic");
    }
    env->ReleaseStringUTFChars(jTopicName, name);
    LOGD("JNIProviderService: nativeRegisterTopic - OUT");
    return (jint) result;
}
JNIEXPORT jint JNICALL Java_org_iotivity_service_ns_provider_ProviderService_nativeUnregisterTopic
(JNIEnv *env, jobject jObj, jstring jTopicName)
{
    LOGD("JNIProviderService: nativeUnregisterTopic - IN");
    if (!jTopicName)
    {
        ThrowNSException(NS_ERROR, "Topic Name Can't be NULL");
        return (jint) OIC::Service::NSResult::ERROR;
    }
    const char *name = env->GetStringUTFChars( jTopicName, NULL);
    std::string topicName(name);
    OIC::Service::NSResult result  = OIC::Service::NSProviderService::getInstance()->unregisterTopic(
                                         topicName);
    if (result !=  OIC::Service::NSResult::OK)
    {
        LOGE("Fail to Unregister Topic");
    }
    env->ReleaseStringUTFChars(jTopicName, name);
    LOGD("JNIProviderService: nativeUnregisterTopic - OUT");
    return (jint) result;
}

JNIEXPORT jobject JNICALL
Java_org_iotivity_service_ns_provider_ProviderService_nativeGetRegisteredTopicList
(JNIEnv *env, jobject jObj)
{
    LOGD("JNIProviderService: nativeGetRegisteredTopicList - IN");

    OIC::Service::NSTopicsList *topicList  =
        OIC::Service::NSProviderService::getInstance()->getRegisteredTopicList();
    if (topicList == nullptr)
    {
        ThrowNSException(NS_ERROR, "Topic List doesn't exist");
        return NULL;
    }

    jobject obj_topicList = getJavaTopicsList(env, topicList);

    LOGD("JNIProviderService: nativeGetRegisteredTopicList - OUT");
    return obj_topicList;
}

JNIEXPORT jint JNICALL Java_org_iotivity_service_ns_provider_Consumer_nativeAcceptSubscription
(JNIEnv *env,  jobject jObj, jstring jConsumerId, jboolean jAccepted)
{
    LOGD("JNIProviderService: nativeAcceptSubscription - IN");
    if (!jConsumerId)
    {
        ThrowNSException(NS_ERROR, "ConsumerId Can't be NULL");
        return (jint) OIC::Service::NSResult::ERROR;
    }
    const char *id = env->GetStringUTFChars( jConsumerId, NULL);
    std::string consumerId(id);
    LOGD("Consumer ID: %s\n", consumerId.c_str());

    OIC::Service::NSConsumer *consumer =
        OIC::Service::NSProviderService::getInstance()->getConsumer(consumerId);
    if (consumer)
        return (jint) consumer->acceptSubscription((bool)jAccepted);

    LOGE("Couldn't find consumer");
    LOGD("JNIProviderService: nativeAcceptSubscription - OUT");
    return (jint) OIC::Service::NSResult::ERROR;
}
JNIEXPORT jint JNICALL Java_org_iotivity_service_ns_provider_Consumer_nativeSetConsumerTopic
(JNIEnv *env, jobject jObj, jstring jConsumerId, jstring jTopicName)
{
    LOGD("JNIProviderService: nativeSetConsumerTopic - IN");
    if (!jConsumerId || !jTopicName)
    {
        ThrowNSException(NS_ERROR, "Topic Name or ConsumerId Can't be NULL");
        return (jint) OIC::Service::NSResult::ERROR;
    }
    const char *name = env->GetStringUTFChars( jTopicName, NULL);
    const char *id = env->GetStringUTFChars( jConsumerId, NULL);
    std::string topicName(name);
    std::string consumerId(id);
    OIC::Service::NSConsumer *nsConsumer =
        OIC::Service::NSProviderService::getInstance()->getConsumer(consumerId);
    if (!nsConsumer)
    {
        ThrowNSException(NS_ERROR, "Consumer does exists");
        return (jint) OIC::Service::NSResult::ERROR;
    }
    OIC::Service::NSResult result  = nsConsumer->setTopic(topicName);

    if (result !=  OIC::Service::NSResult::OK)
    {
        LOGD("Fail to Select Topic");
    }
    env->ReleaseStringUTFChars(jTopicName, name);
    env->ReleaseStringUTFChars(jConsumerId, id);
    LOGD("JNIProviderService: nativeSetConsumerTopic - OUT");
    return (jint) result;
}
JNIEXPORT jint JNICALL Java_org_iotivity_service_ns_provider_Consumer_nativeUnsetConsumerTopic
(JNIEnv *env, jobject jObj, jstring jConsumerId, jstring jTopicName)
{
    LOGD("JNIProviderService: nativeUnsetConsumerTopic - IN");
    if (!jConsumerId || !jTopicName)
    {
        ThrowNSException(NS_ERROR, "Topic Name or ConsumerId Can't be NULL");
        return (jint) OIC::Service::NSResult::ERROR;
    }
    const char *name = env->GetStringUTFChars( jTopicName, NULL);
    const char *id = env->GetStringUTFChars( jConsumerId, NULL);
    std::string topicName(name);
    std::string consumerId(id);
    OIC::Service::NSConsumer *nsConsumer =
        OIC::Service::NSProviderService::getInstance()->getConsumer(consumerId);
    if (!nsConsumer)
    {
        ThrowNSException(NS_ERROR, "Consumer does exists");
        return (jint) OIC::Service::NSResult::ERROR;
    }
    OIC::Service::NSResult result  = nsConsumer->unsetTopic(topicName);

    if (result !=  OIC::Service::NSResult::OK)
    {
        LOGE("Fail to Unselect Topic");
    }
    env->ReleaseStringUTFChars(jTopicName, name);
    env->ReleaseStringUTFChars(jConsumerId, id);
    LOGD("JNIProviderService: nativeUnsetConsumerTopic - OUT");
    return (jint) result;
}

JNIEXPORT jobject JNICALL Java_org_iotivity_service_ns_provider_Consumer_nativeGetConsumerTopicList
(JNIEnv *env, jobject jObj, jstring jConsumerId)
{
    LOGD("JNIProviderService: nativeGetConsumerTopicList - IN");
    if (!jConsumerId)
    {
        ThrowNSException(NS_ERROR, "Topic Name or ConsumerId Can't be NULL");
        return NULL;
    }
    const char *id = env->GetStringUTFChars( jConsumerId, NULL);
    std::string consumerId(id);
    OIC::Service::NSConsumer *nsConsumer =
        OIC::Service::NSProviderService::getInstance()->getConsumer(consumerId);
    if (!nsConsumer)
    {
        ThrowNSException(NS_ERROR, "Consumer does exists");
        return NULL;
    }
    env->ReleaseStringUTFChars(jConsumerId, id);
    OIC::Service::NSTopicsList *topicList  = nsConsumer->getConsumerTopicList();
    if (topicList == nullptr)
    {
        ThrowNSException(NS_ERROR, "Topic List doesn't exist");
        return NULL;
    }
    jobject obj_topicList = getJavaTopicsList(env, topicList);
    LOGD("JNIProviderService: nativeGetConsumerTopicList - OUT");
    return obj_topicList;
}

// JNI OnLoad
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void *reserved)
{
    LOGD("ProviderService_JNI_OnLoad");
    g_jvm_provider = jvm;

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

    jclass localConsumer = env->FindClass(
                               "org/iotivity/service/ns/provider/Consumer");
    if (!localConsumer)
    {
        LOGE("Failed to get local Provider class");
        return JNI_ERR;
    }
    g_cls_Consumer = (jclass) (env->NewGlobalRef(localConsumer));
    if (!g_cls_Consumer)
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
        LOGE("Failed to set Global SyncInfo reference");
        return JNI_ERR;
    }

    jclass localSyncType = env->FindClass(
                               "org/iotivity/service/ns/common/SyncInfo$SyncType");
    if (!localSyncType)
    {
        LOGE("Failed to get local SyncType enum");
        return JNI_ERR;
    }
    g_cls_SyncType = (jclass) (env->NewGlobalRef(localSyncType));
    if (!g_cls_SyncType)
    {
        LOGE("Failed to set Global SyncType reference");
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

    jclass localTopic = env->FindClass(
                            "org/iotivity/service/ns/common/Topic");
    if (!localTopic)
    {
        LOGE("Failed to get local Topic class");
        return JNI_ERR;
    }
    g_cls_Topic = (jclass) (env->NewGlobalRef(localTopic));
    if (!g_cls_Topic)
    {
        LOGE("Failed to set Global Topic reference");
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

    //OcRepresentation
    jclass localOcRepresentation = env->FindClass("org/iotivity/base/OcRepresentation");
    if (!localOcRepresentation)
    {
        LOGE("Failed to get local OcRepresentation class");
        return JNI_ERR;
    }
    g_cls_OcRepresentation = (jclass) env->NewGlobalRef(localOcRepresentation);
    if (!g_cls_OcRepresentation)
    {
        LOGE("Failed to set Global OcRepresentation reference");
        return JNI_ERR;
    }

    g_mid_OcRepresentation_N_ctor_bool = env->GetMethodID(g_cls_OcRepresentation, "<init>", "(JZ)V");
    if (!g_mid_OcRepresentation_N_ctor_bool)
    {
        LOGE("Failed to get Global OcRepresentation Constructor reference");
        return JNI_ERR;
    }

    env->DeleteLocalRef(localMessage);
    env->DeleteLocalRef(localMessageType);
    env->DeleteLocalRef(localConsumer);
    env->DeleteLocalRef(localSyncInfo);
    env->DeleteLocalRef(localSyncType);
    env->DeleteLocalRef(localMediaContents);
    env->DeleteLocalRef(localTopic);
    env->DeleteLocalRef(localTopicsList);
    env->DeleteLocalRef(localTopicState);
    env->DeleteLocalRef(localOcRepresentation);

    return NSExceptionInit(env);
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *jvm, void *reserved)
{
    LOGD("ProviderService_JNI_OnUnload");
    JNIEnv *env;

    if (jvm->GetEnv((void **)&env, JNI_CURRENT_VERSION) != JNI_OK)
    {
        LOGE("Failed to get the environment using GetEnv()");
        return ;
    }

    env->DeleteGlobalRef(g_cls_Message);
    env->DeleteGlobalRef(g_cls_Consumer);
    env->DeleteGlobalRef(g_cls_SyncInfo);
    env->DeleteGlobalRef(g_cls_SyncType);
    env->DeleteGlobalRef(g_cls_MediaContents);
    env->DeleteGlobalRef(g_cls_Message_Type);
    env->DeleteGlobalRef(g_cls_Topic);
    env->DeleteGlobalRef(g_cls_TopicsList);
    env->DeleteGlobalRef(g_cls_TopicState);
    env->DeleteGlobalRef(g_cls_OcRepresentation);
}

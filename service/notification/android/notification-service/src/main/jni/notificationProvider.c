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

#include <android/log.h>
#include <stdio.h>
#include "notificationProvider.h"

#define  LOG_TAG   "JNI_NS_INTERFACE"
#define  LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define  LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

static JavaVM *g_jvm = NULL;
static jobject g_obj_subscriptionListener = NULL;
static jobject g_obj_syncListener = NULL;

JNIEXPORT jint JNI_OnLoad(JavaVM *jvm, void *reserved)
{
    LOGI("Initialize NSInterface");
    g_jvm = jvm;

    return JNI_VERSION_1_6;
}

JNIEXPORT jint JNICALL Java_org_iotivity_service_notification_IoTNotification_NSStartProvider(
        JNIEnv * env, jobject jObj, jboolean jAccess, jobject jSubscriptionListener,
        jobject jSyncListener)
{
    LOGI("NSStartProvider...");

    if (!jSubscriptionListener || !jSyncListener)
    {
        LOGI("Fail to set listeners");
    }

    g_obj_subscriptionListener = (jobject) (*env)->NewGlobalRef(env, jSubscriptionListener);
    g_obj_syncListener = (jobject) (*env)->NewGlobalRef(env, jSyncListener);

    // check access policy
    NSAccessPolicy access = NS_ACCEPTER_PROVIDER;

    if (NSStartProvider(access, NSSubscribeRequestCb, NSSyncCb) != NS_OK)
    {
        LOGE("Fail to start NSProvider service");
        return (jint) NS_ERROR;
    }

    return (jint) NS_OK;
}

JNIEXPORT jint JNICALL Java_org_iotivity_service_notification_IoTNotification_NSStopProvider(
        JNIEnv * env, jobject jObj)
{
    LOGI("NSStopProvider");

    (*env)->DeleteGlobalRef(env, g_obj_subscriptionListener);
    (*env)->DeleteGlobalRef(env, g_obj_syncListener);

    if (NSStopProvider() != NS_OK)
    {
        LOGE("Fail to stop NSProvider service");
        return (jint) NS_ERROR;
    }

    return (jint) NS_OK;
}

JNIEXPORT jint JNICALL Java_org_iotivity_service_notification_IoTNotification_NSSendNotification(
        JNIEnv * env, jobject jObj, jobject jMsg)
{
    LOGI("NSSendNotification");

    if (!jMsg)
    {
        LOGI("Fail to send notification - Message is null");
        return (jint) NS_ERROR;
    }

    NSMessage * nsMsg = NSGetMessage(env, jMsg);

    LOGI("JNI TEST - NSSendNotification");
    NSSendNotification(nsMsg);

    return (jint) NS_OK;
}

JNIEXPORT jint JNICALL Java_org_iotivity_service_notification_IoTNotification_NSProviderReadCheck(
        JNIEnv * env, jobject jObj, jobject jMsg)
{
    LOGI("NSReasCheck");
    return 0;
}

JNIEXPORT jint JNICALL Java_org_iotivity_service_notification_IoTNotification_NSAccept(JNIEnv * env,
        jobject jObj, jobject jConsumer, jboolean jAccepted)
{
    if (jAccepted)
    {
        LOGI("Accepted");
        //NSAccept(consumer, true);
    }
    else
    {
        LOGI("Denied");
        //NSAccept(consumer, false);
    }

    return 0;
}

void NSSubscribeRequestCb(NSConsumer *consumer)
{
    LOGI("Subscription requested by consumer");

    JNIEnv * env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**) &env, JNI_VERSION_1_6);
    if (JNI_OK != res)
    {
        if (res == JNI_EDETACHED)
        {
            if ((*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL) != JNI_OK)
            {
                LOGE("Failed to get the environment");
                return;
            }
            else
            {
                LOGE("Success to get the environment");
            }
        }
        else
        {
            LOGE("Failed to get the environment using GetEnv()");
            return;
        }
    }

    LOGI("consumer ID : %s\n", consumer->mDeviceId);
    jstring consumerId = (*env)->NewStringUTF(env, consumer->mDeviceId);

    jclass cls = (*env)->GetObjectClass(env, g_obj_subscriptionListener);
    if (!cls)
    {
        LOGE("Failed to Get ObjectClass");
        return;
    }
    jmethodID mid = (*env)->GetMethodID(env, cls, "OnNSSubscribedEvent", "(Ljava/lang/String;)V");
    if (!mid)
    {
        LOGE("Failed to Get MethodID");
        return;
    }

    (*env)->CallVoidMethod(env, g_obj_subscriptionListener, mid, consumerId);

    (*g_jvm)->DetachCurrentThread(g_jvm);

    NSAccept(consumer, true);

    return;
}

void NSSyncCb(NSSync *sync)
{
    LOGI("Sync requested");

    JNIEnv * env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**) &env, JNI_VERSION_1_6);
    if (JNI_OK != res)
    {
        if (JNI_EDETACHED)
        {
            if ((*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL) < 0)
            {
                LOGE("Failed to get the environment");
                return;
            }
            else
            {
                LOGE("Success to get the environment");
            }
        }
        else
        {
            LOGE("Failed to get the environment using GetEnv()");
            return;
        }
    }

    LOGI("Sync ID : %s\n", sync->mMessageId);
    LOGI("Sync STATE : %d\n", sync->mState);

    jstring strMessageId = (*env)->NewStringUTF(env, sync->mMessageId);

    jclass cls = (*env)->GetObjectClass(env, g_obj_syncListener);
    if (!cls)
    {
        LOGE("Failed to Get ObjectClass");
        return;
    }
    jmethodID mid = (*env)->GetMethodID(env, cls, "OnNSSynchronizedEvent",
            "(Ljava/lang/String;I)V");
    if (!mid)
    {
        LOGE("Failed to Get MethodID");
        return;
    }

    (*env)->CallVoidMethod(env, g_obj_syncListener, mid, strMessageId, (jint) sync->mState);

    (*g_jvm)->DetachCurrentThread(g_jvm);

    return;

}

NSMessage * NSGetMessage(JNIEnv * env, jobject jMsg)
{
    LOGI("NSGetMessage");

    jclass cls = (*env)->GetObjectClass(env, jMsg);

    // Message ID
    jfieldID fid_id = (*env)->GetFieldID(env, cls, "id", "Ljava/lang/String;");
    if (fid_id == NULL)
    {
        LOGE("Error: jfieldID for message id is null");
        return (jint) NS_ERROR;
    }
    jstring jmsgId = (*env)->GetObjectField(env, jMsg, fid_id);
    const char * messageId = (*env)->GetStringUTFChars(env, jmsgId, NULL);
    if (messageId == NULL)
    {
        LOGE("Error: messageId is null");
        return (jint) NS_ERROR;
    }
    LOGI("Message ID: %s\n", messageId);

    // Message Title
    jfieldID fid_title = (*env)->GetFieldID(env, cls, "title", "Ljava/lang/String;");
    if (fid_title == NULL)
    {
        LOGE("Error: jfieldID for message id is null");
        return (jint) NS_ERROR;
    }
    jstring jmsgTitle = (*env)->GetObjectField(env, jMsg, fid_title);
    const char * messageTitle = (*env)->GetStringUTFChars(env, jmsgTitle, NULL);
    if (messageTitle == NULL)
    {
        LOGE("Error: messageTitle is null");
        return (jint) NS_ERROR;
    }
    LOGI("Message Title: %s\n", messageTitle);

    // Message Body
    jfieldID fid_body = (*env)->GetFieldID(env, cls, "body", "Ljava/lang/String;");
    if (fid_body == NULL)
    {
        LOGE("Error: jfieldID for message id is null");
        return (jint) NS_ERROR;
    }
    jstring jmsgBody = (*env)->GetObjectField(env, jMsg, fid_body);
    const char * messageBody = (*env)->GetStringUTFChars(env, jmsgBody, NULL);
    if (messageBody == NULL)
    {
        LOGE("Error: messageBody is null");
        return (jint) NS_ERROR;
    }
    LOGI("Message Body: %s\n", messageBody);

    // Message Source
    jfieldID fid_source = (*env)->GetFieldID(env, cls, "source", "Ljava/lang/String;");
    if (fid_source == NULL)
    {
        LOGE("Error: jfieldID for message source is null");
        return (jint) NS_ERROR;
    }
    jstring jmsgSource = (*env)->GetObjectField(env, jMsg, fid_source);
    const char * messageSource = (*env)->GetStringUTFChars(env, jmsgSource, NULL);
    if (messageSource == NULL)
    {
        LOGE("Error: messageSource is null");
        return (jint) NS_ERROR;
    }
    LOGI("Message Source: %s\n", messageSource);

    NSMessage * nsMsg = (NSMessage *) malloc(sizeof(NSMessage));

    nsMsg->mId = strdup(messageId);
    nsMsg->mTitle = strdup(messageTitle);
    nsMsg->mContentText = strdup(messageBody);
    nsMsg->mSource = strdup(messageSource);

    return nsMsg;

}

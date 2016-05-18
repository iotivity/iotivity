//
// Created by jaesick.shin on 2016-04-26.
//

#include <android/log.h>
#include <stdio.h>
#include "notificationProvider.h"

#define  LOG_TAG   "JNI_NS_INTERFACE"
#define  LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define  LOGE(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

static JavaVM *g_jvm = NULL;
static jobject g_obj = NULL;

JNIEXPORT jint JNI_OnLoad(JavaVM *jvm, void *reserved)
{
    LOGI("Initialize NSInterface");
    g_jvm = jvm;

    return JNI_VERSION_1_6;
}

JNIEXPORT jint JNICALL Java_org_iotivity_service_notification_IoTNotification_NSStartProvider(
        JNIEnv * env, jobject jObj, jboolean jAccess)
{
    LOGI("JNI TEST - NSStartProvider...");

    g_obj = jObj;

    //JNIlogPrintedCallback(env, jObj, "NSStartProvider");

    NSAccessPolicy access = NS_ACCEPTER_PROVIDER;
    if(NSStartProvider(access, NSSubscribeRequestCb, NSSyncCb) == 0)
    {
        LOGI("JNI TEST - Success to start NSProvider service");
    }
    else
    {
        LOGE("JNI TEST - Fail to start NSProvider service");
    }
    //NSStartProvider(access, NULL, NULL);

    return 0;
}

JNIEXPORT jint JNICALL Java_org_iotivity_service_notification_IoTNotification_NSStopProvider(
        JNIEnv * env, jobject jObj)
{
    LOGI("JNI TEST - NSStopProvider");
    return NSStopProvider();

    return 0;
}

JNIEXPORT jint JNICALL Java_org_iotivity_service_notification_IoTNotification_NSSendNotification(
        JNIEnv * env, jobject jObj, jobject jMsg)
{
    LOGI("JNI TEST - NSSendNotification");

//    bool isAttached = false;
//    //JNIEnv* env;
//    jint res = (*g_jvm)->GetEnv(g_jvm, (void**) &env, JNI_VERSION_1_6);
//    if (JNI_OK != res)
//    {
//        LOGI("AttachCurrentThread for JNIEnv pointer");
//        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);
//
//        if (JNI_OK != res)
//        {
//            LOGE("AttachCurrentThread has failed");
//            return -1;
//        }
//        isAttached = true;
//    }

    jclass cls = (*env)->GetObjectClass(env, jMsg);

    // Message ID
    jfieldID fid_id = (*env)->GetFieldID(env, cls, "id", "Ljava/lang/String;");
    if (fid_id == NULL)
    {
        LOGI("JNI TEST - Error: jfieldID for message id is null");
        return 0;
    }
    jstring jmsgId = (*env)->GetObjectField(env, jMsg, fid_id);
    const char * messageId = (*env)->GetStringUTFChars(env, jmsgId, NULL);
    if (messageId == NULL)
    {
        printf("JNI TEST - Error: messageId is null\n");
        return 0;
    }
    LOGI("JNI TEST - Message ID: %s\n", messageId);
    JNIlogPrintedCallback(env, jObj, messageId);

    (*env)->ReleaseStringUTFChars(env, jmsgId, messageId);

    // Message Title
    jfieldID fid_title = (*env)->GetFieldID(env, cls, "title", "Ljava/lang/String;");
    if (fid_title == NULL)
    {
        LOGE("JNI TEST - Error: jfieldID for message id is null");
        return 0;
    }
    jstring jmsgTitle = (*env)->GetObjectField(env, jMsg, fid_title);
    const char * messageTitle = (*env)->GetStringUTFChars(env, jmsgTitle, NULL);
    if (messageTitle == NULL)
    {
        printf("JNI TEST - Error: messageTitle is null\n");
        return 0;
    }
    LOGI("JNI TEST - Message Title: %s\n", messageTitle);
    (*env)->ReleaseStringUTFChars(env, jmsgTitle, messageTitle);

    // Message Body
    jfieldID fid_body = (*env)->GetFieldID(env, cls, "body", "Ljava/lang/String;");
    if (fid_body == NULL)
    {
        LOGE("JNI TEST - Error: jfieldID for message id is null");
        return 0;
    }
    jstring jmsgBody = (*env)->GetObjectField(env, jMsg, fid_body);
    const char * messageBody = (*env)->GetStringUTFChars(env, jmsgBody, NULL);
    if (messageBody == NULL)
    {
        printf("JNI TEST - Error: messageBody is null\n");
        return 0;
    }
    LOGI("JNI TEST - Message Body: %s\n", messageBody);
    (*env)->ReleaseStringUTFChars(env, jmsgBody, messageBody);

//    if (isAttached)
//    {
//        (*g_jvm)->DetachCurrentThread(g_jvm);
//    }

    return 0;
}

JNIEXPORT jint JNICALL Java_org_iotivity_service_notification_IoTNotification_NSProviderReadCheck(
        JNIEnv * env, jobject jObj, jobject jMsg)
{
    printf("JNI TEST - NSReasCheck\n");
    return 0;
}

JNIEXPORT jint JNICALL Java_org_iotivity_service_notification_IoTNotification_NSAccept(JNIEnv * env,
        jobject jObj, jobject jConsumer, jboolean jAccepted)
{
    if (jAccepted)
    {
        printf("JNI TEST - Accepted\n");
        //NSAccept(consumer, true);
    }
    else
    {
        printf("JNI TEST - Denied\n");
        //NSAccept(consumer, false);
    }

    return 0;
}

void JNIlogPrintedCallback(JNIEnv * env, jobject jObj, const char * log)
{
    static jmethodID cb = NULL;
    //jclass cls = (*env)->FindClass(env, "com/sec/notiproviderexample/ProviderExample");
    jclass cls = (*env)->GetObjectClass(env, jObj);
    if (cls == NULL)
    {
        LOGE("JNI TEST - Error: cannot find callback class");
    }

    if (cb == NULL)
    {
        cb = (*env)->GetMethodID(env, cls, "onLogPrinted", "(Ljava/lang/String;)V");
        if (cb == NULL)
        {
            LOGE("JNI TEST - Error: cannot get callback method");
            return;
        }
        LOGI("JNI TEST - Get callback method - onLogPrinted");

        jstring msg = (*env)->NewStringUTF(env, log);
        LOGI("print: %s", log);

        (*env)->CallVoidMethod(env, jObj, cb, msg);
    }
}

void JNISubscribeRequestCallback(JNIEnv * env, jobject jObj, char * consumer)
{
    static jmethodID cb = NULL;
    //jclass cls = (*env)->FindClass(env, "com/sec/notiproviderexample/ProviderExample");
    jclass cls = (*env)->GetObjectClass(env, jObj);
    if (cls == NULL)
    {
        LOGE("JNI TEST - Error: cannot find callback class");
    }

    if (cb == NULL)
    {
        cb = (*env)->GetMethodID(env, cls, "onSubscribeRequest", "(Ljava/lang/String;)V");
        if (cb == NULL)
        {
            LOGE("JNI TEST - Error: cannot get callback method");
            return;
        }
        LOGI("JNI TEST - Get callback method - onSubscribeRequest");

        jstring msg = (*env)->NewStringUTF(env, consumer);
        LOGI("print: %s", consumer);

        (*env)->CallVoidMethod(env, jObj, cb, msg);
    }
}

void JNIMessageSyncCallback(JNIEnv * env, jobject jObj, char * message)
{
    static jmethodID cb = NULL;
    //jclass cls = (*env)->FindClass(env, "com/sec/notiproviderexample/ProviderExample");
    jclass cls = (*env)->GetObjectClass(env, jObj);
    if (cls == NULL)
    {
        LOGE("JNI TEST - Error: cannot find callback class");
    }

    if (cb == NULL)
    {
        cb = (*env)->GetMethodID(env, cls, "onMessageSync", "(Ljava/lang/String;)V");
        if (cb == NULL)
        {
            LOGE("JNI TEST - Error: cannot get callback method");
            return;
        }
        LOGI("JNI TEST - Get callback method - onMessageSync");

        jstring msg = (*env)->NewStringUTF(env, message);
        LOGI("print: %s", message);

        (*env)->CallVoidMethod(env, jObj, cb, msg);
    }
}

void NSSubscribeRequestCb(NSConsumer *consumer)
{
    LOGI("JNI TEST - consumer requested to subscribe");

    char *cid = consumer->mId;
    LOGI("JNI TEST - NS_ Consumer ID: %s\n", cid);

    bool isAttached = false;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**) &env, JNI_VERSION_1_6);
    if (JNI_OK != res)
    {
        LOGI("AttachCurrentThread for JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);

        if (JNI_OK != res)
        {
            LOGE("AttachCurrentThread has failed");
            return;
        }
        isAttached = true;
    }

    // callback to app
    JNISubscribeRequestCallback(env, g_obj, cid);

    if (isAttached)
    {
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }

    NSAccept(consumer, true);
}

void NSSyncCb(NSProvider *provider, NSSync *sync)
{
    LOGI("JNI TEST - sync requested");
    LOGI("JNI TEST - NS_ Sync MessageID: %s\n", sync->mMessageId);
    LOGI("JNI TEST - NS_ Sync State: %d\n", sync->mState);

    bool isAttached = false;
    JNIEnv* env;
    jint res = (*g_jvm)->GetEnv(g_jvm, (void**) &env, JNI_VERSION_1_6);
    if (JNI_OK != res)
    {
        LOGI("AttachCurrentThread for JNIEnv pointer");
        res = (*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL);

        if (JNI_OK != res)
        {
            LOGE("AttachCurrentThread has failed");
            return;
        }
        isAttached = true;
    }

    // callback to app
    JNIMessageSyncCallback(env, g_obj, sync->mMessageId);

    if (isAttached)
    {
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }

}

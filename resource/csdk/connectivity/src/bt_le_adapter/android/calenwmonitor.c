#include <jni.h>
#include <stdio.h>
#include <android/log.h>
#include "logger.h"
#include "com_iotivity_jar_CALeInterface.h"

#define TAG PCF("CA_LE_SERVER")

#define  LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define  LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

static JavaVM *g_jvm;
static jobject gContext;

//FIXME getting context
void CALENetworkMonitorJNISetContext(JNIEnv *env, jobject context)
{
    OIC_LOG_V(DEBUG, TAG, "CALENetworkMonitorJNISetContext");

    if(context == NULL)
        OIC_LOG_V(DEBUG, TAG, "context is null");

    gContext = (*env)->NewGlobalRef(env, context);
}

//FIXME getting jvm
void CALeNetworkMonitorJniInit(JNIEnv *env, JavaVM *jvm)
{
    OIC_LOG_V(DEBUG, TAG, "CALeNetworkMonitorJniInit");
    g_jvm = jvm;
}

void CALEStartNetworkMonitor(JNIEnv *env, jobject obj)
{
    OIC_LOG_V(DEBUG, TAG, "CALEStartNetworkMonitor");

    jclass jni_cls_CALeInterface = (*env)->FindClass(env, "com/iotivity/jar/CALeInterface");
    if (!jni_cls_CALeInterface)
    {
        OIC_LOG_V(DEBUG, TAG, "Could not get CALeInterface class");
        return;
    }

    jmethodID jni_mid_getActionStateIntentFilter = (*env)->GetStaticMethodID(env,
            jni_cls_CALeInterface, "getActionStateIntentFilter",
            "()Landroid/content/IntentFilter;");
    if (!jni_mid_getActionStateIntentFilter)
    {
        OIC_LOG_V(DEBUG, TAG, "Could not get getActionStateIntentFilter method");
        return;
    }

    jobject jni_obj_intentFilter = (*env)->CallStaticObjectMethod(env, jni_cls_CALeInterface,
            jni_mid_getActionStateIntentFilter);
    if (!jni_obj_intentFilter)
    {
        OIC_LOG_V(DEBUG, TAG, "Could not get jni_obj_intentFilter");
        return;
    }

    jclass jni_cls_context = (*env)->FindClass(env, "Landroid/contect/Context");
    if (!jni_cls_context)
    {
        OIC_LOG_V(DEBUG, TAG, "Could not get jni_cls_context class");
        return;
    }

    jmethodID jni_mid_registerReceiver = (*env)->GetMethodID(env, jni_cls_context, "registerReceiver",
                "(Landroid/content/BroadcastReceiver;Landroid/content/IntentFilter;)Landroid/content/Intent;");
    if (!jni_mid_registerReceiver)
    {
        OIC_LOG_V(DEBUG, TAG, "Could not get jni_mid_registerReceiver method");
        return;
    }

    jmethodID jni_obj_registerReceiver = (*env)->CallObjectMethod(env, gContext, jni_mid_registerReceiver, obj, jni_obj_intentFilter);
    if (!jni_obj_registerReceiver)
    {
        OIC_LOG_V(DEBUG, TAG, "Could not get jni_obj_registerReceiver method");
        return;
    }
}

JNIEXPORT void JNICALL
Java_com_iotivity_jar_CALeInterface_CALeStateChangedCallback
(JNIEnv *env, jobject obj, jint status)
{
    // STATE_ON:12, STATE_OFF:10
    OIC_LOG_V(DEBUG, TAG, "CALeInterface - State Changed Callback(%d)", status);
}

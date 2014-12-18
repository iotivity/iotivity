#include <jni.h>
#include <stdio.h>
#include <android/log.h>
#include "logger.h"
#include "calenwmonitor.h"
#include "caleutils.h"
#include "com_iotivity_jar_CALeInterface.h"

#define TAG PCF("CA_LE_MONITOR")

#define  LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define  LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

static JavaVM *g_jvm;
static jobject gContext;
static CALENetStateChantedCallback gNetworkChangeCb = NULL;

//getting context
void CALENetworkMonitorJNISetContext(JNIEnv *env, jobject context)
{
    OIC_LOG_V(DEBUG, TAG, "CALENetworkMonitorJNISetContext");

    if(context == NULL)
        OIC_LOG_V(DEBUG, TAG, "context is null");

    gContext = (*env)->NewGlobalRef(env, context);
}


//getting jvm
void CALeNetworkMonitorJniInit(JNIEnv *env, JavaVM *jvm)
{
    OIC_LOG_V(DEBUG, TAG, "CALeNetworkMonitorJniInit");
    g_jvm = jvm;
}

void CALESetNetStateCallback(CALENetStateChantedCallback callback)
{
    OIC_LOG_V(DEBUG, TAG, "CALESetNetStateCallback");
    gNetworkChangeCb = callback;
}

JNIEXPORT void JNICALL
Java_com_iotivity_jar_CALeInterface_CALeStateChangedCallback
(JNIEnv *env, jobject obj, jint status)
{
    // STATE_ON:12, STATE_OFF:10
    OIC_LOG_V(DEBUG, TAG, "CALeInterface - Network State Changed");

    if(gNetworkChangeCb == NULL)
        OIC_LOG_V(DEBUG, TAG, "gNetworkChangeCb is null", status);

    jstring jni_address = CALEGetLocalDeviceAddress(env);
    const char* localAddress = (*env)->GetStringUTFChars(env, jni_address, NULL);

    gNetworkChangeCb(localAddress, status);
}

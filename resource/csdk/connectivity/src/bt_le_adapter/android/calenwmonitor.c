/******************************************************************
 *
 * Copyright 2014 Samsung Electronics All Rights Reserved.
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

#include <jni.h>
#include <stdio.h>
#include <android/log.h>
#include "logger.h"
#include "calenwmonitor.h"
#include "caleutils.h"
#include "com_iotivity_jar_caleinterface.h"

#define TAG PCF("CA_LE_MONITOR")

static JavaVM *g_jvm;
static jobject g_context;
static CALENetStateChantedCallback g_networkChangeCb = NULL;

//getting context
void CALENetworkMonitorJNISetContext(JNIEnv *env, jobject context)
{
    OIC_LOG_V(DEBUG, TAG, "CALENetworkMonitorJNISetContext");

    if (context == NULL)
    {
        OIC_LOG_V(DEBUG, TAG, "context is null");
    }

    g_context = (*env)->NewGlobalRef(env, context);
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
    g_networkChangeCb = callback;
}

JNIEXPORT void JNICALL
Java_com_iotivity_jar_caleinterface_CALeStateChangedCallback(JNIEnv *env, jobject obj, jint status)
{
    // STATE_ON:12, STATE_OFF:10
    OIC_LOG_V(DEBUG, TAG, "CALeInterface - Network State Changed");

    if (g_networkChangeCb == NULL)
    {
        OIC_LOG_V(DEBUG, TAG, "g_networkChangeCb is null", status);
    }

    jstring jni_address = CALEGetLocalDeviceAddress(env);
    const char* localAddress = (*env)->GetStringUTFChars(env, jni_address, NULL);

    g_networkChangeCb(localAddress, status);
}


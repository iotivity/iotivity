/******************************************************************
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
#include "JniJvm.h"

JavaVM *g_jvm = NULL;

jclass g_cls_RemoteEnrollee = NULL;
jclass g_cls_ESException = NULL;

jmethodID g_mid_RemoteEnrollee_ctor = NULL;
jmethodID g_mid_ESException_ctor = NULL;

// JNI OnLoad
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
    LOGI("JNI_OnLoad");
    JNIEnv *env;
    g_jvm = vm;

    if (g_jvm->GetEnv((void **)&env, JNI_CURRENT_VERSION) != JNI_OK)
    {
        LOGE("Failed to get the environment using GetEnv()");
        return JNI_ERR;
    }

    jclass clazz = nullptr;

    //Remote Enrollee
    clazz = env->FindClass("org/iotivity/service/easysetup/mediator/RemoteEnrollee");
    if (!clazz) return JNI_ERR;
    g_cls_RemoteEnrollee = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_RemoteEnrollee_ctor = env->GetMethodID(g_cls_RemoteEnrollee, "<init>",
                                "(J)V");
    if (!g_mid_RemoteEnrollee_ctor) return JNI_ERR;

    //ESException
    clazz = env->FindClass("org/iotivity/service/easysetup/mediator/ESException");
    if (!clazz) return JNI_ERR;
    g_cls_ESException = (jclass)env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);

    g_mid_ESException_ctor = env->GetMethodID(g_cls_ESException, "<init>", "(Ljava/lang/String;)V");
    if (!g_mid_ESException_ctor) return JNI_ERR;

    return JNI_CURRENT_VERSION;
}

//JNI OnUnload
JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved)
{
    LOGI("JNI_OnUnload");
    JNIEnv *env;

    if (g_jvm->GetEnv((void **)&env, JNI_CURRENT_VERSION) != JNI_OK)
    {
        LOGE("Failed to get the environment using GetEnv()");
        return;
    }
    env->DeleteGlobalRef(g_cls_RemoteEnrollee);
    env->DeleteGlobalRef(g_cls_ESException);
}

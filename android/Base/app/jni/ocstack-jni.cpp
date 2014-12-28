//******************************************************************
//
// Copyright 2014 MediaTek All Rights Reserved.
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

#include "ocstack-jni.h"

// Global map to maintain callback functions and ocresource
// with key = URL + callback operation
// e.g. key = /a/light/GET
std::map<string, JNICallBackContext*> gJNICallBackContextList;

JavaVM* g_JavaVM = NULL;

jclass g_ocplatform_clazz;
jclass g_ocresource_clazz;
jclass g_ocheaderoption_clazz;
jclass g_ocrepresentation_clazz;
jclass g_platformcfg_clazz;

static const char* TAG = "OCSTACK-JNI";

static JNINativeMethod ocplatform_method_table[] = {
  { "configure", "(Lorg/iotivity/base/PlatformConfig;)V", (void *) configure},
  { "findResource", "(Ljava/lang/String;Ljava/lang/String;Lorg/iotivity/base/AbstractFindCallback;)I", (void *) findResource }
};

static int ocplatform_method_table_size = sizeof(ocplatform_method_table) / sizeof(ocplatform_method_table[0]);

static JNINativeMethod platformconfig_method_table[] = {
  { "createNativeInstance", "(IILjava/lang/String;II)J", (void *) createNativeInstance },
};

static int platformconfig_method_table_size = sizeof(platformconfig_method_table) / sizeof(platformconfig_method_table[0]);

static JNINativeMethod ocresource_method_table[] = {
  { "get", "(Lorg/iotivity/base/AbstractGetCallback;)Lorg/iotivity/base/OCStackResult;", (void *) jniOicGet},

  { "put", "(Lorg/iotivity/base/OCRepresentation;Lorg/iotivity/base/AbstractPutCallback;)Lorg/iotivity/base/OCStackResult;", (void *) jniOicPut},
  { "post", "(Lorg/iotivity/base/OCRepresentation;Lorg/iotivity/base/AbstractPostCallback;)Lorg/iotivity/base/OCStackResult;", (void *) jniOicPost},
  { "observe", "(ILorg/iotivity/base/AbstractObserveCallback;)Lorg/iotivity/base/OCStackResult;", (void *) jniOicObserve},
  { "cancelObserve", "()Lorg/iotivity/base/OCStackResult;", (void *) jniOicCancelObserve},
  { "uri", "()Ljava/lang/String;", (void *) uri},
  { "host", "()Ljava/lang/String;", (void *) host},
  { "getResourceTypes", "()[Ljava/lang/String;", (void *) getResourceTypes},
  { "getResourceInterfaces", "()[Ljava/lang/String;", (void *) getResourceInterfaces},
};

static int ocresource_method_table_size = sizeof(ocresource_method_table) / sizeof(ocresource_method_table[0]);

static JNINativeMethod ocheaderoption_method_table[] = {
  { "getOptionID", "()I", (void *) getOptionID},
  { "getOptionData", "()Ljava/lang/String;", (void *) getOptionData},
  { "OCHeaderOptionConstructor", "(ILjava/lang/String;)J", (void *) OCHeaderOptionConstructor},
};

static int ocheaderoption_method_table_size = sizeof(ocheaderoption_method_table) / sizeof(ocheaderoption_method_table[0]);

static JNINativeMethod ocrepresentation_method_table[] = {
  { "OCRepresentationConstructor", "()J", (void *) OCRepresentationConstructor},
  { "getUri", "()Ljava/lang/String;", (void *) getUri},
  { "getValueInt", "(Ljava/lang/String;)I", (void *) getValueInt},
  { "getValueBool", "(Ljava/lang/String;)Z", (void *) getValueBool},
  { "getValueString", "(Ljava/lang/String;)Ljava/lang/String;", (void *) getValueString},
  { "setValueInt", "(Ljava/lang/String;I)V", (void *) setValueInt},
  { "setValueBool", "(Ljava/lang/String;Z)V", (void *) setValueBool},
  { "setValueString", "(Ljava/lang/String;Ljava/lang/String;)V", (void *) setValueString},
  { "hasAttribute", "(Ljava/lang/String;)Z", (void *) hasAttribute},
};

static int ocrepresentation_method_table_size = sizeof(ocrepresentation_method_table) / sizeof(ocrepresentation_method_table[0]);

// JNI OnLoad
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
  JNIEnv* env;

  g_JavaVM = vm;
  __android_log_print(ANDROID_LOG_ERROR, TAG, "g_JavaVM = %p\n", g_JavaVM);

  if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
    return JNI_ERR;
  } else {
    jint ret;
    jclass clazz;

    clazz = env->FindClass("org/iotivity/base/OCPlatform");
    g_ocplatform_clazz = (jclass) env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);
    ret = env->RegisterNatives(g_ocplatform_clazz, ocplatform_method_table, ocplatform_method_table_size);
    __android_log_print(ANDROID_LOG_ERROR, TAG, "load ocplatform = %d\n", ret);

    clazz = env->FindClass("org/iotivity/base/OCResource");
    g_ocresource_clazz = (jclass) env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);
    ret = env->RegisterNatives(g_ocresource_clazz, ocresource_method_table, ocresource_method_table_size);
    __android_log_print(ANDROID_LOG_ERROR, TAG, "load ocresource = %d\n", ret);

    clazz = env->FindClass("org/iotivity/base/PlatformConfig");
    g_platformcfg_clazz = (jclass) env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);
    ret = env->RegisterNatives(g_platformcfg_clazz, platformconfig_method_table, platformconfig_method_table_size);
    __android_log_print(ANDROID_LOG_ERROR, TAG, "load platformconfig = %d\n", ret);

    clazz = env->FindClass("org/iotivity/base/OCHeaderOption");
    g_ocheaderoption_clazz = (jclass) env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);
    ret = env->RegisterNatives(g_ocheaderoption_clazz, ocheaderoption_method_table, ocheaderoption_method_table_size);
    __android_log_print(ANDROID_LOG_ERROR, TAG, "load ocheaderoption = %d\n", ret);

    clazz = env->FindClass("org/iotivity/base/OCRepresentation");
    g_ocrepresentation_clazz = (jclass) env->NewGlobalRef(clazz);
    env->DeleteLocalRef(clazz);
    ret = env->RegisterNatives(g_ocrepresentation_clazz, ocrepresentation_method_table, ocrepresentation_method_table_size);
    __android_log_print(ANDROID_LOG_ERROR, TAG, "load ocrepresentation = %d\n", ret);

    return JNI_VERSION_1_6;
  }
}

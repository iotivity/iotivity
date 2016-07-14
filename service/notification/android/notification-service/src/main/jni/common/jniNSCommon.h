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

#ifndef _JNI_NS_COMMON_H_
#define _JNI_NS_COMMON_H_

#include <jni.h>
#include <android/log.h>

#include "NSCommon.h"

#define  LOG_TAG    "Notification_Consumer"

#define JNI_CURRENT_VERSION JNI_VERSION_1_6

#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#define JNI_EXCEPTION 1000
#define JNI_NO_NATIVE_POINTER 1001
#define JNI_INVALID_VALUE 1002

jobject getNSException(JNIEnv * env, const char * file, const char * functionName, const int line,
    const int code, const char * message);
void throwNSException(JNIEnv * env, jobject ex);

#define GetNSException(code, message) getNSException(env, __FILE__, __func__, __LINE__, code, message)
#define ThrowNSException(code, message) throwNSException(env, GetNSException(code, message))

static const char * NSResultToChar(const int nsresult);
int NSExceptionInit(JNIEnv * env);

#endif // _JNI_NS_COMMON_H_
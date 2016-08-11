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

#include "jniNSCommon.h"

static jclass g_cls_NSException = NULL;
static jmethodID g_mid_NSException_ctor = NULL;
static jmethodID g_mid_NSException_setNativeExceptionLocation = NULL;

static const char * NSResultToChar(const int nsresult)
{
    switch (nsresult)
    {
        case NS_OK:
            return "OK";
        case NS_ERROR:
            return "ERROR";
        case NS_SUCCESS:
            return "SUCCESS";
        case NS_FAIL:
            return "FAIL";
        case NS_ALLOW:
            return "ALLOW";
        case NS_DENY:
            return "DENY";
        case JNI_EXCEPTION:
            return "JNI_EXCEPTION";
        case JNI_NO_NATIVE_POINTER:
            return "JNI_NO_NATIVE_POINTER";
        case JNI_INVALID_VALUE:
            return "JNI_INVALID_VALUE";
        default:
            return "";
    }
}


jobject getNSException(JNIEnv* env, const char* file, const char* functionName,
    const int line, const int code, const char* message)
{
    const char * codeChar = NSResultToChar(code);
    if (!strcmp(codeChar, ""))
    {
        codeChar = NSResultToChar(JNI_INVALID_VALUE);
    }
    jobject exception = (*env)->NewObject(env,
        g_cls_NSException,
        g_mid_NSException_ctor,
        (*env)->NewStringUTF(env, codeChar),
        (*env)->NewStringUTF(env, message));
    if (!exception)
    {
        return NULL;
    }
    (*env)->CallVoidMethod(env,
        exception,
        g_mid_NSException_setNativeExceptionLocation,
        (*env)->NewStringUTF(env, file),
        (*env)->NewStringUTF(env, functionName),
        line);
    if ((*env)->ExceptionCheck(env))
    {
        return NULL;
    }
    return exception;
}

void throwNSException(JNIEnv * env, jobject exception)
{
    (*env)->Throw(env, (jthrowable)exception);
}

int NSExceptionInit(JNIEnv * env)
{
    if (!env)
    {
        LOGE ("JNIEnv is null");
        return JNI_ERR;
    }

    //OcException
    jclass localNSException = (*env)->FindClass(env,
        "org/iotivity/service/ns/consumer/NSException");
    if (!localNSException)
    {
        LOGE ("Failed to get local NSException");
        return JNI_ERR;
    }
    g_cls_NSException = (jclass)(*env)->NewGlobalRef(env, localNSException);
    

    g_mid_NSException_ctor = (*env)->GetMethodID(env, g_cls_NSException, 
        "<init>", "(Ljava/lang/String;Ljava/lang/String;)V");
    if (!g_mid_NSException_ctor)
    {
        LOGE ("Failed to Get MethodID");
        return JNI_ERR;
    }

    g_mid_NSException_setNativeExceptionLocation = (*env)->GetMethodID(env, g_cls_NSException, 
        "setNativeExceptionLocation",
        "(Ljava/lang/String;Ljava/lang/String;I)V");
    if (!g_mid_NSException_setNativeExceptionLocation)
    {
        LOGE ("Failed to Get MethodID");
        return JNI_ERR;
    }

    (*env)->DeleteLocalRef(env, localNSException);

    return JNI_CURRENT_VERSION;
}
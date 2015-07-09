/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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

#ifndef SIMULATOR_COMMON_JNI_H_
#define SIMULATOR_COMMON_JNI_H_

#include <jni.h>

static jfieldID GetHandleField(JNIEnv *env, jobject jobj)
{
    jclass cls = env->GetObjectClass(jobj);
    return env->GetFieldID(cls, "nativeHandle", "J");
}

template <typename T>
static T *GetHandle(JNIEnv *env, jobject jobj)
{
    jlong handle = env->GetLongField(jobj, GetHandleField(env, jobj));
    return reinterpret_cast<T *>(handle);
}

template <typename T>
static void SetHandle(JNIEnv *env, jobject jobj, T *type)
{
    jlong handle = reinterpret_cast<jlong>(type);

    env->SetLongField(jobj, GetHandleField(env, jobj), handle);
}

typedef struct
{
    jclass classInteger;
    jclass classDouble;
    jclass classString;
    jclass classHashMap;
    jclass classSimulatorResource;
    jclass classSimulatorResourceModel;
    jclass classSimulatorResourceAttribute;

    jmethodID classIntegerCtor;
    jmethodID classDoubleCtor;
    jmethodID classHashMapCtor;
    jmethodID classHashMapPut;
    jmethodID classSimulatorResourceCtor;
    jmethodID classSimulatorResourceSetURI;
    jmethodID classSimulatorResourceSetResourceType;
    jmethodID classSimulatorResourceSetInterfaceType;
    jmethodID classSimulatorResourceSetName;
    jmethodID classSimulatorResourceModelCtor;
    jmethodID classSimulatorResourceAttributeCtor;
} SimulatorClassRefs;

#endif

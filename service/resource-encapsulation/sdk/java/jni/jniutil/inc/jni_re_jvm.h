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

/**
  * @file   jni_re_jvm.h
  *
  * @brief  This file contains the essential declarations and functions required
  *            for JNI implementation
  */

#ifndef __JNI_RE_JVM_H
#define __JNI_RE_JVM_H

#include <jni.h>
#include <android/log.h>
#include<string>

#define RETAG "RE-JNI"
#define JNI_CURRENT_VERSION JNI_VERSION_1_6

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, RETAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, RETAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, RETAG, __VA_ARGS__)

extern JavaVM *g_jvm;

extern jclass g_cls_Integer;
extern jclass g_cls_Double;
extern jclass g_cls_Boolean;
extern jclass g_cls_String;
extern jclass g_cls_LinkedList;
extern jclass g_cls_Iterator;
extern jclass g_cls_Vector;
extern jclass g_cls_Map;
extern jclass g_cls_Set;
extern jclass g_cls_MapEntry;
extern jclass g_cls_RCSRemoteResourceObject;
extern jclass g_cls_RCSRemoteResourceAttributesObject;
extern jclass g_cls_RCSException;

extern jclass g_cls_RCSResourceObject;
extern jclass g_cls_RCSRequest;
extern jclass g_cls_RCSBundleInfo;

extern jmethodID g_mid_Integer_ctor;
extern jmethodID g_mid_Double_ctor;
extern jmethodID g_mid_Boolean_ctor;
extern jmethodID g_mid_LinkedList_ctor;
extern jmethodID g_mid_LinkedList_add_object;
extern jmethodID g_mid_Map_entrySet;
extern jmethodID g_mid_MapEntry_getKey;
extern jmethodID g_mid_MapEntry_getValue;
extern jmethodID g_mid_Set_iterator;
extern jmethodID g_mid_Iterator_hasNext;
extern jmethodID g_mid_Iterator_next;
extern jmethodID g_mid_HashMap_ctor;
extern jmethodID g_mid_HashMap_put;
extern jmethodID g_mid_RCSRemoteResourceObject_ctor;
extern jmethodID g_mid_RCSRemoteResourceAttributesObject_ctor;
extern jmethodID g_mid_RCSException_ctor;

extern jmethodID g_mid_RCSResourceObject_ctor;
extern jmethodID g_mid_RCSRequest_ctor;
extern jmethodID g_mid_RCSBundleInfo_ctor;

typedef void(*RemoveListenerCallback)(JNIEnv *env, jobject jListener);

/**
 * @brief Get the native handle field
 */
static jfieldID REGetHandleField(JNIEnv *env, jobject jobj)
{
    jclass cls = env->GetObjectClass(jobj);
    return env->GetFieldID(cls, "m_nativeHandle", "J");
}

/**
 * @brief Get the native handle
 */
template <typename T>
static T *REGetHandle(JNIEnv *env, jobject jobj)
{
    jlong handle = env->GetLongField(jobj, REGetHandleField(env, jobj));
    return reinterpret_cast<T *>(handle);
}

/**
 * @brief Set the native handle
 */
template <typename T>
static void RESetHandle(JNIEnv *env, jobject jobj, T *type)
{
    jlong handle = reinterpret_cast<jlong>(type);

    env->SetLongField(jobj, REGetHandleField(env, jobj), handle);
}

/**
 * @brief Get the JNI Environment
 */
static JNIEnv *GetREJNIEnv(jint &ret)
{
    JNIEnv *env = NULL;

    ret = g_jvm->GetEnv((void **)&env, JNI_CURRENT_VERSION);
    switch (ret)
    {
        case JNI_OK:
            return env;
        case JNI_EDETACHED:
            if (g_jvm->AttachCurrentThread(&env, NULL) < 0)
            {
                LOGE("Failed to get the environment");
                return nullptr;
            }
            else
            {
                return env;
            }

        case JNI_EVERSION:
            LOGE("JNI version not supported");
        default:
            LOGE("Failed to get the environment");
            return nullptr;
    }
}
#endif // __JNI_RE_JVM_H

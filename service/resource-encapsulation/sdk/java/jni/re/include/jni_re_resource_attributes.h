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

/** @file   jni_re_resource_attributes.h
 *
 *   @brief  This file contains the declaration of Resource Attribute APIs
 *               for JNI implementation
 */

#ifndef __JNI_RE_RCS_RESOURCE_ATTRIBUTES_H_
#define __JNI_RE_RCS_RESOURCE_ATTRIBUTES_H_

#include "jni_re_jvm.h"
#include "jni_re_utils.h"

#include <RCSResourceAttributes.h>

#define JNI_EXCEPTION 1000
#define JNI_NO_NATIVE_POINTER 1001
#define JNI_INVALID_VALUE 1002
#define JNI_NO_SUCH_KEY 1003

struct NullType {};

/**
 * @class   JniResourceAttributes
 * @brief   This class provides the API for getting the RCSResourceAttribute pointer
 *
 */
class JniResourceAttributes
{
    public:
        /**
         * @brief Get the ResourceAttibute pointer
         *      internally get the native handle by calling REGetHandle
         */
        static RCSResourceAttributes *getResourceAttributePtr(JNIEnv *env, jobject thiz);
};

/**
 * @brief Utility structure for jobject conversion
 */
struct JObjectConverter : boost::static_visitor < jobject >
{
        JObjectConverter(JNIEnv *env) : env(env) {}

        jobject operator()(const NullType &) const { return nullptr; }
        jobject operator()(const int &val) const
        {
            jobject jobj = env->NewObject(
                               g_cls_Integer,
                               g_mid_Integer_ctor,
                               static_cast<jint>(val));
            return jobj;
        }
        jobject operator()(const double &val) const
        {
            jobject jobj = env->NewObject(
                               g_cls_Double,
                               g_mid_Double_ctor,
                               static_cast<jdouble>(val));
            return jobj;
        }
        jobject operator()(const bool &val) const
        {
            jobject jobj = env->NewObject(
                               g_cls_Boolean,
                               g_mid_Boolean_ctor,
                               static_cast<jboolean>(val));
            return jobj;
        }
        jobject operator()(const std::string &val) const
        {
            jstring jstr = env->NewStringUTF(val.c_str());
            return static_cast<jobject>(jstr);
        }
    private:
        JNIEnv *env;
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * API for getting attribute value corresponding the requested Key
 *
 * @param key - Attribute key
 *
 */
JNIEXPORT jobject JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeGetValueN
(JNIEnv *env, jobject interfaceObject, jstring key);

/**
 * API for setting integer value for attribute
 *
 * @param key - Attribute key
 * @param value - Attribute value
 *
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSetValueInteger
(JNIEnv *env, jobject interfaceObject, jstring key, jint value);

/**
 * API for setting Double value for attribute
 *
 * @param key - Attribute key
 * @param value - Attribute value
 *
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSetValueDouble
(JNIEnv *env, jobject interfaceObject, jstring key, jdouble value);

/**
 * API for setting Boolean value for attribute
 *
 * @param key - Attribute key
 * @param value - Attribute value
 *
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSetValueBoolean
(JNIEnv *env, jobject interfaceObject, jstring key, jboolean value);

/**
 * API for setting String value for attribute
 *
 * @param Key - Attribute key
 * @param value - Attribute value
 *
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSetValueString
(JNIEnv *env, jobject interfaceObject, jstring key, jstring value);

/**
 * API for setting IntegerArray for attribute
 *
 * @param key - Attribute key
 * @param value - Attribute value
 *
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSetValueIntegerArray
(JNIEnv *env, jobject interfaceObject, jstring key, jintArray value);

/**
 * API for checking whether attribute is empty or not
 *
 * @return  jBoolean - boolean value (true/false)
 *
 */
JNIEXPORT jboolean JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeIsEmpty
(JNIEnv *env, jobject interfaceObject);

/**
 * API for getting the size of the attributes
 *
 * @return jint - size of attributes
 *
 */
JNIEXPORT jint JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSize
(JNIEnv *env, jobject interfaceObject);

/**
 * API for removing a particular attribute
 *
 * @param key - Attribute key
 *
 * @return  jBoolean - boolean value (true/false)
 *
 * NOTE: If requested attribute is successfully removed it will return true otherwise false
 */
JNIEXPORT jboolean JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeRemove
(JNIEnv *env, jobject interfaceObject, jstring key);

/**
 * API for checking a particular attribute
 *
 * @param key - Attribute key
 *
 * @return  jBoolean - boolean value (true/false)
 *
 * NOTE: If requested attribute is present it will return true otherwise false
 */
JNIEXPORT jboolean JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeHasAttribute
(JNIEnv *env, jobject interfaceObject, jstring key);

/**
 * @brief API for creating a new RCSResourceAttributes object and setting of Native handle
 */
JNIEXPORT void JNICALL Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_create
(JNIEnv *env, jobject interfaceObject);

/**
 * @brief API for deleting the RCSResourceAttributes object
 */
JNIEXPORT void JNICALL Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_dispose
(JNIEnv *env, jobject interfaceObject, jboolean jNeedsDelete);

#ifdef __cplusplus
}
#endif
#endif  //__JNI_RE_RCS_RESOURCE_ATTRIBUTES_H_

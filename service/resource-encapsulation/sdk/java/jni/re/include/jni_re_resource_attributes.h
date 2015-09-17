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

/** @file
 * This file contains the declaration of Resource Attribute APIs
 *  for JNI implementation
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
 * @class
 * This class provides the API for getting the RCSResourceAttribute pointer
 */
class JniResourceAttributes
{
    public:

        /**
         * Get the ResourceAttibute pointer
         * internally get the native handle by calling REGetHandle
         */
        static RCSResourceAttributes *getResourceAttributePtr(JNIEnv *env, jobject thiz);
};

/**
 * Utility structure for jobject conversion
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
 * API for getting attribute value corresponding to requested Key
 *
 * @param key - Attribute key
 * @param dataType - dataType for the value.
 *
 * @return value of attribute
 *
 * @throws RCSException
 */
JNIEXPORT jobject JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeGetValueN
(JNIEnv *env, jobject interfaceObject, jstring key, jint dataType);

/**
 * API for setting integer value for attribute
 *
 * @param key - Attribute key
 * @param value - Attribute value
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSetValueInt
(JNIEnv *env, jobject interfaceObject, jstring key, jint value);

/**
 * API for setting Vector of integer as attribute value
 *
 * @param key - Attribute key
 * @param value - Attribute value
 *
 * @throws RCSException - If Key is empty
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSetValueVectorOfInt
(JNIEnv *env, jobject interfaceObject, jstring key, jobject value);

/**
 * API for setting vector of vector of integer as attribute value
 *
 * @param key - Attribute key
 * @param value - Attribute value
 *
 * @throws RCSException - If Key is empty
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSetValueVectorOfVectorOfInt
(JNIEnv *env, jobject interfaceObject, jstring key, jobject value);

/**
 * API for setting vector of vector of vector of integer as attribute value
 *
 * @param key - Attribute key
 * @param value - Attribute value
 *
 * @throws RCSException - If Key is empty
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSetValueVectorOfVectorOfVectorOfInt
(JNIEnv *env, jobject interfaceObject, jstring key, jobject value);

/**
 * API for setting double as attribute value
 *
 * @param key - Attribute key
 * @param value - Attribute value
 *
 * @throws RCSException - If Key is empty
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSetValueDouble
(JNIEnv *env, jobject interfaceObject, jstring key, jdouble value);

/**
 * API for setting Vector of double as attribute value
 *
 * @param key - Attribute key
 * @param value - Attribute value
 *
 * @throws RCSException - If Key is empty
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSetValueVectorOfDouble
(JNIEnv *env, jobject interfaceObject, jstring key, jobject value);

/**
 * API for setting Vector of Vector of double as attribute value
 *
 * @param key - Attribute key
 * @param value - Attribute value
 *
 * @throws RCSException - If Key is empty
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSetValueVectorOfVectorOfDouble
(JNIEnv *env, jobject interfaceObject, jstring key, jobject value);

/**
 * API for setting Vector of Vector of Vector of double as attribute value
 *
 * @param key - Attribute key
 * @param value - Attribute value
 *
 * @throws RCSException - If Key is empty
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSetValueVectorOfVectorOfVectorOfDouble
(JNIEnv *env, jobject interfaceObject, jstring key, jobject value);


/**
 * API for setting boolean as attribute value
 *
 * @param key - Attribute key
 * @param value - Attribute value
 *
 * @throws RCSException - If Key is empty
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSetValueBoolean
(JNIEnv *env, jobject interfaceObject, jstring key, jboolean value);

/**
 * API for setting vector of boolean as attribute value
 *
 * @param key - Attribute key
 * @param value - Attribute value
 *
 * @throws RCSException - If Key is empty
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSetValueVectorOfBoolean
(JNIEnv *env, jobject interfaceObject, jstring key, jobject value);

/**
 * API for setting vector of vector of boolean as attribute value
 *
 * @param key - Attribute key
 * @param value - Attribute value
 *
 * @throws RCSException - If Key is empty
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSetValueVectorOfVectorOfBoolean
(JNIEnv *env, jobject interfaceObject, jstring key, jobject value);

/**
 * API for setting vector of vector of vector of boolean as attribute value
 *
 * @param key - Attribute key
 * @param value - Attribute value
 *
 * @throws RCSException - If Key is empty
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSetValueVectorOfVectorOfVectorOfBoolean
(JNIEnv *env, jobject interfaceObject, jstring key, jobject value);

/**
 * API for setting String as attribute value
 *
 * @param key - Attribute key
 * @param value - Attribute value
 *
 * @throws RCSException - If Key is empty
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSetValueString
(JNIEnv *env, jobject interfaceObject, jstring key, jstring value);

/**
 * API for setting Vector of String as attribute value
 *
 * @param key - Attribute key
 * @param value - Attribute value
 *
 * @throws RCSException - If Key is empty
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSetValueVectorOfString
(JNIEnv *env, jobject interfaceObject, jstring key, jobject value);

/**
 * API for setting Vector of Vector of String as attribute value
 *
 * @param key - Attribute key
 * @param value - Attribute value
 *
 * @throws RCSException - If Key is empty
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSetValueVectorOfVectorOfString
(JNIEnv *env, jobject interfaceObject, jstring key, jobject value);

/**
 * API for setting Vector of Vector of Vector of String as attribute value
 *
 * @param key - Attribute key
 * @param value - Attribute value
 *
 * @throws RCSException - If Key is empty
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSetValueVectorOfVectorOfVectorOfString
(JNIEnv *env, jobject interfaceObject, jstring key, jobject value);

/**
 * API for setting RCSResourceAttribute as attribute value
 *
 * @param key - Attribute key
 * @param value - Attribute value
 *
 * @throws RCSException - If Key is empty
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSetValueAttributes
(JNIEnv *env, jobject interfaceObject, jstring key, jobject value);


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
 * API for creating a new RCSResourceAttributes object and setting of Native handle
 */
JNIEXPORT void JNICALL Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_create
(JNIEnv *env, jobject interfaceObject);

/**
 *  API for deleting the RCSResourceAttributes object
 */
JNIEXPORT void JNICALL Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_dispose
(JNIEnv *env, jobject interfaceObject, jboolean jNeedsDelete);

#ifdef __cplusplus
}
#endif
#endif  //__JNI_RE_RCS_RESOURCE_ATTRIBUTES_H_

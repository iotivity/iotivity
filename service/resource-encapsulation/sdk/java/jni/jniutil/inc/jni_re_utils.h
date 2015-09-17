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
  * @file
  * This file contains the utility functions for JNI
  */

#ifndef __JNI_RE_UTILS_H_
#define __JNI_RE_UTILS_H_

#include "jni_re_jvm.h"
#include "RCSRemoteResourceObject.h"
#include "RCSResourceObject.h"

#include <map>
#include <list>
#include <jni.h>

using namespace OIC::Service;

/**
 * @brief Convert Native ResourceState to Integer
 */
int covertResourceStateToInt(ResourceState state);

/**
 * @brief Convert Native CacheState to Integer
 */
int convertCacheStateToInt(CacheState state);

/**
 * @brief Native Vector to Java list
 */
jobject convertStrVectorToJavaStrList(JNIEnv *env, std::vector<std::string> vector);

/**
 * @brief Convert Native ResourceState to Java ResourceState
 */
jobject convertNativeResourceStateToJavaResourceState(ResourceState nativeResourceState,
        JNIEnv *env);

/**
 * @brief Convert Java AutoNotifyPolicy to Native AutoNotifyPolicy
 */
RCSResourceObject::AutoNotifyPolicy convertJavaNotifyPolicyToNativePolicy( int policy);

/**
 * @brief Convert AutoNotifyPolicy to integer
 */
int  convertAutoNotifyPolicyToInt(RCSResourceObject::AutoNotifyPolicy policy);

/**
 *@brief Convert Java setRequestHandlerPolicy to Native setRequestHandlerPolicy
 */
RCSResourceObject::SetRequestHandlerPolicy convertJavasetRequestHandlerPolicyToNativePolicy(
    int policy);

/**
 *@brief Convert setRequestHandlerPolicy to int
 */
int convertSetRequestHandlerPolicyToInt( RCSResourceObject::SetRequestHandlerPolicy policy);

/**
 *@brief Convert int to OCEntityHandlerResult
 */
OCEntityHandlerResult convertIntToOCEntityHandlerResult(int result);

/**
 *@brief Convert java map to native map
 */
void convertJavaMapToParamsMap(JNIEnv *env, jobject hashMap,
                               std::map<std::string, std::string> &params);

/**
 *@brief Convert native string list to java list
 */
jobject convertStrListToJavaStrList(JNIEnv *env, std::list<std::string> &list);

/**
 *@brief Convert java Integer vector to native Integer vector
 */
std::vector<int> covertJavaIntVectorToNativeVector(JNIEnv *env, jobject javaVector);

/**
 *@brief Convert java  vector of Vector of Integer to native vector
 */
std::vector<std::vector<int>> convertJavaVectorOfVectorOfIntToNativeVector(JNIEnv *env,
                           jobject javaVector);

/**
 *@brief Convert java  vector of Vector of Vector of Integer to native vector
 */
std::vector<std::vector<std::vector<int>>> convertJavaVectorOfVectorOfVectorOfIntToNativeVector(
    JNIEnv *env, jobject javaVector);

/**
 *@brief Convert native Integer vector to Java Integer vector
 */
jobject convertNativeIntVectorToJavaVector(JNIEnv *env, std::vector<int> &nativeVector);

/**
 *@brief Convert native Vector of Vector of Integer to Java vector
 */
jobject convertNativeVectorOfVectorOfIntToJavaVector(JNIEnv *env,
        std::vector<std::vector<int>> &nativeVector);

/**
 *@brief Convert native Vector of Vector of Vector of Integer to Java vector
 */
jobject convertNativeVectorOfVectorOfVectorOfIntToJavaVector(JNIEnv *env,
        std::vector<std::vector<std::vector<int>>> &nativeVector);

/**
 *@brief Convert java Double vector to native vector
 */
std::vector<double> covertJavaDoubleVectorToNativeVector(JNIEnv *env, jobject javaVector);

/**
 *@brief Convert java vector of Vector of Double to native vector
 */
std::vector<std::vector<double>> convertJavaVectorOfVectorOfDoubleToNativeVector(JNIEnv *env,
                              jobject javaVector);

/**
 *@brief Convert java  vector of Vector of Vector of Double to native vector
 */
std::vector<std::vector<std::vector<double>>>
convertJavaVectorOfVectorOfVectorOfDoubleToNativeVector(JNIEnv *env, jobject javaVector);

/**
 *@brief Convert native Double vector to jave vector
 */
jobject convertNativeDoubleVectorToJavaVector(JNIEnv *env, std::vector<double> &nativeVector);

/**
 *@brief Convert native Vector of Vector of Double to Java vector
 */
jobject convertNativeVectorOfVectorOfDoubleToJavaVector(JNIEnv *env,
        std::vector<std::vector<double>> &nativeVector);

/**
 *@brief Convert native Vector of Vector of Vector of Double to Java vector
 */
jobject convertNativeVectorOfVectorOfVectorOfDoubleToJavaVector(JNIEnv *env,
        std::vector<std::vector<std::vector<double>>> &nativeVector);

/**
 *@brief Convert java Boolean vector to native vector
 */
std::vector<bool> covertJavaBooleanVectorToNativeVector(JNIEnv *env, jobject javaVector);

/**
 *@brief Convert java vector of Vector of boolean to native vector
 */
std::vector<std::vector<bool>> convertJavaVectorOfVectorOfBooleanToNativeVector(JNIEnv *env,
                            jobject javaVector);

/**
 *@brief Convert java  vector of Vector of Vector of Boolean to native vector
 */
std::vector<std::vector<std::vector<bool>>>
convertJavaVectorOfVectorOfVectorOfBooleanToNativeVector(JNIEnv *env, jobject javaVector);

/**
 *@brief Convert native Bool vector to Java vector
 */
jobject convertNativeBooleanVectorToJavaVector(JNIEnv *env, std::vector<bool> &nativeVector);

/**
 *@brief Convert native Vector of Vector of Boolean to Java vector
 */
jobject convertNativeVectorOfVectorOfBooleanToJavaVector(JNIEnv *env,
        std::vector<std::vector<bool>> &nativeVector);

/**
 *@brief Convert native Vector of Vector of Vector of Boolean to Java vector
 */
jobject convertNativeVectorOfVectorOfVectorOfBooleanToJavaVector(JNIEnv *env,
        std::vector<std::vector<std::vector<bool>>> &nativeVector);

/**
 *@brief Convert java String vector to native vector
 */
std::vector<std::string> covertJavaStringVectorToNativeVector(JNIEnv *env, jobject javaVector);

/**
 *@brief Convert java vector of Vector of String to native vector
 */
std::vector<std::vector<std::string>> convertJavaVectorOfVectorOfStringToNativeVector(JNIEnv *env,
                                   jobject javaVector);

/**
 *@brief Convert java  vector of Vector of Vector of String to native vector
 */
std::vector<std::vector<std::vector<std::string>>>
convertJavaVectorOfVectorOfVectorOfStringToNativeVector(JNIEnv *env, jobject javaVector);

/**
 *@brief Convert native String vector to Java vector
 */
jobject convertNativeStringVectorToJavaVector(JNIEnv *env, std::vector<std::string> &nativeVector);

/**
 *@brief Convert native Vector of Vector of String to Java vector
 */
jobject convertNativeVectorOfVectorOfStringToJavaVector(JNIEnv *env,
        std::vector<std::vector<std::string>> &nativeVector);

/**
 *@brief Convert native Vector of Vector of Vector of String to Java vector
 */
jobject convertNativeVectorOfVectorOfVectorOfStringToJavaVector(JNIEnv *env,
        std::vector<std::vector<std::vector<std::string>>> &nativeVector);

/**
 * @brief Throw the RCS exception to the upper layer
 */
void throwRCSException(JNIEnv *env, std::string reason);

/**
 * @class   ResourceStateWrapper
 * @brief   This class is JNI wrapper for Java ResourceState.
 *
 */
class ResourceStateWrapper
{
        ResourceStateWrapper() {}
    public :

        //Field IDs
        static jfieldID  ALIVE_ID;
        static jfieldID  REQUESTED_ID;
        static jfieldID  LOST_SIGNAL_ID;
        static jfieldID  DESTROYED_ID;
        static jfieldID  NONE_ID;

        //Method IDs
        static jmethodID ordinal_ID;
        static jmethodID toString_ID;
        static jmethodID valueOf_ID;

        static jclass clazz;

        /**
         *  @brief Get jobject for ALIVE ResourceState
         */
        static jobject getALIVE(JNIEnv *env);

        /**
         *  @brief Get jobject for REQUESTED ResourceState
         */
        static jobject getREQUESTED(JNIEnv *env);

        /**
         *  @brief Get jobject for SIGNAL ResourceState
         */
        static jobject getLOST_SIGNAL(JNIEnv *env);

        /**
         *  @brief Get jobject for DESTROYED ResourceState
         */
        static jobject getDESTROYED(JNIEnv *env);

        /**
         *  @brief Get jobject for NONE ResourceState
         */
        static jobject getNONE(JNIEnv *env);

        /**
         *  @brief Returns the ordinal value of Enum Object
         */
        static jint ordinal(JNIEnv *env, jobject enumObj);

        /**
         *  @brief Returns the name of Enum Object
         */
        static jstring toString(JNIEnv *env, jobject enumObj);

        /**
         *  @brief Returns Enum Object by name
         */
        static jobject valueOf(JNIEnv *env, jstring enumName);

        /**
         * @brief virtual destructor
         */
        virtual ~ResourceStateWrapper() {};
};
#endif //__JNI_RE_UTILS_H_

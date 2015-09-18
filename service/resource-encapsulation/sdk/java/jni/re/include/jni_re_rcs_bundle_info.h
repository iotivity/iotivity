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

/** @file   jni_re_rcs_bundle_info.h
 *
 * @brief This file contains the JniBundleInfo class
 *               & declaration of RCSBundleInfo APIs for JNI implementation
 */

#ifndef __JNI_RE_RCS_BUNDLE_INFO_H_
#define __JNI_RE_RCS_BUNDLE_INFO_H_

#include <jni.h>

#include <RCSBundleInfo.h>

using namespace OIC::Service;

/**
 * @class   JniBundleInfo
 * @brief   This class provides the API for getting the RCSBundleInfo pointer
 *
 */
class JniBundleInfo
{
    public:
        /**
         * @brief Get the RCSBundleInfo pointer
         *      internally get the native handle by calling REGetHandle
         */
        static RCSBundleInfo *getRCSBundleInfoPtr(JNIEnv *env, jobject thiz);
};

#ifdef __cplusplus
extern "C" {
#endif

/**
* API for getting the Id of the bundle
*
* @return jstring - Id of the bundle
*
*/
JNIEXPORT jstring JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSBundleInfo_nativeGetID
(JNIEnv *env, jobject jObject);

/**
* API for getting the path of the bundle
*
* @return jstring - path of the bundle
*
*/
JNIEXPORT jstring JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSBundleInfo_nativeGetPath
(JNIEnv *env, jobject jObject);

/**
* API for getting the Activator name for the bundle
*
* @return jstring - Name of the activator
*
*/
JNIEXPORT jstring JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSBundleInfo_nativeGetActivatorName
(JNIEnv *env, jobject jObject);

/**
* API for getting the library path for the bundle
*
* @return jstring - Library path  in string form
*
*/
JNIEXPORT jstring JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSBundleInfo_nativeGetLibraryPath
(JNIEnv *env, jobject jObject);

/**
* API for getting the version of the bundle
*
* @return jstring - version of the bundle
*
*/
JNIEXPORT jstring JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSBundleInfo_nativeGetVersion
(JNIEnv *env, jobject jObject);


/**
* API for setting the Id of the bundle
*
* @param id - Id of the bundle in string form
*
*/
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSBundleInfo_nativeSetID
(JNIEnv *env, jobject jObject, jstring id);

/**
* API for setting the path of the bundle
*
* @param path - path of the bundle in string form
*
*/
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSBundleInfo_nativeSetPath
(JNIEnv *env, jobject jObject, jstring path);

/**
* API for setting the Activator name for the bundle
*
* @param activator - Activator name in string form
*
*/
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSBundleInfo_nativeSetActivatorName
(JNIEnv *env, jobject jObject, jstring activatorname);

/**
* API for setting the library path for the bundle
*
* @param libpath - Library path in string form
*
*/
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSBundleInfo_nativeSetLibraryPath
(JNIEnv *env, jobject jObject, jstring libraryPath);

/**
* API for setting the version of the bundle
*
* @param version - version of the bundle in string form
*
*/
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSBundleInfo_nativeSetVersion
(JNIEnv *env, jobject jObject, jstring version);

#ifdef __cplusplus
}
#endif
#endif //__JNI_RE_RCS_BUNDLE_INFO_H_


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

/** @file   jni_re_rcs_resource_container.h
 *
 *   @brief  This file contains the declaration of RCSResourceContainer
 *    APIs for JNI implementation
 */

#ifndef __JNI_RE_RCS_RESOURCE_CONTAINER_H_
#define __JNI_RE_RCS_RESOURCE_CONTAINER_H_

#include <jni.h>

#include "jni_re_jvm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * API for starting the Container
 *
 * @details This API start the container with the provided Configuration file.
 *
 * @param configFile - configuration File that contains the Bundle/Bundles information.
 *
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceContainer_nativeStartContainer
(JNIEnv *env, jobject jObject, jstring configFile);


/**
* API for stopping the Container
*/
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceContainer_nativeStopContainer
(JNIEnv *env, jobject jObject);

/**
 * API for adding the bundle to the Container
 *
 * @param bundleId - Id of the Bundle
 * @param bundleUri - Uri of the bundle
 * @param bundlePath - Path of the bundle
 * @param params  - key-value pairs in string form for other Bundle parameters
 *
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceContainer_nativeAddBundle
(JNIEnv *env, jobject jObject, jstring bundleId, jstring bundleUri, jstring bundlePath,
 jobject params);


/**
* API for getting the list of all bundles in the container
*
* @return jobject -List of BundleInfo pointer each associated with a bundle
*
*/
JNIEXPORT jobject JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceContainer_nativeListBundles
(JNIEnv *env, jobject jObject);

/**
 * API for removing the bundle from the container
 *
 * @param bundleId - Id of the Bundle
 *
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceContainer_nativeRemoveBundle
(JNIEnv *env, jobject jObject, jstring bundleId);

/**
 * API for starting the bundle.
 *
 * @param bundleId - Id of the Bundle
 *
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceContainer_nativeStartBundle
(JNIEnv *env, jobject jObject, jstring bundleId);

/**
* API for Stopping the bundle
*
* @param bundleId - Id of the Bundle
*
*/
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceContainer_nativeStopBundle
(JNIEnv *env, jobject jObject, jstring bundleId);

/**
* API for adding the Resource configuration information to the bundle
*
* @param bundleId - Id of the Bundle
* @param resourceUri - URI of the resource
* @param params  - key-value pairs in string form for other Bundle parameters
*
*/
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceContainer_nativeAddResourceConfig
(JNIEnv *env, jobject jObject, jstring bundleId, jstring resourceUri, jobject params);

/**
* API for removing the Resource configuration information from the bundle
*
* @param bundleId - Id of the Bundle
* @param resourceUri - URI of the resource
*
*/
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceContainer_nativeRemoveResourceConfig
(JNIEnv *env, jobject jObject, jstring bundleId, jstring resourceUri);

/**
* API for getting the list of Bundle Resources
*
* @param bundleId - Id of the Bundle
*
*/
JNIEXPORT jobject JNICALL
Java_org_iotivity_ResourceEncapsulation_server_RCSResourceContainer_nativeListBundleResources
(JNIEnv *env, jobject jObject, jstring bundleId);

#ifdef __cplusplus
}
#endif
#endif //__JNI_RE_RCS_RESOURCE_CONTAINER_H_

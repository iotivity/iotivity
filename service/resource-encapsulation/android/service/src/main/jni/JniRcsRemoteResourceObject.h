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

#ifndef JNI_RE_RCS_REMOTE_RESOURCE_OBJECT_H_
#define JNI_RE_RCS_REMOTE_RESOURCE_OBJECT_H_

#include <jni.h>

class JNIEnvWrapper;

/**
 * Initialize rcs remote resource object
 */
void initRCSRemoteResourceObject(JNIEnvWrapper*);

/**
 * Clear rcs remote resource object
 */
void clearRCSRemoteResourceObject(JNIEnvWrapper*);

jobject newRemoteResourceObject(JNIEnvWrapper*);

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @class:     org_iotivity_service_client_RcsRemoteResourceObject
 * Method:     nativeIsMonitoring
 */
JNIEXPORT jboolean JNICALL
Java_org_iotivity_service_client_RcsRemoteResourceObject_nativeIsMonitoring
(JNIEnv*, jobject);

/**
 * @class:     org_iotivity_service_client_RcsRemoteResourceObject
 * Method:     nativeIsCaching
 */
JNIEXPORT jboolean JNICALL
Java_org_iotivity_service_client_RcsRemoteResourceObject_nativeIsCaching
(JNIEnv*, jobject);

/**
 * @class:     org_iotivity_service_client_RcsRemoteResourceObject
 * Method:     nativeIsObservable
 */
JNIEXPORT jboolean JNICALL
Java_org_iotivity_service_client_RcsRemoteResourceObject_nativeIsObservable
(JNIEnv*, jobject);

/**
 * @class:     org_iotivity_service_client_RcsRemoteResourceObject
 * Method:     nativeStartMonitoring
 */
JNIEXPORT void JNICALL
Java_org_iotivity_service_client_RcsRemoteResourceObject_nativeStartMonitoring
(JNIEnv*, jobject, jobject);

/**
 * @class:     org_iotivity_service_client_RcsRemoteResourceObject
 * Method:     nativeStopMonitoring
 */
JNIEXPORT void JNICALL
Java_org_iotivity_service_client_RcsRemoteResourceObject_nativeStopMonitoring
(JNIEnv*, jobject);

/**
 * @class:     org_iotivity_service_client_RcsRemoteResourceObject
 * Method:     nativeGetState
 */
JNIEXPORT jobject JNICALL
Java_org_iotivity_service_client_RcsRemoteResourceObject_nativeGetState
(JNIEnv*, jobject);

/**
 * @class:     org_iotivity_service_client_RcsRemoteResourceObject
 * Method:     nativeStartCaching
 */
JNIEXPORT void JNICALL
Java_org_iotivity_service_client_RcsRemoteResourceObject_nativeStartCaching
(JNIEnv*, jobject, jobject cacheUpdateListener);

/**
 * @class:     org_iotivity_service_client_RcsRemoteResourceObject
 * Method:     nativeStopCaching
 */
JNIEXPORT void JNICALL
Java_org_iotivity_service_client_RcsRemoteResourceObject_nativeStopCaching
(JNIEnv*, jobject);

/**
 * @class:     org_iotivity_service_client_RcsRemoteResourceObject
 * Method:     nativeGetCacheState
 */
JNIEXPORT jobject JNICALL
Java_org_iotivity_service_client_RcsRemoteResourceObject_nativeGetCacheState
(JNIEnv*, jobject);

/**
 * @class:     org_iotivity_service_client_RcsRemoteResourceObject
 * Method:     nativeIsCachedAvailable
 */
JNIEXPORT jboolean JNICALL
Java_org_iotivity_service_client_RcsRemoteResourceObject_nativeIsCachedAvailable
(JNIEnv*, jobject);

/**
 * @class:     org_iotivity_service_client_RcsRemoteResourceObject
 * Method:     nativeGetCachedAttributes
 */
JNIEXPORT jobject JNICALL
Java_org_iotivity_service_client_RcsRemoteResourceObject_nativeGetCachedAttributes
(JNIEnv*, jobject);

/**
 * @class:     org_iotivity_service_client_RcsRemoteResourceObject
 * Method:     nativeGetRemoteAttributes
 */
JNIEXPORT void JNICALL
Java_org_iotivity_service_client_RcsRemoteResourceObject_nativeGetRemoteAttributes
(JNIEnv*, jobject, jobject listener);

/**
 * @class:     org_iotivity_service_client_RcsRemoteResourceObject
 * Method:     nativeSetRemoteAttributes
 */
JNIEXPORT void JNICALL
Java_org_iotivity_service_client_RcsRemoteResourceObject_nativeSetRemoteAttributes
(JNIEnv*, jobject, jobject attrs, jobject listener);

/**
 * @class:     org_iotivity_service_client_RcsRemoteResourceObject
 * Method:     nativeGetUri
 */
JNIEXPORT jstring JNICALL
Java_org_iotivity_service_client_RcsRemoteResourceObject_nativeGetUri
(JNIEnv*, jobject);

/**
 * @class:     org_iotivity_service_client_RcsRemoteResourceObject
 * Method:     nativeGetAddress
 */
JNIEXPORT jstring JNICALL
Java_org_iotivity_service_client_RcsRemoteResourceObject_nativeGetAddress
(JNIEnv*, jobject);

/**
 * @class:     org_iotivity_service_client_RcsRemoteResourceObject
 * Method:     nativeGetTypes
 */
JNIEXPORT jobjectArray JNICALL
Java_org_iotivity_service_client_RcsRemoteResourceObject_nativeGetTypes
(JNIEnv*, jobject);

/**
 * @class:     org_iotivity_service_client_RcsRemoteResourceObject
 * Method:     nativeGetInterfaces
 */
JNIEXPORT jobjectArray JNICALL
Java_org_iotivity_service_client_RcsRemoteResourceObject_nativeGetInterfaces
(JNIEnv*, jobject);

#ifdef __cplusplus
}
#endif
#endif // JNI_RE_RCS_REMOTE_RESOURCE_OBJECT_H_

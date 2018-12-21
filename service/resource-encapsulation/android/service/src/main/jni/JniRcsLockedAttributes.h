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

#ifndef JNI_RCS_LOCKED_ATTRIBUTES_H_
#define JNI_RCS_LOCKED_ATTRIBUTES_H_

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @class:     org_iotivity_service_server_RcsLockedAttributes
 * Method:     nativeIsEmpty
 */
JNIEXPORT jboolean JNICALL
Java_org_iotivity_service_server_RcsLockedAttributes_nativeIsEmpty
(JNIEnv*, jclass, jobject);

/**
 * @class:     org_iotivity_service_server_RcsLockedAttributes
 * Method:     nativeSize
 */
JNIEXPORT jint JNICALL
Java_org_iotivity_service_server_RcsLockedAttributes_nativeSize
(JNIEnv*, jclass, jobject);

/**
 * @class:     org_iotivity_service_server_RcsLockedAttributes
 * Method:     nativeRemove
 */
JNIEXPORT jboolean JNICALL
Java_org_iotivity_service_server_RcsLockedAttributes_nativeRemove
(JNIEnv*, jclass, jobject, jstring keyObj);

/**
 * @class:     org_iotivity_service_server_RcsLockedAttributes
 * Method:     nativeClear
 */
JNIEXPORT void JNICALL
Java_org_iotivity_service_server_RcsLockedAttributes_nativeClear
(JNIEnv*, jclass, jobject);

/**
 * @class:     org_iotivity_service_server_RcsLockedAttributes
 * Method:     nativeContains
 */
JNIEXPORT jboolean JNICALL
Java_org_iotivity_service_server_RcsLockedAttributes_nativeContains
(JNIEnv*, jclass, jobject, jstring keyObj);

/**
 * @class:     org_iotivity_service_server_RcsLockedAttributes
 * Method:     nativeAddKeys
 */
JNIEXPORT void JNICALL
Java_org_iotivity_service_server_RcsLockedAttributes_nativeAddKeys
(JNIEnv*, jclass, jobject, jstring setObj);

/**
 * @class:     org_iotivity_service_server_RcsLockedAttributes
 * Method:     nativeAsJavaObject
 */
JNIEXPORT jobject JNICALL
Java_org_iotivity_service_server_RcsLockedAttributes_nativeAsJavaObject
(JNIEnv*, jclass, jobject, jstring keyObj);

/**
 * @class:     org_iotivity_service_server_RcsLockedAttributes
 * Method:     nativeApply
 */
JNIEXPORT void JNICALL
Java_org_iotivity_service_server_RcsLockedAttributes_nativeApply
(JNIEnv*, jclass, jobject, jstring cacheObj);

/**
 * @class:     org_iotivity_service_server_RcsLockedAttributes
 * Method:     nativeLock
 */
JNIEXPORT void JNICALL
Java_org_iotivity_service_server_RcsLockedAttributes_nativeLock
(JNIEnv*, jobject, jobject);

/**
 * @class:     org_iotivity_service_server_RcsLockedAttributes
 * Method:     nativeUnlock
 */
JNIEXPORT void JNICALL
Java_org_iotivity_service_server_RcsLockedAttributes_nativeUnlock
(JNIEnv*, jobject);

#ifdef __cplusplus
}
#endif

#endif  //JNI_RCS_LOCKED_ATTRIBUTES_H_

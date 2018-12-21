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

#ifndef JNI_RCS_RESOURCE_OBJECT_H
#define JNI_RCS_RESOURCE_OBJECT_H

#include <jni.h>

class JNIEnvWrapper;

/**
 * Initialize rcs resource object
 */
void initRCSResourceObject(JNIEnvWrapper*);

/**
 * Clear rcs resource object
 */
void clearRCSResourceObject(JNIEnvWrapper*);

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @class:     org_iotivity_service_server_RcsResponse
 * Method:     nativeGetDefaultErrorCode
 */
JNIEXPORT jint JNICALL
Java_org_iotivity_service_server_RcsResponse_nativeGetDefaultErrorCode
(JNIEnv*, jclass);

/**
 * @class:     org_iotivity_service_server_RcsResourceObject
 * Method:     nativeBuild
 */
JNIEXPORT jobject JNICALL
Java_org_iotivity_service_server_RcsResourceObject_nativeBuild
(JNIEnv*, jclass, jstring uri, jstring type, jstring interface, jboolean isObservable,
        jboolean isDiscovervable, jobject attrs);

/**
 * @class:     org_iotivity_service_server_RcsResourceObject
 * Method:     nativeSetAttribute
 */
JNIEXPORT void JNICALL
Java_org_iotivity_service_server_RcsResourceObject_nativeSetAttribute
(JNIEnv*, jobject, jstring key, jobject value);

/**
 * @class:     org_iotivity_service_server_RcsResourceObject
 * Method:     nativeGetAttributeValue
 */
JNIEXPORT jobject JNICALL
Java_org_iotivity_service_server_RcsResourceObject_nativeGetAttributeValue
(JNIEnv*, jobject, jstring key);

/**
 * @class:     org_iotivity_service_server_RcsResourceObject
 * Method:     nativeRemoveAttribute
 */
JNIEXPORT jboolean JNICALL
Java_org_iotivity_service_server_RcsResourceObject_nativeRemoveAttribute
(JNIEnv*, jobject, jstring key);

/**
 * @class:     org_iotivity_service_server_RcsResourceObject
 * Method:     nativeContainsAttribute
 */
JNIEXPORT jboolean JNICALL
Java_org_iotivity_service_server_RcsResourceObject_nativeContainsAttribute
(JNIEnv*, jobject, jstring key);

/**
 * @class:     org_iotivity_service_server_RcsResourceObject
 * Method:     nativeGetAttributes
 */
JNIEXPORT jobject JNICALL
Java_org_iotivity_service_server_RcsResourceObject_nativeGetAttributes
(JNIEnv*, jobject);

/**
 * @class:     org_iotivity_service_server_RcsResourceObject
 * Method:     nativeIsObservable
 */
JNIEXPORT jboolean JNICALL
Java_org_iotivity_service_server_RcsResourceObject_nativeIsObservable
(JNIEnv*, jobject);

/**
 * @class:     org_iotivity_service_server_RcsResourceObject
 * Method:     nativeIsDiscoverable
 */
JNIEXPORT jboolean JNICALL
Java_org_iotivity_service_server_RcsResourceObject_nativeIsDiscoverable
(JNIEnv*, jobject);

/**
 * @class:     org_iotivity_service_server_RcsResourceObject
 * Method:     nativeSetGetRequestHandler
 */
JNIEXPORT void JNICALL
Java_org_iotivity_service_server_RcsResourceObject_nativeSetGetRequestHandler
(JNIEnv*, jobject, jobject handler);

/**
 * @class:     org_iotivity_service_server_RcsResourceObject
 * Method:     nativeSetSetRequestHandler
 */
JNIEXPORT void JNICALL
Java_org_iotivity_service_server_RcsResourceObject_nativeSetSetRequestHandler
(JNIEnv*, jobject, jobject handler);

/**
 * @class:     org_iotivity_service_server_RcsResourceObject
 * Method:     nativeAddAttributeUpdatedListener
 */
JNIEXPORT void JNICALL
Java_org_iotivity_service_server_RcsResourceObject_nativeAddAttributeUpdatedListener
(JNIEnv*, jobject, jstring key, jobject listenr);

/**
 * @class:     org_iotivity_service_server_RcsResourceObject
 * Method:     nativeRemoveAttributeUpdatedListener
 */
JNIEXPORT jboolean JNICALL
Java_org_iotivity_service_server_RcsResourceObject_nativeRemoveAttributeUpdatedListener
(JNIEnv*, jobject, jstring key);

/**
 * @class:     org_iotivity_service_server_RcsResourceObject
 * Method:     nativeNotify
 */
JNIEXPORT void JNICALL Java_org_iotivity_service_server_RcsResourceObject_nativeNotify
(JNIEnv*, jobject);

/**
 * @class:     org_iotivity_service_server_RcsResourceObject
 * Method:     nativeSetAutoNotifyPolicy
 */
JNIEXPORT void JNICALL
Java_org_iotivity_service_server_RcsResourceObject_nativeSetAutoNotifyPolicy
(JNIEnv*, jobject, jobject policyObj);

/**
 * @class:     org_iotivity_service_server_RcsResourceObject
 * Method:     nativeGetAutoNotifyPolicy
 */
JNIEXPORT jobject JNICALL
Java_org_iotivity_service_server_RcsResourceObject_nativeGetAutoNotifyPolicy
(JNIEnv*, jobject);

/**
 * @class:     org_iotivity_service_server_RcsResourceObject
 * Method:     nativeSetSetRequestHandlerPolicy
 */
JNIEXPORT void JNICALL
Java_org_iotivity_service_server_RcsResourceObject_nativeSetSetRequestHandlerPolicy
(JNIEnv*, jobject, jobject policyObj);

/**
 * @class:     org_iotivity_service_server_RcsResourceObject
 * Method:     nativeGetSetRequestHandlerPolicy
 */
JNIEXPORT jobject JNICALL
Java_org_iotivity_service_server_RcsResourceObject_nativeGetSetRequestHandlerPolicy
(JNIEnv*, jobject);

#ifdef __cplusplus
}
#endif
#endif // JNI_RCS_RESOURCE_OBJECT_H



/******************************************************************
*
* Copyright 2016 Samsung Electronics All Rights Reserved.
*
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/

#include <jni.h>
/* Header for class org_iotivity_CAJni */

#ifndef _Included_org_iotivity_CAJni
#define _Included_org_iotivity_CAJni
#ifdef __cplusplus
extern "C" {
#endif
JNIEXPORT void JNICALL Java_org_iotivity_CAJni_setNativeResponseListener
  (JNIEnv *, jobject, jobject);
/*
 * Class:     org_iotivity_CAJni
 * Method:    initialize
 * Signature: (Landroid/content/Context;)V
 */
JNIEXPORT void JNICALL Java_org_iotivity_CAJni_initialize
  (JNIEnv *, jobject, jobject, jobject);

/*
 * Class:     org_iotivity_CAJni
 * Method:    terminate
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_org_iotivity_CAJni_terminate
  (JNIEnv *, jobject);

/*
 * Class:     org_iotivity_CAJni
 * Method:    tartListeningServer
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_org_iotivity_CAJni_startListeningServer
  (JNIEnv *, jobject);

/*
 * Class:     org_iotivity_CAJni
 * Method:    startDiscoveryServer
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_org_iotivity_CAJni_startDiscoveryServer
  (JNIEnv *, jobject);

/*
 * Class:     org_iotivity_CAJni
 * Method:    registerHandler
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_org_iotivity_CAJni_registerHandler
  (JNIEnv *, jobject);

JNIEXPORT void JNICALL Java_org_iotivity_CAJni_selectNetwork
  (JNIEnv *, jobject, jint);

/*
 * Class:     org_iotivity_CAJni
 * Method:    unSelectNetwork
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_org_iotivity_CAJni_unSelectNetwork
  (JNIEnv *, jobject, jint);

/*
 * Class:     org_iotivity_CAJni
 * Method:    getNetworkInfomation
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_org_iotivity_CAJni_getNetworkInfomation
  (JNIEnv *, jobject);

/*
 * Class:     org_iotivity_CAJni
 * Method:    handleRequestResponse
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_org_iotivity_CAJni_handleRequestResponse
  (JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif

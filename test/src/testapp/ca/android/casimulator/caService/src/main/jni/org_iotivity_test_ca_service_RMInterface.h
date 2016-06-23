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
/* Header for class org_iotivity_ca_service_RMInterface */

#ifndef _Included_org_iotivity_test_ca_service_RMInterface
#define _Included_org_iotivity_test_ca_service_RMInterface
#ifdef __cplusplus
extern "C" {
#endif
JNIEXPORT void JNICALL Java_org_iotivity_test_ca_service_RMInterface_setNativeResponseListener
  (JNIEnv *, jobject, jobject);
/*
 * Class:     org_iotivity_ca_service_RMInterface
 * Method:    RMInitialize
 * Signature: (Landroid/content/Context;)V
 */
JNIEXPORT void JNICALL Java_org_iotivity_test_ca_service_RMInterface_RMInitialize
  (JNIEnv *, jobject, jobject, jobject);

/*
 * Class:     org_iotivity_ca_service_RMInterface
 * Method:    RMTerminate
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_org_iotivity_test_ca_service_RMInterface_RMTerminate
  (JNIEnv *, jobject);

/*
 * Class:     org_iotivity_ca_service_RMInterface
 * Method:    RMStartListeningServer
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_org_iotivity_test_ca_service_RMInterface_RMStartListeningServer
  (JNIEnv *, jobject);

/*
 * Class:     org_iotivity_ca_service_RMInterface
 * Method:    RMStartDiscoveryServer
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_org_iotivity_test_ca_service_RMInterface_RMStartDiscoveryServer
  (JNIEnv *, jobject);

/*
 * Class:     org_iotivity_ca_service_RMInterface
 * Method:    RMRegisterHandler
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_org_iotivity_test_ca_service_RMInterface_RMRegisterHandler
  (JNIEnv *, jobject);

JNIEXPORT void JNICALL Java_org_iotivity_test_ca_service_RMInterface_RMSelectNetwork
  (JNIEnv *, jobject, jint);

/*
 * Class:     org_iotivity_ca_service_RMInterface
 * Method:    RMUnSelectNetwork
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_org_iotivity_test_ca_service_RMInterface_RMUnSelectNetwork
  (JNIEnv *, jobject, jint);

/*
 * Class:     org_iotivity_ca_service_RMInterface
 * Method:    RMGetNetworkInfomation
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_org_iotivity_test_ca_service_RMInterface_RMGetNetworkInfomation
  (JNIEnv *, jobject);

/*
 * Class:     org_iotivity_ca_service_RMInterface
 * Method:    RMHandleRequestResponse
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_org_iotivity_test_ca_service_RMInterface_RMHandleRequestResponse
  (JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif

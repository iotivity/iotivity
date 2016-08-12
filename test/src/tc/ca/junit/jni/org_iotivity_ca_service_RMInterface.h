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

#ifndef _Included_org_iotivity_service_ca_test_helper_RMInterface
#define _Included_org_iotivity_service_ca_test_helper_RMInterface
#ifdef __cplusplus
extern "C" {
#endif
JNIEXPORT void JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_setNativeResponseListener
  (JNIEnv *, jobject, jobject);

JNIEXPORT bool JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_setContext
  (JNIEnv *, jobject, jobject, jobject);

JNIEXPORT bool JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_setupTestCase
  (JNIEnv *, jobject, jint, jint, jint);

JNIEXPORT bool JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_initialize
  (JNIEnv *, jobject);

JNIEXPORT void JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_terminate
  (JNIEnv *, jobject);

JNIEXPORT bool JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_startListeningServer
  (JNIEnv *, jobject);

JNIEXPORT bool JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_startDiscoveryServer
  (JNIEnv *, jobject);

JNIEXPORT bool JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_registerClientHandler
  (JNIEnv *, jobject);

JNIEXPORT bool JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_registerServerHandler
  (JNIEnv *, jobject);

JNIEXPORT bool JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_registerCustomClientHandler
  (JNIEnv *, jobject, jint, jint);

JNIEXPORT bool JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_registerCustomServerHandler
  (JNIEnv *, jobject, jint, jint);

JNIEXPORT bool JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_createEndpoint
  (JNIEnv *, jobject);

JNIEXPORT bool JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_destroyEndpoint
  (JNIEnv *, jobject);

JNIEXPORT bool JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_generateToken
  (JNIEnv *, jobject);

JNIEXPORT bool JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_destroyToken
  (JNIEnv *, jobject);

JNIEXPORT bool JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_sendRequest
  (JNIEnv *, jobject, jint, jint, jint, jint, jint, jboolean);

JNIEXPORT bool JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_sendRequestToAll
  (JNIEnv *, jobject, jint, jint, jint, jint, jint);

JNIEXPORT bool JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_sendResponse
  (JNIEnv *, jobject, jint, jint, jint, jint, jint);

JNIEXPORT bool JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_selectNetwork
  (JNIEnv *, jobject);

JNIEXPORT bool JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_selectCustomNetwork
  (JNIEnv *, jobject, jint);

JNIEXPORT bool JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_unSelectNetwork
  (JNIEnv *, jobject);

JNIEXPORT bool JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_unSelectCustomNetwork
  (JNIEnv *, jobject, jint);

JNIEXPORT bool JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_getNetworkInfomation
  (JNIEnv *, jobject, jboolean);

JNIEXPORT bool JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_handleRequestResponse
  (JNIEnv *, jobject);

JNIEXPORT bool JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_initNetwork
  (JNIEnv *, jobject);

JNIEXPORT bool JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_setDtls
  (JNIEnv *, jobject);

JNIEXPORT bool JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_initServerNetwork
  (JNIEnv *, jobject);

JNIEXPORT bool JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_initClientNetwork
  (JNIEnv *, jobject);

JNIEXPORT bool JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_initServerWithoutNetwork
  (JNIEnv *, jobject);

JNIEXPORT bool JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_initClientWithoutNetwork
  (JNIEnv *, jobject);

JNIEXPORT bool JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_checkReceiveCount
  (JNIEnv *, jobject, jint);

JNIEXPORT void JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_setNetwork
  (JNIEnv *, jobject, jint);

JNIEXPORT void JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_setRemoteIp
  (JNIEnv *, jobject, jstring);

JNIEXPORT void JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_setRemoteAddressSet
  (JNIEnv *, jobject, jobjectArray);

JNIEXPORT bool JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_sendConfigurationRequest
  (JNIEnv *, jobject, jint, jint, jint);

JNIEXPORT bool JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_startListeningServerCustom
  (JNIEnv *, jobject, jint);
  
JNIEXPORT bool JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_stopListeningServer(JNIEnv *, jobject, jint);

JNIEXPORT bool JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_generateCustomToken
  (JNIEnv *, jobject, jint, jint);

JNIEXPORT bool JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_selectNetworkWithExpectedResult
  (JNIEnv *, jobject, jlong, jint);

JNIEXPORT bool JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_unSelectNetworkWithExpectedResult
  (JNIEnv *, jobject, jlong, jint);

JNIEXPORT bool JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_startDiscoveryWithExpectedResult
  (JNIEnv *, jobject, jint);

JNIEXPORT bool JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_getNetworkInfomationWithExpectedResult
  (JNIEnv *, jobject, jint, jint);

JNIEXPORT bool JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_createEndpointWithExpectedResult
  (JNIEnv *, jobject, jint, jint);

JNIEXPORT bool JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_destroyEndpointInvalidEndpoint
  (JNIEnv *, jobject);

JNIEXPORT bool JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_generateTokenAndDestroyToken
  (JNIEnv *, jobject);

JNIEXPORT bool JNICALL Java_org_iotivity_service_ca_test_helper_RMInterface_establishConnectionWithServer
  (JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif

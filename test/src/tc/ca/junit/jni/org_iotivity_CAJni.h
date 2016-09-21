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

#ifndef _Included_org_iotivity_CAJni
#define _Included_org_iotivity_CAJni
#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT bool JNICALL Java_org_iotivity_CAJni_setContext
  (JNIEnv *, jclass, jclass, jclass);

JNIEXPORT bool JNICALL Java_org_iotivity_CAJni_readConfig
  (JNIEnv *, jclass);

JNIEXPORT bool JNICALL Java_org_iotivity_CAJni_setupTestCase
  (JNIEnv *, jclass, jint, jint, jint);

JNIEXPORT bool JNICALL Java_org_iotivity_CAJni_initialize
  (JNIEnv *, jclass);

JNIEXPORT void JNICALL Java_org_iotivity_CAJni_terminate
  (JNIEnv *, jclass);

JNIEXPORT bool JNICALL Java_org_iotivity_CAJni_setDtls
  (JNIEnv *, jclass);

JNIEXPORT bool JNICALL Java_org_iotivity_CAJni_registerHandler
  (JNIEnv *, jclass, jint, jint);

JNIEXPORT bool JNICALL Java_org_iotivity_CAJni_selectNetwork
  (JNIEnv *, jclass);

JNIEXPORT bool JNICALL Java_org_iotivity_CAJni_selectCustomNetwork
  (JNIEnv *, jclass, jlong, jint);

JNIEXPORT bool JNICALL Java_org_iotivity_CAJni_unSelectNetwork
  (JNIEnv *, jclass);

JNIEXPORT bool JNICALL Java_org_iotivity_CAJni_unSelectCustomNetwork
  (JNIEnv *, jclass, jlong, jint);

JNIEXPORT bool JNICALL Java_org_iotivity_CAJni_startListeningServer
  (JNIEnv *, jclass, jint);

JNIEXPORT bool JNICALL Java_org_iotivity_CAJni_stopListeningServer
  (JNIEnv *, jclass, jint);

JNIEXPORT bool JNICALL Java_org_iotivity_CAJni_startDiscoveryServer
  (JNIEnv *, jclass, jint);

JNIEXPORT bool JNICALL Java_org_iotivity_CAJni_generateToken
  (JNIEnv *, jclass, jint tokenType, jint tokenLength, jint expectedResult);

JNIEXPORT bool JNICALL Java_org_iotivity_CAJni_createEndpoint
  (JNIEnv *, jclass, jint, jint);

JNIEXPORT bool JNICALL Java_org_iotivity_CAJni_destroyEndpoint
  (JNIEnv *, jclass, jint);

JNIEXPORT bool JNICALL Java_org_iotivity_CAJni_getNetworkInfomation
  (JNIEnv *, jclass, jint, jint, jint);

JNIEXPORT bool JNICALL Java_org_iotivity_CAJni_handleRequestResponse
  (JNIEnv *, jclass);

JNIEXPORT bool JNICALL Java_org_iotivity_CAJni_initNetwork
  (JNIEnv *, jclass);

JNIEXPORT bool JNICALL Java_org_iotivity_CAJni_initServerNetwork
  (JNIEnv *, jclass);

JNIEXPORT bool JNICALL Java_org_iotivity_CAJni_initClientNetwork
  (JNIEnv *, jclass);

JNIEXPORT bool JNICALL Java_org_iotivity_CAJni_checkReceiveCount
  (JNIEnv *, jclass, jint);


JNIEXPORT bool JNICALL Java_org_iotivity_CAJni_establishConnectionWithServer
  (JNIEnv *, jclass);

JNIEXPORT bool JNICALL Java_org_iotivity_CAJni_sendRequest
  (JNIEnv *, jclass, jint, jint, jint, jint, jint, jboolean);

JNIEXPORT bool JNICALL Java_org_iotivity_CAJni_sendRequestToAll
  (JNIEnv *, jclass, jint, jint, jint, jint, jint);

JNIEXPORT bool JNICALL Java_org_iotivity_CAJni_sendResponse
  (JNIEnv *, jclass, jint, jint, jint, jint, jint);

JNIEXPORT bool JNICALL Java_org_iotivity_CAJni_sendConfigurationRequest
  (JNIEnv *, jclass, jint, jint, jint);

#ifdef __cplusplus
}
#endif
#endif

//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


#ifndef RESOURCECOORDINATOR_JNI_H_
#define RESOURCECOORDINATOR_JNI_H_

#include <jni.h>
#include <android/log.h>
#include <boost/lexical_cast.hpp>
#include <thread>
#include <atomic>
#include <unistd.h>
#define PRINT_LOG(strError) __android_log_print(ANDROID_LOG_ERROR, "[HM_JNI]", "%s:%d %s", __PRETTY_FUNCTION__, __LINE__, strError)

#define TAG "OIC-JNI"

#define JNI_CURRENT_VERSION JNI_VERSION_1_6
#define int OCSTACK_OK = 0
#define int OCSTACK_ERROR = 255
#define int RESOURCEHOSTING_DO_NOT_THREADRUNNING = -2

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Class:     org_iotivity_service_resourcehostingsampleapp_ResourceHosting
 * Method:    OICCoordinatorStart
 * Signature: ()V
 */
JNIEXPORT jint JNICALL Java_com_example_resourcehostingsampleapp_ResourceHosting_OICCoordinatorStart
(JNIEnv *, jobject);
/*
 * @Class:     org_iotivity_service_resourcehostingsampleapp_ResourceHosting
 * @Method:    OICCoordinatorStop
 * @Signature: ()V
 */

JNIEXPORT jint JNICALL Java_com_example_resourcehostingsampleapp_ResourceHosting_OICCoordinatorStop
(JNIEnv *, jobject);
/*
 * Class:     org_iotivity_service_resourcehostingsampleapp_ResourceHosting
 * Method:    ResourceHostingInit
 * Signature: ()V
 */
JNIEXPORT jint JNICALL Java_com_example_resourcehostingsampleapp_ResourceHosting_ResourceHostingInit
(JNIEnv *env, jobject obj,jstring j_addr);
/*
 * Class:     org_iotivity_service_resourcehostingsampleapp_ResourceHosting
 * Method:    ResourceHostingTerminate
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT jint JNICALL Java_com_example_resourcehostingsampleapp_ResourceHosting_ResourceHostingTerminate
(JNIEnv *env, jobject obj);

#ifdef __cplusplus
}
#endif

#endif /* RESOURCECOORDINATOR_JNI_H_ */

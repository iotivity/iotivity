/* ****************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
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

 #include "JniOcStack.h"
 #include "JniOnPublishResourceListener.h"
 #include "JniOnDeleteResourceListener.h"

#include <mutex>

#ifndef _Included_org_iotivity_base_RDClient
#define _Included_org_iotivity_base_RDClient

void RemoveOnPublishResourceListener(JNIEnv* env, jobject jListener);
JniOnPublishResourceListener* AddOnPublishResourceListener(JNIEnv* env, jobject jListener);

void RemoveOnDeleteResourceListener(JNIEnv* env, jobject jListener);
JniOnDeleteResourceListener* AddOnDeleteResourceListener(JNIEnv* env, jobject jListener);

std::map<jobject, std::pair<JniOnPublishResourceListener*, int>> onPublishResourceListenerMap;
std::map<jobject, std::pair<JniOnDeleteResourceListener*, int>> onDeleteResourceListenerMap;

std::mutex publishResourceListenerMapLock;
std::mutex deleteResourceListenerMapLock;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Class:     org_iotivity_base_OcRDClient
 * Method:    publishResourceToRD0
 * Signature: (Ljava/lang/String;ILorg/iotivity/base/OcRDClient/OnPublishResourceListener;I)V
 */
JNIEXPORT void JNICALL Java_org_iotivity_base_OcRDClient_publishResourceToRD0
    (JNIEnv *, jclass, jstring, jint, jobject, jint);

/*
 * Class:     org_iotivity_base_OcRDClient
 * Method:    publishResourceToRD1
 * Signature: (Ljava/lang/String;I[Lorg/iotivity/base/OcResourceHandle
 *            ;Lorg/iotivity/base/OcRDClient/OnPublishResourceListener;I)V
 */
JNIEXPORT void JNICALL Java_org_iotivity_base_OcRDClient_publishResourceToRD1
    (JNIEnv *, jclass, jstring, jint, jobjectArray, jobject, jint);

/*
 * Class:     org_iotivity_base_OcRDClient
 * Method:    deleteResourceFromRD0
 * Signature: (Ljava/lang/String;ILorg/iotivity/base/OcRDClient/OnDeleteResourceListener;I)V
 */
JNIEXPORT void JNICALL Java_org_iotivity_base_OcRDClient_deleteResourceFromRD0
    (JNIEnv *, jclass, jstring, jint, jobject, jint);

/*
 * Class:     org_iotivity_base_OcRDClient
 * Method:    deleteResourceFromRD1
 * Signature: (Ljava/lang/String;I[Lorg/iotivity/base/OcResourceHandle;Lorg/iotivity/base/OcRDClient/OnDeleteResourceListener;I)V
 */
JNIEXPORT void JNICALL Java_org_iotivity_base_OcRDClient_deleteResourceFromRD1
    (JNIEnv *, jclass, jstring, jint, jobjectArray, jobject, jint);

#ifdef __cplusplus
}
#endif
#endif

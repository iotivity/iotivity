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

#ifndef SIMULATOR_MANAGER_JNI_H_
#define SIMULATOR_MANAGER_JNI_H_

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL
Java_org_iotivity_simulator_SimulatorManagerNativeInterface_initialize
(JNIEnv *env, jclass object);

JNIEXPORT void JNICALL
Java_org_iotivity_simulator_SimulatorManagerNativeInterface_terminate
(JNIEnv *env, jclass object);

JNIEXPORT jobject JNICALL
Java_org_iotivity_simulator_SimulatorManagerNativeInterface_createResource
(JNIEnv *env, jclass object, jstring jConfigPath);

JNIEXPORT jobjectArray JNICALL
Java_org_iotivity_simulator_SimulatorManagerNativeInterface_createResources
(JNIEnv *env, jclass object, jstring jConfigPath, jint count);

JNIEXPORT jobjectArray JNICALL
Java_org_iotivity_simulator_SimulatorManageNativeInterfacer_getResources
(JNIEnv *env, jclass object);

JNIEXPORT void JNICALL
Java_org_iotivity_simulator_SimulatorManagerNativeInterface_deleteResource
(JNIEnv *env, jclass object, jobject jResource);

JNIEXPORT void JNICALL
Java_org_iotivity_simulator_SimulatorManagerNativeInterface_deleteResources
(JNIEnv *env, jclass object, jstring resourceType);

JNIEXPORT void JNICALL
Java_org_iotivity_simulator_SimulatorManagerNativeInterface_setLogger
(JNIEnv *env, jclass object, jobject logger);

#ifdef __cplusplus
}
#endif

#endif //SIMULATOR_MANAGER_JNI_H_

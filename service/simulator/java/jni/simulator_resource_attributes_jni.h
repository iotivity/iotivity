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

#ifndef SIMULATOR_RESOURCE_ATTRIBUTE_JNI_H_
#define SIMULATOR_RESOURCE_ATTRIBUTE_JNI_H_

#include <jni.h>
#include "simulator_resource_model.h"

class JniSimulatorResourceAttribute
{
    public:
        static jobject toJava(JNIEnv *env, jlong resource);
};

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL
Java_org_iotivity_simulator_SimulatorResourceAttribute_create
(JNIEnv *, jobject, jstring);

JNIEXPORT void JNICALL
Java_org_iotivity_simulator_SimulatorResourceAttribute_dispose
(JNIEnv *, jobject);

JNIEXPORT int JNICALL
Java_org_iotivity_simulator_SimulatorResourceAttribute_allowedValuesSize
(JNIEnv *, jobject);

JNIEXPORT jstring JNICALL
Java_org_iotivity_simulator_SimulatorResourceAttribute_valueToString
(JNIEnv *, jobject);

JNIEXPORT jstring JNICALL
Java_org_iotivity_simulator_SimulatorResourceAttribute_allowedValuesToString
(JNIEnv *, jobject);

JNIEXPORT jstring JNICALL
Java_org_iotivity_simulator_SimulatorResourceAttribute_getName
(JNIEnv *, jobject);

JNIEXPORT jobject JNICALL
Java_org_iotivity_simulator_SimulatorResourceAttribute_getValue
(JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif

#endif //SIMULATOR_RESOURCE_ATTRIBUTE_JNI_H_

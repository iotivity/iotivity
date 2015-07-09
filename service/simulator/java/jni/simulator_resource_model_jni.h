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

#ifndef SIMULATOR_RESOURCE_MODEL_JNI_H_
#define SIMULATOR_RESOURCE_MODEL_JNI_H_

#include <jni.h>
#include "simulator_resource_model.h"

class JniSimulatorResourceModel
{

    public:
        JniSimulatorResourceModel(SimulatorResourceModel);

        static jobject toJava(JNIEnv *, jlong);
        static bool getResourceModel(JNIEnv *env, jobject thiz, SimulatorResourceModel &resourceModel);
    private:
        SimulatorResourceModel m_resourceModel;
};


#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jint JNICALL
Java_org_iotivity_simulator_SimulatorResourceModel_size
(JNIEnv *, jobject);

JNIEXPORT jobject JNICALL
Java_org_iotivity_simulator_SimulatorResourceModel_getAttributes
(JNIEnv *, jobject);

JNIEXPORT jobject JNICALL
Java_org_iotivity_simulator_SimulatorResourceModel_getAttribute
(JNIEnv *, jobject, jstring);

JNIEXPORT jobjectArray JNICALL
Java_org_iotivity_simulator_SimulatorResourceModel_getAllowedValues
(JNIEnv *, jobject, jstring);

JNIEXPORT void JNICALL
Java_org_iotivity_simulator_SimulatorResourceModel_dispose
(JNIEnv *, jobject);


#ifdef __cplusplus
}
#endif

#endif

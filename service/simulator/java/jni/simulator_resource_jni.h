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

#ifndef SIMULATOR_RESOURCE_JNI_H_
#define SIMULATOR_RESOURCE_JNI_H_

#include <jni.h>
#include "simulator_resource.h"

class JniSimulatorResource
{
    public:
        JniSimulatorResource(SimulatorResourcePtr &resource);

        static jobject toJava(JNIEnv *env, jlong resource);
        static void setUri(JNIEnv *env, jobject jobj, const std::string &uri);
        static void setResourceType(JNIEnv *env, jobject jobj, const std::string &resourceType);
        static void setResourceName(JNIEnv *env, jobject jobj, const std::string &name);
        static void setInterfaceType(JNIEnv *env, jobject jobject, const std::string &interfaceType);
        static SimulatorResourcePtr getJniSimulatorResourcePtr(JNIEnv *env, jobject thiz);
    private:
        SimulatorResourcePtr m_sharedResource;
};


#ifdef __cplusplus
extern "C" {

JNIEXPORT jobject JNICALL
Java_org_iotivity_simulator_SimulatorResourceServer_getModel
(JNIEnv *, jobject);

JNIEXPORT void JNICALL
Java_org_iotivity_simulator_SimulatorResourceServer_updateAttributeFromAllowedValues
(JNIEnv *, jobject, jstring, jint);

JNIEXPORT void JNICALL
Java_org_iotivity_simulator_SimulatorResourceServer_setRange
(JNIEnv *, jobject, jstring, jint, jint);

JNIEXPORT void JNICALL
Java_org_iotivity_simulator_SimulatorResourceServer_addAttributeInteger
(JNIEnv *, jobject, jstring, jint);

JNIEXPORT void JNICALL
Java_org_iotivity_simulator_SimulatorResourceServer_addAttributeDouble
(JNIEnv *, jobject, jstring, jdouble);

JNIEXPORT void JNICALL
Java_org_iotivity_simulator_SimulatorResourceServer_addAttributeBoolean
(JNIEnv *, jobject, jstring, jboolean);

JNIEXPORT void JNICALL
Java_org_iotivity_simulator_SimulatorResourceServer_addAttributeStringN
(JNIEnv *, jobject, jstring, jstring);

JNIEXPORT void JNICALL
Java_org_iotivity_simulator_SimulatorResourceServer_updateAttributeInteger
(JNIEnv *, jobject, jstring, jint);

JNIEXPORT void JNICALL
Java_org_iotivity_simulator_SimulatorResourceServer_updateAttributeDouble
(JNIEnv *, jobject, jstring, jdouble);

JNIEXPORT void JNICALL
Java_org_iotivity_simulator_SimulatorResourceServer_updateAttributeBoolean
(JNIEnv *, jobject, jstring, jboolean);

JNIEXPORT void JNICALL
Java_org_iotivity_simulator_SimulatorResourceServer_updateAttributeStringN
(JNIEnv *, jobject, jstring, jstring);

JNIEXPORT void JNICALL
Java_org_iotivity_simulator_SimulatorResourceServer_setAllowedValuesInteger
(JNIEnv *, jobject, jstring, jobject);

JNIEXPORT void JNICALL
Java_org_iotivity_simulator_SimulatorResourceServer_setAllowedValuesDouble
(JNIEnv *, jobject, jstring, jobject);

JNIEXPORT void JNICALL
Java_org_iotivity_simulator_SimulatorResourceServer_setAllowedValuesStringN
(JNIEnv *, jobject, jstring, jobject);

JNIEXPORT jint JNICALL
Java_org_iotivity_simulator_SimulatorResourceServer_startResourceAutomation
(JNIEnv *, jobject);

JNIEXPORT jint JNICALL
Java_org_iotivity_simulator_SimulatorResourceServer_startAttributeAutomation
(JNIEnv *, jobject, jstring);

JNIEXPORT void JNICALL
Java_org_iotivity_simulator_SimulatorResourceServer_startAutomation
(JNIEnv *, jobject, jint);

JNIEXPORT void JNICALL
Java_org_iotivity_simulator_SimulatorResourceServer_removeAttribute
(JNIEnv *, jobject, jstring);

JNIEXPORT void JNICALL
Java_org_iotivity_simulator_SimulatorResourceServer_dispose
(JNIEnv *, jobject);

}
#endif
#endif //SIMULATOR_RESOURCE_JNI_H_

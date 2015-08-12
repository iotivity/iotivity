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

#ifndef SIMULATOR_REMOTE_RESOURCE_JNI_H_
#define SIMULATOR_REMOTE_RESOURCE_JNI_H_

#include <jni.h>
#include "simulator_remote_resource.h"

class JniSimulatorRemoteResource
{
    public:
        JniSimulatorRemoteResource(SimulatorRemoteResourcePtr &resource)
        : m_resource(resource) {};
        static SimulatorRemoteResourcePtr getResourceHandle(JNIEnv *env, jobject object);
    private:
        SimulatorRemoteResourcePtr m_resource;
};

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jint JNICALL
Java_org_oic_simulator_SimulatorRemoteResource_observe
(JNIEnv *env, jobject thiz, jint observeType, jobject jQueryParamsMap, jobject jListener);

JNIEXPORT jint JNICALL
Java_org_oic_simulator_SimulatorRemoteResource_cancelObserve
(JNIEnv *env, jobject thiz);

JNIEXPORT jint JNICALL
Java_org_oic_simulator_SimulatorRemoteResource_get
(JNIEnv *env, jobject thiz, jstring jResourceType, jstring jResourceInterface,
 jobject jQueryParamsMap, jobject jListener);

JNIEXPORT jint JNICALL
Java_org_oic_simulator_SimulatorRemoteResource_put
(JNIEnv *env, jobject thiz, jstring jResourceType, jstring jResourceInterface,
 jobject jRepresentation, jobject jQueryParamsMap, jobject jListener);

JNIEXPORT jint JNICALL
Java_org_oic_simulator_SimulatorRemoteResource_post
(JNIEnv *env, jobject thiz, jstring jResourceType, jstring jResourceInterface,
 jobject jRepresentation, jobject jQueryParamsMap, jobject jListener);


#ifdef __cplusplus
}
#endif

#endif


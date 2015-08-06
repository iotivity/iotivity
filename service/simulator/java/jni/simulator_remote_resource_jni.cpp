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

#include "simulator_remote_resource_jni.h"
#include "simulator_common_jni.h"

JniSimulatorRemoteResource::JniSimulatorRemoteResource
(SimulatorRemoteResourcePtr &resource) : m_sharedResource(resource)
{}

JniSimulatorRemoteResource *JniSimulatorRemoteResource::getJniSimulatorResourcePtr(JNIEnv *env,
        jobject thiz)
{
    JniSimulatorRemoteResource *resource = GetHandle<JniSimulatorRemoteResource>(env, thiz);
    if (env->ExceptionCheck())
    {
        std::cout << "Failed to get native handle from SimulatorRemoteResource";
    }
    if (!resource)
    {
        return nullptr;
    }
    return resource;
}

std::string JniSimulatorRemoteResource::getURI()
{
    return m_sharedResource->getURI();
}


JNIEXPORT jstring JNICALL
Java_org_iotivity_simulator_SimulatorRemoteResource_getURI
(JNIEnv *env, jobject thiz)
{
    std::cout << "SimulatorRemoteResource_getURI Entry";
    JniSimulatorRemoteResource *resource = JniSimulatorRemoteResource::getJniSimulatorResourcePtr(env,
                                           thiz);

    if (!resource)
        return nullptr;

    return env->NewStringUTF(resource->getURI().c_str());
}
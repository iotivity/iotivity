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

#ifndef SIMULATOR_RESOURCE_UTILS_JNI_H_
#define SIMULATOR_RESOURCE_UTILS_JNI_H_

#include <jni.h>
#include "simulator_single_resource.h"
#include "simulator_collection_resource.h"
#include "simulator_remote_resource.h"

jobject createSingleResource(JNIEnv *env, SimulatorResourceSP singleResource);
jobject createCollectionResource(JNIEnv *env, SimulatorResourceSP collectionResource);
jobject createSimulatorResource(JNIEnv *env, SimulatorResourceSP resource);
jobject createSimulatorResourceVector(JNIEnv *env, std::vector<SimulatorResourceSP> &resources);
jobject createSimulatorRemoteResource(JNIEnv *env, SimulatorRemoteResourceSP &remoteResource);

#endif

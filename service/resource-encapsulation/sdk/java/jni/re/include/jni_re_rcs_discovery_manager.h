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

/** @file   jni_re_rcs_discovery_manager.h
 *
 *   @brief  This file contains the declaration of Discover Resource API
 *               for JNI implementation
 */

#ifndef __JNI_RE_RCS_DISCOVERY_MANAGER_H_
#define __JNI_RE_RCS_DISCOVERY_MANAGER_H_

#include <stdio.h>
#include <string>
#include <mutex>
#include <map>
#include <jni.h>
#include "jni_re_discover_resource_listener.h"

#define LOG_TAG "JNIDM"

/**
* @brief API for adding listener to discover resource listener Map.
*/
JniDiscoverResourceListener *AddDiscoverResourceListener(JNIEnv *env, jobject jListener);

/**
* @brief API for removing listener from discover resource listener Map.
*/
void RemoveDiscoverResourceListener(JNIEnv *env, jobject jListener);

/**
* @brief discover resource listener Map.
*/
std::map<jobject, std::pair<JniDiscoverResourceListener *, int>> discoverResourceListenerMap;

/**
 *  Mutex for thread synchronization
 */
std::mutex discoverResourceMapLock;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * API for discovering the resource of interest in the network
 *
 * @param address - address
 * @param typeofDiscovery - multicast or unicast
 * @param relativeURI - relative URI for the resource of interest
 * @param resourceType - resource type
 * @param discoveryListener - Listener to obtain discovered resource
 *
 * NOTE: 0 - Unicast  1 - Multicast
 */
JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_client_RCSDiscoveryManager_discoverResource
(JNIEnv *env, jclass interfaceClass, jstring address, jint typeofDiscovery, jstring relativeURI,
 jstring resourceType, jobject discoveryListener);

#ifdef __cplusplus
}
#endif
#endif //__JNI_RE_RCS_DISCOVERY_MANAGER_H_

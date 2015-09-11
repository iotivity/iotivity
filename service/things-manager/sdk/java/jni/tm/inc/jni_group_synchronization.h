/* *****************************************************************
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

/**
 * @file
 * This file contains the declaration of Group Synchronization APIs
 * for JNI implementation.
 */

#ifndef JNI_GROUP_SYNCHRONIZATION_H_
#define JNI_GROUP_SYNCHRONIZATION_H_

#include <stdio.h>
#include <string.h>

#include <jni.h>
#include <jni_string.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * API for finding a specific remote group when a resource tries to join a group.
 * Callback is called when a group is found or not.
 *
 * @param resourceTypes  - resource types of a group to find and join
 *
 * @return OCStackResult - return value of this API.
 *                         It returns OC_STACK_OK if success.
 *
 * NOTE: It return OC_STACK ERROR when It was finding a group.
 *       You should call this api when the group finding process has stopped.
 *       OCStackResult is defined in ocstack.h.
 */

JNIEXPORT jint JNICALL JNIGroupSynchronizationFindGroup
(JNIEnv *env, jobject interfaceObject, jobject resourceTypes);

/**
 * API for creating a new group.
 *
 * @param resourceType   - resource type of a group to create
 *
 * @return OCStackResult - return value of this API.
 *                         It returns OC_STACK_OK if success.
 *
 * NOTE: OCStackResult is defined in ocstack.h.
 */
JNIEXPORT jint JNICALL JNIGroupSynchronizationCreateGroup
(JNIEnv *env, jobject interfaceObject, jstring resourceType);

/**
 * API for joining a group. This API is used when a resource that has a group tries
 * to find a specific remote resource and makes it join a group
 *
 * @param resourceType   - resource type of a group to join.
 * @param resourceHandle - resource handle to join a group.
 *
 * @return OCStackResult - return value of this API.
 *                         It returns OC_STACK_OK if success.
 *
 * NOTE: If you want to join the resource in the remote(other) process, use the API
 *       jniThingsManagerJoinGroupObject instead of this.
 *       OCStackResult is defined in ocstack.h.
 */
JNIEXPORT jint JNICALL JNIGroupSynchronizationJoinGroupString
(JNIEnv *env, jobject interfaceObject, jstring resourceType, jobject resourceHandle);

/**
 * API for joining a group. This API is used when a resource that
 * doesn't have a group tries to find and join a specific remote group.
 *
 * @param resource       - group resource pointer to join.
 * @param resourceHandle - resource handle to join a group.
 *
 * @return OCStackResult - return value of this API.
 *                         It returns OC_STACK_OK if success.
 *
 * NOTE: NOTE: If you want to join the resource in the same process, use the API
 *       jniThingsManagerJoinGroupString instead of this.
 *       OCStackResult is defined in ocstack.h.
 */
JNIEXPORT jint JNICALL JNIGroupSynchronizationJoinGroupObject
(JNIEnv *env, jobject interfaceObject, jobject resource, jobject resourceHandle);

/**
 * API for leaving a joined group.
 *
 * @param resourceType   - resource type of a group to leave.
 * @param resourceHandle - resource handle to join a group.
 *
 * @return OCStackResult - return value of this API.
 *                         It returns OC_STACK_OK if success.
 *
 * NOTE: OCStackResult is defined in ocstack.h.
 */
JNIEXPORT jint JNICALL JNIGroupSynchronizationLeaveGroup
(JNIEnv *env, jobject interfaceObject, jstring resourceType, jobject resourceHandle);

/**
 * API for leaving a joined group.
 *
 * @param resource       - resource of a group to leave.
 * @param resourceType   - resource type of a group to leave.
 * @param resourceHandle - resource handle to join a group.
 *
 * @return OCStackResult - return value of this API.
 *                         It returns OC_STACK_OK if success.
 *
 * NOTE: OCStackResult is defined in ocstack.h.
 */
JNIEXPORT jint JNICALL JNIGroupSynchronizationLeaveGroupForResource
(JNIEnv *env, jobject interfaceObject, jobject resource, jstring resourceType,
 jobject resourceHandle);

/**
 * API for deleting a group.
 *
 * @param collectionResourceType - resource type of a group to delete.
 *
 * @return void
 */
JNIEXPORT void JNICALL JNIGroupSynchronizationDeleteGroup
(JNIEnv *env, jobject interfaceObject, jstring collectionResourceType);

/**
 * API for getting a list of joined groups.
 *
 * @return std::map - return value of this API.
 *                  It returns group resource type and group resource handle as a map type.
 */
JNIEXPORT jobject JNICALL JNIGroupSynchronizationGetGroupList
(JNIEnv *env, jobject interfaceObject);

#ifdef __cplusplus
}
#endif
#endif //JNI_GROUP_SYNCHRONIZATION_H_

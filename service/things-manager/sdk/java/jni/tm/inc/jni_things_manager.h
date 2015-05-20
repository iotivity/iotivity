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

/** @file   jni_things_manager.h
 *
 *   @brief  This file contains the declaration of Things Manager service API's
 *              for JNI implementation
 */

#ifndef __JNI_THINGS_MANAGER_H_
#define __JNI_THINGS_MANAGER_H_


#include <stdio.h>
#include <string.h>

#include <jni.h>
#include <jni_string.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * API for discoverying candidate resources.
 *
 * @param resourceTypes - required resource types(called "candidate")
 * @param waitSec - Delay time in seconds to add before starting to find the resources in network.
 *
 * @return OCStackResult - return value of this API.
 *                         It returns OC_STACK_OK if success.
 *
 * NOTE: OCStackResult is defined in ocstack.h.
 */
JNIEXPORT jint JNICALL JNIThingsManagerFindCandidateResource
(JNIEnv *env, jobject interfaceObject, jobject resourceTypes, jint waitSec);

/**
 * API for subscribing child's state.
 *
 * @param resource - collection resource for subscribing presence of all child resources.
 *
 * @return OCStackResult - return value of this API.
 *                         It returns OC_STACK_OK if success.
 *
 * NOTE: OCStackResult is defined in ocstack.h.
 */
JNIEXPORT jint JNICALL JNIThingsManagerSubscribeCollectionPresence
(JNIEnv *env, jobject interfaceObject, jobject resource);

/**
 * API for register and bind resource to group.
 *
 * @param resource - resource for register and bind to group. It has all data.
 * @param collectionHandle - collection resource handle. It will be added child resource.
 *
 * @return childHandle - child resource handle.
 *
 * NOTE: OCStackResult is defined in ocstack.h.
 */
JNIEXPORT jobject JNICALL JNIThingsManagerBindResourceToGroup
(JNIEnv *env, jobject interfaceObject, jobject resource, jobject collectionHandle);

/**
 * API for finding a specific remote group when a resource tries to join a group.
 * Callback is called when a group is found or not.
 *
 * @param resourceTypes - resource types of a group to find and join
 *
 * @return OCStackResult - return value of this API.
 *                         It returns OC_STACK_OK if success.
 *
 * NOTE: It return OC_STACK ERROR when It was finding a group.
 *       You should call this api when the group finding process has stopped.
 *       OCStackResult is defined in ocstack.h.
 */

JNIEXPORT jint JNICALL JNIThingsManagerFindGroup
(JNIEnv *env, jobject interfaceObject, jobject resourceTypes);

/**
 * API for creating a new group.
 *
 * @param resourceType - resource type of a group to create
 *
 * @return OCStackResult - return value of this API.
 *                         It returns OC_STACK_OK if success.
 *
 * NOTE: OCStackResult is defined in ocstack.h.
 */
JNIEXPORT jint JNICALL JNIThingsManagerCreateGroup
(JNIEnv *env, jobject interfaceObject, jstring resourceType);

/**
 * API for joining a group. This API is used when a resource that has a group tries
 * to find a specific remote resource and makes it join a group
 *
 * @param resourceType - resource type of a group to join.
 * @param resourceHandle - resource handle to join a group.
 *
 * @return OCStackResult - return value of this API.
 *                         It returns OC_STACK_OK if success.
 *
 * NOTE: If you want to join the resource in the remote(other) process, use the API
 *       jniThingsManagerJoinGroupObject instead of this.
 *       OCStackResult is defined in ocstack.h.
 */
JNIEXPORT jint JNICALL JNIThingsManagerJoinGroupString
(JNIEnv *env, jobject interfaceObject, jstring resourceType, jobject resourceHandle);

/**
 * API for joining a group. This API is used when a resource that
 * doesn't have a group tries to find and join a specific remote group.
 *
 * @param resource - group resource pointer to join.
 * @param resourceHandle - resource handle to join a group.
 *
 * @return OCStackResult - return value of this API.
 *                         It returns OC_STACK_OK if success.
 *
 * NOTE: NOTE: If you want to join the resource in the same process, use the API
 *       jniThingsManagerJoinGroupString instead of this.
 *       OCStackResult is defined in ocstack.h.
 */
JNIEXPORT jint JNICALL JNIThingsManagerJoinGroupObject
(JNIEnv *env, jobject interfaceObject, jobject resource, jobject resourceHandle);

/**
 * API for leaving a joined group.
 *
 * @param resourceType - resource type of a group to leave.
 * @param resourceHandle - resource handle to join a group.
 *
 * @return OCStackResult - return value of this API.
 *                         It returns OC_STACK_OK if success.
 *
 * NOTE: OCStackResult is defined in ocstack.h.
 */
JNIEXPORT jint JNICALL JNIThingsManagerLeaveGroup
(JNIEnv *env, jobject interfaceObject, jstring resourceType, jobject resourceHandle);

/**
 * API for leaving a joined group.
 *
 * @param resource - resource of a group to leave.
 * @param resourceType - resource type of a group to leave.
 * @param resourceHandle - resource handle to join a group.
 *
 * @return OCStackResult - return value of this API.
 *                         It returns OC_STACK_OK if success.
 *
 * NOTE: OCStackResult is defined in ocstack.h.
 */
JNIEXPORT jint JNICALL JNIThingsManagerLeaveGroupForResource
(JNIEnv *env, jobject interfaceObject, jobject resource, jstring resourceType,
 jobject resourceHandle);
/**
 * API for deleting a group.
 *
 * @param collectionResourceType - resource type of a group to delete.
 *
 * @return void
 */
JNIEXPORT void JNICALL JNIThingsManagerDeleteGroup
(JNIEnv *env, jobject interfaceObject, jstring collectionResourceType);

/**
 * API for getting a list of joined groups.
 *
 * @return std::map - return value of this API.
 *                  It returns group resource type and group resource handle as a map type.
 */
JNIEXPORT jobject JNICALL JNIThingsManagerGetGroupList
(JNIEnv *env, jobject interfaceObject);

/**
 * API for updating configuration value of multiple things of a target group
 * or a single thing.
 * Before using the below function, a developer should acquire a resource pointer of
 * (collection) resource that he wants to send a request by calling findResource() function
 * provided in OCPlatform. And he should also notice a "Configuration Name" term which
 * represents a nickname of a target attribute of a resource that he wants to update.
 * The base motivation to introduce the term is to avoid a usage of URI to access a resource
 * from a developer. Thus, a developer should know which configuration names are supported
 * by Things Configuration class and what the configuration name means.
 * To get a list of supported configuration names,  use getListOfSupportedConfigurationUnits()
 * function, which provides the list in JSON format.
 *
 * @param resource - resource pointer representing the target group or the single thing.
 * @param configurations - ConfigurationUnit: a nickname of attribute of target resource
 *                         (e.g., installedlocation, currency, (IP)address)
 *                         Value : a value to be updated
 *
 * @return OCStackResult - return value of this API.
 *                         It returns OC_STACK_OK if success.
 *
 * NOTE: OCStackResult is defined in ocstack.h.
 */
JNIEXPORT jint JNICALL JNIThingsManagerUpdateConfigurations
(JNIEnv *env, jobject interfaceObject, jobject resource, jobject configurations);

/**
 * API for getting configuration value of multiple things of a target group
 * or a single thing.
 *
 * @param resource - resource pointer representing the target group or the single thing.
 * @param configurations - ConfigurationUnit: a nickname of attribute of target resource.
 *
 * @return OCStackResult - return value of this API.
 *                         It returns OC_STACK_OK if success.
 *
 * NOTE: OCStackResult is defined in ocstack.h.
 */
JNIEXPORT jint JNICALL JNIThingsManagerGetConfigurations
(JNIEnv *env, jobject interfaceObject, jobject resource, jobject configurations);

/**
 * API for showing the list of supported configuration units (configurable parameters)
 *
 * @return std::string - return value of this API.
 *                       It returns the list in JSON format
 */
JNIEXPORT jstring JNICALL JNIThingsManagerGetListOfSupportedConfigurationUnits
(JNIEnv *env, jobject interfaceObject);

/**
 * API for boostrapping system configuration parameters from a bootstrap server.
 *
 * @return OCStackResult - return value of this API.
 *                         It returns OC_STACK_OK if success.
 *
 * NOTE: OCStackResult is defined in ocstack.h.
 */
JNIEXPORT jint JNICALL JNIThingsManagerDoBootstrap
(JNIEnv *env, jobject interfaceObject);

/**
 * API to let thing(device) reboot.
 * The target thing could be a group of multiple things or a single thing.
 *
 * @param resource - resource type representing the target group
 *
 * @return OCStackResult - return value of this API.
 *                         It returns OC_STACK_OK if success.
 *
 * NOTE: OCStackResult is defined in ocstack.h.
 */
JNIEXPORT jint JNICALL JNIThingsManagerReboot
(JNIEnv *env, jobject interfaceObject, jobject resource);

/**
 * API for factory reset on thing(device).
 * The target thing could be a group of multiple things or a single thing.
 *
 * @param resource - resource type representing the target group
 *
 * @return OCStackResult - return value of this API.
 *                         It returns OC_STACK_OK if success.
 *
 * NOTE: OCStackResult is defined in ocstack.h.
 */
JNIEXPORT jint JNICALL JNIThingsManagerFactoryReset
(JNIEnv *env, jobject interfaceObject, jobject resource);

/**
 * API for adding an Action Set.
 *
 * @param resource - resource type representing the target group
 * @param newActionSet - list of Action Set to be added
 *
 * @return OCStackResult - return value of this API.
 *                         It returns OC_STACK_OK if success.
 *
 * NOTE: OCStackResult is defined in ocstack.h.
 */

JNIEXPORT jint JNICALL JNIThingsManagerAddActionSet
(JNIEnv *env, jobject interfaceObject, jobject resource, jobject newActionSet);

/**
 * API for executing the Action Set.
 *
 * @param resource - resource type representing the target group
 * @param actionSetName - Action Set name for executing the Action set
 *
 * @return OCStackResult - return value of this API.
 *                         It returns OC_STACK_OK if success.
 *
 * NOTE: OCStackResult is defined in ocstack.h.
 */
JNIEXPORT jint JNICALL JNIThingsManagerExecuteActionSet
(JNIEnv *env, jobject interfaceObject, jobject resource, jstring actionSetName);

/**
 * API for executing the Action Set.
 *
 * @param resource - resource type representing the target group
 * @param actionSetName - Action Set name for executing the Action set
 * @param delay - waiting time for until action set run.
 *
 * @return OCStackResult - return value of this API.
 *                         It returns OC_STACK_OK if success.
 *
 * NOTE: OCStackResult is defined in ocstack.h.
 */
JNIEXPORT jint JNICALL JNIThingsManagerExecuteActionSetWithDelay
(JNIEnv *env, jobject interfaceObject, jobject resource, jstring actionSetName, jlong delay);

/**
 * API for cancelling the Action Set.
 *
 * @param resource - resource type representing the target group
 * @param actionSetName - Action Set name for cancelling the Action set
 *
 * @return OCStackResult - return value of this API.
 *                         It returns OC_STACK_OK if success.
 *
 * NOTE: OCStackResult is defined in ocstack.h.
 */
JNIEXPORT jint JNICALL JNIThingsManagerCancelActionSet
(JNIEnv *env, jobject interfaceObject, jobject resource, jstring actionSetName);


/**
 * API for reading the Action Set.
 *
 * @param resource - resource type representing the target group
 * @param actionSetName - Action Set name for reading the Action set
 *
 * @return OCStackResult - return value of this API.
 *                         It returns OC_STACK_OK if success.
 *
 * NOTE: OCStackResult is defined in ocstack.h.
 */
JNIEXPORT jint JNICALL JNIThingsManagerGetActionSet
(JNIEnv *env, jobject interfaceObject, jobject resource, jstring actionSetName);

/**
 * API for removing the Action Set.
 *
 * @param resource - resource type representing the target group
 * @param actionSetName - Action Set name for removing the Action set
 *
 * @return OCStackResult - return value of this API.
 *                         It returns OC_STACK_OK if success.
 *
 * NOTE: OCStackResult is defined in ocstack.h.
 */
JNIEXPORT jint JNICALL JNIThingsManagerDeleteActionSet
(JNIEnv *env, jobject interfaceObject, jobject resource, jstring actionSetName);

#ifdef __cplusplus
}
#endif
#endif //__JNI_THINGS_MANAGER_H_
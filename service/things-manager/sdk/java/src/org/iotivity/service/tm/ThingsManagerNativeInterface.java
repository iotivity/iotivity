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
/**
 * @file    ThingsManagerNativeInterface.java
 *
 * @brief     This file provides a class which provides a set of native functions relating to group
 *        management, synchronization of group, configuration of things, and
 *        diagnostics about things.
 */

package org.iotivity.service.tm;

import java.util.Map;
import java.util.Vector;

import org.iotivity.base.OcResource;
import org.iotivity.base.OcResourceHandle;

/**
 * This class provides a set of native functions relating to group
 * management, synchronization of group, configuration of things, and
 * diagnostics about things.
 *
 */
class ThingsManagerNativeInterface {
    /**
     * Native function for discoverying candidate resources.
     *
     * @param resourceTypes
     *            - required resource types(called "candidate")
     * @param waitTime
     *            - Delay time in seconds to add before starting to find the
     *            resources in network.
     *
     * @return OCStackResult - return value of this API. It returns OC_STACK_OK
     *         if success.
     *
     */
    public static native int findCandidateResources(
            Vector<String> resourceTypes, int waitTime);

    /**
     * Native function for subscribing child's state.
     *
     * @param resource
     *            - collection resource for subscribing presence of all child
     *            resources.
     *
     * @return OCStackResult - return value of this API. It returns OC_STACK_OK
     *         if success.
     *
     */
    public static native int subscribeCollectionPresence(OcResource resource);

    /**
     * Native function for register and bind resource to group.
     *
     * @param resource
     *            - resource for register and bind to group. It has all data.
     * @param collectionHandle
     *            - collection resource handle. It will be added child resource.
     *
     * @return OcResourceHandle - Child resource handle.
     *
     */
    public static native OcResourceHandle bindResourceToGroup(OcResource resource,
            OcResourceHandle collectionHandle);

    /**
     * Native function for finding a specific remote group when a resource tries
     * to join a group. Callback is called when a group is found or not.
     *
     * @param collectionResourceTypes
     *            - resource types of a group to find and join
     *
     * @return OCStackResult - return value of this API. It returns OC_STACK_OK
     *         if success.
     *
     */
    public static native int findGroup(Vector<String> collectionResourceTypes);

    /**
     * Native function for creating a new group.
     *
     * @param collectionResourceType
     *            - resource type of a group to create
     *
     * @return OCStackResult - return value of this API. It returns OC_STACK_OK
     *         if success.
     */
    public static native int createGroup(String collectionResourceType);

    /**
     * Native function for joining a group. This API is used when a resource
     * that has a group tries to find a specific remote resource and makes it
     * join a group.
     *
     * @param collectionResourceType
     *            - resource type of a group to join.
     * @param resourceHandle
     *            - resource handle to join a group.
     *
     * @return OCStackResult - return value of this API. It returns OC_STACK_OK
     *         if success.
     *
     */
    public static native int joinGroup(String collectionResourceType,
            OcResourceHandle resourceHandle);

    /**
     * Native function for joining a group. This is used when a resource that
     * doesn't have a group tries to find and join a specific remote group.
     *
     * @param resource
     *            - group resource to join.
     * @param resourceHandle
     *            - resource handle to join a group.
     *
     * @return OCStackResult - return value of this API. It returns OC_STACK_OK
     *         if success.
     *
     */
    public static native int joinGroup(OcResource resource,
            OcResourceHandle resourceHandle);

    /**
     * Native function for leaving a joined group.
     *
     * @param collectionResourceType
     *            - resource type of a group to leave.
     * @param resourceHandle
     *            - resource handle to leave a group.
     *
     * @return OCStackResult - return value of this API. It returns OC_STACK_OK
     *         if success.
     *
     */
    public static native int leaveGroup(String collectionResourceType,
            OcResourceHandle resourceHandle);

    /**
     * Native function for leaving a joined group.
     *
     * @param resource
     *            - resource of a group to leave.
     * @param collectionResourceType
     *            - resource type of a group to leave.
     * @param resourceHandle
     *            - resource handle to leave a group.
     *
     * @return OCStackResult - return value of this API. It returns OC_STACK_OK
     *         if success.
     *
     */
    public static native int leaveGroup(OcResource resource,
            String collectionResourceType, OcResourceHandle resourceHandle);

    /**
     * Native function for deleting a group.
     *
     * @param collectionResourceType
     *            - resource type of a group to delete.
     *
     *
     */
    public static native void deleteGroup(String collectionResourceType);

    /**
     * Native function for getting a list of joined groups.
     *
     * @return Returns group resource type and group resource handle as a map
     *         table.
     *
     */
    public static native Map<String, OcResourceHandle> getGroupList();

    /**
     * Native function for updating configuration value of multiple things of a
     * target group or a single thing.
     *
     * @param resource
     *            - resource representing the target group or the single thing.
     * @param configurations
     *            - ConfigurationUnit: a nickname of attribute of target
     *            resource (e.g., installed location, currency, (IP)address)
     *            Value : a value to be updated
     *
     * @return OCStackResult - return value of this API. It returns OC_STACK_OK
     *         if success.
     *
     */
    public static native int updateConfigurations(OcResource resource,
            Map<String, String> configurations);

    /**
     * Native function for getting configuration value of multiple things of a
     * target group or a single thing.
     *
     * @param resource
     *            - resource representing the target group or the single thing.
     * @param configurations
     *            - ConfigurationUnit: a nickname of attribute of target
     *            resource.
     *
     * @return OCStackResult - return value of this API. It returns OC_STACK_OK
     *         if success.
     *
     */
    public static native int getConfigurations(OcResource resource,
            Vector<String> configurations);

    /**
     * Native function for showing the list of supported configuration units
     * (configurable parameters)
     *
     * @return Returns the configuration list in JSON format.
     *
     */
    public static native String getListOfSupportedConfigurationUnits();

    /**
     * Native function for bootstrapping system configuration parameters from a
     * bootstrap server.
     *
     * @return OCStackResult - return value of this API. It returns OC_STACK_OK
     *         if success.
     */
    public static native int doBootstrap();

    /**
     * Native function to let thing(device) reboot. The target thing could be a
     * group of multiple things or a single thing.
     *
     * @param resource
     *            - resource pointer representing the target group
     *
     * @return OCStackResult - return value of this API. It returns OC_STACK_OK
     *         if success
     */
    public static native int reboot(OcResource resource);

    /**
     * Native function for factory reset on thing(device). The target thing
     * could be a group of multiple things or a single thing.
     *
     * @param resource
     *            - resource pointer representing the target group
     *
     * @return OCStackResult - return value of this API. It returns OC_STACK_OK
     *         if success.
     */
    public static native int factoryReset(OcResource resource);

    /**
     * Native function for adding an Action Set. Callback is called when the
     * response of PUT operation arrives.
     *
     * @param resource
     *            - resource pointer of the group resource
     * @param actionSet
     *            - pointer of Action Set
     *
     * @return OCStackResult - return value of this API. It returns OC_STACK_OK
     *         if success.
     */
    public static native int addActionSet(OcResource resource,
            ActionSet actionSet);

    /**
     * Native function for executing the Action Set. Callback is called when the
     * response of POST operation arrives.
     *
     * @param resource
     *            - resource pointer of the group resource
     * @param actionsetName
     *            - Action Set name for removing the Action set
     *
     * @return OCStackResult - return value of this API. It returns OC_STACK_OK
     *         if success.
     */
    public static native int executeActionSet(OcResource resource,
            String actionsetName);

    /**
     * Native function for executing the Action Set. Callback is called when the
     * response of POST operation arrives.
     *
     * @param resource
     *            - resource pointer of the group resource
     * @param actionsetName
     *            - Action Set name for removing the Action set
     * @param delay
     *            - waiting time for until action set run.
     *
     * @return OCStackResult - return value of this API. It returns OC_STACK_OK
     *         if success.
     */
    public static native int executeActionSet(OcResource resource,
            String actionsetName, long delay);

    /**
     * Native function for cancelling the Action Set. Callback is called when
     * the response of POST operation arrives.
     *
     * @param resource
     *            - resource pointer of the group resource
     * @param actionsetName
     *            - Action Set name for removing the Action set
     *
     * @return OCStackResult - return value of this API. It returns OC_STACK_OK
     *         if success.
     */
    public static native int cancelActionSet(OcResource resource,
            String actionsetName);

    /**
     * Native function for reading the Action Set. Callback is called when the
     * response of GET operation arrives.
     *
     * @param resource
     *            - resource pointer of the group resource
     * @param actionsetName
     *            - Action Set name for removing the Action set
     *
     * @return OCStackResult - return value of this API. It returns OC_STACK_OK
     *         if success.
     */
    public static native int getActionSet(OcResource resource,
            String actionsetName);

    /**
     * Native function for removing the action set. Callback is called when the
     * response of POST operation arrives.
     *
     * @param resource
     *            - resource pointer of the group resource
     * @param actionsetName
     *            - Action Set name for removing the Action set
     *
     * @return OCStackResult - return value of this API. It returns OC_STACK_OK
     *         if success.
     */
    public static native int deleteActionSet(OcResource resource,
            String actionsetName);
}

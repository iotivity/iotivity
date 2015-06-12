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
 * @file   ThingsManagerInterface.java
 *
 * @brief    This file provides a class which acts as an interface for Things Manager API calls between
 *        ThingsManager and ThingsManagerNativeInterface
 *
 */

package org.iotivity.service.tm;

import java.util.Iterator;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Vector;

import org.iotivity.base.OcException;
import org.iotivity.base.OcResource;
import org.iotivity.base.OcResourceHandle;

import android.util.Log;

/**
 * This class acts as an interface for Things Manager API calls between
 * ThingsManager and ThingsManagerNativeInterface
 *
 */
class ThingsManagerInterface {

    private IFindCandidateResourceListener resourceListener;
    private ISubscribePresenceListener     presenceListener;
    private IFindGroupListener             groupListener;
    private IConfigurationListener         configurationListener;
    private static ThingsManagerInterface  thingsManagerInterfaceObj;

    private ThingsManagerInterface() {
    }

    /**
     * Function for Getting instance of ThingsManagerInterface object.
     *
     * @return ThingsManagerInterface instance.
     *
     */
    public static synchronized ThingsManagerInterface getInstance() {
        if (null == thingsManagerInterfaceObj) {
            thingsManagerInterfaceObj = new ThingsManagerInterface();
        }
        return thingsManagerInterfaceObj;
    }

    /**
     * Register listener for findCandidateResource callback.
     *
     * @param listener
     *            interface for getting notification when resources are
     *            discovered in network.
     *
     */
    public void registerFindCandidateResourceListener(
            IFindCandidateResourceListener listener) {
        resourceListener = listener;
    }

    /**
     * Register listener for subscribeCollectionPresence callback.
     *
     * @param listener
     *            interface for getting notification regarding child resource
     *            presence status.
     *
     */
    public void registerSubscribePresenceListener(
            ISubscribePresenceListener listener) {
        presenceListener = listener;
    }

    /**
     * Register listener for register group and find group callback.
     *
     * @param listener
     *            interface for getting notification on whether the group is
     *            found or not.
     *
     */
    public void registerGroupListener(IFindGroupListener listener) {
        groupListener = listener;
    }

    /**
     * Register listener for updateConfigurations and getConfigurations
     * callback.
     *
     * @param listener
     *            interface for getting notification on configuration values
     *            information or when configuration value is updated for
     *            multiple things of a target group or a single thing.
     *
     */
    public void registerConfigurationListener(IConfigurationListener listener) {
        configurationListener = listener;
    }

    /**
     * Unregister listener for findCandidateResource callback.
     *
     */
    public void unregisterFindCandidateResourceListener() {
        resourceListener = null;
    }

    /**
     * Unregister listener for subscribeCollectionPresence callback.
     *
     */
    public void unregisterSubscribePresenceListener() {
        presenceListener = null;
    }

    /**
     * Unregister listener for registerGroup and findGroup callback.
     *
     */
    public void unregisterGroupListener() {
        groupListener = null;
    }

    /**
     * Unregister listener for updateConfigurations and getConfigurations
     * callback.
     *
     */
    public void unregisterConfigurationListener() {
        configurationListener = null;
    }

    /**
     * API for discovering candidate resources with waiting delay. Listener
     * <p>
     * IFindCandidateResourceListener::onResourceCallback will be notified
     *      when resource is discovered in network.
     * </p>
     *
     * @param resourceTypes
     *            required resource types(called "candidate")
     * @param waitTime
     *            Time duration in seconds to find the resources in network
     *
     * @return OCStackResult - return value of this API. It returns OC_STACK_OK
     *         if success.
     *
     */
    public OCStackResult findCandidateResources(Vector<String> resourceTypes,
            int waitTime) {
        OCStackResult result;
        if (null == resourceListener) {
            result = OCStackResult.OC_STACK_ERROR;
        } else {
            int ordinal = ThingsManagerNativeInterface.findCandidateResources(
                    resourceTypes, waitTime);
            result = OCStackResult.conversion(ordinal);
        }
        return result;
    }

    /**
     * API for subscribing child's state. Listener
     * <p>
     * ISubscribePresenceListener::onPresenceCallback will be notified for
     *      resource presence status
     * </p>
     *
     * @param resource
     *            collection resource for subscribing presence of all child
     *            resources
     *
     * @return OCStackResult - return value of this API. It returns OC_STACK_OK
     *         if success.
     *
     */
    public OCStackResult subscribeCollectionPresence(OcResource resource)
            throws OcException {
        String LOG_TAG = this.getClass().getSimpleName();

        OCStackResult result;
        if (null == presenceListener) {
            result = OCStackResult.OC_STACK_ERROR;
        } else {

            int ordinal = ThingsManagerNativeInterface
                    .subscribeCollectionPresence(resource);
            Log.i(LOG_TAG, "Ordinal value = : " + ordinal);
            result = OCStackResult.conversion(ordinal);
        }
        return result;
    }

    /**
     * API for finding a specific remote group when a resource tries to join a
     * group.
     * <p>
     * IFindGroupListener::onGroupFindCallback will be called to notify
     *      whether requested group found or not.
     * </p>
     *
     * @param resourceTypes
     *            resource types of a group to find and join
     *
     * @return OCStackResult - return value of this API. It returns OC_STACK_OK
     *         if success.
     *
     */
    public OCStackResult findGroup(Vector<String> resourceTypes) {
        OCStackResult result;
        if (null == groupListener) {
            result = OCStackResult.OC_STACK_ERROR;
        } else {
            int ordinal = ThingsManagerNativeInterface.findGroup(resourceTypes);
            result = OCStackResult.conversion(ordinal);
        }
        return result;
    }

    /**
     * API for creating a new group.
     *
     * @param resourceType
     *            resource type of a group to create
     *
     * @return OCStackResult - return value of this API. It returns OC_STACK_OK
     *         if success.
     *
     */
    public OCStackResult createGroup(String resourceType) {
        OCStackResult result;
        int ordinal = ThingsManagerNativeInterface.createGroup(resourceType);
        result = OCStackResult.conversion(ordinal);
        return result;
    }

    /**
     * API for joining a group. This API is used when a resource that has a
     * group tries to find a specific remote resource and makes it join a group.
     *
     * @param resourceType
     *            resource type of a group to join.
     * @param resourceHandle
     *            resource handle to join a group.
     *
     * @return OCStackResult - return value of this API. It returns OC_STACK_OK
     *         if success.
     *
     */
    public OCStackResult joinGroup(String resourceType,
            OcResourceHandle resourceHandle) {
        OCStackResult result;

        int ordinal = ThingsManagerNativeInterface.joinGroup(resourceType,
                resourceHandle);
        result = OCStackResult.conversion(ordinal);

        return result;
    }

    /**
     * API for joining a group. This API is used when a resource that doesn't
     * have a group tries to find and join a specific remote group.
     *
     * @param resource
     *            group resource to join.
     * @param resourceHandle
     *            resource handle to join a group.
     *
     * @return OCStackResult - return value of this API. It returns OC_STACK_OK
     *         if success.
     *
     */
    public OCStackResult joinGroup(OcResource resource,
            OcResourceHandle resourceHandle) throws OcException {

        OCStackResult result;
        int ordinal = ThingsManagerNativeInterface.joinGroup(resource,
                resourceHandle);
        result = OCStackResult.conversion(ordinal);

        return result;
    }

    /**
     * API for leaving a joined group.
     *
     * @param resourceType
     *            resource type of a group to leave.
     * @param resourceHandle
     *            resource handle to leave a group.
     *
     * @return OCStackResult - return value of this API. It returns OC_STACK_OK
     *         if success.
     *
     */
    public OCStackResult leaveGroup(String resourceType,
            OcResourceHandle resourceHandle) {

        OCStackResult result;
        int ordinal = ThingsManagerNativeInterface.leaveGroup(resourceType,
                resourceHandle);
        result = OCStackResult.conversion(ordinal);

        return result;
    }

    /**
     * API for leaving a joined group.
     *
     * @param resource
     *            resource of a group to leave.
     * @param resourceType
     *            resource type of a group to leave.
     * @param resourceHandle
     *            resource handle to leave a group.
     *
     * @return OCStackResult - return value of this API. It returns OC_STACK_OK
     *         if success.
     *
     */
    public OCStackResult leaveGroup(OcResource resource, String resourceType,
            OcResourceHandle resourceHandle) {

        OCStackResult result;
        int ordinal = ThingsManagerNativeInterface.leaveGroup(resource,
                resourceType, resourceHandle);
        result = OCStackResult.conversion(ordinal);

        return result;
    }

    /**
     * API for deleting a group.
     *
     * @param collectionResourceType
     *            resource type of a group to delete.
     *
     */
    public void deleteGroup(String collectionResourceType) {
        ThingsManagerNativeInterface.deleteGroup(collectionResourceType);
    }

    /**
     * API for getting a list of joined groups.
     *
     * @return Returns group resource type and group resource handle as a map
     *         table.
     *
     */
    public Map<String, OcResourceHandle> getGroupList() {
        return ThingsManagerNativeInterface.getGroupList();
    }

    /**
     * API for register and bind resource to group.
     *
     * @param resource
     *            resource for register and bind to group. It has all data.
     * @param collectionHandle
     *            collection resource handle. It will be added child resource.
     *
     * @return OcResourceHandle - Child resource handle.
     */
    public OcResourceHandle bindResourceToGroup(OcResource resource,
            OcResourceHandle collectionHandle)
            throws OcException {
        return ThingsManagerNativeInterface.bindResourceToGroup(
                resource, collectionHandle);
    }

    /**
     * API for updating configuration value of multiple things of a target group
     * or a single thing.
     *
     * @param resource
     *            resource representing the target group or the single thing.
     * @param configurations
     *            ConfigurationUnit: a nickname of attribute of target
     *            resource (e.g., installed location, currency, (IP)address)
     *            Value : a value to be updated
     *
     * @return OCStackResult - return value of this API. It returns OC_STACK_OK
     *         if success.
     *
     */
    public OCStackResult updateConfigurations(OcResource resource,
            Map<String, String> configurations) throws OcException {
        OCStackResult result;
        if (null == configurationListener) {
            result = OCStackResult.OC_STACK_ERROR;
        } else {
            int ordinal = ThingsManagerNativeInterface.updateConfigurations(
                    resource, configurations);
            result = OCStackResult.conversion(ordinal);
        }
        return result;

    }

    /**
     * API for getting configuration value of multiple things of a target group
     * or a single thing.
     *
     * @param resource
     *            resource representing the target group or the single thing.
     * @param configurations
     *            ConfigurationUnit: a nickname of attribute of target
     *            resource.
     *
     * @return OCStackResult - return value of this API. It returns OC_STACK_OK
     *         if success.
     *
     */
    public OCStackResult getConfigurations(OcResource resource,
            Vector<String> configurations) throws OcException {
        OCStackResult result;
        if (null == configurationListener) {
            result = OCStackResult.OC_STACK_ERROR;
        } else {
            int ordinal = ThingsManagerNativeInterface.getConfigurations(
                    resource, configurations);
            result = OCStackResult.conversion(ordinal);
        }
        return result;
    }

    /**
     * API for showing the list of supported configuration units (configurable
     * parameters)
     *
     * @return Returns the configuration list in JSON format.
     *
     */
    public String getListOfSupportedConfigurationUnits() {

        return ThingsManagerNativeInterface
                .getListOfSupportedConfigurationUnits();
    }

    /**
     * API for bootstrapping system configuration parameters from a bootstrap
     * server.
     *
     * @return OCStackResult - return value of this API. It returns OC_STACK_OK
     *         if success.
     */
    public OCStackResult doBootstrap() {

        OCStackResult result;
        int ordinal = ThingsManagerNativeInterface.doBootstrap();
        result = OCStackResult.conversion(ordinal);

        return result;
    }

    /**
     * API to let thing(device) reboot. The target thing could be a group of
     * multiple things or a single thing.
     *
     * @param resource
     *            resource pointer representing the target group
     *
     * @return OCStackResult - return value of this API. It returns OC_STACK_OK
     *         if success
     */
    public OCStackResult reboot(OcResource resource) throws OcException {

        OCStackResult result;
        int ordinal = ThingsManagerNativeInterface.reboot(resource);
        result = OCStackResult.conversion(ordinal);

        return result;
    }

    /**
     * API for factory reset on thing(device). The target thing could be a group
     * of multiple things or a single thing.
     *
     * @param resource
     *            resource pointer representing the target group
     *
     * @return OCStackResult - return value of this API. It returns OC_STACK_OK
     *         if success.
     */
    public OCStackResult factoryReset(OcResource resource) throws OcException {

        OCStackResult result;
        int ordinal = ThingsManagerNativeInterface.factoryReset(resource);
        result = OCStackResult.conversion(ordinal);

        return result;
    }

    /**
     * API for adding an Action Set. Callback is called when the response of PUT
     * operation arrives.
     *
     * @param resource
     *            resource pointer of the group resource
     * @param actionSet
     *            pointer of Action Set
     *
     * @return OCStackResult - return value of this API. It returns OC_STACK_OK
     *         if success.
     */
    public OCStackResult addActionSet(OcResource resource, ActionSet actionSet)
            throws OcException {

        int ordinal = ThingsManagerNativeInterface.addActionSet(resource,
                actionSet);

        return OCStackResult.conversion(ordinal);
    }

    /**
     * API for executing the Action Set. Callback is called when the response of
     * POST operation arrives.
     *
     * @param resource
     *            resource pointer of the group resource
     * @param actionsetName
     *            Action Set name for removing the Action set
     *
     * @return OCStackResult - return value of this API. It returns OC_STACK_OK
     *         if success.
     */
    public OCStackResult executeActionSet(OcResource resource,
            String actionsetName) throws OcException {

        int ordinal = ThingsManagerNativeInterface.executeActionSet(resource,
                actionsetName);

        return OCStackResult.conversion(ordinal);
    }

    /**
     * API for executing the Action Set. Callback is called when the response of
     * POST operation arrives.
     *
     * @param resource
     *            resource pointer of the group resource
     * @param actionsetName
     *            Action Set name for removing the Action set
     * @param delay
     *            waiting time for until action set run.
     *
     * @return OCStackResult - return value of this API. It returns OC_STACK_OK
     *         if success.
     */
    public OCStackResult executeActionSet(OcResource resource,
            String actionsetName, long delay) throws OcException {

        OCStackResult result;
        int ordinal = ThingsManagerNativeInterface.executeActionSet(resource,
                actionsetName, delay);
        result = OCStackResult.conversion(ordinal);

        return result;
    }

    /**
     * API for cancelling the Action Set. Callback is called when the response
     * of POST operation arrives.
     *
     * @param resource
     *            resource pointer of the group resource
     * @param actionsetName
     *            Action Set name for removing the Action set
     *
     * @return OCStackResult - return value of this API. It returns OC_STACK_OK
     *         if success.
     */
    public OCStackResult cancelActionSet(OcResource resource,
            String actionsetName) throws OcException {

        OCStackResult result;
        int ordinal = ThingsManagerNativeInterface.cancelActionSet(resource,
                actionsetName);
        result = OCStackResult.conversion(ordinal);

        return result;
    }

    /**
     * API for reading the Action Set. Callback is called when the response of
     * GET operation arrives.
     *
     * @param resource
     *            resource pointer of the group resource
     * @param actionsetName
     *            Action Set name for removing the Action set
     *
     * @return OCStackResult - return value of this API. It returns OC_STACK_OK
     *         if success.
     */
    public OCStackResult getActionSet(OcResource resource, String actionsetName)
            throws OcException {

        OCStackResult result;
        int ordinal = ThingsManagerNativeInterface.getActionSet(resource,
                actionsetName);
        result = OCStackResult.conversion(ordinal);

        return result;
    }

    /**
     * API for removing the action set. Callback is called when the response of
     * POST operation arrives.
     *
     * @param resource
     *            resource pointer of the group resource
     * @param actionsetName
     *            Action Set name for removing the Action set
     *
     * @return OCStackResult - return value of this API. It returns OC_STACK_OK
     *         if success.
     */
    public OCStackResult deleteActionSet(OcResource resource,
            String actionsetName) throws OcException {

        OCStackResult result;
        int ordinal = ThingsManagerNativeInterface.deleteActionSet(resource,
                actionsetName);
        result = OCStackResult.conversion(ordinal);

        return result;
    }
}

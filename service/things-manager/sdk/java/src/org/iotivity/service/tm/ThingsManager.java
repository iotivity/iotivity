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
 * @file    ThingsManager.java
 *
 * @brief    This file provides a class for a set of APIs relating to group management,
 *        synchronization of group, configuration of things, and diagnostics
 *        about things.
 *
 */

package org.iotivity.service.tm;

import java.util.Map;
import java.util.Vector;

import org.iotivity.base.OcException;
import org.iotivity.base.OcResource;
import org.iotivity.base.OcResourceHandle;

import android.util.Log;

/**
 * @class ThingsManager
 * @brief This class provides a set of APIs relating to group management,
 *        synchronization of group, configuration of things, and diagnostics
 *        about things.
 *
 */
public class ThingsManager {

    private IFindCandidateResourceListener resourceListener;
    private ISubscribePresenceListener     presenceListener;
    private IFindGroupListener             groupListener;
    private IConfigurationListener         configurationListener;
    private IDiagnosticsListener           diagnosticsListener;
    private IActionListener                actionListener;
    private final String                   LOG_TAG = this.getClass()
                                                           .getSimpleName();

    private final ThingsManagerCallback    thingsManagerCallbackInterfaceObj;
    private final ThingsManagerInterface   thingsManagerInterfaceObj;

    static {
        System.loadLibrary("gnustl_shared");
        System.loadLibrary("oc_logger_core");
        System.loadLibrary("oc_logger");
        System.loadLibrary("coap");
        System.loadLibrary("octbstack");
        System.loadLibrary("oc");
        System.loadLibrary("TGMSDKLibrary");
        System.loadLibrary("ocstack-jni");
        System.loadLibrary("things-manager-jni");
    }

    /**
     * Constructor for ThingsManager. Constructs a new ThingsManager.
     */
    public ThingsManager() {
        thingsManagerCallbackInterfaceObj = ThingsManagerCallback.getInstance();
        thingsManagerInterfaceObj = ThingsManagerInterface.getInstance();
    }

    /**
     * Set listener for receiving notification of resource discovery.
     *
     * @param listener
     *            - @ref IFindCandidateResourceListener to receive the
     *            discovered resources.
     */
    public void setFindCandidateResourceListener(
            IFindCandidateResourceListener listener) {
        resourceListener = listener;
        thingsManagerCallbackInterfaceObj
                .registerFindCandidateResourceListener(listener);
        thingsManagerInterfaceObj
                .registerFindCandidateResourceListener(listener);
    }

    /**
     * Set listener for receiving child resource presence notifications.
     *
     * @param listener
     *            - @ref ISubscribePresenceListener to receive child resource
     *            presence state.
     */
    public void setSubscribePresenceListener(ISubscribePresenceListener listener) {
        presenceListener = listener;
        thingsManagerCallbackInterfaceObj
                .registerSubscribePresenceListener(listener);
        thingsManagerInterfaceObj.registerSubscribePresenceListener(listener);
    }

    /**
     * Set listener for receiving notification on whether the requested group is
     * found or not.
     *
     * @param listener
     *            - @ref IFindGroupListener to receive group discovery status.
     */
    public void setGroupListener(IFindGroupListener listener) {
        groupListener = listener;
        thingsManagerCallbackInterfaceObj.registerGroupListener(listener);
        thingsManagerInterfaceObj.registerGroupListener(listener);
    }

    /**
     * Set listener for receiving asynchronous response for configuration APIs.
     *
     * @param listener
     *            - @ref IConfigurationListener to receive asynchronous response
     *            for configuration APIs.
     */
    public void setConfigurationListener(IConfigurationListener listener) {
        configurationListener = listener;
        thingsManagerCallbackInterfaceObj
                .registerConfigurationListener(listener);
        thingsManagerInterfaceObj.registerConfigurationListener(listener);
    }

    /**
     * Set listener for receiving asynchronous response for diagnostic feature
     * APIs.
     *
     * @param listener
     *            - @ref IDiagnosticsListener to receive asynchronous response
     *            for diagnostic feature APIs.
     */
    public void setDiagnosticsListener(IDiagnosticsListener listener) {
        diagnosticsListener = listener;
        thingsManagerCallbackInterfaceObj.registerDiagnosticsListener(listener);
    }

    /**
     * Set listener for receiving notifications of Get, PUT and POST resource.
     *
     * @param listener
     *            - @ref IActionListener to receive Get, PUT and POST request
     *            callbacks.
     */
    public void setActionListener(IActionListener listener) {
        actionListener = listener;
        thingsManagerCallbackInterfaceObj.registerActionListener(listener);
    }

    /**
     * API for discovering candidate resources with waiting delay. This
     * operation returns all resources of given type on the network service.
     * This operation is sent via multicast to all services. However, the filter
     * limits the responders to just those that support the resource type in the
     * query. Currently only exact matches are supported.
     *
     * @param resourceTypes
     *            - required resource types(called "candidate")
     * @param waitTime
     *            - Delay time to add in seconds before starting to find the
     *            resources in network.
     *
     * @return OCStackResult - OC_STACK_OK on success, otherwise a failure error
     *         code.
     *
     * @pre Listener should be set using @ref setFindCandidateResourceListener
     *      API
     *
     * @post Listener @ref IFindCandidateResourceListener::onResourceCallback
     *       will be notified when resource is discovered in network.
     *
     * @code
     * Sample code:
     * public void findLightResource() {
     *       Vector<String> resourceTypes =
     *       new Vector<String>(); resourceTypes.add("core.light");
     *
     *       OCStackResult result = thingsManagerObj.findCandidateResources(resourceTypes, 5);
     *       if (OCStackResult.OC_STACK_OK != result) {
     *           Log.e(LOG_TAG, "Error while calling findCandidateResources");
     *       }
     * }
     * @endcode
     *
     */
    public OCStackResult findCandidateResources(Vector<String> resourceTypes,
            int waitTime) {
        OCStackResult result;
        if (null == resourceListener) {
            result = OCStackResult.OC_STACK_LISTENER_NOT_SET;
        } else {
            result = thingsManagerInterfaceObj.findCandidateResources(
                    resourceTypes, waitTime);
            Log.i(LOG_TAG, "findCandidateResources" + result.name());
        }
        return result;
    }

    /**
     * API for subscribing child's state. It subscribes the presence state of
     * the resource. By making this subscription, every time a server
     * adds/removes/alters a resource, starts or is intentionally stopped, a
     * notification is sent to subscriber.
     *
     * @param resource
     *            - collection resource for subscribing presence of all child
     *            resources
     *
     * @return OCStackResult - OC_STACK_OK on success, otherwise a failure error
     *         code.
     *
     * @pre Listener should be set using @ref setSubscribePresenceListener API.
     *
     * @post Listener @ref ISubscribePresenceListener::onPresenceCallback will
     *       be notified for resource presence status
     *
     */
    public OCStackResult subscribeCollectionPresence(OcResource resource)
            throws OcException {
        OCStackResult result;
        if (null == presenceListener) {
            result = OCStackResult.OC_STACK_LISTENER_NOT_SET;
        } else {
            result = thingsManagerInterfaceObj
                    .subscribeCollectionPresence(resource);
            Log.i(LOG_TAG, "subscribeCollectionPresence" + result.name());
        }
        return result;
    }

    /**
     * API for finding a specific remote group when a resource tries to join a
     * group.
     *
     * @param collectionResourceTypes
     *            - resource types of a group to find and join
     *
     * @return OCStackResult - OC_STACK_OK on success, otherwise a failure error
     *         code.
     *
     * @pre Listener should be set using @ref setGroupListener API
     *
     * @post Listener @ref IFindGroupListener::onGroupFindCallback will be
     *       called to notify whenever a requested group is found.
     *
     * @code
     * Sample code:
     * public void findGroup() {
     *     resourceTypes = new Vector<String>;
     *     resourceTypes.add("b.collection");
     *     result = thingsManagerObj.findGroup(resourceTypes);
     *     if (OCStackResult.OC_STACK_OK != result) {
     *         Log.e(LOG_TAG, "Error while calling findGroup");
     *     }
     * }
     * @endcode
     *
     */
    public OCStackResult findGroup(Vector<String> collectionResourceTypes) {
        OCStackResult result;
        if (null == groupListener) {
            result = OCStackResult.OC_STACK_ERROR;
        } else {
            result = thingsManagerInterfaceObj.findGroup(collectionResourceTypes);
            Log.i(LOG_TAG, "findGroup" + result.name());
        }
        return result;
    }

    /**
     * API for creating a new group.
     *
     * @param collectionResourceType
     *            - resource type of a group to create
     *
     * @return OCStackResult - OC_STACK_OK on success, otherwise a failure error
     *         code.
     */
    public OCStackResult createGroup(String collectionResourceType) {
        OCStackResult result;
        result = thingsManagerInterfaceObj.createGroup(collectionResourceType);
        Log.i(LOG_TAG, "createGroup" + result.name());
        return result;
    }

    /**
     * API for joining an existing group. This API is used when a resource that
     * has a group tries to find a specific remote resource and makes it join a
     * group.
     *
     * @param collectionResourceType
     *            - resource type of a group to join.
     * @param resourceHandle
     *            - resource handle to join a group.
     *
     * @return OCStackResult - OC_STACK_OK on success, otherwise a failure error
     *         code.
     *
     */
    public OCStackResult joinGroup(String collectionResourceType,
            OcResourceHandle resourceHandle) {
        OCStackResult result;
        result = thingsManagerInterfaceObj.joinGroup(collectionResourceType,
                resourceHandle);
        Log.i(LOG_TAG, "joinGroup" + result.name());
        return result;
    }

    /**
     * API for joining an existing group. This API is used when a resource that
     * doesn't have a group tries to find and join a specific remote group.
     *
     * @param resource
     *            - group resource to join.
     * @param resourceHandle
     *            - resource handle to join a group.
     *
     * @return OCStackResult - OC_STACK_OK on success, otherwise a failure error
     *         code.
     *
     */
    public OCStackResult joinGroup(OcResource resource,
            OcResourceHandle resourceHandle) throws OcException {
        OCStackResult result;
        result = thingsManagerInterfaceObj.joinGroup(resource, resourceHandle);
        Log.i(LOG_TAG, "joinGroup" + result.name());
        return result;
    }

    /**
     * API for leaving a joined group.
     *
     * @param collectionResourceType
     *            - resource type of a group to leave.
     * @param resourceHandle
     *            - resource handle to leave a group.
     *
     * @return OCStackResult - OC_STACK_OK on success, otherwise a failure error
     *         code.
     *
     */
    public OCStackResult leaveGroup(String collectionResourceType,
            OcResourceHandle resourceHandle) {
        OCStackResult result;
        result = thingsManagerInterfaceObj.leaveGroup(collectionResourceType,
                resourceHandle);
        Log.i(LOG_TAG, "leaveGroup" + result.name());
        return result;
    }

    /**
     * API for leaving a joined group.
     *
     * @param resource
     *            - resource of a group to leave.
     * @param collectionResourceType
     *            - resource type of a group to leave.
     * @param resourceHandle
     *            - resource handle to leave a group.
     *
     * @return OCStackResult - OC_STACK_OK on success, otherwise a failure error
     *         code.
     *
     */
    public OCStackResult leaveGroup(OcResource resource, String collectionResourceType,
            OcResourceHandle resourceHandle) {
        OCStackResult result;
        result = thingsManagerInterfaceObj.leaveGroup(resource, collectionResourceType,
                resourceHandle);
        Log.i(LOG_TAG, "leaveGroup" + result.name());
        return result;
    }

    /**
     * API for deleting a created group.
     *
     * @param collectionResourceType
     *            - resource type of a group to delete.
     *
     * @return OCStackResult - OC_STACK_OK on success, otherwise a failure error
     *         code.
     *
     */
    public void deleteGroup(String collectionResourceType) {
        thingsManagerInterfaceObj.deleteGroup(collectionResourceType);
    }

    /**
     * API for getting a list of joined groups.
     *
     * @return Returns the map with the resource type of a group and group
     *         resource handle.
     *
     * @code
     * Sample code:
     * public Map<String, OcResourceHandle> getGroupList() {
     *     Map<String, OcResourceHandle> groupMap = null;
     *     groupMap = thingsManagerObj.getGroupList();
     *     return groupMap;
     * }
     * @endcode
     *
     */
    public Map<String, OcResourceHandle> getGroupList() {
        Map<String, OcResourceHandle> groupMap = null;
        groupMap = thingsManagerInterfaceObj.getGroupList();
        return groupMap;
    }

    /**
     * API for registering and binding a resource to group.
     *
     * @param childHandle
     *            - child resource handle. It will be filled from resource
     *            parameter.
     * @param resource
     *            - resource for register and bind to group. It has all data.
     * @param collectionHandle
     *            - collection resource handle. It will be the added child
     *            resource.
     *
     * @return OCStackResult - OC_STACK_OK on success, otherwise a failure error
     *         code.
     *
     */
    public OCStackResult bindResourceToGroup(OcResourceHandle childHandle,
            OcResource resource, OcResourceHandle collectionHandle)
            throws OcException {
        OCStackResult result;
        result = thingsManagerInterfaceObj.bindResourceToGroup(childHandle,
                resource, collectionHandle);
        Log.i(LOG_TAG, "bindResourceToGroup" + result.name());
        return result;
    }

    /**
     * API for updating configuration value of multiple things of a target group
     * or a single thing to a resource server(s).
     *
     * @param resource
     *            - resource representing the target group or the single thing.
     *            It is a pointer of resource instance of Configuration
     *            resource. The resource pointer can be acquired by performing
     *            findResource() function with a dedicated resource type,
     *            "oic.con". Note that, the resource pointer represents not only
     *            a single simple resource but also a collection resource
     *            composing multiple simple resources. In other words, using
     *            these APIs, developers can send a series of requests to
     *            multiple things by calling the corresponding function at once.
     * @param configurations
     *            - ConfigurationUnit: a nickname of attribute of target
     *            resource (e.g., installed location, currency, (IP)address)
     *            Value : a value to be updated. It represents an indicator of
     *            which resource developers want to access and which value
     *            developers want to update. Basically, developers could use a
     *            resource URI to access a specific resource but a resource URI
     *            might be hard for all developers to memorize lots of URIs,
     *            especially in the case of long URIs. To relieve the problem,
     *            Things Configuration introduces a easy-memorizing name, called
     *            ConfigurationName, instead of URI. And ConfigurationValue used
     *            in updateConfiguration() function indicates a value to be
     *            updated. Note that, only one configuration parameter is
     *            supported in this release. Multiple configuration parameters
     *            will be supported in future release.
     *
     * @return OCStackResult - OC_STACK_OK on success, otherwise a failure error
     *         code.
     *
     * @pre Listener should be set using @ref setConfigurationListener API.
     *
     * @post Listener @ref
     *       IConfigurationListener::onUpdateConfigurationsCallback will be
     *       notified when the response of update configuration arrives.
     *
     * @code
     * Sample code:
     * private void updateConfiguration() {
     *     String name = "region";
     *     String value = "INDIA";
     *     Map<String, String> configurations = new HashMap<String, String>();
     *
     *     try {
     *            configurations.put(name, value);
     *     } catch (Exception e) {
     *            Log.e(LOG_TAG, "Exception occured! " + e.toString());
     *     }
     *
     *     OCStackResult result = OCStackResult.values()[30];
     *
     *     try {
     *            result = thingsManager.updateConfigurations(resource, configurations);
     *     } catch (OcException e) {
     *             e.printStackTrace();
     *     }
     *
     *     if (OCStackResult.OC_STACK_OK != result) {
     *         Log.e(LOG_TAG, "updateConfigurations failed!");
     *     }
     * }
     * @endcode
     *
     */
    public OCStackResult updateConfigurations(OcResource resource,
            Map<String, String> configurations) throws OcException {
        OCStackResult result;
        if (null == configurationListener) {
            result = OCStackResult.OC_STACK_LISTENER_NOT_SET;
        } else {
            result = thingsManagerInterfaceObj.updateConfigurations(resource,
                    configurations);
            Log.i(LOG_TAG, "updateConfigurations" + result.name());
        }
        return result;
    }

    /**
     * API for getting configuration value of multiple things of a target group
     * or a single thing.To get a value, users need to know a Configuration Name
     * indicating the target resource. In this release, the Configuration Name
     * is "configuration".An update value is not needed. After that, users store
     * them in form of a Vector and then use a getConfigurations() function.
     *
     * @param resource
     *            - resource representing the target group or the single thing.
     * @param configurations
     *            - ConfigurationUnit: a nickname of attribute of target
     *            resource.
     *
     * @return OCStackResult - OC_STACK_OK on success, otherwise a failure error
     *         code.
     *
     * @code
     * Sample code:
     * private void getConfiguration() {
     *     String name = "configuration";
     *     Vector<String> configs = new Vector<String>();
     *     configs.add(name);
     *
     *     OCStackResult result = OCStackResult.values()[30];
     *     try {
     *         result = thingsManager.getConfigurations(resource, configs);
     *     } catch (OcException e) {
     *         e.printStackTrace();
     *     }
     *
     *     if (OCStackResult.OC_STACK_OK != result) {
     *         Log.e(LOG_TAG, "getConfigurations failed!");
     *     }
     * }
     * @endcode
     *
     */
    public OCStackResult getConfigurations(OcResource resource,
            Vector<String> configurations) throws OcException {
        OCStackResult result;
        if (null == configurationListener) {
            result = OCStackResult.OC_STACK_LISTENER_NOT_SET;
        } else {
            result = thingsManagerInterfaceObj.getConfigurations(resource,
                    configurations);
            Log.i(LOG_TAG, "getConfigurations" + result.name());
        }
        return result;

    }

    /**
     * API for showing the list of supported configuration units (configurable
     * parameters). It shows which Configuration Names are supported and their
     * brief descriptions. This information is provided in JSON format.
     *
     * @return Returns the configuration list in JSON format.
     *
     */
    public String getListOfSupportedConfigurationUnits() {
        return thingsManagerInterfaceObj.getListOfSupportedConfigurationUnits();
    }

    /**
     * API for bootstrapping system configuration parameters from a bootstrap
     * server.
     *
     * @return OCStackResult - OC_STACK_OK on success, otherwise a failure error
     *         code.
     *
     * @pre Listener should be set using @ref setConfigurationListener API.
     *
     * @post Listener @ref IConfigurationListener::onBootStrapCallback will be
     *       notified when the response arrives.
     *
     */
    public OCStackResult doBootstrap() {
        OCStackResult result;
        if (null == diagnosticsListener) {
            result = OCStackResult.OC_STACK_LISTENER_NOT_SET;
        } else {
            result = thingsManagerInterfaceObj.doBootstrap();
            Log.i(LOG_TAG, "doBootstrap" + result.name());
        }
        return result;
    }

    /**
     * API to is used to send a request to a server(thing or device) to be
     * rebooted. On receiving the request, the server attempts to reboot itself
     * in a deterministic time. The target thing could be a group of multiple
     * things or a single thing.
     *
     * @param resource
     *            - resource pointer representing the target group
     *
     * @return OCStackResult - OC_STACK_OK on success, otherwise a failure error
     *         code.
     *
     * @pre Listener should be set using @ref setDiagnosticsListener API.
     *
     * @post Listener @ref IDiagnosticsListener::onRebootCallback will be
     *       notified when the response arrives.
     *
     */
    public OCStackResult reboot(OcResource resource) throws OcException {
        OCStackResult result;
        if (null == diagnosticsListener) {
            result = OCStackResult.OC_STACK_LISTENER_NOT_SET;
        } else {
            result = thingsManagerInterfaceObj.reboot(resource);
            Log.i(LOG_TAG, "reboot" + result.name());
        }
        return result;
    }

    /**
     * API to restore all configuration parameters to default one on
     * thing(device). All configuration parameters refers Configuration
     * resource, which they could have been modified for various reasons (e.g.,
     * for a request to update a value). If developers on the client want to
     * restore the parameters, just use the factoryReset function.The target
     * thing could be a group of multiple things or a single thing.
     *
     * @param resource
     *            - resource pointer representing the target group
     *
     * @return OCStackResult - OC_STACK_OK on success, otherwise a failure error
     *         code.
     *
     * @pre Listener should be set using @ref setDiagnosticsListener API.
     *
     * @post Listener @ref IDiagnosticsListener::onFactoryResetCallback will be
     *       notified when the response arrives.
     *
     */
    public OCStackResult factoryReset(OcResource resource) throws OcException {
        OCStackResult result;
        if (null == diagnosticsListener) {
            result = OCStackResult.OC_STACK_LISTENER_NOT_SET;
        } else {
            result = thingsManagerInterfaceObj.factoryReset(resource);
            Log.i(LOG_TAG, "factoryReset" + result.name());
        }
        return result;
    }

    /**
     * API for adding a new ActionSet onto a specific resource.
     *
     * @param resource
     *            - resource pointer of the group resource
     * @param actionSet
     *            - pointer of ActionSet
     *
     * @return OCStackResult - OC_STACK_OK on success, otherwise a failure error
     *         code.
     *
     * @pre Listener should be set using @ref setActionListener API.
     *
     * @post Listener @ref IActionListener::onPutResponseCallback will be
     *       notified when the response of PUT operation arrives.
     *
     * @code
     * Sample code:
     * public void createActionSet_AllBulbOff() {
     *     ActionSet actionSet = new ActionSet();
     *     Vector<Capability> listOfCap;
     *     Action action = new Action();
     *     Capability capability = new Capability();
     *     Vector<Action> listOfAction = null;
     *     Vector<String> lights = new Vector<String>();
     *     int size = lights.size();
     *     if (size == 0) {
     *         return;
     *     }
     *     for (int i = 0; i < size; i++) {
     *         action.target = lights.get(i);
     *         capability.capability = "power";
     *         capability.status = "off";
     *         listOfCap = new Vector<Capability>();
     *         listOfCap.add(capability);
     *
     *         action.listOfCapability = listOfCap;
     *         listOfAction = new Vector<Action>();
     *         listOfAction.add(action);
     *     }
     *
     *     actionSet.actionsetName = BULBOFF;
     *     actionSet.listOfAction = listOfAction;
     *     String URI = foundResources.getUri();
     *     String host = foundResources.getHost();
     *
     *     OCStackResult result = OCStackResult.values()[30];
     *
     *     try {
     *         result = thingsManagerObj.addActionSet(resource, actionSet);
     *     } catch (OcException e) {
     *         e.printStackTrace();
     *     }
     *
     *     if (OCStackResult.OC_STACK_OK != result) {
     *         Log.e(LOG_TAG, "Error while calling addActionSet : " + result.name());
     *     } else {
     *         Log.i(LOG_TAG, "Success : createActionSet_AllBulbOn : " + result.name());
     *     }
     * }
     * @endcode
     *
     */
    public OCStackResult addActionSet(OcResource resource, ActionSet actionSet)
            throws OcException {
        OCStackResult result;
        if (null == actionListener) {
            result = OCStackResult.OC_STACK_LISTENER_NOT_SET;
        } else {
            result = thingsManagerInterfaceObj
                    .addActionSet(resource, actionSet);
            Log.i(LOG_TAG, "addActionSet" + result.name());
        }
        return result;
    }

    /**
     * API for executing a specific ActionSet belonging to a specific resource.
     *
     * @param resource
     *            - resource pointer of the group resource
     * @param actionsetName
     *            - ActionSet name for removing the ActionSet
     *
     * @return OCStackResult - OC_STACK_OK on success, otherwise a failure error
     *         code.
     *
     * @pre Listener should be set using @ref setActionListener API.
     *
     * @post Listener @ref IActionListener::onPostResponseCallback will be
     *       notified when the response of POST operation arrives.
     *
     * @code
     * Sample code:
     * public void executeActionSetOn() {
     *     OCStackResult result = OCStackResult.values()[30];
     *
     *     try {
     *         result = thingsManagerObj.executeActionSet(resource, "AllBulbOn");
     *     } catch (OcException e) {
     *         e.printStackTrace();
     *     }
     *
     *     if (OCStackResult.OC_STACK_OK != result) {
     *         Log.e(LOG_TAG, "Error while calling executeActionSet : " + result.name());
     *     } else {
     *        Log.i(LOG_TAG, "executeActionSetOn Success : " + result.name());
     *     }
     * }
     * @endcode
     *
     */
    public OCStackResult executeActionSet(OcResource resource,
            String actionsetName) throws OcException {
        OCStackResult result;
        if (null == actionListener) {
            result = OCStackResult.OC_STACK_LISTENER_NOT_SET;
        } else {
            result = thingsManagerInterfaceObj.executeActionSet(resource,
                    actionsetName);
            Log.i(LOG_TAG, "executeActionSet" + result.name());
        }
        return result;
    }

    /**
     * API for executing a specific ActionSet belonging to a specific resource.
     *
     * @param resource
     *            - resource pointer of the group resource
     * @param actionsetName
     *            - ActionSet name for removing the ActionSet
     * @param delay
     *            - Wait time for the ActionSet execution
     *
     * @return OCStackResult - OC_STACK_OK on success, otherwise a failure error
     *         code.
     *
     * @pre Listener should be set using @ref setActionListener API.
     *
     * @post Listener @ref IActionListener::onPostResponseCallback will be
     *       notified when the response of POST operation arrives.
     *
     * @code
     * Sample code:
     * public void executeActionSetOn() {
     *     OCStackResult result = OCStackResult.values()[30];
     *
     *     try {
     *         result = thingsManagerObj.executeActionSet(resource, "AllBulbOn", 1);
     *     } catch (OcException e) {
     *         e.printStackTrace();
     *     }
     *
     *     if (OCStackResult.OC_STACK_OK != result) {
     *         Log.e(LOG_TAG, "Error while calling executeActionSet : " + result.name());
     *     } else {
     *        Log.i(LOG_TAG, "executeActionSetOn Success : " + result.name());
     *     }
     * }
     * @endcode
     *
     */
    public OCStackResult executeActionSet(OcResource resource,
            String actionsetName, long delay) throws OcException {
        OCStackResult result;
        if (null == actionListener) {
            result = OCStackResult.OC_STACK_LISTENER_NOT_SET;
        } else {
            result = thingsManagerInterfaceObj.executeActionSet(resource,
                    actionsetName, delay);
            Log.i(LOG_TAG, "executeActionSet" + result.name());
        }
        return result;
    }

    /**
     * API to cancel the existing ActionSet.
     *
     * @param resource
     *            - resource pointer of the group resource.
     * @param actionsetName
     *            - ActionSet name that has to be cancelled.
     *
     * @return OCStackResult - OC_STACK_OK on success, otherwise a failure error
     *         code.
     *
     * @pre Listener should be set using @ref setActionListener API.
     *
     * @post Listener @ref IActionListener::onPostResponseCallback will be
     *       notified when the response of POST operation arrives.
     *
     * @code
     * Sample code:
     * public void cancelActionSetOn() {
     *     OCStackResult result = OCStackResult.values()[30];
     *
     *     try {
     *         result = thingsManagerObj.cancelActionSet(resource, "AllBulbOn");
     *     } catch (OcException e) {
     *         e.printStackTrace();
     *     }
     *
     *     if (OCStackResult.OC_STACK_OK != result) {
     *        Log.e(LOG_TAG, "Error while calling cancelActionSet : " + result.name());
     *     } else {
     *        Log.i(LOG_TAG, "cancelActionSetOn Success : " + result.name());
     *     }
     * }
     * @endcode
     *
     */

    public OCStackResult cancelActionSet(OcResource resource,
            String actionsetName) throws OcException {
        OCStackResult result;
        result = thingsManagerInterfaceObj.cancelActionSet(resource,
                actionsetName);
        Log.i(LOG_TAG, "executeActionSet" + result.name());
        return result;
    }

    /**
     * API to to get an existing ActionSet belonging to a specific resource.
     *
     * @param resource
     *            - resource pointer of the group resource
     * @param actionsetName
     *            - ActionSet name for removing the ActionSet
     *
     * @return OCStackResult - OC_STACK_OK on success, otherwise a failure error
     *         code.
     *
     * @pre Listener should be set using @ref setActionListener API.
     *
     * @post Listener @ref IActionListener::onPostResponseCallback will be
     *       notified when the response of POST operation arrives.
     *
     * @code
     * Sample code:
     * public void getActionSetOff() {
     *     OCStackResult ocStackResult = OCStackResult.values()[30];
     *     try {
     *         ocStackResult = thingsManagerObj.getActionSet(resource, "AllBulbOff");
     *     } catch (OcException e) {
     *         e.printStackTrace();
     *     }
     *
     *     if (OCStackResult.OC_STACK_OK != ocStackResult) {
     *         Log.e(LOG_TAG, "Error while calling getActionSetOff : "+ ocStackResult.name());
     *     } else {
     *         Log.i(LOG_TAG, "Success : getActionSetOff : " + ocStackResult.name());
     *     }
     * }
     * @endcode
     *
     */
    public OCStackResult getActionSet(OcResource resource, String actionsetName)
            throws OcException {
        OCStackResult result;
        if (null == actionListener) {
            result = OCStackResult.OC_STACK_LISTENER_NOT_SET;
        } else {
            result = thingsManagerInterfaceObj.getActionSet(resource,
                    actionsetName);
            Log.i(LOG_TAG, "getActionSet" + result.name());
        }
        return result;
    }

    /**
     * API to delete an existing ActionSet belonging to a specific resource.
     *
     * @param resource
     *            - resource pointer of the group resource
     * @param actionsetName
     *            - ActionSet name for removing the ActionSet
     *
     * @return OCStackResult - OC_STACK_OK on success, otherwise a failure error
     *         code.
     *
     * @pre Listener should be set using @ref setActionListener API.
     *
     * @post Listener @ref IActionListener::onPutResponseCallback will be
     *       notified when the response of PUT operation arrives.
     *
     * @code
     * Sample code:
     * public void deleteActionSetoff() {
     *     OCStackResult result = OCStackResult.values()[30];
     *
     *     try {
     *             result = thingsManagerObj.deleteActionSet(resource, "AllBulbOff");
     *     } catch (OcException e) {
     *             e.printStackTrace();
     *     }
     *
     *     if (OCStackResult.OC_STACK_OK != result) {
     *         Log.e(LOG_TAG, "Error while calling deleteActionSet : ");
     *     } else {
     *         Log.i(LOG_TAG, "deleteActionSetoff Success : " + result.name());
     *     }
     * }
     * @endcode
     *
     */
    public OCStackResult deleteActionSet(OcResource resource,
            String actionsetName) throws OcException {
        OCStackResult result;
        if (null == actionListener) {
            result = OCStackResult.OC_STACK_LISTENER_NOT_SET;
        } else {
            result = thingsManagerInterfaceObj.deleteActionSet(resource,
                    actionsetName);
            Log.i(LOG_TAG, "deleteActionSet" + result.name());
        }
        return result;
    }

}

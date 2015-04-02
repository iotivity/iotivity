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
package com.tm.sample;

import java.util.EnumSet;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Vector;

import org.iotivity.base.ObserveType;
import org.iotivity.base.OcException;
import org.iotivity.base.OcHeaderOption;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcResource;
import org.iotivity.base.OcResource.OnObserveListener;
import org.iotivity.base.OcResourceHandle;
import org.iotivity.base.ResourceProperty;
import org.iotivity.service.tm.Action;
import org.iotivity.service.tm.ActionSet;
import org.iotivity.service.tm.Capability;
import org.iotivity.service.tm.IFindCandidateResourceListener;
import org.iotivity.service.tm.IFindGroupListener;
import org.iotivity.service.tm.OCStackResult;
import org.iotivity.service.tm.ThingsManager;
import org.iotivity.service.tm.Time.ActionSetType;

import android.os.Message;
import android.util.Log;

/**
 * For calling the group APIs as per user selection on UI and for updating the
 * UI
 */
public class GroupClient {

    private static final String     LOG_TAG             = "GroupClient";
    private static Message          msg;
    public String                   logMessage;

    private final String            groupResourceURI    = "/core/b/collection";
    private final String            groupResourceType   = "b.collection";

    private final ThingsManager     thingsManagerObj;
    private final ActionListener    actionListener;
    private final ObserveListener   observeListener;
    private OcResource              groupResource;
    private OcResourceHandle        groupResourceHandle;
    private OcResourceHandle        foundLightHandle;
    private static GroupApiActivity groupApiActivityObj = null;
    public static Vector<String>    lights              = new Vector<String>();
    public static Vector<String>    bookmarks           = new Vector<String>();

    /**
     * Listener for receiving observe notifications.
     */
    private class ObserveListener implements OnObserveListener {
        @Override
        public void onObserveCompleted(List<OcHeaderOption> headerOptionList,
                OcRepresentation ocRepresentation, int sequenceNumber) {
            Log.i(LOG_TAG, "onObserveCompleted invoked");
            if (0 == ocRepresentation.getValueInt("level")) {
                createActionSetBulbOn();
                executeActionSetBulbOn(0);
            } else if (5 == ocRepresentation.getValueInt("level")) {
                createActionSetBulbOff();
                executeActionSetBulbOff(0);
            }
        }
    }

    /**
     * Listener for receiving groups discovered in network.
     */
    private class FindGroupListener implements IFindGroupListener {
        @Override
        public void onGroupFindCallback(OcResource resource) {
            Log.i(LOG_TAG, "onGroupFindCallback invoked");
            if (resource != null) {
                String uri = resource.getUri();
                if (uri.equals(groupResourceURI) == true) {
                    String hostAddress = resource.getHost();
                    Log.d("URI: onGroupFindCallback", uri);
                    Log.d("HOST: onGroupFindCallback", hostAddress);

                    groupResource = resource;
                    Message msg = Message.obtain();
                    msg.what = 0;
                    groupApiActivityObj.getHandler().sendMessage(msg);

                    logMessage = "onGroupFind" + "\n";
                    logMessage = logMessage + "URI : " + uri + "\n";
                    logMessage = logMessage + "Host :" + hostAddress;
                    GroupApiActivity.setMessageLog(logMessage);
                    msg = Message.obtain();
                    msg.what = 1;
                    groupApiActivityObj.getHandler().sendMessage(msg);
                } else {
                    Log.d("onGroupFindCallback URI : ", uri);
                }
            }
        }
    };

    /**
     * Listener for receiving Light resource and Observe resources discovered in
     * network.
     */
    private class FindCadidateResourceListener implements
            IFindCandidateResourceListener {
        @Override
        public void onResourceCallback(Vector<OcResource> resources) {
            Log.i(LOG_TAG, "onResourceCallback invoked");

            if (resources != null) {
                for (int i = 0; i < resources.size(); i++) {
                    Log.d(LOG_TAG, "Resource information");
                    OcResource ocResource = resources.get(i);
                    String resourceURI = ocResource.getUri();
                    String hostAddress = ocResource.getHost();

                    logMessage = "API RESULT : " + "OC_STACK_OK" + "\n";
                    logMessage = logMessage + "URI: " + resourceURI + "\n";
                    logMessage = logMessage + "Host:" + hostAddress;
                    GroupApiActivity.setMessageLog(logMessage);
                    msg = Message.obtain();
                    msg.what = 1;
                    groupApiActivityObj.getHandler().sendMessage(msg);

                    if (resourceURI.equals("/a/light") == true) {
                        if (lights.contains((hostAddress + resourceURI)) == false) {
                            lights.add((hostAddress + resourceURI));
                            if (groupApiActivityObj != null) {

                                logMessage = "API RESULT : " + "OC_STACK_OK"
                                        + "\n";
                                logMessage = logMessage + "URI: " + resourceURI
                                        + "\n";
                                logMessage = logMessage + "Host:" + hostAddress;
                                GroupApiActivity.setMessageLog(logMessage);
                                msg = Message.obtain();
                                msg.what = 1;
                                groupApiActivityObj.getHandler().sendMessage(
                                        msg);

                            }

                            try {
                                Log.i(LOG_TAG, "Registering Resource Handle");
                                foundLightHandle = OcPlatform
                                        .registerResource(ocResource);

                                Log.i(LOG_TAG, "Binding Resource Handle");
                                OcPlatform.bindResource(groupResourceHandle,
                                        foundLightHandle);
                            } catch (OcException e) {
                                e.printStackTrace();
                            }
                        } else {
                            Log.i(LOG_TAG, "Resource is already registered!");
                        }
                    } else if (resourceURI.equalsIgnoreCase("/core/bookmark")) {
                        if (bookmarks.contains((hostAddress + resourceURI)) == false) {
                            bookmarks.add((hostAddress + resourceURI));
                            if (groupApiActivityObj != null) {
                                logMessage = "API RESULT : " + "OC_STACK_OK"
                                        + "\n";
                                logMessage = logMessage + "URI: " + resourceURI
                                        + "\n";
                                logMessage = logMessage + "Host:" + hostAddress;
                                GroupApiActivity.setMessageLog(logMessage);
                                msg = Message.obtain();
                                msg.what = 1;
                                groupApiActivityObj.getHandler().sendMessage(
                                        msg);

                            }
                            observe(ocResource);
                        }
                    }
                }
            }
        }
    };

    private final FindCadidateResourceListener findCandidateResourceListener;
    private final FindGroupListener            findGroupListener;

    public GroupClient() {
        thingsManagerObj = new ThingsManager();
        findGroupListener = new FindGroupListener();
        actionListener = new ActionListener();
        observeListener = new ObserveListener();
        findCandidateResourceListener = new FindCadidateResourceListener();

        thingsManagerObj.setGroupListener(findGroupListener);
        thingsManagerObj
                .setFindCandidateResourceListener(findCandidateResourceListener);
        thingsManagerObj.setActionListener(actionListener);

        groupApiActivityObj = GroupApiActivity.getGroupApiActivityObj();
    }

    /**
     * This method creates group of the type "b.collection" handling light
     * resources.
     */
    public void createGroup() {
        Log.d(LOG_TAG, "Creating group for handling light resources");

        try {
            Log.d(LOG_TAG, "registering group to stack");
            EnumSet<ResourceProperty> tempResourceProperty = EnumSet.of(
                    ResourceProperty.DISCOVERABLE, ResourceProperty.OBSERVABLE);

            // Registering the group with stack
            groupResourceHandle = OcPlatform.registerResource(groupResourceURI,
                    groupResourceType, OcPlatform.BATCH_INTERFACE, null,
                    tempResourceProperty);

            OcPlatform.bindInterfaceToResource(groupResourceHandle,
                    OcPlatform.GROUP_INTERFACE);
            OcPlatform.bindInterfaceToResource(groupResourceHandle,
                    OcPlatform.DEFAULT_INTERFACE);

        } catch (OcException e) {
            Log.e(LOG_TAG,
                    "Exception in bindInterfaceToResource DEFAULT_INTERFACE");
            e.printStackTrace();
        }
    }

    /**
     * This method finds the group of type "b.collection".
     */
    public void findGroup() {
        Log.d(LOG_TAG, "finding group");

        Vector<String> resourceTypes = new Vector<String>();
        resourceTypes.add(groupResourceType);
        OCStackResult result = thingsManagerObj.findGroup(resourceTypes);
        if (OCStackResult.OC_STACK_OK != result) {
            Log.e(LOG_TAG, "findGroup returned error: " + result.name());
            return;
        }

        logMessage = "API RESULT : " + result.toString();
        GroupApiActivity.setMessageLog(logMessage);
        logMessage = "";
        if (groupApiActivityObj != null) {
            msg = Message.obtain();
            msg.what = 1;
            groupApiActivityObj.getHandler().sendMessage(msg);
        }
    }

    /**
     * This method finds the light resources of type "core.light".
     */
    public void findLightResources() {
        Log.d(LOG_TAG, "finding light resources");

        Vector<String> types = new Vector<String>();
        types.add("core.light");
        OCStackResult result = thingsManagerObj
                .findCandidateResources(types, 5);
        if (OCStackResult.OC_STACK_OK != result) {
            Log.e(LOG_TAG,
                    "findCandidateResources returned error: " + result.name());
        }
    }

    /**
     * This method finds the bookmark resources of type "core.bookmark".
     */
    public void findBookMarkResources() {
        Log.d(LOG_TAG, "finding bookmark resources");

        Vector<String> types = new Vector<String>();
        types.add("core.bookmark");
        OCStackResult result = thingsManagerObj
                .findCandidateResources(types, 5);
        if (OCStackResult.OC_STACK_OK != result) {
            Log.e(LOG_TAG,
                    "findCandidateResources returned error: " + result.name());
        }
    }

    /**
     * This method creates the action set for bulb on action.
     */
    public void createActionSetBulbOn() {
        Log.i(LOG_TAG, "creating action set for bulb on action");

        if (lights.size() == 0) {
            groupApiActivityObj
                    .displayToastMessage("No Light server found in network!");
            return;
        }

        ActionSet actionSet = new ActionSet();
        actionSet.actionsetName = "AllBulbOn";

        // Create actions list
        for (int i = 0; i < lights.size(); i++) {
            Action action = new Action();
            action.target = lights.get(i);

            Capability capability = new Capability();
            capability.capability = "power";
            capability.status = "on";

            action.listOfCapability.add(capability);
            actionSet.listOfAction.add(action);
        }

        try {
            OCStackResult result = thingsManagerObj.addActionSet(groupResource,
                    actionSet);
            if (OCStackResult.OC_STACK_OK != result) {
                Log.e(LOG_TAG, "addActionSet returned error: " + result.name());
                return;
            }
        } catch (OcException e) {
            e.printStackTrace();
            return;
        }
    }

    /**
     * This method creates the action set for bulb off action.
     */
    public void createActionSetBulbOff() {
        Log.i(LOG_TAG, "creating action set for bulb off action");

        if (lights.size() == 0) {
            groupApiActivityObj
                    .displayToastMessage("No Light server found in network!");
            return;
        }

        ActionSet actionSet = new ActionSet();
        actionSet.actionsetName = "AllBulbOff";

        // Create actions list
        for (int i = 0; i < lights.size(); i++) {
            Action action = new Action();
            action.target = lights.get(i);

            Capability capability = new Capability();
            capability.capability = "power";
            capability.status = "off";

            action.listOfCapability.add(capability);
            actionSet.listOfAction.add(action);
        }

        try {
            OCStackResult result = thingsManagerObj.addActionSet(groupResource,
                    actionSet);
            if (OCStackResult.OC_STACK_OK != result) {
                Log.e(LOG_TAG, "addActionSet returned error: " + result.name());
                return;
            }
        } catch (OcException e) {
            e.printStackTrace();
            return;
        }
    }

    /**
     * This method creates the recursive action set for bulb on action.
     */
    public void createRecursiveActionSetBulbOn() {
        Log.i(LOG_TAG, "creating recursive action set for bulb on action");

        if (lights.size() == 0) {
            groupApiActivityObj
                    .displayToastMessage("No Light server found in network!");
            return;
        }

        ActionSet actionSet = new ActionSet();
        actionSet.actionsetName = "AllBulbOnRecursive";
        actionSet.setType(ActionSetType.RECURSIVE);
        actionSet.mYear = 0;
        actionSet.mMonth = 0;
        actionSet.mDay = 0;
        actionSet.mHour = 0;
        actionSet.mMin = 0;
        actionSet.mSec = 5;
        actionSet.setDelay(actionSet.getSecAbsTime());

        // Create actions list
        for (int i = 0; i < lights.size(); i++) {
            Action action = new Action();
            action.target = lights.get(i);

            Capability capability = new Capability();
            capability.capability = "power";
            capability.status = "on";

            action.listOfCapability.add(capability);
            actionSet.listOfAction.add(action);
        }

        try {
            OCStackResult result = thingsManagerObj.addActionSet(groupResource,
                    actionSet);
            if (OCStackResult.OC_STACK_OK != result) {
                Log.e(LOG_TAG, "addActionSet returned error: " + result.name());
                return;
            }
        } catch (OcException e) {
            e.printStackTrace();
            return;
        }
    }

    /**
     * This method creates the scheduled action set for bulb off action.
     */
    public void createScheduledActionSetBulbOff(long delay) {
        Log.i(LOG_TAG, "creating scheduled action set for bulb off action");

        if (lights.size() == 0) {
            groupApiActivityObj
                    .displayToastMessage("No Light server found in network!");
            return;
        }

        ActionSet actionSet = new ActionSet();
        actionSet.actionsetName = "AllBulbOffScheduled";
        actionSet.setType(ActionSetType.SCHEDULED);
        actionSet.setDelay(delay);
        Log.i(LOG_TAG, "Set the delay of " + delay + " seconds");

        // Create actions list
        for (int i = 0; i < lights.size(); i++) {
            Action action = new Action();
            action.target = lights.get(i);

            Capability capability = new Capability();
            capability.capability = "power";
            capability.status = "off";

            action.listOfCapability.add(capability);
            actionSet.listOfAction.add(action);
        }

        try {
            OCStackResult result = thingsManagerObj.addActionSet(groupResource,
                    actionSet);
            if (OCStackResult.OC_STACK_OK != result) {
                Log.e(LOG_TAG, "addActionSet returned error: " + result.name());
                return;
            }
        } catch (OcException e) {
            e.printStackTrace();
            return;
        }
    }

    /**
     * This method is for executing the action Set "AllBulbOn".
     */
    public void executeActionSetBulbOn(long delay) {
        Log.i(LOG_TAG, "executing the action set of bulb on action");

        if (lights.size() == 0) {
            groupApiActivityObj
                    .displayToastMessage("No Light server found in network!");
            return;
        }

        executeActionSet("AllBulbOn", delay);
    }

    /**
     * This method is for executing the action Set "AllBulbOff".
     */
    public void executeActionSetBulbOff(long delay) {
        Log.i(LOG_TAG, "executing the action set of bulb off action");

        if (lights.size() == 0) {
            groupApiActivityObj
                    .displayToastMessage("No Light server found in network!");
            return;
        }

        executeActionSet("AllBulbOff", delay);
    }

    /**
     * This method is for executing the recursive action Set
     * "AllBulbOnRecursive".
     */
    public void executeRecursiveActionSetBulbOn(long delay) {
        Log.i(LOG_TAG, "executing the recursive action set of bulb on action");

        if (lights.size() == 0) {
            groupApiActivityObj
                    .displayToastMessage("No Light server found in network!");
            return;
        }

        executeActionSet("AllBulbOnRecursive", delay);
    }

    /**
     * This method is for executing the schedule action Set
     * "AllBulbOffScheduled".
     */
    public void executeScheduledActionSetBulbOff(long delay) {
        Log.i(LOG_TAG, "executing the schedule action set of bulb off action");

        if (lights.size() == 0) {
            groupApiActivityObj
                    .displayToastMessage("No Light server found in network!");
            return;
        }

        executeActionSet("AllBulbOffScheduled", delay);
    }

    /**
     * This method is for canceling the action Set "AllBulbOn".
     */
    public void cancelActionSetBulbOn() {
        Log.i(LOG_TAG, "cancelling the action set of bulb on action");

        if (lights.size() == 0) {
            groupApiActivityObj
                    .displayToastMessage("No Light server found in network!");
            return;
        }

        cancelActionSet("AllBulbOn");
    }

    /**
     * This method is for canceling the action Set "AllBulbOff".
     */
    public void cancelActionSetBulbOff() {
        Log.i(LOG_TAG, "cancelling the action set of bulb off action");

        if (lights.size() == 0) {
            groupApiActivityObj
                    .displayToastMessage("No Light server found in network!");
            return;
        }

        cancelActionSet("AllBulbOff");
    }

    /**
     * This method is for canceling the recursive action Set
     * "AllBulbOnRecursive".
     */
    public void cancelRecursiveActionSetBulbOn() {
        Log.i(LOG_TAG, "cancelling the recursive action set of bulb on action");

        if (lights.size() == 0) {
            groupApiActivityObj
                    .displayToastMessage("No Light server found in network!");
            return;
        }

        cancelActionSet("AllBulbOnRecursive");
    }

    /**
     * This method is for canceling the scheduled action Set
     * "AllBulbOffScheduled".
     */
    public void cancelScheduledActionSetBulbOff() {
        Log.i(LOG_TAG, "cancelling the scheduled action set of bulb off action");

        if (lights.size() == 0) {
            groupApiActivityObj
                    .displayToastMessage("No Light server found in network!");
            return;
        }

        cancelActionSet("AllBulbOffScheduled");
    }

    /**
     * This method is for getting the action Set "AllBulbOn".
     */
    public void getActionSetBulbOn() {
        Log.i(LOG_TAG, "getting the action set of bulb on action");

        try {
            OCStackResult result = thingsManagerObj.getActionSet(groupResource,
                    "AllBulbOn");
            if (OCStackResult.OC_STACK_OK != result) {
                Log.e(LOG_TAG,
                        "getActionSetOn returned error: " + result.name());
                return;
            }
        } catch (OcException e) {
            e.printStackTrace();
        }
    }

    /**
     * This method is for getting the action Set "AllBulbOff".
     */
    public void getActionSetBulbOff() {
        Log.i(LOG_TAG, "getting the action set of bulb off action");

        try {
            OCStackResult result = thingsManagerObj.getActionSet(groupResource,
                    "AllBulbOff");
            if (OCStackResult.OC_STACK_OK != result) {
                Log.e(LOG_TAG,
                        "getActionSetOn returned error: " + result.name());
                return;
            }
        } catch (OcException e) {
            e.printStackTrace();
        }
    }

    /**
     * This method is for deleting the action Set "AllBulbOn".
     */
    public void deleteActionSetBulbOn() {
        Log.i(LOG_TAG, "deleting the action set of bulb on action");

        try {
            OCStackResult result = thingsManagerObj.deleteActionSet(
                    groupResource, "AllBulbOn");
            if (OCStackResult.OC_STACK_OK != result) {
                Log.e(LOG_TAG,
                        "deleteActionSet returned error : " + result.name());
                return;
            }
        } catch (OcException e) {
            e.printStackTrace();
        }
    }

    /**
     * This method is for deleting the action Set "AllBulbOff".
     */
    public void deleteActionSetBulbOff() {
        Log.i(LOG_TAG, "deleting the action set of bulb off action");

        try {
            OCStackResult result = thingsManagerObj.deleteActionSet(
                    groupResource, "AllBulbOff");
            if (OCStackResult.OC_STACK_OK != result) {
                Log.e(LOG_TAG,
                        "deleteActionSet returned error : " + result.name());
                return;
            }
        } catch (OcException e) {
            e.printStackTrace();
        }
    }

    /**
     * This method is for observing the bulb on/off status.
     */
    public void observe(OcResource resource) {
        Log.i(LOG_TAG, "Registering observer for bookmark resources status");

        Map<String, String> queryMap = new HashMap<String, String>();
        try {
            resource.observe(ObserveType.OBSERVE, queryMap, observeListener);
        } catch (OcException e) {
            e.printStackTrace();
        }
    }

    /**
     * This method is for unbinding and unregister all the found resources and
     * groups.
     */
    public void leaveGroup() {
        thingsManagerObj.setGroupListener(null);
        thingsManagerObj.setFindCandidateResourceListener(null);
        thingsManagerObj.setActionListener(null);

        try {
            Log.i(LOG_TAG, "UnRegistering Light Resource");
            if (null != foundLightHandle) {
                OcPlatform.unregisterResource(foundLightHandle);
                lights.clear();
            } else {
                Log.i(LOG_TAG, "No resouceHandle found to unregister");
            }
        } catch (OcException e) {
            e.printStackTrace();
        }

        try {
            if (null != groupResourceHandle) {
                if (null != foundLightHandle) {
                    Log.i(LOG_TAG, "Resource Unbind Done");
                    OcPlatform.unbindResource(groupResourceHandle,
                            foundLightHandle);
                }
                Log.i(LOG_TAG, "Group Unregistered");
                OcPlatform.unregisterResource(groupResourceHandle);
            } else {
                Log.i(LOG_TAG, "foundLightHandle is NULL");
            }
        } catch (OcException e) {
            Log.e(LOG_TAG, "OcException occured! " + e.toString());
        }
    }

    private void executeActionSet(String actonSetName, long delay) {
        try {
            OCStackResult result = thingsManagerObj.executeActionSet(
                    groupResource, actonSetName, delay);
            if (OCStackResult.OC_STACK_OK != result) {
                Log.e(LOG_TAG,
                        "executeActionSet retuned error : " + result.name());
                return;
            }
        } catch (OcException e) {
            e.printStackTrace();
        }
    }

    private void cancelActionSet(String actionSetName) {
        try {
            OCStackResult result = thingsManagerObj.cancelActionSet(
                    groupResource, actionSetName);
            if (OCStackResult.OC_STACK_OK != result) {
                Log.e(LOG_TAG,
                        "cancelActionSet returned error : " + result.name());
                return;
            }
        } catch (OcException e) {
            e.printStackTrace();
        }
    }
}
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
 * This file contains class which provides functions
 * related to Group Synchronization.
 */

package org.iotivity.service.tm;

import java.util.Map;
import java.util.Vector;

import org.iotivity.base.OcException;
import org.iotivity.base.OcResource;
import org.iotivity.base.OcResourceHandle;

import android.util.Log;

/**
 * This class provides a set of APIs relating to Group Synchronization.
 */
public class GroupSynchronization {

    private IFindGroupListener          groupListener;
    private final String                LOG_TAG = this.getClass()
                                                        .getSimpleName();

    private static GroupSynchronization s_groupSynchronizationInstance;

    // Native methods
    private static native int nativeFindGroup(
            Vector<String> collectionResourceTypes);

    private static native int nativeCreateGroup(String collectionResourceType);

    private static native int nativeJoinGroupString(
            String collectionResourceType, OcResourceHandle resourceHandle);

    private static native int nativeJoinGroupObject(OcResource resource,
            OcResourceHandle resourceHandle);

    private static native int nativeLeaveGroup(String collectionResourceType,
            OcResourceHandle resourceHandle);

    private static native int nativeLeaveGroupForResource(OcResource resource,
            String collectionResourceType, OcResourceHandle resourceHandle);

    private static native void nativeDeleteGroup(String collectionResourceType);

    private static native Map<String, OcResourceHandle> nativeGetGroupList();

    static {
        System.loadLibrary("gnustl_shared");
        System.loadLibrary("oc_logger");
        System.loadLibrary("connectivity_abstraction");
        System.loadLibrary("ca-interface");
        System.loadLibrary("octbstack");
        System.loadLibrary("oc");
        System.loadLibrary("TGMSDKLibrary");
        System.loadLibrary("ocstack-jni");
        System.loadLibrary("things-manager-jni");
    }

    /**
     * Function for Getting instance of GroupSynchronization object.
     *
     * @return GroupSynchronization instance.
     */
    public static GroupSynchronization getInstance() {
        if (null == s_groupSynchronizationInstance) {
            s_groupSynchronizationInstance = new GroupSynchronization();
        }
        return s_groupSynchronizationInstance;
    }

    /**
     * Provides interface for getting group discovery status.
     */
    public interface IFindGroupListener {
        /**
         * This callback method will be called to notify whether group is found
         * or not.
         *
         * @param resource
         *            URI of resource.
         */
        public void onGroupFindCallback(OcResource resource);
    }

    /**
     * Set listener for receiving notification on whether the requested group is
     * found or not.
     *
     * @param listener
     *            IFindGroupListener to receive group discovery status.
     */
    public void setGroupListener(IFindGroupListener listener) {
        groupListener = listener;
    }

    /**
     * API for finding a specific remote group when a resource tries to join a
     * group.
     * <p>
     * IFindGroupListener::onGroupFindCallback will be called to notify whether
     * requested group found or not.
     * </p>
     *
     * @param resourceTypes
     *            resource types of a group to find and join
     *
     * @return OCStackResult - return value of this API. It returns OC_STACK_OK
     *         if success.
     */
    public OCStackResult findGroup(Vector<String> resourceTypes) {
        OCStackResult result;
        if (null == groupListener) {
            result = OCStackResult.OC_STACK_ERROR;
        } else {
            int ordinal = nativeFindGroup(resourceTypes);
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
     */
    public OCStackResult createGroup(String resourceType) {
        OCStackResult result;
        int ordinal = nativeCreateGroup(resourceType);
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
     */
    public OCStackResult joinGroup(String resourceType,
            OcResourceHandle resourceHandle) {
        OCStackResult result;

        int ordinal = nativeJoinGroupString(resourceType, resourceHandle);
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
     */
    public OCStackResult joinGroup(OcResource resource,
            OcResourceHandle resourceHandle) throws OcException {

        OCStackResult result;
        int ordinal = nativeJoinGroupObject(resource, resourceHandle);
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
     */
    public OCStackResult leaveGroup(String resourceType,
            OcResourceHandle resourceHandle) {

        OCStackResult result;
        int ordinal = nativeLeaveGroup(resourceType, resourceHandle);
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
     */
    public OCStackResult leaveGroup(OcResource resource, String resourceType,
            OcResourceHandle resourceHandle) {

        OCStackResult result;
        int ordinal = nativeLeaveGroupForResource(resource, resourceType,
                resourceHandle);
        result = OCStackResult.conversion(ordinal);

        return result;
    }

    /**
     * API for deleting a group.
     *
     * @param collectionResourceType
     *            resource type of a group to delete.
     */
    public void deleteGroup(String collectionResourceType) {
        nativeDeleteGroup(collectionResourceType);
    }

    /**
     * API for getting a list of joined groups.
     *
     * @return Returns group resource type and group resource handle as a map
     *         table.
     */
    public Map<String, OcResourceHandle> getGroupList() {
        return nativeGetGroupList();
    }

    // ******** JNI UTILITY FUNCTION **********//

    /*
     * This callback method is called from JNI to notify whether group is found
     * or not.
     */
    public void onGroupFindCallback(OcResource resource) {
        if (null != groupListener) {
            Log.i("ThingsManagerCallback : onGroupFindCallback",
                    "Received Callback from JNI");
            groupListener.onGroupFindCallback(resource);
        }
    }
}

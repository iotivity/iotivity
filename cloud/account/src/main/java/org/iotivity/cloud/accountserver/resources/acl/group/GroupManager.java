/*
 * //******************************************************************
 * //
 * // Copyright 2016 Samsung Electronics All Rights Reserved.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 * //
 * // Licensed under the Apache License, Version 2.0 (the "License");
 * // you may not use this file except in compliance with the License.
 * // You may obtain a copy of the License at
 * //
 * //      http://www.apache.org/licenses/LICENSE-2.0
 * //
 * // Unless required by applicable law or agreed to in writing, software
 * // distributed under the License is distributed on an "AS IS" BASIS,
 * // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * // See the License for the specific language governing permissions and
 * // limitations under the License.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */
package org.iotivity.cloud.accountserver.resources.acl.group;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.UUID;

import org.iotivity.cloud.accountserver.Constants;
import org.iotivity.cloud.accountserver.db.AccountDBManager;
import org.iotivity.cloud.accountserver.db.GroupTable;
import org.iotivity.cloud.accountserver.util.TypeCastingManager;
import org.iotivity.cloud.base.device.Device;
import org.iotivity.cloud.base.exception.ServerException.BadRequestException;
import org.iotivity.cloud.base.exception.ServerException.InternalServerErrorException;
import org.iotivity.cloud.base.protocols.IRequest;

/**
 *
 * This class provides a set of APIs to manage group
 *
 */

public class GroupManager {
    private static GroupManager            mGrManager = new GroupManager();
    public HashMap<String, Group>          mGroups    = new HashMap<>();
    private TypeCastingManager<GroupTable> mTypeGroup = new TypeCastingManager<GroupTable>();

    private GroupManager() {
    }

    public static GroupManager getInstance() {
        return mGrManager;
    }

    /**
     * API to create a public or private group
     * 
     * @param uuid
     *            User id which provided by Sign-up process
     * @param gtype
     *            group type (ex: "Private" or "Public)
     * @return response payload which includes the created group ID
     */
    public HashMap<String, Object> createGroup(String uuid, String gtype) {

        HashMap<String, Object> responsePayload = new HashMap<>();
        String gid = null;

        switch (gtype) {
            // if the group type is "Private", group id becomes the name of user
            // id itself
            case Constants.REQ_GTYPE_PRIVATE:
                gid = uuid;
                break;
            // if the group type is "Public", group id is randomly generated
            case Constants.REQ_GTYPE_PUBLIC:
                gid = UUID.randomUUID().toString();
                break;
            default:
                throw new BadRequestException(
                        gtype + " group type is not support");
        }

        GroupTable newGroupTable = new GroupTable(gid,
                new HashSet<String>(Arrays.asList(uuid)), null, uuid, null,
                gtype);

        // record newly generated group information to the group table
        AccountDBManager.getInstance().insertRecord(Constants.GROUP_TABLE,
                mTypeGroup.convertObjectToMap(newGroupTable));

        mGroups.put(gid, new Group(gid));

        responsePayload.put(Constants.REQ_GROUP_ID, gid);

        return responsePayload;
    }

    /**
     * API to delete a group
     * 
     * @param gmid
     *            An unique identifier of member who must be a group master.
     *            Group master can be user or resource client.
     * @param gid
     *            An unique identifier of the group created under user entity
     *            who requested for group creation.
     */
    public void deleteGroup(String gmid, String gid) {

        HashMap<String, Object> condition = new HashMap<>();
        condition.put(Constants.REQ_GROUP_ID, gid);
        ArrayList<HashMap<String, Object>> result = AccountDBManager
                .getInstance().selectRecord(Constants.GROUP_TABLE, condition);

        if (!gmid.equals(result.get(0).get(Constants.REQ_GROUP_MASTER_ID))) {

            throw new BadRequestException(
                    gmid + " is not owner for " + gid + "group");
        }
        AccountDBManager.getInstance().deleteRecord(Constants.GROUP_TABLE,
                condition);

        mGroups.remove(gid);
    }

    /**
     * API to delete a device ID from all groups
     * 
     * @param uid
     *            User id which provided by Sign-up process
     * @param di
     *            device id to be deleted from all groups
     */

    public void removeGroupDeviceinEveryGroup(String uid, String di) {
        // check if the device is the resource server (i.e., device ID exists in
        // the private group table
        if (verifyDeviceInGroup(uid, di)) {
            // token table search criteria
            HashMap<String, Object> condition = new HashMap<>();
            condition.put(Constants.REQ_DEVICE_ID_LIST, di);
            // read group record which contains device ID in each group
            ArrayList<HashMap<String, Object>> groupRecord = AccountDBManager
                    .getInstance()
                    .selectRecord(Constants.GROUP_TABLE, condition);
            // update group record to the DB
            for (HashMap<String, Object> record : groupRecord) {
                String gid = (String) record.get(Constants.KEYFIELD_GID);
                HashSet<String> diSet = new HashSet<>();
                diSet.add(di);
                removeGroupDevice(gid, diSet);
            }
        }
    }

    /**
     * API to get group list where the each group has uuid as a member
     * 
     * @param uuid
     *            User id which is provided by Sign-up process
     * @return response payload which has the list of groups where each group
     *         has uuid as a member id
     * 
     */
    public HashMap<String, Object> getGroupList(String uuid) {

        HashMap<String, Object> responsePayload = new HashMap<>();
        ArrayList<String> gidList = new ArrayList<String>();

        HashMap<String, Object> condition = new HashMap<>();
        condition.put(Constants.KEYFIELD_MIDLIST, uuid);

        ArrayList<HashMap<String, Object>> result = AccountDBManager
                .getInstance().selectRecord(Constants.GROUP_TABLE, condition);

        for (HashMap<String, Object> element : result) {

            GroupTable getGroupTable = new GroupTable();

            getGroupTable = mTypeGroup.convertMaptoObject(element,
                    getGroupTable);

            if (getGroupTable.getGtype() == null) {
                throw new InternalServerErrorException("gtype is empty");
            }

            if (getGroupTable.getMidlist() == null) {
                throw new BadRequestException("midList is invalid in Group");
            }

            HashSet<String> midListSet = new HashSet<>(
                    (Collection<? extends String>) getGroupTable.getMidlist());

            if (midListSet.contains(uuid) && getGroupTable.getGtype()
                    .equals(Constants.REQ_GTYPE_PUBLIC)) {

                gidList.add(getGroupTable.getGid());
            }
        }
        responsePayload.put(Constants.KEYFIELD_GIDLIST, gidList);

        return responsePayload;
    }

    /**
     * API to add the member user id list to the group table in the db
     * 
     * @param gid
     *            group id
     * @param midlist
     *            User id list to be added to the group
     */
    public void addGroupMember(String gid, HashSet<String> midlist) {

        getGroup(gid).addMember(midlist);
    }

    /**
     * API to add the device id list to the group table in the db
     * 
     * @param gid
     *            group id
     * @param dilist
     *            device id list to be added to the group
     */
    public void addGroupDevice(String gid, HashSet<String> dilist) {

        getGroup(gid).addDevice(dilist);
    }

    /**
     * API to remove the member user id list from the group table in the db
     * 
     * @param gid
     *            group id
     * @param midlist
     *            member user id list to be removed from the group
     */
    public void removeGroupMember(String gid, HashSet<String> midlist) {

        getGroup(gid).removeMember(midlist);
    }

    /**
     * API to remove the device id list from the group table in the db
     * 
     * @param gid
     *            group id
     * @param dilist
     *            device id list to be removed from the group
     */
    public void removeGroupDevice(String gid, HashSet<String> dilist) {

        getGroup(gid).removeDevice(dilist);
    }

    /**
     * API to get the group information from the db
     * 
     * @param gid
     *            group id
     * @param mid
     *            An unique Identifier of the member(query requester) who is
     *            expected as a member of a group
     * @return group information payload
     */
    public HashMap<String, Object> getGroupInfo(String gid, String mid) {

        return getGroup(gid).getInfo(mid);
    }

    /**
     * API to check if the device is in the group
     * 
     * @param gid
     *            group id
     * @param di
     *            device id
     * @return true if the device exists in the group or false if the device is
     *         not in the group
     */
    public boolean verifyDeviceInGroup(String gid, String di) {
        try {
            return getGroup(gid).checkDeviceExistance(di);
        } catch (NullPointerException e) {
            // if getGroup(gid) throws NullPointerException
            return false;
        }
    }

    /**
     * API to add a group subscriber of a specific group
     * 
     * @param gid
     *            group id to subscribe
     * @param mid
     *            An unique Identifier of the member(query requester) who is
     *            expected as a member of a group
     * @param srcDevice
     *            subscriber device
     * @param request
     *            request message
     * @return group information payload
     */
    public HashMap<String, Object> addGroupSubscriber(String gid, String mid,
            Device srcDevice, IRequest request) {

        return getGroup(gid).addSubscriber(mid, srcDevice, request);
    }

    /**
     * API to unsubscribe group information
     * 
     * @param gid
     *            group id to subscribe
     * @param mid
     *            user Id to unscribe group information
     * @return group information payload
     */
    public HashMap<String, Object> removeGroupSubscriber(String gid,
            String mid) {

        return getGroup(gid).removeSubscriber(mid);
    }

    /**
     * API to get group information for a certain group
     * 
     * @param gid
     *            group id to get
     * @return group information
     */
    public Group getGroup(String gid) {

        return mGroups.get(gid);
    }
}

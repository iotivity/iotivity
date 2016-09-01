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

import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;

import org.iotivity.cloud.accountserver.Constants;
import org.iotivity.cloud.accountserver.db.AccountDBManager;
import org.iotivity.cloud.accountserver.db.GroupTable;
import org.iotivity.cloud.accountserver.util.TypeCastingManager;
import org.iotivity.cloud.base.device.Device;
import org.iotivity.cloud.base.exception.ServerException.BadRequestException;
import org.iotivity.cloud.base.exception.ServerException.InternalServerErrorException;
import org.iotivity.cloud.base.exception.ServerException.UnAuthorizedException;
import org.iotivity.cloud.base.protocols.IRequest;
import org.iotivity.cloud.base.protocols.MessageBuilder;
import org.iotivity.cloud.base.protocols.enums.ContentFormat;
import org.iotivity.cloud.base.protocols.enums.ResponseStatus;
import org.iotivity.cloud.util.Cbor;

/**
 *
 * This class provides a set of APIs to handle group requests
 *
 */

public class Group {

    private Cbor<HashMap<String, Object>>  mCbor      = new Cbor<>();
    private String                         mGid       = null;
    private TypeCastingManager<GroupTable> mTypeGroup = new TypeCastingManager<>();

    public Group(String gid) {
        mGid = gid;
    }

    private class GroupSubscriber {
        GroupSubscriber(Device subscriber, IRequest request) {
            mSubscriber = subscriber;
            mRequest = request;
        }

        public Device   mSubscriber;
        public IRequest mRequest;
    }

    private HashMap<String, GroupSubscriber> mSubscribers = new HashMap<>();

    /**
     * 
     * API to add the member user id to the group table in the db
     * 
     * @param uuid
     *            User id list which are provided by Sign-up process
     */
    public void addMember(HashSet<String> uuid) {

        GroupTable groupTable = getGroupTable();

        groupTable.setMidlist(groupTable.getMidlist() == null ? uuid
                : addGroupListSet(groupTable.getMidlist(), uuid));

        AccountDBManager.getInstance().updateRecord(Constants.GROUP_TABLE,
                mTypeGroup.convertObjectToMap(groupTable));

        notifyToSubscriber(getResponsePayload(true));
    }

    /**
     * API to add the device id to the group table in the db
     * 
     * @param di
     *            device id list to be stored
     */
    public void addDevice(HashSet<String> di) {

        GroupTable groupTable = getGroupTable();

        groupTable.setDilist(groupTable.getDilist() == null ? di
                : addGroupListSet(groupTable.getDilist(), di));

        AccountDBManager.getInstance().updateRecord(Constants.GROUP_TABLE,
                mTypeGroup.convertObjectToMap(groupTable));

        notifyToSubscriber(getResponsePayload(true));
    }

    /**
     * API to remove member user id list from the group table in the db
     * 
     * @param uuid
     *            User id list to be removed from the group table
     */
    public void removeMember(HashSet<String> uuid) {

        GroupTable groupTable = getGroupTable();
        if (groupTable.getGmid() == null) {
            throw new InternalServerErrorException("group master is empty");
        }
        if (uuid.contains(groupTable.getGmid())) {
            GroupManager.getInstance().deleteGroup(groupTable.getGmid(), mGid);
            notifyToSubscriber(getResponsePayload(false));
        } else {
            if (groupTable.getMidlist() == null) {
                throw new BadRequestException("midList is invalid in Group");
            }
            groupTable.setMidlist(
                    removeGroupListSet(groupTable.getMidlist(), uuid));
            AccountDBManager.getInstance().updateRecord(Constants.GROUP_TABLE,
                    mTypeGroup.convertObjectToMap(groupTable));
            notifyToSubscriber(getResponsePayload(true));

            Iterator<String> mid = uuid.iterator();
            while (mid.hasNext()) {
                removeSubscriber(mid.next());
            }
        }
    }

    /**
     * API to remove device id list from the group table in the db
     * 
     * @param di
     *            device id list to be removed from the group table
     */
    public void removeDevice(HashSet<String> di) {

        GroupTable groupTable = getGroupTable();
        if (groupTable.getDilist() == null) {
            throw new BadRequestException("deviceList is invalid in Group");
        }
        groupTable.setDilist(removeGroupListSet(groupTable.getDilist(), di));

        AccountDBManager.getInstance().updateRecord(Constants.GROUP_TABLE,
                mTypeGroup.convertObjectToMap(groupTable));

        notifyToSubscriber(getResponsePayload(true));
    }

    /**
     * API to return the group information payload
     * 
     * @param mid
     *            member id to verify if the id exists in the group table
     * @return group information payload
     */
    public HashMap<String, Object> getInfo(String mid) {

        verifyGroupTableMid(mid);

        return getResponsePayload(true);
    }

    public boolean checkDeviceExistance(String di) {
        return verifyGroupTableDi(di);
    }

    /**
     * API to add group subscriber
     * 
     * @param mid
     *            member id to verify if the id exists in the group table
     * @param subscriber
     *            subscriber device
     * @param request
     *            request message
     * @return group information payload
     */
    public HashMap<String, Object> addSubscriber(String mid, Device subscriber,
            IRequest request) {

        // Check if the user has privilege to observe
        verifyGroupTableMid(mid);

        GroupSubscriber newSubscriber = new GroupSubscriber(subscriber,
                request);

        mSubscribers.put(mid, newSubscriber);

        return getInfo(
                request.getUriQueryMap().get(Constants.REQ_MEMBER).get(0));
    }

    /**
     * API to unsubscribe group information
     * 
     * @param mid
     *            user Id to unscribe group information
     * @return group information payload
     */
    public HashMap<String, Object> removeSubscriber(String mid) {

        HashMap<String, Object> responsePayload = getResponsePayload(true);

        if (mSubscribers.containsKey(mid)) {
            mSubscribers.remove(mid);
        }

        return responsePayload;
    }

    private void verifyGroupTableMid(String mid) {

        GroupTable groupTable = getGroupTable();

        if (groupTable.getMidlist() == null) {
            throw new BadRequestException("midList is invalid in Group");
        }
        HashSet<String> midListSet = new HashSet<>(
                (Collection<? extends String>) groupTable.getMidlist());

        if (!midListSet.contains(mid)) {

            throw new UnAuthorizedException(
                    mid + " is not Group member in gid=" + mGid);
        }
    }

    private boolean verifyGroupTableDi(String di) {

        GroupTable groupTable = getGroupTable();

        if (groupTable.getDilist() == null) {
            return false;
        }

        HashSet<String> diListSet = new HashSet<>(
                (Collection<? extends String>) groupTable.getDilist());

        if (!diListSet.contains(di)) {
            return false;
        }
        return true;
    }

    private void notifyToSubscriber(
            HashMap<String, Object> notifiyBtyePayloadData) {
        synchronized (mSubscribers) {

            Iterator<String> iterator = mSubscribers.keySet().iterator();

            while (iterator.hasNext()) {

                String key = iterator.next();

                GroupSubscriber groupSubscriber = mSubscribers.get(key);

                groupSubscriber.mSubscriber.sendResponse(
                        MessageBuilder.createResponse(groupSubscriber.mRequest,
                                ResponseStatus.CONTENT,
                                ContentFormat.APPLICATION_CBOR,
                                mCbor.encodingPayloadToCbor(
                                        notifiyBtyePayloadData)));
            }
        }
    }

    private GroupTable getGroupTable() {

        GroupTable getGroupTable = new GroupTable();

        getGroupTable = mTypeGroup
                .convertMaptoObject(
                        AccountDBManager.getInstance().selectRecord(
                                Constants.GROUP_TABLE, getContdition()).get(0),
                        getGroupTable);

        return getGroupTable;
    }

    private HashMap<String, Object> getResponsePayload(boolean isAliveGroup) {

        GroupTable groupTable = isAliveGroup ? getGroupTable() : null;

        HashMap<String, Object> responsePayload = new HashMap<>();

        responsePayload.put(Constants.REQ_GROUP_ID,
                isAliveGroup ? groupTable.getGid() : null);
        responsePayload.put(Constants.REQ_GROUP_MASTER_ID,
                isAliveGroup ? groupTable.getGmid() : null);
        responsePayload.put(Constants.REQ_MEMBER_LIST,
                isAliveGroup ? groupTable.getMidlist() : null);
        responsePayload.put(Constants.REQ_DEVICE_ID_LIST,
                isAliveGroup ? groupTable.getDilist() : null);
        responsePayload.put(Constants.REQ_GROUP_TYPE,
                isAliveGroup ? groupTable.getGtype() : null);

        return responsePayload;
    }

    private HashSet<String> addGroupListSet(Object object,
            HashSet<String> addList) {

        HashSet<String> groupSet = new HashSet<>(
                (Collection<? extends String>) object);

        groupSet.addAll(addList);

        return groupSet;
    }

    private HashSet<String> removeGroupListSet(Object object,
            HashSet<String> removeList) {

        HashSet<String> groupSet = new HashSet<>(
                (Collection<? extends String>) object);

        groupSet.removeAll(removeList);

        return groupSet;
    }

    private HashMap<String, Object> getContdition() {

        HashMap<String, Object> condition = new HashMap<>();
        condition.put(Constants.REQ_GROUP_ID, mGid);
        return condition;
    }

}

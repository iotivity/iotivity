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

public class GroupManager {
    public HashMap<String, Group>          mGroups    = new HashMap<>();
    private TypeCastingManager<GroupTable> mTypeGroup = new TypeCastingManager<GroupTable>();

    public HashMap<String, Object> createGroup(String uuid, String gtype) {

        HashMap<String, Object> responsePayload = new HashMap<>();
        String gid = null;

        switch (gtype) {
            case Constants.REQ_GTYPE_PRIVATE:
                gid = uuid;
                break;
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

        AccountDBManager.getInstance().insertRecord(Constants.GROUP_TABLE,
                mTypeGroup.convertObjectToMap(newGroupTable));

        mGroups.put(gid, new Group(gid));

        responsePayload.put(Constants.REQ_GROUP_ID, gid);

        return responsePayload;
    }

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

            HashSet<String> midListSet = new HashSet<String>(
                    (Collection<? extends String>) getGroupTable.getMidlist());

            if (midListSet.contains(uuid) && getGroupTable.getGtype()
                    .equals(Constants.REQ_GTYPE_PUBLIC)) {

                gidList.add(getGroupTable.getGid());
            }
        }
        responsePayload.put(Constants.KEYFIELD_GIDLIST, gidList);

        return responsePayload;
    }

    public void addGroupMember(String gid, HashSet<String> midlist) {

        getGroup(gid).addMember(midlist);
    }

    public void addGroupDevice(String gid, HashSet<String> dilist) {

        getGroup(gid).addDevice(dilist);
    }

    public void removeGroupMember(String gid, HashSet<String> midlist) {

        getGroup(gid).removeMember(midlist);
    }

    public void removeGroupDevice(String gid, HashSet<String> dilist) {

        getGroup(gid).removeDevice(dilist);
    }

    public HashMap<String, Object> getGroupInfo(String gid, String mid) {

        return getGroup(gid).getInfo(mid);
    }

    public HashMap<String, Object> addGroupSubscriber(String gid, String mid,
            Device srcDevice, IRequest request) {

        return getGroup(gid).addSubscriber(mid, srcDevice, request);
    }

    public HashMap<String, Object> removeGroupSubscriber(String gid,
            String mid) {

        return getGroup(gid).removeSubscriber(mid);
    }

    public Group getGroup(String gid) {

        return mGroups.get(gid);
    }
}

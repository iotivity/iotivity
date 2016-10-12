package org.iotivity.cloud.accountserver.resources.acl.group;

import java.util.HashMap;

import org.iotivity.cloud.base.device.Device;
import org.iotivity.cloud.base.protocols.IRequest;

public class GroupBrokerManager {
    public void verifyAuthorization(String mid, String gid,
            HashMap<String, Object> properties) {

    }

    public void getGroupList(String mid) {

    }

    public void createGroup(String mid, String gname, String upperGroup) {

    }

    public void addObserver(String mid, Device srcDevice, IRequest request) {

    }

    public void removeObserver(String mid, IRequest request) {

    }

    public void notiryToObservers(String mid) {

    }
}

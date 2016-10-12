package org.iotivity.cloud.accountserver.resources.acl.group;

import java.util.ArrayList;

public class GroupPolicyManager {
    public ArrayList<String> getUserAuthorization(String mid) {
        ArrayList<String> userAuthz = null;

        return userAuthz;
    }

    public boolean verifyOperationAuthorization(ArrayList<String> userAuthz,
            int operation, ArrayList<String> properties) {

        return true;
    }
}

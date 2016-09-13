package org.iotivity.cloud.accountserver.resources.acl.verify;

import java.util.ArrayList;
import java.util.List;

public enum Permission {
    Create(1), Read(2), Update(4), Delete(8), Notify(16);

    private int value;

    Permission(int val) {
        value = val;
    }

    public int getValue() {
        return value;
    }

}

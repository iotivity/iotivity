/*
 * //******************************************************************
 * //
 * // Copyright 2015 Intel Corporation.
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

package org.iotivity.base;

import java.security.InvalidParameterException;

public enum OcConnectivityType {
    IPV4    (0),
    IPV6    (1),
    EDR     (2),
    LE      (3),
    ALL     (4),
    ;

    private int value;

    private OcConnectivityType(int value) {
        this.value = value;
    }

    public int getValue() {
        return this.value;
    }

    public static OcConnectivityType get(int val) {
        for (OcConnectivityType v : OcConnectivityType.values()) {
            if (v.getValue() == val)
                return v;
        }
        throw new InvalidParameterException("Unexpected OcConnectivityType value:" + val);
    }
}

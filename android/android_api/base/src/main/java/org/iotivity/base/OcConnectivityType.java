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
    CT_DEFAULT (0),
    CT_ADAPTER_IP (1 << 16),
    CT_ADAPTER_GATT_BTLE (1 << 17),
    CT_ADAPTER_RFCOMM_BTEDR (1 << 18),
    ;

    private int value;

    private OcConnectivityType(int value) {
        this.value = value;
    }

    public int getValue() {
        return this.value;
    }

    public static OcConnectivityType get(int val) {
        // val is a combination of OcConnectivityType and OcConnectivityFlags
        for (OcConnectivityType v : OcConnectivityType.values()) {
            for (OcConnectivityFlags f : OcConnectivityFlags.values()) {
                int value = v.getValue() + f.getValue();
                if (value == val)
                    return v;
            }
        }
        throw new InvalidParameterException("Unexpected OcConnectivityType value:" + val);
    }
}

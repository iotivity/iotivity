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
public enum OcConnectivityFlags {
    OC_DEFAULT_FLAGS (0),
    // Insecure transport is the default (subject to change)
    OC_FLAG_SECURE     (1 << 4), // secure the transport path

    // IPv4 & IPv6 autoselection is the default
    OC_IP_USE_V6       (1 << 5), // IP adapter only
    OC_IP_USE_V4       (1 << 6), // IP adapter only

    // Link-Local multicast is the default multicast scope for IPv6.
    // These are placed here to correspond to the IPv6 address bits.
    OC_SCOPE_INTERFACE (0x1), // IPv6 Interface-Local scope (loopback)
    OC_SCOPE_LINK      (0x2), // IPv6 Link-Local scope (default)
    OC_SCOPE_REALM     (0x3), // IPv6 Realm-Local scope
    OC_SCOPE_ADMIN     (0x4), // IPv6 Admin-Local scope
    OC_SCOPE_SITE      (0x5), // IPv6 Site-Local scope
    OC_SCOPE_ORG       (0x8), // IPv6 Organization-Local scope
    OC_SCOPE_GLOBAL    (0xE), // IPv6 Global scope
    ;
    private int value;

    private OcConnectivityFlags(int value) {
        this.value = value;
    }

    public int getValue() {
        return this.value;
    }

    public static OcConnectivityFlags get(int val) {
        for (OcConnectivityType v : OcConnectivityType.values()) {
            for (OcConnectivityFlags f : OcConnectivityFlags.values()) {
                int value = v.getValue() + f.getValue();
                if (value == val)
                    return f;
            }
        }
        throw new InvalidParameterException("Unexpected OcConnectivityFlags value:" + val);
    }
}
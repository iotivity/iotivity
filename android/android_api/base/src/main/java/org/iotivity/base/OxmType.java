/*
 * //******************************************************************
 * //
 * // Copyright 2015 Samsung Electronics All Rights Reserved.
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

public enum OxmType {
    OIC_JUST_WORKS(0),
    OIC_MODE_SWITCH(1),
    OIC_RANDOM_DEVICE_PIN(2),
    OIC_PRE_PROVISIONED_DEVICE_PIN(3),
    OIC_PRE_PROVISION_STRONG_CREDENTIAL(4),
    OIC_OXM_COUNT(5);

    private int value;

    private OxmType(int value) {
        this.value = value;
    }

    public int getValue() {
        return this.value;
    }
}

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
/**
 * @brief   direct pairing Method Type.
 *              0:  not allowed
 *              1:  pre-configured pin
 *              2:  random pin
 */
package org.iotivity.base;

public enum OcPrmType {
    DP_INVALID           (-1),
    DP_NOT_ALLOWED       (0),
    DP_PRE_CONFIGURED    (1 << 0),
    DP_RANDOM_PIN        (1 << 1);

    private int value;

    private OcPrmType(int value) {
        this.value = value;
    }

    public int getValue() {
        return this.value;
    }

    public static OcPrmType convertOcPrmType(int value) {
        switch (value) {
            case 0:
                return OcPrmType.DP_NOT_ALLOWED;
            case 1:
                return OcPrmType.DP_PRE_CONFIGURED;
            case 2:
                return OcPrmType.DP_RANDOM_PIN;
        }
        return OcPrmType.DP_INVALID;
    }
}

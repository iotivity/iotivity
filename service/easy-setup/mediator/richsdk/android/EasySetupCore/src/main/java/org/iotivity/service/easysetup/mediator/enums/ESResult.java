/**
 * ***************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * ****************************************************************
 */

package org.iotivity.service.easysetup.mediator.enums;

/**
 * It defines an result during easy setup process, which is same as one in Easy Setup C++ SDK
 */
public enum ESResult {

    ES_OK(0),
    ES_SECURE_RESOURCE_IS_DISCOVERED(1),
    ES_ENROLLEE_DISCOVERY_FAILURE(11),
    ES_COMMUNICATION_ERROR(12),
    ES_SEC_OPERATION_IS_NOT_SUPPORTED(13),
    ES_SECURE_RESOURCE_DISCOVERY_FAILURE(14),
    ES_OWNERSHIP_TRANSFER_FAILURE(20),
    ES_USER_DENIED_CONFIRMATION_REQ(21),
    ES_AUTHENTICATION_FAILURE_WITH_WRONG_CERT(22),
    ES_AUTHENTICATION_FAILURE_WITH_WRONG_PIN(23),
    ES_OWNERSHIP_IS_NOT_SYNCHRONIZED(24),
    ES_MOT_NOT_SUPPORTED(30),
    ES_MOT_ENABLING_FAILURE(31),
    ES_MOT_METHOD_SELECTION_FAILURE(32),
    ES_PRE_CONFIG_PIN_PROVISIONING_FAILURE(33),
    ES_ACL_PROVISIONING_FAILURE(40),
    ES_CERT_PROVISIONING_FAILURE(41),
    ES_ERROR(255);

    private int value;

    private ESResult(int value) {
        this.value = value;
    }

    public int getValue() {
        return value;
    }

    public static ESResult fromInt(int i) {
        for (ESResult b : ESResult.values()) {
            if (b.getValue() == i) {
                return b;
            }
        }
        return null;
    }
};

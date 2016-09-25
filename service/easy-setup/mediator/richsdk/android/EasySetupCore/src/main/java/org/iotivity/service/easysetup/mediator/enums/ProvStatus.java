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
 * It defines various states of the cloud provisioning during easy setup process
 */
public enum ProvStatus {

    /**
     * Default state of the device
     */
    ES_STATE_INIT(0),

    /**
     * Status indicating being connecting to target network
     */
    ES_STATE_CONNECTING_TO_ENROLLER(1),

    /**
     * Status indicating successful connection to target network
     */
    ES_STATE_CONNECTED_TO_ENROLLER(2),

    /**
     * Status indicating failure connection to target network
     */
    ES_STATE_CONNECTED_FAIL_TO_ENROLLER(3),

    /**
     * Status indicating successful registration to cloud
     */
    ES_STATE_REGISTERED_TO_CLOUD(4),

    /**
     * Status indicating failure registration to cloud
     */
    ES_STATE_REGISTRRED_FAIL_TO_CLOUD(5);

    private int value;

    private ProvStatus(int value) {
        this.value = value;
    }

    public int getValue() {
        return value;
    }

    public static ProvStatus fromInt(int i) {
        for (ProvStatus b : ProvStatus.values()) {
            if (b.getValue() == i) { return b; }
        }
        return null;
    }
};


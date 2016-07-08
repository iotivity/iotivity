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
    ES_ERROR(-1),
    ES_OK(0),
    ES_NETWORKFOUND(1),
    ES_NETWORKCONNECTED(2),
    ES_NETWORKNOTCONNECTED(3),
    ES_RESOURCECREATED(11),
    ES_RECVREQOFPROVRES(21),
    ES_RECVREQOFNETRES(22),
    ES_RECVUPDATEOFPROVRES(23),
    ES_RECVTRIGGEROFPROVRES(24),
    ES_UNAUTHORIZED(25);

    private int value;

    private ESResult(int value) {
        this.value = value;
    }

	public int getValue() {
        return value;
    }

	public static ESResult fromInt(int i) {
        for (ESResult b : ESResult.values()) {
            if (b.getValue() == i) { return b; }
        }
        return null;
    }
};
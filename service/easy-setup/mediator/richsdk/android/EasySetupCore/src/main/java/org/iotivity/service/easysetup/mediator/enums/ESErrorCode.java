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
 * @brief Indicate last error code to describe a reason of error during easy setup.
 */
public enum ESErrorCode {

    /**
     * Init Error Code.
     */
    ES_ERRCODE_NO_ERROR(0),

    /**
     * WiFi's SSID is not found.
     */
    ES_ERRCODE_SSID_NOT_FOUND(1),

    /**
     * WiFi's Password is wrong.
     */
    ES_ERRCODE_PW_WRONG(2),

    /**
     * IP address is not allocated.
     */
    ES_ERRCODE_IP_NOT_ALLOCATED(3),

    /**
     * There is no Internet connection.
     */
    ES_ERRCODE_NO_INTERNETCONNECTION(4),

    /**
     * Timeout occured.
     */
    ES_ERRCODE_TIMEOUT(5),

    /**
     * Auth type is not supported by the Enrollee.
     */
    ES_ERRCODE_AUTH_TYPE_NOT_SUPPORTED(6),

    /**
     * Enc type is not supported by the Enrollee.
     */
    ES_ERRCODE_ENC_TYPE_NOT_SUPPORTED(7),

    /**
     * Auth type is not supported by the Enroller.
     */
    ES_ERRCODE_AUTH_TYPE_INVALID(8),

    /**
     * Enc type is not supported by the Enroller.
     */
    ES_ERRCODE_ENC_TYPE_INVALID(9),

    /**
     * Cloud server is not reachable due to wrong URL of cloud server, for example.
     */
    ES_ERRCODE_FAILED_TO_ACCESS_CLOUD_SERVER(10),

    /**
     * No response from cloud server.
     */
    ES_ERRCODE_NO_RESPONSE_FROM_CLOUD_SERVER(11),

    /**
     * Delivered authcode is not valid.
     */
    ES_ERRCODE_INVALID_AUTHCODE(12),

    /**
     * Access token is not valid due to its expiration, for example.
     */
    ES_ERRCODE_INVALID_ACCESSTOKEN(13),

    /**
     * Refresh of expired access token is failed due to some reasons.
     */
    ES_ERRCODE_FAILED_TO_REFRESH_ACCESSTOKEN(14),

    /**
     * Target device is not discovered in cloud server.
     */
    ES_ERRCODE_FAILED_TO_FIND_REGISTERED_DEVICE_IN_CLOUD(15),

    /**
     * Target user does not exist in cloud server.
     */
    ES_ERRCODE_FAILED_TO_FIND_REGISTERED_USER_IN_CLOUD(16),

    /**
     * Enrollee can not connect to a target WiFi AP because the AP resides in
     * an unsupported WiFi frequency.
     */
    ES_ERRCODE_UNSUPPORTED_WIFI_FREQUENCY(17),

    /**
     * Unknown error occured.
     */
    ES_ERRCODE_UNKNOWN(255);

    private int value;

    private ESErrorCode(int value) {
        this.value = value;
    }

    public int getValue() {
        return value;
    }

    public static ESErrorCode fromInt(int i) {
        for (ESErrorCode b : ESErrorCode.values()) {
            if (b.getValue() == i) {
                return b;
            }
        }
        return null;
    }
};


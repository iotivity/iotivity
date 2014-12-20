//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

package OC.Cm;

public enum EasySetupNotification {
    EASY_SETUP_REQUIRED(-20), DIFFERENT_COUNTRY_CODE(-19), AUTH_CODE_EXPIRED(
            -18), MISSING_MANDATORY_PARAMS(-17), LOCAL_SERVER_ERROR(-16), NO_AUTHORIZATION_DETAILS(
            -15), CONNECTION_ERROR(-14), REMOTE_SERVER_ERROR(-13), INVALID_REQUEST(
            -12), CONNECTION_TIMEOUT(-11), REFRESH_TOKEN_EXPIRED(-10), DEVICE_LOGIN_FAILED_TO_SCS(
            -9), FAILED_TO_START_TIMER(-8), FAILED_TO_REGISTER_DEVICE_TO_SHS(-7), FAILED_TO_FETCH_INFORMATION_FROM_SHS_CR(
            -6), FAILED_TO_UPDATE_REFRESH_TOKEN(-5), ACCESS_TOKEN_EXPIRED(-4), FAILED_TO_GET_TOKEN_DETAILS(
            -3), FAILED_TO_GET_AUTH_CODE_CR(-2), EASY_SETUP_TIME_OUT_CR(-1), DEVICE_NOT_REGISTERED(
            0), WAITING_FOR_HELPING_DEVICE_TO_BE_DISCOVERED_CR(1), AWAITING_WIFI_DETAILS_CD(
            2), SENDING_WIFI_DETAILS_CR(3), WIFI_DETAILS_SENT_CR(4), REQUESTING_CONTROLLER_UUID_CD(
            5), SENDING_UUID_CR(6), AWAITING_DEVICE_DETAILS_CR(7), SENDING_DEVICE_DETAILS_CD(
            8), EASY_SETUP_COMPLETED(9), REQUESTING_DEVICE_TOKEN_CR(10), REGISTRATION_INITIATED(
            11), GETTING_AUTH_CODE_FROM_SERVER_CR(12), AWAITING_AUTHCODE_DETAILS_CD(
            13), SENDING_AUTHCODE_DETAILS_CR(14), GETTING_TOKEN_DETAILS_FROM_SERVER(
            15), GETTING_DEVICE_LIST_FROM_SHS_CR(16), GETTING_USER_INFO_FROM_SHS_CR(
            17), REGISTERING_TO_SHS(18), DEVICE_REGISTERED_TO_SHS_CR(19), WAITING_FOR_PEERID_DETAILS_CD(
            20), SENDING_PEERID_DETAILS_CR(21), ATTEMPTING_REGISTRATION_WITH_SCS(
            22), DEVICE_LOGGED_INTO_SCS(23), ALREADY_REGISTERED_TO_SHS_CR(24), RETRYING_REGISTRATION_WITH_SCS(
            25);

    private int value;

    private EasySetupNotification(int value) {
        this.value = value;
    }

    private EasySetupNotification() {
    }

    public int getValue() {
        return value;
    }

    public static EasySetupNotification getEasySetupNotification(int value) {
        for (EasySetupNotification error : EasySetupNotification.values()) {
            if (error.getValue() == value) {
                return error;
            }
        }

        return DEVICE_NOT_REGISTERED;
    }
}

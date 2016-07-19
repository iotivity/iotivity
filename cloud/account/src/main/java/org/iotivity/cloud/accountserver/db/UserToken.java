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
package org.iotivity.cloud.accountserver.db;

/**
 *
 * This class provides a set of APIs storing session information of authorized
 * user.
 *
 */
public class UserToken {

    private String userId       = null;
    private String accessToken  = null;
    private String refreshToken = null;
    private String issuedTime   = null;

    public void setUserToken(String id, String accessToken,
            String refreshToken) {
        this.userId = id;
        this.accessToken = accessToken;
        this.refreshToken = refreshToken;
    }

    public void setUserId(String id) {
        this.userId = id;
    }

    public void setToken(String accessToken, String refreshToken) {
        this.accessToken = accessToken;
        this.refreshToken = refreshToken;
    }

    public void setIssuedTime(String time) {
        this.issuedTime = time;
    }

    public String getUserId() {
        return this.userId;
    }

    public String getAccessToken() {
        return this.accessToken;
    }

    public String getRefreshToken() {
        return this.refreshToken;
    }

    public String getIssuedTime() {
        return this.issuedTime;
    }
}
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
package org.iotivity.cloud.accountserver;

import java.util.ArrayList;
import java.util.Random;

import org.iotivity.cloud.accountserver.db.AccountDBManager;
import org.iotivity.cloud.accountserver.oauth.GitHub;
import org.iotivity.cloud.util.Logger;

/**
 *
 * This class provides a set of APIs to handle requests about account
 * information of authorized user.
 *
 */
public class AccountServerManager {

    /**
     * API for requesting user account
     *
     * @param userId
     *            user identifier
     * @param deviceId
     *            device identifier
     * @return Boolean - true if registered, otherwise false
     */
    public Boolean registerUserAccount(String userId, String deviceId) {

        Boolean ret = false;

        // store info to OAuthDBManager
        ret = AccountDBManager.getInstance().registerUserDevice(userId,
                deviceId);

        return ret;
    }

    /**
     * API for requesting user account and getting session code for registered
     * user.
     *
     * @param userId
     *            user identifier
     * @return String - session code for registered user
     */
    public String registerUserAccount(String userId) {

        String sessionCode = null;
        sessionCode = generateSessionCode();

        // store info to OAuthDBManager
        AccountDBManager.getInstance().registerUserSessionCode(userId,
                sessionCode);

        return sessionCode;
    }

    /**
     * API for requesting user identifier corresponding with authorization
     * information.
     *
     * @param authCode
     *            authorization code
     * @param authServer
     *            authorization server
     * @return String - user identifier
     */
    public String requestUserId(String authCode, String authServer) {

        String userId = null;

        String accessToken = getAccessToken(authCode, authServer);
        userId = getUserId(accessToken, authServer);

        return userId;
    }

    /**
     * API for requesting user identifier corresponding with session code.
     *
     * @param sessionCode
     *            session code
     * @return String - user identifier
     */
    public String requestUserId(String sessionCode) {

        String userId = null;

        // get userId from MongDB
        userId = AccountDBManager.getInstance().getUserId(sessionCode);

        return userId;
    }

    /**
     * API for getting devices corresponding with user identifier.
     *
     * @param userId
     *            user identifier
     * @return ArrayList<String> - list of devices
     */
    public ArrayList<String> requestAccountDevices(String userId) {

        Logger.d("userId= " + userId);

        ArrayList<String> deviceList = AccountDBManager.getInstance()
                .getDevices(userId);

        return deviceList;
    }

    private String getAccessToken(String authCode, String authServer) {

        String accessToken = null;

        if (authServer.equals(Constants.GITHUB)) {

            GitHub gitHub = new GitHub();
            accessToken = gitHub.requestAccessToken(authCode);

        } else {

            Logger.e("unsupported auth.server = " + authServer);
        }

        return accessToken;
    }

    private String getUserId(String accessToken, String authServer) {

        String userId = null;

        if (authServer.equals(Constants.GITHUB)) {

            GitHub gitHub = new GitHub();
            userId = gitHub.requestGetUserInfo(accessToken);

        } else {

            Logger.e("unsupported auth.server = " + authServer);
        }

        return userId;
    }

    private String generateSessionCode() {

        StringBuffer sessionCode = new StringBuffer();

        Random random = new Random();
        int randomNum = random.nextInt(122);
        char code;

        // generate 16byte key with 0-9, A-Z, a-z
        for (int k = 0; k < 16; k++) {
            while (true) {
                if ((randomNum >= 48 && randomNum <= 57)
                        || (randomNum >= 65 && randomNum <= 90)
                        || (randomNum >= 97 && randomNum <= 122)) {

                    code = (char) randomNum;
                    sessionCode.append(code);

                    randomNum = random.nextInt(122);
                    break;

                } else {

                    randomNum = random.nextInt(122);
                }
            }
        }

        return sessionCode.toString();
    }
}

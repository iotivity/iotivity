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

import org.iotivity.cloud.accountserver.db.AccountDBManager;
import org.iotivity.cloud.accountserver.oauth.OAuthServerFactory;
import org.iotivity.cloud.accountserver.token.Token;
import org.iotivity.cloud.accountserver.token.TokenManager;
import org.iotivity.cloud.util.Logger;

/**
 *
 * This class provides a set of APIs to handle requests about account
 * information of authorized user.
 *
 */
public class AccountServerManager {

    private OAuthServerFactory factory = null;

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
    public Token registerUserAccount(String userId) {

        // String sessionCode = null;
        // sessionCode = generateSessionCode();

        TokenManager tokenManager = new TokenManager();
        Token token = tokenManager.generateToken();

        // store info to OAuthDBManager
        AccountDBManager.getInstance().registerUserToken(userId,
                token.getAccessToken(), token.getRefreshToken());

        // return sessionCode;
        return token;
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
    public String requestUserId(String authCode, String authServer,
            String authServerUrl, String apiServerUrl) {

        String userId = null;

        userId = getUserId(authCode, authServer, authServerUrl, apiServerUrl);

        return userId;
    }

    /**
     * API for requesting user identifier corresponding with session code.
     * 
     * @param sessionCode
     *            session code
     * @return String - user identifier
     */
    public String requestUserId(String accessToken) {

        String userId = null;

        // get userId from MongDB
        userId = AccountDBManager.getInstance()
                .getUserIdByAccessToken(accessToken);

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

    public Boolean loadAuthServer(String authServer) {
        factory = new OAuthServerFactory();

        return factory.create(authServer);
    }

    private String getUserId(String authCode, String authServer,
            String authServerUrl, String apiServerUrl) {

        String userId = null;

        String accessToken = factory.requestAccessToken(authCode,
                authServerUrl);

        if (accessToken == null) {
            return null;
        } else {
            userId = factory.requestGetUserInfo(accessToken, apiServerUrl);
        }

        return userId;
    }

}

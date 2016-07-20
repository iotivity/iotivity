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

import java.util.ArrayList;

import org.iotivity.cloud.accountserver.Constants;

/**
 *
 * This class provides a set of APIs managing database.
 *
 */
public class AccountDBManager {

    private static AccountDBManager accoutDBManager = new AccountDBManager();
    private MongoDB                 mongoDB;

    private AccountDBManager() {

        try {

            mongoDB = new MongoDB(Constants.DB_NAME);

            mongoDB.createTable(Constants.DEVICE_TABLE);
            // mongoDB.createTable(Constants.SESSION_TABLE);
            mongoDB.createTable(Constants.TOKEN_TABLE);

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static AccountDBManager getInstance() {

        return accoutDBManager;
    }

    public Boolean registerUserToken(String userId, String accessToken,
            String refreshToken) {

        UserToken userToken = new UserToken();

        userToken.setUserToken(userId, accessToken, refreshToken);
        mongoDB.createResource(userToken);
        mongoDB.printResources();

        return true;
    }

    public Boolean updateUserToken(String userId, String oldRefreshToken,
            String newAccessToken, String newRefreshToken) {

        Boolean updateUserToken = false;

        UserToken oldUserToken = new UserToken();
        oldUserToken.setUserToken(userId, null, oldRefreshToken);

        UserToken newUserToken = new UserToken();
        newUserToken.setUserToken(userId, newAccessToken, newRefreshToken);

        updateUserToken = mongoDB.updateResource(oldUserToken, newUserToken);
        mongoDB.printResources();

        return updateUserToken;
    }

    /**
     * API for storing device information of authorized user
     * 
     * @param userId
     *            user identifier
     * @param deviceId
     *            device identifier
     * @return Boolean - true if stored, otherwise false
     */
    public Boolean registerUserDevice(String userId, String deviceId) {

        UserDevice userDevice = new UserDevice();

        userDevice.setUserId(userId);
        userDevice.setDeviceId(deviceId);

        mongoDB.createResource(userDevice);
        mongoDB.printResources();

        return true;
    }

    /**
     * API for getting user identifier information corresponding with session
     * code
     * 
     * @param userId
     *            identifier of authorized user
     * @param sessionCode
     *            session code
     * @return Boolean - true if stored, otherwise false
     */
    public String getUserIdByAccessToken(String token) {

        String userId = null;

        userId = mongoDB.getUserIdByAccessToken(token);

        return userId;
    }

    public String getUserIdByRefreshToken(String token) {

        String userId = null;

        userId = mongoDB.getUserIdByRefreshToken(token);

        return userId;
    }

    /**
     * API for getting devices corresponding with user identifier
     * 
     * @param userId
     *            user identifier
     * @return ArrayList<String> - list of devices
     */
    public ArrayList<String> getDevices(String userId) {

        ArrayList<String> deviceList = new ArrayList<>();

        deviceList = mongoDB.getDevices(userId);

        return deviceList;
    }

    public Boolean hasAccessToken(String token) {

        Boolean hasAccessToken = false;
        hasAccessToken = mongoDB.hasAccessToken(token);

        return hasAccessToken;
    }

    public Boolean hasRefreshToken(String token) {

        Boolean hasRefreshToken = false;
        hasRefreshToken = mongoDB.hasRefreshToken(token);

        return hasRefreshToken;
    }

    public String getIssuedTime(String accessToken) {

        String issuedTime = mongoDB.getIssuedTime(accessToken);
        return issuedTime;

    }
}

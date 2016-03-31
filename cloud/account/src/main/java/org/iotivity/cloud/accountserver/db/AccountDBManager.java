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
            mongoDB.createTable(Constants.SESSION_TABLE);

            registerAdminAccount();

        } catch (Exception e) {

            e.printStackTrace();

        }
    }

    public static AccountDBManager getInstance() {

        return accoutDBManager;
    }

    /**
     * API for storing session information of authorized user
     *
     * @param userId
     *            user identifier
     * @param sessionCode
     *            session code
     * @return Boolean - true if stored, otherwise false
     */
    public Boolean registerUserSessionCode(String userId, String sessionCode) {

        UserSession userSession = new UserSession();

        userSession.setUserId(userId);
        userSession.setSessionCode(sessionCode);

        mongoDB.createResource(userSession);
        mongoDB.printResources();

        return true;
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
    public String getUserId(String sessionCode) {

        String userId = null;

        userId = mongoDB.getUserId(sessionCode);

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

        ArrayList<String> deviceList = mongoDB.getDevices(userId);

        return deviceList;
    }

    private void registerAdminAccount() {

        String adminId = "admin";
        String adminSessionCode = "00000000";

        UserSession userSession = new UserSession();

        userSession.setUserId(adminId);
        userSession.setSessionCode(adminSessionCode);

        mongoDB.createResource(userSession);
        mongoDB.printResources();
    }
}

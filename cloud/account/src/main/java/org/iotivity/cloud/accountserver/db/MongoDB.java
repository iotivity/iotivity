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

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;

import org.bson.Document;
import org.iotivity.cloud.accountserver.Constants;
import org.iotivity.cloud.util.Logger;

import com.mongodb.MongoClient;
import com.mongodb.client.MongoCollection;
import com.mongodb.client.MongoCursor;
import com.mongodb.client.MongoDatabase;
import com.mongodb.client.model.Filters;

/**
 *
 * This class provides a set of APIs to use MongoDB APIs.
 *
 */
public class MongoDB {

    private MongoClient   mongoClient = null;
    private MongoDatabase db          = null;

    /**
     * API creating MongoClient and initializing MongoDatabase
     * 
     * @param dbname
     *            database name to create MongoDatabase
     * @throws Exception
     */
    public MongoDB(String dbname) throws Exception {

        mongoClient = new MongoClient();
        mongoClient.dropDatabase(dbname);
        db = mongoClient.getDatabase(dbname);
    }

    /**
     * API creating collection
     * 
     * @param tableName
     *            collection name
     */
    public void createTable(String tableName) {

        db.createCollection(tableName);
    }

    /**
     * API deleting collection
     * 
     * @param tableName
     *            collection name
     */
    public void deleteTable(String tableName) {

        db.getCollection(tableName).drop();
    }

    /**
     * API getting database object
     * 
     */
    public MongoDatabase getMongoDatabase() {

        return db;
    }

    public void createResource(UserToken userToken) {
        Document doc = createDocument(userToken);
        MongoCollection<Document> collection = db
                .getCollection(Constants.TOKEN_TABLE);

        if (collection.findOneAndReplace(
                Filters.and(
                        Filters.eq(Constants.KEY_USER_ID,
                                doc.get(Constants.KEY_USER_ID)),
                        Filters.eq(Constants.KEY_ACCESS_TOKEN,
                                doc.get(Constants.KEY_ACCESS_TOKEN)),
                        Filters.eq(Constants.KEY_REFRESH_TOKEN,
                                doc.get(Constants.KEY_REFRESH_TOKEN))),
                doc) == null) {
            collection.insertOne(doc);
        }

        return;
    }

    /**
     * API for inserting device information of user
     * 
     * @param UserDevice
     *            device information of user
     */
    public void createResource(UserDevice userDevice) {

        Document doc = createDocument(userDevice);
        MongoCollection<Document> collection = db
                .getCollection(Constants.DEVICE_TABLE);

        if (collection.findOneAndReplace(
                Filters.and(
                        Filters.eq(Constants.KEY_USER_ID,
                                doc.get(Constants.KEY_USER_ID)),
                        Filters.eq(Constants.KEY_DEVICE_ID,
                                doc.get(Constants.KEY_DEVICE_ID))),
                doc) == null) {

            collection.insertOne(doc);
        }

        return;
    }

    /**
     * API for getting user identifier corresponding with session code from
     * database
     * 
     * @param sessionCode
     *            session code
     * @return String - user identifier
     */
    public String getUserIdByAccessToken(String token) {

        String userId = null;

        MongoCollection<Document> collection = db
                .getCollection(Constants.TOKEN_TABLE);

        MongoCursor<Document> cursor = collection
                .find(Filters.eq(Constants.KEY_ACCESS_TOKEN, token)).iterator();

        try {

            while (cursor.hasNext()) {

                Document doc = cursor.next();
                UserToken userToken = convertTokenDocToResource(doc);

                userId = userToken.getUserId();
                break;
            }

        } finally {

            cursor.close();
        }

        return userId;
    }

    public String getUserIdByRefreshToken(String token) {

        String userId = null;

        MongoCollection<Document> collection = db
                .getCollection(Constants.TOKEN_TABLE);

        MongoCursor<Document> cursor = collection
                .find(Filters.eq(Constants.KEY_REFRESH_TOKEN, token))
                .iterator();

        try {

            while (cursor.hasNext()) {

                Document doc = cursor.next();
                UserToken userToken = convertTokenDocToResource(doc);

                userId = userToken.getUserId();
                break;
            }

        } finally {

            cursor.close();
        }

        return userId;
    }

    /**
     * API for getting devices corresponding with user identifier from database
     * 
     * @param userId
     *            user identifier
     */
    public ArrayList<String> getDevices(String userId) {

        ArrayList<String> deviceList = new ArrayList<>();

        MongoCollection<Document> collection = db
                .getCollection(Constants.DEVICE_TABLE);

        MongoCursor<Document> cursor = collection
                .find(Filters.eq(Constants.KEY_USER_ID, userId)).iterator();

        try {

            while (cursor.hasNext()) {

                Document doc = cursor.next();
                UserDevice userDeivce = convertDeviceDocToResource(doc);

                deviceList.add(userDeivce.getDeviceId());
            }

        } finally {

            cursor.close();
        }

        return deviceList;
    }

    public Boolean hasAccessToken(String token) {

        Boolean hasAccessToken = false;

        MongoCollection<Document> collection = db
                .getCollection(Constants.TOKEN_TABLE);

        MongoCursor<Document> cursor = collection
                .find(Filters.eq(Constants.KEY_ACCESS_TOKEN, token)).iterator();

        if (cursor.hasNext())
            hasAccessToken = true;

        cursor.close();

        return hasAccessToken;
    }

    public Boolean hasRefreshToken(String token) {

        Boolean hasRefreshToken = false;

        MongoCollection<Document> collection = db
                .getCollection(Constants.TOKEN_TABLE);

        MongoCursor<Document> cursor = collection
                .find(Filters.eq(Constants.KEY_REFRESH_TOKEN, token))
                .iterator();

        if (cursor.hasNext())
            hasRefreshToken = true;

        cursor.close();

        return hasRefreshToken;
    }

    public Boolean updateResource(UserToken oldUserToken,
            UserToken newUserToken) {

        Boolean updateResource = false;
        String userId = oldUserToken.getUserId();
        String oldRefreshToken = oldUserToken.getRefreshToken();

        Document doc = createDocument(newUserToken);

        MongoCollection<Document> collection = db
                .getCollection(Constants.TOKEN_TABLE);

        // update
        if (collection.findOneAndReplace(
                Filters.and(Filters.eq(Constants.KEY_USER_ID, userId), Filters
                        .eq(Constants.KEY_REFRESH_TOKEN, oldRefreshToken)),
                doc) != null) {

            // collection.insertOne(doc);
            updateResource = true;

        } else {
            Logger.e("UpdateResource failed!");
        }

        return updateResource;
    }

    public String getIssuedTime(String accessToken) {

        MongoCollection<Document> collection = db
                .getCollection(Constants.TOKEN_TABLE);

        MongoCursor<Document> cursor = collection
                .find(Filters.eq(Constants.KEY_ACCESS_TOKEN, accessToken))
                .iterator();

        String issuedTime = null;

        try {

            while (cursor.hasNext()) {

                Document doc = cursor.next();
                UserToken userToken = convertTokenDocToResource(doc);

                issuedTime = userToken.getIssuedTime();
                break;
            }

        } finally {

            cursor.close();
        }

        return issuedTime;

    }

    public void printResources() {

        ArrayList<UserDevice> dlist = readDeviceResources();
        int size = dlist.size();
        
        Logger.i("[" + Constants.DEVICE_TABLE + "]Table");
        for (int i = 0; i < size; i++) {

            UserDevice item = dlist.get(i);

            Logger.i("[" + i + "]" + item.getUserId() + ", "
                    + item.getDeviceId());
        }

        /*
         * ArrayList<UserSession> slist = readSessionResources(); size =
         * slist.size();
         * 
         * Logger.i("*Table: " + Constants.SESSION_TABLE);
         * 
         * for (int i = 0; i < size; i++) {
         * 
         * UserSession item = slist.get(i);
         * 
         * Logger.i("[" + i + "]" + item.getUserId() + ", " +
         * item.getSessionCode());
         * 
         * }
         */

        ArrayList<UserToken> tlist = readUserTokenResources();
        size = tlist.size();

        Logger.i("[" + Constants.TOKEN_TABLE + "]Table");

        for (int i = 0; i < size; i++) {

            UserToken item = tlist.get(i);

            Logger.i("[" + i + "]" + item.getUserId() + "/"
                    + item.getAccessToken() + "/" + item.getRefreshToken() + "/"
                    + item.getIssuedTime());

        }
    }

    private Document createDocument(UserToken userToken) {

        String userId = userToken.getUserId();
        String accessToken = userToken.getAccessToken();
        String refreshToken = userToken.getRefreshToken();

        DateFormat f = new SimpleDateFormat("yyyyMMddkkmm");
        Date currentDate = new Date();

        String issuedTime = f.format(currentDate);

        Document doc = new Document(Constants.KEY_USER_ID, userId)
                .append(Constants.KEY_ACCESS_TOKEN, accessToken)
                .append(Constants.KEY_REFRESH_TOKEN, refreshToken)
                .append(Constants.KEY_ISSUED_TIME, issuedTime);

        return doc;
    }

    private Document createDocument(UserDevice userDevice) {

        Document doc = new Document(Constants.KEY_USER_ID,
                userDevice.getUserId()).append(Constants.KEY_DEVICE_ID,
                        userDevice.getDeviceId());

        return doc;
    }

    private UserToken convertTokenDocToResource(Document doc) {

        UserToken userToken = new UserToken();

        String userId = doc.getString(Constants.KEY_USER_ID);
        String accessToken = doc.getString(Constants.KEY_ACCESS_TOKEN);
        String refreshToken = doc.getString(Constants.KEY_REFRESH_TOKEN);
        String issuedTime = doc.getString(Constants.KEY_ISSUED_TIME);

        // Logger.d("issuedTime: " + issuedTime);
        userToken.setUserToken(userId, accessToken, refreshToken);
        userToken.setIssuedTime(issuedTime);

        return userToken;
    }

    private UserDevice convertDeviceDocToResource(Document doc) {

        UserDevice userDevice = new UserDevice();

        userDevice.setUserId(doc.getString(Constants.KEY_USER_ID));
        userDevice.setDeviceId(doc.getString(Constants.KEY_DEVICE_ID));

        return userDevice;
    }

    private ArrayList<UserToken> readUserTokenResources() {

        ArrayList<UserToken> userTokenList = new ArrayList<>();

        MongoCollection<Document> collection = db
                .getCollection(Constants.TOKEN_TABLE);
        MongoCursor<Document> cursor = collection.find().iterator();

        while (cursor.hasNext()) {

            Document doc = cursor.next();
            userTokenList.add(convertTokenDocToResource(doc));
        }

        cursor.close();

        return userTokenList;
    }

    private ArrayList<UserDevice> readDeviceResources() {

        ArrayList<UserDevice> userDeviceList = new ArrayList<>();

        MongoCollection<Document> collection = db
                .getCollection(Constants.DEVICE_TABLE);
        MongoCursor<Document> cursor = collection.find().iterator();

        while (cursor.hasNext()) {

            Document doc = cursor.next();
            userDeviceList.add(convertDeviceDocToResource(doc));
        }

        cursor.close();

        return userDeviceList;
    }

}

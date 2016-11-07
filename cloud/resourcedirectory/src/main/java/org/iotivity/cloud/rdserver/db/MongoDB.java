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
package org.iotivity.cloud.rdserver.db;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;

import org.bson.Document;
import org.iotivity.cloud.rdserver.Constants;
import org.iotivity.cloud.rdserver.resources.presence.resource.ResPresencePayload;

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
        deleteTable(tableName);
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

    private Document createDocument(HashMap<Object, Object> storeRes) {

        Document doc = new Document();
        Set<Entry<Object, Object>> resEntrySet = storeRes.entrySet();
        Iterator<Entry<Object, Object>> entryIter = resEntrySet.iterator();

        while (entryIter.hasNext()) {
            Map.Entry<Object, Object> entry = (Map.Entry<Object, Object>) entryIter
                    .next();
            doc.append(entry.getKey().toString(), entry.getValue());
        }

        return doc;
    }

    private ArrayList<Document> createDocuments(
            ArrayList<HashMap<Object, Object>> storeResList) {

        Iterator<HashMap<Object, Object>> resListIter = storeResList.iterator();

        ArrayList<Document> docList = new ArrayList<>();

        while (resListIter.hasNext()) {
            Document doc = new Document();

            HashMap<Object, Object> storeRes = resListIter.next();
            Set<Entry<Object, Object>> resEntrySet = storeRes.entrySet();
            Iterator<Entry<Object, Object>> entryIter = resEntrySet.iterator();

            while (entryIter.hasNext()) {
                Map.Entry<Object, Object> entry = (Map.Entry<Object, Object>) entryIter
                        .next();
                doc.append(entry.getKey().toString(), entry.getValue());
            }
            docList.add(doc);
        }

        return docList;
    }

    private HashMap<Object, Object> convertDocumentToHashMap(Document doc) {
        HashMap<Object, Object> resourceMap = new HashMap<Object, Object>();

        Set<Entry<String, Object>> entrySet = doc.entrySet();
        Iterator<Entry<String, Object>> entryIter = entrySet.iterator();
        while (entryIter.hasNext()) {
            Map.Entry<String, Object> entry = (Map.Entry<String, Object>) entryIter
                    .next();
            if (entry.getValue() != null) {
                resourceMap.put(entry.getKey().toString(), entry.getValue());
            }
        }

        return resourceMap;
    }

    /**
     * API for storing information of published resources
     *
     * @param publishPayloadFormat
     *            information of published resources to store in collection
     * @param tableName
     *            collection name
     */
    public ArrayList<ResPresencePayload> createRDResource(
            ArrayList<HashMap<Object, Object>> storeResList, String tableName) {
        ArrayList<Document> docList = createDocuments(storeResList);
        Iterator<Document> docIter = docList.iterator();

        MongoCollection<Document> collection = db.getCollection(tableName);

        ArrayList<ResPresencePayload> resPayloadList = new ArrayList<>();

        while (docIter.hasNext()) {
            Document doc = docIter.next();
            byte trigger = 0;

            if (collection.findOneAndReplace(
                    Filters.and(
                            Filters.eq(Constants.DEVICE_ID,
                                    doc.get(Constants.DEVICE_ID)),
                            Filters.eq(Constants.INS, doc.get(Constants.INS))),
                    doc) == null) {
                collection.insertOne(doc);
                trigger = Constants.RES_CREATE;

            } else {
                trigger = Constants.RES_CHANGE;

            }

            resPayloadList.add(makeResourcePresencePayload(doc, trigger));
        }
        return resPayloadList;
    }

    public void createDevicePresenceResource(HashMap<Object, Object> storeRes,
            String tableName) {

        Document doc = createDocument(storeRes);
        MongoCollection<Document> collection = db.getCollection(tableName);

        if (collection
                .findOneAndReplace(
                        Filters.and(Filters.eq(Constants.DEVICE_ID,
                                doc.get(Constants.DEVICE_ID))),
                        doc) == null) {

            collection.insertOne(doc);
        }

        return;
    }

    private ResPresencePayload makeResourcePresencePayload(Document doc,
            byte trigger) {

        ResPresencePayload resPayload = new ResPresencePayload();

        resPayload.setTrg(trigger);

        Object rt = doc.get(Constants.RESOURCE_TYPE);
        if (rt != null) {
            resPayload.setRt(rt.toString());
        }
        Object href = doc.get(Constants.HREF);
        if (href != null) {
            Object di = doc.get(Constants.DEVICE_ID);
            if (di != null) {
                resPayload.setHref(href.toString());
            }
        }
        Object ttl = doc.get(Constants.RESOURCE_TTL);
        if (ttl != null) {
            resPayload.setTtl((int) ttl);
        }
        return resPayload;
    }

    public String readDeviceState(String deviceId, String tableName) {

        String deviceState = null;

        MongoCollection<Document> collection = db.getCollection(tableName);

        MongoCursor<Document> cursor = collection
                .find(Filters.eq(Constants.DEVICE_ID, deviceId))
                .iterator();

        try {

            while (cursor.hasNext()) {
                Document doc = cursor.next();
                deviceState = doc.getString(Constants.PRESENCE_STATE);
                break;
            }

        } finally {

            cursor.close();
        }

        return deviceState;
    }

    public ArrayList<HashMap<Object, Object>> readResourceAboutDid(String di, String tableName) {
        MongoCollection<Document> collection = db.getCollection(tableName);
        ArrayList<HashMap<Object, Object>> resList = null;
        MongoCursor<Document> cursor = collection
                .find(Filters.eq(Constants.DEVICE_ID, di))
                .iterator();

        if (cursor.hasNext()) {
            resList = new ArrayList<>();
            try {
                while (cursor.hasNext()) {
                    Document doc = cursor.next();
                    resList.add(convertDocumentToHashMap(doc));
                }
            } finally {
                cursor.close();
            }
        }

        return resList;
    }

    /**
     * API for finding resources matched filterValue of filterKey and a
     * particular device ID in collection
     *
     * @param di
     *            device id
     * @param filterKey
     *            field name in collection
     * @param filterValue
     *            field value about field name
     * @param tableName
     *            collection name
     * @return ArrayList<PublishPayloadFormat> - array list of resource
     *         information
     */
    public ArrayList<HashMap<Object, Object>> readResourceAboutDidAndFilter(String di,
            String filterKey, String filterValue, String tableName) {
        MongoCollection<Document> collection = db.getCollection(tableName);
        ArrayList<HashMap<Object, Object>> resList = null;
        MongoCursor<Document> cursor = collection
                .find(Filters.and(Filters.eq(Constants.DEVICE_ID, di),
                        Filters.eq(filterKey, filterValue)))
                .iterator();

        if (cursor.hasNext()) {
            resList = new ArrayList<>();
            try {
                while (cursor.hasNext()) {
                    Document doc = cursor.next();
                    resList.add(convertDocumentToHashMap(doc));
                }
            } finally {
                cursor.close();
            }
        }

        return resList;
    }

    public Object readInsAboutDid(String di, String href, String tableName) {
        MongoCollection<Document> collection = db.getCollection(tableName);
        MongoCursor<Document> cursor = collection
                .find(Filters.and(Filters.eq(Constants.DEVICE_ID, di),
                        Filters.eq(Constants.HREF, href)))
                .iterator();
        try {
            while (cursor.hasNext()) {
                Document doc = cursor.next();
                return doc.get(Constants.INS);
            }
        } finally {
            cursor.close();
        }
        return null;
    }

    /**
     * API for deleting resources about a particular device ID in collection
     *
     * @param di
     *            device id
     * @param tableName
     *            collection name
     */
    public ArrayList<ResPresencePayload> deleteResourceAboutDi(String di,
            String tableName) {

        MongoCollection<Document> collection = db.getCollection(tableName);

        MongoCursor<Document> cursor = collection
                .find(Filters.eq(Constants.DEVICE_ID, di)).iterator();

        ArrayList<ResPresencePayload> resPayloadList = new ArrayList<>();

        try {
            while (cursor.hasNext()) {
                Document doc = cursor.next();
                resPayloadList.add(
                        makeResourcePresencePayload(doc, Constants.RES_DELETE));
            }

        } finally {

            cursor.close();
        }

        collection.deleteMany(Filters.eq(Constants.DEVICE_ID, di));

        return resPayloadList;
    }

    /**
     * API for deleting resources about a particular device ID and ins in
     * collection
     *
     * @param di
     *            device id
     * @param ins
     *            ins
     * @param tableName
     *            collection name
     */
    public ArrayList<ResPresencePayload> deleteResourceAboutDiAndIns(String di,
            String ins, String tableName) {

        MongoCollection<Document> collection = db.getCollection(tableName);

        MongoCursor<Document> cursor = collection
                .find(Filters.and(Filters.eq(Constants.DEVICE_ID, di),
                        Filters.eq(Constants.INS, ins)))
                .iterator();

        ArrayList<ResPresencePayload> resPayloadList = new ArrayList<>();

        try {
            while (cursor.hasNext()) {
                Document doc = cursor.next();
                resPayloadList.add(
                        makeResourcePresencePayload(doc, Constants.RES_DELETE));
            }

        } finally {

            cursor.close();
        }

        collection.deleteOne(Filters.and(Filters.eq(Constants.DEVICE_ID, di),
                Filters.eq(Constants.INS, ins)));

        return resPayloadList;

    }
}

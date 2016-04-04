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
package org.iotivity.cloud.rdserver;

import java.util.ArrayList;
import java.util.Iterator;

import org.bson.Document;
import org.iotivity.cloud.rdserver.resources.LinksPayloadFormat;
import org.iotivity.cloud.rdserver.resources.PublishPayloadFormat;

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

    private ArrayList<Document> createDocuments(
            PublishPayloadFormat publishPayloadFormat) {

        Iterator<LinksPayloadFormat> linksPayloadFormatIter = publishPayloadFormat
                .getLinks().iterator();
        ArrayList<Document> docList = new ArrayList<Document>();

        while (linksPayloadFormatIter.hasNext()) {
            LinksPayloadFormat links = linksPayloadFormatIter.next();
            Document doc = new Document(Constants.RS_DEVICE_NAME,
                    publishPayloadFormat.getDeviceName())
                            .append(Constants.RS_DEVICE_ID,
                                    publishPayloadFormat.getDi())
                            .append(Constants.RS_BASE_URI,
                                    publishPayloadFormat.getBaseUri())
                            .append(Constants.RS_BITMAP,
                                    publishPayloadFormat.getBitmap())
                            .append(Constants.RS_HOSTING_PORT,
                                    publishPayloadFormat.getPort())
                            .append(Constants.RS_INS,
                                    publishPayloadFormat.getIns())
                            .append(Constants.RS_RTS,
                                    publishPayloadFormat.getRts())
                            .append(Constants.RS_DREL,
                                    publishPayloadFormat.getDrel())
                            .append(Constants.RS_TTL,
                                    publishPayloadFormat.getTtl())
                            .append(Constants.RS_HREF, links.getHref())
                            .append(Constants.RS_RESOURCE_TYPE, links.getRt())
                            .append(Constants.RS_INTERFACE, links.getItf())
                            .append(Constants.RS_REL, links.getRel())
                            .append(Constants.RS_OBS, links.isObs())
                            .append(Constants.RS_TITLE, links.getTitle())
                            .append(Constants.RS_URI, links.getUri())
                            .append(Constants.RS_INS, links.getIns())
                            .append(Constants.RS_MEDIA_TYPE, links.getMt());
            docList.add(doc);
        }

        return docList;
    }

    private PublishPayloadFormat convertDocumentToResourceFormat(Document doc) {
        PublishPayloadFormat publishPayloadFormat = new PublishPayloadFormat();
        LinksPayloadFormat linksPayloadFormat = new LinksPayloadFormat();
        ArrayList<LinksPayloadFormat> list = new ArrayList<LinksPayloadFormat>();

        Object tmp = null;
        
        tmp = doc.get(Constants.RS_DEVICE_NAME);
        if(tmp != null) {
            publishPayloadFormat
            .setDeviceName(tmp.toString());
        }
        
        tmp = doc.get(Constants.RS_DEVICE_ID);
        if(tmp != null) {
            publishPayloadFormat.setDi(tmp.toString());
        }
        
        tmp = doc.get(Constants.RS_BASE_URI);
        if(tmp != null) {
            publishPayloadFormat.setBaseUri(tmp.toString());
        }
        
        tmp = doc.get(Constants.RS_BITMAP);
        if(tmp != null) {
            publishPayloadFormat.setBitmap((int)tmp);
        }
        
        tmp = doc.get(Constants.RS_HOSTING_PORT);
        if(tmp != null) {
            publishPayloadFormat.setPort((int)tmp);
        }
       
        tmp = doc.get(Constants.RS_INS);
        if(tmp != null) {
            publishPayloadFormat.setIns((int)tmp);
        }
        
        tmp = doc.get(Constants.RS_RTS);
        if(tmp != null) {
            publishPayloadFormat.setRts(tmp.toString());
        }
        
        tmp = doc.get(Constants.RS_DREL);
        if(tmp != null) {
            publishPayloadFormat.setDrel(tmp.toString());
        }
        
        tmp = doc.get(Constants.RS_TTL);
        if(tmp != null) {
            publishPayloadFormat.setTtl((int)tmp);
        }
        
        tmp = doc.get(Constants.RS_HREF);
        if(tmp != null) {
            linksPayloadFormat.setHref(tmp.toString());
        }
        
        tmp = doc.get(Constants.RS_RESOURCE_TYPE);
        if(tmp != null) {
            linksPayloadFormat
            .setRt((ArrayList<String>) tmp);
        }

        tmp = doc.get(Constants.RS_INTERFACE);
        if(tmp != null) {
            linksPayloadFormat
            .setItf((ArrayList<String>) tmp);
        }
        
        tmp = doc.get(Constants.RS_REL);
        if(tmp != null) {
            linksPayloadFormat.setRel(tmp.toString());
        }
        
        tmp = doc.get(Constants.RS_OBS);
        if(tmp != null) {
            linksPayloadFormat.setObs((boolean)tmp);
        }
        
        tmp = doc.get(Constants.RS_TITLE);
        if(tmp != null) {
            linksPayloadFormat.setTitle(tmp.toString());
        }
        
        tmp = doc.get(Constants.RS_URI);
        if(tmp != null) {
            linksPayloadFormat.setUri(tmp.toString());
        }
        
        tmp = doc.get(Constants.RS_INS);
        if(tmp != null) {
            linksPayloadFormat.setIns((int)tmp);
        }
        
        tmp = doc.get(Constants.RS_MEDIA_TYPE);
        if(tmp != null) {
            linksPayloadFormat
            .setMt((ArrayList<String>) tmp);
        }

        list.add(linksPayloadFormat);
        publishPayloadFormat.setLinks(list);

        return publishPayloadFormat;
    }

    /**
     * API for storing information of published resources
     *
     * @param publishPayloadFormat
     *            information of published resources to store in collection
     * @param tablename
     *            collection name
     */
    public void createResource(PublishPayloadFormat publishPayloadFormat,
            String tablename) {
        ArrayList<Document> docList = createDocuments(publishPayloadFormat);
        Iterator<Document> docIter = docList.iterator();

        MongoCollection<Document> collection = db.getCollection(tablename);

        while (docIter.hasNext()) {
            Document doc = docIter.next();

            if (collection.findOneAndReplace(
                    Filters.and(Filters.eq(Constants.RS_DEVICE_ID,
                            doc.get(Constants.RS_DEVICE_ID)),
                    Filters.eq(Constants.RS_INS, doc.get(Constants.RS_INS))),
                    doc) == null) {

                collection.insertOne(doc);
            }
        }
    }

    /**
     * API for finding resources matched filterValue of filterKey in collection
     *
     * @param filterKey
     *            field name in collection
     * @param filterValue
     *            field value about field name
     * @param tablename
     *            collection name
     * @return ArrayList<PublishPayloadFormat> - array list of resource
     *         information
     */
    public ArrayList<PublishPayloadFormat> readResource(String filterKey,
            String filterValue, String tablename) {
        MongoCollection<Document> collection = db.getCollection(tablename);
        ArrayList<PublishPayloadFormat> resourceFormatList = new ArrayList<PublishPayloadFormat>();
        MongoCursor<Document> cursor = collection
                .find(Filters.eq(filterKey, filterValue)).iterator();
        try {
            while (cursor.hasNext()) {
                Document doc = cursor.next();
                resourceFormatList.add(convertDocumentToResourceFormat(doc));
            }
        } finally {
            cursor.close();
        }

        return resourceFormatList;
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
     * @param tablename
     *            collection name
     * @return ArrayList<PublishPayloadFormat> - array list of resource
     *         information
     */
    public ArrayList<PublishPayloadFormat> readResourceAboutDid(String di,
            String filterKey, String filterValue, String tablename) {
        MongoCollection<Document> collection = db.getCollection(tablename);
        ArrayList<PublishPayloadFormat> resourceFormatList = new ArrayList<PublishPayloadFormat>();
        MongoCursor<Document> cursor = collection
                .find(Filters.and(Filters.eq(Constants.RS_DEVICE_ID, di),
                        Filters.eq(filterKey, filterValue)))
                .iterator();
        try {
            while (cursor.hasNext()) {
                Document doc = cursor.next();
                resourceFormatList.add(convertDocumentToResourceFormat(doc));
            }
        } finally {
            cursor.close();
        }

        return resourceFormatList;
    }

    /**
     * API for deleting resources about a particular device ID in collection
     *
     * @param di
     *            device id
     * @param tablename
     *            collection name
     */
    public void deleteResourceAboutDid(String di, String tablename) {

        MongoCollection<Document> collection = db.getCollection(tablename);

        collection.findOneAndDelete(Filters.eq(Constants.RS_DEVICE_ID, di));
    }

    /**
     * API for deleting resources about a particular device ID and ins in
     * collection
     *
     * @param di
     *            device id
     * @param ins
     *            ins
     * @param tablename
     *            collection name
     */
    public void deleteResourceAboutDidAndIns(String di, String ins,
            String tablename) {

        MongoCollection<Document> collection = db.getCollection(tablename);

        collection.findOneAndDelete(
                Filters.and(Filters.eq(Constants.RS_DEVICE_ID, di),
                        Filters.eq(Constants.RS_INS, ins)));

    }
}

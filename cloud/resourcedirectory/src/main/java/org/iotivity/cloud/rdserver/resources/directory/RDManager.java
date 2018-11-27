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
package org.iotivity.cloud.rdserver.resources.directory;

import org.iotivity.cloud.base.exception.ServerException.BadRequestException;
import org.iotivity.cloud.rdserver.Constants;
import org.iotivity.cloud.rdserver.db.DBManager;
import org.iotivity.cloud.rdserver.resources.directory.rd.InsManager;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.*;
import java.util.stream.Collectors;

/**
 *
 * This class provides a set of APIs handle requests about resource information
 *
 */
public class RDManager {
    private final static Logger                Log               = LoggerFactory.getLogger(RDManager.class);
    private InsManager                         mInsManager       = new InsManager();
    private PayloadManager                     mPayloadManager   = new PayloadManager();

    private ArrayList<HashMap<String, Object>> mResourcePresence = new ArrayList<>();

    private static final RDManager INSTANCE = new RDManager();

    public static RDManager getInstance(){
        return INSTANCE;
    }


    /**
     * API for handling resource-publish process
     *
     * @param requestPayload
     *            request payload
     * @return response payload
     */
    public HashMap<String, Object> publishResource(
            HashMap<String, Object> requestPayload) {

        HashMap<String, Object> deviceInfo = setResourceDeviceInfo(
                requestPayload);
        ArrayList<HashMap<String, Object>> links = getLinks(requestPayload);

        // check ins and set ins
        setResourceIns(deviceInfo.get(Constants.DEVICE_ID).toString(), links);

        storeResource(links, deviceInfo);

        return requestPayload;

    }

    // set di, n, lt info
    private HashMap<String, Object> setResourceDeviceInfo(
            HashMap<String, Object> payload) {
        return mPayloadManager.setPayloadData(payload,
                mPayloadManager.pubTagKey);
    }

    private ArrayList<HashMap<String, Object>> getLinks(
            HashMap<String, Object> requestPayload) {
        return (ArrayList<HashMap<String, Object>>) requestPayload
                .get(Constants.LINKS);

    }

    private void storeResource(ArrayList<HashMap<String, Object>> links,
                               HashMap<String, Object> deviceInfo) {

        ArrayList<HashMap<String, Object>> resourcePresence = new ArrayList<>();

        for (HashMap<String, Object> link : links) {
            HashMap<String, Object> storeInfo = new HashMap<>();
            HashMap<String, Object> storeLink = mPayloadManager
                    .setPayloadData(link, mPayloadManager.pubLinkKey);
            mPayloadManager.changePolicyTypeToStore(storeLink);
            storeInfo.putAll(storeLink);
            storeInfo.putAll(deviceInfo);
            storeResourceInDB(storeInfo);
            resourcePresence.add(storeInfo);
        }
        setmResourcePresence(resourcePresence);

    }

    private void storeResourceInDB(HashMap<String, Object> rdInfo) {
        HashMap<String, Object> condition = new HashMap<>();
        condition.put(Constants.DEVICE_ID, rdInfo.get(Constants.DEVICE_ID));
        condition.put(Constants.INS, rdInfo.get(Constants.INS));
        if (checkResourceExist(condition).isEmpty()) {
            DBManager.getInstance().insertRecord(Constants.RD_TABLE, rdInfo);
            // set resource presence
            rdInfo.put(Constants.TRIGGER, Constants.RES_CREATE);
        } else {
            DBManager.getInstance().insertAndReplaceRecord(Constants.RD_TABLE,
                    rdInfo);
            rdInfo.put(Constants.TRIGGER, Constants.RES_CHANGE);
        }

    }

    private void setResourceIns(String di,
                                ArrayList<HashMap<String, Object>> links) {

        for (HashMap<String, Object> link : links) {
            String href = link.get(Constants.HREF).toString();
            int ins = (int) link.get(Constants.INS);
            int newIns = checkResourceIns(di, href, ins);
            if (newIns == -1) {
                throw new BadRequestException("resource ins is not correct");
            }
            link.put(Constants.INS, newIns);
        }
    }

    private int checkResourceIns(String di, String href, int ins) {
        int storedIns = mInsManager.getIns(di, href);
        if (ins == 0) {
            if (storedIns == -1) {
                // create ins
                ins = mInsManager.createIns(di);
            } else {
                ins = storedIns;
            }
        } else {
            if (ins != storedIns) {
                ins = -1;
            }
        }
        return ins;
    }

    /**
     * API for handling resource-delete process
     *
     * @param di
     *            device id
     * @param insList
     *            unique id of resource
     */
    public void deleteResource(String di, List<String> insList) {

        HashMap<String, Object> condition = new HashMap<>();
        condition.put(Constants.DEVICE_ID, di);

        ArrayList<HashMap<String, Object>> foundRecord = new ArrayList<>();

        if (insList == null) {
            foundRecord = checkResourceExist(condition);
            DBManager.getInstance().deleteRecord(Constants.RD_TABLE, condition);
        } else {
            for (String ins : insList) {
                condition.put(Constants.INS, Integer.parseInt(ins));
                if (!checkResourceExist(condition).isEmpty()) {
                    foundRecord.add(checkResourceExist(condition).get(0));
                    DBManager.getInstance().deleteRecord(Constants.RD_TABLE,
                            condition);
                }
            }
        }

        if (!foundRecord.isEmpty()) {
            // set resource presence
            for (HashMap<String, Object> record : foundRecord) {
                record.put(Constants.TRIGGER, Constants.RES_DELETE);
            }
            setmResourcePresence(foundRecord);
        }
    }

    private ArrayList<HashMap<String, Object>> checkResourceExist(
            HashMap<String, Object> condition) {
        ArrayList<HashMap<String, Object>> records = DBManager.getInstance()
                .selectRecord(Constants.RD_TABLE, condition);

        return records;
    }

    /**
     * API for handling resource-discover process
     *
     * @param diList
     *            list of device id
     * @param rtList
     *            list of resource type
     * @param ifList
     *            list of resource interface
     * @return response payload
     */
    public ArrayList<Object> discoverResource(List<String> diList,
                                              List<String> rtList, List<String> ifList) {

        ArrayList<Object> response = new ArrayList<>();

        if (diList == null) {
            return response;
        }

        final List<String> diFiltered = filterDeviceByStatus(diList);
        if(diFiltered == null || diFiltered.size() == 0){
            return response;
        }
        final HashMap<String,Object> conditions = new HashMap<>();
        conditions.put("di",buildInQuery(diFiltered));

        if(rtList != null && rtList.size() > 0){
            conditions.put("rt",buildInQuery(rtList));
        }

        if(ifList != null && ifList.size() > 0){
            conditions.put("if",buildInQuery(rtList));
        }

        readResource(conditions,response);

        Log.debug("discovery payload : " + response);

        return response;
    }

    private void readResource(
            HashMap<String, Object> condition, ArrayList<Object> response) {

        ArrayList<HashMap<String, Object>> records = DBManager.getInstance()
                .selectRecord(Constants.RD_TABLE, condition);
        if (!records.isEmpty()) {
            response.addAll(makeDiscoverResponseSegment(records));
        }
    }

    private List<String> filterDeviceByStatus(final List<String> diList){

        final HashMap<String,Object> query = new HashMap<>();
        if(diList != null && diList.size() > 0){
            query.put("di",buildInQuery(diList));
        }
        query.put("state",Constants.PRESENCE_ON);
        ArrayList<HashMap<String, Object>> records = DBManager.getInstance()
                .selectRecord(Constants.PRESENCE_TABLE, query);
        return records.stream().map(record -> (String)record.get("di")).collect(Collectors.toList());
    }

    private Collection<HashMap<String, Object>> makeDiscoverResponseSegment(
            ArrayList<HashMap<String, Object>> records) {

        final Map<String, HashMap<String, Object>> result = new HashMap<>();
        for (final HashMap<String, Object> record : records) {
            final String di = (String) record.get(Constants.DEVICE_ID);
            ArrayList<Object> links;
            if (!result.containsKey(record.get(Constants.DEVICE_ID))) {
                final HashMap<String, Object> discovered = createDevice(record);
                result.put(di, discovered);
                links = (ArrayList<Object>) discovered.get(Constants.LINKS);
            } else {
                links = (ArrayList<Object>) result.get(di).get(Constants.LINKS);
            }
            HashMap<String, Object> discoveredLinks = mPayloadManager
                    .setPayloadData(record, mPayloadManager.discoverLinkKey);
            mPayloadManager.changePolicyTypeToDiscover(discoveredLinks);
            links.add(discoveredLinks);
        }
        return result.values();
    }

    private HashMap<String,Object> createDevice(final  HashMap<String, Object> record){
        HashMap<String, Object> discoverTag = mPayloadManager.setPayloadData(record, mPayloadManager.discoverTagKey);
        discoverTag.put(Constants.LINKS, new ArrayList<>());
        return discoverTag;
    }

    private void setmResourcePresence(
            ArrayList<HashMap<String, Object>> resourcePresence) {
        this.mResourcePresence = resourcePresence;
    }

    /**
     * API for getting resource information to notify
     *
     * @return resource information
     */
    public ArrayList<HashMap<String, Object>> getmResourcePresence() {
        return mResourcePresence;
    }


    private <T extends Object> HashMap<String,Object> buildInQuery(final Collection<T> items){
        final HashMap<String,Object> query = new HashMap<>();
        query.put("$in",items);
        return query;
    }

}
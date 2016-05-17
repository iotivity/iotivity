// ******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
//
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

package oic.ctt.network.coap;

import oic.ctt.formatter.CborManager;
import oic.ctt.formatter.OCPayloadType;
import oic.ctt.logger.CTLogger;
import oic.ctt.network.OICHelper;
import oic.ctt.network.OICObserver;
import oic.ctt.network.OICRequestData;

import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import org.slf4j.Logger;
import org.ws4d.coap.connection.OICCTFlags;
import org.ws4d.coap.interfaces.CoapResponse;
import org.ws4d.coap.interfaces.CoapServerChannel;
import org.ws4d.coap.messages.CoapMediaType;
import org.ws4d.coap.messages.CoapResponseCode;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map.Entry;

/**
 * OIC CoAP Resource class. It managers and contains information about an OIC
 * resource
 */
public class OICCoapResource {
    private Logger                       mlogger                = CTLogger
            .getInstance();
    public static byte[]                 sRemoveObserverToken   = null;

    private String                       mResourceUri;
    private ArrayList<String>            mResourceTypes;
    private ArrayList<String>            mResourceInterfaces;
    private boolean                      mObservable;
    private HashMap<String, OICObserver> mOicObserverMap        = new HashMap<String, OICObserver>();
    private HashMap<Object, Object>      mOicResourceAttributes = new HashMap<Object, Object>();
    private ArrayList<OICRequestData>    mReceivedRequests      = new ArrayList<OICRequestData>();
    private boolean                      mResetReceived         = false;
    private String                       mResetToken            = "";

    /**
     * OIC CoAP Resource constructor
     *
     * @param uri
     *            URI of the resource
     */
    public OICCoapResource(String uri) {
        this(uri, new ArrayList<String>(), new ArrayList<String>(), true);
    }

    /**
     * OIC CoAP Resource constructor
     *
     * @param uri
     *            URI of the resource
     * @param resourceType
     *            OIC Resource Type (rt)
     * @param observable
     *            Boolean value indicating whether it is observable or not
     */
    public OICCoapResource(String uri, ArrayList<String> resourceTypes,
            ArrayList<String> resourceInterfaces, boolean observable) {
        this.mResourceUri = uri;
        this.mResourceTypes = resourceTypes;
        this.mResourceInterfaces = resourceInterfaces;
        this.mObservable = observable;
    }

    public OICCoapResource(String uri, String resourceType,
            String resourceInterface, boolean observable) {
        this.mResourceUri = uri;
        this.mResourceTypes = new ArrayList<String>();
        this.mResourceTypes.add(resourceType);
        this.mResourceInterfaces = new ArrayList<String>();
        this.mResourceInterfaces.add(resourceInterface);
        this.mObservable = observable;
    }

    /**
     * Checks if the observer has already subscribed the resource
     *
     * @param token
     *            Exchange token identifying the observer
     * @return True if observer already exists, False otherwise
     */
    public boolean hasObserver(String token) {
        if (mOicObserverMap.get(token) != null)
            return true;

        return false;
    }

    /**
     * Adds a client observer to the resource (Subscribe)
     *
     * @param token
     *            Exchange token from the observe request
     * @param oicObserver
     *            OICObserver object
     */
    public void addObserver(String token, OICObserver oicObserver) {
        mOicObserverMap.put(token, oicObserver);
    }

    /**
     * Gets the observer from the list of observers
     *
     * @param token
     *            Exchange token that identifies the observer
     * @return OICObserver object
     */
    public OICObserver getObserver(String token) {
        return mOicObserverMap.get(token);
    }

    /**
     * Removes a client observer from this resource (Unsubscribe)
     *
     * @param token
     */
    public void removeObserver(String token) {
        mOicObserverMap.remove(token);
    }

    private void notifyObservers(byte[] payload) {
        for (Entry<String, OICObserver> entry : mOicObserverMap.entrySet()) {
            OICObserver observer = entry.getValue();
            String token = OICHelper
                    .bytesToHex(observer.getCoapRequest().getToken());

            mResetReceived = OICCTFlags.isMarkedForRemoval(token);

            if (mResetReceived)
                mResetToken = token;

            if (OICCTFlags.isUnreachable(token)
                    || OICCTFlags.isMarkedForRemoval(token)) {
                mlogger.info("Removing observer");
                removeObserver(token);
                continue;
            }

            CoapServerChannel notificationChannel = observer
                    .getCoapServerChannel();

            CoapResponse response = notificationChannel.createNotification(
                    observer.getCoapRequest(), CoapResponseCode.Content_205,
                    observer.getSeqNumber());

            // response.setObserveOption(OICHelper.hexStringToByteArray(String
            // .format("%08d", observer.getSeqNumber())));
            byte[] bytes = ByteBuffer.allocate(4)
                    .putInt(observer.getSeqNumber()).array();
            response.setObserveOption(bytes);

            response.setToken(observer.getCoapRequest().getToken());
            response.setPayload(payload);
            response.setContentType(CoapMediaType.cbor);

            OICCTFlags.addToken(response.getMessageID(), token);
            notificationChannel.sendNotification(response);
        }
    }

    /**
     * Check if a RST (Reset) message was received or not
     *
     * @return True if a RST have arrived, False otherwise
     */
    public boolean hasReset() {
        return this.mResetReceived;
    }

    /**
     * Sets the Reset Flag to mark that a RST (Reset) message was received
     *
     * @param resetReceived
     *            True to set and False to unset
     */
    public void setResetReceived(boolean resetReceived) {
        this.mResetReceived = resetReceived;
    }

    /**
     * Get the token for which a RST was received
     *
     * @return A string containing the RST token
     */
    public String getResetToken() {
        return mResetToken;
    }

    /**
     * Gets the URI of this resource
     *
     * @return String containing the URI
     */
    public String getResourceUri() {
        return mResourceUri;
    }

    /**
     * Gets the OIC resource type value
     *
     * @return String containing the resource type
     */
    public ArrayList<String> getResourceTypes() {
        return mResourceTypes;
    }

    /**
     * Adds an OIC resource interface to this resource
     *
     * @param resourceInterface
     *            A OIC resource interface (String)
     */
    public void addResourceInterface(String resourceInterface) {
        mResourceInterfaces.add(resourceInterface);
    }

    /**
     * Adds an OIC resource attribute to this resource
     *
     * @param attribute
     *            key
     * @param attribute
     *            value
     */
    public void addResourceAttribute(Object key, Object value) {
        mOicResourceAttributes.put(key, value);
    }

    /**
     * Adds an OIC resource attribute to this resource
     *
     * @param attribute
     *            key
     * @param attribute
     *            value
     */
    public void addResourceAttributes(HashMap<Object, Object> attributeMap) {
        mOicResourceAttributes.putAll(attributeMap);
    }

    public HashMap<Object, Object> getResourceAttribute() {
        return (HashMap<Object, Object>) mOicResourceAttributes.clone();
    }

    /**
     * Gets a list of OIC resource interfaces supported by this resource
     *
     * @return Strings containing a resource interface value
     */
    public ArrayList<String> getResourceInterfaces() {
        return mResourceInterfaces;
    }

    /**
     * Checks whether this resource is obserable or not
     *
     * @return true if observable, false otherwise
     */
    public boolean isObservable() {
        return mObservable;
    }

    /**
     * Adds OICRequestData to a storage list in the resource
     *
     * @param oicRequestData
     *            OICRequestData type object
     */
    public void addRequestToList(OICRequestData oicRequestData) {
        this.mReceivedRequests.add(oicRequestData);
    }

    /**
     * Removes OICRequestData from the received request list.
     *
     * @param oicRequestData
     *            Request data to be removed from the list.
     */
    public void removeRequestFromList(OICRequestData oicRequestData) {
        this.mReceivedRequests.remove(oicRequestData);
    }

    /**
     * Clears the local received requests buffer
     */
    public void clearRequestList() {
        this.mReceivedRequests.clear();
    }

    /**
     * Gets all the request messages stored in local buffer
     *
     * @return An ArrayList containing OICRequest objects
     */
    public ArrayList<OICRequestData> getRequestList() {
        return mReceivedRequests;
    }

    /**
     * Gets the representation of this resource
     *
     * @return String containing the json representation
     */
    public byte[] getResourceRepresentation() {
        JSONObject jsonObject = new JSONObject();
        // JSONArray jsonArray = new JSONArray();
        // JSONObject jsonProp = new JSONObject();
        // JSONObject jsonRep = new JSONObject();
        // jsonRep.putAll(mOicResourceAttributes);
        //
        // jsonProp.put("if", mResourceInterface);
        // jsonProp.put("rt", mResourceType);
        //
        // jsonObject.put("href", mResourceUri);
        // jsonObject.put("prop", jsonProp);
        // jsonObject.put("rep", jsonRep);
        //
        // jsonArray.add(jsonObject);

        jsonObject.putAll(mOicResourceAttributes);

        // String jsonStr = jsonArray.toJSONString().replace("\\", "");
        String jsonStr = jsonObject.toJSONString().replace("\\", "");
        mlogger.info("getResourceRepresentation: " + jsonStr);
        return CborManager.convertToCbor(jsonStr,
                OCPayloadType.PAYLOAD_TYPE_REPRESENTATION);
    }

    /**
     * Update the resource with given json payload, also notifies the observers
     *
     * @param payloadString
     *            json representation in string
     */
    public void updateResourceRespresentation(String payloadString) {
        mlogger.info("POST Request for UpdateResource: " + payloadString);
        // String substr = payloadString.substring(1, payloadString.length() -
        // 1);
        String substr = payloadString.substring(0, payloadString.length());

        ObjectMapper mapper = new ObjectMapper();
        try {
            JsonNode jsonNodeTree = mapper.readTree(substr);

            // JsonNode repJsonNode = jsonNodeTree.get("rep");
            JsonNode repJsonNode = jsonNodeTree;
            Iterator<String> it = repJsonNode.fieldNames();
            while (it.hasNext()) {
                String subField = it.next();
                if (mOicResourceAttributes.containsKey(subField))
                    mOicResourceAttributes.put(subField,
                            repJsonNode.get(subField));
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

        if (mObservable)
            notifyObservers(getResourceRepresentation());
    }

    /**
     * Update the resource by adding 5 to each values and notifies available
     * observers
     *
     */
    public void updateResourceRespresentation() {
        mlogger.info("UpdateResource by 5");
        mlogger.debug(
                "before update: " + new String(getResourceRepresentation()));

        for (Entry<Object, Object> resAtb : mOicResourceAttributes.entrySet()) {
            if (resAtb.getValue() instanceof Integer
                    || resAtb.getValue() instanceof Double) {
                Integer val = (Integer) resAtb.getValue() + 5;
                mOicResourceAttributes.put(resAtb.getKey(), val);
            }
        }
        mlogger.debug(
                "after update: " + new String(getResourceRepresentation()));
        if (mObservable)
            notifyObservers(getResourceRepresentation());
    }

    /**
     * Updates complete representation of the resouce. All previous values are
     * removed and new values are set according to the given payload, also
     * notifies the observers.
     *
     * @param payloadString
     *            json representation in string
     */
    public void updateCompleteResourceRespresentation(String payloadString) {
        mlogger.info("PUT Request for UpdateResource: " + payloadString);
        String substr = payloadString.substring(1, payloadString.length() - 1);

        ObjectMapper mapper = new ObjectMapper();
        try {
            mOicResourceAttributes.clear();
            JsonNode jsonNodeTree = mapper.readTree(substr);

            // JsonNode repJsonNode = jsonNodeTree.get("rep");
            JsonNode repJsonNode = jsonNodeTree;
            Iterator<String> it = repJsonNode.fieldNames();
            while (it.hasNext()) {
                String subField = it.next();
                mOicResourceAttributes.put(subField, repJsonNode.get(subField));
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

        if (mObservable)
            notifyObservers(getResourceRepresentation());
    }

    private ArrayList<String> getAsList(String string) {
        String[] list = string.replaceAll("[\\[\"\\]]", "").split(",");
        ArrayList<String> ifList = new ArrayList<String>(Arrays.asList(list));
        return ifList;
    }
}

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
import org.ws4d.coap.messages.CoapResponseCode;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;

import java.io.IOException;
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
    private String                       mResourceType;
    private String                       mResourceInterface;
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
        this(uri, "", "", true);
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
    public OICCoapResource(String uri, String resourceType,
            String resourceInterface, boolean observable) {
        this.mResourceUri = uri;
        this.mResourceType = resourceType;

        if (resourceInterface.length() > 0)
            this.addResourceInterface(resourceInterface);

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

    private void notifyObservers() {

        for (Entry<String, OICObserver> entry : mOicObserverMap.entrySet()) {

            OICObserver observer = entry.getValue();
            String token = OICHelper.bytesToHex(observer.getCoapRequest()
                    .getToken());

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
                    observer.getCoapRequest(), CoapResponseCode.Changed_204,
                    observer.getSeqNumber());
            response.setObserveOption(OICHelper.hexStringToByteArray(String
                    .format("%08d", observer.getSeqNumber())));

            response.setToken(observer.getCoapRequest().getToken());

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
    public String getResourceType() {
        return mResourceType;
    }

    /**
     * Adds an OIC resource interface to this resource
     * 
     * @param resourceInterface
     *            A OIC resource interface (String)
     */
    public void addResourceInterface(String resourceInterface) {
        mResourceInterface = resourceInterface;
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

    public HashMap<Object, Object> getResourceAttribute() {
        return (HashMap<Object, Object>) mOicResourceAttributes.clone();
    }

    /**
     * Gets a list of OIC resource interfaces supported by this resource
     * 
     * @return Strings containing a resource interface value
     */
    public String getResourceInterfaces() {
        return mResourceInterface;
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
        JSONArray jsonArray = new JSONArray();
        JSONObject jsonProp = new JSONObject();
        JSONObject jsonRep = new JSONObject();
        jsonRep.putAll(mOicResourceAttributes);

        jsonProp.put("if", mResourceInterface);
        jsonProp.put("rt", mResourceType);

        jsonObject.put("href", mResourceUri);
        jsonObject.put("prop", jsonProp);
        jsonObject.put("rep", jsonRep);

        jsonArray.add(jsonObject);

        String jsonStr = jsonArray.toJSONString().replace("\\", "");
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
        String substr = payloadString.substring(1, payloadString.length() - 1);

        ObjectMapper mapper = new ObjectMapper();
        try {
            JsonNode jsonNodeTree = mapper.readTree(substr);

            JsonNode repJsonNode = jsonNodeTree.get("rep");
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
            notifyObservers();
    }

    public void updateCompleteResourceRespresentation(String payloadString) {
        mlogger.info("PUT Request for UpdateResource: " + payloadString);
        String substr = payloadString.substring(1, payloadString.length() - 1);

        ObjectMapper mapper = new ObjectMapper();
        try {
            mOicResourceAttributes.clear();
            JsonNode jsonNodeTree = mapper.readTree(substr);

            JsonNode repJsonNode = jsonNodeTree.get("rep");
            Iterator<String> it = repJsonNode.fieldNames();
            while (it.hasNext()) {
                String subField = it.next();
                mOicResourceAttributes.put(subField, repJsonNode.get(subField));
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

        if (mObservable)
            notifyObservers();
    }

    private ArrayList<String> getAsList(String string) {
        String[] list = string.replaceAll("[\\[\"\\]]", "").split(",");
        ArrayList<String> ifList = new ArrayList<String>(Arrays.asList(list));
        return ifList;
    }
}

//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= 
package oic.ctt.network.coap;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.net.Inet6Address;
import java.net.InetAddress;
import java.net.StandardProtocolFamily;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Vector;

import oic.ctt.formatter.CborManager;
import oic.ctt.formatter.OCPayloadType;
import oic.ctt.logger.CTLogger;
import oic.ctt.network.OICHelper;
import oic.ctt.network.OICObserver;
import oic.ctt.network.OICRequestData;

import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import org.slf4j.Logger;
import org.ws4d.coap.connection.BasicCoapChannelManager;
import org.ws4d.coap.interfaces.CoapChannelManager;
import org.ws4d.coap.interfaces.CoapRequest;
import org.ws4d.coap.interfaces.CoapResponse;
import org.ws4d.coap.interfaces.CoapServer;
import org.ws4d.coap.interfaces.CoapServerChannel;
import org.ws4d.coap.messages.CoapBlockOption;
import org.ws4d.coap.messages.CoapMediaType;
import org.ws4d.coap.messages.CoapResponseCode;
import org.ws4d.coap.messages.CoapBlockOption.CoapBlockSize;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;

/**
 * The OIC CoAP Server class. Provides methods to communicate with a OIC client
 * using CoAP.
 */
public class OICCoapServer {

    private Logger                           mlogger               = CTLogger
                                                                           .getInstance();

    private CoapChannelManager               mChannelManager       = null;
    private CoapServer                       mMulticastServer;
    private CoapServer                       mUnicastServer;
    private int                              mUnicastPort;
    private int                              mMulticastPort;
    private byte[]                           mIp6Token             = null;

    private final CoapBlockSize              mDefaultBlockSize     = CoapBlockSize.BLOCK_1024;
    private CoapBlockSize                    mBlockSize            = mDefaultBlockSize;
    private int                              mBlockNumber          = 0;
    private ByteArrayOutputStream            payloadBody           = new ByteArrayOutputStream();
    private CoapRequest                      firstBlkReq;

    private ArrayList<CoapRequest>           mDiscoveryRequestList = new ArrayList<CoapRequest>();
    private HashMap<String, OICCoapResource> mOicResourceMap       = new HashMap<String, OICCoapResource>();

    /**
     * Initializes a OICCoapSever with given ports
     * 
     * @param unicastPort
     *            Port number of the socket it will listen for unicast messages
     * @param multicastPort
     *            Port number of the multicast socket it will listen for
     *            advertisements
     */
    public OICCoapServer(int unicastPort, int multicastPort) {
        this.mUnicastPort = unicastPort;
        this.mMulticastPort = multicastPort;

        mMulticastServer = createMulticastServer();
        mUnicastServer = createUnicastServer();

        mChannelManager = BasicCoapChannelManager.getInstance();
    }

    /**
     * Initializes a OICCoapServer with default ports. (6298, 5683)
     */
    public OICCoapServer() {
        this(6298, 5683);
    }

    /**
     * Starts the server
     */
    public void start() {

        mChannelManager.createServerListener(mUnicastServer, mUnicastPort,
                StandardProtocolFamily.INET, false);
        mChannelManager.createServerListener(mMulticastServer, mMulticastPort,
                StandardProtocolFamily.INET, true);
        mChannelManager.createServerListener(mUnicastServer, mUnicastPort + 1,
                StandardProtocolFamily.INET6, false);
        mChannelManager.createServerListener(mMulticastServer, mMulticastPort,
                StandardProtocolFamily.INET6, true);
    }

    /**
     * Adds a resource to the server
     * 
     * @param oicCoapResource
     *            OICCoapResource object
     */
    public void addResource(OICCoapResource oicCoapResource) {
        if (oicCoapResource == null) {
            mlogger.error("Invalid resource, can't add");
            return;
        }
        mOicResourceMap.put(oicCoapResource.getResourceUri(), oicCoapResource);
    }

    /**
     * Removes a resource from the server
     * 
     * @param resourceUri
     *            URI of the resource to be removed
     */
    public void removeResource(String resourceUri) {
        mOicResourceMap.remove(resourceUri);
    }

    /**
     * Clears the local buffer of received discovery requests
     */
    public void clearDiscoveryRequests() {
        mDiscoveryRequestList.clear();
    }

    /**
     * Gets all the received discovery requests stored in the local buffer
     * 
     * @return An ArrayList of OICRequest objects
     */
    public ArrayList<OICRequestData> getDiscoveryRequests() {
        ArrayList<OICRequestData> oicRequestList = new ArrayList<OICRequestData>();

        if (mDiscoveryRequestList.isEmpty())
            mlogger.debug("Discovery Request List empty");

        for (CoapRequest discoveryRequest : mDiscoveryRequestList) {
            oicRequestList.add(OICRequestData
                    .convertRequestCoap(discoveryRequest));
        }

        return oicRequestList;
    }

    private void handleDiscoveryMessage(CoapServerChannel channel,
            CoapRequest request, boolean unicast) {
        mlogger.info("Handle Discovery Message");
        mlogger.debug("request: " + request);
        mlogger.debug("request header options: " + request.getHeaderOptions());
        mlogger.debug("request packetType: " + request.getPacketType());
        mlogger.debug("request payload: " + request.getPayloadString());
        mlogger.debug("request code: " + request.getRequestCodeString());
        mlogger.debug("request mid: " + request.getMessageID());
        mlogger.debug("request token: " + request.getTokenString());
        mlogger.debug("request uripath: " + request.getUriPath());
        if (request.getBlock2() != null)
            mlogger.debug("requesting for block num: "
                    + request.getBlock2().getNumber());
        // Handle the discovery request and send back response if
        // necessary.

        InetAddress remoteIP = channel.getRemoteAddress();

        // Check ip version
        // If IPv6, continue
        // If IPv4, wait a few seconds, check if IPv6 reply was sent, if it was
        // sent, then return
        if (remoteIP instanceof Inet6Address)
            mIp6Token = request.getToken();
        else {
            // IP4 message, wait some time
            try {
                Thread.sleep(400);
            } catch (InterruptedException ex) {
                Thread.currentThread().interrupt();
            }

            if (mIp6Token != null
                    && Arrays.equals(mIp6Token, request.getToken())) {
                // request already handled by IPv6
                return;
            }
        }

        CoapResponse response = channel.createSeparateResponse(request,
                CoapResponseCode.Content_205);
        response.setUriPath(OICHelper.getDefaultUri());

        Vector<String> query = request.getUriQuery();
        ArrayList<OICCoapResource> resourceList = new ArrayList<OICCoapResource>();
        if (query.isEmpty()) {
            for (Map.Entry<String, OICCoapResource> entry : mOicResourceMap
                    .entrySet()) {
                OICCoapResource resource = entry.getValue();
                resourceList.add(resource);
            }
            byte[] payload = discoverJson(resourceList);
            response.setContentType(CoapMediaType.cbor);
            /* block-wise response transfer code */
            if (payload.length >= mBlockSize.getSize())
                response = blockResponseTransfer(request, response, payload);
            else
                response.setPayload(payload);
            mlogger.debug("Sending discovery response: " + response);
            mlogger.debug("request header options: "
                    + response.getHeaderOptions());
            mlogger.debug("request packetType: " + response.getPacketType());
            mlogger.debug("request payload: " + response.getPayloadString());
            mlogger.debug("request code: " + response.getResponseCodeString());
            mlogger.debug("request mid: " + response.getMessageID());
            mlogger.debug("request token: " + response.getTokenString());
            if (response.getBlock2() != null)
                mlogger.debug("request block2 num: "
                        + response.getBlock2().getNumber());

            channel.sendSeparateResponse(response);
            return;
        }
        String resIf = "";
        String rt = "";
        for (String queryElement : query) {
            if (!queryElement.isEmpty()) {
                if (queryElement.split("=")[0].equals("if"))
                    resIf = queryElement.split("=")[1];
                else if (queryElement.split("=")[0].equals("rt"))
                    rt = queryElement.split("=")[1];
            }
        }
        // Check if this server has any resources, if so, send back
        // resource URI, type & interface information in json.
        for (Map.Entry<String, OICCoapResource> entry : mOicResourceMap
                .entrySet()) {
            OICCoapResource resource = entry.getValue();
            if (!rt.isEmpty() && !rt.equals(resource.getResourceType())) {
                mlogger.info("rt not match");
                continue;
            }

            if (!hasResourseInterfaceMatched(resIf, resource)) {
                mlogger.info("interface not match");
                continue;
            }
            resourceList.add(resource);
        }

        byte[] payload = discoverJson(resourceList);
        if (resourceList.isEmpty()) {
            if (unicast) {
                response = channel.createSeparateResponse(request,
                        CoapResponseCode.Not_Found_404);
            }
        } else {
            response.setContentType(CoapMediaType.cbor);
            /* block-wise response transfer code */
            if (payload.length >= mBlockSize.getSize())
                response = blockResponseTransfer(request, response, payload);
            else
                response.setPayload(payload);
        }
        channel.sendSeparateResponse(response);
    }

    private boolean hasResourseInterfaceMatched(String resIf,
            OICCoapResource resource) {
        if (resIf.isEmpty())
            return true;
        else if (resIf.equals(resource.getResourceInterfaces()))
            return true;
        return false;

        // ArrayList<String> iflist = resource.getResourceInterfaces();
        // for (String intface : iflist) {
        // if (resIf.equals(intface))
        // return true;
        // }
        // return false;
    }

    private void checkRequestWithResource(CoapServerChannel channel,
            CoapRequest request) {
        /* Check for block-wise request */
        if (request.getBlock1() != null) {
            if (request.getBlock1().getNumber() == 0)
                firstBlkReq = request;
            mlogger.info("Initiate receiving Block-wise request");
            request = blockRequestTransfer(request, channel);
            if (!request.getBlock1().isLast())
                return;
            try {
                request.setMessageID(firstBlkReq.getMessageID());
                payloadBody.flush();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        OICCoapResource matchingResource = mOicResourceMap.get(request
                .getUriPath());
        if (matchingResource == null) {
            mlogger.info("Resource NOT found!");
            CoapResponse response = channel.createSeparateResponse(request,
                    CoapResponseCode.Not_Found_404);
            channel.sendSeparateResponse(response);
            return;
        }

        // Getting the resourse type and interface
        Vector<String> query = request.getUriQuery();
        String resIf = "";
        String rt = "";
        for (String queryElement : query) {
            if (!queryElement.isEmpty()) {
                if (queryElement.split("=")[0].equals("if"))
                    resIf = queryElement.split("=")[1];
                else if (queryElement.split("=")[0].equals("rt"))
                    rt = queryElement.split("=")[1];
            }
        }

        // Checking resource type and interface match
        if (!rt.isEmpty()
                && (!matchingResource.getResourceType().equals(rt) || !hasResourseInterfaceMatched(
                        resIf, matchingResource))) {
            mlogger.info("Resource type and interface NOT match!");
            CoapResponse response = channel.createSeparateResponse(request,
                    CoapResponseCode.Not_Found_404);
            channel.sendSeparateResponse(response);
            return;
        }

        // Store the message in the particular resource
        mOicResourceMap.get(request.getUriPath()).addRequestToList(
                OICRequestData.convertRequestCoap(request));

        switch (request.getRequestCode()) {
            case GET:
                handleGET(request, channel);
                break;
            case PUT:
                handlePUT(request, channel);
                break;
            case POST:
                handlePOST(request, channel);
                break;
            case DELETE:
                handleDELETE(request, channel);
                break;
        }
    }

    private CoapServer createMulticastServer() {
        return new CoapServer() {

            @Override
            public void onSeparateResponseFailed(CoapServerChannel channel) {
            }

            @Override
            public void onRequest(CoapServerChannel channel, CoapRequest request) {
                mlogger.info("Multicast: onRequest");

                if (request.getUriPath().equals("/oic/res")) {
                    mDiscoveryRequestList.add(request);
                    handleDiscoveryMessage(channel, request, false);
                    return;
                }

                checkRequestWithResource(channel, request);
            }

            @Override
            public CoapServer onAccept(CoapRequest request) {
                mlogger.info("Multicast: onAccept");
                return this;
            }
        };
    }

    private CoapServer createUnicastServer() {
        return new CoapServer() {

            @Override
            public void onSeparateResponseFailed(CoapServerChannel channel) {

            }

            @Override
            public void onRequest(CoapServerChannel channel, CoapRequest request) {

                mlogger.info("Request received!");
                mlogger.info("Message type: " + request.getPacketType());

                if (request.getUriPath().equals("/oic/res")) {
                    mDiscoveryRequestList.add(request);
                    handleDiscoveryMessage(channel, request, true);
                    return;
                }

                // Commented out for server only, QoS handled at a lower
                // layer in jcoap
                /*
                 * if(request.getPacketType().equals(CoapPacketType.CON)) {
                 * //Send acknowledegment CoapEmptyMessage ackMsg = new
                 * CoapEmptyMessage(CoapPacketType.ACK,
                 * request.getMessageID());; channel.sendMessage(ackMsg); }
                 */
                // CoapResponse response = new CoapMessage();

                checkRequestWithResource(channel, request);
            }

            @Override
            public CoapServer onAccept(CoapRequest request) {
                return this;
            }
        };
    }

    private void handleGET(CoapRequest request, CoapServerChannel channel) {
        mlogger.info("Handle GET Request");

        String requestUri = request.getUriPath();
        String requestToken = request.getTokenString();
        boolean hasObserver = mOicResourceMap.get(requestUri).hasObserver(
                requestToken);
        boolean isObservable = mOicResourceMap.get(requestUri).isObservable();

        CoapResponse getResponse = channel.createSeparateResponse(request,
                CoapResponseCode.Content_205);
        getResponse.setContentType(CoapMediaType.cbor);
        // Check if there is an observe option
        if (request.getObserveOption() != null && isObservable) {
            // add the observer to the resource
            if (!hasObserver) {
                mlogger.info("Adding Observer for " + requestUri);

                OICObserver oicObserver = new OICObserver();
                oicObserver.setCoapServerChannel(channel);
                oicObserver.setCoapRequest(request);
                mOicResourceMap.get(requestUri).addObserver(requestToken,
                        oicObserver);
            }
        } else if (request.getObserveOption() == null && hasObserver) {
            // Cancel observe Request
            mlogger.info("Removing Observer");
            mOicResourceMap.get(requestUri).removeObserver(requestToken);
        }
        /* block-wise response transfer */
        byte[] payload = mOicResourceMap.get(requestUri)
                .getResourceRepresentation();
        if (payload.length >= mBlockSize.getSize())
            getResponse = blockResponseTransfer(request, getResponse, payload);
        else
            getResponse.setPayload(payload);
        channel.sendSeparateResponse(getResponse);
        mlogger.info("Sent GET Response");
    }

    private void handlePUT(CoapRequest request, CoapServerChannel channel) {
        mlogger.info("Handle PUT Request");
        String payload = CborManager.getPayloadAsJsonString(request
                .getPayload());
        // String substr = payload.substring(3, payload.length() - 1);
        String substr = payload.substring(1, payload.length() - 1);
        JsonNode jsonNodeTree = null;
        ObjectMapper mapper = new ObjectMapper();
        String uri = request.getUriPath();
        try {
            jsonNodeTree = mapper.readTree(substr);
            if (jsonNodeTree.get("href") != null
                    && (!jsonNodeTree.get("href").toString().equals("\"\""))) {
                uri = jsonNodeTree.get("href").toString().replace("\"", "");
                createResource(jsonNodeTree);
            } else
                mOicResourceMap.get(uri).updateCompleteResourceRespresentation(
                        payload);
        } catch (IOException e) {
            e.printStackTrace();
        }

        CoapResponse putResponse = channel.createSeparateResponse(request,
                CoapResponseCode.Changed_204);
        putResponse.setContentType(CoapMediaType.cbor);
        byte[] responsePayload = mOicResourceMap.get(uri)
                .getResourceRepresentation();
        /* Check block-wise response transfer */
        if (responsePayload.length >= mBlockSize.getSize())
            putResponse = blockResponseTransfer(request, putResponse,
                    responsePayload);
        else
            putResponse.setPayload(responsePayload);
        channel.sendSeparateResponse(putResponse);
    }

    private void handlePOST(CoapRequest request, CoapServerChannel channel) {
        mlogger.info("Handle POST Request");
        String payload = CborManager.getPayloadAsJsonString(request
                .getPayload());
        CoapResponse postResponse = channel.createSeparateResponse(request,
                CoapResponseCode.Changed_204);
        postResponse.setContentType(CoapMediaType.cbor);
        JsonNode jsonNodeTree;
        // String substr = payload.substring(3, payload.length() - 1);
        String substr = payload.substring(1, payload.length() - 1);
        String newResUri = request.getUriPath();
        ObjectMapper mapper = new ObjectMapper();
        try {
            jsonNodeTree = mapper.readTree(substr);
            mlogger.info("Handle POST Request: before create:"
                    + jsonNodeTree.get("href").toString() + "finish");
            if (jsonNodeTree.get("href") != null
                    && (!jsonNodeTree.get("href").toString().equals("\"\""))) {
                newResUri += jsonNodeTree.get("href").toString()
                        .replace("\"", "");
                createSubordinateResource(jsonNodeTree, newResUri);
            } else
                mOicResourceMap.get(newResUri).updateResourceRespresentation(
                        payload);
        } catch (IOException e) {
            e.printStackTrace();
        }

        byte[] responsePayload = mOicResourceMap.get(newResUri)
                .getResourceRepresentation();
        /* Check block-wise response transfer */
        if (responsePayload.length >= mBlockSize.getSize())
            postResponse = blockResponseTransfer(request, postResponse,
                    responsePayload);
        else
            postResponse.setPayload(responsePayload);
        channel.sendSeparateResponse(postResponse);
    }

    private void handleDELETE(CoapRequest request, CoapServerChannel channel) {
        mlogger.info("Handle DELETE");
        CoapResponse deleteResponse = channel.createSeparateResponse(request,
                CoapResponseCode.Deleted_202);

        mlogger.info("Deleting Resource: " + request.getUriPath());
        mOicResourceMap.remove(request.getUriPath());

        deleteResponse.setPayload(request.getUriPath() + " Resource Deleted");
        channel.sendSeparateResponse(deleteResponse);

    }

    private byte[] discoverJson(ArrayList<OICCoapResource> mOicResource) {
        JSONArray jsonArray = new JSONArray();
        JSONObject toPayload = new JSONObject();
        for (OICCoapResource res : mOicResource) {
            mlogger.info("Res Uri: " + res.getResourceUri());
            mlogger.info("Res Uri: " + res.getResourceType());
            CTLogger.getInstance().info(
                    "Cumulative Array : " + jsonArray.toJSONString());
            jsonArray.add(jSONFromResource(res));
            toPayload = jSONFromResource(res);
        }

        CTLogger.getInstance().info("Inside discover Json without HAshmap : ");
        // String jsonString = jsonArray.toJSONString().replace("\\", "");
        String jsonString = jsonArray.toJSONString().replace("\\", "");
        mlogger.info("Discovery response payload: " + jsonString);
        return CborManager.convertToCbor(jsonString,
                OCPayloadType.PAYLOAD_TYPE_DISCOVERY);
    }

    private JSONObject jSONFromResource(OICCoapResource resource) {
        JSONObject jsonObjectProp = new JSONObject();
        JSONObject jsonObjectOC = new JSONObject();
        JSONArray jsonObjectLINK = new JSONArray();
        JSONObject jsonObjectRES = new JSONObject();
        JSONObject jsonObjectBM = new JSONObject();
        // bm is added
        jsonObjectBM.put("bm", 3);
        jsonObjectRES.put("rt", resource.getResourceType());
        jsonObjectRES.put("p", jsonObjectBM);
        jsonObjectRES.put("if", resource.getResourceInterfaces());
        jsonObjectRES.put("href", resource.getResourceUri());
        jsonObjectLINK.add(jsonObjectRES);

        // jsonObjectOC.put("prop", jsonObjectProp);
        jsonObjectOC.put("di", "059952a1-7f2f-4ef8-b47b-0b3dc834732b");
        jsonObjectOC.put("links", jsonObjectLINK);
        return jsonObjectOC;
    }

    private void createResource(JsonNode jsonNodeTree) {
        mlogger.info("Create Resource");
        String uri;
        String resourceType;
        uri = jsonNodeTree.get("href").toString().replace("\"", "");
        boolean observable = true;

        JsonNode propJsonNode = jsonNodeTree.get("prop");
        resourceType = propJsonNode.get("rt").toString()
                .replaceAll("[\\[\"\\]]", "");
        // resourceInterface = getAsList(propJsonNode.get("if").toString());

        OICCoapResource resource = new OICCoapResource(uri, resourceType,
                "oic.if.baseline", observable);

        JsonNode repJsonNode = propJsonNode.get("rep");
        Iterator<String> it = repJsonNode.fieldNames();
        while (it.hasNext()) {
            String subField = it.next();
            resource.addResourceAttribute(subField, repJsonNode.get(subField));
        }
        addResource(resource);
    }

    private void createSubordinateResource(JsonNode jsonNodeTree,
            String newResUri) {
        mlogger.info("Create Subordinate Resource");
        String uri = newResUri;
        String resourceType;
        boolean observable = true;

        JsonNode propJsonNode = jsonNodeTree.get("prop");
        resourceType = propJsonNode.get("rt").toString()
                .replaceAll("[\\[\"\\]]", "");

        OICCoapResource subRes = new OICCoapResource(uri, resourceType,
                "oic.if.baseline", observable);

        JsonNode repJsonNode = propJsonNode.get("rep");
        Iterator<String> it = repJsonNode.fieldNames();
        while (it.hasNext()) {
            String subField = it.next();
            subRes.addResourceAttribute(subField, repJsonNode.get(subField));
        }
        addResource(subRes);
    }

    private ArrayList<String> getAsList(String string) {
        String[] list = string.replaceAll("[\\[\"\\]]", "").split(",");
        ArrayList<String> ifList = new ArrayList<String>(Arrays.asList(list));
        return ifList;
    }

    private CoapResponse blockResponseTransfer(CoapRequest request,
            CoapResponse response, byte[] payload) {
        mlogger.info("Response Payload size: " + payload.length + " exceeds "
                + mBlockSize.getSize()
                + ". Initiate block-wise response transfer");
        // getting the block number to send; default is 0;
        if (request.getBlock2() != null)
            mBlockNumber = request.getBlock2().getNumber();
        // getting client requested block size
        if (request.getBlock2() != null)
            mBlockSize = request.getBlock2().getBlockSize();
        boolean more = true;
        int blkLength = mBlockSize.getSize();
        int offset = blkLength * mBlockNumber;

        if ((payload.length - offset) <= mBlockSize.getSize()) {
            blkLength = payload.length - offset;
            more = false;
        }
        byte[] payloadBlock = Arrays.copyOfRange(payload, offset, blkLength
                + offset);
        CoapBlockOption block2option = new CoapBlockOption(mBlockNumber, more,
                mBlockSize);
        response.setBlock2(block2option);
        response.setPayload(payloadBlock);
        if (!more) {
            mBlockSize = mDefaultBlockSize;
            mBlockNumber = 0;
        }
        mlogger.info("response code: " + response.getResponseCode());
        mlogger.info("response payload value: " + response.getPayloadString());
        mlogger.info("response payload sent: " + (blkLength + offset) + " of "
                + payload.length);
        return response;
    }

    private CoapRequest blockRequestTransfer(CoapRequest request,
            CoapServerChannel channel) {
        try {
            CoapBlockOption receiveBlk = request.getBlock1();
            CoapBlockSize receiveBlkSize = receiveBlk.getBlockSize();
            mlogger.info("received block num " + receiveBlk.getNumber()
                    + " of size " + receiveBlk.getBlockSize());
            mlogger.debug("Payload in Request: " + request.getPayloadString());

            payloadBody.write(request.getPayload());
            CoapResponseCode responseCode = CoapResponseCode.Continue_231;

            if (mBlockSize.getSize() < receiveBlkSize.getSize()) {
                responseCode = CoapResponseCode.Request_Entity_To_Large_413;
                mlogger.info("receiveBlkSize: " + receiveBlkSize.getSize()
                        + " is larger than " + mBlockSize.getSize());
            } else
                mBlockSize = receiveBlkSize;

            CoapBlockOption block1 = null;
            if (receiveBlk.isLast()) {
                responseCode = CoapResponseCode.Changed_204;
                block1 = new CoapBlockOption(receiveBlk.getNumber(), false,
                        mBlockSize);
                request.setPayload(payloadBody.toByteArray());
                mBlockSize = mDefaultBlockSize;
            } else
                block1 = new CoapBlockOption(receiveBlk.getNumber(), true,
                        mBlockSize);

            CoapResponse putBlockResponse = channel.createSeparateResponse(
                    request, responseCode);
            putBlockResponse.setContentType(CoapMediaType.cbor);
            putBlockResponse.setBlock1(block1);
            channel.sendSeparateResponse(putBlockResponse);

        } catch (IOException e) {
            e.printStackTrace();
        }
        mlogger.info("PayloadBody so far: "
                + new String(payloadBody.toByteArray()));
        return request;
    }
}

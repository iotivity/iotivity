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

import oic.ctt.formatter.CborManager;
import oic.ctt.formatter.OCPayloadType;
import oic.ctt.logger.CTLogger;
import oic.ctt.network.OICHelper;
import oic.ctt.network.OICProtocolClient;
import oic.ctt.network.OICRequestData;
import oic.ctt.network.OICResponseData;
import oic.ctt.network.OICHelper.MessageParameters;
import oic.ctt.network.OICHelper.MessageType;
import oic.ctt.network.OICRequestData.Method;

import org.slf4j.Logger;
import org.ws4d.coap.connection.BasicCoapChannelManager;
import org.ws4d.coap.connection.ChannelKey;
import org.ws4d.coap.interfaces.CoapChannelManager;
import org.ws4d.coap.interfaces.CoapClient;
import org.ws4d.coap.interfaces.CoapClientChannel;
import org.ws4d.coap.interfaces.CoapRequest;
import org.ws4d.coap.interfaces.CoapResponse;
import org.ws4d.coap.messages.CoapEmptyMessage;
import org.ws4d.coap.messages.CoapMediaType;
import org.ws4d.coap.messages.CoapPacketType;
import org.ws4d.coap.messages.CoapRequestCode;

import java.net.Inet4Address;
import java.net.Inet6Address;
import java.net.InetAddress;
import java.net.StandardProtocolFamily;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;

public class OICCoapClient implements OICProtocolClient, CoapClient {

    private Logger                                 mlogger           = CTLogger
                                                                             .getInstance();

    private ArrayList<OICResponseData>             mResponses        = new ArrayList<OICResponseData>();
    private ArrayList<OICResponseData>             mNotifications    = new ArrayList<OICResponseData>();
    private byte[]                                 mObserveToken     = null;
    private int                                    mWaitTime         = 2;
    private boolean                                mObserveResetFlag = false;
    private boolean                                mObserveNoAckFlag = false;

    private HashMap<ChannelKey, CoapClientChannel> mClientChannels   = new HashMap<ChannelKey, CoapClientChannel>();
    private CoapChannelManager                     mChannelManager   = null;

    public OICCoapClient() {
        this.mChannelManager = BasicCoapChannelManager.getInstance();
    }

    /**
     * Sets the duration the client will wait to receive the responses
     * 
     * @param waitTime
     *            Waiting time in seconds
     */
    @Override
    public void setWaitTime(int waitTime) {
        this.mWaitTime = waitTime;
    }

    /**
     * Gets the matching response for a request token
     * 
     * @param token
     *            The token that designates the exchange
     * @return OICResponseData type object or null
     */
    public OICResponseData getResponse(String token) {
        if (mResponses.size() > 0) {
            for (OICResponseData oicResponse : mResponses) {
                if (oicResponse.getResponseValue(MessageParameters.token)
                        .equals(token))
                    return oicResponse;
            }
        }

        return null;
    }

    /**
     * Gets all the received responses stored in the local response buffer
     * 
     * @return An ArrayList of OICResponse objects
     */
    @Override
    public ArrayList<OICResponseData> getResponses() {

        return mResponses;
    }

    /**
     * Clears the received responses in the local response buffer
     */
    @Override
    public void clearResponses() {
        mResponses.clear();
    }

    /**
     * Gets all the received notifications stored in the local notification
     * buffer
     * 
     * @return An ArrayList of OICResponse objects
     */
    @Override
    public ArrayList<OICResponseData> getNotifications() {
        return mNotifications;
    }

    /**
     * Clears the received notifications in the local notification buffer
     */
    @Override
    public void clearNotifications() {
        mNotifications.clear();
    }

    private CoapClientChannel acquireChannel(String ip, int port) {
        InetAddress remoteAddress = null;
        try {
            remoteAddress = InetAddress.getByName(ip);
        } catch (UnknownHostException e) {
            e.printStackTrace();
        }

        StandardProtocolFamily ipProtocol = null;
        if (remoteAddress instanceof Inet4Address)
            ipProtocol = StandardProtocolFamily.INET;
        else if (remoteAddress instanceof Inet6Address)
            ipProtocol = StandardProtocolFamily.INET6;

        CoapClientChannel clientChannel = null;
        ChannelKey ckey = new ChannelKey(remoteAddress, port);
        if (mClientChannels.containsKey(ckey))
            clientChannel = mClientChannels.get(ckey);
        else {
            clientChannel = mChannelManager.connect(this, remoteAddress, port,
                    ipProtocol, 0);
            mClientChannels.put(ckey, clientChannel);
        }

        return clientChannel;
    }

    private void sendDiscoveryMessage(String uriPath, String query, String ip,
            int port, byte[] token) {

        CoapClientChannel clientChannel = acquireChannel(ip, port);
        if (clientChannel == null) {
            mlogger.error("Client channel connection failed");
            return;
        }

        CoapRequest coapRequest = clientChannel.createRequest(false,
                CoapRequestCode.GET);
        coapRequest.setUriPath(uriPath);
        coapRequest.setUriQuery(query);
        coapRequest.setToken(token);
        clientChannel.sendMessage(coapRequest);
    }

    /**
     * Sends a CoAP based unicast/multicast discovery request message
     * 
     * @param uriPath
     *            URI of the discovery resource object
     * @param query
     *            Optional query to specify filters such as resource type,
     *            interface etc.
     * @param ip
     *            Multicast group address or the server IP address to whom the
     *            message will be sent
     * @param port
     *            Server Port number to whom the message will be sent
     * @param token
     *            Token used to match the request/response transactions
     * @return A list containing the responses to the discovery message (if any)
     */
    @Override
    public ArrayList<OICResponseData> discoverResource(String uriPath,
            String query, String ip, int port, byte[] token) {

        // Validate mandatory arguments
        if (uriPath == null || uriPath.equals(""))
            uriPath = OICHelper.getDefaultUri();

        if (ip == null || ip.equals(""))
            ip = OICHelper.DEFAULT_MULTICAST_IPv6;

        if (port == 0)
            port = OICHelper.DEFAULT_COAP_PORT;

        // If ip is unicast, proceed as usual.
        // Else if ip is multicast (either ip4 or ip6)

        InetAddress targetAddress = null;
        InetAddress ip6Multicast = null;
        InetAddress ip4Multicast = null;

        try {
            targetAddress = InetAddress.getByName(ip);
            ip6Multicast = InetAddress
                    .getByName(OICHelper.DEFAULT_MULTICAST_IPv6);
            ip4Multicast = InetAddress
                    .getByName(OICHelper.DEFAULT_MULTICAST_IPv4);
        } catch (UnknownHostException e1) {
            e1.printStackTrace();
        }

        if (targetAddress.equals(ip4Multicast)
                || targetAddress.equals(ip6Multicast)) {

            sendDiscoveryMessage(uriPath, query,
                    OICHelper.DEFAULT_MULTICAST_IPv6, port, token);
            sendDiscoveryMessage(uriPath, query,
                    OICHelper.DEFAULT_MULTICAST_IPv4, port, token);
        } else
            sendDiscoveryMessage(uriPath, query, ip, port, token);

        try {
            Thread.sleep(1000 * mWaitTime);
        } catch (InterruptedException ex) {
            Thread.currentThread().interrupt();
        }

        mlogger.info("Wait time over, returning advertisement responses");

        return this.getResponses();
    }

    /**
     * Sends a CoAP based unicast request message
     * 
     * @param type
     *            The type of the transmission QoS (NON or CON)
     * @param method
     *            Request method code (GET, PUT, POST or DELETE)
     * @param ip
     *            IP address of the recipient
     * @param port
     *            Port of the recipient
     * @param uriPath
     *            Resource URI of the target resource
     * @param query
     *            Query string to mention different filters
     * @param payload
     *            Message payload. For example, JSON representation of the
     *            resource
     * @param token
     *            Token used to match the response with request
     * @return The response from the server if any. Returns null otherwise.
     */
    @Override
    public OICResponseData sendRequest(MessageType type, Method method,
            int messageId, byte[] token, String ip, int port, String uriPath,
            String query, String payload, OCPayloadType payloadType) {
        boolean reliable = false;
        CoapRequestCode reqCode = null;

        if (type != null && type == MessageType.CON)
            reliable = true;

        if (method == null) {
            mlogger.error("Invalid Method");
            return null;
        }

        switch (method) {
            case GET:
                reqCode = CoapRequestCode.GET;
                break;
            case PUT:
                reqCode = CoapRequestCode.PUT;
                break;
            case POST:
                reqCode = CoapRequestCode.POST;
                break;
            case DELETE:
                reqCode = CoapRequestCode.DELETE;
        }

        if (ip == null || ip.equals("")) {
            mlogger.error("IP address empty/null");
            return null;
        }
        // validates IPv4 only
        // if (!OICHelper.validateIpAddress(ip)) {
        // mlogger.error("Invalid IP address");
        // return null;
        // }

        if (port == 0) {
            mlogger.error("Invalid port");
            return null;
        }

        if (uriPath == null || uriPath.equals("")) {
            mlogger.error("Invalid uri");
            return null;
        }

        CoapClientChannel clientChannel = acquireChannel(ip, port);

        if (clientChannel == null) {
            mlogger.error("Client channel connection failed");
            return null;
        }

        CoapRequest coapRequest = clientChannel
                .createRequest(reliable, reqCode);
        coapRequest.setUriPath(uriPath);
        coapRequest.setMessageID(messageId);
        coapRequest.setToken(token);
        coapRequest.setUriQuery(query);

        coapRequest.addAccept(CoapMediaType.cbor);

        if (method == Method.PUT || method == Method.POST) {
            coapRequest.setContentType(CoapMediaType.cbor);
        }

        if (mObserveToken != null && Arrays.equals(token, mObserveToken))
            coapRequest.setObserveOption(OICHelper
                    .hexStringToByteArray("00000001"));

        if (payload == null)
            payload = "";

        if (payloadType == null)
            payloadType = OCPayloadType.PAYLOAD_TYPE_REPRESENTATION;

        if (payload.length() > 0) {
            coapRequest.setPayload(CborManager.convertToCbor(payload,
                    payloadType));
        }

        this.clearResponses();

        clientChannel.sendMessage(coapRequest);

        try {
            Thread.sleep(1000 * mWaitTime);
        } catch (InterruptedException ex) {
            Thread.currentThread().interrupt();
        }

        mlogger.info("Wait time over, returning response for unicast request");

        return this.getResponse(OICHelper.bytesToHex(token));
    }

    /**
     * Sends a CoAP message to a resource server to subscribe/register as an
     * observer
     * 
     * @param type
     *            The type of the transmission QoS (NON or CON)
     * @param ip
     *            IP address of the recipient
     * @param port
     *            Port of the recipient
     * @param uriPath
     *            Resource URI of the target resource
     * @param query
     *            Query string to mention different filters
     * @param token
     *            Token used to match the response with request
     */
    @Override
    public void observeResource(MessageType type, int messageId, byte[] token,
            String ip, int port, String uriPath, String query) {

        resetObserveFlags();

        mObserveToken = token;
        boolean reliable = false;
        CoapRequestCode reqCode = CoapRequestCode.GET;

        if (type != null && type == MessageType.CON)
            reliable = true;

        if (ip == null || ip.equals("")) {
            mlogger.error("IP address empty/null");
            return;
        }

        // if (!OICHelper.validateIpAddress(ip)) {
        // mlogger.error("Invalid IP address");
        // return;
        // }

        if (port == 0) {
            mlogger.error("Invalid port");
            return;
        }

        if (uriPath == null || uriPath.equals("")) {
            mlogger.error("Invalid uri");
            return;
        }

        CoapClientChannel clientChannel = acquireChannel(ip, port);

        if (clientChannel == null) {
            mlogger.error("Client channel connection failed");
            return;
        }

        CoapRequest coapRequest = clientChannel
                .createRequest(reliable, reqCode);
        coapRequest.setUriPath(uriPath);
        coapRequest.addAccept(CoapMediaType.cbor);
        coapRequest
                .setObserveOption(OICHelper.hexStringToByteArray("00000000"));
        coapRequest.setMessageID(messageId);
        coapRequest.setToken(token);
        if (query.length() > 0)
            coapRequest.setUriQuery(query);

        this.clearResponses();

        clientChannel.sendMessage(coapRequest);

    }

    private void resetObserveFlags() {
        mObserveResetFlag = false;
        mObserveNoAckFlag = false;
    }

    /**
     * Cancels CoAP observe in a passive manner (by not sending any ACK for
     * Confirmable type Notifications)
     */
    @Override
    public void cancelObservePassively() {
        // Stop sending acknowledgments
        mObserveToken = null;
        mObserveNoAckFlag = true;
        mlogger.info("Cancel observe passively (No ACK)");
    }

    /**
     * Cancels CoAP observe in an active manner (by sending RST message in
     * response to Notifications)
     */
    @Override
    public void cancelObserveWithReset() {
        // Send a RST in response to notification
        mObserveToken = null;
        mObserveResetFlag = true;
        mlogger.info("Cancel observe with reset (RST)");
    }

    /**
     * Cancels observe with a GET message
     * 
     * @param type
     *            The type of the transmission QoS (NON or CON)
     * @param ip
     *            IP address of the recipient
     * @param port
     *            Port of the recipient
     * @param uriPath
     *            Resource URI of the target resource
     * @param query
     *            Query string to mention different filters
     * @return The response from the server if any. Returns null otherwise.
     */
    @Override
    public OICResponseData cancelObserveWithGetMessage(MessageType type,
            int messageId, String ip, int port, String uriPath, String query) {
        return this.sendRequest(type, OICRequestData.Method.GET, messageId,
                mObserveToken, ip, port, uriPath, query, "",
                OCPayloadType.PAYLOAD_TYPE_REPRESENTATION);
    }

    /**
     * Asynchronous response callback handler for the CoAP Client.
     */
    @Override
    public void onResponse(CoapClientChannel channel, CoapResponse response) {
        mlogger.info("onResponse callback");

        OICResponseData oicResponse = OICResponseData
                .convertResponseCoap(response);
        if (oicResponse != null) {
            if (response.getObserveOption() != null) {
                CoapPacketType notificationResponseType = CoapPacketType.ACK;

                if (mObserveResetFlag)
                    notificationResponseType = CoapPacketType.RST;

                if (response.getPacketType().equals(CoapPacketType.CON)
                        && (!mObserveNoAckFlag || mObserveResetFlag)) {
                    CoapEmptyMessage notificationResponse = new CoapEmptyMessage(
                            notificationResponseType, response.getMessageID());
                    channel.sendMessage(notificationResponse);
                }

                mlogger.info("Notification received");
                if (mObserveToken != null
                        && Arrays.equals(mObserveToken, response.getToken())) {
                    mNotifications.add(oicResponse);
                    mlogger.info("Notification validated & added");
                } else {
                    mlogger.warn("Unexpected notifications!");
                }
            } else {
                if (response.getPacketType().equals(CoapPacketType.CON))
                    channel.sendMessage(new CoapEmptyMessage(
                            CoapPacketType.ACK, response.getMessageID()));

                mlogger.info("Response received");
                mResponses.add(oicResponse);
            }
        }
    }

    /**
     * Called when a connection is not reachable or reset by the server.
     */
    @Override
    public void onConnectionFailed(CoapClientChannel channel,
            boolean notReachable, boolean resetByServer) {

    }
}

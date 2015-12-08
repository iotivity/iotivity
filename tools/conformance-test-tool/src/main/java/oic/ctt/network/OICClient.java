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

package oic.ctt.network;

import java.util.ArrayList;
import java.util.HashMap;

import org.slf4j.Logger;

import oic.ctt.formatter.OCPayloadType;
import oic.ctt.logger.CTLogger;
import oic.ctt.network.OICHelper.MessageParameters;
import oic.ctt.network.coap.OICCoapClient;
import oic.ctt.network.http.OICHttpClient;

/**
 * The OIC Client class. Provides methods to communicate with a OIC service.
 * Also provides abstraction over HTTP & CoAP.
 */
public class OICClient {

	private OICCoapClient mCoapClient = new OICCoapClient();
	private OICHttpClient mHttpClient = new OICHttpClient();
	private HashMap<Protocol, OICProtocolClient> mClientMap = new HashMap<Protocol, OICProtocolClient>();
	private HashMap<String, String> mObserverTokenMap = new HashMap<String, String>();
	private Logger mlogger = CTLogger.getInstance();

	public OICClient() {
		mClientMap.put(Protocol.COAP, mCoapClient);
		mClientMap.put(Protocol.HTTP, mHttpClient);
	}

	/**
	 * Defines the communication protocol. The protocol can be either CoAP or
	 * HTTP
	 */
	public static enum Protocol {
		HTTP, COAP;
	}

	/**
	 * Sets the duration the client will wait to receive the responses
	 * 
	 * @param waitTime
	 *            Waiting time in seconds
	 */
	public void setWaitTime(int waitTime) {
		mCoapClient.setWaitTime(waitTime);
	}

	/**
	 * Sends out a multicast discovery message to discover resources in the
	 * network
	 * 
	 * @param protocol
	 *            The protocol which will be used here (HTTP or CoAP)
	 * @return A list of responses received from available the resource servers
	 */
	public ArrayList<OICResponseData> discoverResource(OICClient.Protocol protocol) {
		return discoverResource(protocol, OICHelper.getDefaultUri());
	}

	/**
	 * Sends out a multicast discovery message to discover resources in the
	 * network
	 * 
	 * @param protocol
	 *            The protocol which will be used here (HTTP or CoAP)
	 * @param uriPath
	 *            Discovery resource URI to be used in the message. For example,
	 *            oic/res
	 * @return A list of responses received from available the resource servers
	 */
	public ArrayList<OICResponseData> discoverResource(OICClient.Protocol protocol, String uriPath) {
		return discoverResource(protocol, uriPath, OICHelper.DEFAULT_QUERY);
	}

	/**
	 * Sends out a multicast discovery message to discover resources in the
	 * network
	 * 
	 * @param protocol
	 *            The protocol which will be used here (HTTP or CoAP)
	 * @param uriPath
	 *            Discovery resource URI to be used in the message. For example,
	 *            oic/res
	 * @param query
	 *            Query string to mention different filters
	 * @return A list of responses received from available the resource servers
	 */
	public ArrayList<OICResponseData> discoverResource(OICClient.Protocol protocol, String uriPath, String query) {
		return discoverResource(protocol, uriPath, query, OICHelper.DEFAULT_MULTICAST_IPv6,
				OICHelper.DEFAULT_COAP_PORT); // IPv6
												// as
												// Default
	}

	/**
	 * Sends out a discovery message to the specified IP & port to discover
	 * resources
	 * 
	 * @param protocol
	 *            The protocol which will be used here (HTTP or CoAP)
	 * @param uriPath
	 *            Discovery resource URI to be used in the message. For example,
	 *            oic/res
	 * @param query
	 *            Query string to mention different filters
	 * @param ip
	 *            IP address of the recipient
	 * @param port
	 *            Port of the recipient
	 * @return A list of response received from the resource server
	 */
	public ArrayList<OICResponseData> discoverResource(OICClient.Protocol protocol, String uriPath, String query,
			String ip, int port) {
		return discoverResource(protocol, uriPath, query, ip, port,
				OICHelper.createToken(OICHelper.DEFAULT_TOKEN_LENGTH * 2));
	}

	/**
	 * Sends out a discovery message to the specified IP & port to discover
	 * resources
	 * 
	 * @param protocol
	 *            The protocol which will be used here (HTTP or CoAP)
	 * @param uriPath
	 *            Discovery resource URI to be used in the message. For example,
	 *            oic/res
	 * @param query
	 *            Query string to mention different filters
	 * @param ip
	 *            IP address of the recipient
	 * @param port
	 *            Port of the recipient
	 * @param token
	 *            Token used to match the discovery request/response
	 * @return A list of response received from the resource server
	 */
	public ArrayList<OICResponseData> discoverResource(OICClient.Protocol protocol, String uriPath, String query,
			String ip, int port, byte[] token) {

		if (protocol == null)
			protocol = OICClient.Protocol.COAP;

		ArrayList<OICResponseData> responseList = mClientMap.get(protocol).discoverResource(uriPath, query, ip, port,
				token);

		return responseList;
	}

	/**
	 * Sends out a unicast request message and returns the received response
	 * 
	 * @param protocol
	 *            The protocol which will be used here (HTTP or CoAP)
	 * @param type
	 *            The type of the transmission QoS (NON or CON)
	 * @param method
	 *            Request method code (GET, PUT, POST or DELETE)
	 * @param ip
	 *            IP address of the recipient
	 * @param port
	 *            Port of the recipient
	 * @return The response from the server if any. Returns null otherwise.
	 */
	public OICResponseData sendRequest(OICClient.Protocol protocol, OICHelper.MessageType type,
			OICRequestData.Method method, String messageId, String token, String ip, int port) {
		return sendRequest(protocol, type, method, messageId, token, ip, port, OICHelper.getDefaultUri());
	}

	/**
	 * Sends out a unicast request message and returns the received response
	 * 
	 * @param protocol
	 *            The protocol which will be used here (HTTP or CoAP)
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
	 * @return The response from the server if any. Returns null otherwise.
	 */
	public OICResponseData sendRequest(OICClient.Protocol protocol, OICHelper.MessageType type,
			OICRequestData.Method method, String messageId, String token, String ip, int port, String uriPath) {
		return sendRequest(protocol, type, method, messageId, token, ip, port, uriPath, OICHelper.DEFAULT_QUERY);
	}

	/**
	 * Sends out a unicast request message and returns the received response
	 * 
	 * @param protocol
	 *            The protocol which will be used here (HTTP or CoAP)
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
	 * @return The response from the server if any. Returns null otherwise.
	 */
	public OICResponseData sendRequest(OICClient.Protocol protocol, OICHelper.MessageType type,
			OICRequestData.Method method, String messageId, String token, String ip, int port, String uriPath,
			String query) {
		return sendRequest(protocol, type, method, messageId, token, ip, port, uriPath, query,
				OICHelper.DEFAULT_PAYLOAD);
	}

	/**
	 * Sends out a unicast request message and returns the received response
	 * 
	 * @param protocol
	 *            The protocol which will be used here (HTTP or CoAP)
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
	 * @return The response from the server if any. Returns null otherwise.
	 */
	public OICResponseData sendRequest(OICClient.Protocol protocol, OICHelper.MessageType type,
			OICRequestData.Method method, String messageId, String token, String ip, int port, String uriPath,
			String query, String payload) {
		return sendRequest(protocol, type, method, messageId, token, ip, port, uriPath, query, payload,
				OCPayloadType.PAYLOAD_TYPE_REPRESENTATION);
	}

	/**
	 * Sends out a unicast request message and returns the received response
	 * 
	 * @param protocol
	 *            The protocol which will be used here (HTTP or CoAP)
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
	 * @param payloadType
	 *            One of the OCPayloadType values to specify payload type
	 * @return The response from the server if any. Returns null otherwise.
	 */
	public OICResponseData sendRequest(OICClient.Protocol protocol, OICHelper.MessageType type,
			OICRequestData.Method method, String messageId, String token, String ip, int port, String uriPath,
			String query, String payload, OCPayloadType payloadType) {
		OICResponseData response = null;

		if (protocol == null)
			protocol = OICClient.Protocol.COAP;

		response = mClientMap.get(protocol).sendRequest(type, method, Integer.parseInt(messageId),
				OICHelper.hexStringToByteArray(token), ip, port, uriPath, query, payload, payloadType);
		// OICHelper.createToken(OICHelper.DEFAULT_TOKEN_LENGTH * 2)

		return response;

	}

	/**
	 * Sends a message to a resource server to subscribe/register as an observer
	 * 
	 * @param protocol
	 *            The protocol which will be used here (HTTP or CoAP)
	 * @param type
	 *            The type of the transmission QoS (NON or CON)
	 * @param ip
	 *            IP address of the recipient
	 * @param port
	 *            Port of the recipient
	 */
	public OICResponseData observeResource(OICClient.Protocol protocol, OICHelper.MessageType type, String messageId,
			String token, String ip, int port) {
		return observeResource(protocol, type, messageId, token, ip, port, OICHelper.getDefaultUri());
	}

	/**
	 * Sends a message to a resource server to subscribe/register as an observer
	 * 
	 * @param protocol
	 *            The protocol which will be used here (HTTP or CoAP)
	 * @param type
	 *            The type of the transmission QoS (NON or CON)
	 * @param ip
	 *            IP address of the recipient
	 * @param port
	 *            Port of the recipient
	 * @param uriPath
	 *            Resource URI of the target resource
	 */
	public OICResponseData observeResource(OICClient.Protocol protocol, OICHelper.MessageType type, String messageId,
			String token, String ip, int port, String uriPath) {
		return observeResource(protocol, type, messageId, token, ip, port, uriPath, OICHelper.DEFAULT_QUERY);
	}

	/**
	 * Sends a message to a resource server to subscribe/register as an observer
	 * 
	 * @param protocol
	 *            The protocol which will be used here (HTTP or CoAP)
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
	 */
	public OICResponseData observeResource(OICClient.Protocol protocol, OICHelper.MessageType type, String messageId,
			String token, String ip, int port, String uriPath, String query) {

		if (protocol == null)
			protocol = OICClient.Protocol.COAP;
		mObserverTokenMap.put(uriPath, token);
		mlogger.info("Token for observing  uri: " + uriPath + " is = " + token);
		mClientMap.get(protocol).clearNotifications();
		return mClientMap.get(protocol).observeResource(type, Integer.parseInt(messageId),
				OICHelper.hexStringToByteArray(token), ip, port, uriPath, query);
	}

	/**
	 * Returns all the received notifications currently in the local buffer
	 * 
	 * @param protocol
	 *            The protocol being used (HTTP or CoAP)
	 * @return An ArrayList of OICResponse type objects
	 */
	public ArrayList<OICResponseData> getNotifications(OICClient.Protocol protocol) {

		String token = "";

		return getNotifications(protocol, token);
	}

	/**
	 * Returns all the received notifications currently in the local buffer
	 * 
	 * @param protocol
	 *            The protocol being used (HTTP or CoAP)
	 * @return An ArrayList of OICResponse type objects
	 */
	public ArrayList<OICResponseData> getNotifications(OICClient.Protocol protocol, String href) {

		ArrayList<OICResponseData> desiredNotifications = new ArrayList<OICResponseData>();
		ArrayList<OICResponseData> allNotifications = new ArrayList<OICResponseData>();
		String token = getObserveTokenFromHref(href);
		if (protocol == null)
			protocol = OICClient.Protocol.COAP;
		allNotifications = mClientMap.get(protocol).getNotifications();

		if ((token.equals("")) || (allNotifications.size() == 0)) {
			return allNotifications;
		}

		for (OICResponseData notification : allNotifications) {
			if (notification.getResponseValue(MessageParameters.token).equals(token)) {
				desiredNotifications.add(notification);
			}
		}

		return desiredNotifications;
	}

	/**
	 * Discards the notifications in the local buffer
	 * 
	 * @param protocol
	 *            The protocol being used (HTTP or CoAP)
	 */
	public void clearNotifications(OICClient.Protocol protocol) {

		if (protocol == null)
			protocol = OICClient.Protocol.COAP;

		mClientMap.get(protocol).clearNotifications();
	}

	/**
	 * Cancels observe in a passive manner (by not sending any ACK for
	 * Confirmable type Notifications)
	 * 
	 * @param protocol
	 *            The protocol being used (HTTP or CoAP)
	 */
	public void cancelObservePassively(OICClient.Protocol protocol, String href) {

		String tokenString = mObserverTokenMap.get(href);
		byte[] token = OICHelper.hexStringToByteArray(tokenString);
		
		if (protocol == null)
			protocol = OICClient.Protocol.COAP;
		mObserverTokenMap.remove(href);
		mClientMap.get(protocol).cancelObservePassively(token);
	}

	/**
	 * Cancels observe in an active manner (by sending RST message in response
	 * to Notifications)
	 * 
	 * @param protocol
	 *            The protocol being used (HTTP or CoAP)
	 */
	public void cancelObserveWithReset(OICClient.Protocol protocol, String href) {

		String tokenString = mObserverTokenMap.get(href);
		byte[] token = OICHelper.hexStringToByteArray(tokenString);
		
			if (protocol == null)
				protocol = OICClient.Protocol.COAP;
		mObserverTokenMap.remove(href);
		mClientMap.get(protocol).cancelObserveWithReset(token);
	}

	/**
	 * Cancels observe using a GET message
	 * 
	 * @param type
	 *            The type of the transmission QoS (NON or CON)
	 * @param ip
	 *            IP address of the recipient
	 * @param port
	 *            Port of the recipient
	 * @param uriPath
	 *            Resource URI of the target resource
	 * @return The response received from the server
	 */
	public OICResponseData cancelObserveWithGetMessage(OICClient.Protocol protocol, OICHelper.MessageType type,
			String messageId, String ip, int port, String uriPath) {
		return cancelObserveWithGetMessage(protocol, type, messageId, ip, port, uriPath, OICHelper.DEFAULT_QUERY);
	}

	/**
	 * Cancels observe using a GET message
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
	 * @return The response received from the server
	 */
	public OICResponseData cancelObserveWithGetMessage(OICClient.Protocol protocol, OICHelper.MessageType type,
			String messageId, String ip, int port, String uriPath, String query) {

		String token = "";
		if (protocol == null)
			protocol = OICClient.Protocol.COAP;
		mClientMap.get(protocol).clearNotifications();
		if (mObserverTokenMap.containsKey(uriPath)) {
			mlogger.info("uri already found as observed. Getting token for observe request.");
			token = mObserverTokenMap.get(uriPath);
			mObserverTokenMap.remove(uriPath);
			mlogger.info("Token to cancel observe = " + token);
			return mClientMap.get(protocol).cancelObserveWithGetMessage(type, Integer.parseInt(messageId),
					OICHelper.hexStringToByteArray(token), ip, port, uriPath, query);
		} else {
			return null;
		}
	}

	private String getObserveTokenFromHref(String href) {
		String token = "";
		if (mObserverTokenMap.containsKey(href)) {
			token = mObserverTokenMap.get(href);
		}

		return token;
	}
}

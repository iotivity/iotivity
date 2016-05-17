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

import oic.ctt.formatter.OCPayloadType;

/**
 * The CTT Client class. Provides methods to communicate with a OIC service.
 * Also provides abstraction over HTTP & CoAP.
 */
public interface CttClient {

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
    void setWaitTime(int waitTime);

    /**
     * Sends out a multicast discovery message to discover resources in the
     * network
     *
     * @param protocol
     *            The protocol which will be used here (HTTP or CoAP)
     * @return A list of responses received from available the resource servers
     */
    ArrayList<OICResponseData> discoverResource(CttClient.Protocol protocol);

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
    ArrayList<OICResponseData> discoverResource(
            CttClient.Protocol protocol, String uriPath);

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
    ArrayList<OICResponseData> discoverResource(
            CttClient.Protocol protocol, String uriPath, String query);

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
    ArrayList<OICResponseData> discoverResource(
            CttClient.Protocol protocol, String uriPath, String query,
            String ip, int port);

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
    ArrayList<OICResponseData> discoverResource(
            CttClient.Protocol protocol, String uriPath, String query,
            String ip, int port, byte[] token);
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
    OICResponseData sendRequest(CttClient.Protocol protocol,
            OICHelper.MessageType type, OICRequestData.Method method,
            String messageId, String token, String ip, int port);
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
    OICResponseData sendRequest(CttClient.Protocol protocol,
            OICHelper.MessageType type, OICRequestData.Method method,
            String messageId, String token, String ip, int port,
            String uriPath);
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
    OICResponseData sendRequest(CttClient.Protocol protocol,
            OICHelper.MessageType type, OICRequestData.Method method,
            String messageId, String token, String ip, int port, String uriPath,
            String query);

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
    OICResponseData sendRequest(CttClient.Protocol protocol,
            OICHelper.MessageType type, OICRequestData.Method method,
            String messageId, String token, String ip, int port, String uriPath,
            String query, String payload);

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
    OICResponseData sendRequest(CttClient.Protocol protocol,
            OICHelper.MessageType type, OICRequestData.Method method,
            String messageId, String token, String ip, int port, String uriPath,
            String query, String payload, OCPayloadType payloadType);
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
    OICResponseData observeResource(CttClient.Protocol protocol,
            OICHelper.MessageType type, String messageId, String token,
            String ip, int port);
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
    OICResponseData observeResource(CttClient.Protocol protocol,
            OICHelper.MessageType type, String messageId, String token,
            String ip, int port, String uriPath);

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
    OICResponseData observeResource(CttClient.Protocol protocol,
            OICHelper.MessageType type, String messageId, String token,
            String ip, int port, String uriPath, String query);
    /**
     * Returns all the received notifications currently in the local buffer
     *
     * @param protocol
     *            The protocol being used (HTTP or CoAP)
     * @return An ArrayList of OICResponse type objects
     */
    ArrayList<OICResponseData> getNotifications(
            CttClient.Protocol protocol);
    /**
     * Returns all the received notifications currently in the local buffer
     *
     * @param protocol
     *            The protocol being used (HTTP or CoAP)
     * @return An ArrayList of OICResponse type objects
     */
    ArrayList<OICResponseData> getNotifications(
            CttClient.Protocol protocol, String href);
    /**
     * Discards the notifications in the local buffer
     *
     * @param protocol
     *            The protocol being used (HTTP or CoAP)
     */
    void clearNotifications(CttClient.Protocol protocol);
    /**
     * Cancels observe in a passive manner (by not sending any ACK for
     * Confirmable type Notifications)
     *
     * @param protocol
     *            The protocol being used (HTTP or CoAP)
     */
    void cancelObservePassively(CttClient.Protocol protocol,
            String href);
    /**
     * Cancels observe in an active manner (by sending RST message in response
     * to Notifications)
     *
     * @param protocol
     *            The protocol being used (HTTP or CoAP)
     */
    void cancelObserveWithReset(CttClient.Protocol protocol,
            String href);
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
    OICResponseData cancelObserveWithGetMessage(
            CttClient.Protocol protocol, OICHelper.MessageType type,
            String messageId, String ip, int port, String uriPath);

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
    OICResponseData cancelObserveWithGetMessage(
            CttClient.Protocol protocol, OICHelper.MessageType type,
            String messageId, String ip, int port, String uriPath,
            String query);
    
    /**
     * Returns the data about last request 
     *
     * @param protocol
     *            The protocol being used (HTTP or CoAP)
     */
    OICRequestData getLastRequest(CttClient.Protocol protocol);

}

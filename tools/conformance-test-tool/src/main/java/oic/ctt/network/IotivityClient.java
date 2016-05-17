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

import java.util.Map;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Arrays;
import java.util.Vector;
import java.util.Iterator;

import org.slf4j.Logger;

import oic.ctt.formatter.JsonAnalyzer;
import oic.ctt.formatter.OCPayloadType;
import oic.ctt.logger.CTLogger;
import oic.ctt.network.CttClient.Protocol;
import oic.ctt.network.OICHelper.MessageParameters;
import oic.ctt.network.coap.IotivityCoapClient;
import oic.ctt.network.http.IotivityHttpClient;
import oic.ctt.network.OICRequestData.Method;

import org.oic.simulator.AttributeValue;
import org.oic.simulator.AttributeValue.ValueType;
import org.oic.simulator.InvalidArgsException;
import org.oic.simulator.SimulatorException;
import org.oic.simulator.SimulatorManager;
import org.oic.simulator.SimulatorResourceModel;
import org.oic.simulator.SimulatorResult;
import org.oic.simulator.AttributeValue.ValueType;
import org.oic.simulator.client.FindResourceListener;
import org.oic.simulator.client.SimulatorRemoteResource;
import org.oic.simulator.client.SimulatorRemoteResource.GetResponseListener;
import org.oic.simulator.client.SimulatorRemoteResource.PutResponseListener;
import org.oic.simulator.client.SimulatorRemoteResource.PostResponseListener;
import org.oic.simulator.client.SimulatorRemoteResource.ObserveNotificationListener;

/**
 * The OIC Client class. Provides methods to communicate with a OIC service.
 * Also provides abstraction over HTTP & CoAP.
 */
public class IotivityClient implements CttClient {

    private static IotivityCoapClient                        mCoapClient       = new IotivityCoapClient();
    private static IotivityHttpClient                        mHttpClient       = new IotivityHttpClient();
    private HashMap<Protocol, IotivityProtocolClient> mClientMap        = new HashMap<Protocol, IotivityProtocolClient>();
    private HashMap<String, String>              mObserverTokenMap = new HashMap<String, String>();
    private Logger                               mlogger           = CTLogger
            .getInstance();

    public IotivityClient() {
        mClientMap.put(Protocol.COAP, mCoapClient);
        mClientMap.put(Protocol.HTTP, mHttpClient);
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
    public ArrayList<OICResponseData> discoverResource(
            CttClient.Protocol protocol) {
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
    public ArrayList<OICResponseData> discoverResource(
            CttClient.Protocol protocol, String uriPath) {
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
    public ArrayList<OICResponseData> discoverResource(
            CttClient.Protocol protocol, String uriPath, String query) {
        return discoverResource(protocol, uriPath, query,
                OICHelper.DEFAULT_MULTICAST_IPv6, OICHelper.DEFAULT_COAP_PORT); // IPv6
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
    public ArrayList<OICResponseData> discoverResource(
            CttClient.Protocol protocol, String uriPath, String query,
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
    public ArrayList<OICResponseData> discoverResource(
            CttClient.Protocol protocol, String uriPath, String query,
            String ip, int port, byte[] token) {

        if (protocol == null)
            protocol = OICClient.Protocol.COAP;

        mClientMap.get(protocol).clearResponses();
        
        System.out.println("discoverResource in");
    	System.out.println("query: " + query);
    	
        if (uriPath == null || uriPath.equals(""))
        {
            uriPath = OICHelper.getDefaultUri();
        }
        
        String resourceType = "";
        
    	if(!query.isEmpty())
    	{
    		int index = query.indexOf("rt=");
    		
    		if(index >= 0)
    		{
    			resourceType = query.substring(index + "rt=".length());
    			System.out.println("resourceType: " + resourceType);
    		}
    	}
       
    	try
    	{
    		ArrayList<OICResponseData> responseList = mClientMap.get(protocol).discoverResource(resourceType);
    		
    		return responseList;
    	}
    	catch(Exception ex)
    	{
    		
    	}

        return null;
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
    public OICResponseData sendRequest(CttClient.Protocol protocol,
            OICHelper.MessageType type, OICRequestData.Method method,
            String messageId, String token, String ip, int port) {
        return sendRequest(protocol, type, method, messageId, token, ip, port,
                OICHelper.getDefaultUri());
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
    public OICResponseData sendRequest(CttClient.Protocol protocol,
            OICHelper.MessageType type, OICRequestData.Method method,
            String messageId, String token, String ip, int port,
            String uriPath) {
        return sendRequest(protocol, type, method, messageId, token, ip, port,
                uriPath, OICHelper.DEFAULT_QUERY);
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
    public OICResponseData sendRequest(CttClient.Protocol protocol,
            OICHelper.MessageType type, OICRequestData.Method method,
            String messageId, String token, String ip, int port, String uriPath,
            String query) {
        return sendRequest(protocol, type, method, messageId, token, ip, port,
                uriPath, query, OICHelper.DEFAULT_PAYLOAD);
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
    public OICResponseData sendRequest(CttClient.Protocol protocol,
            OICHelper.MessageType type, OICRequestData.Method method,
            String messageId, String token, String ip, int port, String uriPath,
            String query, String payload) {
        return sendRequest(protocol, type, method, messageId, token, ip, port,
                uriPath, query, payload,
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
    public OICResponseData sendRequest(CttClient.Protocol protocol,
            OICHelper.MessageType type, OICRequestData.Method method,
            String messageId, String token, String ip, int port, String uriPath,
            String query, String payload, OCPayloadType payloadType) {
    	
        OICResponseData response = null;

        if (protocol == null)
        {
            protocol = OICClient.Protocol.COAP;
        }
        
        if (ip == null || ip.equals("")) 
        {
            mlogger.error("IP address empty/null");
            return null;
        }

        if (port == 0) 
        {
            mlogger.error("Invalid port");
            return null;
        }

        if (uriPath == null || uriPath.equals("")) 
        {
            mlogger.error("Invalid uri");
            return null;
        }
        
        if (method == null) 
        {
            mlogger.error("Invalid Method");
            return null;
        }        
        
        SimulatorRemoteResource remoteResource = mClientMap.get(protocol).getRemoteResource(ip, port, uriPath);    		
		
		if(remoteResource == null)
		{
			System.out.println("Resource not found. Unable to send request");
			return null;
		}
    	
    	Map<String, String> queryParams = getQueryParams(query);
    	
    	String interfaceName = getInterfaceName(query);
    	
    	System.out.println("clearing Unicast Response Data");
    	
    	try
    	{
	        if(method == Method.GET)
	        {
                response = mClientMap.get(protocol).sendGetRequest(remoteResource, interfaceName, queryParams);
	        }
	        else if(method == Method.PUT)
	        {
            	SimulatorResourceModel resourceModel = getResourceModel(payload);
                response = mClientMap.get(protocol).sendPutRequest(remoteResource, interfaceName, queryParams, resourceModel);
	        }
	        else if(method == Method.POST)
	        {
            	SimulatorResourceModel resourceModel = getResourceModel(payload);
                response = mClientMap.get(protocol).sendPostRequest(remoteResource, interfaceName, queryParams, resourceModel);
	        }
	        else
	        {
            	mlogger.error("Unsupported Method");
                return null;
	        }
    	}
    	catch(Exception ex)
    	{
    		mlogger.error("Exception occured in simulator library");
    		return null;
    	}
        
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
    public OICResponseData observeResource(CttClient.Protocol protocol,
            OICHelper.MessageType type, String messageId, String token,
            String ip, int port) {
        
    	return observeResource(protocol, type, messageId, token, ip, port,
                OICHelper.getDefaultUri());
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
    public OICResponseData observeResource(CttClient.Protocol protocol,
            OICHelper.MessageType type, String messageId, String token,
            String ip, int port, String uriPath) {
        
    	return observeResource(protocol, type, messageId, token, ip, port,
                uriPath, OICHelper.DEFAULT_QUERY);
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
    public OICResponseData observeResource(CttClient.Protocol protocol,
            OICHelper.MessageType type, String messageId, String token,
            String ip, int port, String uriPath, String query) {

        if (protocol == null)
        {
            protocol = OICClient.Protocol.COAP;
        }

      if (ip == null || ip.equals("")) 
      {
          mlogger.error("IP address empty/null");
          return null;
      }

      if (port == 0) 
      {
          mlogger.error("Invalid port");
          return null;
      }

      if (uriPath == null || uriPath.equals("")) 
      {
          mlogger.error("Invalid uri");
          return null;
      }
      
      SimulatorRemoteResource remoteResource = mClientMap.get(protocol).getRemoteResource(ip, port, uriPath);    		
		
		if(remoteResource == null)
		{
			System.out.println("Resource not found. Unable to send request");
			return null;
		}
      
		Map<String, String> queryParams = getQueryParams(query);
  	        
        mClientMap.get(protocol).clearNotifications(remoteResource);
        
        try
        {
        	return mClientMap.get(protocol).startObserve(remoteResource, queryParams);
        }
        catch(Exception ex)
    	{
    		mlogger.error("Exception occured in simulator library");
    		return null;
    	}
    }

    /**
     * Returns all the received notifications currently in the local buffer
     *
     * @param protocol
     *            The protocol being used (HTTP or CoAP)
     * @return An ArrayList of OICResponse type objects
     */
    public ArrayList<OICResponseData> getNotifications(
            CttClient.Protocol protocol) {

        return null;
    }

    /**
     * Returns all the received notifications currently in the local buffer
     *
     * @param protocol
     *            The protocol being used (HTTP or CoAP)
     * @return An ArrayList of OICResponse type objects
     */
    public ArrayList<OICResponseData> getNotifications(
            CttClient.Protocol protocol, String href) 
    {
        if (protocol == null)
        {
            protocol = OICClient.Protocol.COAP;
        }
        
        System.out.println("Ic getNotifications in");

        SimulatorRemoteResource remoteResource = mClientMap.get(protocol).getRemoteResource("", 0, href);    		
		
		if(remoteResource == null)
		{
			System.out.println("Resource not found. Unable to send request");
			return null;
		}
		
        return mClientMap.get(protocol).getNotifications(remoteResource);
    }

    /**
     * Discards the notifications in the local buffer
     *
     * @param protocol
     *            The protocol being used (HTTP or CoAP)
     */
    public void clearNotifications(CttClient.Protocol protocol) 
    {
    	
    }

    /**
     * Cancels observe in a passive manner (by not sending any ACK for
     * Confirmable type Notifications)
     *
     * @param protocol
     *            The protocol being used (HTTP or CoAP)
     */
    public void cancelObservePassively(CttClient.Protocol protocol,
            String href) 
    {
    }

    /**
     * Cancels observe in an active manner (by sending RST message in response
     * to Notifications)
     *
     * @param protocol
     *            The protocol being used (HTTP or CoAP)
     */
    public void cancelObserveWithReset(CttClient.Protocol protocol,
            String href) 
    {

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
    public OICResponseData cancelObserveWithGetMessage(
            CttClient.Protocol protocol, OICHelper.MessageType type,
            String messageId, String ip, int port, String uriPath) 
    {
        return cancelObserveWithGetMessage(protocol, type, messageId, ip, port,
                uriPath, OICHelper.DEFAULT_QUERY);
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
    public OICResponseData cancelObserveWithGetMessage(
            CttClient.Protocol protocol, OICHelper.MessageType type,
            String messageId, String ip, int port, String uriPath,
            String query) {

        if (protocol == null)
        {
            protocol = OICClient.Protocol.COAP;
        }
        
        System.out.println("uriPath: " + uriPath);
        
        SimulatorRemoteResource remoteResource = mClientMap.get(protocol).getRemoteResource("", 0, uriPath);    		
		
		if(remoteResource == null)
		{
			System.out.println("Resource not found. Unable to send request");
			return null;
		}
        
        OICResponseData responseData = null;
        
        try
        {
        	responseData =  mClientMap.get(protocol).stopObserve(remoteResource);
        }
        catch(Exception ex)
    	{
    		mlogger.error("Exception occured in simulator library");
    		return null;
    	}
        
        return responseData;
    }

    public OICRequestData getLastRequest(CttClient.Protocol protocol) {
        return mClientMap.get(protocol).getLastRequest();
    }
       	
    private SimulatorResourceModel getResourceModel(String payload)
    {
		SimulatorResourceModel simulatorResourceModel = new SimulatorResourceModel();

		JsonAnalyzer jsonAnalyzer = new JsonAnalyzer(payload); 
		
		ArrayList<String> listKey = jsonAnalyzer.getKeys();
		
		for(String key : listKey)
		{
			System.out.println("key: " + key);
		
			if(key.equals("if") || key.equals("rt"))
			{
				continue;
			}
			
			ArrayList<String> listValue = jsonAnalyzer.getValue(key);
			
			if(listValue == null || listValue.isEmpty())
			{
				System.out.println("no value found");
				continue;
			}
			
			String attributeKey = key;// key.substring(1, key.length()-1);
			String value = listValue.get(0);
			
			if(value.startsWith("\""))
	    	{
				value = value.substring(1);
	    	}
	    	
			if(value.endsWith("\""))
	    	{
	    		value = value.substring(0, value.length()-1);
	    	}
	    
			System.out.println("attributeKey: " + attributeKey + " ; value: " + value);
			AttributeValue attributeValue = new AttributeValue(value);
			
			try
			{
				simulatorResourceModel.set(attributeKey, attributeValue);
			}
			catch(Exception ex)
			{
				return null;
			}
		}
		
		return simulatorResourceModel;
    }
    
    private Map<String, String> getQueryParams(String query)
    {
    	Map<String, String> queryParams = new HashMap<String, String>();
    	    	
    	System.out.println("query: " + query);
    	
    	String[] queries = query.split("&");
    	
    	System.out.println("total query: " + queries.length);
    	
    	for(String singleQuery : queries)
    	{
    		System.out.println("singleQuery: " + singleQuery);
    		
    		int index = singleQuery.indexOf("=");
    		
    		if(index >= 0)
    		{
	    		String key = singleQuery.substring(0, index);
	    		String value = singleQuery.substring(index+1);
	    		
	    		System.out.println("key: " + key);
	    		System.out.println("value: " + value);
	    		
	    		if(key.equals("if") || key.equals("rt"))
	    		{
	    			continue;
	    		}
	    		
	    		queryParams.put(key, value);
    		}
    	}
    	
    	System.out.println("total parameter: " + queryParams.size());
    	
    	return queryParams;
    }
    
    private String getInterfaceName(String query)
    {    	
    	System.out.println("query: " + query);
    	
    	String[] queries = query.split("&");
    	
    	System.out.println("total query: " + queries.length);
    	
    	for(String singleQuery : queries)
    	{
    		System.out.println("singleQuery: " + singleQuery);
    		
    		int index = singleQuery.indexOf("=");
    		
    		if(index >= 0)
    		{
	    		String key = singleQuery.substring(0, index);
	    		String value = singleQuery.substring(index+1);
	    		
	    		System.out.println("key: " + key);
	    		System.out.println("value: " + value);
	    		
	    		if(key.equals("if"))
	    		{
	    			return value;
	    		}
    		}
    	}
    	
    	return null;
    }
}

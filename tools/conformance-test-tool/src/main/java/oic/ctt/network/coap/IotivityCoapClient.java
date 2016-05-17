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

import oic.ctt.formatter.JsonAnalyzer;
import oic.ctt.formatter.CborManager;
import oic.ctt.formatter.OCPayloadType;
import oic.ctt.logger.CTLogger;
import oic.ctt.network.OICHelper;
import oic.ctt.network.IotivityProtocolClient;
import oic.ctt.network.OICRequestData;
import oic.ctt.network.OICResponseData;
import oic.ctt.network.OICHelper.MessageParameters;
import oic.ctt.network.OICHelper.MessageType;
import oic.ctt.network.OICRequestData.Method;

import org.slf4j.Logger;

import org.ws4d.coap.interfaces.CoapRequest;
import org.ws4d.coap.interfaces.CoapResponse;
import org.ws4d.coap.messages.CoapEmptyMessage;
import org.ws4d.coap.messages.CoapMediaType;
import org.ws4d.coap.messages.CoapPacketType;
import org.ws4d.coap.messages.CoapRequestCode;
import org.ws4d.coap.messages.CoapResponseCode;
import org.ws4d.coap.messages.BasicCoapRequest;

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

import java.net.Inet4Address;
import java.net.Inet6Address;
import java.net.InetAddress;
import java.net.StandardProtocolFamily;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Map;
import java.util.TreeMap;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Vector;
import java.util.Iterator;

public class IotivityCoapClient implements IotivityProtocolClient, FindResourceListener {

    private Logger                                 mlogger           = CTLogger
            .getInstance();

    private int                                    mWaitTime         = 60;
    private int                                    mDiscoverTime     = 5;
    private OICRequestData                         mLastRequest      = null;
    private HashMap<SimulatorRemoteResource, UnicastResponseListener> mapObserveListener = new HashMap<>();
    private String sourceIp;
	private int sourcePort;
	private String destinationIp;
	private int destinationPort;	
	
    private ArrayList<SimulatorRemoteResource> listRemoteResource;

    public IotivityCoapClient() {
    	this.listRemoteResource = new ArrayList<>();
    }

    @Override
    public void setWaitTime(int waitTime) {
        this.mWaitTime = waitTime;
    }    
    
    @Override
    public ArrayList<OICResponseData> discoverResource(String resourceType)
    		 throws InvalidArgsException, SimulatorException, InterruptedException
    {

    	System.out.println("discoverResource in");

    	if(resourceType.isEmpty())
    	{
    		SimulatorManager.findResource(this);
    	}
    	else
    	{
    		SimulatorManager.findResource(resourceType, this);
    	}
        
    	int prevSize = 0;
    	
		for (int t = 0; t < mWaitTime; t++) 
		{
			Thread.sleep(1000);
			
			int currentSize = listRemoteResource.size();
			
			if(currentSize > 0 && prevSize == currentSize)
			{
				break;
			}
			
			prevSize = currentSize;
		}
	  
      	ArrayList<OICResponseData> listDiscoveredResponseData = getDiscoveredResponseData();      
      
      	System.out.println("total resource added: " + listDiscoveredResponseData.size());
      	
        return listDiscoveredResponseData;
    }
    
    @Override
	public void onResourceFound(SimulatorRemoteResource simulatorRemoteResource) 
	{
		synchronized (this) 
		{
			System.out.println("[onResourceFound] IN");
			
			listRemoteResource.add(simulatorRemoteResource);
							
			System.out.println("[onResourceFound] OUT");
		}
	}

    private ArrayList<OICResponseData> getDiscoveredResponseData()
    {
    	TreeMap<String, ArrayList<ResourceProperty>> mapHostPayload = new TreeMap<>();
    	HashMap<String, String> mapHostDi = new HashMap<>();
    	ArrayList<OICResponseData> listDiscoveredResponseData = new ArrayList<>();
    	HashMap<String, HashSet<String>> mapHostResources = new HashMap<>();
    	
    	for(SimulatorRemoteResource simulatorRemoteResource : listRemoteResource)
    	{
	    	String resourceHost = simulatorRemoteResource.getHost();
			String resourceUri = simulatorRemoteResource.getURI();
			Vector<String> resourceTypes = simulatorRemoteResource.getResourceTypes();
			Vector<String> resourceInterfaces = simulatorRemoteResource.getResourceInterfaces();
			
			if(mapHostResources.containsKey(resourceHost))
			{
				HashSet<String> setResource = mapHostResources.get(resourceHost);
				if(setResource.contains(resourceUri))
				{
					continue;
				}
				setResource.add(resourceUri);
			}
			else
			{
				HashSet<String> setResource = new HashSet<>();
				setResource.add(resourceUri);
				mapHostResources.put(resourceHost, setResource);
			}
			
			System.out.println(resourceHost);
			System.out.println(resourceUri);
			System.out.println(resourceTypes);
			System.out.println(resourceInterfaces);
			
			ResourceProperty resourceProperty = new ResourceProperty(resourceUri, resourceTypes, resourceInterfaces);
						
			ArrayList<ResourceProperty> listResourceProperty = mapHostPayload.get(resourceHost);
			
			if(listResourceProperty == null)
			{
				listResourceProperty = new ArrayList<ResourceProperty>();
				mapHostPayload.put(resourceHost, listResourceProperty);
				mapHostDi.put(resourceHost, simulatorRemoteResource.getId());
			}
			
			listResourceProperty.add(resourceProperty);
    	}

    	for(String host : mapHostPayload.keySet())
    	{
    		ArrayList<ResourceProperty> listResourceProperty = mapHostPayload.get(host);
    		String di = mapHostDi.get(host);
    		
    		String payload = "[ { ";
    		
    		int slashPosition = di.indexOf('/');
    		
    		di = di.substring(0, slashPosition);
    		
    		payload += "\"di\" : \"" + di + "\", ";
    		
    		payload += "\"links\" : [ ";
    		
    		boolean notFirst = false;
    		
    		for(ResourceProperty resourceProperty : listResourceProperty)
    		{
		    	if(notFirst)
		    	{
		    		payload += ", ";
		    	}
		    	else
		    	{
		    		notFirst = true;
		    	}
		    	
    			payload += "{ ";
		    	
    			payload += "\"href\" : \"" + resourceProperty.getResourceUri() + "\", ";
		    	
    			payload += "\"rt\" : ";
		    	payload += "[ ";
		    	
		    	boolean flag = false;
		    	
		    	Iterator iterator = resourceProperty.getResourceTypes().iterator();
		    	
		    	while(iterator.hasNext())
		    	{
		    		if(flag)
		    		{
		    			payload += ", ";
		    		}
		    		else
		    		{
		    			flag = true;
		    		}
		    		
		    		payload += "\"" + iterator.next() + "\"";
		    	}
		    	
		    	payload += " ], ";
		    	
		    	payload += "\"if\" : ";
		    	payload += "[ ";
		    	
		    	flag = false;
		    	iterator = resourceProperty.getResourceInterfaces().iterator();
		    	
		    	while(iterator.hasNext())
		    	{
		    		if(flag)
		    		{
		    			payload += ", ";
		    		}
		    		else
		    		{
		    			flag = true;
		    		}
		    		
		    		payload += "\"" + iterator.next() + "\"";
		    	}
		    	
		    	payload += " ] ";
		    	
		    	payload += " } ";			    	
    		}
    		
    		payload += " ] ";
	    	payload += " } ]";
	    	
	    	System.out.println(payload);
			
	    	int beginIndex = host.indexOf('[');
			int lastIndex = host.lastIndexOf(']');
			String address = host.substring(beginIndex+1, lastIndex);
			
			lastIndex = host.lastIndexOf(':');
			int port = Integer.valueOf(host.substring(lastIndex + 1));
			
			
			OICResponseData oicResponseData = new OICResponseData();
			oicResponseData.setResponseValue(OICHelper.MessageParameters.responseCode, "2.05");
			oicResponseData.setResponseValue(OICHelper.MessageParameters.srcAddress, address);
			oicResponseData.setResponseValue(OICHelper.MessageParameters.srcPort, port + "");
			oicResponseData.setResponseValue(OICHelper.MessageParameters.payload, payload);
			oicResponseData.setResponseValue(OICHelper.MessageParameters.token, "");
			oicResponseData.setResponseValue(OICHelper.MessageParameters.mId, "0");
			oicResponseData.setResponseValue(OICHelper.MessageParameters.msgType, OICHelper.MessageType.NON.name());
			oicResponseData.setResponseValue(OICHelper.MessageParameters.secPort, "0");
			oicResponseData.setResponseValue(OICHelper.MessageParameters.contentFormat, "CBOR");
	        
			listDiscoveredResponseData.add(oicResponseData);
    	}	        
    	
    	return listDiscoveredResponseData;
    }
    
    public SimulatorRemoteResource getRemoteResource(String ip, int port, String resourceUri)
    {
    	System.out.println("resourceUri: " + resourceUri);
    	System.out.println("listRemoteResource Size: " + listRemoteResource.size());
    	
    	for(SimulatorRemoteResource simulatorRemoteResource : listRemoteResource)
    	{
    		System.out.println("simulatorRemoteResource uri: " + simulatorRemoteResource.getURI());
    		
    		if(simulatorRemoteResource.getURI().contains(resourceUri))
    		{
    			return simulatorRemoteResource;
    		}
    	}
    	
    	System.out.println("resource not found");
    	
    	return null;
    }    
    
    private OICResponseData getResponseData(UnicastResponseListener responseListener)
    		throws InterruptedException
    {
        for (int t = 0; t < mWaitTime; t++) 
        {
            Thread.sleep(1000);
            
            OICResponseData unicastResponseData = responseListener.getResponseData();
            
            if(unicastResponseData != null)            
            {
            	return unicastResponseData;
            }
        }
                
        System.out.println("Wait time over, returning response for unicast request");
        
        return null;
    }
    
    @Override
    public OICResponseData sendGetRequest(SimulatorRemoteResource remoteResource,
    		String interfaceName, Map<String, String> queryParams)
    			throws InvalidArgsException, SimulatorException, InterruptedException
    {
		UnicastResponseListener responseListener = new UnicastResponseListener(remoteResource); 
		
		if(interfaceName == null || interfaceName.isEmpty())
		{	    				
			remoteResource.get(queryParams, responseListener);
		}
		else
		{
			remoteResource.get(interfaceName, queryParams, responseListener);
		}

        return getResponseData(responseListener);
    }
        
    @Override
    public OICResponseData sendPutRequest(SimulatorRemoteResource remoteResource,
    		String interfaceName, Map<String, String> queryParams, SimulatorResourceModel resourceModel)
    			throws InvalidArgsException, SimulatorException, InterruptedException
    {
    	UnicastResponseListener responseListener = new UnicastResponseListener(remoteResource); 
		
		if(interfaceName == null || interfaceName.isEmpty())
		{	    				
			remoteResource.put(queryParams, resourceModel, responseListener);
		}
		else
		{
			remoteResource.put(interfaceName, queryParams, resourceModel, responseListener);
		}
  
		return getResponseData(responseListener);
    }
    
    @Override
    public OICResponseData sendPostRequest(SimulatorRemoteResource remoteResource,
    		String interfaceName, Map<String, String> queryParams, SimulatorResourceModel resourceModel)
    			throws InvalidArgsException, SimulatorException, InterruptedException
    {
    	UnicastResponseListener responseListener = new UnicastResponseListener(remoteResource); 
		
		if(interfaceName == null || interfaceName.isEmpty())
		{	    				
			remoteResource.post(queryParams, resourceModel, responseListener);
		}
		else
		{
			remoteResource.post(interfaceName, queryParams, resourceModel, responseListener);
		}
  
		return getResponseData(responseListener);
    }

    @Override
    public OICResponseData startObserve(SimulatorRemoteResource remoteResource, Map<String, String> queryParams) 
    		throws InvalidArgsException, SimulatorException, InterruptedException
    {
    	System.out.println("startObserve in");
    	UnicastResponseListener observeListener = new UnicastResponseListener(remoteResource, new ArrayList<SimulatorResourceModel>());
    	
    	mapObserveListener.put(remoteResource, observeListener);
    	
    	observeListener.setObserveStatus(true);
    	
    	remoteResource.observe(queryParams, observeListener);
        
        ArrayList<OICResponseData> listNotifications = null;
        
        for (int t = 0; t < mWaitTime; t++) 
        {
        	Thread.sleep(1000);
            
            listNotifications = observeListener.getNotifications();
            
            if(!listNotifications.isEmpty())
            {
            	break;
            }
        }     
        
        if(listNotifications == null || listNotifications.isEmpty())
        {
        	System.out.println("No listNotifictions");
        	return null;
        }
        
        System.out.println("listNotifications size: " + listNotifications.size());
        
        return listNotifications.get(0);
    }

    @Override
    public ArrayList<OICResponseData> getNotifications(SimulatorRemoteResource remoteResource) 
    {
    	System.out.println("Icc getNotifications in");
    	ArrayList<OICResponseData> listNotifications = mapObserveListener.get(remoteResource).getNotifications();
    	System.out.println("listNotifications size: " + listNotifications.size());
    	System.out.println("Icc getNotifications out");
        return listNotifications;
    }

    @Override
    public void clearNotifications(SimulatorRemoteResource remoteResource) 
    {
    	System.out.println("clearNotifications in");
    	
    	UnicastResponseListener observeListener = mapObserveListener.get(remoteResource);
    	
    	if(observeListener != null)
    	{
    		ArrayList<OICResponseData> listNotifications = observeListener.getNotifications();
        	System.out.println("listNotifications size: " + listNotifications.size());
    		observeListener.clearNotifications();
    		listNotifications = observeListener.getNotifications();
    		System.out.println("listNotifications size: " + listNotifications.size());
    	}
    	
    	System.out.println("clearNotifications out");
    }
    
    @Override
    public OICResponseData stopObserve(SimulatorRemoteResource remoteResource) 
    		throws InvalidArgsException, SimulatorException, InterruptedException
    {
    	System.out.println("stopObserve in");
    	
    	UnicastResponseListener responseListener = mapObserveListener.get(remoteResource);
    	
    	if(responseListener == null)
    	{
    		System.out.println("no observation found for resourceUri " + remoteResource.getURI());
    		return null;
    	}
    	
    	responseListener.clearNotifications();
    	
    	remoteResource.stopObserve();
        
        for (int t = 0; t < mWaitTime; t++) 
        {
        	Thread.sleep(1000);
            
        	if(responseListener.getTotalNotification() > 0)
        	{
        		break;
        	}
        }     
        
        ArrayList<OICResponseData> listNotifications = responseListener.getNotifications();
        
        if(listNotifications == null || listNotifications.isEmpty())
        {
        	return null;
        }
        
        OICResponseData responseData = listNotifications.get(0);
        
        responseListener.setObserveStatus(false);
        
        responseListener.clearNotifications();
        
        System.out.println("stopObserve out");
        
        return responseData;
    }


    @Override
    public void setLastRequest(MessageType type, Method method,
            int messageId, byte[] token, String ip, int port, String uriPath,
            String query, String payload, OCPayloadType payloadType)
    {
    	CoapRequestCode reqCode = null;
    	
        switch (method) 
        {
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

        CoapRequest coapRequest = new BasicCoapRequest(CoapPacketType.NON, reqCode, 0);
        
        coapRequest.setUriPath(uriPath);
        coapRequest.setMessageID(messageId);
        coapRequest.setToken(token);
        coapRequest.setUriQuery(query);
        coapRequest.addAccept(CoapMediaType.cbor);
        
        if (method == Method.PUT || method == Method.POST) 
        {        	
            coapRequest.setContentType(CoapMediaType.cbor);
        }
                        
        if (payload == null)
        {
            payload = "";
        }

        if (payloadType == null)
        {
            payloadType = OCPayloadType.PAYLOAD_TYPE_REPRESENTATION;
        }
        
        if (payload.length() > 0) 
        {
            coapRequest.setPayload(CborManager.convertToCbor(payload, payloadType));
        }
        
        mLastRequest = OICRequestData.convertRequestCoap(coapRequest);
        mLastRequest.setRequestValue(MessageParameters.destAddress, ip);
        mLastRequest.setRequestValue(MessageParameters.destPort, port+"");
    }
            
    @Override
    public OICRequestData getLastRequest() {
    	return mLastRequest;
    }
    
    public OICResponseData getResponse(String token) 
    {
        return null;
    }

    @Override
    public ArrayList<OICResponseData> getResponses() 
    {
        return null;
    }

    @Override
    public void clearResponses() 
    {
        
    }
    
    private class ResourceProperty
    {
    	private String resourceUri;
    	private Vector<String> resourceTypes;
    	private Vector<String> resourceInterfaces;
    	
    	public ResourceProperty(String resourceUri)
    	{
    		this.resourceUri = resourceUri;
    	}
    			
    	public ResourceProperty(String resourceUri, Vector<String> resourceTypes, Vector<String> resourceInterfaces)
    	{
    		this.resourceUri = resourceUri;
    		this.resourceTypes = resourceTypes;
    		this.resourceInterfaces = resourceInterfaces;
    	}
    	
    	public void setResourceUri(String resourceUri)
    	{
    		this.resourceUri = resourceUri;
    	}    	
    	
    	public void setResourceTypes(Vector<String> resourceTypes)
    	{
    		this.resourceTypes = resourceTypes;
    	}    	
    	
    	public void setResourceInterfaces(Vector<String> resourceInterfaces)
    	{
    		this.resourceInterfaces = resourceInterfaces;
    	}    	
    	
    	public String getResourceUri()
    	{
    		return resourceUri;
    	}    	
    	
    	public Vector<String> getResourceTypes()
    	{
    		return resourceTypes;
    	}    	
    	
    	public Vector<String> getResourceInterfaces()
    	{
    		return resourceInterfaces;
    	}    	
    }
    
    private class UnicastResponseListener implements GetResponseListener, 
    	PutResponseListener, PostResponseListener, ObserveNotificationListener
    {    	
    	private String srcIp;
    	private int srcPort;
    	boolean observeStatus;
    	
    	private OICResponseData unicastResponseData;
    	private SimulatorRemoteResource mSimulatorRemoteResource;
    	private ArrayList<SimulatorResourceModel> listNotificationResourceModel;
    	
    	public UnicastResponseListener(SimulatorRemoteResource remoteResource)
    	{ 
    		this(remoteResource, null);
    	}
    	
    	public UnicastResponseListener(SimulatorRemoteResource remoteResource, ArrayList<SimulatorResourceModel> listResourceModel)
    	{ 
    		mSimulatorRemoteResource = remoteResource;
    		this.listNotificationResourceModel = listResourceModel;
    		
    		String host = remoteResource.getHost();
    		
    		System.out.println("host: " + host);
    		
    		int index = host.lastIndexOf(':');
    		
    		if(index > 0)
    		{
				this.srcPort = Integer.valueOf(host.substring(index + 1));
				index = host.indexOf('[');
				if(index >= 0)
				{
					System.out.println("IPv6 Address");
					
					int lastIndex = host.indexOf(']');
					this.srcIp = host.substring(index+1, lastIndex);
				}
				else
				{
					System.out.println("IPv4 Address");
					index = host.indexOf("//");
					if(index >= 0)
					{
						int lastIndex = host.lastIndexOf(':');
						this.srcIp = host.substring(index+2, lastIndex);
					}
					else
					{
						System.out.println("No double slash found in address");
					}
				}
    		}
    	}
    	
	    @Override
	    public void onGetResponse(String uid, SimulatorResult result, SimulatorResourceModel resourceModel)
	    {
	    	String responseCode = "2.05";
	    	
	    	if(result != SimulatorResult.SIMULATOR_OK)
	    	{
	    		responseCode = "4.00";
	    	}
	    	
	    	setRequestedResponseData(uid, resourceModel, CoapRequestCode.GET, responseCode);
	    }
	    
	    @Override
	    public void onPutResponse(String uid, SimulatorResult result, SimulatorResourceModel resourceModel)
	    {
	    	String responseCode = "2.04";
	    	
	    	if(result != SimulatorResult.SIMULATOR_OK)
	    	{
	    		responseCode = "4.00";
	    	}
	    	
	    	setRequestedResponseData(uid, resourceModel, CoapRequestCode.PUT, responseCode);
	    }
	    
	    @Override
	    public void onPostResponse(String uid, SimulatorResult result, SimulatorResourceModel resourceModel)
	    {
	    	String responseCode = "2.04";
	    	
	    	if(result != SimulatorResult.SIMULATOR_OK)
	    	{
	    		responseCode = "4.00";
	    	}
	    	
	    	setRequestedResponseData(uid, resourceModel, CoapRequestCode.POST, responseCode);
	    }
	    
	    @Override
	    public void onObserveNotification(String uid, SimulatorResourceModel resourceModel, int sequenceNumber)
	    {
	    	synchronized (this) 
	    	{	    		
	    		
		    	System.out.println("onObserveNotification in");
		    	System.out.println("uid: " + uid);
		    	
		    	System.out.println("observeStatus: " + observeStatus);
		    	
		    	if(!observeStatus)
	    		{
	    			return;
	    		}
		    	
		    	listNotificationResourceModel.add(resourceModel);
		    	
		    	System.out.println("onObserveNotification out");
	    	}
	    }
	    
	    private String getResponsePayload(SimulatorResourceModel resourceModel)
	    {
	    	Vector<String> resourceTypes = mSimulatorRemoteResource.getResourceTypes();
			Vector<String> resourceInterfaces = mSimulatorRemoteResource.getResourceInterfaces();
			
	    	String payload = "{ ";
	    	
			payload += "\"rt\" : ";
	    	payload += "[ ";
	    	
	    	boolean flag = false;
	    	
	    	Iterator iterator = mSimulatorRemoteResource.getResourceTypes().iterator();
	    	
	    	while(iterator.hasNext())
	    	{
	    		if(flag)
	    		{
	    			payload += ", ";
	    		}
	    		else
	    		{
	    			flag = true;
	    		}
	    		
	    		payload += "\"" + iterator.next() + "\"";
	    	}
	    	
	    	payload += " ], ";
	    	
	    	payload += "\"if\" : ";
	    	payload += "[ ";
	    	
	    	flag = false;
	    	iterator = mSimulatorRemoteResource.getResourceInterfaces().iterator();
	    	
	    	while(iterator.hasNext())
	    	{
	    		if(flag)
	    		{
	    			payload += ", ";
	    		}
	    		else
	    		{
	    			flag = true;
	    		}
	    		
	    		payload += "\"" + iterator.next() + "\"";
	    	}
	    	
	    	payload += " ] ";

    	    System.out.println("resource mode size: " + resourceModel.get().keySet().size());

	        for(String attributeName : resourceModel.get().keySet())
			{
	        	payload += " , ";
	        	payload += "\"" + attributeName + "\"";
	        	payload += ":";
	        	
	        	Object value = resourceModel.get(attributeName).get();
	        	ValueType valueType = resourceModel.get(attributeName).typeInfo().mType;
				
	        	if(valueType == ValueType.STRING)
				{
	        		payload += "\"" + value + "\"";
				}
				else if(valueType == ValueType.INTEGER || valueType == ValueType.DOUBLE || valueType == ValueType.BOOLEAN)
				{
					payload += value;
				}
	        	
				System.out.println(attributeName + " : " + value);
			}
		        
	    	payload += " } ";			    	
    	
	        
	        return payload;
	    }
	    
	    private void setResponseData(String payload, String responseCode, String token, OICResponseData unicastResponseData)
	    {	    	
	    	unicastResponseData.setResponseValue(OICHelper.MessageParameters.srcAddress, srcIp);
	        unicastResponseData.setResponseValue(OICHelper.MessageParameters.srcPort, srcPort + "");
	        unicastResponseData.setResponseValue(OICHelper.MessageParameters.payload, payload);
	        unicastResponseData.setResponseValue(OICHelper.MessageParameters.token, token);
	        unicastResponseData.setResponseValue(OICHelper.MessageParameters.mId, "0");
	        unicastResponseData.setResponseValue(OICHelper.MessageParameters.msgType, OICHelper.MessageType.NON.name());
	        unicastResponseData.setResponseValue(OICHelper.MessageParameters.secPort, "0");
	        unicastResponseData.setResponseValue(OICHelper.MessageParameters.contentFormat, "CBOR");
	        unicastResponseData.setResponseValue(OICHelper.MessageParameters.responseCode, responseCode);
	    }
	    
	    private void setRequestedResponseData(String uid, SimulatorResourceModel resourceModel, CoapRequestCode requestCode, String responseCode)
	    {
	        System.out.println("setResponse in");	        	        	     
			
	        unicastResponseData = new OICResponseData();	      
	        
	        String payload = getResponsePayload(resourceModel);
	        
	        setResponseData(payload, responseCode, "", unicastResponseData);
	        	        
	        System.out.println("setResponse out");
	    }
	    
	    public OICResponseData getResponseData()
	    {
	    	return unicastResponseData; 
	    }
	    
	    public int getTotalNotification()
	    {
	    	return listNotificationResourceModel.size();
	    }
	    
	    public ArrayList<OICResponseData> getNotifications()
	    {
	    	System.out.println("getNotifications in");
	    	
	    	System.out.println("listNotificationResourceModel : " + listNotificationResourceModel.size());
	    	
	    	ArrayList<OICResponseData> listResponseData = new ArrayList<>();
	    	
	    	for(SimulatorResourceModel resourceModel : listNotificationResourceModel)
	    	{
	    		OICResponseData responseData = new OICResponseData();
	    		
	    		String payload = "";
	    		
	    		if(!resourceModel.get().isEmpty())
	    		{
	    			payload = getResponsePayload(resourceModel);
	    		}
	    		
	    		System.out.println("payload: " + payload);
	    		
	    		setResponseData(payload, "2.05", "", responseData);
	    		
	    		listResponseData.add(responseData);
	    	}
	    	
	    	System.out.println("getNotifications out");;
	    	
	    	return listResponseData;
	    }
	    
	    public void clearNotifications()
	    {
	    	if(listNotificationResourceModel != null)
	    	{
	    		listNotificationResourceModel.clear();
	    	}
	    }
	    
	    public void setObserveStatus(boolean observeStatus)
	    {
	    	this.observeStatus = observeStatus;
	    }
    }   
}

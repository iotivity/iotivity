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

package oic.ctt.network;

import java.util.ArrayList;
import java.util.Map;

import oic.ctt.network.OICHelper;
import oic.ctt.network.OICHelper.MessageParameters;
import oic.ctt.network.OICHelper.MessageType;
import oic.ctt.network.OICRequestData.Method;

import oic.ctt.formatter.OCPayloadType;

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
 * Iotivity Client interface. Defines which methods must be implemented in a client
 * irrespective of protocol.
 */
public interface IotivityProtocolClient {

	void setWaitTime(int waitTime);
	
	ArrayList<OICResponseData> discoverResource(String resourceType)
   		 throws InvalidArgsException, SimulatorException, InterruptedException;
       
   SimulatorRemoteResource getRemoteResource(String ip, int port, String resourceUri);    
   
   ArrayList<OICResponseData> getNotifications(SimulatorRemoteResource remoteResource);
   
   OICResponseData sendGetRequest(SimulatorRemoteResource remoteResource,
   		String interfaceName, Map<String, String> queryParams)
   			throws InvalidArgsException, SimulatorException, InterruptedException;
       
   OICResponseData sendPutRequest(SimulatorRemoteResource remoteResource,
   		String interfaceName, Map<String, String> queryParams, SimulatorResourceModel resourceModel)
   			throws InvalidArgsException, SimulatorException, InterruptedException;
   
   OICResponseData sendPostRequest(SimulatorRemoteResource remoteResource,
   		String interfaceName, Map<String, String> queryParams, SimulatorResourceModel resourceModel)
   			throws InvalidArgsException, SimulatorException, InterruptedException;
       
   OICResponseData startObserve(SimulatorRemoteResource remoteResource, Map<String, String> queryParams) 
   		throws InvalidArgsException, SimulatorException, InterruptedException;
   
   void clearNotifications(SimulatorRemoteResource remoteResource); 
   
   OICResponseData stopObserve(SimulatorRemoteResource remoteResource) 
   		throws InvalidArgsException, SimulatorException, InterruptedException;
   
   void setLastRequest(MessageType type, Method method,
           int messageId, byte[] token, String ip, int port, String uriPath,
           String query, String payload, OCPayloadType payloadType);
   
   OICRequestData getLastRequest();   

   OICResponseData getResponse(String token);
   
   ArrayList<OICResponseData> getResponses(); 
   
   void clearResponses();
}

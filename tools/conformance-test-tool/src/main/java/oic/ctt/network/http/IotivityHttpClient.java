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

package oic.ctt.network.http;

import java.util.Map;
import java.util.ArrayList;

import oic.ctt.formatter.OCPayloadType;
import oic.ctt.network.OICHelper;
import oic.ctt.network.IotivityProtocolClient;
import oic.ctt.network.OICRequestData;
import oic.ctt.network.OICResponseData;
import oic.ctt.network.OICHelper.MessageType;
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
 * The Iotivity HTTP Client class. Provides methods to communicate with a OIC client
 * using HTTP.
 */
public class IotivityHttpClient implements IotivityProtocolClient {

    // TODO: Implement a HTTP Client here. Provide methods to facilitate HTTP
    // communication.

	public void setWaitTime(int waitTime)
	{
		
	}
	
    @Override
    public ArrayList<OICResponseData> discoverResource(String resourceType)
      		 throws InvalidArgsException, SimulatorException, InterruptedException
      		 {
    	return null;
      		 }
          
    @Override
    public SimulatorRemoteResource getRemoteResource(String ip, int port, String resourceUri)
      {
    	return null;
      }
      
    @Override
    public ArrayList<OICResponseData> getNotifications(SimulatorRemoteResource remoteResource)
      {
    	return null;
      }
      
    @Override
    public OICResponseData sendGetRequest(SimulatorRemoteResource remoteResource,
      		String interfaceName, Map<String, String> queryParams)
      			throws InvalidArgsException, SimulatorException, InterruptedException
      			{
    	return null;
      			}
          
    @Override
    public OICResponseData sendPutRequest(SimulatorRemoteResource remoteResource,
      		String interfaceName, Map<String, String> queryParams, SimulatorResourceModel resourceModel)
      			throws InvalidArgsException, SimulatorException, InterruptedException
      			{
    	return null;
      			}
      
    @Override
    public OICResponseData sendPostRequest(SimulatorRemoteResource remoteResource,
      		String interfaceName, Map<String, String> queryParams, SimulatorResourceModel resourceModel)
      			throws InvalidArgsException, SimulatorException, InterruptedException
      			{
    	return null;
      			}
          
    @Override
    public OICResponseData startObserve(SimulatorRemoteResource remoteResource, Map<String, String> queryParams) 
      		throws InvalidArgsException, SimulatorException, InterruptedException
      		{
    	return null;
      		}
      
    @Override
    public void clearNotifications(SimulatorRemoteResource remoteResource)
      {
    	  
      }
      
    @Override
    public OICResponseData stopObserve(SimulatorRemoteResource remoteResource) 
      		throws InvalidArgsException, SimulatorException, InterruptedException
      		{
    	return null;
      		}
      
    @Override
    public void setLastRequest(MessageType type, Method method,
              int messageId, byte[] token, String ip, int port, String uriPath,
              String query, String payload, OCPayloadType payloadType)
      {
    	  
      }
      
    @Override
    public OICRequestData getLastRequest()
      {
    	return null;
      }

    @Override
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
}

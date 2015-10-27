/* Copyright [2011] [University of Rostock]
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

package org.ws4d.coap.connection;

import java.net.InetAddress;

import org.ws4d.coap.interfaces.CoapChannel;
import org.ws4d.coap.interfaces.CoapMessage;
import org.ws4d.coap.interfaces.CoapRequest;
import org.ws4d.coap.interfaces.CoapResponse;
import org.ws4d.coap.interfaces.CoapServer;
import org.ws4d.coap.interfaces.CoapServerChannel;
import org.ws4d.coap.interfaces.CoapSocketHandler;
import org.ws4d.coap.messages.BasicCoapRequest;
import org.ws4d.coap.messages.BasicCoapResponse;
import org.ws4d.coap.messages.CoapEmptyMessage;
import org.ws4d.coap.messages.CoapMediaType;
import org.ws4d.coap.messages.CoapPacketType;
import org.ws4d.coap.messages.CoapResponseCode;

/**
 * @author Christian Lerche <christian.lerche@uni-rostock.de>
 */

public class BasicCoapServerChannel extends BasicCoapChannel implements CoapServerChannel{
	CoapServer server = null;
	public BasicCoapServerChannel(CoapSocketHandler socketHandler,
			CoapServer server, InetAddress remoteAddress,
			int remotePort) {
		super(socketHandler, remoteAddress, remotePort);
		this.server = server;
	}
	
    @Override
    public void close() {
        socketHandler.removeServerChannel(this);
    }
	
	
	@Override
	public void handleMessage(CoapMessage message) {
		/* message MUST be a request */
		if (message.isEmpty()){
			return; 
		}
		
		if (!message.isRequest()){
			return;
			//throw new IllegalStateException("Incomming server message is not a request");
		}
		
		BasicCoapRequest request = (BasicCoapRequest) message;
		CoapChannel channel = request.getChannel();
		/* TODO make this cast safe */
		server.onRequest((CoapServerChannel) channel, request);
	}
	
    /*TODO: implement */
	public void lostConnection(boolean notReachable, boolean resetByServer){
		server.onSeparateResponseFailed(this);
	}
	
    @Override
    public BasicCoapResponse createResponse(CoapMessage request, CoapResponseCode responseCode) {
    	return createResponse(request, responseCode, null);
    }  
    
    @Override
    public BasicCoapResponse createResponse(CoapMessage request, CoapResponseCode responseCode, CoapMediaType contentType){
    	BasicCoapResponse response;
    	if (request.getPacketType() == CoapPacketType.CON) {
    		response = new BasicCoapResponse(CoapPacketType.ACK, responseCode, request.getMessageID(), request.getToken());
    	} else if (request.getPacketType() == CoapPacketType.NON) {
    		response = new BasicCoapResponse(CoapPacketType.NON, responseCode, request.getMessageID(), request.getToken());
    	} else {
    		throw new IllegalStateException("Create Response failed, Request is neither a CON nor a NON packet");
    	}
    	if (contentType != null && contentType != CoapMediaType.UNKNOWN){
    		response.setContentType(contentType);
    	}
    	
    	response.setChannel(this);
    	return response;
    }


	@Override
	public CoapResponse createSeparateResponse(CoapRequest request,	CoapResponseCode responseCode) {
		
		BasicCoapResponse response = null;
		if (request.getPacketType() == CoapPacketType.CON) {
			/* The separate Response is CON (normally a Response is ACK or NON) */
    		response = new BasicCoapResponse(CoapPacketType.CON, responseCode, channelManager.getNewMessageID(), request.getToken());
    		/*send ack immediately */
    		sendMessage(new CoapEmptyMessage(CoapPacketType.ACK, request.getMessageID()));
		} else if (request.getPacketType() == CoapPacketType.NON){
			/* Just a normal response*/
			response = new BasicCoapResponse(CoapPacketType.NON, responseCode, request.getMessageID(), request.getToken());
		} else {
    		throw new IllegalStateException("Create Response failed, Request is neither a CON nor a NON packet");
		}

		response.setChannel(this);
		return response;
	}


	@Override
	public void sendSeparateResponse(CoapResponse response) {
		sendMessage(response);
	}

	@Override
	public CoapResponse createNotification(CoapRequest request, CoapResponseCode responseCode, int sequenceNumber){
		/*use the packet type of the request: if con than con otherwise non*/
		if (request.getPacketType() == CoapPacketType.CON){
			return createNotification(request, responseCode, sequenceNumber, true);
		} else {
			return createNotification(request, responseCode, sequenceNumber, false);
		}
	}
	
	@Override
	public CoapResponse createNotification(CoapRequest request, CoapResponseCode responseCode, int sequenceNumber, boolean reliable){
		BasicCoapResponse response = null;
		CoapPacketType packetType;
		if (reliable){
			packetType = CoapPacketType.CON;
		} else {
			packetType = CoapPacketType.NON;
		}
		
		response = new BasicCoapResponse(packetType, responseCode, channelManager.getNewMessageID(), request.getToken());
		response.setChannel(this);
		//response.setObserveOption(sequenceNumber);
		return response;
	}


	@Override
	public void sendNotification(CoapResponse response) {
		sendMessage(response);
	}

}

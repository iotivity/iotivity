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

import java.io.ByteArrayOutputStream;
import java.net.InetAddress;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Iterator;

import org.ws4d.coap.interfaces.CoapClient;
import org.ws4d.coap.interfaces.CoapClientChannel;
import org.ws4d.coap.interfaces.CoapMessage;
import org.ws4d.coap.interfaces.CoapRequest;
import org.ws4d.coap.interfaces.CoapResponse;
import org.ws4d.coap.interfaces.CoapSocketHandler;
import org.ws4d.coap.messages.AbstractCoapMessage.CoapHeaderOptionType;
import org.ws4d.coap.messages.BasicCoapRequest;
import org.ws4d.coap.messages.BasicCoapResponse;
import org.ws4d.coap.messages.CoapBlockOption;
import org.ws4d.coap.messages.CoapBlockOption.CoapBlockSize;
import org.ws4d.coap.messages.CoapEmptyMessage;
import org.ws4d.coap.messages.CoapPacketType;
import org.ws4d.coap.messages.CoapRequestCode;

/**
 * @author Christian Lerche <christian.lerche@uni-rostock.de>
 */

public class BasicCoapClientChannel extends BasicCoapChannel implements CoapClientChannel {
	CoapClient client = null;
	ClientBlockContext blockContext = null;
	CoapRequest lastRequest = null;
	Object trigger = null;
	private CoapBlockSize mDefaultBlockSize = CoapBlockSize.BLOCK_1024;
	private CoapBlockSize mMaxBlockSize = mDefaultBlockSize;
	private int offset = 0;
	private byte[] payloadBody;
	private HashMap<String, Integer> tokenMidmap = new HashMap<>();
	private ArrayList<Integer> receivedACKList = new ArrayList<>();

	public BasicCoapClientChannel(CoapSocketHandler socketHandler,
			CoapClient client, InetAddress remoteAddress,
			int remotePort) {
		super(socketHandler, remoteAddress, remotePort);
		this.client = client;
	}

	@Override
    public void close() {
        socketHandler.removeClientChannel(this);
    }

	@Override
	public void handleMessage(CoapMessage message) {
		if (message.isRequest()){
			/* this is a client channel, no requests allowed */
			message.getChannel().sendMessage(new CoapEmptyMessage(CoapPacketType.RST, message.getMessageID()));
			return;
		}

		if (message.isEmpty() && message.getPacketType() == CoapPacketType.ACK){
			/* this is the ACK of a separate response */
			//TODO: implement a handler or listener, that informs a client when a sep. resp. ack was received
		        if (!receivedACKList.contains(message.getMessageID()))
		            receivedACKList.add(message.getMessageID());
		        else
		            logger.error("Duplicate ACK received"); // should not happen
			return;
		}

		if (message.getPacketType() == CoapPacketType.CON) {
		        logger.info("Separate response received");
		        int mid = tokenMidmap.get(message.getTokenString());
		        if (mid != 0){
		            if (receivedACKList.contains(mid)) {
		                message.setMessageID(mid);
		                message.setPacketType(CoapPacketType.ACK);
		                Iterator<Integer> it = receivedACKList.iterator();
		                while(it.hasNext()){
		                    int tmpMid = it.next();
		                    if (tmpMid == mid){
		                        it.remove();
		                    }
		                }
		            } else {
		                logger.error("No previous ACK received for this separate response. Request Msg ID: " + mid);
		            }
		        } else{
		            logger.error("No request Msg ID found against this separate response token: " + message.getTokenString());
		        }
		}
		/* Block-wise request transfer */
		CoapBlockOption block1 = message.getBlock1();
		if (block1 != null){
			// sending ACK
			if (message.getPacketType().equals(CoapPacketType.CON))
				super.sendMessage(new CoapEmptyMessage(CoapPacketType.ACK, message.getMessageID()));

			if (!block1.isLast()){
				logger.info("Continue sending request blocks");
				maxSendBlocksize = block1.getBlockSize().getSize() < mMaxBlockSize.getSize() ? block1.getBlockSize() : mMaxBlockSize;
				BasicCoapRequest request =  new BasicCoapRequest(lastRequest.getPacketType(), lastRequest.getRequestCode(), channelManager.getNewMessageID());
				request.copyHeaderOptions((BasicCoapRequest) lastRequest);
				request = (BasicCoapRequest) blockRequestTransfer(request);
				request.setToken(message.getToken());
				super.sendMessage(request);
				return;
			}
		}
		/* check for blockwise response transfer */
		CoapBlockOption block2 = message.getBlock2();
		if (blockContext == null && block2 != null){
			logger.info("initiate blockwise response transfer of block size " + block2.getBlockSize());
			maxReceiveBlocksize = mMaxBlockSize;
			blockContext = new ClientBlockContext(block2, maxReceiveBlocksize);
			blockContext.setFirstRequest(lastRequest);
			blockContext.setFirstResponse((CoapResponse) message);
		}

		if (blockContext!= null){
			/*blocking option*/
			if (!blockContext.addBlock(message, block2)){
				/*this was not a correct block*/
			    logger.error("failes to add block");
			}

			if (!blockContext.isFinished()){
				CoapBlockOption newBlock = blockContext.getNextBlock();
				logger.info("Received block num: " + block2.getNumber() + " of size " + message.getPayloadLength());
				if (lastRequest == null){
					/*TODO: this should never happen*/
					logger.error("ERROR: client channel: lastRequest == null");
				} else {
				    if (message.getPacketType().equals(CoapPacketType.CON))
				        sendMessage(new CoapEmptyMessage(CoapPacketType.ACK, message.getMessageID()));
				    /* create a new request for the next block */
				    BasicCoapRequest request =  new BasicCoapRequest(lastRequest.getPacketType(), lastRequest.getRequestCode(), channelManager.getNewMessageID());                    
				    request.copyHeaderOptions((BasicCoapRequest) blockContext.getFirstRequest());
				    request.setBlock2(newBlock);
				    request.setToken(message.getToken()); // Setting same token as previous request
				    request.removeOption(CoapHeaderOptionType.Block1);
				    sendMessage(request);
				    logger.info("Getting next block "+request.getBlock2().getNumber()+" of size " + request.getBlock2().getBlockSize());
				}
				/* TODO: implement handler, inform the client that a block (but not the complete message) was received*/
				return;
			}
			/* blockwise transfer finished */
			logger.info("blockwise receive complete: " + new String(blockContext.getPayload()));
			message.setPayload(blockContext.getPayload());
			message.setMessageID(blockContext.getFirstRequest().getMessageID());
			blockContext = null;
			/* TODO: give the payload separately and leave the original message as they is*/
		}
		/* normal or separate response */
		client.onResponse(this, (BasicCoapResponse) message);
	}

	@Override
	public void lostConnection(boolean notReachable, boolean resetByServer) {
		client.onConnectionFailed(this, notReachable, resetByServer);
	}

    @Override
    public BasicCoapRequest createRequest(boolean reliable, CoapRequestCode requestCode) {
    	BasicCoapRequest msg = new BasicCoapRequest(
                reliable ? CoapPacketType.CON : CoapPacketType.NON, requestCode,
                channelManager.getNewMessageID());
        msg.setChannel(this);
        return msg;
    }

    @Override
    public void sendMessage(CoapMessage msg) {
        /* check to initiate block-wise resquest transfer */
        logger.debug(" Mapped token: " + msg.getTokenString() + " to Mid: " + msg.getMessageID());
        tokenMidmap.put(msg.getTokenString(), msg.getMessageID());
        if (msg.getPayloadLength() >= mMaxBlockSize.getSize()){
        	logger.info("Total msg payload length: " + msg.getPayloadLength() + " exceeds " + mMaxBlockSize.getSize() + ".\nInitiate block transfer");
            payloadBody = msg.getPayload();
            maxSendBlocksize = mMaxBlockSize;
            msg = blockRequestTransfer(msg);
        }
        super.sendMessage(msg);
        //TODO: check
        if (!(msg instanceof CoapEmptyMessage))
            lastRequest = (CoapRequest) msg;
    }

    private class ClientBlockContext{
        ByteArrayOutputStream payload = new ByteArrayOutputStream();
    	boolean finished = false;
    	CoapBlockSize blockSize; //null means no block option
    	CoapRequest request;
    	CoapResponse response;

    	public ClientBlockContext(CoapBlockOption blockOption, CoapBlockSize maxBlocksize) {
    		/* determine the right blocksize (min of remote and max)*/
    		if (maxBlocksize == null){
    			blockSize = blockOption.getBlockSize();
    		} else {
    			int max = maxBlocksize.getSize();
    			int remote = blockOption.getBlockSize().getSize();
    			if (remote < max){
    				blockSize = blockOption.getBlockSize();
    			} else {
    				blockSize = maxBlocksize;
    			}
    		}
    	}

		public byte[] getPayload() {
			return payload.toByteArray();
		}

		public boolean addBlock(CoapMessage msg, CoapBlockOption block){
		    int blockPos = block.getBytePosition();
		    int blockLength =  msg.getPayloadLength();
		    int bufSize = payload.size();

		    /*TODO: check if payload length = blocksize (except for the last block)*/
		    if (blockPos > bufSize){
		        /* data is missing before this block */
		        return false;
		    } else if ((blockPos + blockLength) <= bufSize){
		        /* data already received */
		        return false;
		    }
		    int offset = bufSize - blockPos;
		    payload.write(msg.getPayload(), offset, blockLength - offset);

		    if (block.isLast()){
		        /* was this the last block */
		        finished = true;
		    }
		    return true;
		}

		public CoapBlockOption getNextBlock() {
			int num = payload.size() / blockSize.getSize(); //ignore the rest (no rest should be there)
			return new CoapBlockOption(num, false, blockSize);
		}

		public boolean isFinished() {
			return finished;
		}

		public CoapRequest getFirstRequest() {
			return request;
		}

		public void setFirstRequest(CoapRequest request) {
			this.request = request;
		}

		public CoapResponse getFirstResponse() {
			return response;
		}

		public void setFirstResponse(CoapResponse response) {
			this.response = response;
		}
    }

	@Override
	public void setTrigger(Object o) {
		trigger = o;
	}

	@Override
	public Object getTrigger() {
		return trigger;
	}

	private CoapMessage blockRequestTransfer(CoapMessage request) {
	    boolean more = true;
	    int blkLength = maxSendBlocksize.getSize();
	    int blkNum = offset/blkLength;

	    if ((payloadBody.length - offset) <= maxSendBlocksize.getSize()) { // for last block
	        blkLength = payloadBody.length - offset;
	        more = false;
	    }
	    logger.info("transferring request block num: " + blkNum + " size: " + blkLength);
	    byte[] payloadBlock = Arrays.copyOfRange(payloadBody, offset, blkLength + offset);
	    offset += blkLength;
	    logger.info("payload sent: " + offset + " of " + payloadBody.length);
	    CoapBlockOption block1option = new CoapBlockOption(blkNum, more, maxSendBlocksize);
	    request.setBlock1(block1option);
	    request.setPayload(payloadBlock);
	    if(!more){
	        offset = 0;
	        logger.info("block transfer of request is complete");
	    }
	    return request;
	}
}

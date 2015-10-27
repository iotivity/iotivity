package org.ws4d.coap.messages;

/**
 * @author Christian Lerche <christian.lerche@uni-rostock.de>
 */

public class CoapEmptyMessage extends AbstractCoapMessage {

	public CoapEmptyMessage(byte[] bytes, int length){
		this(bytes, length, 0);
	}
	
	public CoapEmptyMessage(byte[] bytes, int length, int offset){
		serialize(bytes, length, offset);
		/* check if response code is valid, this function throws an error in case of an invalid argument */
		if (this.messageCodeValue != 0){
			throw new IllegalArgumentException("Not an empty CoAP message.");
		}
		
		if (length != HEADER_LENGTH){
			throw new IllegalArgumentException("Invalid length of an empty message");
		}
	}

	public CoapEmptyMessage(CoapPacketType packetType, int messageId) {
		this.version = 1;
		this.packetType = packetType;
		this.messageCodeValue = 0;
		this.messageId = messageId;		
	}

	@Override
	public boolean isRequest() {
		return false;
	}

	@Override
	public boolean isResponse() {
		return false;
	}

	@Override
	public boolean isEmpty() {
		return true;
	}

}

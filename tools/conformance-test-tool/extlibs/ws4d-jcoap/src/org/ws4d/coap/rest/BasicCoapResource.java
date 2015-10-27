package org.ws4d.coap.rest;

import java.util.HashMap;
import java.util.Vector;

import org.apache.log4j.Logger;
import org.ws4d.coap.interfaces.CoapChannel;
import org.ws4d.coap.interfaces.CoapRequest;
import org.ws4d.coap.interfaces.CoapResponse;
import org.ws4d.coap.interfaces.CoapServerChannel;
import org.ws4d.coap.messages.CoapMediaType;
import org.ws4d.coap.messages.CoapResponseCode;

/**
 * @author Christian Lerche <christian.lerche@uni-rostock.de>
 */

public class BasicCoapResource implements CoapResource {
	/* use the logger of the resource server */
	private final static Logger logger = Logger.getLogger(CoapResourceServer.class); 
	
    private CoapMediaType mediaType;
    private String path;
    private byte[] value;
    ResourceHandler resourceHandler = null;
    ResourceServer serverListener = null; //could be a list of listener
    String resourceType = null;
    HashMap<CoapChannel, CoapRequest> observer = new HashMap<CoapChannel, CoapRequest>();
    boolean observable = false;
    int observeSequenceNumber = 0; //MUST NOT greater than 0xFFFF (2 byte integer)
    Boolean reliableNotification = null;
    long expires = -1; //DEFAULT: expires never

    public BasicCoapResource(String path, byte[] value, CoapMediaType mediaType) {
	this.path = path;
	this.value = value;
	this.mediaType = mediaType;
    }

    public void setCoapMediaType(CoapMediaType mediaType) {
	this.mediaType = mediaType;
    }

    @Override
    public CoapMediaType getCoapMediaType() {
	return mediaType;
    }
    
    public String getMimeType(){
    	//TODO: implement
    	return null;
    }

    @Override
    public String getPath() {
	return path;
    }

    @Override
    public String getShortName() {
	return null;
    }

    @Override
    public byte[] getValue() {
	return value;
    }

    @Override
    public byte[] getValue(Vector<String> query) {
	return value;
    }
 
    public void setValue(byte[] value) {
		this.value = value;
	}

	@Override
    public String getResourceType() {
    	return resourceType;
    }
    
    public void setResourceType(String resourceType) {
		this.resourceType = resourceType;
	}

	public Boolean getReliableNotification() {
		return reliableNotification;
	}

	/* NULL lets the client decide */
	public void setReliableNotification(Boolean reliableNotification) {
		this.reliableNotification = reliableNotification;
	}

	@Override
    public String toString() {
		return getPath(); //TODO implement 
	}

	@Override
	public void post(byte[] data) {
		if (resourceHandler != null){
			resourceHandler.onPost(data);
		}
		return;
	}
	
	@Override	
    public void changed() {
		if (serverListener != null){
			serverListener.resourceChanged(this);
		}
		observeSequenceNumber++;
		if (observeSequenceNumber > 0xFFFF){
			observeSequenceNumber = 0;
		}
		
		/* notify all observers */
		for (CoapRequest obsRequest : observer.values()) {
			CoapServerChannel channel = (CoapServerChannel) obsRequest.getChannel();
			CoapResponse response;
			if (reliableNotification == null){
				response = channel.createNotification(obsRequest, CoapResponseCode.Content_205, observeSequenceNumber);
			} else {
				response = channel.createNotification(obsRequest, CoapResponseCode.Content_205, observeSequenceNumber, reliableNotification);
			}
			response.setPayload(getValue());
			channel.sendNotification(response);
		}
	}
	
	public void registerResourceHandler(ResourceHandler handler){
		this.resourceHandler = handler;
	}
	
	public void registerServerListener(ResourceServer server){
		this.serverListener = server;
	}
	
	public void unregisterServerListener(ResourceServer server){
		this.serverListener = null;
	}

	@Override
	public boolean addObserver(CoapRequest request) {
		observer.put(request.getChannel(), request);
		return true;
	}
	
	public void removeObserver(CoapChannel channel){
		observer.remove(channel);
	}
	
	public boolean  isObservable(){
		return observable;
	}

	public void setObservable(boolean observable) {
		this.observable = observable;
	}
	
	public int getObserveSequenceNumber(){
		return observeSequenceNumber;
	}

	@Override
	public long expires() {
		return expires;
	}
	
	@Override
	public boolean isExpired(){
		if (expires == -1){
			return false; //-1 == never expires
		}
		
		if(expires < System.currentTimeMillis()){
			return true;
		} else {
			return false;
		}
	}
	
	public void setExpires(long expires){
		this.expires = expires;
	}


}

package org.ws4d.coap.interfaces;

import org.ws4d.coap.messages.CoapResponseCode;
/**
 * @author Christian Lerche <christian.lerche@uni-rostock.de>
 */

public interface CoapResponse extends CoapMessage{
	
	/* TODO: Response Code is part of BasicCoapResponse */
	public CoapResponseCode getResponseCode();
	public String getResponseCodeString();
	public void setMaxAge(int maxAge);
    public long getMaxAge();
    
    public void setUriPath(String path);
    public void setUriQuery(String query);
    public void setETag(byte[] etag);
    public byte[] getETag();
    public void setResponseCode(CoapResponseCode responseCode);
    public void setToken(byte[] token);
    public String getHeaderOptions();

		
}

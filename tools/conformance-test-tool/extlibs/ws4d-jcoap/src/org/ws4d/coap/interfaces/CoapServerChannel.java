
package org.ws4d.coap.interfaces;
/**
 * @author Christian Lerche <christian.lerche@uni-rostock.de>
 */

import org.ws4d.coap.messages.CoapMediaType;
import org.ws4d.coap.messages.CoapResponseCode;


public interface CoapServerChannel extends CoapChannel {
    
    /* creates a normal response */
    public CoapResponse createResponse(CoapMessage request, CoapResponseCode responseCode);

    /* creates a normal response */
    public CoapResponse createResponse(CoapMessage request, CoapResponseCode responseCode, CoapMediaType contentType);
    
	/* creates a separate response and acks the current request witch an empty ACK in case of a CON.
	 * The separate response can be send later using sendSeparateResponse()  */
	public CoapResponse createSeparateResponse(CoapRequest request,
			CoapResponseCode responseCode);

	/* used by a server to send a separate response */
	public void sendSeparateResponse(CoapResponse response);
	
	
	/* used by a server to create a notification (observing resources),  reliability is base on the request packet type (con or non) */
	public CoapResponse createNotification(CoapRequest request, CoapResponseCode responseCode, int sequenceNumber);
	
	/* used by a server to create a notification (observing resources) */
	public CoapResponse createNotification(CoapRequest request, CoapResponseCode responseCode, int sequenceNumber, boolean reliable);
	
	/* used by a server to send a notification (observing resources) */
	public void sendNotification(CoapResponse response);


}

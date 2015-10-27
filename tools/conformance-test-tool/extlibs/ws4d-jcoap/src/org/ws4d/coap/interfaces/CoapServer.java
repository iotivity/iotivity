
package org.ws4d.coap.interfaces;

/**
 * @author Christian Lerche <christian.lerche@uni-rostock.de>
 */

public interface CoapServer extends CoapChannelListener {
    public CoapServer onAccept(CoapRequest request);
    public void onRequest(CoapServerChannel channel, CoapRequest request);
	public void onSeparateResponseFailed(CoapServerChannel channel);
}

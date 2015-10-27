package org.ws4d.coap.interfaces;

import java.net.InetAddress;
/**
 * @author Christian Lerche <christian.lerche@uni-rostock.de>
 */

public interface CoapSocketHandler {
    // public void registerResponseListener(CoapResponseListener
    // responseListener);
    // public void unregisterResponseListener(CoapResponseListener
    // responseListener);
    // public int sendRequest(CoapMessage request);
    // public void sendResponse(CoapResponse response);
    // public void establish(DatagramSocket socket);
    // public void testConfirmation(int msgID);
    //
    // public boolean isOpen();
    /* TODO */
    public CoapClientChannel connect(CoapClient client, InetAddress remoteAddress, int remotePort);

    public void close();

    public void sendMessage(CoapMessage msg);

    public CoapChannelManager getChannelManager();

	int getLocalPort();

	void removeClientChannel(CoapClientChannel channel);

	void removeServerChannel(CoapServerChannel channel);
}

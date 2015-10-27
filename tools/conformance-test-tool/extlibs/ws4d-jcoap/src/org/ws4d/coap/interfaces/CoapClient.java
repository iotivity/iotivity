
package org.ws4d.coap.interfaces;

/**
 * @author Christian Lerche <christian.lerche@uni-rostock.de>
 */


public interface CoapClient extends CoapChannelListener {
    public void onResponse(CoapClientChannel channel, CoapResponse response);
    public void onConnectionFailed(CoapClientChannel channel, boolean notReachable, boolean resetByServer);
}

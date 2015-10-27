
package org.ws4d.coap.interfaces;

import org.ws4d.coap.messages.CoapRequestCode;

/**
 * @author Christian Lerche <christian.lerche@uni-rostock.de>
 */

public interface CoapClientChannel extends CoapChannel {
    public CoapRequest createRequest(boolean reliable, CoapRequestCode requestCode);
    public void setTrigger(Object o);
    public Object getTrigger();
}

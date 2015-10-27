package org.ws4d.coap.rest;

/**
 * @author Christian Lerche <christian.lerche@uni-rostock.de>
 */

public interface ResourceHandler {
		public void onPost(byte[] data);
}

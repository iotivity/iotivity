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

package org.ws4d.coap.interfaces;
/**
 * @author Christian Lerche <christian.lerche@uni-rostock.de>
 */

import java.net.InetAddress;
import java.net.StandardProtocolFamily;

import org.ws4d.coap.messages.BasicCoapRequest;


public interface CoapChannelManager {
    public int getNewMessageID();

    /* called by the socket Listener to create a new Server Channel
     * the Channel Manager then asked the Server Listener if he wants to accept a new connection */
    public CoapServerChannel createServerChannel(CoapSocketHandler socketHandler, CoapMessage message, InetAddress addr, int port);

	/* creates a server socket listener for incoming connections */
    public void createServerListener(CoapServer serverListener, int localPort, StandardProtocolFamily ipVersion, boolean isMulticast);

    /* called by a client to create a connection
     * TODO: allow client to bind to a special port */
    public CoapClientChannel connect(CoapClient client, InetAddress addr, int port, StandardProtocolFamily ipVersion, int localPort);
    
    /* This function is for testing purposes only, to have a determined message id*/
    public void setMessageId(int globalMessageId);
    
    public void initRandom();
}

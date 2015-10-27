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

package org.ws4d.coap.connection;

import java.io.IOException;
import java.net.InetAddress;
import java.net.StandardProtocolFamily;
import java.util.HashMap;
import java.util.Random;

import org.apache.log4j.ConsoleAppender;
import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.apache.log4j.SimpleLayout;
import org.ws4d.coap.Constants;
import org.ws4d.coap.interfaces.CoapChannelManager;
import org.ws4d.coap.interfaces.CoapClient;
import org.ws4d.coap.interfaces.CoapClientChannel;
import org.ws4d.coap.interfaces.CoapMessage;
import org.ws4d.coap.interfaces.CoapServer;
import org.ws4d.coap.interfaces.CoapServerChannel;
import org.ws4d.coap.interfaces.CoapSocketHandler;
import org.ws4d.coap.messages.BasicCoapRequest;

/**
 * @author Christian Lerche <christian.lerche@uni-rostock.de>
 */

public class BasicCoapChannelManager implements CoapChannelManager {
    // global message id
	private final static Logger logger = Logger.getLogger(BasicCoapChannelManager.class); 
    private int globalMessageId;
    private static BasicCoapChannelManager instance;
    private HashMap<Integer, SocketInformation> socketMapIp4 = new HashMap<Integer, SocketInformation>();
    private HashMap<Integer, SocketInformation> socketMapIp6 = new HashMap<Integer, SocketInformation>();
    CoapServer serverListener = null;

    private BasicCoapChannelManager() {
        logger.addAppender(new ConsoleAppender(new SimpleLayout()));
        // ALL | DEBUG | INFO | WARN | ERROR | FATAL | OFF:
        logger.setLevel(Level.WARN);
    	initRandom();
    }

    public synchronized static CoapChannelManager getInstance() {
        if (instance == null) {
            instance = new BasicCoapChannelManager();
        }
        return instance;
    }
  
    /**
     * Creates a new server channel
     */
    @Override
    public synchronized CoapServerChannel createServerChannel(CoapSocketHandler socketHandler, CoapMessage message, InetAddress addr, int port){
    	SocketInformation socketInfo = null; 
    	
    	socketInfo = socketMapIp4.get(socketHandler.getLocalPort());
    	
    	if(socketInfo == null)
    	    socketInfo = socketMapIp6.get(socketHandler.getLocalPort());

    	if (socketInfo.serverListener == null) {
			/* this is not a server socket */
    		throw new IllegalStateException("Invalid server socket");
		}
    	
    	if (!message.isRequest()){
    		throw new IllegalStateException("Incomming message is not a request message");
    	}    	    	
    	
    	CoapServer server = socketInfo.serverListener.onAccept((BasicCoapRequest) message);
    	if (server == null){
    		/* Server rejected channel */
    		return null;
    	}
    	CoapServerChannel newChannel= new BasicCoapServerChannel( socketHandler, server, addr, port);
    	return newChannel;
    }

    /**
     * Creates a new, global message id for a new COAP message
     */
    @Override
    public synchronized int getNewMessageID() {
        if (globalMessageId < Constants.MESSAGE_ID_MAX) {
            ++globalMessageId;
        } else
            globalMessageId = Constants.MESSAGE_ID_MIN;
        return globalMessageId;
    }

    @Override
    public synchronized void initRandom() {
        // generate random 16 bit messageId
        Random random = new Random();
        globalMessageId = random.nextInt(Constants.MESSAGE_ID_MAX + 1);
    }

   
    @Override
    public void createServerListener(CoapServer serverListener, int localPort, StandardProtocolFamily ipVersion, boolean isMulticast) {
        if(ipVersion == StandardProtocolFamily.INET)
        {
            if (!socketMapIp4.containsKey(localPort)) {
                try {
                	SocketInformation socketInfo = new SocketInformation(new BasicCoapSocketHandler(this, localPort, ipVersion, isMulticast), serverListener);
                	socketMapIp4.put(localPort, socketInfo);
                } catch (IOException e) {
    				e.printStackTrace();
    			}
            } else {
            	/*TODO: raise exception: address already in use */
            	throw new IllegalStateException();
            }
        }
        else
        {
            if (!socketMapIp6.containsKey(localPort)) {
                try {
                        SocketInformation socketInfo = new SocketInformation(new BasicCoapSocketHandler(this, localPort, ipVersion, isMulticast), serverListener);
                        socketMapIp6.put(localPort, socketInfo);
                } catch (IOException e) {
                                e.printStackTrace();
                        }
            } else {
                /*TODO: raise exception: address already in use */
                throw new IllegalStateException();
            }
        }
    }

    @Override
	public CoapClientChannel connect(CoapClient client, InetAddress addr, int port, StandardProtocolFamily ipVersion, int localPort) {
    	CoapSocketHandler socketHandler = null;
		try {
			socketHandler = new BasicCoapSocketHandler(this, localPort, ipVersion);
			SocketInformation sockInfo = new SocketInformation(socketHandler, null); 
			
			if(ipVersion == StandardProtocolFamily.INET)
			    socketMapIp4.put(socketHandler.getLocalPort(), sockInfo);
			else
			    socketMapIp6.put(socketHandler.getLocalPort(), sockInfo);
			
			return socketHandler.connect(client, addr, port);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return null;
	}

	private class SocketInformation {
		public CoapSocketHandler socketHandler = null;
		public CoapServer serverListener = null;
		public SocketInformation(CoapSocketHandler socketHandler,
				CoapServer serverListener) {
			super();
			this.socketHandler = socketHandler;
			this.serverListener = serverListener;
		}
	}

	@Override
	public void setMessageId(int globalMessageId) {
		this.globalMessageId = globalMessageId;
	}
}

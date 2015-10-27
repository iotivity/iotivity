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

import java.net.InetAddress;

import org.apache.log4j.Logger;
import org.ws4d.coap.interfaces.CoapChannel;
import org.ws4d.coap.interfaces.CoapChannelManager;
import org.ws4d.coap.interfaces.CoapMessage;
import org.ws4d.coap.interfaces.CoapSocketHandler;
import org.ws4d.coap.messages.CoapBlockOption.CoapBlockSize;

/**
 * @author Christian Lerche <christian.lerche@uni-rostock.de>
 */

public abstract class BasicCoapChannel implements CoapChannel {
	/* use the logger of the channel manager */
	protected final static Logger logger = Logger.getLogger(BasicCoapChannelManager.class); 
	protected CoapSocketHandler socketHandler = null;
    protected CoapChannelManager channelManager = null;
    protected InetAddress remoteAddress;
    protected int remotePort;
    protected int localPort;
	CoapBlockSize maxReceiveBlocksize = null; //null means no block option
	CoapBlockSize maxSendBlocksize = null; //null means no block option

    public BasicCoapChannel(CoapSocketHandler socketHandler, InetAddress remoteAddress, int remotePort) {
        this.socketHandler = socketHandler;
        channelManager = socketHandler.getChannelManager();
        this.remoteAddress = remoteAddress;
        this.remotePort = remotePort;
        this.localPort = socketHandler.getLocalPort(); //FIXME:can be 0 when socketHandler is not yet ready
    }
    
    public int getLocalPort()
    {
        return this.localPort;
    }

    @Override
    public void sendMessage(CoapMessage msg) {
        msg.setChannel(this);
        socketHandler.sendMessage(msg);
    }
    
	
    @Override
	public CoapBlockSize getMaxReceiveBlocksize() {
		return maxReceiveBlocksize;
	}

    @Override
	public void setMaxReceiveBlocksize(CoapBlockSize maxReceiveBlocksize) {
		this.maxReceiveBlocksize = maxReceiveBlocksize;
	}

    @Override
	public CoapBlockSize getMaxSendBlocksize() {
		return maxSendBlocksize;
	}

    @Override
	public void setMaxSendBlocksize(CoapBlockSize maxSendBlocksize) {
		this.maxSendBlocksize = maxSendBlocksize;
	}

    @Override
    public InetAddress getRemoteAddress() {
        return remoteAddress;
    }

    @Override
    public int getRemotePort() {
        return remotePort;
    }

    /*A channel is identified (and therefore unique) by its remote address, remote port and the local port 
     * TODO: identify channel also by a token */
	@Override
	public int hashCode() {
		final int prime = 31;
		int result = 1;
		result = prime * result + localPort;
		result = prime * result
				+ ((remoteAddress == null) ? 0 : remoteAddress.hashCode());
		result = prime * result + remotePort;
		return result;
	}

	@Override
	public boolean equals(Object obj) {
		if (this == obj)
			return true;
		if (obj == null)
			return false;
		if (getClass() != obj.getClass())
			return false;
		BasicCoapChannel other = (BasicCoapChannel) obj;
		if (localPort != other.localPort)
			return false;
		if (remoteAddress == null) {
			if (other.remoteAddress != null)
				return false;
		} else if (!remoteAddress.equals(other.remoteAddress))
			return false;
		if (remotePort != other.remotePort)
			return false;
		return true;
	}
    
}

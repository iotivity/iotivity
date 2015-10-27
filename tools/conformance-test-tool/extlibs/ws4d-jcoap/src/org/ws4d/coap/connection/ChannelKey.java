package org.ws4d.coap.connection;

import java.net.InetAddress;

public class ChannelKey {
	public InetAddress inetAddr;
	public int port;

	public ChannelKey(InetAddress inetAddr, int port) {
		this.inetAddr = inetAddr;
		this.port = port;
	}

	@Override
	public int hashCode() {
		final int prime = 31;
		int result = 1;
		result = prime * result
				+ ((inetAddr == null) ? 0 : inetAddr.hashCode());
		result = prime * result + port;
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
		ChannelKey other = (ChannelKey) obj;
		if (inetAddr == null) {
			if (other.inetAddr != null)
				return false;
		} else if (!inetAddr.equals(other.inetAddr))
			return false;
		if (port != other.port)
			return false;
		return true;
	}
}

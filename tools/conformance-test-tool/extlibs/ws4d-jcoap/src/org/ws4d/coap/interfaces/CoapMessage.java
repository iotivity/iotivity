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

import org.ws4d.coap.messages.AbstractCoapMessage.CoapHeaderOptionType;
import org.ws4d.coap.messages.CoapBlockOption;
import org.ws4d.coap.messages.CoapMediaType;
import org.ws4d.coap.messages.CoapPacketType;
/**
 * @author Christian Lerche <christian.lerche@uni-rostock.de>
 */

public interface CoapMessage {
    public static final int RESPONSE_TIMEOUT_MS = 2000;
    public static final double RESPONSE_RANDOM_FACTOR = 1.5;
    public static final int MAX_RETRANSMIT = 4;
    /* TODO: what is the right value? */
    public static final int ACK_RST_RETRANS_TIMEOUT_MS = 120000;

    /* returns the value of the internal message code
     * in case of an error this function returns -1 */
    public int getMessageCodeValue();

    public int getMessageID();

    public void setMessageID(int msgID);

    public String getSourceIP();

    public void setSourceIP(String sourceIP);

    public int getSourcePort();

    public void setSourcePort(int sourcePort);

    public String getDestinationIP();

    public void setDestinationIP(String destinationIP);

    public int getDestinationPort();

    public void setDestinationPort(int destinationPort);

    public byte[] serialize();

    public void incRetransCounterAndTimeout();

    public CoapPacketType getPacketType();

    public byte[] getPayload();

    public String getPayloadString();

    public void setPacketType(CoapPacketType ack);

    public void setPayload(byte[] payload);

    public void setPayload(char[] payload);

    public void setPayload(String payload);

    public int getPayloadLength();

    public void setContentType(CoapMediaType mediaType);

    public CoapMediaType getContentType();

    public byte[] getToken();

    public void setToken(byte[] token);

    public String getTokenString();

    CoapBlockOption getBlock1();

    void setBlock1(CoapBlockOption blockOption);

    CoapBlockOption getBlock2();

    void setBlock2(CoapBlockOption blockOption);

    public Integer getObserveOption();

    public void setObserveOption(int sequenceNumber);

    public void setObserveOption(byte[] oicObserve);

    public void removeOption(CoapHeaderOptionType optionType); //TODO: could this compromise the internal state?

    public String toString();

    public CoapChannel getChannel();

    public void setChannel(CoapChannel channel);

    public int getTimeout();

    public boolean maxRetransReached();

    public boolean isReliable();

    public boolean isRequest();

    public boolean isResponse();

    public boolean isEmpty();

    /* unique by remote address, remote port, local port and message id */
    public int hashCode();
    
    public boolean equals(Object obj);
}

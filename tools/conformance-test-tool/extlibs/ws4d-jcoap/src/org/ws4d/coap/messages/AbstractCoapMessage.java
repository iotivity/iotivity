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

/* WS4D Java CoAP Implementation
 * (c) 2011 WS4D.org
 *
 * written by Sebastian Unger
 */

package org.ws4d.coap.messages;

import java.nio.ByteBuffer;
import java.nio.charset.Charset;
import java.util.Collections;
import java.util.Iterator;
import java.util.Random;
import java.util.Vector;

import org.apache.log4j.Logger;
import org.ws4d.coap.connection.BasicCoapChannelManager;
import org.ws4d.coap.interfaces.CoapChannel;
import org.ws4d.coap.interfaces.CoapMessage;

/**
 * @author Christian Lerche <christian.lerche@uni-rostock.de>
 */

public abstract class AbstractCoapMessage implements CoapMessage {
    /* use the logger of the channel manager */
    private final static Logger logger                = Logger
            .getLogger(BasicCoapChannelManager.class);
    protected static final int  HEADER_LENGTH         = 4;

    /* Addressing information */
    protected String            sourceIP;
    protected int               sourcePort;
    protected String            destinationIP;
    protected int               destinationPort;

    /* Header */
    protected int               version;
    protected CoapPacketType    packetType;
    protected int               messageCodeValue;
    // protected int optionCount;
    protected int               messageId;

    protected byte[]            token                 = null;                   // OIC
    protected int               tokenLength           = 0;

    /* Options */
    protected CoapHeaderOptions options               = new CoapHeaderOptions();

    /* Payload */
    protected byte[]            payload               = null;
    protected int               payloadLength         = 0;

    /* corresponding channel */
    CoapChannel                 channel               = null;

    /* Retransmission State */
    int                         timeout               = 0;
    int                         retransmissionCounter = 0;

    protected void serialize(byte[] bytes, int length, int offset) {
        /* check length to avoid buffer overflow exceptions */
        this.version = 1;
        this.packetType = (CoapPacketType
                .getPacketType((bytes[offset + 0] & 0x30) >> 4));
        this.tokenLength = bytes[offset + 0] & 0x0F;
        this.messageCodeValue = (bytes[offset + 1] & 0xFF);
        this.messageId = ((bytes[offset + 2] << 8) & 0xFF00)
                + (bytes[offset + 3] & 0xFF);

        /* get the token */
        if (tokenLength > 0) {
            token = new byte[tokenLength];
            for (int i = 0; i < tokenLength; i++) {
                token[i] = bytes[i + offset + 4];
            }
        }

        /* serialize options */
        this.options = new CoapHeaderOptions(bytes, length,
                offset + HEADER_LENGTH + tokenLength);

        /* get and check payload length */
        payloadLength = length - HEADER_LENGTH - options.getDeserializedLength()
                - tokenLength;
        if (payloadLength < 0) {
            throw new IllegalStateException(
                    "Invalid CoAP Message (payload length negative)");
        }

        int payloadMarker = 0;
        if (payloadLength > 0) {
            payloadLength--;
            payloadMarker = 1;
        }

        /* copy payload */
        int payloadOffset = offset + HEADER_LENGTH + tokenLength
                + options.getDeserializedLength() + payloadMarker;
        payload = new byte[payloadLength];
        for (int i = 0; i < payloadLength; i++) {
            payload[i] = bytes[i + payloadOffset];
        }
    }

    /* TODO: this function should be in another class */
    public static CoapMessage parseMessage(byte[] bytes, int length) {
        return parseMessage(bytes, length, 0);
    }

    public static CoapMessage parseMessage(byte[] bytes, int length,
            int offset) {
        /*
         * we "peek" the header to determine the kind of message TODO: duplicate
         * Code
         */
        int messageCodeValue = (bytes[offset + 1] & 0xFF);

        if (messageCodeValue == 0) {
            return new CoapEmptyMessage(bytes, length, offset);
        } else if (messageCodeValue >= 0 && messageCodeValue <= 31) {
            return new BasicCoapRequest(bytes, length, offset);
        } else if (messageCodeValue >= 64 && messageCodeValue <= 191) {
            return new BasicCoapResponse(bytes, length, offset);
        } else {
            throw new IllegalArgumentException("unknown CoAP message");
        }
    }

    @Override
    public String getSourceIP() {
        return sourceIP;
    }

    @Override
    public void setSourceIP(String sourceIP) {
        this.sourceIP = sourceIP;
    }

    @Override
    public int getSourcePort() {
        return sourcePort;
    }

    @Override
    public void setSourcePort(int sourcePort) {
        this.sourcePort = sourcePort;
    }

    @Override
    public String getDestinationIP() {
        return destinationIP;
    }

    @Override
    public void setDestinationIP(String destinationIP) {
        this.destinationIP = destinationIP;
    }

    @Override
    public int getDestinationPort() {
        return destinationPort;
    }

    @Override
    public void setDestinationPort(int destinationPort) {
        this.destinationPort = destinationPort;
    }

    public int getVersion() {
        return version;
    }

    @Override
    public int getMessageCodeValue() {
        return messageCodeValue;
    }

    @Override
    public CoapPacketType getPacketType() {
        return packetType;
    }

    @Override
    public byte[] getPayload() {
        return payload;
    }

    @Override
    public String getPayloadString() {
        if (payloadLength == 0)
            return "";

        return new String(payload, Charset.forName("UTF-8"));
    }

    @Override
    public int getPayloadLength() {
        return payloadLength;
    }

    @Override
    public int getMessageID() {
        return messageId;
    }

    @Override
    public void setMessageID(int messageId) {
        this.messageId = messageId;
    }

    @Override
    public byte[] serialize() {
        /* TODO improve memory allocation */

        /* serialize header options first to get the length */
        int optionsLength = 0;
        byte[] optionsArray = null;
        if (options != null) {
            optionsArray = this.options.serialize();
            optionsLength = this.options.getSerializedLength();
        }

        /* allocate memory for the complete packet */
        int payloadMarker = 0;
        if (payloadLength > 0)
            payloadMarker = 1;

        int length = HEADER_LENGTH + tokenLength + optionsLength + payloadLength
                + payloadMarker;
        byte[] serializedPacket = new byte[length];

        /* serialize header */
        serializedPacket[0] = (byte) ((this.version & 0x03) << 6);
        serializedPacket[0] |= (byte) ((this.packetType.getValue()
                & 0x03) << 4);
        serializedPacket[0] |= (byte) (tokenLength & 0x0F); // OIC
        serializedPacket[1] = (byte) (this.getMessageCodeValue() & 0xFF);
        serializedPacket[2] = (byte) ((this.messageId >> 8) & 0xFF);
        serializedPacket[3] = (byte) (this.messageId & 0xFF);

        for (int pos = 0; pos < tokenLength; pos++) {
            serializedPacket[pos + 4] = (byte) (this.token[pos] & 0xFF);
        }

        /* copy serialized options to the final array */
        int offset = HEADER_LENGTH + tokenLength;
        if (options != null) {
            for (int i = 0; i < optionsLength; i++)
                serializedPacket[i + offset] = optionsArray[i];
        }

        offset = HEADER_LENGTH + tokenLength + optionsLength;

        if (this.payloadLength > 0) {
            /* Use the payload marker */
            serializedPacket[offset] = (byte) 0xFF;
            offset++;
            /* copy payload to the final array */
            for (int i = 0; i < this.payloadLength; i++) {
                serializedPacket[i + offset] = payload[i];
            }
        }

        return serializedPacket;
    }

    @Override
    public void setPacketType(CoapPacketType packetType) {
        this.packetType = packetType;
    }

    @Override
    public void setPayload(byte[] payload) {
        this.payload = payload;
        if (payload != null)
            this.payloadLength = payload.length;
        else
            this.payloadLength = 0;
    }

    @Override
    public void setPayload(char[] payload) {
        this.payload = new byte[payload.length];
        for (int i = 0; i < payload.length; i++) {
            this.payload[i] = (byte) payload[i];
        }
        this.payloadLength = payload.length;
    }

    @Override
    public void setPayload(String payload) {
        setPayload(payload.toCharArray());
    }

    @Override
    public void setContentType(CoapMediaType mediaType) {
        CoapHeaderOption option = options
                .getOption(CoapHeaderOptionType.Content_Type);
        if (option != null) {
            /* content Type MUST only exists once */
            throw new IllegalStateException("added content option twice");
        }

        if (mediaType == CoapMediaType.UNKNOWN) {
            throw new IllegalStateException("unknown content type");
        }
        /* convert value */
        byte[] data = long2CoapUint(mediaType.getValue());
        /* no need to check result, mediaType is safe */
        /* add option to Coap Header */
        options.addOption(
                new CoapHeaderOption(CoapHeaderOptionType.Content_Type, data));
    }

    @Override
    public CoapMediaType getContentType() {
        CoapHeaderOption option = options
                .getOption(CoapHeaderOptionType.Content_Type);
        if (option == null) {
            /* not content type TODO: return UNKNOWN ? */
            return null;
        }
        /* no need to check length, CoapMediaType parse function will do */
        int mediaTypeCode = (int) coapUint2Long(options
                .getOption(CoapHeaderOptionType.Content_Type).getOptionData());
        return CoapMediaType.parse(mediaTypeCode);
    }

    @Override
    public String getContentTypeString() {
        CoapHeaderOption option = options
                .getOption(CoapHeaderOptionType.Content_Type);

        if (option == null)
            return null;
        int mediaTypeCode = (int) coapUint2Long(option.getOptionData());

        return CoapMediaType.parse(mediaTypeCode).toString();
    }

    @Override
    public String getAcceptString() {
        CoapHeaderOption option = options
                .getOption(CoapHeaderOptionType.Accept);

        if (option == null)
            return null;
        int mediaTypeCode = (int) coapUint2Long(option.getOptionData());

        return CoapMediaType.parse(mediaTypeCode).toString();
    }

    @Override
    public byte[] getToken() {
        return token;
    }

    @Override
    public String getTokenString() {

        if (tokenLength == 0)
            return "";

        return new String(token, Charset.forName("UTF-8"));
    }

    @Override
    public void setToken(byte[] token) {
        if (token == null) {
            return;
        }
        if (token.length < 1 || token.length > 8) {
            throw new IllegalArgumentException("Invalid Token Length");
        }
        tokenLength = token.length;
        this.token = token;
    }

    @Override
    public CoapBlockOption getBlock1() {
        CoapHeaderOption option = options
                .getOption(CoapHeaderOptionType.Block1);
        if (option == null) {
            return null;
        }

        CoapBlockOption blockOpt = new CoapBlockOption(option.getOptionData());
        return blockOpt;
    }

    @Override
    public void setBlock1(CoapBlockOption blockOption) {
        CoapHeaderOption option = options
                .getOption(CoapHeaderOptionType.Block1);
        if (option != null) {
            // option already exists
            options.removeOption(CoapHeaderOptionType.Block1);
        }
        options.addOption(CoapHeaderOptionType.Block1, blockOption.getBytes());
    }

    @Override
    public CoapBlockOption getBlock2() {
        CoapHeaderOption option = options
                .getOption(CoapHeaderOptionType.Block2);
        if (option == null) {
            return null;
        }
        CoapBlockOption blockOpt = new CoapBlockOption(option.getOptionData());
        return blockOpt;
    }

    @Override
    public void setBlock2(CoapBlockOption blockOption) {
        CoapHeaderOption option = options
                .getOption(CoapHeaderOptionType.Block2);
        if (option != null) {
            // option already exists
            options.removeOption(CoapHeaderOptionType.Block2);
        }
        options.addOption(CoapHeaderOptionType.Block2, blockOption.getBytes());
    }

    @Override
    public Integer getObserveOption() {
        CoapHeaderOption option = options
                .getOption(CoapHeaderOptionType.Observe);
        if (option == null) {
            return null;
        }
        byte[] data = option.getOptionData();

        if (data.length < 0 || data.length > 4) {
            logger.warn("invalid observe option length, return null");
            return null;
        }

        // Reorder bytes
        for (int i = 0; i < data.length / 2; i++) {
            byte temp = data[i];
            data[i] = data[(data.length - i - 1)];
            data[(data.length - i - 1)] = temp;
        }

        return (int) AbstractCoapMessage.coapUint2Long(data);
    }

    @Override
    public void setObserveOption(byte[] oicObserve) {
        for (int i = 0; i < oicObserve.length / 2; i++) {
            byte temp = oicObserve[i];
            oicObserve[i] = oicObserve[(oicObserve.length - i - 1)];
            oicObserve[(oicObserve.length - i - 1)] = temp;
        }

        options.addOption(CoapHeaderOptionType.Observe, oicObserve);
    }

    @Override
    public void setObserveOption(int sequenceNumber) {
        CoapHeaderOption option = options
                .getOption(CoapHeaderOptionType.Observe);
        if (option != null) {
            options.removeOption(CoapHeaderOptionType.Observe);
        }

        byte[] data = long2CoapUint(sequenceNumber);

        if (data.length < 0 || data.length > 2) {
            throw new IllegalArgumentException("invalid observe option length");
        }

        options.addOption(CoapHeaderOptionType.Observe, data);
    }

    public void copyHeaderOptions(AbstractCoapMessage origin) {
        options.removeAll();
        options.copyFrom(origin.options);
    }

    @Override
    public void removeOption(CoapHeaderOptionType optionType) {
        options.removeOption(optionType);
    }

    @Override
    public CoapChannel getChannel() {
        return channel;
    }

    @Override
    public void setChannel(CoapChannel channel) {
        this.channel = channel;
    }

    @Override
    public int getTimeout() {
        if (timeout == 0) {
            Random random = new Random();
            timeout = RESPONSE_TIMEOUT_MS + random.nextInt(
                    (int) (RESPONSE_TIMEOUT_MS * RESPONSE_RANDOM_FACTOR)
                            - RESPONSE_TIMEOUT_MS);
        }
        return timeout;
    }

    @Override
    public boolean maxRetransReached() {
        if (retransmissionCounter < MAX_RETRANSMIT) {
            return false;
        }
        return true;
    }

    @Override
    public void incRetransCounterAndTimeout() { /* TODO: Rename */
        retransmissionCounter += 1;
        timeout *= 2;
    }

    @Override
    public boolean isReliable() {
        if (packetType == CoapPacketType.NON) {
            return false;
        }
        return true;
    }

    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + ((channel == null) ? 0 : channel.hashCode());
        result = prime * result + getMessageID();
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
        AbstractCoapMessage other = (AbstractCoapMessage) obj;
        if (channel == null) {
            if (other.channel != null)
                return false;
        } else if (!channel.equals(other.channel))
            return false;
        if (getMessageID() != other.getMessageID())
            return false;
        return true;
    }

    protected static long coapUint2Long(byte[] data) {
        /* avoid buffer overflow */
        if (data.length > 8) {
            return -1;
        }

        /* fill with leading zeros */
        byte[] tmp = new byte[8];
        for (int i = 0; i < data.length; i++) {
            tmp[i + 8 - data.length] = data[i];
        }

        /* convert to long */
        ByteBuffer buf = ByteBuffer.wrap(tmp);
        /* byte buffer contains 8 bytes */
        return buf.getLong();
    }

    protected static byte[] long2CoapUint(long value) {
        /* only unsigned values supported */
        if (value < 0) {
            return null;
        }

        /* a zero length value implies zero */
        if (value == 0) {
            return new byte[0];
        }

        /* convert long to byte array with a fixed length of 8 byte */
        ByteBuffer buf = ByteBuffer.allocate(8);
        buf.putLong(value);
        byte[] tmp = buf.array();

        /* remove leading zeros */
        int leadingZeros = 0;
        for (int i = 0; i < tmp.length; i++) {
            if (tmp[i] == 0) {
                leadingZeros = i + 1;
            } else {
                break;
            }
        }
        /* copy to byte array without leading zeros */
        byte[] result = new byte[8 - leadingZeros];
        for (int i = 0; i < result.length; i++) {
            result[i] = tmp[i + leadingZeros];
        }

        return result;
    }

    public enum CoapHeaderOptionType {
        // OIC
        UNKNOWN(-1), If_Match(1), Uri_Host(3), Etag(4), If_None_Match(
                5), Observe(6), Uri_Port(7), Location_Path(8), Uri_Path(
                        11), Content_Type(12), Max_Age(14), Uri_Query(
                                15), Accept(17), Location_Query(20), Block2(
                                        23), Block1(27), Size2(28), Proxy_Uri(
                                                35), Size1(60);

        int value;

        CoapHeaderOptionType(int optionValue) {
            value = optionValue;
        }

        public static CoapHeaderOptionType parse(int optionTypeValue) {
            switch (optionTypeValue) {
                // OIC
                case 1:
                    return If_Match;
                case 3:
                    return Uri_Host;
                case 4:
                    return Etag;
                case 5:
                    return If_None_Match;
                case 6:
                    return Observe;
                case 7:
                    return Uri_Port;
                case 8:
                    return Location_Path;
                case 11:
                    return Uri_Path;
                case 12:
                    return Content_Type;
                case 14:
                    return Max_Age;
                case 15:
                    return Uri_Query;
                case 17:
                    return Accept;
                case 20:
                    return Location_Query;
                case 23:
                    return Block2;
                case 27:
                    return Block1;
                case 28:
                    return Size2;
                case 35:
                    return Proxy_Uri;
                case 60:
                    return Size1;
                default:
                    return UNKNOWN;
            }
        }

        public String getString() {
            String optionName = "";
            switch (value) {
                case 11:
                    optionName = "Uri-Path";
                    break;
                case 15:
                    optionName = "Uri-Query";
                    break;
            }
            return optionName;
        }

        public int getValue() {
            return value;
        }
        /* TODO: implement validity checks */
        /* TODO: implement isCritical(int optionTypeValue), isElective() */

    }

    protected class CoapHeaderOption implements Comparable<CoapHeaderOption> {

        CoapHeaderOptionType optionType;
        int                  optionTypeValue;   /*
                                                 * integer representation of
                                                 * optionType
                                                 */
        byte[]               optionData;
        int                  shortLength;
        int                  longLength;
        int                  deserializedLength;
        static final int     MAX_LENGTH = 270;

        public int getDeserializedLength() {
            return deserializedLength;
        }

        public CoapHeaderOption(CoapHeaderOptionType optionType, byte[] value) {
            if (optionType == CoapHeaderOptionType.UNKNOWN) {
                /* TODO: implement check if it is a critical option */
                throw new IllegalStateException("Unknown header option");
            }
            if (value == null) {
                throw new IllegalArgumentException(
                        "Header option value MUST NOT be null");
            }

            this.optionType = optionType; // This bit was missing
            this.optionTypeValue = optionType.getValue();
            this.optionData = value;
            if (value.length < 13) {
                shortLength = value.length;
                longLength = 0;
            } else {
                shortLength = 13;
                longLength = value.length - shortLength;
            }
        }

        public CoapHeaderOption(byte[] bytes, int offset,
                int lastOptionNumber) {
            int headerLength;
            /* parse option type */

            optionTypeValue = ((bytes[offset] & 0xF0) >> 4);
            if (optionTypeValue == 13) {
                optionTypeValue = optionTypeValue + (bytes[offset + 1] & 0xFF)
                        + lastOptionNumber;
                shortLength = bytes[offset] & 0x0F;
                longLength = 0;
                headerLength = 2;

                /* copy value */
                optionData = new byte[shortLength + longLength];
                for (int i = 0; i < shortLength + longLength; i++) {
                    optionData[i] = bytes[i + headerLength + offset];
                }
                deserializedLength += headerLength + shortLength + longLength;
                optionType = CoapHeaderOptionType.parse(optionTypeValue);
                return;
            }
            if (optionTypeValue == 14) {
                optionTypeValue = optionTypeValue
                        + (bytes[offset + 1] & 0xFF) * 16
                        + (bytes[offset + 2] & 0xFF) + lastOptionNumber;
                shortLength = bytes[offset] & 0x0F;
                longLength = 0;
                headerLength = 3;

                /* copy value */
                optionData = new byte[shortLength + longLength];
                for (int i = 0; i < shortLength + longLength; i++) {
                    optionData[i] = bytes[i + headerLength + offset];
                }
                deserializedLength += headerLength + shortLength + longLength;
                optionType = CoapHeaderOptionType.parse(optionTypeValue);
                return;
            }
            optionTypeValue = optionTypeValue + lastOptionNumber;
            if (optionTypeValue == 28) {
                shortLength = bytes[offset] & 0x0F;
                longLength = 0;
                headerLength = 1;

                /* copy value */
                optionData = new byte[shortLength + longLength];
                for (int i = 0; i < shortLength + longLength; i++) {
                    optionData[i] = bytes[i + headerLength + offset];
                }
                deserializedLength += headerLength + shortLength + longLength;
                optionType = CoapHeaderOptionType.parse(optionTypeValue);
                return;
            }
            optionType = CoapHeaderOptionType.parse(optionTypeValue);
            if (optionType == CoapHeaderOptionType.UNKNOWN) {
                // Temporary code to ignore unknown option
                if (optionTypeValue > 12) {
                    shortLength = bytes[offset] & 0x0F;
                    longLength = 0;
                    headerLength = 3;
                    deserializedLength += headerLength + shortLength
                            + longLength;
                    optionData = null;
                    return;
                }

                if (optionTypeValue % 14 == 0) {
                    /* no-op: no operation for deltas > 14 */
                } else {

                    /* TODO: implement check if it is a critical option */
                    throw new IllegalArgumentException("Unknown header option");
                }
            }
            /* parse length */
            if ((bytes[offset] & 0x0F) < 13) {
                shortLength = bytes[offset] & 0x0F;
                longLength = 0;
                headerLength = 1;
            } else {
                shortLength = 13;
                longLength = bytes[offset + 1];
                headerLength = 2; /* additional length byte */
            }

            /* copy value */
            optionData = new byte[shortLength + longLength];
            for (int i = 0; i < shortLength + longLength; i++) {
                optionData[i] = bytes[i + headerLength + offset];
            }

            deserializedLength += headerLength + shortLength + longLength;
        }

        @Override
        public int compareTo(CoapHeaderOption option) {
            /*
             * compare function for sorting TODO: check what happens in case of
             * equal option values IMPORTANT: order must be the same for e.g.,
             * URI path
             */
            if (this.optionTypeValue != option.optionTypeValue)
                return this.optionTypeValue < option.optionTypeValue ? -1 : 1;
            else
                return 0;
        }

        public boolean hasLongLength() {
            if (shortLength == 13) {
                return true;
            } else
                return false;
        }

        public int getLongLength() {
            return longLength;
        }

        public int getShortLength() {
            return shortLength;
        }

        public int getOptionTypeValue() {
            return optionTypeValue;
        }

        public byte[] getOptionData() {
            return optionData;
        }

        public int getSerializeLength() {
            if (hasLongLength()) {
                return optionData.length + 2;
            } else {
                return optionData.length + 1;
            }
        }

        @Override
        public String toString() {
            char[] printableOptionValue = new char[optionData.length];
            for (int i = 0; i < optionData.length; i++)
                printableOptionValue[i] = (char) optionData[i];
            return "Option Number: " + " (" + optionTypeValue + ")"
                    + ", Option Value: "
                    + String.copyValueOf(printableOptionValue);
        }

        public CoapHeaderOptionType getOptionType() {
            return optionType;
        }
    }

    protected class CoapHeaderOptions implements Iterable<CoapHeaderOption> {

        private Vector<CoapHeaderOption> headerOptions    = new Vector<CoapHeaderOption>();
        private int                      deserializedLength;
        private int                      serializedLength = 0;

        /*
         * public CoapHeaderOptions(byte[] bytes, int option_count){ this(bytes,
         * option_count, option_count); }
         */

        public CoapHeaderOptions(byte[] bytes, int messageLength, int offset) {
            /* note: we only receive deltas and never concrete numbers */
            /* TODO: check integrity */
            deserializedLength = 0;
            int lastOptionNumber = 0;
            int optionOffset = offset;

            while ((optionOffset < messageLength)
                    && ((bytes[optionOffset] & 0xFF) != 0xFF)) {
                CoapHeaderOption option = new CoapHeaderOption(bytes,
                        optionOffset, lastOptionNumber);
                lastOptionNumber = option.getOptionTypeValue();
                deserializedLength += option.getDeserializedLength();
                optionOffset += option.getDeserializedLength();

                if (option.getOptionData() != null) // TANVIR: added for
                                                    // ignoring unknown option
                    addOption(option);
            }
        }

        public CoapHeaderOptions() {
            /* creates empty header options */
        }

        public CoapHeaderOption getOption(int optionNumber) {
            for (CoapHeaderOption headerOption : headerOptions) {
                if (headerOption.getOptionTypeValue() == optionNumber) {
                    return headerOption;
                }
            }
            return null;
        }

        public CoapHeaderOption getOption(CoapHeaderOptionType optionType) {
            for (CoapHeaderOption headerOption : headerOptions) {
                if (headerOption.getOptionType() == optionType) {
                    return headerOption;
                }
            }
            return null;
        }

        public boolean optionExists(CoapHeaderOptionType optionType) {
            CoapHeaderOption option = getOption(optionType);
            if (option == null) {
                return false;
            } else
                return true;
        }

        public void addOption(CoapHeaderOption option) {
            headerOptions.add(option);
            /* TODO: only sort when options are serialized */
            Collections.sort(headerOptions);
        }

        public void addOption(CoapHeaderOptionType optionType, byte[] value) {
            addOption(new CoapHeaderOption(optionType, value));
        }

        public void removeOption(CoapHeaderOptionType optionType) {
            CoapHeaderOption headerOption;
            // get elements of Vector

            /*
             * note: iterating over and changing a vector at the same time is
             * not allowed
             */
            int i = 0;
            while (i < headerOptions.size()) {
                headerOption = headerOptions.get(i);
                if (headerOption.getOptionType() == optionType) {
                    headerOptions.remove(i);
                } else {
                    /* only increase when no element was removed */
                    i++;
                }
            }
            Collections.sort(headerOptions);
        }

        public void removeAll() {
            headerOptions.clear();
        }

        public void copyFrom(CoapHeaderOptions origin) {
            for (CoapHeaderOption option : origin) {
                addOption(option);
            }
        }

        public int getOptionCount() {
            return headerOptions.size();
        }

        public byte[] serialize() {
            /*
             * options are serialized here to be more efficient (only one byte
             * array necessary)
             */
            int length = 0;

            /* calculate the overall length first */
            for (CoapHeaderOption option : headerOptions) {
                length += option.getSerializeLength();
            }

            byte[] data = new byte[length];
            int arrayIndex = 0;

            int lastOptionNumber = 0; /* let's keep track of this */
            for (CoapHeaderOption headerOption : headerOptions) {
                /*
                 * TODO: move the serialization implementation to
                 * CoapHeaderOption
                 */
                int optionDelta = headerOption.getOptionTypeValue()
                        - lastOptionNumber;
                lastOptionNumber = headerOption.getOptionTypeValue();
                // set length(s)
                data[arrayIndex++] = (byte) (((optionDelta & 0x0F) << 4)
                        | (headerOption.getShortLength() & 0x0F));
                if (headerOption.hasLongLength()) {
                    data[arrayIndex++] = (byte) (headerOption.getLongLength()
                            & 0xFF);
                }
                // copy option value
                byte[] value = headerOption.getOptionData();
                for (int i = 0; i < value.length; i++) {
                    data[arrayIndex++] = value[i];
                }
            }
            serializedLength = length;
            return data;
        }

        public int getDeserializedLength() {
            return deserializedLength;
        }

        public int getSerializedLength() {
            return serializedLength;
        }

        @Override
        public Iterator<CoapHeaderOption> iterator() {
            return headerOptions.iterator();
        }

        @Override
        public String toString() {
            String result = "\tOptions:\n";
            for (CoapHeaderOption option : headerOptions) {
                result += "\t\t" + option.toString() + "\n";
            }
            return result;
        }
    }

}

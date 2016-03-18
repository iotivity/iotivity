//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

package oic.ctt.network.control;

import oic.ctt.network.OICHelper;
import oic.ctt.network.control.SecurityConstants.ArgumentType;
import oic.ctt.network.control.SecurityConstants.CipherSuite;
import oic.ctt.network.control.SecurityConstants.MessageCode;

import java.net.Inet4Address;
import java.net.Inet6Address;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.ArrayList;

/**
 * ControlMessage class. The interactions and data exchange between
 * ControlClient and DTLS relay are structured into message objects defined by
 * this class
 *
 * @author Tanvir Ferdousi (tanvir.f@samsung.com)
 *
 */
public class ControlMessage {

    public static final int            HEADER_LENGTH     = 4;
    private MessageCode                mMessageCode;
    private int                        mMessageId;
    private InetAddress                mRemoteAddress;
    private int                        mRemotePort;
    private ArrayList<MessageArgument> mMessageArguments = new ArrayList<MessageArgument>();

    /**
     * Creates a ControlMessage object
     */
    public ControlMessage() {
        mMessageCode = null;
        mMessageId = OICHelper.getRandomMessageId();
    }

    /**
     * Gets the message code
     *
     * @return MessageCode enum type
     */
    public MessageCode getMessageCode() {
        return mMessageCode;
    }

    /**
     * Sets the message code
     *
     * @param messageCode
     *            Enum type (MessageCode)
     */
    public void setMessageCode(MessageCode messageCode) {
        this.mMessageCode = messageCode;
    }

    /**
     * Gets the message ID
     *
     * @return Integer containing the message ID
     */
    public int getMessageId() {
        return mMessageId;
    }

    /**
     * Sets the message ID
     *
     * @param messageId
     *            16 bit unsigned integer
     */
    public void setMessageId(int messageId) {
        this.mMessageId = messageId;
    }

    /**
     * Adds an argument to this message
     *
     * @param msgArgument
     *            MessageArgument object
     */
    public void addArgument(MessageArgument msgArgument) {
        mMessageArguments.add(msgArgument);
    }

    /**
     * Adds the private data (PSK) argument to this message
     *
     * @param privateData
     */
    public void addArgumentPrivateData(byte[] privateData) {
        MessageArgument pvDataArg = new MessageArgument();
        pvDataArg.setType(ArgumentType.PRIVATE_DATA);
        pvDataArg.setValue(privateData);
        this.addArgument(pvDataArg);
    }

    /**
     * Gets the private data (PSK) from this message
     *
     * @return byte array containing the pvdata
     */
    public byte[] getPrivateData() {
        for (MessageArgument messageArgument : mMessageArguments) {
            if (messageArgument.getType().equals(ArgumentType.PRIVATE_DATA)) {
                return messageArgument.getValue();
            }
        }

        return null;
    }

    /**
     * Adds the network address argument
     *
     * @param ipAddress
     *            String containing the IPv4 or IPv6 address
     */
    public void addArgumentIpAddress(String ipAddress) {
        MessageArgument argIpAddress = new MessageArgument();
        InetAddress inetAddress = null;

        try {
            inetAddress = InetAddress.getByName(ipAddress);
        } catch (UnknownHostException e1) {
            e1.printStackTrace();
        }

        if (inetAddress == null) {
            System.err.println("IP address not valid!");
            return;
        }

        if (inetAddress instanceof Inet6Address)
            argIpAddress.setType(ArgumentType.IP_V6_ADDRESS);
        else if (inetAddress instanceof Inet4Address)
            argIpAddress.setType(ArgumentType.IP_V4_ADDRESS);
        else {
            System.err.println("IP address not valid!");
            return;
        }
        argIpAddress.setValue(inetAddress.getAddress());

        this.addArgument(argIpAddress);
    }

    /**
     * Adds the Port argument
     *
     * @param port
     *            integer specifying the port number
     */
    public void addArgumentPort(int port) {
        MessageArgument argPort = new MessageArgument();
        argPort.setType(ArgumentType.PORT);

        byte[] portByte = new byte[2];
        portByte[0] = (byte) ((port >> 8) & 0xFF);
        portByte[1] = (byte) (port & 0xFF);
        argPort.setValue(portByte);

        this.addArgument(argPort);
    }

    /**
     * Adds the ciphersuite argument
     *
     * @param cipherSuite
     *            Enum of type CipherSuite
     */
    public void addArgumentCipherSuite(CipherSuite cipherSuite) {
        MessageArgument argCipherSuite = new MessageArgument();
        argCipherSuite.setType(ArgumentType.CIPHER_SUITE);

        byte[] cipherSuiteByte = new byte[2];
        cipherSuiteByte[0] = (byte) ((cipherSuite.getValue() >> 8) & 0xFF);
        cipherSuiteByte[1] = (byte) (cipherSuite.getValue() & 0xFF);
        argCipherSuite.setValue(cipherSuiteByte);

        this.addArgument(argCipherSuite);
    }

    public static ControlMessage parse(byte[] bytes, int position) {
        ControlMessage controlMessage = new ControlMessage();
        int argCount = 0;
        int offset = 0;
        int i, j;

        controlMessage.mMessageCode = SecurityConstants.MessageCode
                .parseMessageCode(bytes[offset + 0] & 0xFF);
        controlMessage.mMessageId = ((bytes[offset + 1] << 8) & 0xFF00)
                + (bytes[offset + 2] & 0xFF);

        argCount = bytes[offset + 3] & 0xFF;

        offset = HEADER_LENGTH;

        if (argCount > 0) {
            MessageArgument msgArgument = null;
            int length = 0;
            byte[] argValue = null;
            for (i = 0; i < argCount; i++) {

                msgArgument = new MessageArgument();

                // read argument parameters
                msgArgument.setType(ArgumentType
                        .parseArgumentType(bytes[offset + 0] & 0xFF));

                length = ((bytes[offset + 1] << 8) & 0xFF00)
                        + (bytes[offset + 2] & 0xFF);

                // msgArgument.setLength(length);

                argValue = new byte[length];

                for (j = 0; j < length; j++) {
                    argValue[j] = bytes[offset + 3 + j];
                }

                msgArgument.setValue(argValue);

                // add to arraylist
                controlMessage.addArgument(msgArgument);
                offset += 3 + length;
            }
        }

        return controlMessage;
    }

    /**
     * Sets the destination (remote) address for this message
     *
     * @param remoteAddress
     *            Object of type InetAddress
     */
    public void setRemoteAddress(InetAddress remoteAddress) {
        this.mRemoteAddress = remoteAddress;
    }

    /**
     * Sets the destination (remote) port for this message
     *
     * @param remotePort
     *            Integer specifying the port number
     */
    public void setRemotePort(int remotePort) {
        this.mRemotePort = remotePort;
    }

    /**
     * Gets the remote address of this message
     *
     * @return Object of type InetAddress
     */
    public InetAddress getRemoteAddress() {
        return this.mRemoteAddress;
    }

    /**
     * Gets the remote port of this message
     *
     * @return Integer containing the Port number
     */
    public int getRemotePort() {
        return this.mRemotePort;
    }

    public byte[] toByteData() {
        // Determine the length of message
        int bufferLength;
        int argLength = 0;
        int argCount = mMessageArguments.size();
        int offset = 0;
        int valueLength = 0;
        int i;

        if (argCount > 0) {
            for (MessageArgument msgArgument : mMessageArguments) {
                argLength += msgArgument.getLength() + 3; // Type: 1 byte,
                                                          // Length: 2 bytes
            }
        }
        bufferLength = HEADER_LENGTH + argLength;
        byte[] msgBytes = new byte[bufferLength];

        // Header
        msgBytes[0] = (byte) (this.mMessageCode.getValue() & 0xFF);
        msgBytes[1] = (byte) ((this.mMessageId >> 8) & 0xFF);
        msgBytes[2] = (byte) (this.mMessageId & 0xFF);
        msgBytes[3] = (byte) (argCount & 0xFF);

        offset = HEADER_LENGTH;

        // Arguments
        if (argCount > 0) {
            for (MessageArgument msgArgument : mMessageArguments) {
                // [Type] [Length] [Value]
                msgBytes[offset++] = (byte) (msgArgument.getType().getValue()
                        & 0xFF);

                valueLength = msgArgument.getLength();
                msgBytes[offset++] = (byte) ((valueLength >> 8) & 0xFF);
                msgBytes[offset++] = (byte) (valueLength & 0xFF);

                byte[] argValue = msgArgument.getValue();

                for (i = 0; i < valueLength; i++) {
                    msgBytes[offset++] = argValue[i];
                }
            }
        }
        return msgBytes;
    }

}
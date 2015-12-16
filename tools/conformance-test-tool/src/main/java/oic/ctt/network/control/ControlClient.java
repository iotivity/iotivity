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

import oic.ctt.logger.CTLogger;
import oic.ctt.network.OICHelper;
import oic.ctt.network.control.SecurityConstants.CipherSuite;
import oic.ctt.network.control.SecurityConstants.MessageCode;

import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.ArrayList;

import org.slf4j.Logger;

public class ControlClient implements ControllerEntity {

    private InetAddress               mRelayIp       = null;
    private int                       mControlPort   = 0;
    private UdpSocketHandler          mSocketHandler = null;
    private ArrayList<ControlMessage> mResponses     = new ArrayList<ControlMessage>();
    private Logger                    mlogger        = CTLogger.getInstance();

    public ControlClient() {
        this(0);
    }

    public ControlClient(int listeningPort) {
        mSocketHandler = new UdpSocketHandler(listeningPort, this);
    }

    public ControlMessage startDtlsWithPvData(String relayIp, int relayPort,
            String dutIp, int dutPortSecured, int cipherSuite, String clientId,
            String serverId, byte[] privateData) {

        int messageId = OICHelper.getRandomMessageId();

        ControlMessage controlMessage = new ControlMessage();
        controlMessage.setMessageId(messageId);
        controlMessage.setMessageCode(MessageCode.INIT_DTLS_CONNECTION);
        controlMessage.addArgumentIpAddress(dutIp);
        controlMessage.addArgumentPort(dutPortSecured);
        controlMessage.addArgumentCipherSuite(
                CipherSuite.parseCipherSuite(cipherSuite));

        MessageArgument argClientIdentity = new MessageArgument();
        argClientIdentity.setType(
                oic.ctt.network.control.SecurityConstants.ArgumentType.CLIENT_IDENTITY);
        argClientIdentity.setValue(clientId.getBytes());
        controlMessage.addArgument(argClientIdentity);

        MessageArgument argServerIdentity = new MessageArgument();
        argServerIdentity.setType(
                oic.ctt.network.control.SecurityConstants.ArgumentType.SERVER_IDENTITY);
        argServerIdentity.setValue(serverId.getBytes());
        controlMessage.addArgument(argServerIdentity);

        controlMessage.addArgumentPrivateData(privateData);

        try {
            mRelayIp = InetAddress.getByName(relayIp);
        } catch (UnknownHostException e) {
            e.printStackTrace();
        }

        if (mRelayIp == null)
            return null;

        mControlPort = relayPort;
        controlMessage.setRemoteAddress(mRelayIp);
        controlMessage.setRemotePort(mControlPort);
        mSocketHandler.sendMessage(controlMessage);

        return getResponse(messageId);
    }

    public ControlMessage initDtls(String relayIp, int relayPort, String dutIp,
            int dutPortSecured, int cipherSuite, String clientId,
            String serverId, String password) {

        mResponses.clear();

        int messageId = OICHelper.getRandomMessageId();

        ControlMessage controlMessage = new ControlMessage();
        controlMessage.setMessageId(messageId);
        controlMessage.setMessageCode(MessageCode.INIT_DTLS_CONNECTION);
        controlMessage.addArgumentIpAddress(dutIp);
        controlMessage.addArgumentPort(dutPortSecured);
        controlMessage.addArgumentCipherSuite(
                CipherSuite.parseCipherSuite(cipherSuite));

        MessageArgument argClientIdentity = new MessageArgument();
        argClientIdentity.setType(
                oic.ctt.network.control.SecurityConstants.ArgumentType.CLIENT_IDENTITY);
        argClientIdentity.setValue(clientId.getBytes());
        controlMessage.addArgument(argClientIdentity);

        MessageArgument argServerIdentity = new MessageArgument();
        argServerIdentity.setType(
                oic.ctt.network.control.SecurityConstants.ArgumentType.SERVER_IDENTITY);
        argServerIdentity.setValue(serverId.getBytes());
        controlMessage.addArgument(argServerIdentity);

        MessageArgument argPassword = new MessageArgument();
        argPassword.setType(
                oic.ctt.network.control.SecurityConstants.ArgumentType.PASSWORD);
        argPassword.setValue(password.getBytes());
        controlMessage.addArgument(argPassword);

        try {
            mRelayIp = InetAddress.getByName(relayIp);
        } catch (UnknownHostException e) {
            e.printStackTrace();
        }

        if (mRelayIp == null)
            return null;

        mControlPort = relayPort;
        controlMessage.setRemoteAddress(mRelayIp);
        controlMessage.setRemotePort(mControlPort);
        mSocketHandler.sendMessage(controlMessage);

        return getResponse(messageId);
    }

    public ControlMessage terminateDtls() {
        mResponses.clear();
        int messageId = OICHelper.getRandomMessageId();

        ControlMessage controlMessage = new ControlMessage();
        controlMessage.setMessageId(messageId);
        controlMessage.setMessageCode(MessageCode.TERMINATE_DTLS_CONNECTION);
        controlMessage.setRemoteAddress(mRelayIp);
        controlMessage.setRemotePort(mControlPort);
        mSocketHandler.sendMessage(controlMessage);

        return getResponse(messageId);
    }

    private ControlMessage getResponse(int messageId) {
        try {
            Thread.sleep(1000);
        } catch (InterruptedException ex) {
            Thread.currentThread().interrupt();
        }

        for (ControlMessage response : mResponses) {
            if (messageId == response.getMessageId())
                return response;
        }

        return null;
    }

    @Override
    public void handleMessage(ControlMessage controlMsg) {
        mResponses.add(controlMsg);
        mlogger.info("Response Received");
    }

}

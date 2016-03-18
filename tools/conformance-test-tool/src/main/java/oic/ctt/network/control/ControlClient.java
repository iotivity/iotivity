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

/**
 * The ControlClient class. Provides APIs to interact with the DTLS relay
 *
 * @author Tanvir Ferdousi (tanvir.f@samsung.com)
 */
public class ControlClient implements ControllerEntity {

    private InetAddress               mRelayIp       = null;
    private int                       mControlPort   = 0;
    private UdpSocketHandler          mSocketHandler = null;
    private ArrayList<ControlMessage> mResponses     = new ArrayList<ControlMessage>();
    private Logger                    mlogger        = CTLogger.getInstance();

    /**
     * Creates an instance of ControlClient. Starts listening on a random port
     */
    public ControlClient() {
        this(0);
    }

    /**
     * Creates an instance of ControlClient. Starts listening on the specified
     * port
     *
     * @param listeningPort
     *            The UDP port number the client will listen to
     */
    public ControlClient(int listeningPort) {
        mSocketHandler = new UdpSocketHandler(listeningPort, this);
    }

    /**
     * Instructs the DTLS relay to start a DTLS connection using pre-shared key
     * (PSK).
     *
     * @param relayIp
     *            Network address of the DTLS relay
     * @param relayPort
     *            Listening port of the DTLS relay
     * @param dutIp
     *            Network address of the device under test (DUT)
     * @param dutPortSecured
     *            Secured port number of the device under test (DUT)
     * @param cipherSuite
     *            The Ciphersuite to be used for DTLS connection
     * @param clientId
     *            Client device ID
     * @param serverId
     *            Server device ID
     * @param privateData
     *            The pre-shared key (Private data)
     * @return ControlMessage object that contains the response of the DTLS
     *         relay
     */
    public ControlMessage startDtlsWithPvData(String relayIp, int relayPort,
            String dutIp, int dutPortSecured, int cipherSuite, String clientId,
            String serverId, byte[] privateData) {

        mResponses.clear();

        int messageId = OICHelper.getRandomMessageId();
        ControlMessage controlMessage = createControlMessage(messageId, dutIp,
                dutPortSecured, cipherSuite, clientId, serverId);

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

    /**
     * Instructs the DTLS relay to initialize a DTLS connection using the random
     * pin method
     *
     * @param relayIp
     *            Network address of the DTLS relay
     * @param relayPort
     *            Listening port of the DTLS relay
     * @param dutIp
     *            Network address of the device under test (DUT)
     * @param dutPortSecured
     *            Secured port number of the device under test (DUT)
     * @param cipherSuite
     *            The Ciphersuite to be used for DTLS connection
     * @param clientId
     *            Client device ID
     * @param serverId
     *            Server device ID
     * @param password
     *            The random pin
     * @return ControlMessage object that contains the response of the DTLS
     *         relay
     */
    public ControlMessage initDtlsWithRndPin(String relayIp, int relayPort,
            String dutIp, int dutPortSecured, int cipherSuite, String clientId,
            String serverId, String password) {

        mResponses.clear();
        int messageId = OICHelper.getRandomMessageId();
        ControlMessage controlMessage = createControlMessage(messageId, dutIp,
                dutPortSecured, cipherSuite, clientId, serverId);

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

    /**
     * Instructs the DTLS relay to initialize a DTLS connection using the
     * justworks method
     *
     * @param relayIp
     *            Network address of the DTLS relay
     * @param relayPort
     *            Listening port of the DTLS relay
     * @param dutIp
     *            Network address of the device under test (DUT)
     * @param dutPortSecured
     *            Secured port number of the device under test (DUT)
     * @param cipherSuite
     *            The Ciphersuite to be used for DTLS connection
     * @param clientId
     *            Client device ID
     * @param serverId
     *            Server device ID
     * @return ControlMessage object that contains the response of the DTLS
     *         relay
     */
    public ControlMessage initDtlsWithJustWorks(String relayIp, int relayPort,
            String dutIp, int dutPortSecured, int cipherSuite, String clientId,
            String serverId) {

        mResponses.clear();
        int messageId = OICHelper.getRandomMessageId();
        ControlMessage controlMessage = createControlMessage(messageId, dutIp,
                dutPortSecured, cipherSuite, clientId, serverId);

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

    private ControlMessage createControlMessage(int messageId, String dutIp,
            int dutPortSecured, int cipherSuite, String clientId,
            String serverId) {
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

        return controlMessage;
    }

    /**
     * Instructs the DTLS relay to terminate the DTLS connection
     *
     * @return ControlMessage object containing the response of the DTLS relay
     */
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

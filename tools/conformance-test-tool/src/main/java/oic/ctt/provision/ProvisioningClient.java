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
package oic.ctt.provision;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;
import java.net.Inet4Address;
import java.net.Inet6Address;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.ArrayList;
import java.util.Enumeration;

import org.apache.commons.codec.binary.Base64;
import org.slf4j.Logger;

import oic.ctt.logger.CTLogger;
import oic.ctt.network.OICClient;
import oic.ctt.network.OICHelper;
import oic.ctt.network.OICResponseData;
import oic.ctt.network.OICClient.Protocol;
import oic.ctt.network.OICHelper.MessageParameters;
import oic.ctt.network.OICHelper.MessageType;
import oic.ctt.network.OICRequestData.Method;
import oic.ctt.network.control.ControlClient;
import oic.ctt.network.control.ControlMessage;
import oic.ctt.network.control.SecurityConstants.CipherSuite;
import oic.ctt.network.control.SecurityConstants.MessageCode;
import oic.ctt.provision.SecuredDeviceInfo.OTM;

/**
 * The Provisioning Client class. Provides client side APIs for device
 * provisioning
 *
 * @author Tanvir Ferdousi (tanvir.f@samsung.com)
 *
 */
public class ProvisioningClient {

    private Logger        mlogger                   = CTLogger.getInstance();

    private String        mDutIp                    = null;
    private int           mUnsecuredPort            = -1;
    private int           mSecuredPort              = -1;
    private String        mDeviceId                 = null;

    private final String  DEVICE_ID_ENCODED_DEFAULT = "AAAAAAAAAAAAAAAAAAAAAA==";
    private final String  CRED_FILENAME             = "credentials";

    private OICClient     mOicClient                = new OICClient();
    private ControlClient mController               = new ControlClient();

    public ProvisioningClient() {
        mOicClient.setWaitTime(2);
    }

    /**
     * Discovers owned devices. Filters the results with the given network
     * address.
     *
     * @param filterIp
     *            The network address to filter results
     * @return SecureDeviceInfo object containing information about owned device
     */
    public SecuredDeviceInfo discoverOwnedDevice(String filterIp) {
        return discoverDevice(filterIp, "Owned=True");
    }

    /**
     * Discovers unowned devices. Filters the results with the given network
     * address.
     *
     * @param filterIp
     *            The network address to filter results
     * @return SecureDeviceInfo object containing information about unowned
     *         device
     */
    public SecuredDeviceInfo discoverUnownedDevice(String filterIp) {
        return discoverDevice(filterIp, "Owned=False");
    }

    /**
     * Looks for private data in a stored credential file. Matches the data in
     * the file against the device id.
     *
     * @param deviceId
     *            The device id for which the private data is required
     * @return String containing private data
     */
    public String findPrivateData(String deviceId) {
        FileReader fileReader = null;
        String line = null;

        if (deviceId == null || deviceId.isEmpty()) {
            mlogger.error("Null or empty device id!");
            return null;
        }

        try {
            fileReader = new FileReader(CRED_FILENAME);
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }

        if (fileReader == null) {
            mlogger.error("Unable to read file!");
            return null;
        }

        BufferedReader bufferedReader = new BufferedReader(fileReader);

        // Note: May need some upgrade here in the future
        try {
            line = bufferedReader.readLine();
        } catch (IOException e) {
            e.printStackTrace();
        }

        if (line == null || line.length() == 0)
            return null;

        if (line.contains(deviceId))
            return line.split(":")[1];

        return null;
    }

    /**
     * Stores private data in a credential file in the format,
     * deviceId:privateData
     *
     * @param deviceId
     *            The id of the device for which the private data is stored
     * @param privateData
     *            The private data (PSK) that must be stored
     */
    public void storePrivateData(String deviceId, String privateData) {
        PrintWriter writer = null;

        if (deviceId == null || deviceId.isEmpty()) {
            mlogger.error("Null or empty device id!");
            return;
        }

        if (privateData == null || privateData.isEmpty()) {
            mlogger.error("Null or empty private data!");
            return;
        }

        try {
            writer = new PrintWriter(CRED_FILENAME, "UTF-8");
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        }
        if (writer == null) {
            mlogger.error("Unable to create file");
            return;
        }
        writer.println(deviceId + ":" + privateData);
        writer.close();
    }

    /**
     * Establishes the DTLS connection using Pre-shared Keys (PSK)
     *
     * @param privateData
     *            The pre-shared key (pvdata)
     * @param clientDeviceId
     *            Device id of the client
     * @param serverDeviceId
     *            Device id of the server
     * @param relayIp
     *            Network address of the DTLS relay
     * @param relayControlPort
     *            Control port number of the DTLS relay
     * @param dutIp
     *            Network address of the device under test (DUT)
     * @param dutSecuredPort
     *            Secured port number of the device under test (DUT)
     * @return True if connection is successful, False otherwise
     */
    public boolean dtlsConnectWithPSK(String privateData, String clientDeviceId,
            String serverDeviceId, String relayIp, int relayControlPort,
            String dutIp, int dutSecuredPort) {

        if (privateData == null || privateData.isEmpty()) {
            mlogger.error("Null or empty private data!");
            return false;
        }

        if (clientDeviceId == null || clientDeviceId.isEmpty()) {
            mlogger.error("Null or empty client device id!");
            return false;
        }

        if (serverDeviceId == null || serverDeviceId.isEmpty()) {
            mlogger.error("Null or empty server device id!");
            return false;
        }

        if (relayIp == null || relayIp.isEmpty()) {
            mlogger.error("Null or empty relay ip address!");
            return false;
        }

        if (dutIp == null || dutIp.isEmpty()) {
            mlogger.error("Null or empty dut ip address!");
            return false;
        }

        if (relayControlPort < 1) {
            mlogger.error("Invalid relay control port number!");
            return false;
        }

        if (dutSecuredPort < 1) {
            mlogger.error("Invalid dut secured port number!");
            return false;
        }

        ControlMessage response = null;

        response = mController.startDtlsWithPvData(relayIp, relayControlPort,
                dutIp, dutSecuredPort,
                CipherSuite.TLS_PSK_WITH_AES_128_CCM_8.getValue(),
                clientDeviceId, serverDeviceId,
                OICHelper.hexStringToByteArray(privateData));

        if (response == null) {
            mlogger.error("Null response from DTLS relay");
            return false;
        }

        mlogger.info(
                "DTLS connect with PSK response: " + response.getMessageCode());

        if (response.getMessageCode() == MessageCode.DTLS_HANDSHAKE_SUCCESS)
            return true;

        return false;
    }

    /**
     * Configures the device (server) for provisioning. It must be called before
     * a provisioning API is called (JW/RDP)
     *
     * @param otm
     *            Ownership Transfer Method (OTM)
     * @param clientDeviceId
     *            Device id of the client
     * @param serverDeviceId
     *            Device id of the server
     */
    public void configureDeviceForProvisioning(OTM otm, String clientDeviceId,
            String serverDeviceId) {

        if (clientDeviceId == null || clientDeviceId.isEmpty()) {
            mlogger.error("Null or empty client device id!");
            return;
        }

        if (serverDeviceId == null || serverDeviceId.isEmpty()) {
            mlogger.error("Null or empty server device id!");
            return;
        }
        setOwnerTransferMethod(otm, clientDeviceId, serverDeviceId);
        checkAndConfirmProvisioningStatus();
    }

    /**
     * Provisions with just works mechanism. Establishes a DTLS connection and
     * generates a key (pvdata) at the end.
     *
     * @param clientDeviceId
     *            Device id of the client
     * @param serverDeviceId
     *            Device id of the server
     * @param relayIp
     *            Network address of the DTLS relay
     * @param relayControlPort
     *            Control port of the DTLS relay
     * @param dutIp
     *            Network address of the device under test (DUT)
     * @param dutSecuredPort
     *            Secured port number of the device under test (DUT)
     * @return The generated private data in a String form
     */
    public String provisionWithJustWorks(String clientDeviceId,
            String serverDeviceId, String relayIp, int relayControlPort,
            String dutIp, int dutSecuredPort) {

        if (clientDeviceId == null || clientDeviceId.isEmpty()) {
            mlogger.error("Null or empty client device id!");
            return null;
        }

        if (serverDeviceId == null || serverDeviceId.isEmpty()) {
            mlogger.error("Null or empty server device id!");
            return null;
        }

        if (relayIp == null || relayIp.isEmpty()) {
            mlogger.error("Null or empty relay ip address!");
            return null;
        }

        if (dutIp == null || dutIp.isEmpty()) {
            mlogger.error("Null or empty dut ip address!");
            return null;
        }

        if (relayControlPort < 1) {
            mlogger.error("Invalid relay control port number!");
            return null;
        }

        if (dutSecuredPort < 1) {
            mlogger.error("Invalid dut secured port number!");
            return null;
        }

        ControlMessage response = null;
        byte[] privateData = null;

        response = mController.initDtlsWithJustWorks(relayIp, relayControlPort,
                dutIp, dutSecuredPort,
                CipherSuite.TLS_ECDH_anon_WITH_AES_128_CBC_SHA_256.getValue(),
                clientDeviceId, serverDeviceId);

        if (response == null) {
            mlogger.error("Null response from DTLS relay!");
            return null;
        }

        if (response.getMessageCode()
                .equals(MessageCode.DTLS_HANDSHAKE_SUCCESS)) {
            mlogger.info("DTLS Handshake Success");
            privateData = response.getPrivateData();
        } else {
            mlogger.error("DTLS Handshake Failed!");
        }

        if (privateData == null) {
            mlogger.error("Invalid private data!");
            return null;
        }

        mlogger.debug(
                "Private Data obtained: " + OICHelper.bytesToHex(privateData));

        return OICHelper.bytesToHex(privateData);

    }

    /**
     * Provisions with Random pin mechanism. Establishes a DTLS connection and
     * generates a key (pvdata) at the end.
     *
     * @param pin
     *            The random pin/password generated in the server
     * @param clientDeviceId
     *            Device id of the client
     * @param serverDeviceId
     *            Device id of the server
     * @param relayIp
     *            Network address of the DTLS relay
     * @param relayControlPort
     *            Control port of the DTLS relay
     * @param dutIp
     *            Network address of the device under test (DUT)
     * @param dutSecuredPort
     *            Secured port number of the device under test (DUT)
     * @return The generated private data in a String form
     */
    public String provisionWithRandomPin(String pin, String clientDeviceId,
            String serverDeviceId, String relayIp, int relayControlPort,
            String dutIp, int dutSecuredPort) {

        if (pin == null || pin.isEmpty()) {
            mlogger.error("Null or empty random pin!");
            return null;
        }

        if (clientDeviceId == null || clientDeviceId.isEmpty()) {
            mlogger.error("Null or empty client device id!");
            return null;
        }

        if (serverDeviceId == null || serverDeviceId.isEmpty()) {
            mlogger.error("Null or empty server device id!");
            return null;
        }

        if (relayIp == null || relayIp.isEmpty()) {
            mlogger.error("Null or empty relay ip address!");
            return null;
        }

        if (dutIp == null || dutIp.isEmpty()) {
            mlogger.error("Null or empty dut ip address!");
            return null;
        }

        if (relayControlPort < 1) {
            mlogger.error("Invalid relay control port number!");
            return null;
        }

        if (dutSecuredPort < 1) {
            mlogger.error("Invalid dut secured port number!");
            return null;
        }

        ControlMessage response = null;
        byte[] privateData = null;

        response = mController.initDtlsWithRndPin(relayIp, relayControlPort,
                dutIp, dutSecuredPort,
                CipherSuite.TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA_256.getValue(),
                clientDeviceId, serverDeviceId, pin);

        if (response == null) {
            mlogger.error("Null response from DTLS relay!");
            return null;
        }

        if (response.getMessageCode()
                .equals(MessageCode.DTLS_HANDSHAKE_SUCCESS)) {
            mlogger.info("DTLS Handshake Success");
            privateData = response.getPrivateData();
        } else {
            mlogger.error("DTLS Handshake Failed!");
        }

        if (privateData == null) {
            mlogger.error("Invalid private data!");
            return null;
        }

        mlogger.debug(
                "Private Data obtained: " + OICHelper.bytesToHex(privateData));

        return OICHelper.bytesToHex(privateData);

    }

    /**
     * Gets the current systems network address (IP version 4 or 6)
     *
     * @param useIp6
     *            Specify which IP version to use
     * @return A string containing the network address
     */
    public String getCurrentSystemIpAddress(boolean useIp6) {
        Enumeration<NetworkInterface> networkInterfaces = null;
        Enumeration<InetAddress> inetAddresses = null;
        try {
            networkInterfaces = NetworkInterface.getNetworkInterfaces();
        } catch (SocketException e) {
            e.printStackTrace();
        }

        while (networkInterfaces.hasMoreElements()) {
            NetworkInterface networkInterface = networkInterfaces.nextElement();

            try {
                if (networkInterface.isUp() && !networkInterface.isVirtual()
                        && !networkInterface.isLoopback()) {
                    inetAddresses = networkInterface.getInetAddresses();
                }
            } catch (SocketException e) {
                e.printStackTrace();
            }
        }

        while (inetAddresses.hasMoreElements()) {
            InetAddress inetAddress = inetAddresses.nextElement();

            if (useIp6) {
                if (inetAddress instanceof Inet6Address)
                    return inetAddress.getHostAddress();
            } else {
                if (inetAddress instanceof Inet4Address)
                    return inetAddress.getHostAddress();
            }
        }

        return null;

    }

    /**
     * Get the secured port of the server
     *
     * @return Secured port as Integer
     */
    public int getSecuredPort() {
        return mSecuredPort;
    }

    /**
     * Get the device id of the server
     *
     * @return Device id as String
     */
    public String getDeviceId() {
        return mDeviceId;
    }

    /**
     * Transfers ownership of the DUT server (Sets the value of ownr parameter
     * and set owned: true)
     *
     * @param clientDeviceId
     *            Device id of the client
     * @param serverDeviceId
     *            Device id of the server
     * @param relayIp
     *            Network address of the DTLS relay
     * @param relayDataPort
     *            Data port of the DTLS relay
     * @return OICResponseData object containing the response of the owner
     *         transfer request
     */
    public OICResponseData transferOwnership(String clientDeviceId,
            String serverDeviceId, String relayIp, int relayDataPort) {

        if (clientDeviceId == null || clientDeviceId.isEmpty()) {
            mlogger.error("Null or empty client device id!");
            return null;
        }

        if (serverDeviceId == null || serverDeviceId.isEmpty()) {
            mlogger.error("Null or empty server device id!");
            return null;
        }

        if (relayIp == null || relayIp.isEmpty()) {
            mlogger.error("Null or empty relay ip address!");
            return null;
        }

        if (relayDataPort < 1) {
            mlogger.error("Invalid relay data port number!");
            return null;
        }

        String clientDeviceIdEncoded = new String(
                Base64.encodeBase64(clientDeviceId.getBytes()));
        String serverDeviceIdEncoded = new String(
                Base64.encodeBase64(serverDeviceId.getBytes()));

        String payload = "[{\"rep\":\"{\\\"doxm\\\":{\\\"oxm\\\":[0],\\\"oxmsel\\\":0,\\\"sct\\\":1,\\\"owned\\\":true,\\\"deviceid\\\":\\\""
                + serverDeviceIdEncoded + "\\\",\\\"ownr\\\":\\\""
                + clientDeviceIdEncoded + "\\\"}}\"}]";

        OICResponseData response = mOicClient.sendRequest(Protocol.COAP,
                MessageType.NON, Method.PUT,
                OICHelper.getRandomMessageIdString(),
                OICHelper.createTokenString(), relayIp, relayDataPort,
                "/oic/sec/doxm", "", payload);
        return response;
    }

    /**
     * Terminates the DTLS connection
     */
    public void terminateDtls() {
        ControlMessage response = null;
        response = mController.terminateDtls();
        if (response == null) {
            mlogger.error("Invalid response from DTLS relay!");
            return;
        }
        mlogger.info("DTLS Termination response: " + response.getMessageCode());
    }

    private SecuredDeviceInfo discoverDevice(String filterIp,
            String ownedState) {
        mUnsecuredPort = -1;
        mSecuredPort = -1;

        if (filterIp == null || filterIp.isEmpty()) {
            mlogger.error("Null or empty filter ip address!");
            return null;
        }

        mDutIp = filterIp;

        ArrayList<OICResponseData> responses = discoverDoxmResources(
                ownedState);

        if (responses == null || responses.size() == 0) {
            mlogger.info("No device found with " + ownedState);
            return null;
        }

        OICResponseData dutResponse = filterResponseWithIp(responses, filterIp);

        if (dutResponse == null) {
            mlogger.info("No device found with " + ownedState + " and IP: "
                    + filterIp);
            return null;
        }

        mUnsecuredPort = Integer.parseInt(
                dutResponse.getResponseValue(MessageParameters.srcPort));
        if (mUnsecuredPort < 1) {
            mlogger.error("Unable to find DUT or DUT unsecured port");
            return null;
        }

        mSecuredPort = discoverSecuredPort();
        if (mSecuredPort < 1) {
            mlogger.error("Unable to retrieve DUT secured port");
            return null;
        }

        String deviceIdEncoded = getDeviceIdFromPayload(
                dutResponse.getResponseValue(MessageParameters.payload));

        if (deviceIdEncoded != null)
            mDeviceId = new String(Base64.decodeBase64(deviceIdEncoded));
        else
            mDeviceId = null;

        OTM otm = getOtmFromPayload(
                dutResponse.getResponseValue(MessageParameters.payload));

        if (otm == null || mDeviceId == null) {
            mlogger.debug("OTM or Device Id null!");
            return null;
        }

        return new SecuredDeviceInfo(mDeviceId, otm);
    }

    private ArrayList<OICResponseData> discoverDoxmResources(String query) {
        ArrayList<OICResponseData> responses = mOicClient
                .discoverResource(Protocol.COAP, "/oic/sec/doxm", query);
        return responses;
    }

    private OICResponseData filterResponseWithIp(
            ArrayList<OICResponseData> responses, String ipAddress) {
        if (responses == null)
            return null;

        for (OICResponseData response : responses) {
            if (response.getResponseValue(MessageParameters.srcAddress)
                    .contains(ipAddress)
                    && response.getResponseValue(MessageParameters.payload)
                            .contains("doxm"))
                return response;
        }

        return null;
    }

    private int discoverSecuredPort() {
        OICResponseData response = mOicClient.sendRequest(Protocol.COAP,
                MessageType.NON, Method.GET,
                OICHelper.getRandomMessageIdString(),
                OICHelper.createTokenString(), mDutIp, mUnsecuredPort,
                "/oic/res", "");
        if (response == null) {
            mlogger.error("No response received!");
            return -1;
        }
        String payload = response.getResponseValue(MessageParameters.payload);

        if (payload == null || payload.isEmpty()) {
            mlogger.error("Null of empty payload!");
            return -1;
        }

        int lastIndex = payload.lastIndexOf("}}]}]");
        int firstIndex = payload.lastIndexOf("port\":") + "port\":".length();

        if (lastIndex == -1 || firstIndex == -1) {
            mlogger.error("Payload format mismatch!");
            return -1;
        }

        return Integer.parseInt(payload.substring(firstIndex, lastIndex));
    }

    private String getDeviceIdFromPayload(String payload) {
        String deviceId = null;

        if (payload == null || payload.isEmpty()) {
            mlogger.error("Null or empty payload!");
            return null;
        }

        if (!payload.contains("doxm")) {
            mlogger.error("Payload does not contain doxm");
            return null;
        }

        String endMarker = "\\\",\\\"ownr\\\"";
        String beginMarker = "deviceid\\\":\\\"";

        int lastIndex = payload.lastIndexOf(endMarker);
        int firstIndex = payload.lastIndexOf(beginMarker);

        if (lastIndex < 0 || firstIndex < 0) {
            mlogger.error("Payload format mismatch!");
            return null;
        }

        deviceId = payload.substring(firstIndex + beginMarker.length(),
                lastIndex);

        return deviceId;
    }

    private OTM getOtmFromPayload(String payload) {
        if (payload == null || payload.isEmpty())
            return null;

        String rdpMarker = "\\\"oxm\\\":[0,1]";
        String jwMarker = "\\\"oxm\\\":[0]";

        if (payload.contains(rdpMarker))
            return OTM.RANDOM_PIN;
        else if (payload.contains(jwMarker))
            return OTM.JUST_WORKS;
        else
            return null;
    }

    private void setOwnerTransferMethod(OTM otm, String clientDeviceId,
            String serverDeviceId) {

        String clientDeviceIdEncoded = new String(
                Base64.encodeBase64(clientDeviceId.getBytes()));
        String serverDeviceIdEncoded = new String(
                Base64.encodeBase64(serverDeviceId.getBytes()));

        String payload = null;

        if (otm == OTM.RANDOM_PIN)
            payload = "[{\"rep\":\"{\\\"doxm\\\":{\\\"oxm\\\":[0,1],\\\"oxmsel\\\":1,\\\"sct\\\":1,\\\"owned\\\":false,\\\"deviceid\\\":\\\""
                    + serverDeviceIdEncoded + "\\\",\\\"ownr\\\":\\\""
                    + clientDeviceIdEncoded + "\\\"}}\"}]";
        else if (otm == OTM.JUST_WORKS)
            payload = "[{\"rep\":\"{\\\"doxm\\\":{\\\"oxm\\\":[0],\\\"oxmsel\\\":0,\\\"sct\\\":1,\\\"owned\\\":false,\\\"deviceid\\\":\\\""
                    + serverDeviceIdEncoded + "\\\",\\\"ownr\\\":\\\""
                    + DEVICE_ID_ENCODED_DEFAULT + "\\\"}}\"}]";
        else
            mlogger.error("Invalid OTM");

        mOicClient.sendRequest(Protocol.COAP, MessageType.NON, Method.PUT,
                OICHelper.getRandomMessageIdString(),
                OICHelper.createTokenString(), mDutIp, mUnsecuredPort,
                "/oic/sec/doxm", "", payload);
    }

    private void checkAndConfirmProvisioningStatus() {
        OICResponseData response;

        response = mOicClient.sendRequest(Protocol.COAP, MessageType.NON,
                Method.GET, OICHelper.getRandomMessageIdString(),
                OICHelper.createTokenString(), mDutIp, mUnsecuredPort,
                "/oic/sec/pstat", "");
        if (response == null) {
            mlogger.error("Null response!");
            return;
        }

        response = mOicClient.sendRequest(Protocol.COAP, MessageType.NON,
                Method.PUT, OICHelper.getRandomMessageIdString(),
                OICHelper.createTokenString(), mDutIp, mUnsecuredPort,
                "/oic/sec/pstat", "",
                response.getResponseValue(MessageParameters.payload));

    }

}

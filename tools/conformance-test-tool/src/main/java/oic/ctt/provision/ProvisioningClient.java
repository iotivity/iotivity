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

public class ProvisioningClient {

    private Logger        mlogger        = CTLogger.getInstance();

    private String        mDutIp         = null;
    private int           mUnsecuredPort = -1;
    private int           mSecuredPort   = -1;

    private String        mDeviceId      = null;

    private final String  CRED_FILENAME  = "credentials";

    private OICClient     mOicClient     = new OICClient();
    private ControlClient mController    = new ControlClient();

    public ProvisioningClient() {
        mOicClient.setWaitTime(2);
    }

    public String discoverOwnedDevice(String filterIp) {
        return discoverDevice(filterIp, "Owned=True");
    }

    public String discoverUnownedDevice(String filterIp) {
        return discoverDevice(filterIp, "Owned=False");
    }

    public String findPrivateData(String deviceId) {
        FileReader fileReader = null;
        String line = null;

        try {
            fileReader = new FileReader(CRED_FILENAME);
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }

        if (fileReader == null)
            return null;

        BufferedReader bufferedReader = new BufferedReader(fileReader);

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

    public void storePrivateData(String deviceId, String privateData) {
        PrintWriter writer = null;

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

    public boolean dtlsConnectWithPSK(String privateData, String clientDeviceId,
            String serverDeviceId, String relayIp, int relayControlPort,
            String dutIp, int dutSecuredPort) {

        ControlMessage response = null;

        response = mController.startDtlsWithPvData(relayIp, relayControlPort,
                dutIp, dutSecuredPort,
                CipherSuite.TLS_PSK_WITH_AES_128_CCM_8.getValue(),
                clientDeviceId, serverDeviceId,
                OICHelper.hexStringToByteArray(privateData));

        mlogger.info(
                "DTLS connect with PSK response: " + response.getMessageCode());

        if (response.getMessageCode() == MessageCode.DTLS_HANDSHAKE_SUCCESS)
            return true;

        return false;
    }

    public void configureDeviceForProvisioning(String otm,
            String clientDeviceId, String serverDeviceId) {
        setOwnerTransferMethod(otm, clientDeviceId, serverDeviceId);
        checkAndConfirmProvisioningStatus();
    }

    public String provisionWithRandomPin(String pin, String clientDeviceId,
            String serverDeviceId, String relayIp, int relayControlPort,
            String dutIp, int dutSecuredPort) {

        ControlMessage response = null;
        byte[] privateData = null;

        // configureDeviceForProvisioning("oic.sec.doxm.rdp");

        response = mController.initDtlsWithRndPin(relayIp, relayControlPort,
                dutIp, dutSecuredPort,
                CipherSuite.TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA_256.getValue(),
                clientDeviceId, serverDeviceId, pin);

        if (response.getMessageCode()
                .equals(MessageCode.DTLS_HANDSHAKE_SUCCESS)) {
            mlogger.info("DTLS Handshake Success");
            privateData = response.getPrivateData();
        } else {
            mlogger.error("DTLS Handshake Failed!");
        }

        if (privateData != null) {
            mlogger.debug("Private Data obtained: "
                    + OICHelper.bytesToHex(privateData));
        }

        return OICHelper.bytesToHex(privateData);

    }

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

    public int getSecuredPort() {
        return mSecuredPort;
    }

    public String getDeviceId() {
        return mDeviceId;
    }

    public void transferOwnership(String clientDeviceId, String serverDeviceId,
            String relayIp, int relayDataPort) {

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
    }

    public void terminateDtls() {
        ControlMessage response = null;
        response = mController.terminateDtls();
        mlogger.info("DTLS Termination response: " + response.getMessageCode());
    }

    // temp
    public void checkSecuredMessaging(String relayIp, int relayDataPort) {

        System.out.println(
                "----------Checked secured line with GET - Start----------");
        OICResponseData response = mOicClient.sendRequest(Protocol.COAP,
                MessageType.NON, Method.GET,
                OICHelper.getRandomMessageIdString(),
                OICHelper.createTokenString(), relayIp, relayDataPort,
                "/oic/sec/doxm", "");

        printResponse(response);
        System.out.println(
                "----------Checked secured line with GET - End----------");
    }

    // temp
    private void printResponse(OICResponseData response) {
        if (response == null) {
            System.out.println("No Response Found");
            return;
        }
        System.out.println("Source address: "
                + response.getResponseValue(MessageParameters.srcAddress));
        System.out.println("Response code: "
                + response.getResponseValue(MessageParameters.responseCode));
        System.out.println("Payload: "
                + response.getResponseValue(MessageParameters.payload));
    }

    private String discoverDevice(String filterIp, String ownedState) {
        mUnsecuredPort = -1;
        mSecuredPort = -1;
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

        mDeviceId = getDeviceIdFromPayload(
                dutResponse.getResponseValue(MessageParameters.payload));

        return mDeviceId;
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
        String payload = response.getResponseValue(MessageParameters.payload);
        int lastIndex = payload.lastIndexOf("}}]}]");
        int firstIndex = payload.lastIndexOf("port\":") + "port\":".length();
        return Integer.parseInt(payload.substring(firstIndex, lastIndex));
    }

    private String getDeviceIdFromPayload(String payload) {
        String deviceId = null;

        if (!payload.contains("doxm"))
            return null;

        String endMarker = "\\\",\\\"ownr\\\"";
        String beginMarker = "deviceid\\\":\\\"";

        int lastIndex = payload.lastIndexOf(endMarker);
        int firstIndex = payload.lastIndexOf(beginMarker)
                + beginMarker.length();

        deviceId = payload.substring(firstIndex, lastIndex);

        return deviceId;
    }

    private void setOwnerTransferMethod(String otm, String clientDeviceId,
            String serverDeviceId) {

        String clientDeviceIdEncoded = new String(
                Base64.encodeBase64(clientDeviceId.getBytes()));
        String serverDeviceIdEncoded = new String(
                Base64.encodeBase64(serverDeviceId.getBytes()));

        String payload = "[{\"rep\":\"{\\\"doxm\\\":{\\\"oxm\\\":[0,1],\\\"oxmsel\\\":1,\\\"sct\\\":1,\\\"owned\\\":false,\\\"deviceid\\\":\\\""
                + serverDeviceIdEncoded + "\\\",\\\"ownr\\\":\\\""
                + clientDeviceIdEncoded + "\\\"}}\"}]";

        OICResponseData response = mOicClient.sendRequest(Protocol.COAP,
                MessageType.NON, Method.PUT,
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

        response = mOicClient.sendRequest(Protocol.COAP, MessageType.NON,
                Method.PUT, OICHelper.getRandomMessageIdString(),
                OICHelper.createTokenString(), mDutIp, mUnsecuredPort,
                "/oic/sec/pstat", "",
                response.getResponseValue(MessageParameters.payload));

    }

}

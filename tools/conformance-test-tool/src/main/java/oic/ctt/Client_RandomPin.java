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
package oic.ctt;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.Scanner;
import org.apache.commons.codec.binary.Base64;

import oic.ctt.network.OICClient;
import oic.ctt.network.OICClient.Protocol;
import oic.ctt.network.OICHelper;
import oic.ctt.network.OICHelper.MessageParameters;
import oic.ctt.network.OICHelper.MessageType;
import oic.ctt.network.OICRequestData.Method;
import oic.ctt.network.OICResponseData;
import oic.ctt.network.control.ControlClient;
import oic.ctt.network.control.ControlMessage;
import oic.ctt.network.control.SecurityConstants.CipherSuite;
import oic.ctt.network.control.SecurityConstants.MessageCode;

public class Client_RandomPin {

    private static int           securedPort;
    private static int           unsecuredPort;
    private final static String  DUT_IP                    = "107.109.214.138";
    private final static String  RELAY_IP                  = "107.109.214.138";
    private final static int     CONTROL_PORT              = 12345;
    private final static int     DATA_PORT                 = 12346;

    private final static String  CLIENT_DEVICE_ID          = "adminDeviceUUID0";
    private final static String  SERVER_DEVICE_ID          = "randomPinDevUUID";
    private final static String  CRED_FILENAME             = "credentials";

    private final static String  DEVICE_ID_ENCODED_DEFAULT = "AAAAAAAAAAAAAAAAAAAAAA==";
    private static String        mClientDeviceIdEncoded    = null;
    private static String        mServerDeviceIdEncoded    = null;

    private static OICClient     mOicClient                = new OICClient();
    private static ControlClient mController               = new ControlClient();
    private static String        mDeviceId                 = null;

    public static OICResponseData filterResponseWithIp(
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

    public static void printResponse(OICResponseData response) {
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

    public static void printResponses(ArrayList<OICResponseData> responses) {
        if (responses == null) {
            System.out.println("No Responses Found");
            return;
        }
        for (OICResponseData response : responses) {
            printResponse(response);
        }
    }

    public static ArrayList<OICResponseData> discoverDevices(String query) {
        ArrayList<OICResponseData> responses = mOicClient
                .discoverResource(Protocol.COAP, "/oic/sec/doxm", query);
        return responses;
    }

    public static void setOwnerTransferMethod(String oxmMethod) {
        String payload = "[{\"rep\":\"{\\\"doxm\\\":{\\\"oxm\\\":[0,1],\\\"oxmsel\\\":1,\\\"sct\\\":1,\\\"owned\\\":false,\\\"deviceid\\\":\\\""
                + mServerDeviceIdEncoded + "\\\",\\\"ownr\\\":\\\""
                + mClientDeviceIdEncoded + "\\\"}}\"}]";
        System.out.println("Put request");
        OICResponseData response = mOicClient.sendRequest(Protocol.COAP,
                MessageType.NON, Method.PUT,
                OICHelper.getRandomMessageIdString(),
                OICHelper.createTokenString(), DUT_IP, unsecuredPort,
                "/oic/sec/doxm", "", payload);
        printResponse(response);
        System.out.println(
                "-----------------------------------------------------");
    }

    public static void checkAndConfirmProvisioningStatus() {
        OICResponseData response;
        System.out.println("Get request");
        response = mOicClient.sendRequest(Protocol.COAP, MessageType.NON,
                Method.GET, OICHelper.getRandomMessageIdString(),
                OICHelper.createTokenString(), DUT_IP, unsecuredPort,
                "/oic/sec/pstat", "");
        printResponse(response);
        System.out.println(
                "-----------------------------------------------------");

        System.out.println("Put request");
        response = mOicClient.sendRequest(Protocol.COAP, MessageType.NON,
                Method.PUT, OICHelper.getRandomMessageIdString(),
                OICHelper.createTokenString(), DUT_IP, unsecuredPort,
                "/oic/sec/pstat", "",
                response.getResponseValue(MessageParameters.payload));
        printResponse(response);
        System.out.println(
                "-----------------------------------------------------");
    }

    public static void configureDeviceForProvisioning(
            String ownerTransferMethod) {
        setOwnerTransferMethod(ownerTransferMethod);
        checkAndConfirmProvisioningStatus(); // optional step
    }

    public static String getDeviceIdFromPayload(String payload) {
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

    public static int getSecuredPort() {
        OICResponseData response = mOicClient.sendRequest(Protocol.COAP,
                MessageType.NON, Method.GET,
                OICHelper.getRandomMessageIdString(),
                OICHelper.createTokenString(), DUT_IP, unsecuredPort,
                "/oic/res", "");
        String payload = response.getResponseValue(MessageParameters.payload);
        int lastIndex = payload.lastIndexOf("}}]}]");
        int firstIndex = payload.lastIndexOf("port\":") + "port\":".length();
        return Integer.parseInt(payload.substring(firstIndex, lastIndex));

    }

    public static void transferOwnership() {
        System.out.println("----------Transfer Ownership - Start----------");
        String payload = "[{\"rep\":\"{\\\"doxm\\\":{\\\"oxm\\\":[0],\\\"oxmsel\\\":0,\\\"sct\\\":1,\\\"owned\\\":true,\\\"deviceid\\\":\\\""
                + mServerDeviceIdEncoded + "\\\",\\\"ownr\\\":\\\""
                + mClientDeviceIdEncoded + "\\\"}}\"}]";
        OICResponseData response = mOicClient.sendRequest(Protocol.COAP,
                MessageType.NON, Method.PUT,
                OICHelper.getRandomMessageIdString(),
                OICHelper.createTokenString(), RELAY_IP, DATA_PORT,
                "/oic/sec/doxm", "", payload);
        printResponse(response);
        System.out.println("----------Transfer Ownership - End----------");
    }

    private static byte[] findPrivateData(String deviceId) {
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
            return OICHelper.hexStringToByteArray(line.split(":")[1]);

        return null;
    }

    private static void storePrivateData(String deviceId, byte[] privateData) {
        PrintWriter writer = null;

        try {
            writer = new PrintWriter(CRED_FILENAME, "UTF-8");
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        }
        if (writer == null) {
            System.out.println("Unable to create file");
            System.exit(0);
        }
        writer.println(deviceId + ":" + OICHelper.bytesToHex(privateData));
        writer.close();
    }

    public static void checkSecuredMessaging() {

        System.out.println(
                "----------Checked secured line with GET - Start----------");
        OICResponseData response = mOicClient.sendRequest(Protocol.COAP,
                MessageType.NON, Method.GET,
                OICHelper.getRandomMessageIdString(),
                OICHelper.createTokenString(), RELAY_IP, DATA_PORT,
                "/oic/sec/doxm", "");

        printResponse(response);
        System.out.println(
                "----------Checked secured line with GET - End----------");
    }

    public static void discoverPortsAndDeviceId(String ipAddress,
            String ownedState) {
        System.out.println("----------Discovering " + ownedState
                + " devices - Start----------");
        unsecuredPort = -1;
        securedPort = -1;

        ArrayList<OICResponseData> responses = discoverDevices(ownedState);

        if (responses.size() == 0) {
            System.out.println("No device found with " + ownedState);
            return;
        }

        OICResponseData dutResponse = filterResponseWithIp(responses,
                ipAddress);

        printResponse(dutResponse);

        if (dutResponse == null) {
            System.out.println("No device found with " + ownedState
                    + " and IP: " + ipAddress);
            return;
        }

        unsecuredPort = Integer.parseInt(
                dutResponse.getResponseValue(MessageParameters.srcPort));
        if (unsecuredPort < 1) {
            System.out.println("Unable to find DUT or DUT unsecured port");
            System.exit(0);
        }

        securedPort = getSecuredPort();
        if (securedPort < 1) {
            System.out.println("Unable to retrieve DUT secured port");
            System.exit(0);
        }

        mDeviceId = getDeviceIdFromPayload(
                dutResponse.getResponseValue(MessageParameters.payload));

        System.out.println("----------Discovering " + ownedState
                + " devices - End----------");
    }

    private static byte[] provisionWithRandomPin() {
        System.out.println(
                "----------Provision with random pin - Start----------");
        String pin = null;
        ControlMessage response = null;
        byte[] privateData = null;

        configureDeviceForProvisioning("oic.sec.doxm.rdp");

        System.out.println("Enter pin: ");
        pin = new Scanner(System.in).nextLine();

        response = mController.initDtlsWithRndPin(RELAY_IP, CONTROL_PORT,
                DUT_IP, securedPort,
                CipherSuite.TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA_256.getValue(),
                CLIENT_DEVICE_ID, SERVER_DEVICE_ID, pin);

        if (response.getMessageCode()
                .equals(MessageCode.DTLS_HANDSHAKE_SUCCESS)) {
            System.out.println("DTLS Handshake Success");
            privateData = response.getPrivateData();
        } else {
            System.out.println("DTLS Handshake Failed!");
        }

        if (privateData != null) {
            System.out.println("Private Data obtained: "
                    + OICHelper.bytesToHex(privateData));
        }

        System.out
                .println("----------Provision with random pin - End----------");
        return privateData;

    }

    private static void initWithPSK(byte[] privateData) {
        System.out.println("----------DTLS connect with PSK - Start----------");
        ControlMessage response = null;
        response = mController.startDtlsWithPvData(RELAY_IP, CONTROL_PORT,
                DUT_IP, securedPort,
                CipherSuite.TLS_PSK_WITH_AES_128_CCM_8.getValue(),
                CLIENT_DEVICE_ID, SERVER_DEVICE_ID, privateData);
        System.out.println(
                "DTLS connect with PSK response: " + response.getMessageCode());
        System.out.println("----------DTLS connect with PSK - End----------");
    }

    public static void terminateDtls() {
        ControlMessage response = null;
        System.out.println("DTLS Termination request");
        response = mController.terminateDtls();
        System.out.println(
                "DTLS Termination response: " + response.getMessageCode());
    }

    public static void main(String[] args) {

        /**
         * 1. Look for owned devices. 2. If there is one, look for stored
         * credentials with the matching device id. 3. If a matching credential
         * is found, start DTLS connection with PSK. Procedure Ends here. 4. If
         * 2 or 3 is false, look for UnOwned devices with expected IP. If there
         * is one, start provisioning from step 5. 5. Query with the device on
         * which provisioning method it supports. 6. Provision with the
         * supported method. 7. Store the generated private data in a credential
         * file as deviceId:credential pair. 8. Perform the ownership transfer
         * and change the device state to Owned.
         **/

        mClientDeviceIdEncoded = new String(
                Base64.encodeBase64(CLIENT_DEVICE_ID.getBytes()));
        mServerDeviceIdEncoded = new String(
                Base64.encodeBase64(SERVER_DEVICE_ID.getBytes()));

        byte[] privateData = null;
        mOicClient.setWaitTime(2);

        // Look for owned devices
        discoverPortsAndDeviceId(DUT_IP, "Owned=True");

        if (mDeviceId != null) {
            // Owned device discovered with given DUT IP
            System.out.println("Found owned device with given DUT IP");
            System.out.println("Device ID: " + mDeviceId);

            privateData = findPrivateData(mDeviceId);

            if (privateData != null) {
                System.out
                        .println("Found stored credential of the owned device");
                initWithPSK(privateData);
            } else {
                System.out.println("Credential for the owned device not found");
            }

        } else {
            // Owned device not found for given DUT IP
            System.out.println("No owned device found");
            mDeviceId = null;
            discoverPortsAndDeviceId(DUT_IP, "Owned=False");

            if (mDeviceId != null) {
                // UnownedDevice with given DUT IP found. Provision it.
                System.out.println("UnOwned device found");

                privateData = provisionWithRandomPin();

                storePrivateData(mDeviceId, privateData);

                transferOwnership();
            }
        }

        checkSecuredMessaging();

        terminateDtls();
    }
}

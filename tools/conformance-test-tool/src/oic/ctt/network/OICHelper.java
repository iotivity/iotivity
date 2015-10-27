// ******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
//
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

package oic.ctt.network;

import java.util.ArrayList;
import java.util.Random;
import java.util.regex.Pattern;

import oic.ctt.formatter.IotivityKey;

/**
 * Helper class. Provides miscellaneous utility API's for some common tasks.
 */
public class OICHelper {

    /** Defines various Request/Response parameter keys */
    public static enum MessageParameters {
        srcAddress, srcPort, requestCode, responseCode, payload, token, destAddress, destPort, msgType, mId, seqNumber, observeFlag;
    }

    /** QoS Definition */
    public static enum MessageType {
        NON, CON, ACK, RST;
    }

    public static final boolean  DEFAULT_MESSAGE_CONFIRMATION_TYPE = false;
    public static final String   DEFAULT_QUERY                     = "";
    public static final String   DEFAULT_PAYLOAD                   = "";
    public static final String   DEFAULT_MULTICAST_IPv4            = "224.0.1.187";
    public static final String   DEFAULT_MULTICAST_IPv6            = "FF02::FD";
    public static final int      DEFAULT_COAP_PORT                 = 5683;
    public static final int      DEFAULT_TOKEN_LENGTH              = 8;
    public static final int      MESSAGE_ID_MAX                    = 65535;
    public final static int      MESSAGE_ID_MIN                    = 0;
    private static final char[]  HEX_ARRAY                         = "0123456789abcdef"
                                                                           .toCharArray();

    private static String        mDefaultUri                       = "/oic/res";
    private static String        mDefaultOic                       = IotivityKey.ROOT
                                                                           .toString();

    private static final Pattern PATTERN                           = Pattern
                                                                           .compile("^(([01]?\\d\\d?|2[0-4]\\d|25[0-5])\\.){3}([01]?\\d\\d?|2[0-4]\\d|25[0-5])$");

    /**
     * Sets default value for OIC
     * 
     * @param oic
     *            OIC value as string
     */
    public static void setDefaultOic(String oic) {
        mDefaultOic = oic;
    }

    /**
     * Set default value for URI
     * 
     * @param uri
     *            URI value as string
     */
    public static void setDefaultUri(String uri) {
        mDefaultUri = uri;
    }

    /**
     * Gets the default value for URI
     * 
     * @return the default uri
     * 
     */
    public static String getDefaultUri() {
        return mDefaultUri;
    }

    /**
     * Validates ipv4 address
     * 
     * @param ip
     *            the ip address to be validated
     * @return true if valid, false if invalid
     */
    public static boolean validateIpAddress(final String ip) {
        if (ip == null)
            return false;
        return PATTERN.matcher(ip).matches();
    }

    /**
     * Converts string of hexadecimal values to byte data
     * 
     * @param s
     *            String of HEX values
     * @return byte data array
     */
    public static byte[] hexStringToByteArray(String s) {
        int len = s.length();
        byte[] data = new byte[len / 2];
        for (int i = 0; i < len; i += 2) {
            data[i / 2] = (byte) ((Character.digit(s.charAt(i), 16) << 4) + Character
                    .digit(s.charAt(i + 1), 16));
        }
        return data;
    }

    /**
     * Converts byte data to string of hexadecimals
     * 
     * @param bytes
     *            byte data array
     * @return String of HEX values
     */
    public static String bytesToHex(byte[] bytes) {
        char[] hexChars = new char[bytes.length * 2];
        for (int j = 0; j < bytes.length; j++) {
            int v = bytes[j] & 0xFF;
            hexChars[j * 2] = HEX_ARRAY[v >>> 4];
            hexChars[j * 2 + 1] = HEX_ARRAY[v & 0x0F];
        }
        return new String(hexChars);
    }

    /**
     * Makes a complete URI by concatenation of prefix, ip & port
     * 
     * @param prefix
     *            URI prefix, for example, coap:// or http://
     * @param ip
     *            IP Address
     * @param port
     *            Port number
     * @return The complete URI string
     */
    public static String makeUriPath(String prefix, String ip, int port) {
        return prefix + ip + ":" + Integer.toString(port);
    }

    /**
     * Parses and divides URI into segments (separated by "/")
     * 
     * @param uriPath
     *            The URI to be segmented
     * @return An ArrayList of String segments from the given URI
     */
    public static ArrayList<String> getUriParts(String uriPath) {
        ArrayList<String> uriParts = new ArrayList<String>();

        String[] uriPaths = uriPath.split("/");
        for (String uriPart : uriPaths) {
            if (uriPart.length() > 0) {
                uriParts.add(uriPart);
            }
        }

        return uriParts;
    }

    /**
     * Parses and divides Query into segments (separated by "&")
     * 
     * @param query
     *            The Query to be segmented
     * @return An ArrayList of String segments from the given Query
     */
    public static ArrayList<String> getQueryParts(String query) {
        ArrayList<String> queryParts = new ArrayList<String>();

        if (query.contains("&")) {
            String[] queries = query.split("&");
            for (String queryPart : queries) {
                if (queryPart.length() > 0) {
                    queryParts.add(queryPart);
                }
            }

        } else {
            queryParts.add(query);
        }

        return queryParts;
    }

    /**
     * Generates a random string of HEX numbers of the given length
     * 
     * @param numchars
     *            Length of the string
     * @return Random generated string of Hex numbers
     */
    private static String getRandomHexString(int numchars) {
        Random r = new Random();
        StringBuffer sb = new StringBuffer();

        while (sb.length() < numchars) {
            sb.append(Integer.toHexString(r.nextInt()));
        }

        return sb.toString().substring(0, numchars);
    }

    /**
     * Generates a random Token of the given length
     * 
     * @param length
     *            Length of the Token
     * @return Token in byte array format
     */
    public static byte[] createToken(int length) {
        return hexStringToByteArray(getRandomHexString(length));
    }

    public static String createTokenString() {
        return getRandomHexString(OICHelper.DEFAULT_TOKEN_LENGTH * 2);
    }

    public static String getRandomMessageIdString() {
        Random random = new Random();
        return Integer.toString(random.nextInt(OICHelper.MESSAGE_ID_MAX + 1));
    }

    public static int getRandomMessageId() {
        Random random = new Random();
        return random.nextInt(OICHelper.MESSAGE_ID_MAX + 1);
    }
}

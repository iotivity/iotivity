/*
 * //******************************************************************
 * //
 * // Copyright 2016 Samsung Electronics All Rights Reserved.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 * //
 * // Licensed under the Apache License, Version 2.0 (the "License");
 * // you may not use this file except in compliance with the License.
 * // You may obtain a copy of the License at
 * //
 * //      http://www.apache.org/licenses/LICENSE-2.0
 * //
 * // Unless required by applicable law or agreed to in writing, software
 * // distributed under the License is distributed on an "AS IS" BASIS,
 * // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * // See the License for the specific language governing permissions and
 * // limitations under the License.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */
package org.iotivity.cloud.base.protocols.coap;

import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;

import org.iotivity.cloud.base.protocols.coap.enums.CoapOption;
import org.iotivity.cloud.util.Cbor;
import org.iotivity.cloud.util.Logger;

public class CoapMessage {

    private int            tokenLength    = 0;
    protected int          code           = 0;
    private byte[]         token          = null;

    private byte[]         payload        = null;

    // Option fields
    protected List<byte[]> if_match       = null;
    protected byte[]       uri_host       = null;
    protected List<byte[]> etag           = null;
    protected boolean      if_none_match  = false;
    protected byte[]       uri_port       = null;
    protected List<byte[]> location_path  = null;
    protected List<byte[]> uri_path       = null;
    protected byte[]       content_format = null;
    protected byte[]       max_age        = null;
    protected List<byte[]> uri_query      = null;
    protected byte[]       accept         = null;
    protected List<byte[]> location_query = null;
    protected byte[]       proxy_uri      = null;
    protected byte[]       proxy_scheme   = null;
    protected byte[]       size1          = null;
    protected byte[]       observe        = null;

    public CoapMessage() {
    }

    public int getTokenLength() {
        return this.tokenLength;
    }

    public int getCode() {
        return code;
    }

    public byte[] getToken() {
        return token;
    }

    public void setToken(byte[] token) {
        this.token = token;
        this.tokenLength = token.length;
    }

    public int getPayloadSize() {
        return payload == null ? 0 : payload.length;
    }

    public byte[] getPayload() {
        return payload;
    }

    public void setPayload(byte[] payload) {
        this.payload = payload;
    }

    public void addOption(int optnum, byte[] value) {
        switch (optnum) {
            // IF_MATCH
            case 1:
                if (if_match == null) {
                    if_match = new ArrayList<byte[]>();
                }
                if_match.add(value);
                break;

            // URI_HOST
            case 3:
                uri_host = value;
                break;

            // ETAG
            case 4:
                if (etag == null) {
                    etag = new ArrayList<byte[]>();
                }
                etag.add(value);
                break;

            // IF_NONE_MATCH
            case 5:
                if_none_match = true;
                break;

            // URI_PORT
            case 7:
                uri_port = value;
                break;

            // LOCATION_PATH
            case 8:
                if (location_path == null) {
                    location_path = new ArrayList<byte[]>();
                }
                location_path.add(value);
                break;

            // URI_PATH
            case 11:
                if (uri_path == null) {
                    uri_path = new ArrayList<byte[]>();
                }
                uri_path.add(value);
                break;

            // CONTENT_FORMAT
            case 12:
                content_format = value;
                break;

            // MAX_AGE
            case 14:
                max_age = value;
                break;

            // URI_QUERY
            case 15:
                if (uri_query == null) {
                    uri_query = new ArrayList<byte[]>();
                }
                uri_query.add(value);
                break;

            // ACCEPT
            case 17:
                accept = value;
                break;

            // LOCATION_QUERY
            case 20:
                if (location_query == null) {
                    location_query = new ArrayList<byte[]>();
                }
                location_query.add(value);
                break;

            // PROXY_URI
            case 35:
                proxy_uri = value;
                break;

            // PROXY_SCHEME
            case 39:
                proxy_scheme = value;
                break;

            // SIZE1
            case 60:
                size1 = value;
                break;

            // OBSERVE
            case 6:
                observe = value;
                break;
        }
    }

    public List<byte[]> getOption(int optnum) {
        switch (optnum) {
            // IF_MATCH
            case 1:
                return if_match;

            // URI_HOST
            case 3:
                return uri_host != null ? Arrays.asList(uri_host) : null;

            // ETAG
            case 4:
                return etag;

            // IF_NONE_MATCH
            case 5:
                return if_none_match == true ? new ArrayList<byte[]>() : null;

            // URI_PORT
            case 7:
                return uri_port != null ? Arrays.asList(uri_port) : null;

            // LOCATION_PATH
            case 8:
                return location_path;

            // URI_PATH
            case 11:
                return uri_path;

            // CONTENT_FORMAT
            case 12:
                return content_format != null ? Arrays.asList(content_format)
                        : null;

            // MAX_AGE
            case 14:
                return max_age != null ? Arrays.asList(max_age) : null;

            // URI_QUERY
            case 15:
                return uri_query;

            // ACCEPT
            case 17:
                return accept != null ? Arrays.asList(accept) : null;

            // LOCATION_QUERY
            case 20:
                return location_query;

            // PROXY_URI
            case 35:
                return proxy_uri != null ? Arrays.asList(proxy_uri) : null;

            // PROXY_SCHEME
            case 39:
                return proxy_scheme != null ? Arrays.asList(proxy_scheme)
                        : null;

            // SIZE1
            case 60:
                return size1 != null ? Arrays.asList(size1) : null;

            // OBSERVE
            case 6:
                return observe != null ? Arrays.asList(observe) : null;
        }

        return null;
    }

    public String getTokenString() {
        StringBuffer strBuffer = new StringBuffer(token == null ? "null" : "");
        if (token != null)
            for (byte b : token)
                strBuffer.append(String.format("%02x", b & 0xff)); // hexadecimal(16)
        return strBuffer.toString();
    }

    public String getContentFormatString() {
        List<byte[]> contentFormatList = getOption(
                CoapOption.CONTENT_FORMAT.getvalue());
        byte[] contentFormat = null;
        if (contentFormatList != null && !contentFormatList.isEmpty())
            contentFormat = contentFormatList.get(0);
        StringBuffer strBuffer = new StringBuffer(
                contentFormat == null ? "null" : "");
        if (contentFormat != null)
            for (byte b : contentFormat)
                strBuffer.append(String.format("%02d", b & 0xff)); // decimal(10)
        return strBuffer.toString();
    }

    public String getPayloadString() {
        if (payload == null)
            return "";
        return new String(payload, Charset.forName("UTF-8"));
    }

    public String decodeDeviceId() {
        Cbor<ArrayList<Object>> cbor = new Cbor<ArrayList<Object>>();
        ArrayList<Object> decodedPayload = null;

        if (payload == null) {
            throw new IllegalArgumentException("payload is null");
        }

        else {
            decodedPayload = cbor.parsePayloadFromCbor(payload,
                    ArrayList.class);
            String deviceId = null;
            if (decodedPayload != null) {
                HashMap<Object, Object> tags = (HashMap<Object, Object>) decodedPayload
                        .get(0);

                deviceId = tags.get("di").toString();

                if (deviceId == null) {
                    throw new IllegalArgumentException("deviceId is null");
                }

                Logger.i("deviceId : " + deviceId);
            }
            return deviceId;
        }
    }
}

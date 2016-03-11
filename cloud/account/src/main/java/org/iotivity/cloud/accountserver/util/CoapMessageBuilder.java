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
package org.iotivity.cloud.accountserver.util;

import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;

import org.iotivity.cloud.base.protocols.coap.CoapRequest;
import org.iotivity.cloud.base.protocols.coap.CoapResponse;
import org.iotivity.cloud.base.protocols.coap.enums.CoapMethod;
import org.iotivity.cloud.base.protocols.coap.enums.CoapOption;
import org.iotivity.cloud.base.protocols.coap.enums.CoapStatus;

/**
 * 
 * This class provides a set of APIs to build build data of CoAP request and
 * response type.
 * 
 */
public class CoapMessageBuilder {

    public static final int APPLICATION_JSON = 50;

    /**
     * API for building data of CoAP response type without payload.
     * 
     * @param token
     *            token
     * @param status
     *            response status
     * @return CoapResponse - data of CoAP response type
     */
    public CoapResponse buildCoapResponse(byte[] token, CoapStatus status) {

        return buildCoapResponse(token, null, status);
    }

    /**
     * API for building data of CoAP response type with payload.
     * 
     * @param token
     *            token
     * @param jsonString
     *            payload data
     * @param status
     *            response status
     * @return CoapResponse - data of CoAP response type
     */
    public CoapResponse buildCoapResponse(byte[] token, String jsonString,
            CoapStatus status) {

        CoapResponse coapResponse = new CoapResponse(status);

        coapResponse.setToken(token);

        byte[] bytes = ByteBuffer.allocate(4)
                .putInt(CoapMessageBuilder.APPLICATION_JSON).array();
        coapResponse.addOption(CoapOption.CONTENT_FORMAT.getvalue(), bytes);

        if (jsonString != null)
            coapResponse
                    .setPayload(jsonString.getBytes(StandardCharsets.UTF_8));

        return coapResponse;
    }

    /**
     * API for building data of CoAP requeset type with payload.
     * 
     * @param token
     *            token
     * @param jsonString
     *            payload data
     * @return CoapRequest - data of CoAP request type
     */
    public CoapRequest buildCoapRequest(byte[] token, String jsonString) {

        CoapRequest coapRequest = new CoapRequest(CoapMethod.GET);

        coapRequest.setToken(token);

        byte[] bytes = ByteBuffer.allocate(4)
                .putInt(CoapMessageBuilder.APPLICATION_JSON).array();
        coapRequest.addOption(CoapOption.CONTENT_FORMAT.getvalue(), bytes);

        coapRequest.setPayload(jsonString.getBytes(StandardCharsets.UTF_8));

        return coapRequest;
    }

    /*
     * 
     * public String getJsonAuthServerResponse() {
     * 
     * return "{ \"" + OAuthConstant.AUTH_SERVER + "\" : \"" +
     * OAuthConstant.AUTH_SERVER_GITHUB + "\" }"; }
     * 
     * public String getJsonAuthInfoResponse() {
     * 
     * return "\n{\n" + " \"" + OAuthConstant.AUTH_ADDRESS + "\" : \"" +
     * OAuthConstant.GITHUB_ADDRESS + "\",\n" + " \"" +
     * OAuthConstant.AUTH_RESPONSE_TYPE + "\" : \"" +
     * OAuthConstant.AUTH_CODE_VALUE + "\",\n" + " \"" +
     * OAuthConstant.AUTH_CLIENT_ID + "\" : \"" + OAuthConstant.GITHUB_CLIENT_ID
     * + "\",\n" + " \"" + OAuthConstant.AUTH_REDIRECT_URI + "\" : \"" +
     * OAuthConstant.GITHUB_REDIRECT_URL + "\"" + "\n}"; }
     * 
     * public String getJsonAuthQueryRequest(String auth_server, String
     * auth_code, String auth_di) {
     * 
     * return "\n{\n" + " \"" + OAuthConstant.AUTH_TYPE + "\" : \"" +
     * OAuthConstant.AUTH_TYPE_QUERY + "\",\n" + " \"" +
     * OAuthConstant.AUTH_SERVER + "\" : \"" + auth_server + "\",\n" + " \"" +
     * OAuthConstant.AUTH_CODE + "\" : \"" + auth_code + "\",\n" + " \"" +
     * OAuthConstant.AUTH_DEVICE_ID + "\" : \"" + auth_di + "\"" + "\n}"; }
     */
}

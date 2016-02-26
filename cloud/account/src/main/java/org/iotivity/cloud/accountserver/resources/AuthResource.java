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
package org.iotivity.cloud.accountserver.resources;

import java.util.HashMap;
import java.util.List;

import org.iotivity.cloud.accountserver.AccountServerManager;
import org.iotivity.cloud.accountserver.Const;
import org.iotivity.cloud.accountserver.util.CoapMessageBuilder;
import org.iotivity.cloud.accountserver.util.JSONUtil;
import org.iotivity.cloud.base.Resource;
import org.iotivity.cloud.base.protocols.coap.CoapRequest;
import org.iotivity.cloud.base.protocols.coap.CoapResponse;
import org.iotivity.cloud.base.protocols.coap.enums.CoapMethod;
import org.iotivity.cloud.base.protocols.coap.enums.CoapStatus;
import org.iotivity.cloud.util.Logger;

import io.netty.channel.ChannelHandlerContext;

/**
 *
 * This class provides a set of APIs to register account information of
 * authorized user.
 *
 */
public class AuthResource extends Resource {

    public AuthResource() {
        setUri(Const.AUTH_URI);
    }

    @Override
    public void onRequestReceived(ChannelHandlerContext ctx, CoapRequest request) {

        Logger.d("AuthResource IN");

        if (ctx == null || request == null) {
            Logger.d("ctx or request msg is null");
        }

        else {
            CoapMethod method = request.getRequestMethod();

            switch (method) {
                case POST:
                    try {
                        handlePostRequest(ctx, request);
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                    break;

                default:
                    Logger.w("method(" + method + ") is not supported");
                    break;
            }
        }
    }

    /**
     * API for handling POST message
     * 
     * @param ctx
     *            ChannelHandlerContext of request message
     * @param request
     *            CoAP request message
     * @throws Exception
     */
    private void handlePostRequest(ChannelHandlerContext ctx,
            CoapRequest request) throws Exception {

        String reqType = extractQuery(request, Const.REQ_TYPE);

        if (reqType == null)
            throw new IllegalArgumentException("request type is null in query!");

        CoapResponse response = null;

        switch (reqType) {
            case Const.TYPE_REGISTER:
                response = handleRegisterRequest(request);
                break;
            case Const.TYPE_LOGIN:
                response = handleLoginRequest(request);
                break;
            default:
                throw new IllegalArgumentException(
                        "request type is not supported");
        }

        ctx.write(response);
    }

    private CoapResponse handleLoginRequest(CoapRequest request) {

        String payload = request.getPayloadString();

        JSONUtil util = new JSONUtil();
        String sessionCode = util
                .parseJSON(payload, Const.REQUEST_SESSION_CODE);

        Logger.d("sessionCode: " + sessionCode);

        AccountServerManager oauthServerManager = new AccountServerManager();
        String userId = oauthServerManager.requestUserId(sessionCode);
        Logger.d("userId: " + userId);

        CoapMessageBuilder responseMessage = new CoapMessageBuilder();
        CoapResponse coapResponse = null;

        if (userId != null) {

            ResponseObject response = new ResponseObject();
            response.setUserId(userId);

            String responseJson = convertLoginResponseToJson(response);
            Logger.d("responseJson: " + responseJson);

            coapResponse = responseMessage.buildCoapResponse(
                    request.getToken(), responseJson, CoapStatus.CREATED);

        } else {

            coapResponse = responseMessage.buildCoapResponse(
                    request.getToken(), CoapStatus.INTERNAL_SERVER_ERROR);

        }

        return coapResponse;
    }

    private CoapResponse handleRegisterRequest(CoapRequest request) {

        String payload = request.getPayloadString();

        JSONUtil util = new JSONUtil();
        String authCode = util.parseJSON(payload, Const.REQUEST_AUTH_CODE);
        String authServer = util.parseJSON(payload, Const.REQUEST_AUTH_SERVER);

        Logger.d("authCode: " + authCode + ", authServer: " + authServer);

        AccountServerManager oauthServerManager = new AccountServerManager();

        String userId = oauthServerManager.requestUserId(authCode, authServer);
        String sessionCode = oauthServerManager.registerUserAccount(userId);

        Logger.d("userId: " + userId + ", sessionCode: " + sessionCode);

        CoapMessageBuilder responseMessage = new CoapMessageBuilder();
        CoapResponse coapResponse = null;

        if (userId != null && sessionCode != null) {

            ResponseObject response = new ResponseObject();
            response.setSessionCode(sessionCode);
            response.setUserId(userId);

            String responseJson = convertRegisterResponseToJson(response);
            Logger.d("responseJson: " + responseJson);

            coapResponse = responseMessage.buildCoapResponse(
                    request.getToken(), responseJson, CoapStatus.CREATED);

        } else {

            coapResponse = responseMessage.buildCoapResponse(
                    request.getToken(), CoapStatus.UNAUTHORIZED);

        }

        return coapResponse;
    }

    private String convertRegisterResponseToJson(ResponseObject response) {

        HashMap<Object, Object> responseMap = new HashMap<Object, Object>();

        String sessionCode = response.getSessionCode();
        String userId = response.getUserId();

        if (userId != null)
            responseMap.put(Const.RESPONSE_USER_ID, userId);

        if (sessionCode != null)
            responseMap.put(Const.RESPONSE_SESSION_CODE, sessionCode);

        JSONUtil jsonUtil = new JSONUtil();
        String responseJson = jsonUtil.writeJSON(responseMap);

        return responseJson;
    }

    private String convertLoginResponseToJson(ResponseObject response) {

        HashMap<Object, Object> responseMap = new HashMap<Object, Object>();

        String userId = response.getUserId();

        if (userId != null)
            responseMap.put(Const.RESPONSE_USER_ID, userId);

        JSONUtil jsonUtil = new JSONUtil();
        String responseJson = jsonUtil.writeJSON(responseMap);

        return responseJson;
    }

    private String extractQuery(CoapRequest request, String key) {

        String value = null;

        List<String> Segments = request.getUriQuerySegments();

        for (String s : Segments) {

            String pair[] = s.split("=");

            if (pair[0].equals(key)) {

                value = pair[1];
            }
        }

        return value;
    }

    /*
     * private static String getPayloadString(byte[] payload) {
     * 
     * if (payload == null) return "";
     * 
     * return new String(payload, Charset.forName("UTF-8")); }
     */

}

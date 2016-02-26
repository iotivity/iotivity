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

import java.util.ArrayList;
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
 * This class provides a set of APIs to handle requests for publishing and
 * finding resources.
 *
 */
public class AccountResource extends Resource {

    public AccountResource() {
        setUri(Const.ACCOUNT_URI);
    }

    @Override
    public void onRequestReceived(ChannelHandlerContext ctx,
            CoapRequest request) {

        Logger.d("AccountResource IN");

        if (ctx == null || request == null) {
            Logger.d("ctx or request msg is null");
        }

        else {
            CoapMethod method = request.getRequestMethod();

            switch (method) {
                case GET:
                    try {
                        handleGetRequest(ctx, request);
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                    break;

                case POST:
                    try {
                        handlePostRequest(ctx, request);
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                    break;

                default:
                    Logger.w("method[" + method + "] is not supported");
                    break;
            }
        }
    }

    /**
     * API for handling GET message
     * 
     * @param ctx
     *            ChannelHandlerContext of request message
     * @param request
     *            CoAP request message
     * @throws Exception
     */
    private void handleGetRequest(ChannelHandlerContext ctx,
            CoapRequest request) throws Exception {

        String reqType = extractQuery(request, Const.REQ_TYPE);

        if (reqType == null)
            throw new IllegalArgumentException(
                    "request type is null in query!");

        CoapResponse response = null;

        switch (reqType) {

            case Const.TYPE_FIND:
                response = handleFindRequest(request);
                break;
            default:
                Logger.w("reqType[" + reqType + "] is not supported");
        }

        ctx.write(response);

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
            throw new IllegalArgumentException(
                    "request type is null in query!");

        CoapResponse response = null;

        switch (reqType) {
            case Const.TYPE_PUBLISH:
                response = handlePublishRequest(request);
                break;
            default:
                throw new IllegalArgumentException(
                        "request type is not supported");
        }

        ctx.write(response);
    }

    private CoapResponse handlePublishRequest(CoapRequest request) {

        String payload = request.getPayloadString();

        JSONUtil util = new JSONUtil();
        String userId = util.parseJSON(payload, Const.REQUEST_USER_ID);
        String deviceId = util.parseJSON(payload, Const.REQUEST_DEVICE_ID);

        Logger.d("userId: " + userId + ", deviceId: " + deviceId);

        AccountServerManager oauthServerManager = new AccountServerManager();
        Boolean status = oauthServerManager.registerUserAccount(userId,
                deviceId);

        Logger.d("status : " + status);

        CoapMessageBuilder responseMessage = new CoapMessageBuilder();
        CoapResponse coapResponse = null;

        if (status) {
            coapResponse = responseMessage.buildCoapResponse(request.getToken(),
                    CoapStatus.CREATED);
        } else {
            coapResponse = responseMessage.buildCoapResponse(request.getToken(),
                    CoapStatus.INTERNAL_SERVER_ERROR);
        }

        return coapResponse;
    }

    private CoapResponse handleFindRequest(CoapRequest request) {

        String payload = request.getPayloadString();
        // String payload = getPayloadString(request.getPayload());

        JSONUtil util = new JSONUtil();
        String userId = util.parseJSON(payload, Const.REQUEST_USER_ID);

        Logger.d("userId: " + userId);

        AccountServerManager oauthServerManager = new AccountServerManager();
        ArrayList<String> deviceList = oauthServerManager
                .requestAccountDevices(userId);

        ResponseObject response = new ResponseObject();
        response.setDeviceList(deviceList);

        String responseJson = convertFindResponseToJson(response);
        Logger.d("responseJson: " + responseJson);

        CoapMessageBuilder responseMessage = new CoapMessageBuilder();
        CoapResponse coapResponse = responseMessage.buildCoapResponse(
                request.getToken(), responseJson, CoapStatus.CONTENT);

        return coapResponse;
    }

    private String convertFindResponseToJson(ResponseObject response) {

        HashMap<Object, Object> responseMap = new HashMap<Object, Object>();

        ArrayList<String> deviceList = response.getDeviceList();
        responseMap.put(Const.RESPONSE_DEVICES, deviceList);

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

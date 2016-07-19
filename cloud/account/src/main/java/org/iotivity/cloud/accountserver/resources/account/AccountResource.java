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
package org.iotivity.cloud.accountserver.resources.account;

import java.util.Arrays;
import java.util.HashMap;

import org.iotivity.cloud.accountserver.AccountServerManager;
import org.iotivity.cloud.accountserver.Constants;
import org.iotivity.cloud.accountserver.token.Token;
import org.iotivity.cloud.accountserver.token.TokenPolicy;
import org.iotivity.cloud.base.device.Device;
import org.iotivity.cloud.base.exception.ServerException;
import org.iotivity.cloud.base.exception.ServerException.BadRequestException;
import org.iotivity.cloud.base.exception.ServerException.InternalServerErrorException;
import org.iotivity.cloud.base.exception.ServerException.PreconditionFailedException;
import org.iotivity.cloud.base.protocols.IRequest;
import org.iotivity.cloud.base.protocols.IResponse;
import org.iotivity.cloud.base.protocols.MessageBuilder;
import org.iotivity.cloud.base.protocols.enums.ContentFormat;
import org.iotivity.cloud.base.protocols.enums.ResponseStatus;
import org.iotivity.cloud.base.resource.Resource;
import org.iotivity.cloud.util.Cbor;
import org.iotivity.cloud.util.Logger;

/**
 *
 * This class provides a set of APIs to manage resources corresponding with user
 * account
 *
 */
public class AccountResource extends Resource {

    private Cbor<HashMap<String, Object>> mCbor      = new Cbor<>();

    private AccountServerManager          mAsManager = new AccountServerManager();

    public AccountResource() {
        super(Arrays.asList(Constants.PREFIX_WELL_KNOWN, Constants.PREFIX_OCF,
                Constants.ACCOUNT_URI));

        // addQueryHandler(Arrays.asList("if=" + Constants.INTERFACE_DEFAULT),
        // this::onDefaultInterfaceReceived);
    }

    @Override
    public void onDefaultRequestReceived(Device srcDevice, IRequest request)
            throws ServerException {

        IResponse response = null;

        switch (request.getMethod()) {

            case POST:
                // Used for user sign-up
                response = handlePostSignUp(request);
                break;

            default:
                throw new BadRequestException(
                        request.getMethod() + " request type is not support");
        }

        srcDevice.sendResponse(response);
    }

    private IResponse handlePostSignUp(IRequest request)
            throws ServerException {

        if (request.getPayload() == null) {
            throw new BadRequestException("payload is null");
        }

        HashMap<String, Object> payloadData = mCbor
                .parsePayloadFromCbor(request.getPayload(), HashMap.class);

        exceptionPayloadCheck(payloadData);

        String deviceId = payloadData.get(Constants.REQ_DEVICE_ID).toString();
        String authCode = payloadData.get(Constants.REQ_AUTH_CODE).toString();
        String authProvider = payloadData.get(Constants.REQ_AUTH_PROVIDER)
                .toString();

        Logger.d("authCode: " + authCode);

        @SuppressWarnings("unchecked")
        HashMap<String, String> options = (HashMap<String, String>) payloadData
                .get(Constants.REQ_AUTH_OPTIONS);

        String authServerUrl = null;
        String apiServerUrl = null;

        if (options != null) {
            authServerUrl = (String) options.get(Constants.REQ_AUTH_SERVER_URL);
            apiServerUrl = (String) options.get(Constants.REQ_API_SERVER_URL);
        }

        boolean res = false;

        res = mAsManager.loadAuthServer(authProvider);

        if (!res) {
            throw new InternalServerErrorException(
                    authProvider + " library is not loaded");
        }

        String userId = mAsManager.requestUserId(authCode, authProvider,
                authServerUrl, apiServerUrl);

        if (userId == null) {
            throw new BadRequestException("authCode is invalid");
        }

        Token token = new Token();
        token = mAsManager.registerUserAccount(userId);

        String accessToken = token.getAccessToken();
        String refreshToken = token.getRefreshToken();

        if (accessToken == null || refreshToken == null) {
            throw new InternalServerErrorException("MongoDB is not operating");
        }

        HashMap<String, Object> responsePayload = null;

        responsePayload = new HashMap<String, Object>();

        responsePayload.put(Constants.RESP_ACCESS_TOKEN, accessToken);
        responsePayload.put(Constants.RESP_REFRESH_TOKEN, refreshToken);
        responsePayload.put(Constants.RESP_TOKEN_TYPE,
                TokenPolicy.BEARER_TOKEN);
        responsePayload.put(Constants.RESP_EXPIRES_IN, TokenPolicy.EXPIRES_IN);
        responsePayload.put(Constants.RESP_USER_ID, userId);

        res = mAsManager.registerUserAccount(userId, deviceId);

        if (!res) {
            throw new InternalServerErrorException("MongoDB is not operating");
        }

        return MessageBuilder.createResponse(request, ResponseStatus.CREATED,
                ContentFormat.APPLICATION_CBOR,
                mCbor.encodingPayloadToCbor(responsePayload));
    }

    private void exceptionPayloadCheck(HashMap<String, Object> payloadData)
            throws ServerException {
        if (payloadData == null) {
            throw new BadRequestException("payload is null");
        }

        // check if mandatory properties exist
        if (!payloadData.containsKey(Constants.REQ_DEVICE_ID)) {
            throw new PreconditionFailedException("di property is not include");
        }

        if (!payloadData.containsKey(Constants.REQ_AUTH_CODE)) {
            throw new PreconditionFailedException(
                    "authcode property is not include");
        }

        if (!payloadData.containsKey(Constants.REQ_AUTH_PROVIDER)) {
            throw new PreconditionFailedException(
                    "authprovider property is not include");
        }

        // check if mandatory properties have null values
        if (payloadData.get(Constants.REQ_DEVICE_ID) == null) {
            throw new PreconditionFailedException("di param is null");
        }

        if (payloadData.get(Constants.REQ_AUTH_CODE) == null) {
            throw new PreconditionFailedException("authcode param is null");
        }

        if (payloadData.get(Constants.REQ_AUTH_PROVIDER) == null) {
            throw new PreconditionFailedException("authprovider param is null");
        }
    }
}

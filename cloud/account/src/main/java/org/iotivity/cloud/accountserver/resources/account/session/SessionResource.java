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
package org.iotivity.cloud.accountserver.resources.account.session;

import java.util.Arrays;
import java.util.HashMap;

import org.iotivity.cloud.accountserver.Constants;
import org.iotivity.cloud.accountserver.token.TokenManager;
import org.iotivity.cloud.base.device.Device;
import org.iotivity.cloud.base.exception.ServerException;
import org.iotivity.cloud.base.exception.ServerException.BadRequestException;
import org.iotivity.cloud.base.exception.ServerException.UnAuthorizedException;
import org.iotivity.cloud.base.protocols.IRequest;
import org.iotivity.cloud.base.protocols.IResponse;
import org.iotivity.cloud.base.protocols.MessageBuilder;
import org.iotivity.cloud.base.protocols.enums.ContentFormat;
import org.iotivity.cloud.base.protocols.enums.ResponseStatus;
import org.iotivity.cloud.base.resource.Resource;
import org.iotivity.cloud.util.Cbor;

public class SessionResource extends Resource {

    private Cbor<HashMap<String, Object>> mCbor         = new Cbor<>();

    TokenManager                          mTokenManager = new TokenManager();

    public SessionResource() {
        super(Arrays.asList(Constants.PREFIX_WELL_KNOWN, Constants.PREFIX_OCF,
                Constants.ACCOUNT_URI, Constants.SESSION_URI));

        // addQueryHandler(Arrays.asList("if=" + Constants.INTERFACE_DEFAULT),
        // this::onDefaultInterfaceReceived);
    }

    @Override
    public void onDefaultRequestReceived(Device srcDevice, IRequest request)
            throws ServerException {

        IResponse response = null;

        switch (request.getMethod()) {
            case POST:
                // Used for user sign-in & out
                response = handlePostSigninout(request);
                break;

            default:
                throw new BadRequestException(
                        request.getMethod() + " request type is not support");
        }

        srcDevice.sendResponse(response);
    }

    private IResponse handlePostSigninout(IRequest request)
            throws ServerException {

        // exception handle before getting payload
        if (request.getPayload() == null) {
            throw new BadRequestException("payload is empty");
        }

        HashMap<String, Object> payloadData = mCbor
                .parsePayloadFromCbor(request.getPayload(), HashMap.class);

        if (payloadData == null) {
            throw new BadRequestException("CBOR parsing failed");
        }

        // String deviceId =
        // payloadData.get(Constants.REQUEST_DEVICE_ID).toString();
        String accessToken = payloadData.get(Constants.REQ_ACCESS_TOKEN)
                .toString();
        boolean signinRequest = (boolean) payloadData.get(Constants.REQ_STATUS);

        Boolean res = false;

        res = mTokenManager.verifyAccessToken(accessToken);

        if (!res) {
            throw new UnAuthorizedException("AccessToken is unauthorized");
        }

        if (!signinRequest) {
            return MessageBuilder.createResponse(request, ResponseStatus.VALID);
        }

        HashMap<String, Object> responsePayload = null;

        responsePayload = new HashMap<String, Object>();

        responsePayload.put(Constants.RESP_EXPIRES_IN,
                mTokenManager.getRemainExpiredTime(accessToken));

        return MessageBuilder.createResponse(request, ResponseStatus.VALID,
                ContentFormat.APPLICATION_CBOR,
                mCbor.encodingPayloadToCbor(responsePayload));
    }
}

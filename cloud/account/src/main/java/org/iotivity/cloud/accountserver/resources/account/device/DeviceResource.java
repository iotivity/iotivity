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
package org.iotivity.cloud.accountserver.resources.account.device;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;

import org.iotivity.cloud.accountserver.AccountServerManager;
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

public class DeviceResource extends Resource {

    private Cbor<HashMap<String, Object>> mCbor         = new Cbor<>();

    TokenManager                          mTokenManager = new TokenManager();

    private AccountServerManager          mAsManager    = new AccountServerManager();

    public DeviceResource() {
        super(Arrays.asList(Constants.PREFIX_WELL_KNOWN, Constants.PREFIX_OCF,
                Constants.ACCOUNT_URI, Constants.DEVICE_URI));
    }

    @Override
    public void onDefaultRequestReceived(Device srcDevice, IRequest request)
            throws ServerException {

        IResponse response = null;

        switch (request.getMethod()) {
            case GET:
                // Used for getting devices.
                response = handleGetDevice(request);
                break;

            default:
                throw new BadRequestException(
                        request.getMethod() + " request type is not support");
        }
        srcDevice.sendResponse(response);
    }

    private IResponse handleGetDevice(IRequest request) throws ServerException {

        String accessToken = request.getUriQueryMap().get("accesstoken").get(0);

        if (accessToken == null) {
            throw new BadRequestException("AccessToken is empty");
        }

        Boolean res = false;

        res = mTokenManager.verifyAccessToken(accessToken);

        if (!res) {
            throw new UnAuthorizedException("AccessToken is unauthorized");
        }

        String userId = mAsManager.requestUserId(accessToken);

        if (userId == null) {
            throw new BadRequestException("userid is invalid");
        }

        ArrayList<String> deviceList = mAsManager.requestAccountDevices(userId);

        if (request.getUriQueryMap().get("di") != null) {
            String di = request.getUriQueryMap().get("di").get(0);
            if (deviceList.contains(di)) {
                return MessageBuilder.createResponse(request,
                        ResponseStatus.VALID);
            } else {
                throw new UnAuthorizedException("di is invalid");
            }
        } else {
            HashMap<String, Object> responsePayload = new HashMap<String, Object>();
            responsePayload.put(Constants.RESP_DEVICES, deviceList);

            return MessageBuilder.createResponse(request,
                    ResponseStatus.CONTENT, ContentFormat.APPLICATION_CBOR,
                    mCbor.encodingPayloadToCbor(responsePayload));
        }
    }
}

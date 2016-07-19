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
package org.iotivity.cloud.ciserver.resources.proxy;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;

import org.iotivity.cloud.base.connector.ConnectorPool;
import org.iotivity.cloud.base.device.CoapDevice;
import org.iotivity.cloud.base.device.Device;
import org.iotivity.cloud.base.device.IRequestChannel;
import org.iotivity.cloud.base.device.IResponseEventHandler;
import org.iotivity.cloud.base.exception.ClientException;
import org.iotivity.cloud.base.exception.ClientException.BadResponseException;
import org.iotivity.cloud.base.exception.ServerException;
import org.iotivity.cloud.base.protocols.IRequest;
import org.iotivity.cloud.base.protocols.IResponse;
import org.iotivity.cloud.base.protocols.MessageBuilder;
import org.iotivity.cloud.base.protocols.enums.RequestMethod;
import org.iotivity.cloud.base.resource.Resource;
import org.iotivity.cloud.ciserver.Constants;
import org.iotivity.cloud.util.Cbor;

public class ResourceFind extends Resource {
    IRequestChannel mASServer = null;

    public ResourceFind() {
        super(Arrays.asList(Constants.PREFIX_OIC, Constants.WELL_KNOWN_URI));

        mASServer = ConnectorPool.getConnection("account");
    }

    class AccountReceiveHandler implements IResponseEventHandler {

        IRequestChannel  mRDServer = null;
        private Device   mSrcDevice;
        private IRequest mRequest;

        public AccountReceiveHandler(IRequest request, Device srcDevice) {
            mRDServer = ConnectorPool.getConnection("rd");
            mSrcDevice = srcDevice;
            mRequest = request;
        }

        @Override
        public void onResponseReceived(IResponse response)
                throws ClientException {

            switch (response.getStatus()) {
                case VALID:
                    mRDServer.sendRequest(mRequest, mSrcDevice);
                    break;

                case CONTENT:
                    StringBuilder additionalQuery = new StringBuilder();

                    Cbor<HashMap<String, ArrayList<String>>> responsePayload = new Cbor<>();

                    ArrayList<String> deviceList = responsePayload
                            .parsePayloadFromCbor(response.getPayload(),
                                    HashMap.class)
                            .get("devices");

                    int index = deviceList.size();

                    for (String deviceId : deviceList) {
                        if (!deviceId.equals(mSrcDevice.getDeviceId())) {
                            additionalQuery.append("di=");
                            additionalQuery.append(deviceId);
                            if (--index > 0) {
                                additionalQuery.append("&");
                            }
                        }
                    }
                    String uriQuery = (mRequest.getUriQuery() == null ? ""
                            : mRequest.getUriQuery() + "&")
                            + additionalQuery.toString();
                    IRequest requestToAS = MessageBuilder.modifyRequest(
                            mRequest, null, uriQuery, null, null);

                    mRDServer.sendRequest(requestToAS, mSrcDevice);
                    break;

                default:
                    throw new BadResponseException(
                            response.getStatus().toString()
                                    + " response type is not supported");
            }
        }
    }

    @Override
    public void onDefaultRequestReceived(Device srcDevice, IRequest request)
            throws ServerException {
        // Token exchange is done by CoapClient
        CoapDevice coapDevice = (CoapDevice) srcDevice;
        StringBuffer uriQuery = new StringBuffer();
        uriQuery.append(Constants.SEARCH_ACCESS_TOKEN + "=");
        uriQuery.append(coapDevice.getAccessToken());

        if (request.getUriQueryMap().get("di") != null) {
            String di = request.getUriQueryMap().get("di").get(0);
            if (di != null) {
                uriQuery.append("&");
                uriQuery.append("di" + "=");
                uriQuery.append(di);
            }
        }

        StringBuffer uriPath = new StringBuffer();
        uriPath.append(Constants.PREFIX_WELL_KNOWN + "/");
        uriPath.append(Constants.PREFIX_OCF + "/");
        uriPath.append(Constants.ACCOUNT_URI + "/");
        uriPath.append(Constants.DEVICE_URI);

        IRequest requestToAS = MessageBuilder.createRequest(RequestMethod.GET,
                uriPath.toString(), uriQuery.toString());

        mASServer.sendRequest(requestToAS,
                new AccountReceiveHandler(request, srcDevice));
    }
}

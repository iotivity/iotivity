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
package org.iotivity.cloud.ciserver.resources.proxy.rd;

import java.util.Arrays;
import java.util.HashMap;
import java.util.List;

import org.iotivity.cloud.base.connector.ConnectorPool;
import org.iotivity.cloud.base.device.Device;
import org.iotivity.cloud.base.device.IRequestChannel;
import org.iotivity.cloud.base.device.IResponseEventHandler;
import org.iotivity.cloud.base.exception.ClientException;
import org.iotivity.cloud.base.exception.ClientException.BadResponseException;
import org.iotivity.cloud.base.exception.ServerException;
import org.iotivity.cloud.base.exception.ServerException.UnAuthorizedException;
import org.iotivity.cloud.base.protocols.IRequest;
import org.iotivity.cloud.base.protocols.IResponse;
import org.iotivity.cloud.base.protocols.MessageBuilder;
import org.iotivity.cloud.base.protocols.enums.RequestMethod;
import org.iotivity.cloud.base.resource.Resource;
import org.iotivity.cloud.ciserver.Constants;
import org.iotivity.cloud.util.Cbor;

public class DevicePresence extends Resource {
    IRequestChannel                       mASServer = null;
    private Cbor<HashMap<String, Object>> mCbor     = new Cbor<>();

    public DevicePresence() {
        super(Arrays.asList(Constants.PREFIX_WELL_KNOWN, Constants.PREFIX_OCF,
                Constants.DEVICE_PRESENCE_URI));

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
                case CONTENT:
                    StringBuilder additionalQuery = new StringBuilder();

                    HashMap<String, Object> payloadData = mCbor
                            .parsePayloadFromCbor(response.getPayload(),
                                    HashMap.class);
                    List<String> deviceList = (List<String>) payloadData
                            .get(Constants.REQ_DEVICE_LIST);
                    if (deviceList != null) {
                        int index = deviceList.size();
                        if (mRequest.getUriQuery() == null) {
                            for (String device : deviceList) {
                                if (!device.equals(mSrcDevice.getDeviceId())) {
                                    additionalQuery
                                            .append(Constants.REQ_DEVICE_ID
                                                    + "=" + device);
                                    if (--index > 0) {
                                        additionalQuery.append(";");
                                    }
                                }
                            }
                            String uriQuery = (mRequest.getUriQuery() == null
                                    ? "" : mRequest.getUriQuery()) + ";"
                                    + additionalQuery.toString();
                            mRequest = MessageBuilder.modifyRequest(mRequest,
                                    null, uriQuery, null, null);
                        } else {
                            List<String> originDeviceList = mRequest
                                    .getUriQueryMap()
                                    .get(Constants.REQ_DEVICE_ID);
                            if (originDeviceList == null) {
                                throw new UnAuthorizedException(
                                        "device is not include in group");
                            }
                            for (String device : originDeviceList) {
                                if (!deviceList.contains(device)) {
                                    throw new UnAuthorizedException(device
                                            + " device is not include in group");
                                }
                            }
                        }
                        mRDServer.sendRequest(mRequest, mSrcDevice);
                    } else
                        throw new UnAuthorizedException(
                                "There is not device in group");
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
        StringBuffer uriQuery = new StringBuffer();
        uriQuery.append(Constants.REQ_MEMBER_ID + "=" + srcDevice.getUserId());

        StringBuffer uriPath = new StringBuffer();
        uriPath.append(Constants.PREFIX_WELL_KNOWN + "/");
        uriPath.append(Constants.PREFIX_OCF + "/");
        uriPath.append(Constants.ACL_URI + "/");
        uriPath.append(Constants.GROUP_URI + "/");
        uriPath.append(srcDevice.getUserId());

        IRequest requestToAS = MessageBuilder.createRequest(RequestMethod.GET,
                uriPath.toString(), uriQuery.toString());

        mASServer.sendRequest(requestToAS,
                new AccountReceiveHandler(request, srcDevice));
    }
}
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
package org.iotivity.cloud.ciserver.resources;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;

import org.iotivity.cloud.base.device.CoapDevice;
import org.iotivity.cloud.base.device.Device;
import org.iotivity.cloud.base.device.IRequestChannel;
import org.iotivity.cloud.base.device.IResponseEventHandler;
import org.iotivity.cloud.base.exception.ServerException;
import org.iotivity.cloud.base.exception.ServerException.BadRequestException;
import org.iotivity.cloud.base.exception.ServerException.NotFoundException;
import org.iotivity.cloud.base.exception.ServerException.PreconditionFailedException;
import org.iotivity.cloud.base.protocols.IRequest;
import org.iotivity.cloud.base.protocols.IResponse;
import org.iotivity.cloud.base.protocols.MessageBuilder;
import org.iotivity.cloud.base.protocols.coap.CoapResponse;
import org.iotivity.cloud.base.protocols.enums.ContentFormat;
import org.iotivity.cloud.base.protocols.enums.ResponseStatus;
import org.iotivity.cloud.base.resource.Resource;
import org.iotivity.cloud.ciserver.Constants;
import org.iotivity.cloud.ciserver.DeviceServerSystem.CoapDevicePool;
import org.iotivity.cloud.util.Cbor;

/**
 *
 * This class provides a set of APIs to send requests about message to another
 * device
 *
 */
public class DiResource extends Resource {

    private CoapDevicePool mDevicePool = null;

    public DiResource(CoapDevicePool devicePool) {
        super(Arrays.asList(Constants.REQ_DEVICE_ID));
        mDevicePool = devicePool;

        addQueryHandler(
                Arrays.asList(Constants.RS_INTERFACE + "="
                        + Constants.LINK_INTERFACE),
                this::onLinkInterfaceRequestReceived);
    }

    private IRequestChannel getTargetDeviceChannel(IRequest request)
            throws ServerException {
        List<String> uriPathSegment = request.getUriPathSegments();

        if (uriPathSegment.size() < 2) {
            throw new PreconditionFailedException();
        }

        String deviceId = uriPathSegment.get(1);
        CoapDevice targetDevice = (CoapDevice) mDevicePool
                .queryDevice(deviceId);

        if (targetDevice == null) {
            throw new NotFoundException();
        }

        // Do request and receive response
        return targetDevice.getRequestChannel();
    }

    private String extractTargetUriPath(IRequest request) {
        List<String> uriPathSegment = request.getUriPathSegments();

        // Remove prefix path
        uriPathSegment.remove(0);
        uriPathSegment.remove(0);

        StringBuilder uriPath = new StringBuilder();
        for (String path : uriPathSegment) {
            uriPath.append("/" + path);
        }

        return uriPath.toString();
    }

    private IResponse convertReponseUri(IResponse response, String di) {

        String convertedUri = new String();

        CoapResponse coapResponse = (CoapResponse) response;

        if (coapResponse.getUriPath().isEmpty() == false) {
            convertedUri = "/di/" + di + "/" + coapResponse.getUriPath();
        }

        return MessageBuilder.modifyResponse(response, convertedUri, null,
                null);
    }

    /**
     *
     * This class provides a set of APIs to handling message contains link
     * interface.
     *
     */
    class LinkInterfaceHandler implements IResponseEventHandler {
        private Cbor<List<HashMap<String, Object>>> mCbor      = new Cbor<>();
        private String                              mTargetDI  = null;
        private Device                              mSrcDevice = null;

        public LinkInterfaceHandler(String targetDI, Device srcDevice) {
            mTargetDI = targetDI;
            mSrcDevice = srcDevice;
        }

        private void convertHref(List<HashMap<String, Object>> linkPayload) {
            for (HashMap<String, Object> link : linkPayload) {
                link.put("href", "/di/" + mTargetDI + link.get("href"));
            }
        }

        @Override
        public void onResponseReceived(IResponse response) {
            List<HashMap<String, Object>> linkPayload = null;
            if (response.getStatus() == ResponseStatus.CONTENT) {
                linkPayload = mCbor.parsePayloadFromCbor(response.getPayload(),
                        ArrayList.class);
                if (linkPayload == null) {
                    throw new BadRequestException("payload is null");
                }
                convertHref(linkPayload);
            }

            mSrcDevice.sendResponse(MessageBuilder.modifyResponse(
                    convertReponseUri(response, mTargetDI),
                    ContentFormat.APPLICATION_CBOR, linkPayload != null
                            ? mCbor.encodingPayloadToCbor(linkPayload) : null));
        }
    }

    /**
     * API for handling optional method for handling packet contains link
     * interface.
     * 
     * @param srcDevice
     *            device information contains response channel
     * @param request
     *            received request to relay
     */
    public void onLinkInterfaceRequestReceived(Device srcDevice,
            IRequest request) throws ServerException {
        IRequestChannel requestChannel = getTargetDeviceChannel(request);

        if (requestChannel == null) {
            throw new NotFoundException();
        }

        String deviceId = request.getUriPathSegments().get(1);

        requestChannel.sendRequest(
                MessageBuilder.modifyRequest(request,
                        extractTargetUriPath(request), null, null, null),
                new LinkInterfaceHandler(deviceId, srcDevice));
    }

    class DefaultResponseHandler implements IResponseEventHandler {
        private String mTargetDI  = null;
        private Device mSrcDevice = null;

        public DefaultResponseHandler(String targetDI, Device srcDevice) {
            mTargetDI = targetDI;
            mSrcDevice = srcDevice;
        }

        @Override
        public void onResponseReceived(IResponse response) {

            mSrcDevice.sendResponse(convertReponseUri(response, mTargetDI));
        }
    }

    @Override
    public void onDefaultRequestReceived(Device srcDevice, IRequest request)
            throws ServerException {
        // Find proper request channel using di in URI path field.
        IRequestChannel requestChannel = getTargetDeviceChannel(request);

        if (requestChannel == null) {
            throw new NotFoundException();
        }

        String deviceId = request.getUriPathSegments().get(1);

        requestChannel.sendRequest(
                MessageBuilder.modifyRequest(request,
                        extractTargetUriPath(request), null, null, null),
                new DefaultResponseHandler(deviceId, srcDevice));
    }

}
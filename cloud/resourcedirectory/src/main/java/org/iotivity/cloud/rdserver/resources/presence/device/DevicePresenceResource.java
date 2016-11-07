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
package org.iotivity.cloud.rdserver.resources.presence.device;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;

import org.iotivity.cloud.base.device.Device;
import org.iotivity.cloud.base.exception.ServerException;
import org.iotivity.cloud.base.exception.ServerException.BadRequestException;
import org.iotivity.cloud.base.protocols.IRequest;
import org.iotivity.cloud.base.protocols.IResponse;
import org.iotivity.cloud.base.protocols.MessageBuilder;
import org.iotivity.cloud.base.protocols.enums.ContentFormat;
import org.iotivity.cloud.base.protocols.enums.ResponseStatus;
import org.iotivity.cloud.base.resource.Resource;
import org.iotivity.cloud.rdserver.Constants;
import org.iotivity.cloud.rdserver.db.DBManager;
import org.iotivity.cloud.rdserver.util.TypeCastingManager;
import org.iotivity.cloud.util.Cbor;
import org.iotivity.cloud.util.Log;

public class DevicePresenceResource extends Resource {

    private Cbor<HashMap<String, Object>> mCbor = new Cbor<>();

    private class PresenceSubscriber {
        PresenceSubscriber(Device subscriber, IRequest request) {
            mSubscriber = subscriber;
            mRequest = request;
        }

        public Device   mSubscriber;
        public IRequest mRequest;
    }

    // di , token, Subscriber list
    private HashMap<String, HashMap<String, PresenceSubscriber>> mDeviceSubscriber  = null;
    // token, di list
    private HashMap<String, List<String>>                        mSubscribedDevices = null;

    public DevicePresenceResource() {
        super(Arrays.asList(Constants.PREFIX_OIC,
                Constants.DEVICE_PRESENCE_URI));

        mDeviceSubscriber = new HashMap<>();
        mSubscribedDevices = new HashMap<>();
    }

    @Override
    public void onDefaultRequestReceived(Device srcDevice, IRequest request)
            throws ServerException {

        IResponse response = null;

        switch (request.getMethod()) {
            case GET:
                response = handleGetRequest(srcDevice, request);
                break;

            case POST:
                response = handlePostRequest(request);
                break;

            default:
                throw new BadRequestException(
                        request.getMethod() + " request type is not supported");
        }

        srcDevice.sendResponse(response);
    }

    private IResponse handleGetRequest(Device srcDevice, IRequest request)
            throws ServerException {
        HashMap<String, List<String>> queryMap = request.getUriQueryMap();

        byte[] payload = null;

        if (checkQueryException(Arrays.asList(Constants.DEVICE_ID), queryMap)) {

            List<String> deviceList = queryMap.get(Constants.DEVICE_ID);

            switch (request.getObserve()) {
                case SUBSCRIBE:
                    addObserver(srcDevice, request, deviceList);
                    break;
                case UNSUBSCRIBE:
                    removeObserver(request);
                    break;
                default:
            }

            payload = makeResponsePayload(deviceList);
        }

        return MessageBuilder.createResponse(request, ResponseStatus.CONTENT,
                ContentFormat.APPLICATION_CBOR, payload);
    }

    public IResponse handlePostRequest(IRequest request)
            throws ServerException {
        // check payload
        byte[] payload = request.getPayload();

        HashMap<String, Object> parsedPayload = mCbor
                .parsePayloadFromCbor(payload, HashMap.class);

        if (checkPayloadException(
                Arrays.asList(Constants.DEVICE_ID, Constants.PRESENCE_STATE),
                parsedPayload)) {

            String deviceId = parsedPayload.get(Constants.DEVICE_ID).toString();
            String state = parsedPayload.get(Constants.PRESENCE_STATE)
                    .toString();
            DeviceState deviceState = new DeviceState();
            deviceState.setDi(deviceId);
            deviceState.setState(state);

            TypeCastingManager<DeviceState> deviceStateTypeManager = new TypeCastingManager<DeviceState>();
            HashMap<Object, Object> storeMap = deviceStateTypeManager
                    .convertObjectToMap(deviceState);

            // store db
            DBManager.getInstance().updateDeviceState(storeMap);

            // notification to observers
            notifyToObservers(deviceId);
        }

        return MessageBuilder.createResponse(request, ResponseStatus.CHANGED);
    }

    private void addObserver(Device srcDevice, IRequest request,
            List<String> deviceIdList) {

        for (String deviceId : deviceIdList) {
            HashMap<String, PresenceSubscriber> subscribers = mDeviceSubscriber
                    .get(deviceId);

            if (subscribers == null) {
                subscribers = new HashMap<>();
                mDeviceSubscriber.put(deviceId, subscribers);
            }

            subscribers.put(request.getRequestId(),
                    new PresenceSubscriber(srcDevice, request));
        }

        mSubscribedDevices.put(request.getRequestId(), deviceIdList);
    }

    private void removeObserver(IRequest request) {

        List<String> deviceIdList = mSubscribedDevices
                .get(request.getRequestId());

        if (deviceIdList == null) {
            return;
        }

        for (String deviceId : deviceIdList) {
            HashMap<String, PresenceSubscriber> subscribers = mDeviceSubscriber
                    .get(deviceId);

            if (subscribers == null) {
                continue;
            }

            subscribers.remove(request.getRequestId());
        }
    }

    private void notifyToObservers(String deviceId) {

        HashMap<String, PresenceSubscriber> tokenNSubscribers = mDeviceSubscriber
                .get(deviceId);

        if (tokenNSubscribers != null) {
            byte[] paylod = makeResponsePayload(Arrays.asList(deviceId));

            for (PresenceSubscriber subscriber : tokenNSubscribers.values()) {

                subscriber.mSubscriber.sendResponse(
                        MessageBuilder.createResponse(subscriber.mRequest,
                                ResponseStatus.CONTENT,
                                ContentFormat.APPLICATION_CBOR, paylod));
            }
        }
    }

    private byte[] makeResponsePayload(List<String> deviceList) {

        HashMap<String, Object> getPayload = new HashMap<>();
        ArrayList<HashMap<String, String>> prsList = new ArrayList<HashMap<String, String>>();

        for (String deviceId : deviceList) {
            HashMap<String, String> payloadSegment = new HashMap<String, String>();
            payloadSegment.put(Constants.DEVICE_ID, deviceId);
            payloadSegment.put(Constants.PRESENCE_STATE,
                    DBManager.getInstance().findDeviceState(deviceId));
            prsList.add(payloadSegment);
        }
        getPayload.put(Constants.PRESENCE_LIST, prsList);
        Log.i("Get observe response" + getPayload.toString());

        return mCbor.encodingPayloadToCbor(getPayload);
    }
}

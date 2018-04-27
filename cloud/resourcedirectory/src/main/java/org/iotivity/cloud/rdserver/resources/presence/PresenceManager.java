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
package org.iotivity.cloud.rdserver.resources.presence;

import java.io.ByteArrayOutputStream;
import java.util.*;

import org.bson.Document;
import org.iotivity.cloud.rdserver.resources.directory.RDManager;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.iotivity.cloud.base.device.Device;
import org.iotivity.cloud.base.exception.ServerException.InternalServerErrorException;
import org.iotivity.cloud.base.protocols.IRequest;
import org.iotivity.cloud.base.protocols.MessageBuilder;
import org.iotivity.cloud.base.protocols.enums.ContentFormat;
import org.iotivity.cloud.base.protocols.enums.ResponseStatus;
import org.iotivity.cloud.rdserver.Constants;
import org.iotivity.cloud.rdserver.db.DBManager;
import org.iotivity.cloud.util.Cbor;

import com.fasterxml.jackson.core.JsonEncoding;
import com.fasterxml.jackson.core.JsonGenerator;
import com.fasterxml.jackson.dataformat.cbor.CBORFactory;
import com.fasterxml.jackson.dataformat.cbor.CBORGenerator;

/**
 * 
 * This class provides a set of APIs handle requests about presence
 *
 */
public class PresenceManager {
    private final static Logger           Log              = LoggerFactory.getLogger(PresenceManager.class);
    private static PresenceManager        mPresenceManager = new PresenceManager();

    private Cbor<HashMap<String, Object>> mCbor            = new Cbor<>();

    private static class PresenceSubscriber {
        PresenceSubscriber(Device subscriber, IRequest request) {
            mSubscriber = subscriber;
            mRequest = request;
        }

        public Device   mSubscriber;
        public IRequest mRequest;
    }

    private class PresenceInfo {

        PresenceInfo() {
            mSubscriber = new HashMap<>();
            mSubscribedDevices = new HashMap<>();
            mSequenceNumber = new HashMap<>();
        }

        // di , token, Subscriber list
        protected HashMap<String, HashMap<String, PresenceSubscriber>> mSubscriber;
        // token, di list
        protected HashMap<String, List<String>>                        mSubscribedDevices;
        protected HashMap<String, Long>                                mSequenceNumber;


        public void subscribe(Device srcDevice, IRequest request,
                              List<String> deviceIdList){
            for (String deviceId : deviceIdList) {
                HashMap<String, PresenceSubscriber> subscribers = mSubscriber
                        .get(deviceId);

                if (subscribers == null) {
                    subscribers = new HashMap<>();
                    mSubscriber.put(deviceId, subscribers);
                }

                subscribers.put(request.getRequestId(),
                        new PresenceSubscriber(srcDevice, request));
            }

            mSubscribedDevices.put(request.getRequestId(),
                    deviceIdList);
            mSequenceNumber.put(request.getRequestId(), (long) 1);
        }

        public void unSubscribe(IRequest request, List<String> deviceIdList) {

            for (String deviceId : deviceIdList) {
                HashMap<String, PresenceSubscriber> subscribers = mSubscriber
                        .get(deviceId);

                if (subscribers == null) {
                    continue;
                }

                subscribers.remove(request.getRequestId());
            }
        }

        public void notify(String deviceId) {

            HashMap<String, PresenceSubscriber> tokenNSubscribers = mSubscriber
                    .get(deviceId);

            if (tokenNSubscribers != null) {
                byte[] payload = makeResponsePayload(Arrays.asList(deviceId));

                for (PresenceSubscriber subscriber : tokenNSubscribers.values()) {

                    subscriber.mSubscriber.sendResponse(
                            MessageBuilder.createResponse(subscriber.mRequest,
                                    ResponseStatus.CONTENT,
                                    ContentFormat.APPLICATION_CBOR, payload));
                }
            }
        }

    }

    private class CompletePresenceInfo extends PresenceInfo {

        // di , token, Subscriber list
        private Map<String, PresenceSubscriber> mAllSubscribers;

        public CompletePresenceInfo() {
            super();
            this.mAllSubscribers = new HashMap<>();
        }

        @Override
        public void subscribe(Device srcDevice, IRequest request, List<String> deviceIdList) {
            if(deviceIdList != null && deviceIdList.size() > 0){
                super.subscribe(srcDevice,request,deviceIdList);
            }else {
                mAllSubscribers.put(request.getRequestId(),
                        new PresenceSubscriber(srcDevice, request));
                mSequenceNumber.put(request.getRequestId(), (long) 1);
            }
        }

        @Override
        public void unSubscribe(IRequest request, List<String> deviceIdList) {
            if (deviceIdList != null && deviceIdList.size() > 0) {
                super.unSubscribe(request, deviceIdList);
            } else {
                mAllSubscribers.remove(request.getRequestId());
            }
        }

        @Override
        public void notify(String deviceId) {

            byte[] payload = makeResponsePayload(Arrays.asList(deviceId));
            for (PresenceSubscriber subscriber : this.mAllSubscribers.values()) {
                subscriber.mSubscriber.sendResponse(
                        MessageBuilder.createResponse(subscriber.mRequest,
                                ResponseStatus.CONTENT,
                                ContentFormat.APPLICATION_CBOR, payload));
            }
        }
    }

    private PresenceInfo mDevicePresence   = null;
    private PresenceInfo mAllDevicePresence = null;
    private PresenceInfo mResourcePresence = null;

    private PresenceManager() {
        mDevicePresence = new PresenceInfo();
        mResourcePresence = new PresenceInfo();
        mAllDevicePresence = new CompletePresenceInfo();
    }

    /**
     * API to return PresenceManager object
     * 
     * @return PresenceManager object
     */
    public static PresenceManager getInstance() {
        return mPresenceManager;
    }

    /**
     * API to add observer
     * 
     * @param srcDevice
     *            channel information
     * @param request
     *            request message
     * @param deviceIdList
     *            subscribed device list
     * @param presenceType
     *            device presence or resource presence
     */
    public void subscribePresence(Device srcDevice, IRequest request,
            List<String> deviceIdList, String presenceType) {

        PresenceInfo presenceInfo = getPresenceInfo(presenceType,deviceIdList);
        presenceInfo.subscribe(srcDevice,request,deviceIdList);
    }

    /**
     * API to remove observer
     * 
     * @param request
     *            request message
     * @param deviceIdList
     *            unsubscribed device list
     * @param presenceType
     *            device presence or resource presence
     */
    public void unsubscribePresence(IRequest request, List<String> deviceIdList,
            String presenceType) {

        PresenceInfo presenceInfo = getPresenceInfo(presenceType,deviceIdList);
        presenceInfo.unSubscribe(request,deviceIdList);
    }

    /**
     * API for notifying to observers about device presence
     * 
     * @param deviceId
     *            device id
     */
    public void notifyToObservers(String deviceId) {
        this.mDevicePresence.notify(deviceId);
        this.mAllDevicePresence.notify(deviceId);
    }

    /**
     * API to make response payload about device presence
     * 
     * @param deviceList
     *            device id list
     * @return payload data
     */
    public byte[] makeResponsePayload(List<String> deviceList) {

        HashMap<String, Object> getPayload = new HashMap<>();
        ArrayList<HashMap<String, Object>> prsList = new ArrayList<>();

        for (String deviceId : deviceList) {
            HashMap<String, Object> payloadSegment = new HashMap<>();

            String deviceState = getDeviceState(deviceId);

            payloadSegment.put(Constants.DEVICE_ID, deviceId);

            if (deviceState != null) {
                payloadSegment.put(Constants.PRESENCE_STATE, deviceState);
            } else {
                payloadSegment.put(Constants.PRESENCE_STATE,
                        Constants.PRESENCE_OFF);
            }
            prsList.add(payloadSegment);
        }
        getPayload.put(Constants.PRESENCE_LIST, prsList);
        Log.info("Device presence observe response : " + getPayload.toString());

        return mCbor.encodingPayloadToCbor(getPayload);

    }

    private String getDeviceState(String deviceId) {

        HashMap<String, Object> condition = new HashMap<>();
        condition.put(Constants.DEVICE_ID, deviceId);

        String state = null;

        ArrayList<HashMap<String, Object>> readRecords = DBManager.getInstance()
                .selectRecord(Constants.PRESENCE_TABLE, condition);

        if (!readRecords.isEmpty()
                && readRecords.get(0).get(Constants.PRESENCE_STATE) != null) {
            state = readRecords.get(0).get(Constants.PRESENCE_STATE).toString();
        }

        return state;
    }

    private PresenceInfo getPresenceInfo(String presenceType,final List<String> deviceIds) {

        PresenceInfo presenceInfo = null;
        switch (presenceType) {
            case Constants.DEVICE_PRESENCE:
                if(deviceIds == null || deviceIds.size() == 0){
                    presenceInfo = mAllDevicePresence;
                }else {
                    presenceInfo = mDevicePresence;
                }
                break;
            case Constants.RESOURCE_PRESENCE:
                presenceInfo = mResourcePresence;
                break;
            default:
        }
        return presenceInfo;
    }

    /**
     * API for notifying to observers about resource presence
     * 
     * @param resourceInfo
     *            resource information
     */
    public void notifyToObservers(
            ArrayList<HashMap<String, Object>> resourceInfo) {

        if (resourceInfo.isEmpty()) {
            return;
        }

        Object obj = resourceInfo.get(0).get(Constants.DEVICE_ID);

        if (obj == null) {
            return;
        }

        String deviceId = obj.toString();

        HashMap<String, PresenceSubscriber> tokenNSubscribers = mResourcePresence.mSubscriber
                .get(deviceId);

        if (tokenNSubscribers != null) {

            for (PresenceSubscriber subscriber : tokenNSubscribers.values()) {

                for (HashMap<String, Object> resource : resourceInfo) {
                    subscriber.mSubscriber.sendResponse(
                            MessageBuilder.createResponse(subscriber.mRequest,
                                    ResponseStatus.CONTENT,
                                    ContentFormat.APPLICATION_CBOR,
                                    makeResponsePayload(
                                            subscriber.mRequest.getRequestId(),
                                            resource)));
                }
            }
        }
    }

    private byte[] makeResponsePayload(String requestId,
            HashMap<String, Object> resource) {

        ByteArrayOutputStream out = new ByteArrayOutputStream();
        CBORFactory f = new CBORFactory();
        try {
            JsonGenerator gen = f.createGenerator(out, JsonEncoding.UTF8);
            gen.writeStartObject();
            long sequenceId = mResourcePresence.mSequenceNumber.get(requestId);
            gen.writeNumberField(Constants.NON, sequenceId);
            mResourcePresence.mSequenceNumber.put(requestId, sequenceId + 1);
            gen.writeNumberField(Constants.RESOURCE_TTL, Long.parseLong(
                    checkPayload(resource, Constants.RESOURCE_TTL).toString()));

            gen.writeFieldName(Constants.TRIGGER);

            ((CBORGenerator) gen).writeRaw((byte) (224
                    + (byte) (checkPayload(resource, Constants.TRIGGER))));

            gen.writeStringField(Constants.RESOURCE_TYPE,
                    checkPayload(resource, Constants.RESOURCE_TYPE).toString());

            gen.writeStringField(Constants.HREF,
                    checkPayload(resource, Constants.HREF).toString());
            gen.writeEndObject();

            gen.close();
        } catch (Exception e) {
            throw new InternalServerErrorException(
                    "notification payload cbor encoding error");
        }

        return out.toByteArray();
    }

    private Object checkPayload(HashMap<String, Object> resource, String key) {
        Object obj = resource.get(key);

        if (obj == null) {
            throw new InternalServerErrorException(
                    "property (" + key + ") is null");
        }

        return obj;
    }

    /**
     * API to update device state
     * 
     * @param payload
     *            payload included device state
     */
    public void updateDevicePresence(HashMap<String, Object> payload) {
        DBManager.getInstance().insertAndReplaceRecord(Constants.PRESENCE_TABLE,
                payload);
    }
}

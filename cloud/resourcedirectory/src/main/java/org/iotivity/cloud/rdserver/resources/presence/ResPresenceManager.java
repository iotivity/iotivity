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
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import org.iotivity.cloud.base.device.Device;
import org.iotivity.cloud.base.protocols.IRequest;
import org.iotivity.cloud.base.protocols.MessageBuilder;
import org.iotivity.cloud.base.protocols.enums.ContentFormat;
import org.iotivity.cloud.base.protocols.enums.ResponseStatus;
import org.iotivity.cloud.rdserver.Constants;
import org.iotivity.cloud.rdserver.resources.presence.resource.ResPresencePayload;

import com.fasterxml.jackson.core.JsonEncoding;
import com.fasterxml.jackson.core.JsonGenerator;
import com.fasterxml.jackson.dataformat.cbor.CBORFactory;
import com.fasterxml.jackson.dataformat.cbor.CBORGenerator;

public class ResPresenceManager {

    public static ResPresenceManager mResPresenceManager = new ResPresenceManager();

    private class PresenceSubscriber {
        PresenceSubscriber(Device subscriber, IRequest request) {
            mSubscriber = subscriber;
            mRequest = request;
        }

        public Device   mSubscriber;
        public IRequest mRequest;
    }

    // di , token, Subscriber list
    private HashMap<String, HashMap<String, PresenceSubscriber>> mDeviceSubscriber         = null;
    // token, di list
    private HashMap<String, List<String>>                        mSubscribedDevices        = null;

    private HashMap<String, Long>                                mSubscriberSequenceNumber = null;

    public ResPresenceManager() {

        mDeviceSubscriber = new HashMap<>();
        mSubscribedDevices = new HashMap<>();
        mSubscriberSequenceNumber = new HashMap<>();
    }

    public static ResPresenceManager getInstance() {
        return mResPresenceManager;
    }

    public void addObserver(Device srcDevice, IRequest request,
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
        mSubscriberSequenceNumber.put(request.getRequestId(), (long) 1);

        return;
    }

    public void removeObserver(IRequest request) {

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

    public void notifyToObservers(String deviceId,
            ArrayList<ResPresencePayload> resPayloadList) {

        HashMap<String, PresenceSubscriber> tokenNSubscribers = mDeviceSubscriber
                .get(deviceId);

        if (tokenNSubscribers != null) {

            for (PresenceSubscriber subscriber : tokenNSubscribers.values()) {

                for (ResPresencePayload resPayload : resPayloadList) {
                    subscriber.mSubscriber.sendResponse(
                            MessageBuilder.createResponse(subscriber.mRequest,
                                    ResponseStatus.CONTENT,
                                    ContentFormat.APPLICATION_CBOR,
                                    makeResponsePayload(
                                            subscriber.mRequest.getRequestId(),
                                            resPayload)));
                }

            }
        }
    }

    private byte[] makeResponsePayload(String requestId,
            ResPresencePayload resPayload) {

        ByteArrayOutputStream out = new ByteArrayOutputStream();
        CBORFactory f = new CBORFactory();
        try {
            JsonGenerator gen = f.createGenerator(out, JsonEncoding.UTF8);
            gen.writeStartObject();
            long sequenceId = mSubscriberSequenceNumber.get(requestId);
            gen.writeNumberField(Constants.RS_NON, sequenceId);
            mSubscriberSequenceNumber.put(requestId, sequenceId + 1);
            gen.writeNumberField(Constants.RESOURCE_TTL,
                    (long) resPayload.getTtl());

            gen.writeFieldName(Constants.RS_TRIGGER);

            ((CBORGenerator) gen).writeRaw((byte) (224 + resPayload.getTrg()));

            gen.writeStringField(Constants.RESOURCE_TYPE, resPayload.getRt());

            gen.writeStringField(Constants.HREF, resPayload.getHref());
            gen.writeEndObject();

            gen.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
        return out.toByteArray();
    }
}

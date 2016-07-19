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
package org.iotivity.cloud.util;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.dataformat.cbor.CBORFactory;
import org.iotivity.cloud.base.exception.ServerException.PreconditionFailedException;

public class Cbor<T> {
    private CBORFactory  f;
    private ObjectMapper mapper;

    public Cbor() {
        f = new CBORFactory();
        mapper = new ObjectMapper(f);
    }

    @SuppressWarnings("unchecked")
    public T parsePayloadFromCbor(byte[] cborPayload,
            Class<? extends Object> class1) {
        T payload = null;
        try {
            payload = (T) mapper.readValue(cborPayload, class1);
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }

        return payload;
    }

    public byte[] encodingPayloadToCbor(Object payload) {
        byte[] cborData = null;
        try {
            cborData = mapper.writeValueAsBytes(payload);
        } catch (JsonProcessingException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        return cborData;
    }

    public String decodeDeviceId(byte[] payload) {
        Cbor<ArrayList<Object>> cbor = new Cbor<ArrayList<Object>>();
        ArrayList<Object> decodedPayload = null;

        if (payload == null) {
            throw new PreconditionFailedException("payload is null");
        }

        else {
            decodedPayload = cbor.parsePayloadFromCbor(payload,
                    ArrayList.class);

            HashMap<Object, Object> tags = (HashMap<Object, Object>) decodedPayload
                    .get(0);

            String deviceId = tags.get("di").toString();

            if (deviceId == null) {
                throw new PreconditionFailedException("deviceId is null");
            }

            Logger.i("deviceId : " + deviceId);

            return deviceId;
        }
    }
}
// ******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
//
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

package oic.ctt.formatter;

import oic.ctt.logger.CTLogger;
import oic.ctt.network.OICHelper;

import org.apache.commons.codec.binary.Base64;
import org.json.simple.JSONArray;
import java.io.IOException;
import java.util.LinkedHashSet;
import java.util.Set;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.dataformat.cbor.CBORFactory;

/**
 * 
 * This Class manages CBOR library to parse or convert CBOR data
 *
 */
public class CborManager {

    private static int          sCount          = 1;
    private static final String DI_KEY          = "di";
    public static byte[]        receivedPayload = null;

    /**
     * This Method will parse the CBOR byte[] data to JSON String
     *
     * @param payload
     *            : byte array of CBOR data
     * @return String : Converted JSON String of the CBOR data
     */
    public static String getPayloadAsJsonString(byte[] payload) {

        String payloadJsonString = "";
        String encodedDI = "";
        String decodedDI = "";
        if (payload != null) {
            JsonNode payloadJsonNode = getJsonNode(payload);
            if (payloadJsonNode != null) {
                payloadJsonString = payloadJsonNode.toString();

                if (payloadJsonString.contains("\"" + DI_KEY + "\"")) {
                    JsonNode diNode = payloadJsonNode.findValue(DI_KEY);
                    encodedDI = diNode.toString();

                    byte[] diBuffer = Base64.decodeBase64(encodedDI.getBytes());
                    decodedDI = OICHelper.bytesToHex(diBuffer);

                    Set<String> parts = new LinkedHashSet<>();
                    parts.add(decodedDI.substring(0, 8));
                    parts.add(decodedDI.substring(8, 12));
                    parts.add(decodedDI.substring(12, 16));
                    parts.add(decodedDI.substring(16, 20));
                    parts.add(decodedDI.substring(20, 32));
                    decodedDI = String.join("-", parts);

                    payloadJsonString = payloadJsonString.replace(encodedDI,
                            "\"" + decodedDI + "\"");
                }

                CTLogger.getInstance().debug("Payload Received: " + sCount++
                        + ": " + payloadJsonString);
            } else {
                CTLogger.getInstance().debug("Json Node from Payload is Null");
            }
        } else {
            CTLogger.getInstance().debug("Payload byte[] is Null");
        }

        return payloadJsonString;
    }

    /**
     * This Method will parse the CBOR byte[] data tto get the Payload Type
     * 
     * @param payload
     *            : byte array of CBOR data
     * @return OCPayloadType : Payload Type Enum
     */
    public static OCPayloadType getPayloadType(byte[] payload) {

        OCPayloadType payloadType = OCPayloadType.PAYLOAD_TYPE_INVALID;

        if (payload != null) {
            JsonNode jsonNode = getJsonNode(payload);

            if (jsonNode != null) {
                try {
                    if (jsonNode.get(0) != null) {
                        int index = Integer.valueOf(jsonNode.get(0).toString());
                        payloadType = OCPayloadType.values()[index];
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                    CTLogger.getInstance().error(e.getMessage());
                }

            } else {
                CTLogger.getInstance().debug("JsonNode is Null or Empty");
            }
        }

        return payloadType;
    }

    /**
     * This Method will Convert the JSON String Payload to CBOR byte array data
     * 
     * @param payload
     *            : JSON String payload
     * @param payloadType
     *            : Payload Type Enum
     * @return byte[] : returns the Converted CBOR byte[] data
     */
    @SuppressWarnings("unchecked")
    public static byte[] convertToCbor(String payload,
            OCPayloadType payloadType) {

        byte[] convertedPayload = null;

        if (!payload.isEmpty()) {
            JSONArray jsonArray = new JSONArray();
            ObjectMapper mapper = new ObjectMapper();
            JsonNode jsonNode = null;

            try {
                jsonNode = mapper.readTree(payload);

            } catch (IOException e) {
                e.printStackTrace();
                CTLogger.getInstance().error(e.getMessage());
            }

            // jsonArray.add(jsonNode);

            CTLogger.getInstance()
                    .info("PayLoad To Send [Before Adding Type] : " + payload
                            + "\n[After] : " + jsonNode.toString());

            CBORFactory cborFactory = new CBORFactory();
            ObjectMapper cborMapper = new ObjectMapper(cborFactory);

            try {
                convertedPayload = cborMapper.writeValueAsBytes(jsonNode);

            } catch (IOException e) {
                e.printStackTrace();
            }

        } else {
            CTLogger.getInstance().info("Payload to Send is empty");
        }
        System.out.println(getPayloadAsJsonString(convertedPayload));
        return convertedPayload;
    }

    private static JsonNode getJsonNode(byte[] payload) {
        CBORFactory cborFactory = new CBORFactory();
        ObjectMapper mapper = new ObjectMapper(cborFactory);
        JsonNode jsonNode = null;
        try {
            jsonNode = mapper.readTree(payload);
        } catch (JsonProcessingException e) {
            CTLogger.getInstance().debug(e.getMessage());
        } catch (IOException e) {
            CTLogger.getInstance().debug(e.getMessage());
        }

        return jsonNode;
    }

}

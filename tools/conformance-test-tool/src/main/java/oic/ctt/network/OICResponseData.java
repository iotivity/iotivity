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

package oic.ctt.network;

import oic.ctt.formatter.CborManager;
import oic.ctt.network.OICHelper.MessageParameters;

import org.ws4d.coap.interfaces.CoapResponse;

import java.util.HashMap;

/**
 * The OIC Response class. It provides a structure to contain response message
 * data. Provides abstraction over CoAP & HTTP.
 */
public class OICResponseData {
    private String                             mOptionSet;
    private OICClient.Protocol                 mProtocolType;
    private HashMap<MessageParameters, String> mResponseValue;

    /**
     * Initializes an OICResponse object with default protocol CoAP
     */
    public OICResponseData() {
        this.mProtocolType = OICClient.Protocol.COAP;
        mResponseValue = new HashMap<MessageParameters, String>();
    }

    /**
     * Sets a Response message parameter value
     * 
     * @param key
     *            MessageParameter key
     * @param value
     *            The value to be set against the key.
     */
    public void setResponseValue(MessageParameters key, String value) {
        mResponseValue.put(key, value);
    }

    /**
     * Gets a Response message parameter value
     * 
     * @param key
     *            MessageParameter key
     * @return The value of the given key
     */
    public String getResponseValue(MessageParameters key) {
        return mResponseValue.get(key);
    }

    /**
     * Gets the protocol type of the contained message
     * 
     * @return Protocol type (COAP or HTTP)
     */
    public OICClient.Protocol getProtocolType() {
        return mProtocolType;
    }

    /**
     * Sets the protocol type for the message
     * 
     * @param protocolType
     *            Protocol Type (COAP/HTTP)
     */
    public void setProtocolType(OICClient.Protocol protocolType) {
        this.mProtocolType = protocolType;
    }

    /**
     * Gets the HeaderOptions of COAP/HTTP
     * 
     * @return Set of options converted to string
     */
    public String getOptionSet() {
        return mOptionSet;
    }

    /**
     * Sets the HeaderOptions of COAP/HTTP
     * 
     * @param optionSet
     *            Set of options converted to string
     */
    public void setOptionSet(String optionSet) {
        this.mOptionSet = optionSet;
    }

    /**
     * Converts the CoAP response into more abstract OICResponse
     * 
     * @param coapResponse
     *            CoapResponse message object
     * @return OICResponse message object
     */
    public static OICResponseData convertResponseCoap(
            CoapResponse coapResponse) {
        if (coapResponse == null)
            return null;

        OICResponseData response = new OICResponseData();

        response.setProtocolType(OICClient.Protocol.COAP);
        response.setResponseValue(MessageParameters.msgType,
                coapResponse.getPacketType().toString());
        response.setResponseValue(MessageParameters.srcAddress,
                coapResponse.getSourceIP().split("/")[1]);
        response.setResponseValue(MessageParameters.srcPort,
                Integer.toString(coapResponse.getSourcePort()));
        response.setResponseValue(MessageParameters.responseCode,
                coapResponse.getResponseCodeString());
        response.setOptionSet(coapResponse.getHeaderOptions());
        response.setResponseValue(MessageParameters.token,
                OICHelper.bytesToHex(coapResponse.getToken()));
        response.setResponseValue(MessageParameters.mId,
                String.valueOf(coapResponse.getMessageID()));
        response.setResponseValue(MessageParameters.payload,
                CborManager.getPayloadAsJsonString(coapResponse.getPayload()));
        response.setResponseValue(MessageParameters.seqNumber,
                String.valueOf(coapResponse.getObserveOption()));

        if (coapResponse.getObserveOption() != null)
            response.setResponseValue(MessageParameters.observeFlag,
                    Integer.toString(coapResponse.getObserveOption()));

        return response;
    }

}

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

import org.ws4d.coap.interfaces.CoapRequest;

import java.util.HashMap;

/**
 * The OIC Request class. It provides a structure to contain request message
 * data. Provides abstraction over CoAP & HTTP.
 */
public class OICRequestData {

    /** Defines REST API methods */
    public static enum Method {
        GET, PUT, POST, DELETE;
    }

    private OICClient.Protocol                 mProtocolType;
    private String                             mOptionSet;
    private HashMap<MessageParameters, String> mRequestValue;

    /**
     * Initializes an OICRequest object with default protocol CoAP
     */
    public OICRequestData() {
        this.mProtocolType = OICClient.Protocol.COAP;
        mRequestValue = new HashMap<MessageParameters, String>();
    }

    /**
     * Sets a Request message parameter value
     * 
     * @param key
     *            MessageParameter key
     * @param value
     *            The value to be set against the key.
     */
    public void setRequestValue(MessageParameters key, String value) {
        mRequestValue.put(key, value);
    }

    /**
     * Gets a Request message parameter value
     * 
     * @param key
     *            MessageParameter key
     * @return The value of the given key
     */
    public String getRequestValue(MessageParameters key) {
        return mRequestValue.get(key);
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
     * Converts the CoAP Request into more abstract OICRequest
     * 
     * @param request
     *            CoAP Request message object
     * @return OIC Request message object
     */
    public static OICRequestData convertRequestCoap(CoapRequest request) {
        if (request == null)
            return null;

        OICRequestData oicRequest = new OICRequestData();

        oicRequest.setProtocolType(OICClient.Protocol.COAP);
        oicRequest.setRequestValue(MessageParameters.msgType, request
                .getPacketType().toString());
        oicRequest.setRequestValue(MessageParameters.srcAddress, request
                .getSourceIP().split("/")[1]);
        oicRequest.setRequestValue(MessageParameters.srcPort,
                Integer.toString(request.getSourcePort()));
        oicRequest.setRequestValue(MessageParameters.requestCode,
                request.getRequestCodeString());
        oicRequest.setOptionSet(request.getHeaderOptions());
        oicRequest.setRequestValue(MessageParameters.payload,
                CborManager.getPayloadAsJsonString(request.getPayload()));
        oicRequest.setRequestValue(MessageParameters.token,
                OICHelper.bytesToHex(request.getToken()));
        oicRequest.setRequestValue(MessageParameters.mId,
                Integer.toString(request.getMessageID()));

        if (request.getObserveOption() != null)
            oicRequest.setRequestValue(MessageParameters.observeFlag,
                    Integer.toString(request.getObserveOption()));

        return oicRequest;
    }
}

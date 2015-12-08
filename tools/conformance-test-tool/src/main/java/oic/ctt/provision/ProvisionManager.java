//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= 
package oic.ctt.provision;

import java.util.ArrayList;

import oic.ctt.logger.CTLogger;
import oic.ctt.network.OICClient;
import oic.ctt.network.OICResponseData;
import oic.ctt.network.OICClient.Protocol;
import oic.ctt.network.OICHelper.MessageParameters;

import org.slf4j.Logger;

public class ProvisionManager {

    private Logger mlogger = CTLogger.getInstance();

    public ProvisionManager() {

    }

    public OICResponseData getMulticastDiscoveryResponse(Protocol protocol,
            String dutUri, String query) {
        OICClient client = new OICClient();
        client.setWaitTime(2);

        ArrayList<OICResponseData> discoveryResponses = client
                .discoverResource(protocol, dutUri, query);
        OICResponseData validResponse = null;

        for (OICResponseData dRes : discoveryResponses) {
            validResponse = dRes;

            if (dRes.getResponseValue(MessageParameters.responseCode).equals(
                    "2.05")) {
                break;
            }
        }

        return validResponse;
    }

    public String getResponseCode(OICResponseData response) {
        return response.getResponseValue(MessageParameters.responseCode);
    }

    public String getSecuredServerIp(OICResponseData response) {
        return response.getResponseValue(MessageParameters.srcAddress);
    }

    public String getSecuredServerPort(OICResponseData response) {
        return response.getResponseValue(MessageParameters.srcPort);
    }

    public String getResponsePayload(OICResponseData response) {
        return response.getResponseValue(MessageParameters.payload);
    }
}

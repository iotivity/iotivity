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

package oic.ctt.network.http;

import java.util.ArrayList;

import oic.ctt.formatter.OCPayloadType;
import oic.ctt.network.OICHelper;
import oic.ctt.network.OICProtocolClient;
import oic.ctt.network.OICRequestData;
import oic.ctt.network.OICResponseData;
import oic.ctt.network.OICHelper.MessageType;
import oic.ctt.network.OICRequestData.Method;

/**
 * The OIC HTTP Client class. Provides methods to communicate with a OIC client
 * using HTTP.
 */
public class OICHttpClient implements OICProtocolClient {

    // TODO: Implement a HTTP Client here. Provide methods to facilitate HTTP
    // communication.

    @Override
    public void setWaitTime(int waitTime) {

    }

    @Override
    public ArrayList<OICResponseData> getResponses() {
        return null;
    }

    @Override
    public void clearResponses() {

    }

    @Override
    public ArrayList<OICResponseData> getNotifications() {
        return null;
    }

    @Override
    public void clearNotifications() {

    }

    @Override
    public ArrayList<OICResponseData> discoverResource(String uriPath,
            String query, String ip, int port, byte[] token) {
        return null;
    }

    @Override
    public OICResponseData sendRequest(OICHelper.MessageType type,
            Method method, int messageId, byte[] token, String ip, int port,
            String uriPath, String query, String payload,
            OCPayloadType payloadType) {
        return null;
    }

    @Override
    public void cancelObservePassively(byte[] token) {

    }

    @Override
    public void cancelObserveWithReset(byte[] token) {

    }

    /**
     * Reset observe Status
     *
     */
    @Override
    public void resetObserveStatus() {

    }

    @Override
    public OICResponseData observeResource(MessageType type, int messageId,
            byte[] token, String ip, int port, String uriPath, String query) {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public OICResponseData cancelObserveWithGetMessage(MessageType type,
            int messageId, byte[] token, String ip, int port, String uriPath,
            String query) {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public OICRequestData getLastRequest() {
        // TODO Auto-generated method stub
        return null;
    }
}

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

package oic.ctt.network;

import java.util.ArrayList;

import oic.ctt.formatter.OCPayloadType;

/**
 * OIC Client interface. Defines which methods must be implemented in a client
 * irrespective of protocol.
 */
public interface OICProtocolClient {

    public void setWaitTime(int waitTime);

    public ArrayList<OICResponseData> getResponses();

    public void clearResponses();

    public ArrayList<OICResponseData> getNotifications();

    public void clearNotifications();

    public ArrayList<OICResponseData> discoverResource(String uriPath,
            String query, String ip, int port, byte[] token);

    public OICResponseData sendRequest(OICHelper.MessageType type,
            OICRequestData.Method method, int messageId, byte[] token,
            String ip, int port, String uriPath, String query, String payload,
            OCPayloadType payloadType);

    public void observeResource(OICHelper.MessageType type, int messageId,
            byte[] token, String ip, int port, String uriPath, String query);

    public void cancelObservePassively();

    public void cancelObserveWithReset();

    public OICResponseData cancelObserveWithGetMessage(
            OICHelper.MessageType type, int messageId, String ip, int port,
            String uriPath, String query);
}

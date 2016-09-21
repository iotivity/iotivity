/******************************************************************
*
* Copyright 2016 Samsung Electronics All Rights Reserved.
*
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/

package org.iotivity;

import android.content.Context;
import android.app.Activity;

public class CAJni {

    static {
        System.loadLibrary("CAJni");
    }

    public static native void setNativeResponseListener(Object listener);

    public static native boolean setContext(Context context, Activity activity);

    public static native boolean readConfig();

    public static native void setupTestCase(int intoutType, int messageDataType,
            int communicationType);

    public static native boolean initialize();

    public static native void terminate();

    public static native void registerHandler(int requestHandler,
            int responseHandler);

    public static native boolean setDtls();

    public static native boolean startListeningServer(int expectedResult);

    public static native boolean stopListeningServer(int expectedResult);

    public static native boolean startDiscoveryServer(int expectedResult);

    public static native boolean registerHandler();

    public static native boolean selectNetwork();

    public static native boolean selectCustomNetwork(long interestedNetwork, int expectedResult);

    public static native boolean unSelectNetwork();

    public static native boolean unSelectCustomNetwork(long uninterestedNetwork, int expectedResult);

    public static native boolean generateToken(int tokenType, int tokenLength, int expectedResult);

    public static native boolean destroyToken();

    public static native boolean createEndpoint(int endpointType,
            int expectedResult);

    public static native boolean destroyEndpoint(int endpointType);

    public static native boolean handleRequestResponse();

    public static native boolean getNetworkInfomation(int endpointType, int sizeType,
            int expectedResult);

    public static native boolean initNetwork();

    public static native boolean initServerNetwork();

    public static native boolean initClientNetwork();

    public static native boolean checkReceiveCount(int count);

    public static native boolean establishConnectionWithServer();

    public static native boolean sendRequest(int uriType, int payloadType,
            int msgType, int methodType, int numOfHeader, boolean isSecured);

    public static native boolean sendRequestToAll(int uriType, int payloadType,
            int msgType, int methodType, int numOfHeader);

    public static native boolean sendResponse(int uriType, int payloadType,
            int msgType, int responseValue, int numOfHeader);

    public static native boolean sendConfigurationRequest(int taskType, int msgType,
            int method);

    public void sleep(int seconds) {
        try {
            Thread.sleep(seconds * 1000);
        } catch (Exception ex) {

        }
    }
}

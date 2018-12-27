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

    private org.iotivity.ca.simulator.MainActivity mResponseListener = null;

    public native void setNativeResponseListener(Object listener);

    public native void initialize(Context context, Activity activity);

    public native void terminate();

    public native void startListeningServer();

    public native void startDiscoveryServer();

    public native void registerHandler();

    public native void sendRequest(String uri, String payload,
            int selectedNetwork, int isSecured, int msgType, boolean isBigData);

    public native void sendReqestToAll(String uri, int selectedNetwork);

    public native void RMSendResponse(int selectedNetwork, int isSecured,
            int msgType, int responseValue);

    public native void selectNetwork(int interestedNetwork);

    public native void unSelectNetwork(int uninterestedNetwork);

    public native void getNetworkInfomation();

    public native void handleRequestResponse();

    public void setResponseListener(org.iotivity.ca.simulator.MainActivity listener) {
        mResponseListener = listener;
        setNativeResponseListener(this);
    }

    private void OnResponseReceived(String subject, String receivedData) {
        if (null != mResponseListener) {
            mResponseListener.OnResponseReceived(subject, receivedData);
        }
    }

}

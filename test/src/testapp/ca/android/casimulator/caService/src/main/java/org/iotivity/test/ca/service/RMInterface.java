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

package org.iotivity.test.ca.service;

import android.content.Context;
import android.app.Activity;

public class RMInterface {

    static {
        // Load RI JNI interface
        System.loadLibrary("RMInterface");
    }

    private org.iotivity.test.ca.service.MainActivity mResponseListener = null;

    public native void setNativeResponseListener(Object listener);

    public native void RMInitialize(Context context, Activity activity);

    public native void RMTerminate();

    public native void RMStartListeningServer();

    public native void RMStartDiscoveryServer();

    public native void RMRegisterHandler();

    public native void RMFindResource(String uri);

    public native void RMSendRequest(String uri, String payload,
            int selectedNetwork, int isSecured, int msgType, boolean isBigData);

    public native void RMSendReqestToAll(String uri, int selectedNetwork);

    public native void RMSendResponse(int selectedNetwork, int isSecured,
            int msgType, int responseValue);

    public native void RMAdvertiseResource(String advertiseResource);

    public native void RMSendNotification(String uri, String payload,
            int selectedNetwork, int isSecured, int msgType);

    public native void RMSelectNetwork(int interestedNetwork);

    public native void RMUnSelectNetwork(int uninterestedNetwork);

    public native void RMGetNetworkInfomation();

    public native void RMHandleRequestResponse();

    public void setResponseListener(org.iotivity.test.ca.service.MainActivity listener) {
        mResponseListener = listener;
        setNativeResponseListener(this);
    }

    private void OnResponseReceived(String subject, String receivedData) {
        if (null != mResponseListener) {
            mResponseListener.OnResponseReceived(subject, receivedData);
        }
    }

}

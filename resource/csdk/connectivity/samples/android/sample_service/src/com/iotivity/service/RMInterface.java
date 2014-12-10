
package com.iotivity.service;

import android.content.Context;

public class RMInterface {

    static {
        // Load RI JNI interface
        System.loadLibrary("RMInterface");
        
        // Load CA JNI interface
        System.loadLibrary("CAInterface");
    }

    public native int RMInitialize(Context context);

    public native void RMTerminate();

    public native int RMStartListeningServer();

    public native int RMStartDiscoveryServer();

    public native int RMRegisterHandler();

    public native int RMFindResource(String uri);

    public native int RMSendRequest(String requestData);

    public native int RMSendResponse(String responseData);

    public native int RMSendNotification(String notification);

    public native int RMSelectNetwork(int interestedNetwork);

    public native int RMHandleRequestResponse();

}

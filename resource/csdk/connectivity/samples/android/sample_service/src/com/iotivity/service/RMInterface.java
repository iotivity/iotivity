
package com.iotivity.service;

import android.content.Context;

public class RMInterface {

    static {
        // Load RI JNI interface
        System.loadLibrary("RMInterface");
        
        // Load CA JNI interface
        System.loadLibrary("CAInterface");
    }

    public native void RMInitialize(Context context);

    public native void RMTerminate();

    public native void RMStartListeningServer();

    public native void RMStartDiscoveryServer();

    public native void RMRegisterHandler();

    public native void RMFindResource(String uri);

    public native void RMSendRequest(String requestUri, int selectedNetwork, int isSecured, int msgType);

    public native void RMSendNotification(String notification, int selectedNetwork);

    public native void RMSelectNetwork(int interestedNetwork);

    public native void RMHandleRequestResponse();

}

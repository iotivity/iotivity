package org.iotivity.service.easysetup.mediator;

import android.content.Context;
import android.util.Log;

import org.iotivity.base.OcConnectivityType;
import org.iotivity.service.easysetup.mediator.common.ProvisioningInfo;
import org.iotivity.service.easysetup.mediator.ip.IPProvisioningInfo;

public class ProvisionEnrollee {
    private static final String   TAG                            = "ProvisionEnrollee";
    private EasySetupManager      easySetupManagerNativeInstance = null;
    private IProvisioningListener provisioningListener;

    static {
        // Load Easy Setup JNI interface
        System.loadLibrary("gnustl_shared");
        System.loadLibrary("octbstack");
        System.loadLibrary("connectivity_abstraction");
        System.loadLibrary("easysetup-jni");
    }
    /**
     * Constructor for ProvisionEnrollee. Constructs a new ProvisionEnrollee.
     */
    public ProvisionEnrollee(Context context) {
        easySetupManagerNativeInstance = EasySetupManager.getInstance();
    }
    
    @Override
    protected void finalize() throws Throwable {
        super.finalize();
        easySetupManagerNativeInstance.terminateEasySetup();
    }

    public void provisionEnrollee(ProvisioningInfo provisioningInfo,
                                  OcConnectivityType connectivityType) {
        if(connectivityType == OcConnectivityType.IPV4)
        {
            easySetupManagerNativeInstance.initEasySetup();
            easySetupManagerNativeInstance.provisionIPEnrollee(
                    ((IPProvisioningInfo)provisioningInfo).getIpAddress(),
                    ((IPProvisioningInfo)provisioningInfo).getNetSSID(),
                    ((IPProvisioningInfo)provisioningInfo).getNetPWD(),
                    OcConnectivityType.IPV4.getValue());
        }
    }

    public void stopEnrolleeProvisioning(OcConnectivityType connectivityType) {
        easySetupManagerNativeInstance
                .stopEnrolleeProvisioning(OcConnectivityType.IPV4.getValue());
    }

    public void ProvisioningStatusCallBack(int statuscode) {
        // TODO Auto-generated method stub
        Log.d(TAG,
                "onFinishProvisioning() inside Android Java application. statuscode - "
                        + statuscode);
        this.provisioningListener.onFinishProvisioning(statuscode);
    }

    public void registerProvisioningHandler(
            IProvisioningListener provisioningListener) {
        this.provisioningListener = provisioningListener;
        EasySetupCallbackHandler.getInstance()
                .registerProvisioningHandler(this);
    }
}

package org.iotivity.service.easysetup.mediator;

import android.content.Context;
import android.util.Log;

public class ProvisionEnrollee {
    private static final String   TAG                            = "ProvisionEnrollee";
    private Context               appContext                     = null;
    private EasySetupManager      easySetupManagerNativeInstance = null;
    private IProvisioningListener provisioningListener;

    /**
     * Constructor for ProvisionEnrollee. Constructs a new ProvisionEnrollee.
     */
    public ProvisionEnrollee(Context context) {
        appContext = context;
        easySetupManagerNativeInstance = EasySetupManager.getInstance();
        easySetupManagerNativeInstance.initEasySetup();
    }
    
    @Override
    protected void finalize() throws Throwable {
        super.finalize();
        easySetupManagerNativeInstance.terminateEasySetup();
    }

    public void provisionEnrollee(String ipAddress, String netSSID,
            String netPWD, int connectivityType) {
        easySetupManagerNativeInstance.provisionEnrollee(ipAddress, netSSID,
                netPWD, connectivityType);
    }

    public void stopEnrolleeProvisioning(int connectivityType) {
        easySetupManagerNativeInstance
                .stopEnrolleeProvisioning(connectivityType);
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

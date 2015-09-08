package org.iotivity.service.easysetup.core;

public abstract class EnrolleeDevice {

    protected EnrolleeState mState;
    private EnrolleeSetupError mError;

    protected  ConnectionInterface mConnection;
    protected final ProvisioningConfig mProvConfig;
    protected final OnBoardingConfig mOnBoardingConfig;

    protected OnBoardingCallback mOnBoardingCallback;
    protected ProvisioningCallback mProvisioningCallback;

    protected EnrolleeDevice(OnBoardingConfig onBoardingConfig, ProvisioningConfig provConfig) {
        mProvConfig = provConfig;
        mOnBoardingConfig = onBoardingConfig;
    }


    // Application can override this API to provide their own implementation for on boarding.
    protected abstract void startOnBoardingProcess();
    protected abstract void stopOnBoardingProcess();

    protected abstract void startProvisioningProcess(ConnectionInterface conn);

    public void setConnection(ConnectionInterface conn) {
        mConnection = conn;
    }

    // Not accessible by the Application
    void startOnBoarding(OnBoardingCallback onBoardingCallback){
        mOnBoardingCallback = onBoardingCallback;
        startOnBoardingProcess();
    }

    // Not accessible by the Application
    void startProvisioning(ProvisioningCallback provisioningCallback){
        mProvisioningCallback = provisioningCallback;
        startProvisioningProcess(mConnection);
    }


    public boolean isSetupSuccessful(){
        return (mState == EnrolleeState.DEVICE_PROVISIONING_SUCCESS_STATE) ? true : false;
    }

    public EnrolleeSetupError getError() {
        return mError;
    }

    public EnrolleeState getState() {
        return mState;
    }

    public boolean onBoarded(){
        return ( mState == EnrolleeState.DEVICE_PROVISIONING_STATE ) ? true : false;
    }

}

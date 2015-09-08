package org.iotivity.service.easysetup.core;

import android.content.Context;
import android.util.Log;

import org.iotivity.service.easysetup.impl.EnrolleeDeviceFactory;

import java.io.IOException;
import java.util.ArrayList;

public class EasySetupService {

    private static EasySetupService sInstance ;

    private final EasySetupStatus mCallback;

    private ArrayList<EnrolleeDevice>mEnrolleeDeviceList;

    private final ProvisioningCallback mProvisioningCallback;

    private static Context mContext;

    public EnrolleeDeviceFactory mDeviceFactory;


    private EasySetupService(EasySetupStatus callback) {
        mCallback = callback;
        mProvisioningCallback = new ProvisioningCallbackImpl(mCallback);
        mEnrolleeDeviceList = new ArrayList<EnrolleeDevice> ();
        mContext = null;
        mDeviceFactory = null;
    }

    // TODO : Implement singleton method
    public synchronized static EasySetupService getInstance(Context context, EasySetupStatus callback) {
        if(sInstance == null) {
            sInstance = new EasySetupService(callback);
            mContext = context;
        }
        return sInstance ;
    }

    public synchronized void startSetup (final EnrolleeDevice enrolledevice) throws IOException{

        mEnrolleeDeviceList.add(enrolledevice);

        // Starts the provisioning directly if the device is already on boarded on the network.
        if(enrolledevice.onBoarded()) {
            enrolledevice.startProvisioning(mProvisioningCallback);
            return;
        }

        enrolledevice.startOnBoarding(new OnBoardingCallback() {

            @Override
            public void onFinished(ConnectionInterface connection) {
                if(connection.isConnected()) {
                    try {
                        Thread.sleep(15000);//Sleep for allowing thin device to start the services
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }

                    // Start provisioning here
                    enrolledevice.setConnection(connection);
                    enrolledevice.startProvisioning(mProvisioningCallback);
                }else{
                    enrolledevice.mState = EnrolleeState.DEVICE_PROVISIONING_FAILED_STATE;
                    mProvisioningCallback.onFinished(enrolledevice);
                }

            }

        });

    }

    public synchronized void stopSetup (EnrolleeDevice enrolledevice) {
        enrolledevice.stopOnBoardingProcess();
        mEnrolleeDeviceList.remove(enrolledevice);
    }

    public synchronized void getEnrolleeDevice (OnBoardingConfig connType) {
        mDeviceFactory = EnrolleeDeviceFactory.newInstance(mContext);
    }

    class ProvisioningCallbackImpl extends ProvisioningCallback {

        private final EasySetupStatus mCallback ;

        ProvisioningCallbackImpl (EasySetupStatus callback) {
            mCallback  = callback;
        }

        @Override
        public void onFinished(EnrolleeDevice enrolledevice) {
            //if(mEnrolleeDeviceList.contains(enrolledevice)) {
                mCallback.onFinished(enrolledevice);
            //}
        }

    }


}

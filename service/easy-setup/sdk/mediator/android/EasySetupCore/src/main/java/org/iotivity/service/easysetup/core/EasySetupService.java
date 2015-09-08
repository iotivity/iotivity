package org.iotivity.service.easysetup.core;

import android.content.Context;
import android.util.Log;

import org.iotivity.service.easysetup.impl.EnrolleeDeviceFactory;

import java.io.IOException;
import java.util.ArrayList;

/**
 * This is facade class, a single point of contact for Application.
 * It contains set of APIs to do easy setup of the enrolling device.
 * ON-BOARDING - This is a step to establish connectivity between the device & Mediator device.
 * PROVISION   - This is a step where the netowork's detail & credentials are given to the enrolling device.
 */
public class EasySetupService {

    private static EasySetupService sInstance;

    private final EasySetupStatus mCallback;

    private ArrayList<EnrolleeDevice> mEnrolleeDeviceList;

    private final ProvisioningCallback mProvisioningCallback;

    private static Context mContext;

    public EnrolleeDeviceFactory mDeviceFactory;


    private EasySetupService(EasySetupStatus callback) {
        mCallback = callback;
        mProvisioningCallback = new ProvisioningCallbackImpl(mCallback);
        mEnrolleeDeviceList = new ArrayList<EnrolleeDevice>();
        mContext = null;
        mDeviceFactory = null;
    }

    /**
     * Gives a singleton instance of Easy setup service.
     *
     * @param callback Application needs to provide this callback to receive the status of of easy setup process.
     */

    public synchronized static EasySetupService getInstance(Context context, EasySetupStatus callback) {
        if (sInstance == null) {
            sInstance = new EasySetupService(callback);
            mContext = context;
        }
        return sInstance;
    }

    /**
     * Starts Easy setup process for the enrolling device.
     *
     * @param enrolledevice Device to be enrolled in network
     * @throws IOException Throws exception in case of any connection error.
     */

    public synchronized void startSetup(final EnrolleeDevice enrolledevice) throws IOException {

        mEnrolleeDeviceList.add(enrolledevice);

        // Starts the provisioning directly if the device is already on boarded on the network.
        if (enrolledevice.onBoarded()) {
            enrolledevice.startProvisioning(mProvisioningCallback);
            return;
        }

        enrolledevice.startOnBoarding(new OnBoardingCallback() {

            @Override
            public void onFinished(ConnectionInterface connection) {
                if (connection.isConnected()) {
                    try {
                        Thread.sleep(15000);//Sleep for allowing thin device to start the services
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }

                    // Start provisioning here
                    enrolledevice.setConnection(connection);
                    enrolledevice.startProvisioning(mProvisioningCallback);
                } else {
                    enrolledevice.mState = EnrolleeState.DEVICE_PROVISIONING_FAILED_STATE;
                    mProvisioningCallback.onFinished(enrolledevice);
                }

            }

        });

    }

    /**
     * Stops on-going Easy setup process for enrolling device.
     *
     * @param enrolledevice Device to be enrolled in network
     */
    public synchronized void stopSetup(EnrolleeDevice enrolledevice) {
        enrolledevice.stopOnBoardingProcess();
        mEnrolleeDeviceList.remove(enrolledevice);
    }

    public synchronized void getEnrolleeDevice(OnBoardingConfig connType) {
        mDeviceFactory = EnrolleeDeviceFactory.newInstance(mContext);
    }

    class ProvisioningCallbackImpl extends ProvisioningCallback {

        private final EasySetupStatus mCallback;

        ProvisioningCallbackImpl(EasySetupStatus callback) {
            mCallback = callback;
        }

        @Override
        public void onFinished(EnrolleeDevice enrolledevice) {
            //if(mEnrolleeDeviceList.contains(enrolledevice)) {
            mCallback.onFinished(enrolledevice);
            //}
        }

    }


}

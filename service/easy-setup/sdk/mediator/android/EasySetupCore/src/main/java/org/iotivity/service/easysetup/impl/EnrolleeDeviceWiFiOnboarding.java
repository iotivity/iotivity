package org.iotivity.service.easysetup.impl;

import java.util.Timer;
import java.util.TimerTask;

import org.iotivity.service.easysetup.core.ConnectionInterface;
import org.iotivity.service.easysetup.core.EnrolleeDevice;
import org.iotivity.service.easysetup.core.EnrolleeState;
import org.iotivity.service.easysetup.core.IpConnection;
import org.iotivity.service.easysetup.core.OnBoardingConfig;
import org.iotivity.service.easysetup.core.ProvisioningConfig;
import org.iotivity.service.easysetup.mediator.EasySetupCallbackHandler;
import org.iotivity.service.easysetup.mediator.EasySetupManager;
import org.iotivity.service.easysetup.mediator.EnrolleeInfo;
import org.iotivity.service.easysetup.mediator.IOnBoardingStatus;
import org.iotivity.service.easysetup.mediator.IProvisioningListener;
import org.iotivity.service.easysetup.mediator.ProvisionEnrollee;
import org.iotivity.service.easysetup.mediator.ip.WiFiSoftAPManager;

import android.content.Context;
import android.net.wifi.WifiConfiguration;
import android.util.Log;
import android.widget.Toast;

/**
 * This is a ready to use class for Enrollee device having Soft AP as on-boarding connectivity.
 */
public class EnrolleeDeviceWiFiOnboarding extends EnrolleeDevice {

    public static final String TAG = EnrolleeDeviceWiFiOnboarding.class.getName();

    final Context mContext;
    final WiFiSoftAPManager mWifiSoftAPManager;
    EnrolleeInfo connectedDevice;
    private EasySetupManager easySetupManagerNativeInstance;
    ProvisionEnrollee provisionEnrolleInstance;

    IOnBoardingStatus deviceScanListener = new IOnBoardingStatus() {

        @Override
        public void deviceOnBoardingStatus(EnrolleeInfo enrolleStatus) {
            Log.d("ESSoftAPOnBoarding", "Entered");
            if (enrolleStatus != null && enrolleStatus.getIpAddr() != null) {
                String finalResult = "Easy Connect : ";

                if (enrolleStatus.isReachable()) {
                    finalResult = "Device OnBoarded" + "["
                            + enrolleStatus.getIpAddr() + "]";

                        /*
                         * easySetupInstance.StartEasySetup(enrolleStatus.getIpAddr()) ;
                         * easySetupCount++; Log.i("EasyConnect",
                         * "easy Setup Count-"+easySetupCount); Log.i("EasyConnect",
                         * "IP Address-"+enrolleStatus.getIpAddr());
                         */
                    connectedDevice = enrolleStatus;
                    IpConnection conn = new IpConnection();
                    conn.setConnectivity(true);
                    conn.setIp(connectedDevice.getIpAddr());
                    Log.d("ESSoftAPOnBoarding", "Entered");
                    mOnBoardingCallback.onFinished(conn);
                    return;

                }

/*                    textView1.setText("");
                    textView1.append("Clients: \n");
                    textView1.append("####################\n");
                    textView1.append("IP Address     : " + enrolleStatus.getIpAddr()
                            + "\n");
                    textView1.append("Device          : " + enrolleStatus.getDevice() + "\n");
                    textView1.append("HW Address     : " + enrolleStatus.getHWAddr()
                            + "\n");
                    textView1.append("Is OnBoarded    : " + enrolleStatus.isReachable()
                            + "\n");

                    Toast.makeText(getApplicationContext(), finalResult,
                            Toast.LENGTH_LONG).show();*/


            }

            IpConnection conn = new IpConnection();
            conn.setConnectivity(false);
            mOnBoardingCallback.onFinished(conn);
        }
    };


    protected EnrolleeDeviceWiFiOnboarding(Context context, OnBoardingConfig onBoardingConfig, ProvisioningConfig provConfig) {
        super(onBoardingConfig, provConfig);
        mContext = context;
        mWifiSoftAPManager = new WiFiSoftAPManager(mContext);
    }

    @Override
    protected void startOnBoardingProcess() {
        // TODO Auto-generated method stub

        //2. Get call back once enrolling device connect to the Soft AP
        //3. fetch the IP assigned to enrolling device
        //4. Call below steps

        Log.i(TAG, "Starging on boarding process");

        //1. Create Soft AP
        boolean status = mWifiSoftAPManager.setWifiApEnabled((WifiConfiguration) mOnBoardingConfig.getConfig(), true);

        Log.i(TAG, "Soft AP is created with status " + status);


        Timer myTimer;
        myTimer = new Timer();
        myTimer.schedule(new TimerTask() {
            @Override
            public void run() {
                // Below function to be called after 5 seconds
                mWifiSoftAPManager.getClientList(deviceScanListener, 300);
            }

        }, 0, 5000);


    }

    protected void stopOnBoardingProcess() {
        Log.i(TAG, "Stopping on boarding process");
        boolean status = mWifiSoftAPManager.setWifiApEnabled(null, false);
        Log.i(TAG, "Soft AP is disabled with status " + status);
    }

    @Override
    protected void startProvisioningProcess(ConnectionInterface conn) {

        if (mProvConfig.getConnType() == ProvisioningConfig.ConnType.WiFi) {

            provisionEnrolleInstance = new ProvisionEnrollee(mContext);
            provisionEnrolleInstance.registerProvisioningHandler(new IProvisioningListener() {
                @Override
                public void onFinishProvisioning(int statuscode) {
                    mState = (statuscode == 0) ? EnrolleeState.DEVICE_PROVISIONING_SUCCESS_STATE : EnrolleeState.DEVICE_PROVISIONING_FAILED_STATE;
                    mProvisioningCallback.onFinished(EnrolleeDeviceWiFiOnboarding.this);
                }
            });

            IpConnection connection = (IpConnection) conn;
            WiFiProvConfig wifiProvConfig = (WiFiProvConfig) mProvConfig;
            easySetupManagerNativeInstance = EasySetupManager.getInstance();
            easySetupManagerNativeInstance.initEasySetup();
            easySetupManagerNativeInstance.provisionEnrollee(connection.getIp(), wifiProvConfig.getSsId(), wifiProvConfig.getPassword(), 0 /*In base code '0' is hard coded*/);

        }

    }


}

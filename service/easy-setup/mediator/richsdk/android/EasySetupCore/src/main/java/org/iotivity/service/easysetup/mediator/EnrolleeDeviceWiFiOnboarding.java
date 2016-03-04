/**
 * ***************************************************************
 * <p/>
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 * <p/>
 * <p/>
 * <p/>
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * <p/>
 * http://www.apache.org/licenses/LICENSE-2.0
 * <p/>
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * <p/>
 * ****************************************************************
 */

package org.iotivity.service.easysetup.mediator;

import java.util.Timer;
import java.util.TimerTask;

import org.iotivity.service.easysetup.core.EnrolleeInfo;
import org.iotivity.service.easysetup.core.IOnBoardingStatus;
import org.iotivity.service.easysetup.core.ip.WiFiSoftAPManager;

import android.content.Context;
import android.net.wifi.WifiConfiguration;
import android.util.Log;

/**
 * This is a ready to use class for Enrollee device having Soft AP as on-boarding connectivity.
 */
public class EnrolleeDeviceWiFiOnboarding extends EnrolleeDevice {

    public static final String TAG = EnrolleeDeviceWiFiOnboarding.class.getName();

    final Context mContext;
    final WiFiSoftAPManager mWifiSoftAPManager;
    EnrolleeInfo connectedDevice;
    Timer myTimer = null;

    IOnBoardingStatus deviceScanListener = new IOnBoardingStatus() {

        @Override
        public void deviceOnBoardingStatus(EnrolleeInfo enrolleStatus) {
            myTimer.cancel();
            Log.d("ESSoftAPOnBoarding", "Entered");
            if (mState == EnrolleeState.DEVICE_ON_BOARDING_STATE) {
                Log.d("ESSoftAPOnBoarding", "Device in OnBoarding State");
                if (enrolleStatus != null && enrolleStatus.getIpAddr() != null) {
                    String finalResult = "Easy Connect : ";

                    if (enrolleStatus.isReachable()) {
                        finalResult = "Device OnBoarded" + "["
                                + enrolleStatus.getIpAddr() + "]";

                        connectedDevice = enrolleStatus;
                        IpOnBoardingConnection conn = new IpOnBoardingConnection();

                        conn.setConnectivity(true);
                        conn.setIp(connectedDevice.getIpAddr());
                        conn.setHardwareAddress(enrolleStatus.getHWAddr());
                        conn.setDeviceName(enrolleStatus.getDevice());

                        Log.d("ESSoftAPOnBoarding", "Entered" + finalResult);
                        mOnBoardingCallback.onFinished(conn);
                        return;

                    }
                }

                IpOnBoardingConnection conn = new IpOnBoardingConnection();
                conn.setConnectivity(false);
                mOnBoardingCallback.onFinished(conn);
            } else {
                Log.e("ESSoftAPOnBoarding", "Device NOT in OnBoarding State. Ignoring the event");
            }
        }
    };


    protected EnrolleeDeviceWiFiOnboarding(Context context, OnBoardingConfig onBoardingConfig,
                                           ProvisioningConfig provConfig) {
        super(onBoardingConfig, provConfig);
        mContext = context;
        mState = EnrolleeState.DEVICE_INIT_STATE;
        mWifiSoftAPManager = new WiFiSoftAPManager(mContext);
    }

    protected EnrolleeDeviceWiFiOnboarding(Context context, IpOnBoardingConnection conn,
                                           ProvisioningConfig provConfig) {
        super(new WiFiOnBoardingConfig(), provConfig);
        mContext = context;
        mState = EnrolleeState.DEVICE_ON_BOARDED_STATE;
        mConnection = conn;
        mWifiSoftAPManager = new WiFiSoftAPManager(mContext);
    }

    @Override
    protected void startOnBoardingProcess() {
        Log.i(TAG, "Starting on boarding process");

        //1. Create Soft AP
        boolean status = mWifiSoftAPManager.setWifiApEnabled((WifiConfiguration)
                mOnBoardingConfig.getConfig(), true);

        mState = EnrolleeState.DEVICE_ON_BOARDING_STATE;

        Log.i(TAG, "Soft AP is created with status " + status);

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
        if(myTimer != null)
        {
            myTimer.cancel();
        }
        boolean status = mWifiSoftAPManager.setWifiApEnabled(null, false);
        Log.i(TAG, "Soft AP is disabled with status " + status);
    }

    @Override
    protected void startProvisioningProcess(OnBoardingConnection conn)  {

        mState = EnrolleeState.DEVICE_PROVISIONING_STATE;
        mProvisioningCallback.onProgress(this);
        final EnrolleeDevice device = this;
        if (mProvConfig.getConnType() == ProvisioningConfig.ConnType.WiFi) {
             try {
                 mRemoteEnrollee.registerProvisioningHandler(new IProvisionStatusNativeHandler() {
                     @Override
                     public void onStatusRecieved(int state) {
                         device.mState = convertIntToProvisioningState(state);
                         Log.i(TAG,"Device state changed :"+device.mState);
                         mProvisioningCallback.onProgress(device);
                         if(0==state) {
                             mProvisioningCallback.onFinished(EnrolleeDeviceWiFiOnboarding.this);
                         }
                     }
                 });
                 //native call
                mRemoteEnrollee.startProvision();
             }catch(ESException e) {
                 Log.i(TAG,"startProvisioningProcess Register Listener to native exception");
             }
        }
    }

    protected void stopProvisioningProcess(){
        if(mState == EnrolleeState.DEVICE_PROVISIONING_STATE)
        {    //native call
            try {
                mRemoteEnrollee.stopProvision();
            } catch (ESException e) {
                Log.i(TAG,"stopProvisioningProcess exception");
            }
        }else{
           Log.i(TAG,"stopProvisioningProcess : Provisioning is not in progress");
        }
    }

    private EnrolleeState convertIntToProvisioningState(int state){

        switch(state)
        {
            case 0 :
                return EnrolleeState.DEVICE_PROVISIONED_STATE;
            case 1:
                return EnrolleeState.DEVICE_NOT_PROVISIONED;
            case 2:
                return EnrolleeState.DEVICE_PROVISIONED_STATE;
            case 3:
                return EnrolleeState.DEVICE_NOT_PROVISIONED;
        }
        return EnrolleeState.DEVICE_INIT_STATE;
    }
}

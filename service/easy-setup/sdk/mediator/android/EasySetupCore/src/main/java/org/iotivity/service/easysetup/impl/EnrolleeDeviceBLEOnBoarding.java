//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
package org.iotivity.service.easysetup.impl;

import android.content.Context;
import android.util.Log;

import org.iotivity.base.OcConnectivityType;
import org.iotivity.service.easysetup.core.BleConnection;
import org.iotivity.service.easysetup.core.OnBoardingConnection;
import org.iotivity.service.easysetup.core.EnrolleeDevice;
import org.iotivity.service.easysetup.core.EnrolleeState;
import org.iotivity.service.easysetup.core.OnBoardingConfig;
import org.iotivity.service.easysetup.core.ProvisioningConfig;
import org.iotivity.service.easysetup.mediator.EasySetupManager;
import org.iotivity.service.easysetup.mediator.EnrolleeInfo;
import org.iotivity.service.easysetup.mediator.IOnBoardingStatus;
import org.iotivity.service.easysetup.mediator.IProvisioningListener;
import org.iotivity.service.easysetup.mediator.ProvisionEnrollee;
import org.iotivity.service.easysetup.mediator.ble.BLEManager;

public class EnrolleeDeviceBLEOnBoarding extends EnrolleeDevice {
    /**
     * @param onBoardingConfig Contains details about the connectivity to be established between
     * the Enrollee device & Mediator device in order to perform
     * on-boarding
     * @param provConfig       Contains details about the network to which Enrollee device is
     */
    public static final String TAG = EnrolleeDeviceWiFiOnboarding.class.getName();

    final Context mContext;
    BLEManager bleManager;
    EnrolleeInfo connectedDevice;
    ProvisionEnrollee provisionEnrolleInstance;
    private EasySetupManager easySetupManagerNativeInstance;


    IOnBoardingStatus deviceScanListener = new IOnBoardingStatus() {

        @Override
        public void deviceOnBoardingStatus(EnrolleeInfo enrolleStatus) {
            Log.d("ESBLEOnBoarding", "Entered");
            if (enrolleStatus != null && enrolleStatus.getHWAddr() != null) {
                String finalResult = "Easy Connect : ";

                if (enrolleStatus.isReachable()) {
                    finalResult = "Device OnBoarded" + "["
                            + enrolleStatus.getHWAddr() + "]";
                    connectedDevice = enrolleStatus;
                    BleConnection conn = new BleConnection();
                    conn.setMacaddress(connectedDevice.getHWAddr());
                    conn.setConnectivity(true);
                    conn.setmServiceUUID(connectedDevice.getIpAddr());
                    conn.setmDeviceName(connectedDevice.getDevice());
                    Log.d("ESBLEOnBoarding", "Entered" + ":" + finalResult);
                    mOnBoardingCallback.onFinished(conn);
                    return;

                }
            }

            BleConnection conn = new BleConnection();
            conn.setConnectivity(false);
            mOnBoardingCallback.onFinished(conn);


        }
    };

    protected EnrolleeDeviceBLEOnBoarding(Context context, OnBoardingConfig onBoardingConfig,
                                          ProvisioningConfig provConfig) {
        super(onBoardingConfig, provConfig);
        mContext = context;
        bleManager = new BLEManager(mContext, (BLEOnBoardingConfig) onBoardingConfig);
    }

    @Override
    protected void startOnBoardingProcess() {
        Log.i(TAG, "Starging on boarding process");

        boolean status = bleManager.setupBluetooth();
        mState = EnrolleeState.DEVICE_ON_BOARDING_STATE;
        Log.i(TAG, "Bluetooth started with status " + status);
        if (status) {
            Log.i(TAG, "Scanning available BLE devices");
            bleManager.startScan(deviceScanListener);

        }

    }

    @Override
    protected void stopOnBoardingProcess() {

        if (bleManager != null)
            bleManager.stopscan();

    }

    @Override
    protected void startProvisioningProcess(OnBoardingConnection conn) {

        Log.i("start provisioning BLE", mProvConfig.getConnType() + "");

        mState = EnrolleeState.DEVICE_PROVISIONING_STATE;
        provisionEnrolleInstance = new ProvisionEnrollee(mContext);
        provisionEnrolleInstance.registerProvisioningHandler(new IProvisioningListener() {
            @Override
            public void onFinishProvisioning(int statuscode) {
                mState = (statuscode == 0) ? EnrolleeState.DEVICE_PROVISIONING_SUCCESS_STATE :
                        EnrolleeState.DEVICE_PROVISIONING_FAILED_STATE;
                mProvisioningCallback.onFinished(EnrolleeDeviceBLEOnBoarding.this);
            }
        });

        BleConnection connection = (BleConnection) conn;
        WiFiProvConfig wifiProvConfig = (WiFiProvConfig) mProvConfig;
        easySetupManagerNativeInstance = EasySetupManager.getInstance();
        easySetupManagerNativeInstance.setApplicationContext(mContext);
        easySetupManagerNativeInstance.initEasySetup();
        Log.d("init", "successful");
        OcConnectivityType ocConnectivityType = OcConnectivityType.CT_ADAPTER_GATT_BTLE;
        easySetupManagerNativeInstance.provisionEnrollee(connection.getMacaddress(),
                wifiProvConfig.getSsId(),
                wifiProvConfig.getPassword(),
                ocConnectivityType.getValue());


    }
}
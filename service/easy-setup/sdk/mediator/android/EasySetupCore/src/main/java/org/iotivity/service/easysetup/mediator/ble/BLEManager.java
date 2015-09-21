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
package org.iotivity.service.easysetup.mediator.ble;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothManager;
import android.content.Context;
import android.os.Handler;
import android.util.Log;

import org.iotivity.service.easysetup.impl.BLEOnBoardingConfig;
import org.iotivity.service.easysetup.mediator.EnrolleeInfo;
import org.iotivity.service.easysetup.mediator.IOnBoardingStatus;

import java.util.UUID;

public class BLEManager {
    private BluetoothAdapter mBluetoothAdapter;
    private boolean is_scanning;
    private Context mcontext;
    private Handler mHandler;
    private static final long SCAN_PERIOD = 10000;
    private BLEOnBoardingConfig bleOnBoardingConfig;
    private IOnBoardingStatus finishlistener;

    public BLEManager(Context context, BLEOnBoardingConfig config) {
        mcontext = context;
        bleOnBoardingConfig = config;
    }

    public boolean setupBluetooth() {
        mHandler = new Handler();
        if (mHandler == null) return false;
        final BluetoothManager bluetoothManager =
                (BluetoothManager) mcontext.getSystemService(Context.BLUETOOTH_SERVICE);
        mBluetoothAdapter = bluetoothManager.getAdapter();
        if (mBluetoothAdapter == null) return false;
        if (!mBluetoothAdapter.isEnabled()) {
            Log.d("bluetooth Disabled", "");
            return false;
        }


        return true;
    }

    public void startScan(IOnBoardingStatus listener) {

        mHandler.postDelayed(new Runnable() {
            @Override
            public void run() {
                is_scanning = false;
                mBluetoothAdapter.stopLeScan(mLeScanCallback);

            }
        }, SCAN_PERIOD);

        is_scanning = true;
        UUID[] uuids = {bleOnBoardingConfig.getUuid()};
        mBluetoothAdapter.startLeScan(uuids, mLeScanCallback);
        finishlistener = listener;
    }

    public void stopscan() {
        mBluetoothAdapter.stopLeScan(mLeScanCallback);

    }


    private BluetoothAdapter.LeScanCallback mLeScanCallback =
            new BluetoothAdapter.LeScanCallback() {

                @Override
                public void onLeScan(final BluetoothDevice device, int rssi, byte[] scanRecord) {
                    Log.d("device found", device.getAddress());
                    bleOnBoardingConfig.setMacaddress(device.getAddress());
                    {
                        stopscan();
                        EnrolleeInfo result = new EnrolleeInfo();
                        result.setReachable(true);
                        result.setHWAddr(bleOnBoardingConfig.getMacaddress());
                        NotifyApplication(result);

                    }
                }
            };


    void NotifyApplication(final EnrolleeInfo result) {
        // Get a handler that can be used to post to the main thread
        Handler mainHandler = new Handler(mcontext.getMainLooper());
        Runnable myRunnable = new Runnable() {
            @Override
            public void run() {
                finishlistener.deviceOnBoardingStatus(result);
            }
        };
        mainHandler.post(myRunnable);
    }
}

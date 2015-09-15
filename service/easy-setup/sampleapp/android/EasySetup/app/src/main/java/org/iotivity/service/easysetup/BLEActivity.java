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
package org.iotivity.service.easysetup;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothManager;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.Toast;

import org.iotivity.service.easysetup.core.EasySetupService;
import org.iotivity.service.easysetup.core.EasySetupStatus;
import org.iotivity.service.easysetup.core.EnrolleeDevice;
import org.iotivity.service.easysetup.core.EnrolleeState;
import org.iotivity.service.easysetup.impl.BLEOnBoardingConfig;
import org.iotivity.service.easysetup.impl.EnrolleeDeviceFactory;
import org.iotivity.service.easysetup.impl.WiFiProvConfig;

import java.io.IOException;

public class BLEActivity extends Activity {
    WiFiProvConfig mWiFiProvConfig;
    BLEOnBoardingConfig bleOnBoardingConfig;
    EasySetupService mEasySetupService;
    EnrolleeDeviceFactory mDeviceFactory;
    EnrolleeDevice mDevice;

    private final int REQUEST_ENABLE_BT = 1;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_ble);

        mDeviceFactory = EnrolleeDeviceFactory.newInstance(getApplicationContext());

        mDevice = mDeviceFactory.newEnrolleeDevice(getOnBoardingWifiConfig(), getEnrollerWifiConfig());

        EasySetupStatus easySetupStatus = new EasySetupStatus() {
            @Override
            public void onFinished(EnrolleeDevice enrolleeDevice) {
                final String msg = mDevice.isSetupSuccessful() ? "Device configured successfully" : "Device configuration failed";
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Toast.makeText(getApplicationContext(), msg, Toast.LENGTH_SHORT).show();
                    }
                });

            }

            @Override
            public void onProgress(EnrolleeState enrolleeState) {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Toast.makeText(getApplicationContext(), "Device state changed", Toast.LENGTH_SHORT).show();
                    }
                });


            }
        };
        mEasySetupService = EasySetupService.getInstance(getApplicationContext(), easySetupStatus);
        start();
    }

    public WiFiProvConfig getEnrollerWifiConfig() {
        mWiFiProvConfig = new WiFiProvConfig("linksysy", "12345678");
        return mWiFiProvConfig;
    }

    public BLEOnBoardingConfig getOnBoardingWifiConfig() {
        //TODO : Check proper configuration
        bleOnBoardingConfig = new BLEOnBoardingConfig();

        bleOnBoardingConfig.setUuid("ade3d529-c784-4f63-a987-eb69f70ee816");

        return bleOnBoardingConfig;
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_ble, menu);
        return true;
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        // User chose not to enable Bluetooth.
        if (requestCode == REQUEST_ENABLE_BT && resultCode == Activity.RESULT_CANCELED) {
            Log.e("error enablg bluetooth", resultCode + "");
            finish();
            return;
        } else try {
            //bluetooth is  enabled, now start the setup of enrollee devices
            mEasySetupService.startSetup(mDevice);
        } catch (IOException e) {
            e.printStackTrace();
        }
        super.onActivityResult(requestCode, resultCode, data);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    public void start() {
        //This function starts the bluetooth adpater so that the easysetup can start scanning for BLE devices
        //IF bluetooth is directly enabled it will directly start the setup of enrollee devices
        final BluetoothManager bluetoothManager =
                (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
        BluetoothAdapter mBluetoothAdapter = bluetoothManager.getAdapter();

        if (!mBluetoothAdapter.isEnabled()) {
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
        } else try {
            mEasySetupService.startSetup(mDevice);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}

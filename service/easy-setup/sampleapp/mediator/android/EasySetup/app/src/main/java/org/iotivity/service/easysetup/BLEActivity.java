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

import java.io.IOException;

import org.iotivity.service.easysetup.core.BleConnection;
import org.iotivity.service.easysetup.core.EasySetupService;
import org.iotivity.service.easysetup.core.EasySetupStatus;
import org.iotivity.service.easysetup.core.EnrolleeDevice;
import org.iotivity.service.easysetup.core.EnrolleeState;
import org.iotivity.service.easysetup.impl.BLEOnBoardingConfig;
import org.iotivity.service.easysetup.impl.EnrolleeDeviceFactory;
import org.iotivity.service.easysetup.impl.WiFiProvConfig;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothManager;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

public class BLEActivity extends Activity {


    /*Status to update the UI */
    public static final int SUCCESS = 0;
    public static final int FAILED = 1;
    public static final int STATE_CHANGED = 2;

    EditText mEnrolleeSsidText;
    EditText mmEnrolleePasswordPassText;


    TextView mDeviceNameTextView;
    TextView mDeviceMacTextView;
    TextView mDeviceUuidTextView;

    TextView mResultTextView;
    ProgressBar mProgressbar;
    Button mStartButton;
    Button mStopButton;
    Handler mHandler = new ThreadHandler();

    /**
     * Objects to be instantiated by the programmer
     */
    WiFiProvConfig mWiFiProvConfig;
    BLEOnBoardingConfig mBleOnBoardingConfig;
    EasySetupService mEasySetupService;
    EnrolleeDeviceFactory mDeviceFactory;
    EnrolleeDevice mDevice;

    private final int REQUEST_ENABLE_BT = 1;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_ble);

        /* Initialize widgets to get user input for target network's SSID & password*/
        mEnrolleeSsidText = (EditText) findViewById(R.id.enrolleeSsid);
        mmEnrolleePasswordPassText = (EditText) findViewById(R.id.enrolleePass);
        mDeviceNameTextView = (TextView) findViewById(R.id.devicename);
        mDeviceMacTextView = (TextView) findViewById(R.id.hardAddr);
        mDeviceUuidTextView = (TextView) findViewById(R.id.uuid);
        mResultTextView = (TextView) findViewById(R.id.status);
        mProgressbar = (ProgressBar) findViewById(R.id.progressBar);
        mStartButton = (Button) findViewById(R.id.startSetup);
        mStopButton = (Button) findViewById(R.id.stopSetup);

        //default SSID and password
        mEnrolleeSsidText.setText("hub2.4G");
        mmEnrolleePasswordPassText.setText("11112222");

       /* Create Easy Setup Service instance*/
        mEasySetupService = EasySetupService.getInstance(getApplicationContext(),
                new EasySetupStatus() {

                    @Override
                    public void onFinished(final EnrolleeDevice enrolledevice) {
                        Log.i("BleActivity", "onFinished() is received " + enrolledevice
                                .isSetupSuccessful());
                        if (enrolledevice.isSetupSuccessful()) {
                            mHandler.sendEmptyMessage(SUCCESS);
                        } else {
                            mHandler.sendEmptyMessage(FAILED);
                        }
                    }

                    @Override
                    public void onProgress(EnrolleeState state) {
                        Log.i("MainActivity", "onProgress() is received ");
                        mHandler.sendEmptyMessage(STATE_CHANGED);
                    }

                });

        /* Create EnrolleeDevice Factory instance*/
        mDeviceFactory = EnrolleeDeviceFactory.newInstance(getApplicationContext());

        addListnerforStartES();
        addListenerForStopES();


    }

    public void addListnerforStartES() {

        mStartButton.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                mProgressbar.setVisibility(View.VISIBLE);
                mProgressbar.setIndeterminate(true);
                mStartButton.setEnabled(false);
                mResultTextView.setText(R.string.running);
                mStopButton.setEnabled(true);
                start();
            }
        });
    }

    public void addListenerForStopES() {
        mStopButton = (Button) findViewById(R.id.stopSetup);

        mStopButton.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View arg0) {
                mStartButton.setEnabled(true);
                mStopButton.setEnabled(false);
                mResultTextView.setText(R.string.stopped);
                mProgressbar.setIndeterminate(false);
                mProgressbar.setVisibility(View.INVISIBLE);
                mEasySetupService.stopSetup(mDevice);
            }
        });
    }


    public WiFiProvConfig getEnrollerWifiConfig() {
        /* Provide the credentials for the Mediator Soft AP to be connected by Enrollee*/
        mWiFiProvConfig = new WiFiProvConfig(mEnrolleeSsidText.getText().toString(),
                mmEnrolleePasswordPassText.getText().toString());
        return mWiFiProvConfig;
    }

    public BLEOnBoardingConfig getOnBoardingWifiConfig() {
        // Set the uuid of the OIC device here
        mBleOnBoardingConfig = new BLEOnBoardingConfig();
        /*
        set the uuid of the OIC-device, so that the Easysetup API can find the device
         */
        mBleOnBoardingConfig.setUuid("ade3d529-c784-4f63-a987-eb69f70ee816");

        return mBleOnBoardingConfig;
    }


    public void onDestroy() {
        super.onDestroy();
        /*Reset the Easy setup process*/
        if (mEasySetupService != null) {
            mEasySetupService.finish();
        }
    }

    public void start() {
        //This function starts the bluetooth adpater so that the easysetup can start scanning for BLE devices
        final BluetoothManager bluetoothManager =
                (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
        BluetoothAdapter mBluetoothAdapter = bluetoothManager.getAdapter();

        if (!mBluetoothAdapter.isEnabled()) {
            //Bluetooth is disabled, enable it
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
        } else try {
            //IF bluetooth is directly enabled it will directly start the setup of enrollee devices

                    /* Create a device using Factory instance*/
            mDevice = mDeviceFactory.newEnrolleeDevice(getOnBoardingWifiConfig(),
                    getEnrollerWifiConfig());
            mEasySetupService.startSetup(mDevice);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        // User chose not to enable Bluetooth.
        if (requestCode == REQUEST_ENABLE_BT && resultCode == Activity.RESULT_CANCELED) {
            Log.e("error bluetooth", "Bluetooth not enabled..Try again");
            Toast.makeText(BLEActivity.this, "Bluetooth not enabled..Try again", Toast.LENGTH_SHORT).show();
            return;
        } else try {
            //bluetooth is  enabled, now start the setup of enrollee devices
                    /* Create a device using Factory instance*/
            mDevice = mDeviceFactory.newEnrolleeDevice(getOnBoardingWifiConfig(),
                    getEnrollerWifiConfig());
            mEasySetupService.startSetup(mDevice);
        } catch (IOException e) {
            e.printStackTrace();
        }
        super.onActivityResult(requestCode, resultCode, data);
    }


    class ThreadHandler extends Handler {
        @Override
        public void handleMessage(Message msg) {

            switch (msg.what) {
                case SUCCESS: {

                    mProgressbar.setIndeterminate(false);
                    mStopButton.setEnabled(false);
                    mStartButton.setEnabled(true);
                    mProgressbar.setVisibility(View.INVISIBLE);
                    String resultMsg = "Device configured successfully";
                    mResultTextView.setText(R.string.success);

                    /* Update device information on the Ui */
                    BleConnection connection = (BleConnection) mDevice
                            .getConnection();
                    mDeviceNameTextView.setText(connection.getmDeviceName());
                    mDeviceMacTextView.setText(connection.getMacaddress());
                    mDeviceUuidTextView.setText(connection.getmServiceUUID());
                    Toast.makeText(getApplicationContext(), resultMsg, Toast.LENGTH_SHORT).show();
                    break;
                }
                case FAILED: {

                    mProgressbar.setIndeterminate(false);
                    mStopButton.setEnabled(false);
                    mStartButton.setEnabled(true);
                    mProgressbar.setVisibility(View.INVISIBLE);
                    String resultMsg = "Device configuration failed";
                    mResultTextView.setText(R.string.failed);
                    Toast.makeText(getApplicationContext(), resultMsg, Toast.LENGTH_SHORT).show();
                    break;
                }

                case STATE_CHANGED: {
                    String resultMsg = "Device state changed";
                    Toast.makeText(getApplicationContext(), resultMsg, Toast.LENGTH_SHORT).show();
                    break;
                }

            }


        }
    }

}

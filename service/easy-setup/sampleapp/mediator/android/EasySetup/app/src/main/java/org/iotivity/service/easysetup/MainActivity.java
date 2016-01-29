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

package org.iotivity.service.easysetup;

import org.iotivity.service.easysetup.mediator.EasySetupService;
import org.iotivity.service.easysetup.mediator.EasySetupStatus;
import org.iotivity.service.easysetup.mediator.EnrolleeDevice;
import org.iotivity.service.easysetup.mediator.IpOnBoardingConnection;
import org.iotivity.service.easysetup.mediator.EnrolleeDeviceFactory;
import org.iotivity.service.easysetup.mediator.WiFiOnBoardingConfig;
import org.iotivity.service.easysetup.mediator.WiFiProvConfig;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.RadioButton;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends Activity {

    /* Status to update the UI */
    public static final int SUCCESS       = 0;
    public static final int FAILED        = 1;
    public static final int STATE_CHANGED = 2;

    String                  mSoftAPSsid;
    String                  mSoftAPPassword;
    String                  mEnrollerSsid;
    String                  mEnrollerPassword;

    EditText                mSoftAPSsidText;
    EditText                mSoftAPPassText;
    EditText                mEnrollerSsidText;
    EditText                mEnrollerPasswordPassText;

    TextView                mDeviceIpTextView;
    TextView                mDeviceMacTextView;
    TextView                mResultTextView;

    ProgressBar             mProgressbar;

    Button                  mStartButton;
    Button                  mStopButton;

    RadioButton             mEnrollee;
    RadioButton             mMediator;

    LinearLayout            mSoftAP;
    RelativeLayout          mDeviceInfo;
    TextView                mDeviceText;

    Handler                 mHandler      = new ThreadHandler();

    /**
     * Objects to be instantiated by the programmer
     */
    WiFiProvConfig          mWiFiProvConfig;
    WiFiOnBoardingConfig    mWiFiOnBoardingConfig;
    EasySetupService        mEasySetupService;
    EnrolleeDeviceFactory   mDeviceFactory;
    EnrolleeDevice          mDevice;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        /*
         * Initialize widgets to get user input for target network's SSID &
         * password
         */
        mSoftAP = (LinearLayout) findViewById(R.id.softAP);
        mDeviceInfo = (RelativeLayout) findViewById(R.id.deviceInfo);
        mDeviceText = (TextView) findViewById(R.id.textViewDeviceinfo);

        mSoftAPSsidText = (EditText) findViewById(R.id.ssid);
        mSoftAPPassText = (EditText) findViewById(R.id.password);
        mEnrollerSsidText = (EditText) findViewById(R.id.enrolleeSsid);
        mEnrollerPasswordPassText = (EditText) findViewById(R.id.enrolleePass);
        mDeviceIpTextView = (TextView) findViewById(R.id.ipAddr);
        mDeviceMacTextView = (TextView) findViewById(R.id.hardAddr);

        mResultTextView = (TextView) findViewById(R.id.status);
        mProgressbar = (ProgressBar) findViewById(R.id.progressBar);

        mEnrollee = (RadioButton) findViewById(R.id.enrollee);
        mMediator = (RadioButton) findViewById(R.id.mediator);

        mStartButton = (Button) findViewById(R.id.startSetup);

        mEnrollee.setChecked(false);
        mMediator.setChecked(false);
        mStartButton.setEnabled(false);

        /* Create EnrolleeDevice Factory instance */
        mDeviceFactory = EnrolleeDeviceFactory
                .newInstance(getApplicationContext());
        addListenerForStartAP();
        addListenerForStopAP();

        // default values for target network
        mEnrollerSsidText.setText(R.string.target_default_ssid);
        mEnrollerPasswordPassText.setText(R.string.target_default_pwd);
        mSoftAPSsidText.setEnabled(false);
        mSoftAPPassText.setEnabled(false);

        mEnrollee.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {

                mMediator.setChecked(false);
                AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(
                        MainActivity.this);
                alertDialogBuilder.setTitle("Network selection");
                alertDialogBuilder
                        .setMessage("Proceed to select the network!")
                        .setCancelable(false)
                        .setPositiveButton("Yes",
                                new DialogInterface.OnClickListener() {
                                    public void onClick(DialogInterface dialog,
                                            int id) {
                                        MainActivity.this
                                                .startActivity(new Intent(
                                                        WifiManager.ACTION_PICK_WIFI_NETWORK));

                                        mStartButton.setEnabled(true);
                                        mSoftAP.setVisibility(View.INVISIBLE);
                                        mDeviceInfo
                                                .setVisibility(View.INVISIBLE);
                                        mDeviceText
                                                .setVisibility(View.INVISIBLE);
                                    }
                                })
                        .setNegativeButton("No",
                                new DialogInterface.OnClickListener() {
                                    public void onClick(DialogInterface dialog,
                                            int id) {
                                        mEnrollee.setChecked(false);
                                        mStartButton.setEnabled(false);
                                        dialog.cancel();
                                    }
                                });
                alertDialogBuilder.create().show();
            }
        });

        mMediator.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                mStartButton.setEnabled(true);
                mEnrollee.setChecked(false);
                mSoftAPSsidText.setEnabled(false);
                mSoftAPPassText.setEnabled(false);
                mSoftAPSsidText.setText(R.string.softAP_ssid);
                mSoftAPPassText.setText(R.string.softAP_pwd);
                mDeviceIpTextView.setText(R.string.not_available);
                mDeviceMacTextView.setText(R.string.not_available);
                mResultTextView.setText(R.string.not_started);
                mSoftAP.setVisibility(View.VISIBLE);
                mDeviceInfo.setVisibility(View.VISIBLE);
                mDeviceText.setVisibility(View.VISIBLE);
            }
        });

        /* Create Easy Setup Service instance */
        mEasySetupService = EasySetupService.getInstance(
                getApplicationContext(), new EasySetupStatus() {

                    @Override
                    public void onFinished(final EnrolleeDevice enrolledevice) {
                        Log.i("MainActivity", "onFinished() is received "
                                + enrolledevice.isSetupSuccessful());
                        if (enrolledevice.isSetupSuccessful()) {
                            mHandler.sendEmptyMessage(SUCCESS);
                        } else {
                            mHandler.sendEmptyMessage(FAILED);
                        }
                    }

                    @Override
                    public void onProgress(EnrolleeDevice enrolleeDevice) {
                        Log.i("MainActivity", "onProgress() is received ");
                        mHandler.sendEmptyMessage(STATE_CHANGED);
                    }
                });
    }

    public void onDestroy() {
        super.onDestroy();
        /* Reset the Easy setup process */
        if (mEasySetupService != null) {
            mEasySetupService.finish();
        }
    }

    public void addListenerForStartAP() {
        mStartButton = (Button) findViewById(R.id.startSetup);
        mStartButton.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View arg0) {
                try {
                    if (mEnrollee.isChecked()) {
                        // Check the wifi connectivity
                        if (!isConnectedTowifi()) {
                            return;
                        }

                        mEnrollerSsid = mEnrollerSsidText.getText().toString();
                        mEnrollerPassword = mEnrollerPasswordPassText.getText()
                                .toString();

                        mWiFiProvConfig = new WiFiProvConfig(mEnrollerSsid,
                                mEnrollerPassword);
                        mDevice = mDeviceFactory
                                .newEnrolleeDevice(mWiFiProvConfig);
                        mEasySetupService.startSetup(mDevice);
                    } else {
                        mSoftAPSsid = mSoftAPSsidText.getText().toString();
                        mSoftAPPassword = mSoftAPPassText.getText().toString();
                        mEnrollerSsid = mEnrollerSsidText.getText().toString();
                        mEnrollerPassword = mEnrollerPasswordPassText.getText()
                                .toString();

                        mWiFiProvConfig = new WiFiProvConfig(mEnrollerSsid,
                                mEnrollerPassword);
                        mWiFiOnBoardingConfig = new WiFiOnBoardingConfig();

                        /*
                         * Provide the target credentials to be provisioned to
                         * the Enrollee by Mediator
                         */
                        mWiFiOnBoardingConfig.setSSId("EasySetup123");
                        mWiFiOnBoardingConfig.setSharedKey("EasySetup123");
                        mWiFiOnBoardingConfig
                                .setAuthAlgo(WifiConfiguration.AuthAlgorithm.OPEN);
                        mWiFiOnBoardingConfig
                                .setKms(WifiConfiguration.KeyMgmt.WPA_PSK);
                        mDevice = mDeviceFactory.newEnrolleeDevice(
                                mWiFiProvConfig, mWiFiOnBoardingConfig);
                        mEasySetupService.startSetup(mDevice);
                    }
                    mProgressbar.setVisibility(View.VISIBLE);
                    mProgressbar.setIndeterminate(true);
                    mStartButton.setEnabled(false);
                    mResultTextView.setText(R.string.running);

                    // Reset Device information
                    mDeviceIpTextView.setText(R.string.not_available);
                    mDeviceMacTextView.setText(R.string.not_available);
                    mStopButton.setEnabled(true);

                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        });
    }

    public void addListenerForStopAP() {
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

    private boolean isConnectedTowifi() {

        AlertDialog dialog;
        ConnectivityManager connManager;
        NetworkInfo wifi;

        // Check the wifi connectivity
        connManager = (ConnectivityManager) getSystemService(CONNECTIVITY_SERVICE);
        wifi = connManager.getNetworkInfo(ConnectivityManager.TYPE_WIFI);
        if (false == wifi.isConnected()) {

            AlertDialog.Builder dialogBuilder = new AlertDialog.Builder(this);
            dialogBuilder.setTitle("Error");
            dialogBuilder
                    .setMessage("WiFi is not enabled/connected! Please connect the WiFi..");
            dialogBuilder.setCancelable(false);
            dialogBuilder.setPositiveButton("OK",
                    new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {

                            MainActivity.this.startActivity(new Intent(
                                    WifiManager.ACTION_PICK_WIFI_NETWORK));

                        }
                    });
            dialogBuilder.setNegativeButton("Cancel",
                    new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                        }
                    });

            dialog = dialogBuilder.create();
            dialog.show();
            return false;
        }
        return true;
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
                    IpOnBoardingConnection connection = (IpOnBoardingConnection) mDevice
                            .getConnection();
                    mDeviceIpTextView.setText(connection.getIp());
                    mDeviceMacTextView.setText(connection.getHardwareAddress());

                    Toast.makeText(getApplicationContext(), resultMsg,
                            Toast.LENGTH_SHORT).show();
                    break;
                }
                case FAILED: {

                    mProgressbar.setIndeterminate(false);
                    mStopButton.setEnabled(false);
                    mStartButton.setEnabled(true);
                    mProgressbar.setVisibility(View.INVISIBLE);
                    String resultMsg = "Device configuration failed";
                    mResultTextView.setText(R.string.failed);
                    Toast.makeText(getApplicationContext(), resultMsg,
                            Toast.LENGTH_SHORT).show();
                    break;
                }

                case STATE_CHANGED: {
                    String resultMsg = "Device state changed";
                    Toast.makeText(getApplicationContext(), resultMsg,
                            Toast.LENGTH_SHORT).show();
                    break;
                }
            }
        }
    }
}

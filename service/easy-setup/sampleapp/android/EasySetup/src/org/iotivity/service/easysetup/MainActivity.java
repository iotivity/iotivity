/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/
package org.iotivity.service.easysetup;

import java.util.Timer;
import java.util.TimerTask;

import org.iotivity.service.easysetup.mediator.EnrolleeInfo;
import org.iotivity.service.easysetup.mediator.EasySetupManager;
import org.iotivity.service.easysetup.mediator.IOnBoardingStatus;
import org.iotivity.service.easysetup.mediator.IProvisioningListener;
import org.iotivity.service.easysetup.mediator.OnBoardEnrollee;
import org.iotivity.service.easysetup.mediator.ProvisionEnrollee;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.net.wifi.WifiConfiguration;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends Activity implements IProvisioningListener,
        IOnBoardingStatus {
    String            devicesConnected;
    TextView          textView1;

    private Timer     myTimer;
    private Timer     myTimer2;
    static int        scanCount             = 0;
    static int        easySetupCount        = 0;
    static final int  REQUEST_IMAGE_CAPTURE = 1;
    ImageView         imageView;
    EnrolleeInfo      connectedDevice;

    OnBoardEnrollee   onBoardingHandlerInstance;
    ProvisionEnrollee provisionEnrolleInstance;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        textView1 = (TextView) findViewById(R.id.textView1);

        // OnBoarding Process
        onBoardingHandlerInstance = new OnBoardEnrollee(this);
        onBoardingHandlerInstance.registerOnBoardingStatusHandler(this);

        // Provisioning Process
        provisionEnrolleInstance = new ProvisionEnrollee(this);
        provisionEnrolleInstance.registerProvisioningHandler(this);

        // Get intent, action and MIME type
        Intent intent = getIntent();
        String action = intent.getAction();
        String type = intent.getType();

        if (Intent.ACTION_SEND.equals(action) && type != null) {
            if ("text/plain".equals(type)) {
                handleSendText(intent); // Handle text being sent
            }
        } else {
            // Handle other intents, such as being started from the home screen
        }

        addListenerForStartAP();
        addListenerForStopAP();
        addListenerOnProvisioning();

        myTimer2 = new Timer();
        myTimer2.schedule(new TimerTask() {
            @Override
            public void run() {
                onBoardingHandlerInstance.startDeviceScan();
            }

        }, 0, 2000);
    }

    public void onDestroy() {
        super.onDestroy();
        provisionEnrolleInstance.stopEnrolleeProvisioning(0);
        onBoardingHandlerInstance.disableWiFiAP();
        finish();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        menu.add(0, 0, 0, "Get Clients");
        menu.add(0, 1, 0, "Start AP");
        menu.add(0, 2, 0, "Stop AP");
        menu.add(0, 3, 0, "Provision");
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

    @Override
    public void onFinishProvisioning(final int statuscode) {
        // TODO Auto-generated method stub
        try {
            // code runs in a thread
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    // TODO Auto-generated method stub
                    Toast toast = null;
                    if (statuscode == 0) {
                        toast = Toast.makeText(getApplicationContext(),
                                connectedDevice.getIpAddr()
                                        + " - is Provisioned",
                                Toast.LENGTH_LONG);
                        Log.i("EasyConnect", "Provisioned statuscode-"
                                + statuscode);
                    } else {
                        toast = Toast.makeText(getApplicationContext(),
                                connectedDevice.getIpAddr()
                                        + " - is NOT Provisioned",
                                Toast.LENGTH_LONG);
                        Log.i("EasyConnect", "Not Provisioned statuscode-"
                                + statuscode);
                    }

                    toast.show();
                }
            });
        } catch (final Exception ex) {
            Log.i("---", "Exception in thread");
        }
    }

    void handleSendText(Intent intent) {
        String sharedText = intent.getStringExtra(Intent.EXTRA_TEXT);
        if (sharedText != null) {
            // Update UI to reflect text being shared
            WifiConfiguration netConfig = new WifiConfiguration();
            netConfig.SSID = "EasyConnect";
            netConfig.allowedAuthAlgorithms
                    .set(WifiConfiguration.AuthAlgorithm.OPEN);
            // netConfig.allowedProtocols.set(WifiConfiguration.Protocol.RSN);
            // netConfig.allowedProtocols.set(WifiConfiguration.Protocol.WPA);
            netConfig.allowedKeyManagement
                    .set(WifiConfiguration.KeyMgmt.WPA_PSK);
            netConfig.preSharedKey = "EasyConnect";
            // netConfig.allowedPairwiseCiphers.set(WifiConfiguration.PairwiseCipher.CCMP);
            // netConfig.allowedPairwiseCiphers.set(WifiConfiguration.PairwiseCipher.TKIP);
            // netConfig.allowedGroupCiphers.set(WifiConfiguration.GroupCipher.CCMP);
            // netConfig.allowedGroupCiphers.set(WifiConfiguration.GroupCipher.TKIP);
            onBoardingHandlerInstance.enableWiFiAP(netConfig, true);

            Toast.makeText(getApplicationContext(),
                    "QR Code Captured. Starting Wi-Fi Access Point!",
                    Toast.LENGTH_LONG).show();

            myTimer = new Timer();
            myTimer.schedule(new TimerTask() {
                @Override
                public void run() {
                    scan();
                    scanCount++;
                    Log.i("EasyConnect", "Scan Count -" + scanCount);
                }

            }, 0, 5000);
        }
    }

    private void scan() {
        onBoardingHandlerInstance.registerOnBoardingStatusHandler(this);
        onBoardingHandlerInstance.startDeviceScan();
    }

    public void addListenerForStartAP() {
        Button button = (Button) findViewById(R.id.button1);

        button.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View arg0) {
                WifiConfiguration netConfig = new WifiConfiguration();
                netConfig.SSID = "EasyConnect";
                netConfig.allowedAuthAlgorithms
                        .set(WifiConfiguration.AuthAlgorithm.OPEN);
                // netConfig.allowedProtocols.set(WifiConfiguration.Protocol.RSN);
                // netConfig.allowedProtocols.set(WifiConfiguration.Protocol.WPA);
                netConfig.allowedKeyManagement
                        .set(WifiConfiguration.KeyMgmt.WPA_PSK);
                netConfig.preSharedKey = "EasyConnect";
                // netConfig.allowedPairwiseCiphers.set(WifiConfiguration.PairwiseCipher.CCMP);
                // netConfig.allowedPairwiseCiphers.set(WifiConfiguration.PairwiseCipher.TKIP);
                // netConfig.allowedGroupCiphers.set(WifiConfiguration.GroupCipher.CCMP);
                // netConfig.allowedGroupCiphers.set(WifiConfiguration.GroupCipher.TKIP);
                onBoardingHandlerInstance.enableWiFiAP(netConfig, true);
            }
        });
    }

    public void addListenerForStopAP() {
        Button button = (Button) findViewById(R.id.stopapbutton);

        button.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View arg0) {
                onBoardingHandlerInstance.disableWiFiAP();
            }
        });
    }

    public void addListenerOnProvisioning() {
        Button button = (Button) findViewById(R.id.button2);

        button.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View arg0) {
                provisionEnrolleInstance.provisionEnrollee(
                        connectedDevice.getIpAddr(), "NewAccessPoint",
                        "NewAccessPoint", 0);
                easySetupCount++;
                Log.i("EasyConnect", "easy Setup Count-" + easySetupCount);
                Log.i("EasyConnect",
                        "IP Address-" + connectedDevice.getIpAddr());
            }
        });

    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == REQUEST_IMAGE_CAPTURE && resultCode == RESULT_OK) {
            Bundle extras = data.getExtras();
            Bitmap imageBitmap = (Bitmap) extras.get("data");
            imageView.setImageBitmap(imageBitmap);
        }
    }

    @Override
    public void deviceOnBoardingStatus(EnrolleeInfo enrolleStatus) {
        // TODO Auto-generated method stub
        // TODO Auto-generated method stub
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
            } else {
                finalResult = "Device Removed" + "["
                        + enrolleStatus.getIpAddr() + "]";
            }

            textView1.setText("");
            textView1.append("Clients: \n");
            textView1.append("####################\n");
            textView1.append("IP Address 	: " + enrolleStatus.getIpAddr()
                    + "\n");
            textView1.append("Device 	 	: " + enrolleStatus.getDevice() + "\n");
            textView1.append("HW Address 	: " + enrolleStatus.getHWAddr()
                    + "\n");
            textView1.append("Is OnBoarded	: " + enrolleStatus.isReachable()
                    + "\n");

            Toast.makeText(getApplicationContext(), finalResult,
                    Toast.LENGTH_LONG).show();

            /*
             * myTimer2 = new Timer(); myTimer2.schedule(new TimerTask() {
             *
             * @Override public void run() {
             * easySetupInstance.StartEasySetup(enrolleStatus.getIpAddr());
             * easySetupCount++; Log.i("EasyConnect",
             * "easy Setup Count-"+easySetupCount); Log.i("EasyConnect",
             * "IP Address-"+enrolleStatus.getIpAddr()); }
             *
             * }, 0, 10000);
             */
        }
    }

}

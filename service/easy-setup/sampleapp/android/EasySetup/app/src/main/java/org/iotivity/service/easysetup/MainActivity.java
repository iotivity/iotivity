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

import org.iotivity.base.ModeType;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;
import org.iotivity.service.easysetup.mediator.common.EnrolleeDevice;
import org.iotivity.service.easysetup.mediator.IOnBoardingStatus;
import org.iotivity.service.easysetup.mediator.IProvisioningListener;
import org.iotivity.service.easysetup.mediator.OnBoardEnrollee;
import org.iotivity.service.easysetup.mediator.ProvisionEnrollee;
import org.iotivity.base.OcConnectivityType;
import org.iotivity.service.easysetup.mediator.ip.IPEnrolleeDevice;
import org.iotivity.service.easysetup.mediator.ip.IPProvisioningInfo;
import org.iotivity.service.easysetup.mediator.ip.WiFiSoftAPOnBoardingConfig;

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
    EnrolleeDevice    connectedDevice;

    OnBoardEnrollee   onBoardingHandlerInstance;
    ProvisionEnrollee provisionEnrolleInstance;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        textView1 = (TextView) findViewById(R.id.textView1);

        // OnBoarding Process
        onBoardingHandlerInstance = new OnBoardEnrollee(this, OcConnectivityType.CT_IP_USE_V4);
        onBoardingHandlerInstance.registerOnBoardingStatusHandler(this);

        // Get intent, action and MIME type
        Intent intent = getIntent();
        String action = intent.getAction();
        String type = intent.getType();

        addListenerForStartAP();
        addListenerForStopAP();
        addListenerOnProvisioning();
        initOICStack();

        myTimer2 = new Timer();
        myTimer2.schedule(new TimerTask() {
            @Override
            public void run() {
                onBoardingHandlerInstance.startDeviceScan(300);
            }

        }, 0, 2000);
    }

    public void onDestroy() {
        super.onDestroy();
        if(provisionEnrolleInstance != null)
        {
            provisionEnrolleInstance.stopEnrolleeProvisioning(OcConnectivityType.CT_IP_USE_V4);
        }
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
                    IPEnrolleeDevice ipEnrolleeDevice = (IPEnrolleeDevice)connectedDevice;
                    if (statuscode == 0) {
                        toast = Toast.makeText(getApplicationContext(),
                                ipEnrolleeDevice.getIpAddr()
                                        + " - is Provisioned",
                                Toast.LENGTH_LONG);
                        Log.i("EasyConnect", "Provisioned statuscode-"
                                + statuscode);
                    } else {
                        toast = Toast.makeText(getApplicationContext(),
                                ipEnrolleeDevice.getIpAddr()
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

    private void scan() {
        onBoardingHandlerInstance.registerOnBoardingStatusHandler(this);
        onBoardingHandlerInstance.startDeviceScan(300);
    }

    public void addListenerForStartAP() {
        Button button = (Button) findViewById(R.id.button1);

        button.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View arg0) {
                WiFiSoftAPOnBoardingConfig transportConfig = new WiFiSoftAPOnBoardingConfig();

                WifiConfiguration netConfig = new WifiConfiguration();
                netConfig.SSID = "EasySetup123";
                netConfig.allowedAuthAlgorithms
                        .set(WifiConfiguration.AuthAlgorithm.OPEN);
                netConfig.allowedKeyManagement
                        .set(WifiConfiguration.KeyMgmt.WPA_PSK);
                netConfig.preSharedKey = "EasySetup123";

                transportConfig.setConnectivityType(OcConnectivityType.CT_IP_USE_V4);
                transportConfig.setNetConfig(netConfig);
                onBoardingHandlerInstance.enableNetwork(transportConfig, true);
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
                IPEnrolleeDevice ipEnrolleeDevice = (IPEnrolleeDevice)connectedDevice;
                IPProvisioningInfo ipProvisioningInfo = new IPProvisioningInfo();

                ipProvisioningInfo.setConnectivityType(OcConnectivityType.CT_IP_USE_V4);
                ipProvisioningInfo.setIpAddress(ipEnrolleeDevice.getIpAddr());
                ipProvisioningInfo.setNetSSID("EasySetup123");
                ipProvisioningInfo.setNetPWD("EasySetup123");

                provisionEnrolleInstance.provisionEnrollee(
                        ipProvisioningInfo, OcConnectivityType.CT_IP_USE_V4);
                easySetupCount++;
                Log.i("EasyConnect", "easy Setup Count-" + easySetupCount);
                Log.i("EasyConnect",
                        "IP Address-" + ipProvisioningInfo.getIpAddress());
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

    /**
     * configure OIC platform and call findResource
     */
    private void initOICStack() {
        //create platform config
        PlatformConfig cfg = new PlatformConfig(
                this,
                ServiceType.IN_PROC,
                ModeType.CLIENT,
                "0.0.0.0", // bind to all available interfaces
                0,
                QualityOfService.LOW);
        OcPlatform.Configure(cfg);
        Log.i("EasyConnect","OcPlatform Configure is invoked");
    }

    @Override
    public void deviceOnBoardingStatus(EnrolleeDevice enrolleeDevice) {
        // TODO Auto-generated method stub
        // TODO Auto-generated method stub
        if(enrolleeDevice.getConnectivityType() == OcConnectivityType.CT_IP_USE_V4) {
            IPEnrolleeDevice ipEnrolleeDevice = (IPEnrolleeDevice)enrolleeDevice;
            if (ipEnrolleeDevice.getIpAddr() != null) {
                String finalResult = "Easy Connect : ";
                if (ipEnrolleeDevice.isReachable()) {
                    finalResult = "Device OnBoarded" + "["
                            + ipEnrolleeDevice.getIpAddr() + "]";

                    /*
                     * easySetupInstance.StartEasySetup(enrolleStatus.getIpAddr()) ;
                     * easySetupCount++; Log.i("EasyConnect",
                     * "easy Setup Count-"+easySetupCount); Log.i("EasyConnect",
                     * "IP Address-"+enrolleStatus.getIpAddr());
                     */
                    connectedDevice = ipEnrolleeDevice;

                    // Only after onboarding is successful, provisioning is performed
                    provisionEnrolleInstance = new ProvisionEnrollee(this);
                    provisionEnrolleInstance.registerProvisioningHandler(this);

                } else {
                    finalResult = "Device Removed" + "["
                            + ipEnrolleeDevice.getIpAddr() + "]";
                }

                textView1.setText("");
                textView1.append("Clients: \n");
                textView1.append("####################\n");
                textView1.append("IP Address 	: " + ipEnrolleeDevice.getIpAddr()
                        + "\n");
                textView1.append("HW Address 	: " + ipEnrolleeDevice.getHWAddr()
                        + "\n");
                textView1.append("Is OnBoarded	: " + ipEnrolleeDevice.isReachable()
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

}

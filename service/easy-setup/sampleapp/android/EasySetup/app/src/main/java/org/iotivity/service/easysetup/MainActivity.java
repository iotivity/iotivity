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

import java.io.IOException;

import org.iotivity.service.easysetup.core.EasySetupService;
import org.iotivity.service.easysetup.core.EasySetupStatus;
import org.iotivity.service.easysetup.core.EnrolleeDevice;
import org.iotivity.service.easysetup.core.EnrolleeState;
import org.iotivity.service.easysetup.impl.EnrolleeDeviceFactory;
import org.iotivity.service.easysetup.impl.WiFiOnBoardingConfig;
import org.iotivity.service.easysetup.impl.WiFiProvConfig;
import org.iotivity.service.easysetup.mediator.ProvisionEnrollee;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.net.wifi.WifiConfiguration;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends Activity {
    TextView textView1;

    static final int REQUEST_IMAGE_CAPTURE = 1;
    ImageView imageView;
    ProvisionEnrollee provisionEnrolleInstance;

    /**
     * Objects to be instantiated by the programmer
     */
    WiFiProvConfig mWiFiProvConfig;
    WiFiOnBoardingConfig mWiFiOnBoardingConfig;
    EasySetupService mEasySetupService;
    EnrolleeDeviceFactory mDeviceFactory;
    EnrolleeDevice mDevice;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        textView1 = (TextView) findViewById(R.id.textView1);

        // Provisioning Process - This instantiation  will be removed in the next version
        provisionEnrolleInstance = new ProvisionEnrollee(this);
        /* Invocation of APIs using the Easy Setup SDK APIs */

        /* Create Easy Setup Service instance*/
        mEasySetupService = EasySetupService.getInstance(getApplicationContext(),
                new EasySetupStatus() {

                    @Override
                    public void onFinished(EnrolleeDevice enrolledevice) {
                        final String msg = enrolledevice.isSetupSuccessful() ?
                                "Device configured successfully" : "Device configuration failed";
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                Toast.makeText(getApplicationContext(),
                                        msg, Toast.LENGTH_SHORT).show();
                            }
                        });
                    }

                    @Override
                    public void onProgress(EnrolleeState state) {
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                Toast.makeText(getApplicationContext(),
                                        "Device state changed", Toast.LENGTH_SHORT).show();
                            }
                        });
                    }

                });

        /* Create EnrolleeDevice Factory instance*/
        mDeviceFactory = EnrolleeDeviceFactory.newInstance(getApplicationContext());

        /* Create a device using Factory instance*/
        mDevice = mDeviceFactory.newEnrolleeDevice(getOnBoardingWifiConfig(),
                getEnrollerWifiConfig());

        addListenerForStartAP();
        addListenerForStopAP();
    }

    public WiFiProvConfig getEnrollerWifiConfig() {
        /* Provide the credentials for the Mediator Soft AP to be connected by Enrollee*/
        mWiFiProvConfig = new WiFiProvConfig("EasySetup123", "EasySetup123");
        return mWiFiProvConfig;
    }

    public WiFiOnBoardingConfig getOnBoardingWifiConfig() {
        mWiFiOnBoardingConfig = new WiFiOnBoardingConfig();

        /* Provide the target credentials to be provisioned to the Enrollee by Mediator*/
        mWiFiOnBoardingConfig.setSSId("EasySetup123");
        mWiFiOnBoardingConfig.setSharedKey("EasySetup123");
        mWiFiOnBoardingConfig.setAuthAlgo(WifiConfiguration.AuthAlgorithm.OPEN);
        mWiFiOnBoardingConfig.setKms(WifiConfiguration.KeyMgmt.WPA_PSK);
        return mWiFiOnBoardingConfig;
    }


    public void onDestroy() {
        super.onDestroy();
        provisionEnrolleInstance.stopEnrolleeProvisioning(0);
    }

    public void addListenerForStartAP() {
        Button button = (Button) findViewById(R.id.button1);

        button.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View arg0) {
                try {
                    mEasySetupService.startSetup(mDevice);
                } catch (IOException e) {
                    e.printStackTrace();
                }

            }
        });
    }

    public void addListenerForStopAP() {
        Button button = (Button) findViewById(R.id.stopapbutton);

        button.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View arg0) {
                mEasySetupService.stopSetup(mDevice);
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
}

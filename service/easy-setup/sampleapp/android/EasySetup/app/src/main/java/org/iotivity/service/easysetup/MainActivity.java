package org.iotivity.service.easysetup;

import java.io.IOException;

import org.iotivity.service.easysetup.core.EasySetupService;
import org.iotivity.service.easysetup.core.EasySetupStatus;
import org.iotivity.service.easysetup.core.EnrolleeDevice;
import org.iotivity.service.easysetup.core.EnrolleeState;
import org.iotivity.service.easysetup.impl.EnrolleeDeviceFactory;
import org.iotivity.service.easysetup.impl.EnrolleeDeviceWiFiOnboarding;
import org.iotivity.service.easysetup.impl.WiFiOnBoardingConfig;
import org.iotivity.service.easysetup.impl.WiFiProvConfig;
import org.iotivity.service.easysetup.mediator.EnrolleeInfo;
import org.iotivity.service.easysetup.mediator.ProvisionEnrollee;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.net.wifi.WifiConfiguration;
import android.os.Bundle;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends Activity {
    TextView          textView1;

    static final int  REQUEST_IMAGE_CAPTURE = 1;
    ImageView         imageView;
    ProvisionEnrollee provisionEnrolleInstance;

    /****Start new API implementation */
    WiFiProvConfig mWiFiProvConfig;
    WiFiOnBoardingConfig mWiFiOnBoardingConfig;
    EasySetupService mEasySetupService ;
    EnrolleeDeviceFactory mDeviceFactory;
    EnrolleeDevice mDevice;

 /*   IOnBoardingStatus deviceScanListener = new IOnBoardingStatus() {

           @Override
            public void deviceOnBoardingStatus(EnrolleeInfo enrolleStatus) {
                // TODO Auto-generated method stub
                // TODO Auto-generated method stub
                if (enrolleStatus != null && enrolleStatus.getIpAddr() != null) {
                    String finalResult = "Easy Connect : ";

                    if (enrolleStatus.isReachable()) {
                        finalResult = "Device OnBoarded" + "["
                                + enrolleStatus.getIpAddr() + "]";


                         * easySetupInstance.StartEasySetup(enrolleStatus.getIpAddr()) ;
                         * easySetupCount++; Log.i("EasyConnect",
                         * "easy Setup Count-"+easySetupCount); Log.i("EasyConnect",
                         * "IP Address-"+enrolleStatus.getIpAddr());

                        connectedDevice = enrolleStatus;
                    } else {
                        finalResult = "Device Removed" + "["
                                + enrolleStatus.getIpAddr() + "]";
                    }

                    textView1.setText("");
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
                            Toast.LENGTH_LONG).show();


                     * myTimer2 = new Timer(); myTimer2.schedule(new TimerTask() {
                     *
                     * @Override public void run() {
                     * easySetupInstance.StartEasySetup(enrolleStatus.getIpAddr());
                     * easySetupCount++; Log.i("EasyConnect",
                     * "easy Setup Count-"+easySetupCount); Log.i("EasyConnect",
                     * "IP Address-"+enrolleStatus.getIpAddr()); }
                     *
                     * }, 0, 10000);

                }
            }
    };
    */


    /* End new API implementation*/

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        textView1 = (TextView) findViewById(R.id.textView1);

        // OnBoarding Process
        // onBoardingHandlerInstance = new OnBoardEnrollee(this);

        //onBoardingHandlerInstance.registerOnBoardingStatusHandler(this);

        // Provisioning Process
        provisionEnrolleInstance = new ProvisionEnrollee(this);
        // provisionEnrolleInstance.registerProvisioningHandler(this);

  /*      // Get intent, action and MIME type
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
                //onBoardingHandlerInstance.startDeviceScan();
                mWifiSoftAPManager.getClientList(false, deviceScanListener);
            }

        }, 0, 2000);*/


        /* Start new API implementation */

        mEasySetupService = EasySetupService.getInstance(getApplicationContext(), new EasySetupStatus() {

            @Override
            public void onFinished(EnrolleeDevice enrolledevice) {
                final String msg = enrolledevice.isSetupSuccessful()? "Device configured successfully" : "Device configuration failed" ;
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Toast.makeText(getApplicationContext(), msg, Toast.LENGTH_SHORT).show();
                    }
                });
            }

            @Override
            public void onProgress(EnrolleeState state) {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Toast.makeText(getApplicationContext(), "Device state changed", Toast.LENGTH_SHORT).show();
                    }
                });
            }

        });

        mDeviceFactory = EnrolleeDeviceFactory.newInstance(getApplicationContext());

        mDevice = mDeviceFactory.newEnrolleeDevice(getOnBoardingWifiConfig(), getEnrollerWifiConfig());

        addListenerForStartAP();
        addListenerForStopAP();

        /* End new API implementation */

    }

    public WiFiProvConfig getEnrollerWifiConfig() {
        mWiFiProvConfig = new WiFiProvConfig("EasySetup123", "EasySetup123");
        return mWiFiProvConfig;
    }

    public WiFiOnBoardingConfig getOnBoardingWifiConfig() {
        //TODO : Check proper configuration
        mWiFiOnBoardingConfig = new WiFiOnBoardingConfig();
        mWiFiOnBoardingConfig.setSSId("EasySetup123");
        mWiFiOnBoardingConfig.setSharedKey("EasySetup123");
        mWiFiOnBoardingConfig.setAuthAlgo(WifiConfiguration.AuthAlgorithm.OPEN);
        mWiFiOnBoardingConfig.setKms(WifiConfiguration.KeyMgmt.WPA_PSK);
        return mWiFiOnBoardingConfig;
    }


    public void onDestroy() {
        super.onDestroy();
        provisionEnrolleInstance.stopEnrolleeProvisioning(0);
        //onBoardingHandlerInstance.disableWiFiAP();
        //  mWifiSoftAPManager.setWifiApEnabled(null, false);

        //finish();
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

/*    @Override
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
    }*/

/*    void handleSendText(Intent intent) {
        String sharedText = intent.getStringExtra(Intent.EXTRA_TEXT);
        if (sharedText != null) {

          WifiConfiguration netConfig = new WifiConfiguration();
            netConfig.SSID = "DLNA_LISMORE1";
            netConfig.allowedAuthAlgorithms
                    .set(WifiConfiguration.AuthAlgorithm.OPEN);
            netConfig.allowedKeyManagement
                    .set(WifiConfiguration.KeyMgmt.WPA_PSK);
            netConfig.preSharedKey = "dlna@010203";

            mWifiSoftAPManager.setWifiApEnabled((WifiConfiguration)getOnBoardingWifiConfig().getConfig(), true);

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
    }*/

/*    private void scan() {
        //onBoardingHandlerInstance.registerOnBoardingStatusHandler(this);
        mWifiSoftAPManager.getClientList(false, deviceScanListener);
        //onBoardingHandlerInstance.startDeviceScan();
    }*/

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

/*    public void addListenerOnProvisioning() {
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

    }*/

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == REQUEST_IMAGE_CAPTURE && resultCode == RESULT_OK) {
            Bundle extras = data.getExtras();
            Bitmap imageBitmap = (Bitmap) extras.get("data");
            imageView.setImageBitmap(imageBitmap);
        }
    }


}

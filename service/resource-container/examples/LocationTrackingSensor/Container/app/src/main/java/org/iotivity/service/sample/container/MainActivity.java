//******************************************************************
//
// Copyright 2015 Duseok Kang (Seoul National University) All Rights Reserved.
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

package org.iotivity.service.sample.container;

import android.annotation.SuppressLint;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.res.AssetManager;
import android.net.ConnectivityManager;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Gravity;
import android.widget.Toast;

import org.iotivity.base.ModeType;
import org.iotivity.base.OcConnectivityType;
import org.iotivity.base.OcException;
import org.iotivity.base.OcHeaderOption;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcResource;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;

import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.EnumSet;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class MainActivity extends AppCompatActivity implements
        OcPlatform.OnResourceFoundListener,
        OcResource.OnPutListener {
    private final String LOG_TAG = "[" + "MainActivity" + "]";
    private static String logMessage;

    private static MainActivity resContainerActivityInstance;

    private static Handler mHandler;

    private OcResource accelResource;
    private OcResource linearAccelResource;
    private OcResource magneticResource;
    private OcResource wifiResource;
    private OcResource trackingResource;
    private OcResource.OnPutListener trackingResourceListener;

    public static MainActivity getActivity() {
        return resContainerActivityInstance;
    }

    public static void setMessageLog(String message) {
        logMessage = message;
    }

    private void startClient() {
        configurePlatform();

            try {
                Log.i(LOG_TAG, "Find Resource");
                OcPlatform.findResource("", "/oic/res?rt=", EnumSet.of(OcConnectivityType.CT_DEFAULT), this);
                Log.i(LOG_TAG, "Find Resource called");
            } catch (OcException e) {
                e.printStackTrace();
            }
        do {
            try {
                wait(500);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            getAccelResource();
            getLinearAccelResource();
            getMagneticResource();
            getWifiResource();
        }
        while (true);
    }

    @Override
    public void onPutCompleted(List<OcHeaderOption> list, OcRepresentation ocRepresentation) {
        //Log.i(LOG_TAG, "put completed");

    }

    @Override
    public void onPutFailed(Throwable throwable) {

    }

    @SuppressLint("HandlerLeak")
    private class handleMsg extends Handler {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case 0:
                    break;
                case 1:
                    // for log
                    Toast toast = Toast.makeText(getApplicationContext(),
                            logMessage, Toast.LENGTH_LONG);
                    toast.setGravity(Gravity.CENTER, 0, 0);
                    toast.show();
                    break;
                case 2:
                    break;
            }
        }
    }

    public synchronized void onResourceFound(OcResource ocResource) {
        Log.i(LOG_TAG, "Uri : " + ocResource.getUri());
        Log.i(LOG_TAG, "Host : " + ocResource.getHost());
        Log.i(LOG_TAG, "Type : " + ocResource.getResourceTypes());
        switch(ocResource.getUri()) {
            case "/android/accel/1":
                accelResource = ocResource;
                break;
            case "/android/linearaccel/1":
                linearAccelResource = ocResource;
                break;
            case "/android/magnetic/1":
                magneticResource = ocResource;
                break;
            case "/android/wifi/1":
                wifiResource = ocResource;
                break;
            case "/android/trackingsensor/1":
                trackingResource = ocResource;
                break;
            default:
                break;
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        final String sdCardPath;

        super.onCreate(savedInstanceState);

        if (!isWifiConnected()){
            showWifiUnavailableDialog();
            return;
        }
        setContentView(R.layout.activity_main);

        resContainerActivityInstance = this;

        ResourceContainer resContainerInstance = new ResourceContainer();

        CopyAssetsToSDCard();
        sdCardPath = this.getFilesDir().getPath();

        mHandler = new handleMsg();
        setHandler(mHandler);

        resContainerInstance.startContainer(sdCardPath);

        resContainerInstance.addTracking();
        resContainerInstance.startTracking();

        trackingResourceListener = this;

        new Thread(new Runnable() {
            public void run() {
                startClient();
            }
        }).start();
    }

    private void getMagneticResource() {
        Map<String, String> map = new HashMap<>();
        OcResource.OnGetListener magneticResourceListener = new OcResource.OnGetListener() {
            @Override
            public void onGetCompleted(List<OcHeaderOption> list, OcRepresentation ocRepresentation) {
                double value[] = new double[3];
                try {
                    value[0] = ocRepresentation.getValue("mag_x");
                    value[1] = ocRepresentation.getValue("mag_y");
                    value[2] = ocRepresentation.getValue("mag_z");
                } catch (OcException e) {
                    e.printStackTrace();
                }

                Map<String, String> map = new HashMap<>();
                try {
                    trackingResource.put(ocRepresentation, map, trackingResourceListener);
                } catch (OcException e) {
                    e.printStackTrace();
                }

                Log.i(LOG_TAG, "magnetic : " + value[0] + ", " + value[1] + ", " + value[2]);
            }

            @Override
            public void onGetFailed(Throwable throwable) {

            }
        };

        try {
            magneticResource.get(map, magneticResourceListener);
        } catch (OcException e) {
            e.printStackTrace();
        }
    }

    private void getAccelResource() {
        Map<String, String> map = new HashMap<>();
        OcResource.OnGetListener accelResourceListener = new OcResource.OnGetListener() {
            @Override
            public void onGetCompleted(List<OcHeaderOption> list, OcRepresentation ocRepresentation) {
                double value[] = new double[3];

                try {
                    value[0] = ocRepresentation.getValue("accel_x");
                    value[1] = ocRepresentation.getValue("accel_y");
                    value[2] = ocRepresentation.getValue("accel_z");
                } catch (OcException e) {
                    e.printStackTrace();
                }

                Map<String, String> map = new HashMap<>();
                try {
                    trackingResource.put(ocRepresentation, map, trackingResourceListener);
                } catch (OcException e) {
                    e.printStackTrace();
                }

                Log.i(LOG_TAG, "acceleration : " + value[0] + ", " + value[1] + ", " + value[2]);
            }

            @Override
            public void onGetFailed(Throwable throwable) {

            }
        };

        try {
            accelResource.get(map, accelResourceListener);
        } catch (OcException e) {
            e.printStackTrace();
        }
    }

    private void getLinearAccelResource() {
        Map<String, String> map = new HashMap<>();
        OcResource.OnGetListener linearaccelResourceListener = new OcResource.OnGetListener() {
            @Override
            public void onGetCompleted(List<OcHeaderOption> list, OcRepresentation ocRepresentation) {
                double value[] = new double[3];
                try {
                    value[0] = ocRepresentation.getValue("linearaccel_x");
                    value[1] = ocRepresentation.getValue("linearaccel_y");
                    value[2] = ocRepresentation.getValue("linearaccel_z");
                } catch (OcException e) {
                    e.printStackTrace();
                }

                Map<String, String> map = new HashMap<>();
                try {
                    trackingResource.put(ocRepresentation, map, trackingResourceListener);
                } catch (OcException e) {
                    e.printStackTrace();
                }

                Log.i(LOG_TAG, "linear acceleration : " + value[0] + ", " + value[1] + ", " + value[2]);
            }

            @Override
            public void onGetFailed(Throwable throwable) {

            }
        };

        try {
            linearAccelResource.get(map, linearaccelResourceListener);
        } catch (OcException e) {
            e.printStackTrace();
        }
    }

    private void getWifiResource() {
        Map<String, String> map = new HashMap<>();
        OcResource.OnGetListener wifiResourceListener = new OcResource.OnGetListener() {
            @Override
            public void onGetCompleted(List<OcHeaderOption> list, OcRepresentation ocRepresentation) {
                int value[] = new int[2];

                try {
                    value[0] = ocRepresentation.getValue("wifi_rssi");
                    value[1] = ocRepresentation.getValue("wifi_frequency");
                } catch (OcException e) {
                    e.printStackTrace();
                }

                Map<String, String> map = new HashMap<>();
                try {
                    trackingResource.put(ocRepresentation, map, trackingResourceListener);
                } catch (OcException e) {
                    e.printStackTrace();
                }

                Log.i(LOG_TAG, "wifi : " + value[0] + ", " + value[1]);
            }

            @Override
            public void onGetFailed(Throwable throwable) {

            }
        };
        try {
            wifiResource.get(map, wifiResourceListener);
        } catch (OcException e) {
            e.printStackTrace();
        }
    }

    public Handler getHandler() {
        return mHandler;
    }

    public void setHandler(Handler mHandler) {
        MainActivity.mHandler = mHandler;
    }

    private void CopyAssetsToSDCard() {
        AssetManager assetManager = getAssets();
        String[] files = null;
        try {
            files = assetManager.list("lib");
        } catch (IOException e) {
            Log.e(LOG_TAG, e.getMessage());
        }

        assert files != null;
        for (String filename : files) {
            InputStream in;
            OutputStream out;
            try {
                in = assetManager.open("lib/" + filename);
                out = new FileOutputStream(resContainerActivityInstance.getFilesDir().getParent()
                        + "/files/" + filename);
                copyIndividualFile(in, out);
                in.close();
                out.flush();
                out.close();
            } catch (Exception e) {
                Log.e(LOG_TAG, e.getMessage());
            }
        }
    }

    private void copyIndividualFile(InputStream in, OutputStream out)
            throws IOException {
        Log.i(LOG_TAG, "copyIndividualFile");
        byte[] buffer = new byte[2048];
        int read;
        while ((read = in.read(buffer)) != -1) {
            out.write(buffer, 0, read);
        }
    }

    private void showWifiUnavailableDialog() {
        new AlertDialog.Builder(this).setTitle("Error")
                .setMessage("WiFi is not enabled/connected!")
                .setCancelable(false)
                .setPositiveButton("OK", new DialogInterface.OnClickListener(){
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        finish();
                    }
                }).create().show();
    }

    private void configurePlatform() {
        OcPlatform.Configure(new PlatformConfig(getApplicationContext(),
                ServiceType.IN_PROC, ModeType.CLIENT_SERVER, "0.0.0.0", 0, QualityOfService.LOW));
        Log.i(LOG_TAG, "Configuration done successfully");
    }

    private boolean isWifiConnected() {
        ConnectivityManager connManager = (ConnectivityManager) getSystemService(CONNECTIVITY_SERVICE);
        //noinspection deprecation
        return connManager.getNetworkInfo(ConnectivityManager.TYPE_WIFI).isConnected();
    }
}

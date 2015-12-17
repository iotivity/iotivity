//******************************************************************
//
// Copyright 2015 Euiseok Kim (Seoul National University) All Rights Reserved.
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

package org.iotivity.service.sample.sleepplacesensorcontainer;

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
import org.iotivity.base.OcResource.OnGetListener;
import org.iotivity.base.OcResource.OnPutListener;
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

public class ResourceContainerActivity extends AppCompatActivity implements
        OcPlatform.OnResourceFoundListener, OcResource.OnPutListener {
    private final String LOG_TAG = "[" + ResourceContainerActivity.class.getName() + "]";
    private static String logMessage;

    private static ResourceContainerActivity resContainerActivityInstance;
    private ResourceContainer resContainerInstance;
    private OcResource lightResource;
    private OcResource emfResource;
    private OcResource soundResource;
    private OcResource temphumResource;
    private OcResource sleepPlaceSensorResource;
    private OnPutListener sleepPlaceSensorListener;

    private static Handler mHandler;

    public static ResourceContainerActivity getActivity() {
        return resContainerActivityInstance;
    }

    public static void setMessageLog(String message) {
        logMessage = message;
    }

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

    public void onPutCompleted(List<OcHeaderOption> list, OcRepresentation ocRep)   {

    }

    public synchronized void onResourceFound(OcResource ocResource) {
        Log.i(LOG_TAG, "MainActivity " + ocResource.getUri() + " found");
        if(ocResource.getUri().equals("/android/lightsensor/1"))
        {
            lightResource = ocResource;
        }
        else if(ocResource.getUri().equals("/android/emfsensor/1"))
        {
            emfResource = ocResource;
        }
        else if(ocResource.getUri().equals("/android/soundsensor/1"))
        {
            soundResource = ocResource;
        }
        else if(ocResource.getUri().equals("/Thing_TempHumSensor"))
        {
            temphumResource = ocResource;
        }
        else if(ocResource.getUri().equals("/android/sleepplacesensor/1"))
        {
            sleepPlaceSensorResource = ocResource;
        }
    }

    public void onPutFailed(Throwable throwable) {

    }

    private void getLightResource() {
        Map<String, String> map = new HashMap<>();
        OnGetListener lightListener = new OnGetListener() {
            @Override
            public void onGetCompleted(List<OcHeaderOption> list, OcRepresentation ocRepresentation) {
                int value = 0;
                try {
                    ocRepresentation.getValue("light-intensity");
                } catch (OcException e) {
                    e.printStackTrace();
                }

                Map<String, String> map = new HashMap<>();
                try {
                    sleepPlaceSensorResource.put(ocRepresentation, map, sleepPlaceSensorListener);
                }
                catch (OcException e) {
                    e.printStackTrace();
                }

                Log.i(LOG_TAG, "light-intensity: " + value);

            }

            @Override
            public void onGetFailed(Throwable throwable) {

            }
        };

        if(lightResource != null)
        {
            try {
                lightResource.get(map, lightListener);
            } catch (OcException e) {
                e.printStackTrace();
            }
        }

    }

    private void getEMFResource() {
        Map<String, String> map = new HashMap<>();
        OnGetListener emfListener = new OnGetListener() {
            @Override
            public void onGetCompleted(List<OcHeaderOption> list, OcRepresentation ocRepresentation) {
                double value = 0;
                try {
                    ocRepresentation.getValue("emf-intensity");
                } catch (OcException e) {
                    e.printStackTrace();
                }

                Map<String, String> map = new HashMap<>();
                try {
                    sleepPlaceSensorResource.put(ocRepresentation, map, sleepPlaceSensorListener);
                }
                catch (OcException e) {
                    e.printStackTrace();
                }

                Log.i(LOG_TAG, "emf-intensity: " + value);

            }

            @Override
            public void onGetFailed(Throwable throwable) {

            }
        };

        if(emfResource != null)
        {
            try {
                emfResource.get(map, emfListener);
            } catch (OcException e) {
                e.printStackTrace();
            }
        }
    }

    private void getSoundResource() {
        Map<String, String> map = new HashMap<>();
        OnGetListener soundListener = new OnGetListener() {
            @Override
            public void onGetCompleted(List<OcHeaderOption> list, OcRepresentation ocRepresentation) {
                double value = 0;
                try {
                    value = ocRepresentation.getValue("sound-intensity");
                } catch (OcException e) {
                    e.printStackTrace();
                }

                Map<String, String> map = new HashMap<>();
                try {
                    sleepPlaceSensorResource.put(ocRepresentation, map, sleepPlaceSensorListener);
                }
                catch (OcException e) {
                    e.printStackTrace();
                }

                Log.i(LOG_TAG, "sound-intensity: " + value);

            }

            @Override
            public void onGetFailed(Throwable throwable) {

            }
        };
        if(soundResource != null) {
            try {
                soundResource.get(map, soundListener);
            } catch (OcException e) {
                e.printStackTrace();
            }
        }
    }

    private void getTempHumidResource() {
        Map<String, String> map = new HashMap<>();
        OnGetListener tempHumListener = new OnGetListener() {
            @Override
            public void onGetCompleted(List<OcHeaderOption> list, OcRepresentation ocRepresentation) {
                double tempValue = 0;
                double humValue = 0;
                try {
                    tempValue = Double.parseDouble((String) ocRepresentation.getValue("temperature"));
                    humValue = Double.parseDouble((String) ocRepresentation.getValue("humidity"));
                } catch (OcException e) {
                    e.printStackTrace();
                }

                Map<String, String> map = new HashMap<>();
                try {
                    sleepPlaceSensorResource.put(ocRepresentation, map, sleepPlaceSensorListener);
                }
                catch (OcException e) {
                    e.printStackTrace();
                }

                Log.i(LOG_TAG, "temperature: " + tempValue + ", humValue: " + humValue);

            }

            @Override
            public void onGetFailed(Throwable throwable) {

            }
        };

        if(temphumResource != null)
        {
            try {
                temphumResource.get(map, tempHumListener);
            } catch (OcException e) {
                e.printStackTrace();
            }
        }
    }

    private void resourceMonitor() {
        try {
            OcPlatform.findResource("", "/oic/res", EnumSet.of(OcConnectivityType.CT_DEFAULT), this);
        }
        catch(OcException e)
        {
            e.printStackTrace();
        }

        try {
            OcPlatform.findResource("", "/oic/res", EnumSet.of(OcConnectivityType.CT_DEFAULT), this);
        }
        catch(OcException e)
        {
            e.printStackTrace();
        }

        while(true) {
            {
                try {
                    //Thread.sleep(300);
                    synchronized (this)
                    {
                        wait(500);
                    }
                }
                catch(InterruptedException e)
                {
                    e.printStackTrace();
                }

                getLightResource();
                getEMFResource();
                getSoundResource();
                getTempHumidResource();
            }
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

        lightResource = null;
        emfResource = null;
        soundResource = null;
        temphumResource = null;
        sleepPlaceSensorResource = null;
        sleepPlaceSensorListener = this;

        resContainerActivityInstance = this;
        sdCardPath = this.getFilesDir().getPath();

        //Log.e(LOG_TAG, "Main activity path: " + resContainerActivityInstance.getFilesDir());

        CopyAssetsToSDCard();
        configurePlatform();

        this.resContainerInstance = new ResourceContainer();

        mHandler = new handleMsg();
        setHandler(mHandler);

        this.resContainerInstance.startContainer(sdCardPath);

        new Thread(new Runnable() {
            public void run() {
                resourceMonitor();
            }
        }).start();
    }

    public Handler getHandler() {
        return mHandler;
    }

    public void setHandler(Handler mHandler) {
        ResourceContainerActivity.mHandler = mHandler;
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

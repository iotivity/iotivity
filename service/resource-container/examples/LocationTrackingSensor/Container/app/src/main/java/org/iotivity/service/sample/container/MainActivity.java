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
import org.iotivity.base.OcPlatform;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;

import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public class MainActivity extends AppCompatActivity {
    private final String LOG_TAG = "[" + MainActivity.class.getName() + "]";
    private static String logMessage;

    private static MainActivity resContainerActivityInstance;
    private ResourceContainer resContainerInstance;

    private static Handler mHandler;

    public static MainActivity getActivity() {
        return resContainerActivityInstance;
    }

    public static void setMessageLog(String message) {
        logMessage = message;
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

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        final String sdCardPath;

        super.onCreate(savedInstanceState);

        if (!isWifiConnected()){
            showWifiUnavailableDialog();
            return;
        }
        setContentView(R.layout.activity_main);

        configurePlatform();
        resContainerActivityInstance = this;

        resContainerInstance = new ResourceContainer();

        CopyAssetsToSDCard();
        sdCardPath = this.getFilesDir().getPath();

        mHandler = new handleMsg();
        setHandler(mHandler);

        resContainerInstance.startContainer(sdCardPath);

        resContainerInstance.addTracking();
        resContainerInstance.startTracking();
//        resContainerInstance.addResourceConfig();
//        resContainerInstance.addAndroidResource();
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

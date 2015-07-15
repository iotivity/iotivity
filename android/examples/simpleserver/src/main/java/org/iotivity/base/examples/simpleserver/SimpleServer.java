/*
 * //******************************************************************
 * //
 * // Copyright 2015 Intel Corporation.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 * //
 * // Licensed under the Apache License, Version 2.0 (the "License");
 * // you may not use this file except in compliance with the License.
 * // You may obtain a copy of the License at
 * //
 * //      http://www.apache.org/licenses/LICENSE-2.0
 * //
 * // Unless required by applicable law or agreed to in writing, software
 * // distributed under the License is distributed on an "AS IS" BASIS,
 * // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * // See the License for the specific language governing permissions and
 * // limitations under the License.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

package org.iotivity.base.examples.simpleserver;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.Message;
import android.support.v4.content.LocalBroadcastManager;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.content.SharedPreferences;
import android.content.res.AssetManager;
import android.preference.PreferenceManager;

import org.iotivity.base.ModeType;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import base.iotivity.org.examples.message.IMessageLogger;

/**
 * SimpleServer
 *
 * SimpleServer instantiates a TextView and creates and configures OICPlatform.
 * It also creates a LightResource and waits for the incoming client requests to handle specific scenarios.
 * This implements IMessageLogger to display messages on the screen
 */

public class SimpleServer extends Activity implements IMessageLogger {
    private final static String TAG = "SimpleServer: ";
    private static final int BUFFER_SIZE = 1024;
    private String filePath = "";
    private TextView mEventsTextView;
    private MessageReceiver mMessageReceiver = new MessageReceiver();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        registerReceiver(mMessageReceiver, new IntentFilter("org.iotivity.base.examples.simpleserver"));

        mEventsTextView = new TextView(this);
        mEventsTextView.setMovementMethod(new ScrollingMovementMethod());
        LinearLayout layout = (LinearLayout)findViewById(R.id.linearLayout);
        layout.addView(mEventsTextView, new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, 0, 1f));

        OcRepresentation rep = new OcRepresentation();
        rep.setValueBool("test", false);
        boolean result = rep.getValueBool("test");
        filePath = getFilesDir().getPath() + "/";//  data/data/<package>/files/
        //copy json when application runs first time
        SharedPreferences wmbPreference = PreferenceManager.getDefaultSharedPreferences(this);
        boolean isFirstRun = wmbPreference.getBoolean("FIRSTRUN", true);
        if (isFirstRun) {
            copyJsonFromAsset();
            SharedPreferences.Editor editor = wmbPreference.edit();
            editor.putBoolean("FIRSTRUN", false);
            editor.commit();
        }

        initOICStack();
    }

    /**
     * Copy svr db json file from assets folder to app data files dir
     */
    private void copyJsonFromAsset() {
        AssetManager assetManager = getAssets();

        InputStream in = null;
        OutputStream out = null;
        try {

            in = assetManager.open(StringConstants.OIC_SERVER_JSON_DB_FILE);
            File file = new File(filePath);
            //check files directory exists
            if (!(file.exists() && file.isDirectory())) {
                file.mkdirs();
            }
            out = new FileOutputStream(filePath + StringConstants.OIC_SERVER_JSON_DB_FILE);
            copyFile(in, out);
        } catch (NullPointerException e) {
            logMessage(TAG + "Null pointer exception " + e.getMessage());
            Log.e(TAG, e.getMessage());
        } catch (FileNotFoundException e) {
            logMessage(TAG + "Json svr db file not found " + e.getMessage());
            Log.e(TAG, e.getMessage());
        } catch (IOException e) {
            logMessage(TAG + StringConstants.OIC_SERVER_JSON_DB_FILE + " file copy failed");
            Log.e(TAG, e.getMessage());
        } finally {
            if (in != null) {
                try {
                    in.close();
                } catch (IOException e) {
                    Log.e(TAG, e.getMessage());
                }
            }
            if (out != null) {
                try {
                    out.close();
                } catch (IOException e) {
                    Log.e(TAG, e.getMessage());
                }
            }
        }
    }

    private void copyFile(InputStream in, OutputStream out) throws IOException {
        byte[] buffer = new byte[BUFFER_SIZE];
        int read;
        while ((read = in.read(buffer)) != -1) {
            out.write(buffer, 0, read);
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
                ModeType.SERVER,
                "0.0.0.0", // bind to all available interfaces
                0,
                QualityOfService.LOW,
                filePath + StringConstants.OIC_SERVER_JSON_DB_FILE);
        OcPlatform.Configure(cfg);
        // Create instance of lightResource
        LightResource myLight = new LightResource(this);
        // create and register a resource
        myLight.createResource0();
    }

    public class MessageReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String message = intent.getStringExtra(StringConstants.MESSAGE);
            logMessage(message);
        }
    }

    public void logMessage(final String text) {
        runOnUiThread(new Runnable() {
            public void run() {
                final Message msg = new Message();
                msg.obj = text;
                mEventsTextView.append("\n");
                mEventsTextView.append(text);
            }
        });
        Log.i(TAG, text);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();
        if (id == R.id.action_settings) {
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        onStop();
    }

    @Override
    protected void onStop() {
        LocalBroadcastManager.getInstance(this).unregisterReceiver(mMessageReceiver);
        super.onStop();
    }
}
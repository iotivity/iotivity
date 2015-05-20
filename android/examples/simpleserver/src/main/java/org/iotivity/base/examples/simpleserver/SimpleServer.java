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

import org.iotivity.base.ModeType;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;

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

        initOICStack();
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
                QualityOfService.LOW);
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
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

package org.iotivity.base.examples.fridgeserver;

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
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;

import base.iotivity.org.examples.message.IMessageLogger;

/**
 * FridgeServer
 * <p/>
 * This is the main fridgeServer class. This instantiates Refrigerator object
 * which has different resources (DeviceResource, LightResource, DoorResource).
 */
public class FridgeServer extends Activity implements IMessageLogger {
    private Context mContext;
    private static String TAG = "FridgeServer: ";
    private TextView mEventsTextView;
    private MessageReceiver mMessageReceiver = new MessageReceiver();
    private Refrigerator refrigerator;

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
        logMessage(TAG + "Creating refrigerator resources");

        refrigerator = new Refrigerator(mContext);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_fridge_server);
        registerReceiver(mMessageReceiver, new IntentFilter(StringConstants.INTENT));

        mEventsTextView = new TextView(this);
        mEventsTextView.setMovementMethod(new ScrollingMovementMethod());
        LinearLayout layout = (LinearLayout) findViewById(R.id.linearLayout);
        layout.addView(mEventsTextView, new LinearLayout.LayoutParams
                (LinearLayout.LayoutParams.MATCH_PARENT, 0, 1f));
        mContext = this;

        initOICStack();
    }

    public class MessageReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String message = intent.getStringExtra(StringConstants.MESSAGE);
            logMessage(message);
        }
    }

    @Override
    public void logMessage(final String text) {
        if (StringConstants.ENABLE_PRINTING) {
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
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_fridge_server, menu);
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

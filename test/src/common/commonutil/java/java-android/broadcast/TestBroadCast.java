/******************************************************************
 * Copyright 2018 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************/

package org.iotivity.test.braodcast;

import java.util.EnumSet;
import android.app.ActivityManager;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.util.Log;

public class TestBroadCast {
    private static final String      TAG   = "TestBroadCast";

    private static Context           s_mContext;
    private static BroadcastReceiver s_mBroadcastReciever;
    private boolean                  mFlag = false;
    private IntentFilter             mIntentFilter;

    public TestBroadCast(Context context) {
        s_mContext = context;
        mFlag = false;
        s_mBroadcastReciever = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {

                Log.d(TAG, "TestBroadCast BroadcastReceiver Invoked");
                mFlag = true;

            }
        };

        try {
            s_mContext.unregisterReceiver(s_mBroadcastReciever);

        } catch (IllegalArgumentException e) {
            Log.d(TAG, "tcpClient Broadcast is not registered");
        }
        mIntentFilter = new IntentFilter("tcpClient");
        s_mContext.registerReceiver(s_mBroadcastReciever, mIntentFilter);
    }

    public void clearAllListener() {
        s_mContext.unregisterReceiver(s_mBroadcastReciever);
    }

    public boolean sendBroadCastToTcpClient(String command,
            boolean waitForCallback) {
        Log.d(TAG, "Exec Command : " + command);
        Intent startServer = new Intent("service");
        startServer.putExtra("key", command);
        s_mContext.sendBroadcast(startServer);
        if (waitForCallback) {
            return waitTillBroadCastInvoked();
        }

        return true;
    }

    private boolean waitTillBroadCastInvoked() {
        for (int i = 0; i < 60; i++) {
            delay(1);
            if (mFlag == true) {
                mFlag = false;
                return true;
            }
            Log.d(TAG, "Waiting");
        }
        return false;
    }

    private static void delay(int second) {
        try {
            Thread.sleep(second * 1000);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}

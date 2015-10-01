/**
 * ***************************************************************
 * <p>
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 * <p>
 * <p>
 * <p>
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * <p>
 * http://www.apache.org/licenses/LICENSE-2.0
 * <p>
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * <p>
 * ****************************************************************
 */

package org.iotivity.service.notificationservice.impl;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.util.Log;

import org.iotivity.service.notificationservice.NotificationService;

public class ConnectivityHandler extends BroadcastReceiver {
    @Override
    public void onReceive(Context context, Intent intent) {
        ConnectivityManager connectivityManager = (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo networkIno = connectivityManager.getActiveNetworkInfo();
        boolean isConnected = networkIno != null && networkIno.isConnectedOrConnecting();

        Intent i = new Intent(context, WifiIntentService.class);
        if (isConnected) {


            i.putExtra("WIFISTATUS",true);
            context.startService(i);

            /*final NotificationService notificationService = NotificationService.getInstance();

            new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        Thread.sleep(3000);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                    Log.d("ConnectivityHandler", "wifi state connected");
                    notificationService.discover();

                }
            }).start();
            */

        } else {
            i.putExtra("WIFISTATUS",false);
            context.startService(i);
            Log.d("ConnectivityHandler", "wifi state Disconnected");
        }

    }
}

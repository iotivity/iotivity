/******************************************************************
 *
 * Copyright 2014 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

package org.iotivity.ca;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.util.Log;

public class CaIpInterface {
    private static Context mContext;
    private static volatile boolean isIpInitialized = false;
    private static String TAG          = "OIC_IP_CB_INTERFACE";

    private CaIpInterface(Context context) {
        synchronized(CaIpInterface.class) {
            mContext = context;
        }
        if (!isIpInitialized) {
            registerIpStateReceiver();
            isIpInitialized = true;
        }
    }

    private void registerIpStateReceiver() {
        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(ConnectivityManager.CONNECTIVITY_ACTION);

        mContext.registerReceiver(mReceiver, intentFilter);
    }

    public static void destroyIpInterface() {
        if (isIpInitialized) {
            mContext.unregisterReceiver(mReceiver);
            isIpInitialized = false;
        }
    }

    private static BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            if (intent.getAction().equals(ConnectivityManager.CONNECTIVITY_ACTION)) {
                NetworkInfo activeNetwork = ((ConnectivityManager) mContext
                        .getSystemService(Context.CONNECTIVITY_SERVICE)).getActiveNetworkInfo();
                if (activeNetwork != null && activeNetwork.isConnected()) {
                    Log.d(TAG, "CONNECTIVITY_ACTION - activeNetwork: "
                            + activeNetwork.getTypeName());
                    caIpStateEnabled();
                } else {
                    Log.d(TAG, "CONNECTIVITY_ACTION - activeNetwork: NONE");
                    caIpStateDisabled();
                }
            }
        }
    };

    private native static void caIpStateEnabled();

    private native static void caIpStateDisabled();
}

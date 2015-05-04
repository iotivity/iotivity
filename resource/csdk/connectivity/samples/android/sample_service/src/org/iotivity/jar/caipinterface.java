package org.iotivity.jar;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.wifi.WifiManager;
import android.util.Log;

public class caipinterface {

    private static String TAG = "caipinterface";
    private static Context mContext = null;

    public caipinterface(Context context) {
        Log.d(TAG, "caipinterface");
        mContext = context;
        registerIpStateReceiver();
    }

    private void registerIpStateReceiver() {

        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(WifiManager.WIFI_STATE_CHANGED_ACTION);
        intentFilter.addAction(ConnectivityManager.CONNECTIVITY_ACTION);

        mContext.registerReceiver(mReceiver, intentFilter);
    }

    private static BroadcastReceiver mReceiver = new BroadcastReceiver() {

        @Override
        public void onReceive(Context context, Intent intent) {
            if (intent.getIntExtra(WifiManager.EXTRA_WIFI_STATE,
                WifiManager.WIFI_STATE_UNKNOWN) == WifiManager.WIFI_STATE_DISABLED) {
                CAIPStateDisabled();
            } else if (intent.getAction().equals(ConnectivityManager.CONNECTIVITY_ACTION)) {
                ConnectivityManager manager = (ConnectivityManager)
                        mContext.getSystemService(Context.CONNECTIVITY_SERVICE);
                NetworkInfo nwInfo = manager.getNetworkInfo(ConnectivityManager.TYPE_WIFI);

                if(nwInfo.isConnected()) {
                    CAIPStateEnabled();
                }
            }
        }
    };

    public native static void CAIPStateEnabled();

    public native static void CAIPStateDisabled();
}


package com.iotivity.jar;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.wifi.WifiManager;
import android.util.Log;

public class CAWiFiInterface {

    private static String TAG = "CAWiFiInterface";
    private static Context mContext = null;

    public CAWiFiInterface(Context context) {
        Log.d(TAG, "CAWiFiInterface");
        mContext = context;
        registerWiFiStateReceiver();
    }

    private void registerWiFiStateReceiver() {

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
                CAWiFiStateDisabled();
            } else if (intent.getAction().equals(ConnectivityManager.CONNECTIVITY_ACTION)) {
            	ConnectivityManager manager = (ConnectivityManager) mContext.getSystemService(Context.CONNECTIVITY_SERVICE);
            	NetworkInfo nwInfo = manager.getNetworkInfo(ConnectivityManager.TYPE_WIFI);

            	if(nwInfo.isConnected()) {
            		CAWiFiStateEnabled();
                }
            }
        }
    };

    public native static void CAWiFiStateEnabled();

    public native static void CAWiFiStateDisabled();
}

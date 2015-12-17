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

package org.iotivity.service.sample.locationtrackingapp;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.util.Log;

import org.iotivity.service.resourcecontainer.AndroidBundleResource;
import org.iotivity.service.resourcecontainer.RcsResourceAttributes;
import org.iotivity.service.resourcecontainer.RcsValue;

/**
 * Created by duseok on 15. 12. 13.
 */
public class AndroidWifiPowerSensor extends AndroidBundleResource {
    private final String LOG_TAG = "[" + this.getName() + "]";

    private WifiManager wifiManager;
    private WifiInfo wifiInfo;

    public AndroidWifiPowerSensor(Context appContext) {
        super(appContext);
        this.setResourceType("oic.r.wifipower");
        this.setName("androidWifiPower");

        wifiManager = (WifiManager) appContext.getApplicationContext().getSystemService(Context.WIFI_SERVICE);
        wifiInfo = wifiManager.getConnectionInfo();

        int _rssi = wifiInfo.getRssi();

        appContext.getApplicationContext().registerReceiver(rssiReceiver, new IntentFilter(WifiManager.SCAN_RESULTS_AVAILABLE_ACTION));
        wifiManager.startScan();
    }

    private BroadcastReceiver rssiReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            wifiManager.startScan();
            wifiInfo = wifiManager.getConnectionInfo();

            int _rssi = wifiInfo.getRssi();
            int _freq = wifiInfo.getFrequency();

            setAttribute("wifi_rssi", new RcsValue(_rssi), true);
            setAttribute("wifi_frequency", new RcsValue(_freq), true);
        }
    };

    @Override
    protected void initAttributes() {
        this.m_attributes.put("wifi_rssi", 0);
        this.m_attributes.put("wifi_frequency", 0);
    }

    @Override
    public void handleSetAttributesRequest(RcsResourceAttributes rcsResourceAttributes) {
        Log.i(LOG_TAG, "Set Attribute called with ");
        for (String key : rcsResourceAttributes.keySet()) {
            Log.i(LOG_TAG, " " + key + " : " + rcsResourceAttributes.get(key));
        }
    }

    @Override
    public RcsResourceAttributes handleGetAttributesRequest() {
        Log.i(LOG_TAG, "Get Attributes called");
        Log.i(LOG_TAG, "Returning: ");
        for(String key: m_attributes.keySet()) {
            Log.i(LOG_TAG, " " + key + " : " + m_attributes.get(key));
        }
        return this.m_attributes;
    }
}

/**
 * ***************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
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
 *
 * ****************************************************************
 */

package org.iotivity.service.easysetup.mediator;

import android.util.Log;

import org.iotivity.service.easysetup.mediator.enums.WIFI_FREQ;
import org.iotivity.service.easysetup.mediator.enums.WIFI_MODE;

import java.util.ArrayList;

/**
 * This class stores an Enrollee's configuration specific to WiFi configuration
 * supported WiFi modes and frequency are stored
 */
public class WiFiConfig
{
    public static final String TAG = WiFiConfig.class.getName();
    private final ArrayList<WIFI_MODE> mWifiModes;
    private final WIFI_FREQ mWifiFreq;

    /**
     * Constructor
     *
     * @param types
     *          stores an array of Wifi modes like 11G and 11N.
     *          11A(0), 11B, 11G, 11N, 11AC(4) are supported
     * @param wifiFreq
     *          stores WiFi frequency like 2.4G, 5G, or both
     */
    public WiFiConfig(ArrayList<Integer> types, int wifiFreq)
    {
        mWifiModes = convertWifiModes(types);
        mWifiFreq = WIFI_FREQ.fromInt(wifiFreq);
    }

    private ArrayList<WIFI_MODE> convertWifiModes(ArrayList<Integer> types)
    {
        ArrayList<WIFI_MODE> wifiModes = new ArrayList<>();
        for (Integer type: types) {
            wifiModes.add(WIFI_MODE.fromInt(type.intValue()));
        };
        return wifiModes;
    }

    /**
     * get WiFi mode
     *
     * @return ArrayList<WIFI_MODE>
     *                  11A(0), 11B, 11G, 11N, 11AC(4) are supported
     * @see WIFI_MODE
     */
    public ArrayList<WIFI_MODE> getWifiModes()
    {
        return mWifiModes;
    }

    /**
     * get WiFi frequency
     *
     * @param WIFI_FREQ
     *          2.4G, 5G, or both
     *
     * @see WIFI_FREQ
     */
    public WIFI_FREQ getWifiFreq()
    {
        return mWifiFreq;
    }
}

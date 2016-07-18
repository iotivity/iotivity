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

public class WiFiConfig
{
    public static final String TAG = WiFiConfig.class.getName();
    private final ArrayList<WIFI_MODE> mWifiModeTypes;
    private final WIFI_FREQ mWifiFreq;

    public WiFiConfig(ArrayList<Integer> types, int wifiFreq)
    {
        mWifiModeTypes = convertWifiModeTypes(types);
        mWifiFreq = WIFI_FREQ.fromInt(wifiFreq);
    }

    private ArrayList<WIFI_MODE> convertWifiModeTypes(ArrayList<Integer> types)
    {
        ArrayList<WIFI_MODE> wifiModeTypes = new ArrayList<>();
        for (Integer type: types) {
            wifiModeTypes.add(WIFI_MODE.fromInt(type.intValue()));
        };
        return wifiModeTypes;
    }

    public ArrayList<WIFI_MODE> getWifiModeTypes()
    {
        return mWifiModeTypes;
    }

    public WIFI_FREQ getWifiFreq()
    {
        return mWifiFreq;
    }
}

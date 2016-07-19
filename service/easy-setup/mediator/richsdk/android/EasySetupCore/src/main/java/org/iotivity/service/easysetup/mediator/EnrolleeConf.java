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

import org.iotivity.service.easysetup.mediator.enums.WIFI_FREQ;
import org.iotivity.service.easysetup.mediator.enums.WIFI_MODE;

import java.util.ArrayList;

/**
 * This class stores Enrollee's configuration including WiFi and Device configuration
 * including supported WiFi frequency and device name
 */
public class EnrolleeConf
{
    private final DeviceConfig mDevConf;
    private final WiFiConfig mWiFiConf;
    private final boolean mCloudable;

    /**
     * Constructor
     *
     * @param devConf
     *          device name, language, and country
     * @param wifiConf
     *          supported WiFi modes and frequency
     * @param cloudable
     *          a preference if Enrollee is supposed to be registered to Cloud
     */
    public EnrolleeConf(DeviceConfig devConf, WiFiConfig wifiConf, boolean cloudable)
    {
        mDevConf = devConf;
        mWiFiConf = wifiConf;
        mCloudable = cloudable;
    }

    /**
     * Get device configuration
     *
     * @return DeviceConfig
     *          device name, language, and country
     *
     * @see DeviceConfig
     */
    public DeviceConfig getDeviceConfig()
    {
        return mDevConf;
    }

    /**
     * Get WiFi configuration
     *
     * @return WiFiConfig
     *          supported WiFi modes and frequency
     *
     * @see WiFiConfig
     */
    public WiFiConfig getWiFiConfig()
    {
        return mWiFiConf;
    }

    /**
     * Get a preference of cloud registration of Enrollee
     *
     * @return true if Enrollee is supposed to be registered to Cloud
     */
    public boolean isCloudable()
    {
        return mCloudable;
    }
}



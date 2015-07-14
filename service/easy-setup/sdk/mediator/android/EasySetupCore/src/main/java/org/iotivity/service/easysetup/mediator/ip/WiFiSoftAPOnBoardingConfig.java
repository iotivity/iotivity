/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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
package org.iotivity.service.easysetup.mediator.ip;

import android.net.wifi.WifiConfiguration;
import org.iotivity.service.easysetup.mediator.common.OnBoardingConfig;

/*
* WiFiSoftAPOnBoardingConfig is a sub class for handling WiFi SoftAP transport configurations.
* WiFiSoftAPOnBoardingConfig will be used during the OnBoarding phase to provide the
* WiFi Soft AP transport details for OnBoarding Enrollee's
* The base class OnBoardingConfig provides the connecitivity type information which is needed
* for dynamic casting of subclasses at runtime.
*/
public class WiFiSoftAPOnBoardingConfig extends OnBoardingConfig {
    private WifiConfiguration netConfig;

    public WifiConfiguration getNetConfig() {
        return netConfig;
    }

    public void setNetConfig(WifiConfiguration netConfig) {
        this.netConfig = netConfig;
    }
}

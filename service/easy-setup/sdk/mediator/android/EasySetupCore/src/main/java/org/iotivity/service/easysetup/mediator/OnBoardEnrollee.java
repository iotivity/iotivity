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
package org.iotivity.service.easysetup.mediator;

import android.content.Context;

import org.iotivity.base.OcConnectivityType;
import org.iotivity.service.easysetup.mediator.common.OnBoardingConfig;
import org.iotivity.service.easysetup.mediator.ip.WiFiSoftAPManager;
import org.iotivity.service.easysetup.mediator.ip.WiFiSoftAPOnBoardingConfig;

public class OnBoardEnrollee {
    WiFiSoftAPManager wifiSoftAPManager;
    IOnBoardingStatus deviceScanListener;
    OcConnectivityType selectedConnectivityType;

    /**
     * Constructor for OnBoardEnrollee.
     */
    public OnBoardEnrollee(Context context, OcConnectivityType connectivityType) {
        if(connectivityType == OcConnectivityType.IPV4) {
            wifiSoftAPManager = new WiFiSoftAPManager(context);
        }
        this.selectedConnectivityType = connectivityType;
    }

    public void registerOnBoardingStatusHandler(
            IOnBoardingStatus deviceScanListener) {
        this.deviceScanListener = deviceScanListener;
    }

    public void startDeviceScan(final int reachableTimeout) {
        if(selectedConnectivityType == OcConnectivityType.IPV4) {
            wifiSoftAPManager.getClientList(this.deviceScanListener, reachableTimeout);
        }
    }

    public void enableNetwork(OnBoardingConfig transportConfig, boolean enabled) {
        if(selectedConnectivityType == OcConnectivityType.IPV4) {
            wifiSoftAPManager.setWifiApEnabled(
                    ((WiFiSoftAPOnBoardingConfig)transportConfig).getNetConfig(),
                    true);
        }
    }

    public void disableWiFiAP() {
        if(selectedConnectivityType == OcConnectivityType.IPV4) {
            wifiSoftAPManager.setWifiApEnabled(null, false);
        }
    }
}

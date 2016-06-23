/******************************************************************
*
* Copyright 2016 Samsung Electronics All Rights Reserved.
*
*
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
******************************************************************/

package org.iotivity.service.phy.test.helper;


import org.iotivity.service.easysetup.core.IOnBoardingStatus;
import org.iotivity.service.easysetup.core.ip.WiFiSoftAPManager;
import org.iotivity.service.phy.test.helper.PHYWiFiOnBoardingConfigHelper;

import android.content.Context;
import android.net.wifi.WifiConfiguration;

public class PHYWiFiSoftAPManagerHelper {
	
	WiFiSoftAPManager wiFiSoftAPManager;
	IOnBoardingStatus finishListener;
	private static final int  reachableTimeout = 10;

	public PHYWiFiSoftAPManagerHelper(Context context) {
		wiFiSoftAPManager = new WiFiSoftAPManager(context);
	}

	public boolean setWifiApEnabled(WifiConfiguration wifiConfig, boolean enabled) {
		return wiFiSoftAPManager.setWifiApEnabled(getWifiConfig(), enabled);

	}
	
	 public synchronized void getClientList(){
		 wiFiSoftAPManager.getClientList(finishListener, reachableTimeout);
	 }

	public static WifiConfiguration getWifiConfig() {
		WifiConfiguration wifiConfig = new WifiConfiguration();
		wifiConfig.SSID = PHYWiFiOnBoardingConfigHelper.SS_ID;
		wifiConfig.preSharedKey = PHYWiFiOnBoardingConfigHelper.SHARED_KEY;
		wifiConfig.allowedAuthAlgorithms
				.set(PHYWiFiOnBoardingConfigHelper.AUTH_ALGO_LBV_VALUE);
		wifiConfig.allowedKeyManagement.set(PHYWiFiOnBoardingConfigHelper.KMS_RV_VALUE);
		return wifiConfig;
	}

}

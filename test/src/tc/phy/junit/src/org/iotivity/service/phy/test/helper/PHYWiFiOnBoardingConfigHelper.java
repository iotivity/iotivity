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

import java.util.ArrayList;

import org.iotivity.service.easysetup.mediator.OnBoardingConfig;
import org.iotivity.service.easysetup.mediator.WiFiOnBoardingConfig;

import android.net.wifi.WifiConfiguration;

public class PHYWiFiOnBoardingConfigHelper {

	WiFiOnBoardingConfig mWiFiOnBoardingConfig;
	public static final String SS_ID = "EasySetup123";
	public static final String SHARED_KEY = "EasySetup123";
        public static final int AUTH_ALGO_LBV_VALUE = WifiConfiguration.AuthAlgorithm.OPEN;
	public static final int AUTH_ALGO_RV_VALUE = WifiConfiguration.AuthAlgorithm.SHARED;
	public static final int AUTH_ALGO_UBV_VALUE = WifiConfiguration.AuthAlgorithm.LEAP;
	public static final int AUTH_ALGO_LOBV_VALUE = -1;
	public static final int AUTH_ALGO_UOBV_VALUE = 3;
	public static final int KMS_RV_VALUE = WifiConfiguration.KeyMgmt.WPA_PSK;
	public static final int KMS_UBV_VALUE = 4;
	public static final int KMS_LBV_VALUE = 0;
	public static final int KMS_LOBV_VALUE = -1;
	public static final int KMS_UOBV_VALUE = 5;
	
	public static final int AUTH_ALGO = WifiConfiguration.AuthAlgorithm.OPEN;
	public static final int KMS = WifiConfiguration.KeyMgmt.WPA_PSK;

	public PHYWiFiOnBoardingConfigHelper() {
		mWiFiOnBoardingConfig = new WiFiOnBoardingConfig();
	}

	public Object getConfig() {
		return mWiFiOnBoardingConfig.getConfig();
	}

	public WiFiOnBoardingConfig setOnBoardingWifiConfig() {
		setSSId(SS_ID);
		setSharedKey(SHARED_KEY);
		setAuthAlgo(AUTH_ALGO);
		setKms(KMS);
		setSecured(false);
		return mWiFiOnBoardingConfig;
	}
	
	public WiFiOnBoardingConfig setSecuredOnBoardingWifiConfig() {
		setSSId(SS_ID);
		setSharedKey(SHARED_KEY);
		setAuthAlgo(AUTH_ALGO);
		setKms(KMS);
		setSecured(true);
		return mWiFiOnBoardingConfig;
	}

        public WiFiOnBoardingConfig getOnBoardingWifiConfig() {
		setSSId(SS_ID);
		setSharedKey(SHARED_KEY);
		setAuthAlgo(AUTH_ALGO_LBV_VALUE);
		setKms(KMS_RV_VALUE );
		return mWiFiOnBoardingConfig;
	}

	public void setSSId(String ssid) {
		mWiFiOnBoardingConfig.setSSId(ssid);
	}

	public void setSharedKey(String sharedKey) {
		mWiFiOnBoardingConfig.setSharedKey(sharedKey);
	}

	public void setAuthAlgo(int aurthAlgo) {
		mWiFiOnBoardingConfig.setAuthAlgo(aurthAlgo);
	}

	public void setKms(int kms) {
		mWiFiOnBoardingConfig.setKms(kms);
	}
	
	public void setSecured(boolean isSecured) {
		mWiFiOnBoardingConfig.setSecured(isSecured);
	}
	
	public boolean isSecured() {
	    return mWiFiOnBoardingConfig.isSecured();

	}
	
	public OnBoardingConfig.ConnType getConnType() {
		return mWiFiOnBoardingConfig.getConnType();
	}
	
	public static ArrayList<String> getConnTypes(){
		ArrayList<String> connTypes = new ArrayList<String>();
		connTypes.add(OnBoardingConfig.ConnType.values()[0].toString());
		connTypes.add(OnBoardingConfig.ConnType.values()[1].toString());
		return connTypes;	
	}
	
	public static String  getConnValueOf(String name){
		return OnBoardingConfig.ConnType.valueOf(name).toString();
	}
	
}

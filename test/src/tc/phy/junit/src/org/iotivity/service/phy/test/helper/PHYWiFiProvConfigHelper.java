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

import org.iotivity.service.easysetup.mediator.ProvisioningConfig;
import org.iotivity.service.easysetup.mediator.WiFiProvConfig;

public class PHYWiFiProvConfigHelper {

	WiFiProvConfig mWiFiProvConfig;
	public static final String SS_ID = "hub2.4G";
	public static final String PASSWORD = "11112222";

	public PHYWiFiProvConfigHelper() {
		mWiFiProvConfig = new WiFiProvConfig(SS_ID, PASSWORD);
	}
	
	public WiFiProvConfig getWiFiProvConfig(){
		mWiFiProvConfig.setSecured(false);
		return mWiFiProvConfig;
	}
	
	public WiFiProvConfig getSecuredWiFiProvConfig(){
		mWiFiProvConfig.setSecured(true);
		return mWiFiProvConfig;
	}

	public Object getConfig() {
		return mWiFiProvConfig.getConfig();
	}

	public String getSsId() {
		return mWiFiProvConfig.getSsId();
	}

	public String getPassword() {
		return mWiFiProvConfig.getPassword();
	}

        public void setSecured(boolean isSecured) {
		mWiFiProvConfig.setSecured(isSecured);
	}

        public boolean isSecured() {
	    return mWiFiProvConfig.isSecured();
	}

	public ProvisioningConfig.ConnType getConnType() {
		return mWiFiProvConfig.getConnType();
	}
	
	public static ArrayList<String> getConnTypes(){
		ArrayList<String> connTypes = new ArrayList<String>();
		connTypes.add(ProvisioningConfig.ConnType.values()[0].toString());
		connTypes.add(ProvisioningConfig.ConnType.values()[1].toString());
		return connTypes;	
	}
	
	public static String  getConnValueOf(String name){
		return ProvisioningConfig.ConnType.valueOf(name).toString();
	}

}

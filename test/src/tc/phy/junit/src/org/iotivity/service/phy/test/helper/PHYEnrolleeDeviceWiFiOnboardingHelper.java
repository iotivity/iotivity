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

import org.iotivity.service.easysetup.mediator.EnrolleeDevice;
import org.iotivity.service.easysetup.mediator.EnrolleeDevice;
import org.iotivity.service.easysetup.mediator.EnrolleeSetupError;
import org.iotivity.service.easysetup.mediator.EnrolleeState;
import org.iotivity.service.easysetup.mediator.OnBoardingConnection;

import android.content.Context;

public class PHYEnrolleeDeviceWiFiOnboardingHelper {

	PHYEnrolleeDeviceFactoryHelper phyEnrolleeDeviceFactoryHelper;
	PHYWiFiProvConfigHelper phyWiFiProvConfigHelper;
	PHYWiFiOnBoardingConfigHelper phyWiFiOnBoardingConfigHelper;
	EnrolleeDevice enrolleeDevice;

	public PHYEnrolleeDeviceWiFiOnboardingHelper(Context context) {
		phyEnrolleeDeviceFactoryHelper = new PHYEnrolleeDeviceFactoryHelper(
				context);
		phyWiFiProvConfigHelper = new PHYWiFiProvConfigHelper();
		phyWiFiOnBoardingConfigHelper = new PHYWiFiOnBoardingConfigHelper();
	}

	public EnrolleeDevice getEnrolleeDevice() {
		EnrolleeDevice enrolleeDevice = phyEnrolleeDeviceFactoryHelper
				.newEnrolleeDevice(phyWiFiProvConfigHelper.getWiFiProvConfig(),
						phyWiFiOnBoardingConfigHelper.setOnBoardingWifiConfig());
		return enrolleeDevice;
	}

	public boolean isSetupSuccessful() {
		return getEnrolleeDevice().isSetupSuccessful();
	}

	public EnrolleeState getState() {
			return getEnrolleeDevice().getState();
	}

	public EnrolleeSetupError getError() {
			return getEnrolleeDevice().getError();
	}
	
	public OnBoardingConnection getConnection() {
		return getEnrolleeDevice().getConnection();
        }

}

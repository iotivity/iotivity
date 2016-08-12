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
import org.iotivity.service.easysetup.mediator.OnBoardingConfig;
import org.iotivity.service.easysetup.mediator.ProvisioningConfig;
import org.iotivity.service.easysetup.mediator.EnrolleeDeviceFactory;

import android.content.Context;

public class PHYEnrolleeDeviceFactoryHelper {

	private static EnrolleeDeviceFactory enrolleeDeviceFactory;
        public static final OnBoardingConfig NULL_ONBOARDING_CONFIGURATION = null;
	public static final ProvisioningConfig NULL_PROVISIONING_CONFIGURATION = null;

	public PHYEnrolleeDeviceFactoryHelper(Context context) {
		enrolleeDeviceFactory = EnrolleeDeviceFactory.newInstance(context);
	}

	public EnrolleeDeviceFactory getNewDeviceFactory() {
		return enrolleeDeviceFactory;
	}

	public EnrolleeDevice newEnrolleeDevice(ProvisioningConfig provConfig,
			OnBoardingConfig onboardingConfig) {
		return enrolleeDeviceFactory.newEnrolleeDevice(provConfig,
				onboardingConfig);
	}

}

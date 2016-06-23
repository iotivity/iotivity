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

package org.iotivity.service.phy.test.stc;

import java.io.IOException;

import org.iotivity.service.easysetup.mediator.EnrolleeDevice;
import org.iotivity.service.easysetup.mediator.EnrolleeDeviceFactory;
import org.iotivity.service.easysetup.mediator.EnrolleeDeviceWiFiOnboarding;
import org.iotivity.service.easysetup.mediator.WiFiOnBoardingConfig;
import org.iotivity.service.easysetup.mediator.WiFiProvConfig;
import org.iotivity.service.phy.test.helper.PHYWiFiOnBoardingConfigHelper;

import static org.iotivity.service.phy.test.helper.PHYWiFiProvConfigHelper.*;

import android.test.AndroidTestCase;

public class PHYEnrolleeDeviceFactoryTest extends AndroidTestCase {

	private EnrolleeDeviceFactory mFactory;

	@Override
	protected void setUp() throws Exception {
		super.setUp();
		mFactory = EnrolleeDeviceFactory.newInstance(getContext());
		assertTrue(mFactory != null);
	}

	@Override
	protected void tearDown() throws Exception {
		super.tearDown();
		mFactory = null;
	}

	/**
	 * @throws none
	 * @since 2016-03-02
	 * @see public void setSSId(String ssid)
	 * @see public void setSharedKey(String sharedKey)
	 * @see public void setAuthAlgo(int aurthAlgo)
	 * @see public void setKms(int kms)
	 * @see public static EnrolleeDeviceFactory newInstance(Context context)
	 * @objective test EnrolleDeviceFactory can create enrollee device as per
	 *            the given configuration
	 * @target public EnrolleeDevice newEnrolleeDevice(OnBoardingConfig
	 *         onboardingConfig, ProvisioningConfig provConfig)
	 * @test_data EasySetup123 WifiConfiguration.AuthAlgorithm.OPEN
	 *            WifiConfiguration.KeyMgmt.WPA_PSK hub2.4G 11112222
	 * @pre_condition 1. Create EnrolleeDeviceFactory instance 2. Create On
	 *                boarding configuration 3. Create provisioning
	 *                configuration 4. Create enrolling device 5. Check if the
	 *                the device is created 6. Check if the the correct device
	 *                is created as per the given configuration
	 * @post_condition none
	 * @expected Enrollee device will be created with WiFiOnboarding
	 */
	public void testNewEnrolleeDeviceWithWiFiOnboarding_P() {

		PHYWiFiOnBoardingConfigHelper phyWiFiOnBoardingConfigHelper = new PHYWiFiOnBoardingConfigHelper();
		WiFiOnBoardingConfig mWiFiOnBoardingConfig = phyWiFiOnBoardingConfigHelper
				.setOnBoardingWifiConfig();
		WiFiProvConfig mWiFiProvConfig = new WiFiProvConfig(SS_ID, PASSWORD);
		EnrolleeDevice device = mFactory.newEnrolleeDevice(mWiFiProvConfig,
				mWiFiOnBoardingConfig);
		assertTrue(device != null);
		assertTrue((device instanceof EnrolleeDeviceWiFiOnboarding));

	}

}

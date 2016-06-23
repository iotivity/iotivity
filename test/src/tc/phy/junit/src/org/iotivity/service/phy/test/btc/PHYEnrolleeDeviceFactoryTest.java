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

package org.iotivity.service.phy.test.btc;

import org.iotivity.service.easysetup.mediator.EnrolleeDevice;
import org.iotivity.service.easysetup.mediator.EnrolleeDeviceFactory;
import org.iotivity.service.phy.test.helper.PHYEnrolleeDeviceFactoryHelper;
import org.iotivity.service.phy.test.helper.PHYWiFiOnBoardingConfigHelper;
import org.iotivity.service.phy.test.helper.PHYWiFiProvConfigHelper;

import android.content.Context;
import android.test.AndroidTestCase;

public class PHYEnrolleeDeviceFactoryTest extends AndroidTestCase {

	private PHYEnrolleeDeviceFactoryHelper phyEnrolleeDeviceFactoryHelper;
	private PHYWiFiOnBoardingConfigHelper phyWiFiOnBoardingConfigHelper;
	PHYWiFiProvConfigHelper phyWiFiProvConfigHelper;
	private static final Context NULL_CONTEXT = null;

	protected void setUp() throws Exception {
		super.setUp();
		phyEnrolleeDeviceFactoryHelper = new PHYEnrolleeDeviceFactoryHelper(
				getContext());
		phyWiFiOnBoardingConfigHelper = new PHYWiFiOnBoardingConfigHelper();
		phyWiFiProvConfigHelper = new PHYWiFiProvConfigHelper();
	}

	protected void tearDown() throws Exception {
		super.tearDown();
	}

	/**
	 * @since 2015-09-18
	 * @see public void setSSId(String ssid)
	 * @see public void setSharedKey(String sharedKey)
	 * @see public void setAuthAlgo(int aurthAlgo)
	 * @see public void setKms(int kms)
	 * @objective test newEnrolleeDevice API positively
	 * @target public EnrolleeDevice newEnrolleeDevice(OnBoardingConfig
	 *         onboardingConfig, ProvisioningConfig provConfig)
	 * @test_data onBoardingConfig, provisioningConfig
	 * @pre_condition Create a enrolleeDeviceFactory instance
	 * @pre_condition set OnBoarding Wifi Configuration
	 * @pre_condition set WifiProvisioning Configuration
	 * @procedure Call newEnrolleeDevice
	 * @post_condition none
	 * @expected create & returns instance of Enrollee device of supported
	 *           configuration
	 */
	public void testPHYNewEnrolleeDevice_RSV_SRC_P() {
		assertTrue(phyEnrolleeDeviceFactoryHelper.newEnrolleeDevice(
				phyWiFiProvConfigHelper.getWiFiProvConfig(),
				phyWiFiOnBoardingConfigHelper.getOnBoardingWifiConfig()) instanceof EnrolleeDevice);
	}

	/**
	 * @since 2015-09-18
	 * @see none
	 * @objective test newEnrolleeDevice API with null OnBoardingConfig and
	 *            ProvisioningConfig
	 * @target public EnrolleeDevice newEnrolleeDevice(OnBoardingConfig
	 *         onboardingConfig, ProvisioningConfig provConfig)
	 * @test_data null onBoardingConfig, null provisioningConfig
	 * @pre_condition Create a enrolleeDeviceFactory instance
	 * @pre_condition set OnBoarding Wifi Configuration
	 * @pre_condition set WifiProvisioning Configuration
	 * @procedure Call newEnrolleeDevice
	 * @post_condition none
	 * @expected IllegalArgumentException will be thrown with specified error
	 *           message
	 */
	public void testPHYNewEnrolleeDevice_NV_ETC_ECRC_N() {

		try {
			phyEnrolleeDeviceFactoryHelper
					.newEnrolleeDevice(
                                                        PHYEnrolleeDeviceFactoryHelper.NULL_PROVISIONING_CONFIGURATION,
							PHYEnrolleeDeviceFactoryHelper.NULL_ONBOARDING_CONFIGURATION
							);
			assertFalse(
					"newEnrolleeDevice should not be called with null parameter",
					true);
		} catch (IllegalArgumentException ex) {
			assertEquals("provConfig/onboardingConfig is NULL",
					ex.getMessage());
		}
	}

	/**
	 * @since 2015-09-18
	 * @see none
	 * @objective test newInstance API positively
	 * @target public EnrolleeDeviceFactory newInstance(Context context)
	 * @test_data android context
	 * @procedure Call newEnrolleeDevice
	 * @post_condition none
	 * @expected creates and returns instance of EnrolleeDeviceFactory
	 */
	public void testPHYNewInstance_RSV_SRC_P() {
		assertTrue(EnrolleeDeviceFactory.newInstance(getContext()) instanceof EnrolleeDeviceFactory);
	}

	/**
	 * @since 2015-09-18
	 * @see none
	 * @objective test newInstance API with null context
	 * @target public static EnrolleDEviceFactory newInstance(Context context)
	 * @test_data null context
	 * @procedure Call newEnrolleeDevice with null context
	 * @post_condition none
	 * @expected newInstance API will be called without exception
	 */
	public void testPHYNewInstance_NV_N() {
		try {
			EnrolleeDeviceFactory.newInstance(NULL_CONTEXT);
		} catch (Exception ex) {
			fail("Fail to create new instance of EnrolleeDeviceFactory");
		}
	}
}

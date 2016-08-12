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

import org.iotivity.service.easysetup.mediator.EnrolleeSetupError;
import org.iotivity.service.easysetup.mediator.EnrolleeState;
import org.iotivity.service.easysetup.mediator.OnBoardingConnection;
import org.iotivity.service.phy.test.helper.PHYEnrolleeDeviceWiFiOnboardingHelper;

import android.test.AndroidTestCase;

public class PHYEnrolleeDeviceWiFiOnboardingTest extends AndroidTestCase {

	PHYEnrolleeDeviceWiFiOnboardingHelper phyEnrolleeDeviceWiFiOnboardingHelper;

	protected void setUp() throws Exception {
		super.setUp();
		phyEnrolleeDeviceWiFiOnboardingHelper = new PHYEnrolleeDeviceWiFiOnboardingHelper(
				getContext());
	}

	protected void tearDown() throws Exception {
		super.tearDown();
	}

	/**
	 * @since 2015-09-20
	 * @see none
	 * @objective test isSetupSuccessful API positively
	 * @target boolean isSetupSuccessful()
	 * @test_data none
	 * @pre_condition Create a enrolleeDeviceFactory instance
	 * @pre_condition Perform OnBoardingWifiConfig
	 * @pre_condition Perform ProvisioningConfig
	 * @procedure Call isSetupSuccessful()
	 * @post_condition none
	 * @expected true if successful or false
	 */
	public void testPHYIsSetupSuccessful_SRC_P() {
		assertEquals(false,
				phyEnrolleeDeviceWiFiOnboardingHelper.isSetupSuccessful());
	}

	/**
	 * @since 2015-09-20
	 * @see none
	 * @objective test getState API positively
	 * @target EnrolleeState getState()
	 * @test_data OnBoardingConfig ProvisioningConfig
	 * @pre_condition Create a enrolleeDeviceFactory instance
	 * @pre_condition Prform OnBoardingWifiConfig
	 * @pre_condition Prform ProvisioningConfig
	 * @procedure Call getState()
	 * @post_condition none
	 * @expected Get enrollee state
	 */
	public void testPHYGetState_SRC_P() {
		assertTrue(phyEnrolleeDeviceWiFiOnboardingHelper.getState() instanceof EnrolleeState);
	}

	/**
	 * @since 2015-09-20
	 * @see none
	 * @objective test getError API positively
	 * @target EasySetupError getError()
	 * @test_data none
	 * @pre_condition Create a enrolleeDeviceFactory instance
	 * @pre_condition Prform OnBoardingWifiConfig
	 * @pre_condition Prform ProvisioningConfig
	 * @procedure Call getError()
	 * @post_condition none
	 * @expected True EnrolleeSetupError object
	 */
	public void testPHYGetError_SRC_P() {
		assertNotNull(phyEnrolleeDeviceWiFiOnboardingHelper.getError() instanceof EnrolleeSetupError);
	}

	/**
	 * @since 2015-09-20
	 * @see none
	 * @objective test getConnection API positively
	 * @target public OnBoardingConnection getConnection()
	 * @test_data none
	 * @pre_condition Create a enrolleeDeviceFactory instance
	 * @pre_condition Prform OnBoardingWifiConfig
	 * @pre_condition Prform ProvisioningConfig
	 * @procedure Call getConnection()
	 * @post_condition none
	 * @expected returns the OnBoardingConnection object depending on the
	 *           connection type
	 */
	public void testPHYGetConnection_SRC_P() {
		assertNotNull(phyEnrolleeDeviceWiFiOnboardingHelper.getConnection() instanceof OnBoardingConnection);
	}
}

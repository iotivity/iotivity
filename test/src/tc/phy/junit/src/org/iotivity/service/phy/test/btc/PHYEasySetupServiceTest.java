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

import java.io.IOException;

import org.iotivity.service.easysetup.mediator.EasySetupService;
import org.iotivity.service.easysetup.mediator.ESException;
import org.iotivity.service.easysetup.mediator.EasySetupStatus;
import org.iotivity.service.easysetup.mediator.EnrolleeDevice;
import org.iotivity.service.phy.test.helper.PHYEasySetupServiceHelper;
import org.iotivity.service.phy.test.helper.PHYEnrolleeDeviceWiFiOnboardingHelper;

import android.test.AndroidTestCase;

public class PHYEasySetupServiceTest extends AndroidTestCase {

	PHYEasySetupServiceHelper phyEasySetupServiceHelper;

	protected void setUp() throws Exception {
		super.setUp();
		phyEasySetupServiceHelper = new PHYEasySetupServiceHelper(getContext());
	}

	protected void tearDown() throws Exception {
		super.tearDown();
	}

	/**
	 * @since 2015-09-20
	 * @see none
	 * @objective test getInstance API positively
	 * @target public static EasySetupService getInstance(Context context,
	 *         EasySetupCallback callback)
	 * @test_data context,EasySetupStatus instance
	 * @pre_condition none
	 * @procedure Call getInstance()
	 * @post_condition none
	 * @expected a singleton instance of Easy setup service and initialize the
	 *           service
	 */
	public void testPHYGetInstance_SRC_RSV_P() {
		assertTrue(EasySetupService.getInstance(getContext(),
				new EasySetupStatus() {
					@Override
					public void onProgress(EnrolleeDevice arg0) {

					}

					@Override
					public void onFinished(EnrolleeDevice arg0) {

					}
				}) instanceof EasySetupService);
	}

	/**
	 * @since 2015-09-20
	 * @see none
	 * @objective test getInstance API will null EasySetupStatus instance
	 * @target public static EasySetupService getInstance(Context context,
	 *         EasySetupCallback callback)
	 * @test_data context, null
	 * @pre_condition none
	 * @procedure Call getInstance()
	 * @post_condition none
	 * @expected getInstance will be called without exception
	 */
	public void testPHYGetInstanceWithNullEasySetupStatus_NV_N() {
		try {
			EasySetupService.getInstance(getContext(), null);
		} catch (Exception ex) {
			fail("Fail to call getInstance() without exception");
		}
	}

	/**
	 * @since 2015-09-20
	 * @see none
	 * @objective test getInstance API with null context
	 * @target public static EasySetupService getInstance(Context context,
	 *         EasySetupCallback callback)
	 * @test_data null, EasySetupStatus instance
	 * @pre_condition none
	 * @procedure Call getInstance()
	 * @post_condition none
	 * @expected a singleton instance of Easy setup service and initialize the
	 *           service
	 */
	public void testPHYGetInstanceWithNullContext_NV_N() {

		try {
			EasySetupService.getInstance(null, new EasySetupStatus() {

				@Override
				public void onProgress(EnrolleeDevice arg0) {

				}

				@Override
				public void onFinished(EnrolleeDevice arg0) {

				}
			});
		} catch (Exception ex) {
			fail("Fail to call getInstance() without exception");
		}
	}

	/**
	 * @since 2015-09-20
	 * @see public EnrolleeDevice newEnrolleeDevice(OnboardingConfig
	 *      onboardingConfig, ProvisioningConfig provConfig)
	 * @see public void setSSId(String ssid)
	 * @see public void setSharedKey(String sharedKey)
	 * @see public void setAuthAlgo(int aurthAlgo)
	 * @see public void setKms(int kms)
	 * @objective test startSetup API positively
	 * @target public void startSetup(EnrolleeDevice enrolledevice) throws
	 *         IOException
	 * @test_data enrollee device
	 * @pre_condition Create a enrollee device
	 * @procedure Call startSetup()
	 * @post_condition none
	 * @expected Call startSetup() without exception
	 */
	public void testPHYStartSetup_RSV_P() {
		PHYEnrolleeDeviceWiFiOnboardingHelper phyEnrolleeDeviceWiFiOnboardingHelper = new PHYEnrolleeDeviceWiFiOnboardingHelper(
				getContext());
		EasySetupService easySetupService = EasySetupService.getInstance(
				getContext(), new EasySetupStatus() {

					@Override
					public void onProgress(EnrolleeDevice arg0) {
					}

					@Override
					public void onFinished(EnrolleeDevice arg0) {

					}
				});
		try {
			easySetupService.startSetup(phyEnrolleeDeviceWiFiOnboardingHelper
					.getEnrolleeDevice());
		} catch (Exception ex) {
			fail(ex.getMessage());
		}
	}

	/**
	 * @since 2015-09-20
	 * @see none
	 * @objective test stopSetup API positively
	 * @target public void stopSetup(EnrolleeDevice enrolledevice)
	 * @test_data context
	 * @pre_condition Create a enrollee device
	 * @procedure Call stopSetup()
	 * @post_condition none
	 * @expected Call stopSetup() without exception
	 */
	public void testPHYStopSetup_RSV_P() {
		try {
			phyEasySetupServiceHelper.stopSetup(getContext());
		} catch (Exception ex) {
			fail("Fail to call stopSetup without exception");
		}
	}

	/**
	 * @since 2015-09-20
	 * @see none
	 * @objective test startSetup API with null enrollee device
	 * @target public void startSetup(EnrolleeDevice enrolledevice) throws
	 *         IOException
	 * @test_data null enrolle device
	 * @pre_condition Create a enrollee device
	 * @procedure Call startSetup()
	 * @post_condition none
	 * @expected Call startSetup() without exception
	 */
	public void testPHYStartSetup_NV_ETC_N() {
		try {
			phyEasySetupServiceHelper
					.startSetup(PHYEasySetupServiceHelper.NULL_ENROLLEE_DEVICE);
		} catch (IOException ex) {
			assertTrue(true);
		} catch (ESException ex) {
			assertTrue(true);
		}
	}

	/**
	 * @since 2015-09-20
	 * @see none
	 * @objective test stopSetup API with null enrolle device
	 * @target public void stopSetup(EnrolleeDevice enrolledevice)
	 * @test_data null enrolle device
	 * @pre_condition Create a enrollee device
	 * @procedure Call stopSetup()
	 * @post_condition none
	 * @expected Call stopSetup() without exception
	 */
	public void testPHYStopSetup_NV_ETC_N() {
		try {
			phyEasySetupServiceHelper.stopSetup(null);
		} catch (ESException ex) {
			assertTrue(true);
		}
	}

	/**
	 * @since 2015-09-20
	 * @see none
	 * @objective test finish API positively
	 * @target public void finish
	 * @test_data none
	 * @pre_condition Create a enrollee device
	 * @procedure Call finish()
	 * @post_condition none
	 * @expected Call startSetup() without exception
	 */
	public void testPHYfinish_RV_P() {
		EasySetupService easySetupService = EasySetupService.getInstance(
				getContext(), new EasySetupStatus() {
					@Override
					public void onProgress(EnrolleeDevice arg0) {
					}

					@Override
					public void onFinished(EnrolleeDevice arg0) {

					}
				});
		try {
			easySetupService.finish();
		} catch (Exception ex) {
			fail(ex.getMessage());
		}
	}
}

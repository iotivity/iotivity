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

import org.iotivity.service.easysetup.mediator.EasySetupService;
import org.iotivity.service.easysetup.mediator.ESException;
import org.iotivity.service.easysetup.mediator.EasySetupStatus;
import org.iotivity.service.easysetup.mediator.EnrolleeDevice;
import org.iotivity.service.easysetup.mediator.EnrolleeSetupError;
import org.iotivity.service.phy.test.helper.PHYEasySetupProcessHelper;
import static org.iotivity.service.phy.test.helper.PHYEasySetupProcessHelper.*;
import org.iotivity.service.phy.test.helper.Utility;

import android.test.AndroidTestCase;

public class PHYEnrolleeSetupErrorTest extends AndroidTestCase {
	EasySetupService mService;

	protected void setUp() throws Exception {
		super.setUp();
		IS_ONFINISHED_CALLED = false;
		IS_ONPROGRESS_CALLED = false;
	}

	protected void tearDown() throws Exception {
		super.tearDown();
		if (mService != null) {
			mService.finish();
			mService = null;
		}
		IS_ONFINISHED_CALLED = false;
		IS_ONPROGRESS_CALLED = false;
	}

	/**
	 * @throws IOException
	 * @since 2016-03-01
	 * @see public void setSSId(String ssid)
	 * @see public void setSharedKey(String sharedKey)
	 * @see public void setAuthAlgo(int aurthAlgo)
	 * @see public void setKms(int kms)
	 * @see public static EnrolleeDeviceFactory newInstance(Context context)
	 * @see public EnrolleeDevice newEnrolleeDevice(OnBoardingConfig
	 *      onboardingConfig, ProvisioningConfig provConfig)
	 * @see public static EasySetupService getInstance(EasySetupCallback
	 *      callback)
	 * @see public void finish()
	 * @objective test if easy setup process send error if error occured during
	 *            setup process
	 * @target public void startSetup(EnrolleeDevice enrolledevice) throws
	 *         IOException
	 * @test_data EasySetup123 WifiConfiguration.AuthAlgorithm.OPEN
	 *            WifiConfiguration.KeyMgmt.WPA_PSK hub2.4G 11112222
	 * @pre_condition none
	 * @procedure 1. Create On boarding configuration 2. Create provisioning
	 *            configuration 3. Create enrolling device factory instance 4.
	 *            Create enrolling device 5. Create Easy Setup service and
	 *            implement callback method 6. Start setup 7. Check Enrollee
	 *            setup state
	 * @post_condition finish setup
	 * @expected Fail easy setup process with error
	 */
	public void testEnrolleeSetupErrorWifiOnboarding_CCC_P() {
		final Object lock = new Object();
		EnrolleeDevice enrolleeDevice = PHYEasySetupProcessHelper
				.getEnrolleeDevice(getContext());

		mService = EasySetupService.getInstance(getContext(),
				new EasySetupStatus() {

					@Override
					public void onFinished(EnrolleeDevice enrolleeDevice) {
						Utility.toNotify(lock);
						IS_ONFINISHED_CALLED = true;
						EnrolleeSetupError error = enrolleeDevice.getError();
						assertNotNull(error);
					}

					@Override
					public void onProgress(EnrolleeDevice enrolleeDevice) {
						IS_ONPROGRESS_CALLED = true;
						EnrolleeSetupError error = enrolleeDevice.getError();
						assertNotNull(error);
					}

				});

		try {
			mService.startSetup(enrolleeDevice);
		} catch (IOException e) {
			assertTrue(e.getMessage(), false);
		} catch (ESException e) {
			assertTrue(e.getMessage(), false);
		}
		try {
			Utility.toWait(lock);
			assertTrue("onFinished callback not called", IS_ONFINISHED_CALLED);
			assertTrue("onProgress callback not called", IS_ONPROGRESS_CALLED);
		} catch (Exception e) {
			e.printStackTrace();
		}

	}
}

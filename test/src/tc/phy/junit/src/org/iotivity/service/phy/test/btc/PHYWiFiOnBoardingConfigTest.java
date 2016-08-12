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

import junit.framework.TestCase;

import org.iotivity.service.easysetup.mediator.OnBoardingConfig;
import org.iotivity.service.phy.test.helper.PHYWiFiOnBoardingConfigHelper;
import android.net.wifi.WifiConfiguration;
import static org.iotivity.service.phy.test.helper.PHYArgument.*;

public class PHYWiFiOnBoardingConfigTest extends TestCase {

	PHYWiFiOnBoardingConfigHelper phyWiFiOnBoardingConfigHelper;

	protected void setUp() throws Exception {
		super.setUp();
		phyWiFiOnBoardingConfigHelper = new PHYWiFiOnBoardingConfigHelper();

	}

	protected void tearDown() throws Exception {
		super.tearDown();
	}

	/**
	 * @since 2015-09-16
	 * @see none
	 * @objective Test setSSId positively
	 * @target public void setSSId(String ssid)
	 * @test_data none
	 * @pre_condition none
	 * @procedure Call setSSId
	 * @post_condition none
	 * @expected setSSId will be called without exception
	 */
	public void testPHYsetSSId_FSV_P() {
		try {
			phyWiFiOnBoardingConfigHelper
					.setSSId(PHYWiFiOnBoardingConfigHelper.SS_ID);
		} catch (Exception ex) {
			fail("Fail to call setSSId without exception");
		}

	}

	/**
	 * @since 2015-09-16
	 * @see none
	 * @objective Test setSSId negatively using empty string value
	 * @target public void setSSId(String ssid)
	 * @test_data none
	 * @pre_condition none
	 * @procedure Call setSSId
	 * @post_condition none
	 * @expected setSSId will be called without exception
	 */

	public void testPHYsetSSId_ESV_N() {
		try {
			phyWiFiOnBoardingConfigHelper.setSSId(EMPTY_STRING_VALUE);
		} catch (Exception ex) {
			fail("Fail to call setSSId without exception");
		}

	}

	/**
	 * @since 2015-09-16
	 * @see none
	 * @objective Test setSSId negatively
	 * @target public void setSSId(String ssid)
	 * @test_data null
	 * @pre_condition none
	 * @procedure Call setSSId
	 * @post_condition none
	 * @expected setSSId will be called without exception
	 */

	public void testPHYsetSSId_NV_N() {
		try {
			phyWiFiOnBoardingConfigHelper.setSSId(NULL_VALUE);
		} catch (Exception ex) {
			fail("Fail to call setSSId without exception");
		}

	}

	/**
	 * @since 2015-09-16
	 * @see none
	 * @objective Test setSharedKey API positively
	 * @target public void setSharedKey(String sharedKey)
	 * @test_data none
	 * @pre_condition none
	 * @procedure Call setSharedKey API
	 * @post_condition none
	 * @expected setSharedKey will be called without exception
	 */
	public void testPHYsetSharedKey_FSV_P() {
		try {
			phyWiFiOnBoardingConfigHelper
					.setSharedKey(PHYWiFiOnBoardingConfigHelper.SHARED_KEY);
		} catch (Exception ex) {
			fail("Fail to call setSharedKey without exception");
		}
	}

	/**
	 * @since 2015-09-16
	 * @see none
	 * @objective Test setSharedKey negatively using empty string value
	 * @target public void setSharedKey(String sharedKey)
	 * @test_data none
	 * @pre_condition none
	 * @procedure Call setSharedKey
	 * @post_condition none
	 * @expected setSharedKey will not be called without exception
	 */
	public void testPHYsetSharedKey_ESV_N() {
		try {
			phyWiFiOnBoardingConfigHelper.setSharedKey(EMPTY_STRING_VALUE);
		} catch (Exception ex) {
			fail("Fail to call setSSId without exception");
		}

	}

	/**
	 * @since 2015-09-16
	 * @see none
	 * @objective Test setSharedKey API negatively
	 * @target public void setSharedKey(String sharedKey)
	 * @test_data null
	 * @pre_condition none
	 * @procedure Call setSharedKey API
	 * @post_condition none
	 * @expected setSharedKey will be called without exception
	 */

	public void testPHYsetSharedKey_NV_N() {
		try {
			phyWiFiOnBoardingConfigHelper.setSharedKey(null);
		} catch (Exception ex) {
			fail("Fail to call setSharedKey without exception");
		}
	}

	/**
	 * @since 2015-09-16
	 * @see none
	 * @objective Test setAuthAlgo API positively
	 * @target public void setAuthAlgo(int aurthAlgo)
	 * @test_data none
	 * @pre_condition none
	 * @procedure Call setAuthAlgo()
	 * @post_condition none
	 * @expected setAuthAlgo will be called without exception
	 */

	public void testPHYsetAuthAlgo_RV_P() {
		try {
			phyWiFiOnBoardingConfigHelper
					.setAuthAlgo(PHYWiFiOnBoardingConfigHelper.AUTH_ALGO_RV_VALUE);
		} catch (Exception ex) {
			fail("Fail to call setAuthAlgo without exception");
		}
	}

	/**
	 * @since 2015-09-16
	 * @see none
	 * @objective Test setAuthAlgo API positively
	 * @target public void setAuthAlgo(int aurthAlgo)
	 * @test_data none
	 * @pre_condition none
	 * @procedure Call setAuthAlgo()
	 * @post_condition none
	 * @expected setAuthAlgo will be called without exception
	 */

	public void testPHYsetAuthAlgo_LBV_P() {
		try {
			phyWiFiOnBoardingConfigHelper
					.setAuthAlgo(PHYWiFiOnBoardingConfigHelper.AUTH_ALGO_LBV_VALUE);
		} catch (Exception ex) {
			fail("Fail to call setAuthAlgo without exception");
		}
	}

	/**
	 * @since 2015-09-16
	 * @see none
	 * @objective Test setAuthAlgo API positively
	 * @target public void setAuthAlgo(int aurthAlgo)
	 * @test_data none
	 * @pre_condition none
	 * @procedure Call setAuthAlgo()
	 * @post_condition none
	 * @expected setAuthAlgo will be called without exception
	 */

	public void testPHYsetAuthAlgo_LOBV_N() {
		try {
			phyWiFiOnBoardingConfigHelper
					.setAuthAlgo(PHYWiFiOnBoardingConfigHelper.AUTH_ALGO_LOBV_VALUE);
			fail("setAuthAlgo should throws exception");
		} catch (Exception ex) {
			assertTrue(true);
		}
	}

	/**
	 * @since 2015-09-16
	 * @see none
	 * @objective Test setAuthAlgo API positively
	 * @target public void setAuthAlgo(int aurthAlgo)
	 * @test_data none
	 * @pre_condition none
	 * @procedure Call setAuthAlgo()
	 * @post_condition none
	 * @expected setAuthAlgo will be called without exception
	 */

	public void testPHYsetAuthAlgo_UBV_P() {
		try {
			phyWiFiOnBoardingConfigHelper
					.setAuthAlgo(PHYWiFiOnBoardingConfigHelper.AUTH_ALGO_UBV_VALUE);
		} catch (Exception ex) {
			fail("Fail to call setAuthAlgo without exception");
		}
	}

	/**
	 * @since 2015-09-16
	 * @see none
	 * @objective Test setAuthAlgo API positively
	 * @target public void setAuthAlgo(int aurthAlgo)
	 * @test_data none
	 * @pre_condition none
	 * @procedure Call setAuthAlgo()
	 * @post_condition none
	 * @expected setAuthAlgo will be called without exception
	 */

	public void testPHYsetAuthAlgo_UOBV_N() {
		try {
			phyWiFiOnBoardingConfigHelper
					.setAuthAlgo(PHYWiFiOnBoardingConfigHelper.AUTH_ALGO_UOBV_VALUE);
		} catch (Exception ex) {
			fail("Fail to call setAuthAlgo without exception");
		}
	}

	/**
	 * @since 2015-09-16
	 * @see none
	 * @objective Test setKms positively
	 * @target public void setKms(int kms)
	 * @test_data none
	 * @pre_condition none
	 * @procedure Call setKms
	 * @post_condition none
	 * @expected setKms will be called without exception
	 */
	public void testPHYSetKms_RV_P() {

		try {
			phyWiFiOnBoardingConfigHelper
					.setKms(PHYWiFiOnBoardingConfigHelper.KMS_RV_VALUE);
		} catch (Exception ex) {
			fail("Fail to call setKms without exception");
		}
	}

	/**
	 * @since 2015-09-16
	 * @see none
	 * @objective Test setKms positively
	 * @target public void setKms(int kms)
	 * @test_data none
	 * @pre_condition none
	 * @procedure Call setKms
	 * @post_condition none
	 * @expected setKms will be called without exception
	 */
	public void testPHYSetKms_LBV_P() {

		try {
			phyWiFiOnBoardingConfigHelper
					.setKms(PHYWiFiOnBoardingConfigHelper.KMS_LBV_VALUE);
		} catch (Exception ex) {
			fail("Fail to call setKms without exception");
		}
	}

	/**
	 * @since 2015-09-16
	 * @see none
	 * @objective Test setKms positively
	 * @target public void setKms(int kms)
	 * @test_data none
	 * @pre_condition none
	 * @procedure Call setKms
	 * @post_condition none
	 * @expected setKms will be called without exception
	 */
	public void testPHYSetKms_UBV_P() {

		try {
			phyWiFiOnBoardingConfigHelper
					.setKms(PHYWiFiOnBoardingConfigHelper.KMS_UBV_VALUE);
		} catch (Exception ex) {
			fail("Fail to call setKms without exception");
		}
	}

	/**
	 * @since 2015-09-16
	 * @see none
	 * @objective Test setKms positively
	 * @target public void setKms(int kms)
	 * @test_data none
	 * @pre_condition none
	 * @procedure Call setKms
	 * @post_condition none
	 * @expected setKms will be called without exception
	 */
	public void testPHYSetKms_LOBV_N() {
		try {
			phyWiFiOnBoardingConfigHelper
					.setKms(PHYWiFiOnBoardingConfigHelper.KMS_LOBV_VALUE);
			fail("setKms should throws exception");
		} catch (Exception ex) {
			assertTrue(true);
		}
	}

	/**
	 * @since 2015-09-16
	 * @see none
	 * @objective Test setKms positively
	 * @target public void setKms(int kms)
	 * @test_data none
	 * @pre_condition none
	 * @procedure Call setKms
	 * @post_condition none
	 * @expected setKms will be called without exception
	 */
	public void testPHYSetKms_UOBV_N() {

		try {
			phyWiFiOnBoardingConfigHelper
					.setKms(PHYWiFiOnBoardingConfigHelper.KMS_UOBV_VALUE);
		} catch (Exception ex) {
			fail("Fail to call setKms without exception");
		}
	}

	/**
	 * @since 2015-09-16
	 * @see none
	 * @objective Test getConnType API positively
	 * @target public OnBoardingConfig.ConnType getConnType
	 * @test_data none
	 * @pre_condition none
	 * @procedure Call getConnType API
	 * @post_condition none
	 * @expected Connectivity type of target network
	 */
	public void testPHYGetConnType_SRC_P() {
		assertEquals(OnBoardingConfig.ConnType.WiFi,
				phyWiFiOnBoardingConfigHelper.getConnType());
	}

	/**
	 * @since 2016-02-24
	 * @see none
	 * @objective Test getConfig API positively
	 * @target public Object getConfig()
	 * @test_data none
	 * @pre_condition none
	 * @procedure Call getConfig API
	 * @post_condition none
	 * @expected instance object of configuration according to connectivity type
	 *           of target network
	 */
	public void testPHYGetConfig_SRC_P() {
		assertNotNull("Fail to get WiFiOnBoardingConfiguration",
				phyWiFiOnBoardingConfigHelper.getConfig());
	}

	/**
	 * @since 2016-02-24
	 * @see none
	 * @objective Test setSecured API positively
	 * @target public void setSecured(boolean isSecured)
	 * @test_data true false
	 * @pre_condition none
	 * @procedure Call setSecured API
	 * @post_condition none
	 * @expected setSecured will be called without exception
	 */
	public void testPHYSetSecured_SRC_P() {
		try {
			phyWiFiOnBoardingConfigHelper.setSecured(true);
			phyWiFiOnBoardingConfigHelper.setSecured(false);
		} catch (Exception ex) {
			fail("Fail to call setSSId without exception");
		}
	}

        /**
	 * @throws none
	 * @since 2016-02-29
	 * @see none
	 * @objective call isSecured() API Positively
	 * @target void isSEcured()
	 * @test_data true
	 * @pre_condition none
	 * @procedure 1. Create WiFi On boarding configuration
	 *            2. Check if set security successfully.
	 * @post_condition none
	 * @expected call isSecured() API successfully
	 */
	public void testIsSecured_SRC_P() {
		assertFalse("Fail to call isSecure() ",
				phyWiFiOnBoardingConfigHelper.isSecured());
	}
}

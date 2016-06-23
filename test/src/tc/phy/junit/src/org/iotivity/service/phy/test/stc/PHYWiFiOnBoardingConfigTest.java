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

import org.iotivity.service.easysetup.mediator.OnBoardingConfig;
import org.iotivity.service.phy.test.helper.PHYWiFiOnBoardingConfigHelper;

import android.net.wifi.WifiConfiguration;
import android.test.AndroidTestCase;

public class PHYWiFiOnBoardingConfigTest extends AndroidTestCase {

	PHYWiFiOnBoardingConfigHelper phyWiFiOnBoardingConfigHelper;

	protected void setUp() throws Exception {
		super.setUp();
		phyWiFiOnBoardingConfigHelper = new PHYWiFiOnBoardingConfigHelper();

	}

	protected void tearDown() throws Exception {
		super.tearDown();
	}

	/**
	 * @throws none
	 * @since 2016-02-29
	 * @see none
	 * @objective test if WiFi OnBoarding can be configured
	 * @target public void setSSId(String ssid)
	 * @target public void setSharedKey(String sharedKey)
	 * @target public void setAuthAlgo(int aurthAlgo)
	 * @target public void setKms(int kms)
	 * @target public Object getConfig()
	 * @target public OnBoardingConfig.ConnType getConnType()
	 * @test_data none
	 * @pre_condition none
	 * @procedure 1. Create WiFi On boarding configuration. 2. Get WiFi On
	 *            boarding configuration. 3. Check if configuration
	 *            successfully. 4. Get connection type and check the type is
	 *            Wifi.
	 * @post_condition none
	 * @expected WiFi On boarding config is same as cretaed
	 */
	public void testSetAndgetOnBoardingWifiConfig_GSRV_P() {
		phyWiFiOnBoardingConfigHelper
				.setSSId(PHYWiFiOnBoardingConfigHelper.SS_ID);
		phyWiFiOnBoardingConfigHelper.setOnBoardingWifiConfig();
		WifiConfiguration config = (WifiConfiguration) phyWiFiOnBoardingConfigHelper
				.getConfig();
		assertEquals("Failed to set SSID", PHYWiFiOnBoardingConfigHelper.SS_ID,
				config.SSID.toString());
		assertEquals("Fail to set Shared Key",
				PHYWiFiOnBoardingConfigHelper.SHARED_KEY,
				config.preSharedKey.toString());
		assertEquals("Invalid Connection type",
				phyWiFiOnBoardingConfigHelper.getConnType(),
				OnBoardingConfig.ConnType.WiFi);
	}

	/**
	 * @throws none
	 * @since 2016-02-29
	 * @see none
	 * @objective test if SSId can be set
	 * @target public void setSSId(String ssid)
	 * @target public Object getConfig()
	 * @test_data SS_ID
	 * @pre_condition none
	 * @procedure 1.Set SSId. 2.Get SsId by WiFi Onboarding configuration
	 *            object. 3.check if SSId set successfully.
	 * @post_condition none
	 * @expected SSId is same as set
	 */
	public void testSetSsIdAndGetSsIdByGetConfig_GSRV_P() {
		phyWiFiOnBoardingConfigHelper
				.setSSId(PHYWiFiOnBoardingConfigHelper.SS_ID);
		WifiConfiguration config = (WifiConfiguration) phyWiFiOnBoardingConfigHelper
				.getConfig();
		assertEquals("Failed to set SSID", PHYWiFiOnBoardingConfigHelper.SS_ID,
				config.SSID.toString());
	}

	/**
	 * @throws none
	 * @since 2016-02-29
	 * @see none
	 * @objective test if WiFi provision can be configured
	 * @target void setSecured(boolean isSecured)
	 * @target void isSEcured()
	 * @target public Object getConfig()
	 * @test_data true
	 * @pre_condition none
	 * @procedure 1. Create WiFi On provisioning configuration. 2. Set security.
	 *            3. Check if set security successfully.
	 * @post_condition none
	 * @expected enable secutity successfully
	 */
	public void testSetSecurityTrueAndCheckSecurity_GSRV_P() {
		phyWiFiOnBoardingConfigHelper.setSecured(true);
		assertTrue("Fail to enable security",
				phyWiFiOnBoardingConfigHelper.isSecured());
	}

	/**
	 * @throws none
	 * @since 2016-02-29
	 * @see none
	 * @objective test if WiFi provision can be configured
	 * @target void setSecured(boolean isSecured)
	 * @target void isSEcured()
	 * @target public Object getConfig()
	 * @test_data true
	 * @pre_condition none
	 * @procedure 1. Create WiFi On provisioning configuration. 2. Set security.
	 *            3. Check if security disable
	 * @post_condition none
	 * @expected Disable security successfully
	 */
	public void testSetSecurityFalseAndCheckSecurity_GSRV_P() {
		phyWiFiOnBoardingConfigHelper.setSecured(false);
		assertFalse("Fail to disable security",
				phyWiFiOnBoardingConfigHelper.isSecured());
	}

}

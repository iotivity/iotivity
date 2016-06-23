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

import org.iotivity.service.easysetup.mediator.ProvisioningConfig;
import org.iotivity.service.easysetup.mediator.WiFiProvConfig;
import org.iotivity.service.phy.test.helper.PHYWiFiOnBoardingConfigHelper;
import org.iotivity.service.phy.test.helper.PHYWiFiProvConfigHelper;

import android.test.AndroidTestCase;

public class PHYWiFiProvConfigTest extends AndroidTestCase {
	PHYWiFiOnBoardingConfigHelper phyWiFiOnBoardingConfigHelper;
	WiFiProvConfig mWiFiProvConfig;

	protected void setUp() throws Exception {
		super.setUp();
		mWiFiProvConfig = new WiFiProvConfig(PHYWiFiProvConfigHelper.SS_ID,
				PHYWiFiProvConfigHelper.PASSWORD);
	}

	protected void tearDown() throws Exception {
		super.tearDown();
	}

	/**
	 * @throws none
	 * @since 2016-02-29
	 * @see none
	 * @objective test if WiFi OnBoarding can be configured
	 * @target WiFiProvConfig(String ssid, String pass)
	 * @target public String getSsId()
	 * @target public String getPassword()
	 * @target public Object getConfig()
	 * @target public OnBoardingConfig.ConnType getConnType()
	 * @test_data none
	 * @pre_condition none
	 * @procedure 1. Create WiFi provisioning configuration. 2. Get SsID 3. Get
	 *            password 4. Check if SsId and password match to previously set
	 *            SssId and password 5. Check if connection type is WiFi
	 * @post_condition none
	 * @expected WiFi provisioning config is same as cretaed
	 */
	public void testSetAndGetWifiProvConfig_GSRV_P() {

		assertEquals("Invalid SS ID", PHYWiFiProvConfigHelper.SS_ID,
				mWiFiProvConfig.getSsId());
		assertEquals("Fail to get passward", PHYWiFiProvConfigHelper.PASSWORD,
				mWiFiProvConfig.getPassword());
		assertEquals("Invalid Connection type", mWiFiProvConfig.getConnType(),
				ProvisioningConfig.ConnType.WiFi);

	}

	/**
	 * @throws none
	 * @since 2016-02-29
	 * @see none
	 * @objective test if WiFi provision can be configured
	 * @target WiFiProvConfig(String ssid, String pass)
	 * @target public Object getConfig()
	 * @test_data none
	 * @pre_condition none
	 * @procedure 1. Create WiFi On provisioning configuration. 2. Get WiFi
	 *            provisioning configuration. 3. Check if configuration is done
	 *            successfully.
	 * @post_condition none
	 * @expected WiFi On boarding config is same as cretaed
	 */
	public void testGetWiFiProvConfigByGetConfigObject_GSRV_P() {
		WiFiProvConfig wiFiProvConfig = (WiFiProvConfig) mWiFiProvConfig
				.getConfig();
		assertEquals("Fail to get SsId", PHYWiFiProvConfigHelper.SS_ID,
				wiFiProvConfig.getSsId());
		assertEquals("Fail to get SsId", PHYWiFiProvConfigHelper.PASSWORD,
				wiFiProvConfig.getPassword());
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
	 * @procedure 1.Create WiFi On provisioning configuration. 2.Set security.
	 *            3.Check if security is set successfully.
	 * @post_condition none
	 * @expected enable secutity successfully
	 */
	public void testSetSecurityTrueAndCheckSecurity_GSRV_P() {
		mWiFiProvConfig.setSecured(true);
		assertTrue("Fail to enable security", mWiFiProvConfig.isSecured());
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
	 *            3. Check if security is disabled
	 * @post_condition none
	 * @expected Disable security successfully
	 */
	public void testSetSecurityFalseAndCheckSecurity_GSRV_P() {
		mWiFiProvConfig.setSecured(false);
		assertFalse("Fail to disable security", mWiFiProvConfig.isSecured());
	}
}

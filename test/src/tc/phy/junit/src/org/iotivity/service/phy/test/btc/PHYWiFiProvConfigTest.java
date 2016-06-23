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
import org.iotivity.service.easysetup.mediator.ProvisioningConfig;
import org.iotivity.service.phy.test.helper.PHYWiFiOnBoardingConfigHelper;
import org.iotivity.service.phy.test.helper.PHYWiFiProvConfigHelper;

public class PHYWiFiProvConfigTest extends TestCase {

	PHYWiFiProvConfigHelper phyWiFiProvConfigHelper;

	protected void setUp() throws Exception {
		super.setUp();
		phyWiFiProvConfigHelper = new PHYWiFiProvConfigHelper();
	}

	protected void tearDown() throws Exception {
		super.tearDown();
	}

	/**
	 * @since 2016-02-24
	 * @see none
	 * @objective Test getSsId API positively
	 * @target public String getSsId()
	 * @test_data none
	 * @pre_condition initialize WiFiProvConfig with SSId and password
	 * @procedure Call getSsId
	 * @post_condition none
	 * @expected SSID of Target Network
	 */
	public void testPHYGetSSId_SRC_P() {
		assertEquals("Failed to get previously set ssid",
				PHYWiFiProvConfigHelper.SS_ID,
				phyWiFiProvConfigHelper.getSsId());
	}

	/**
	 * @since 2016-02-24
	 * @see none
	 * @objective Test getPassword() API positively
	 * @target public String getPassword()()
	 * @test_data none
	 * @pre_condition initialize WiFiProvConfig with SSId and password
	 * @procedure Call getPassword()
	 * @post_condition none
	 * @expected password of Target Network
	 */
	public void testPHYGetPassword_SRC_P() {
		assertEquals("Failed to get previously set password",
				PHYWiFiProvConfigHelper.PASSWORD,
				phyWiFiProvConfigHelper.getPassword());
	}

	/**
	 * @since 2016-02-24
	 * @see none
	 * @objective test getConnType API positively
	 * @target public ProvisioningConfig.ConnType getConnType
	 * @test_data initialize WiFiProvConfig with SSId and password
	 * @pre_condition none
	 * @procedure Call getConnType API
	 * @post_condition none
	 * @expected Connectivity type of target network
	 */
	public void testPHYGetProvisioningConnType_SRC_P() {
		assertEquals(ProvisioningConfig.ConnType.WiFi,
				phyWiFiProvConfigHelper.getConnType());
	}

	/**
	 * @since 2016-02-24
	 * @see none
	 * @objective test getConfig API positively
	 * @target public Object getConfig()
	 * @test_data none
	 * @pre_condition initialize WiFiProvConfig with SSId and password
	 * @procedure Call getConfig API
	 * @post_condition none
	 * @expected instance object of configuration according to connectivity type
	 *           of target network
	 */
	public void testPHYGetProvisioningConfig_SRC_P() {
		assertNotNull("Fail to get WiFi Provisioning Configuration",
				phyWiFiProvConfigHelper.getConfig());
	}

	/**
	 * @since 2016-02-24
	 * @see none
	 * @objective Test setSecured API positively
	 * @target public void setSecured
	 * @test_data true false
	 * @pre_condition none
	 * @procedure Call setSecured API
	 * @post_condition none
	 * @expected setSecured will be called without exception
	 */
	public void testPHYSetSecured_SRC_P() {
		try {
			phyWiFiProvConfigHelper.setSecured(true);
			phyWiFiProvConfigHelper.setSecured(false);
		} catch (Exception ex) {
			fail("Fail to call setSSId without exception");
		}
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
	 * @procedure 1. Create WiFi On provisioning configuration
	 *            2. Check if security is disabled
	 * @post_condition none
	 * @expected call isSecured() API successfully
	 */
	public void testIsSecured_SRC_P() {
            assertFalse("Fail to call isSecure() ", phyWiFiProvConfigHelper.isSecured());
        }

}

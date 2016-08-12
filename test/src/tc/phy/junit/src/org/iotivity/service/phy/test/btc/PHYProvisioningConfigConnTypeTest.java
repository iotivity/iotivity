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

import org.iotivity.service.phy.test.helper.PHYWiFiOnBoardingConfigHelper;
import org.iotivity.service.phy.test.helper.PHYWiFiProvConfigHelper;

public class PHYProvisioningConfigConnTypeTest extends TestCase {

	protected void setUp() throws Exception {
		super.setUp();
	}

	protected void tearDown() throws Exception {
		super.tearDown();
	}

	/**
	 * @since 2015-09-17
	 * @see none
	 * @objective test getConnType API positively
	 * @target static OnBoardingConfig.getConnType()
	 * @test_data none
	 * @pre_condition none
	 * @procedure Call getConnType API
	 * @post_condition none
	 * @expected Returns an array containing the constants of this enum type
	 *           with specified name
	 */

	public void testGetConnTypes_P() {
		try {
			assertEquals("[WiFi, BLE]", PHYWiFiProvConfigHelper.getConnTypes()
					.toString());
		} catch (Exception ex) {
			fail("Fail to call getConnTypes API without exception");
		}
	}

}

/******************************************************************
*
* Copyright 2015 Samsung Electronics All Rights Reserved.
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

import org.iotivity.service.phy.test.helper.PHYWiFiSoftAPManagerHelper;

import android.test.AndroidTestCase;

public class PHYWiFiSoftAPManagerTest extends AndroidTestCase {

	PHYWiFiSoftAPManagerHelper phyWiFiSoftAPManagerHelper;

	protected void setUp() throws Exception {
		super.setUp();
		phyWiFiSoftAPManagerHelper = new PHYWiFiSoftAPManagerHelper(
				getContext());

	}

	/**
	 * @since 2015-10-27
	 * @see none
	 * @objective Test setWifiApEnabled API positively
	 * @target public boolean setWifiApEnabled(WifiConfiguration wifiConfig,
	 *         boolean enabled)
	 * @test_data wificonfig true
	 * @test_data wificonfig false
	 * @pre_condition none
	 * @procedure Call setWifiApEnabled
	 * @post_condition none
	 * @expected setWifiApEnabled will be called without exception and return
	 *           true
	 */

	public void testSetWifiApEnabled_P() {
		try {
			assertTrue(phyWiFiSoftAPManagerHelper.setWifiApEnabled(
					PHYWiFiSoftAPManagerHelper.getWifiConfig(), true));
			assertTrue(phyWiFiSoftAPManagerHelper.setWifiApEnabled(
					PHYWiFiSoftAPManagerHelper.getWifiConfig(), false));

		} catch (Exception ex) {
			fail("Fail to call setWifiApEnabled without exception");
		}
	}

	/**
	 * @since 2015-10-27
	 * @see none
	 * @objective test setWifiApEnabled API negatively
	 * @target public boolean setWifiApEnabled(WifiConfiguration wifiConfig,
	 *         boolean enabled)
	 * @test_data null true
	 * @test_data null false
	 * @pre_condition none
	 * @procedure Call setWifiApEnabled
	 * @post_condition none
	 * @expected setWifiApEnabled will be called without exception and return
	 *           true
	 */

	public void testSetWifiApEnabled_N() {
		try {
			assertTrue(phyWiFiSoftAPManagerHelper.setWifiApEnabled(null, true));
			assertTrue(phyWiFiSoftAPManagerHelper.setWifiApEnabled(null, false));

		} catch (Exception ex) {
			fail("Fail to call setWifiApEnabled without exception");
		}
	}

	/**
	 * @since 2015-10-27
	 * @see none
	 * @objective Test getClientList API positively
	 * @target public synchronized void getClientList(IOnBoardingStatus
	 *         finishListener, final int reachableTimeout)
	 * @test_data none
	 * @pre_condition none
	 * @procedure Call getClientList
	 * @post_condition none
	 * @expected getClientList will be called without exception
	 */

	public void testGetClientList_P() {
		try {
			phyWiFiSoftAPManagerHelper.getClientList();

		} catch (Exception ex) {
			fail("Fail to call getClientList without exception");
		}
	}
}

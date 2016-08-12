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
import org.iotivity.service.easysetup.mediator.EnrolleeDeviceFactory;
import org.iotivity.service.phy.test.helper.PHYEasySetupServiceHelper;
import org.iotivity.service.phy.test.helper.PHYEnrolleeDeviceFactoryHelper;
//import org.iotivity.service.phy.itc.helper;

import android.test.AndroidTestCase;

public class PHYEasySetupServiceConfigTest extends AndroidTestCase {
	PHYEasySetupServiceHelper phyEasySetupServiceHelper;
	PHYEnrolleeDeviceFactoryHelper phyEnrolleeDeviceFactoryHelper;

	protected void setUp() throws Exception {
		super.setUp();
		phyEasySetupServiceHelper = new PHYEasySetupServiceHelper(getContext());
		phyEnrolleeDeviceFactoryHelper = new PHYEnrolleeDeviceFactoryHelper(
				getContext());
	}

	protected void tearDown() throws Exception {
		super.tearDown();
	}

	/**
	 * @throws IOException
	 * @since 2015-11-02
	 * @see none
	 * @objective test easy setup service creation positively
	 * @target public void startSetup(EnrolleeDevice enrolledevice) throws
	 *         IOException
	 * @test_data none
	 * @pre_condition none
	 * @procedure 1. Create On boarding configuration 2. Create provisioning
	 *            configuration 3. Create enrolling device factory instance 4.
	 *            Create enrolling device 5. Create Easy Setup service
	 * @post_condition none
	 * @expected service creation will be successful
	 */

	public void testCreateSetrvice_P() {
		EasySetupService service = phyEasySetupServiceHelper
				.getInstance(getContext());
		assertNotNull("Fail to create service successfully ", service);
	}

	/**
	 * @throws IOException
	 * @since 2015-11-02
	 * @see none
	 * @objective test easy setup service creation positively
	 * @target public void startSetup(EnrolleeDevice enrolledevice) throws
	 *         IOException
	 * @test_data none
	 * @pre_condition none
	 * @procedure 1. Create On boarding configuration 2. Create provisioning
	 *            configuration 3. Create enrolling device factory instance 4.
	 *            Create enrolling device 5. Create Easy Setup service
	 * @procedure Finish Easy Setup service
	 * @post_condition none
	 * @expected service can be finished which is created
	 */

	public void testFinishService_P() {
		EasySetupService service = phyEasySetupServiceHelper
				.getInstance(getContext());
		try {
			service.finish();
			assertTrue(true);

		} catch (Exception ex) {
			fail("Fail to call finish service without exception");
		}
	}

	/**
	 * 
	 * @throws IOException
	 * @since 2015-11-02
	 * @see none
	 * @objective test easy device factory creation positively
	 * @target public void startSetup(EnrolleeDevice enrolledevice) throws
	 *         IOException
	 * @test_data none
	 * @pre_condition none
	 * @procedure 1. Create On boarding configuration 2. Create provisioning
	 *            configuration 3. Create enrolling device factory instance
	 * @post_condition none
	 * @expected device factory can be created successfully
	 */

	public void testCreateEnrolleeDeviceFacotry_P() {
		EnrolleeDeviceFactory factory = phyEnrolleeDeviceFactoryHelper
				.getNewDeviceFactory();
		assertTrue(factory != null);
	}

}

//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
package src.test.java.oic.ctt.devicecontroller;

import static org.junit.Assert.*;
import oic.ctt.devicecontroller.*;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Rule;
import org.junit.Test;

import oic.ctt.testhelper.ConformanceTestHelper;
import oic.ctt.testhelper.ReportGenerator;
import oic.ctt.devicecontroller.FileUtil;
import oic.ctt.devicecontroller.LinuxProcessObserver;
import oic.ctt.devicecontroller.LogManager;
import oic.ctt.devicecontroller.interfaces.IDeviceController;
import oic.ctt.devicecontroller.interfaces.ILogObserver;

import org.slf4j.Logger;

import oic.ctt.logger.CTLogger;

public class LogManagerTest extends ConformanceTestHelper {

	private static final String DEVICE_UUID0 = "adminDeviceUUID0";

	@Rule
	public ReportGenerator reportGenerator = new ReportGenerator();

	private Logger mlogger = CTLogger.getInstance();
	private LogManager logManager;

	@BeforeClass
	public static void setUpBeforeClass() throws Exception {
	}

	@AfterClass
	public static void tearDownAfterClass() throws Exception {
	}

	@Before
	public void setUp() throws Exception {
		logManager = logManager.getInstance();
	}

	@After
	public void tearDown() throws Exception {
	}

	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2016-01-08
	 * @reviewer
	 * @since 2016-01-08
	 * @see none
	 * @objective Test 'registerDeviceLogger' positively
	 * @target public boolean registerDeviceLogger(String deviceID,
	 *         IDeviceController device, boolean makeFile)
	 * @test_data none
	 * @pre_condition none
	 * @procedure call registerDeviceLogger()
	 * @post_condition none
	 * @expected registerDeviceLogger() will be called without exception
	 */

	@Test
	public void registerDeviceLogger_P() {
		IDeviceController device = null;
		try {
			logManager.registerDeviceLogger(DEVICE_UUID0, device, false);
		} catch (Exception ex) {
			fail("Fail" + ex.getLocalizedMessage());
		}
	}

	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2016-02-02
	 * @reviewer
	 * @since 2016-02-02
	 * @see none
	 * @objective Test 'registerProcessLogger' positively
	 * @target public boolean registerProcessLogger(String deviceID,
	 *         IDeviceController device, String processID, boolean makeFile)
	 * @test_data none
	 * @pre_condition none
	 * @procedure call registerProcessLogger()
	 * @post_condition none
	 * @expected registerProcessLogger() will be called without exception
	 */

	@Test
	public void registerProcessLogger_P() {
		IDeviceController device = null;
		try {
			logManager.registerProcessLogger(DEVICE_UUID0, device, "12345",
					false);
		} catch (Exception ex) {
			fail("Fail" + ex.getLocalizedMessage());
		}
	}

	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2016-02-02
	 * @reviewer
	 * @since 2016-02-02
	 * @see none
	 * @objective Test 'getConsoleProcessLogger' positively
	 * @target public ILogObserver getConsoleProcessLogger(String deviceID,
	 *         String processID)
	 * @test_data none
	 * @pre_condition none
	 * @procedure call getConsoleProcessLogger()
	 * @post_condition none
	 * @expected getConsoleProcessLogger() will be called without exception and
	 *           return null
	 */

	@Test
	public void getConsoleProcessLogger_P() {
		try {
			assertNull("No consoleProcess Logger found",
					logManager.getConsoleProcessLogger(DEVICE_UUID0, "12345"));
		} catch (Exception ex) {
			fail("Fail" + ex.getLocalizedMessage());
		}
	}

	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2016-02-02
	 * @reviewer
	 * @since 2016-02-02
	 * @see none
	 * @objective Test 'getProcessLogger' positively
	 * @target public ILogObserver getProcessLogger(String deviceID, String
	 *         processID)
	 * @test_data none
	 * @pre_condition none
	 * @procedure call getProcessLogger()
	 * @post_condition none
	 * @expected getProcessLogger() will be called without exception
	 */

	@Test
	public void getProcessLogger_P() {
		try {
			assertNull("No Process Logger found",
					logManager.getProcessLogger(DEVICE_UUID0, "12345"));
		} catch (Exception ex) {
			fail("Fail" + ex.getLocalizedMessage());
		}
	}

	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2016-02-02
	 * @reviewer
	 * @since 2016-02-02
	 * @see none
	 * @objective Test 'unregisterProcessLogger' positively
	 * @target public boolean unregisterProcessLogger(String deviceID,
	 *         IDeviceController device, String processID)
	 * @test_data none
	 * @pre_condition none
	 * @procedure call unregisterProcessLogger()
	 * @post_condition none
	 * @expected unregisterProcessLogger() will be called without exception
	 */

	@Test
	public void unregisterProcessLogger_P() {
		IDeviceController device = null;
		try {
			logManager.unregisterProcessLogger(DEVICE_UUID0, device, "12345");
		} catch (Exception ex) {
			fail("Fail" + ex.getLocalizedMessage());
		}
	}

}

//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
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
import oic.ctt.devicecontroller.LinuxProcessManager;
import oic.ctt.devicecontroller.LinuxProcessObserver;
import oic.ctt.devicecontroller.LogManager;
import oic.ctt.devicecontroller.interfaces.IDeviceController;
import oic.ctt.devicecontroller.interfaces.ILogObserver;

import org.slf4j.Logger;

import oic.ctt.logger.CTLogger;

public class LinuxProcessTest extends ConformanceTestHelper {

	private static final String DEVICE_UUID0 = "adminDeviceUUID0";
	private LinuxProcessManager linuxProcessManager;

	@Rule
	public ReportGenerator reportGenerator = new ReportGenerator();
	private LinuxProcess linuxProcess;

	@BeforeClass
	public static void setUpBeforeClass() throws Exception {
	}

	@AfterClass
	public static void tearDownAfterClass() throws Exception {
	}

	@Before
	public void setUp() throws Exception {
		linuxProcess = new LinuxProcess("processName", "cmd");
	}

	@After
	public void tearDown() throws Exception {
	}

	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2016-02-02
	 * @reviewer
	 * @since 2016-02-02
	 * @see none
	 * @objective Test 'isProcessRunning' positively
	 * @target public boolean isProcessRunning()
	 * @test_data none
	 * @pre_condition none
	 * @procedure call isProcessRunning()
	 * @post_condition none
	 * @expected isProcessRunning() will be called without exception and return
	 *           true
	 */

	@Test
	public void isProcessRunning_P() {
		try {
			// assertTrue(linuxProcess.isProcessRunning());
			linuxProcess.isProcessRunning();
		} catch (Exception ex) {
			fail("Fail" + ex.getLocalizedMessage());
		}
	}

	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2016-02-02
	 * @reviewer
	 * @since 2016-02-02
	 * @see none
	 * @objective Test 'inputCommand' positively
	 * @target public boolean inputCommand(String command)
	 * @test_data none
	 * @pre_condition none
	 * @procedure call inputCommand()
	 * @post_condition none
	 * @expected inputCommand() will be called without exception
	 */

	@Test
	public void inputCommand_P() {
		try {
			linuxProcess.inputCommand("command");
		} catch (Exception ex) {
			fail("Fail" + ex.getLocalizedMessage());
		}
	}

	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2016-02-02
	 * @reviewer
	 * @since 2016-02-02
	 * @see none
	 * @objective Test 'inputCommand()' negatively
	 * @target public boolean inputCommand(String command)
	 * @test_data none
	 * @pre_condition none
	 * @procedure call inputCommand() with null
	 * @post_condition none
	 * @expected inputCommand() will be called without exception and return
	 *           faslse
	 */

	@Test
	public void inputCommand_N() {
		try {
			assertFalse(linuxProcess.inputCommand(null));
		} catch (Exception ex) {
			fail("Fail" + ex.getLocalizedMessage());
		}
	}

}

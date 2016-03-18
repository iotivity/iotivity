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
import oic.ctt.devicecontroller.LinuxProcess;
import oic.ctt.devicecontroller.LinuxProcessManager;
import oic.ctt.devicecontroller.LinuxProcessObserver;
import oic.ctt.devicecontroller.LogManager;
import oic.ctt.devicecontroller.interfaces.IDeviceController;
import oic.ctt.devicecontroller.interfaces.ILogObserver;

import org.mockito.Mockito;
import org.mockito.internal.util.reflection.Whitebox;

import org.slf4j.Logger;

import oic.ctt.logger.CTLogger;

public class LinuxProcessManagerTest extends ConformanceTestHelper {

	private static final String DEVICE_UUID0 = "adminDeviceUUID0";
	private Logger logger = CTLogger.getInstance();
	@Rule
	public ReportGenerator reportGenerator = new ReportGenerator();
	private LinuxProcessManager linuxProcessManager;

	@BeforeClass
	public static void setUpBeforeClass() throws Exception {
	}

	@AfterClass
	public static void tearDownAfterClass() throws Exception {
	}

	@Before
	public void setUp() throws Exception {
		linuxProcessManager = new LinuxProcessManager();
		linuxProcessManager.createProcess("processName", "ProcessageCommand");
	}

	@After
	public void tearDown() throws Exception {
	}

	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2016-02-02
	 * @reviewer
	 * @since 2016-02-02
	 * @see none
	 * @objective Test 'createProcess' positively
	 * @target public boolean createProcess(String processName, String
	 *         processCommand()
	 * @test_data none
	 * @pre_condition none
	 * @procedure call createProcess()
	 * @post_condition none
	 * @expected createProcess() will be called without exception and return
	 *           true
	 */

	@Test
	public void createProcess_P() {
		try {
			assertTrue(linuxProcessManager.createProcess("processName",
					"ProcessageCommand"));
		} catch (Exception ex) {
			fail("Fail" + ex.getLocalizedMessage());
		}
	}

	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2016-02-02
	 * @reviewer
	 * @since 2016-02-02
	 * @see none
	 * @objective Test 'removeProcess' positively
	 * @target public boolean removeProcess(String processName()
	 * @test_data none
	 * @pre_condition none
	 * @procedure call removeProcess()
	 * @post_condition none
	 * @expected removeProcess() will be called without exception and return
	 *           true
	 */

	@Test
	public void removeProcess_P() {
		try {
			linuxProcessManager.removeProcess("processName");
		} catch (Exception ex) {
			fail("Fail" + ex.getLocalizedMessage());
		}
	}

	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2016-02-02
	 * @reviewer
	 * @since 2016-02-02
	 * @see none
	 * @objective Test 'removeAllProcess' positively
	 * @target public boolean removeAllProcess()
	 * @test_data none
	 * @pre_condition none
	 * @procedure call removeAllProcess()
	 * @post_condition none
	 * @expected removeAllProcess() will be called without exception and return
	 *           true
	 */

	@Test
	public void removeAllProcess_P() {
		try {
			assertTrue(linuxProcessManager.removeAllProcess());
		} catch (Exception ex) {
			fail("Fail" + ex.getLocalizedMessage());
		}
	}

	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2016-02-02
	 * @reviewer
	 * @since 2016-02-02
	 * @see none
	 * @objective Test 'inputProcessCommand' positively
	 * @target public boolean inputProcessCommand(String processName, String
	 *         command)
	 * @test_data none
	 * @pre_condition none
	 * @procedure call inputProcessCommand()
	 * @post_condition none
	 * @expected inputProcessCommand() will be called without exception
	 * 
	 */

	@Test
	public void inputProcessCommand_P() {

		try {
			linuxProcessManager.inputProcessCommand("processName",
					"ProcessageComman");
		} catch (Exception ex) {
			fail("Fail" + ex.getLocalizedMessage());
		}
	}

	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2016-02-02
	 * @reviewer
	 * @since 2016-02-02
	 * @see none
	 * @objective Test 'inputProcessCommand' negatively
	 * @target public boolean inputProcessCommand(String processName, String
	 *         command)
	 * @test_data none
	 * @pre_condition none
	 * @procedure call inputProcessCommand()
	 * @post_condition none
	 * @expected inputProcessCommand() will be called without exception and
	 *           return false
	 * 
	 */

	@Test
	public void inputProcessCommand_N() {
		try {
			assertFalse(linuxProcessManager.inputProcessCommand(
					"WrongProcessName", "WrongProcessageComman"));
		} catch (Exception ex) {
			fail("Fail" + ex.getLocalizedMessage());
		}
	}

	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2016-02-02
	 * @reviewer
	 * @since 2016-02-02
	 * @see none
	 * @objective Test 'registerOutput' positively
	 * @target public boolean registerOutput(String processName, ILogObserver
	 *         observer)
	 * @test_data none
	 * @pre_condition none
	 * @procedure call registerOutput()
	 * @post_condition none
	 * @expected registerOutput() will be called without exception and return
	 *           true
	 */

	@Test
	public void registerOutput_P() {
		ILogObserver observer = null;// = new ILogObserver();
		try {
			assertTrue(linuxProcessManager.registerOutput("processName",
					observer));
		} catch (Exception ex) {
			fail("Fail" + ex.getLocalizedMessage());
		}
	}

	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2016-02-02
	 * @reviewer
	 * @since 2016-02-02
	 * @see none
	 * @objective Test 'unregisterOutput' positively
	 * @target public boolean unregisterOutput(String processName, ILogObserver
	 *         observer)
	 * @test_data none
	 * @pre_condition none
	 * @procedure call unregisterOutput()
	 * @post_condition none
	 * @expected unregisterOutput() will be called without exception and return
	 *           true
	 */

	@Test
	public void unregisterOutput_P() {
		ILogObserver observer = null;
		try {
			assertTrue(linuxProcessManager.unregisterOutput("processName",
					observer));
		} catch (Exception ex) {
			fail("Fail" + ex.getLocalizedMessage());
		}
	}

	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2016-02-02
	 * @reviewer
	 * @since 2016-02-02
	 * @see none
	 * @objective Test 'unregisterOutput' negatively
	 * @target public boolean unregisterOutput(String processName, ILogObserver
	 *         observer)
	 * @test_data none
	 * @pre_condition none
	 * @procedure call unregisterOutput() with wrong param
	 * @post_condition none
	 * @expected unregisterOutput() will be called without exception and return
	 *           false
	 */

	@Test
	public void unregisterOutput_N() {
		ILogObserver observer = null;
		try {
			assertFalse(linuxProcessManager.unregisterOutput(
					"WrongprocessName", observer));
		} catch (Exception ex) {
			fail("Fail" + ex.getLocalizedMessage());
		}
	}

	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2016-02-02
	 * @reviewer
	 * @since 2016-02-02
	 * @see none
	 * @objective Test 'isProcessRunning' positively
	 * @target public boolean isProcessRunning(String processName)
	 * @test_data none
	 * @pre_condition none
	 * @procedure call isProcessRunning()
	 * @post_condition none
	 * @expected isProcessRunning() will be called without exception
	 * 
	 */

	@Test
	public void isProcessRunning_P() {
		ILogObserver observer = null;// = new ILogObserver();
		try {
			linuxProcessManager.isProcessRunning("processName");
		} catch (Exception ex) {
			fail("Fail" + ex.getLocalizedMessage());
		}
	}

	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2016-02-02
	 * @reviewer
	 * @since 2016-02-02
	 * @see none
	 * @objective Test 'isProcessRunning' negatively
	 * @target public boolean isProcessRunning(String processName)
	 * @test_data none
	 * @pre_condition none
	 * @procedure call isProcessRunning() with wrong parameter
	 * @post_condition none
	 * @expected isProcessRunning() will be called without exception and return
	 *           false
	 */
	@Test
	public void isProcessRunning_N() {
		ILogObserver observer = null;// = new ILogObserver();
		try {
			assertFalse(linuxProcessManager
					.isProcessRunning("WrongprocessName"));
		} catch (Exception ex) {
			fail("Fail" + ex.getLocalizedMessage());
		}
	}

	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2016-02-02
	 * @reviewer
	 * @since 2016-02-02
	 * @see none
	 * @objective Test 'getObserverable' positively
	 * @target public ILogObservable getObserverable(String processName)
	 * @test_data none
	 * @pre_condition none
	 * @procedure call getObserverable()
	 * @post_condition none
	 * @expected getObserverable() will be called without exception
	 * 
	 */

	@Test
	public void getObserverable_P() {
		ILogObserver observer = null;// = new ILogObserver();
		try {
			linuxProcessManager.getObserverable("processName");
		} catch (Exception ex) {
			fail("Fail" + ex.getLocalizedMessage());
		}
	}

	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2016-02-02
	 * @reviewer
	 * @since 2016-02-02
	 * @see none
	 * @objective Test 'getObserverable' negatively
	 * @target public ILogObservable getObserverable(String processName)
	 * @test_data none
	 * @pre_condition none
	 * @procedure call getObserverable() with wrong param
	 * @post_condition none
	 * @expected getObserverable() will be called without exception
	 * 
	 */

	@Test
	public void getObserverable_N() {
		ILogObserver observer = null;// = new ILogObserver();
		try {
			assertNull(linuxProcessManager.getObserverable("wrongprocessName"));
		} catch (Exception ex) {
			fail("Fail" + ex.getLocalizedMessage());
		}
	}
}
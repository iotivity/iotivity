package src.test.java.oic.ctt.devicecontroller;

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

import static org.junit.Assert.*;

import java.net.URL;
import java.util.Scanner;

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

public class FileUtilTest extends ConformanceTestHelper {

	private static final String DEVICE_UUID0 = "adminDeviceUUID0";
	private Logger mlogger = CTLogger.getInstance();

	@Rule
	public ReportGenerator reportGenerator = new ReportGenerator();
	private FileUtil fileUtil;

	@BeforeClass
	public static void setUpBeforeClass() throws Exception {
	}

	@AfterClass
	public static void tearDownAfterClass() throws Exception {
	}

	@Before
	public void setUp() throws Exception {
		fileUtil = new FileUtil();
	}

	@After
	public void tearDown() throws Exception {
	}

	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2016-02-03
	 * @reviewer
	 * @since 2016-02-03
	 * @see none
	 * @objective Test 'IsExistWordInFile' positively
	 * @target public static boolean IsExistWordInFile(String word, String path)
	 * @test_data none
	 * @pre_condition none
	 * @procedure call IsExistWordInFile()
	 * @post_condition none
	 * @expected IsExistWordInFile() will be called without exception and return
	 *           true
	 */

	@Test
	public void IsExistWordInFile_P() {
		String path = System.getProperty("user.dir");
		path = path.concat("/src/test/java/oic/ctt/devicecontroller/test.json");
		try {
			assertTrue(fileUtil.IsExistWordInFile("key", path));
		} catch (Exception ex) {
			fail("Fail" + ex.getLocalizedMessage());
		}
	}

	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2016-02-03
	 * @reviewer
	 * @since 2016-02-03
	 * @see none
	 * @objective Test 'IsExistWordInFile' negatively
	 * @target public static boolean IsExistWordInFile(String word, String path)
	 * @test_data none
	 * @pre_condition none
	 * @procedure call IsExistWordInFile() with wrong path
	 * @post_condition none
	 * @expected IsExistWordInFile() will be called without exception and return
	 *           false
	 */

	@Test
	public void IsExistWordInFile_N() {
		try {
			assertFalse(fileUtil.IsExistWordInFile("key", "path"));
		} catch (Exception ex) {
			fail("Fail" + ex.getLocalizedMessage());
		}
	}

}

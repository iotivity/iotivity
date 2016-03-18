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

package oic.ctt.network.control;

import oic.ctt.logger.CTLogger;
import oic.ctt.network.OICHelper;
import oic.ctt.network.control.ControlMessage;
import oic.ctt.network.control.MessageArgument;
import oic.ctt.network.control.SecurityConstants.CipherSuite;
import oic.ctt.network.control.SecurityConstants.MessageCode;
import oic.ctt.testhelper.ConformanceTestHelper;
import oic.ctt.testhelper.ReportGenerator;
import static org.junit.Assert.*;

import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.ArrayList;

import javax.management.loading.PrivateClassLoader;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Rule;
import org.junit.Test;
import org.omg.CORBA.PRIVATE_MEMBER;
import org.slf4j.Logger;

public class ControlMessageTest extends ConformanceTestHelper {

	@Rule
	public ReportGenerator reportGenerator = new ReportGenerator();

	private ControlMessage controlMessage;
	public MessageArgument messageArgument;
	// int relayPort = 12345;
	// int messageID =12345;
	// String clientDeviceId = "adminDeviceUUID0";
	// String serverDeviceId = "justworksDevUUID";
	// ControlMessage controlMsg;
	private Logger mlogger = CTLogger.getInstance();

	@BeforeClass
	public static void setUpBeforeClass() throws Exception {
	}

	@AfterClass
	public static void tearDownAfterClass() throws Exception {
	}

	@Before
	public void setUp() throws Exception {
		controlMessage = new ControlMessage();
	}

	@After
	public void tearDown() throws Exception {
	}

	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2015-01-08
	 * @reviewer
	 * @since 2015-01-08
	 * @see none
	 * @objective Test 'addArgumentPrivateData' positively
	 * @target public void addArgumentPrivateData
	 * @test_data none
	 * @pre_condition none
	 * @procedure call addArgumentPrivateData(byte[] privateData)
	 * @post_condition none
	 * @expected addArgumentPrivateData() will be called without exception and
	 *           add an private argument properly
	 */

	@Test
	public void addArgumentPrivateData_P() {
		try {
			controlMessage.addArgumentPrivateData(OICHelper
					.hexStringToByteArray("6fa5e39969bc8eb4d76a412f7872267c"));
			assertEquals(OICHelper.bytesToHex(controlMessage.getPrivateData()),
					"6fa5e39969bc8eb4d76a412f7872267c");
		} catch (Exception ex) {
			fail("Fail" + ex.getLocalizedMessage());
		}
	}

	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2015-01-08
	 * @reviewer
	 * @since 2015-01-08
	 * @see none
	 * @objective Test 'addArgumentPrivateData' negatively
	 * @target public void addArgumentPrivateData
	 * @test_data none
	 * @pre_condition none
	 * @procedure call addArgumentPrivateData(byte[] privateData) with null
	 *            argument
	 * @post_condition none
	 * @expected addArgumentPrivateData return true
	 */

	// @Test
	// public void addArgumentPrivateData_N() {
	// try {
	// assertTrue(controlMessage.addArgumentPrivateData(null));// This API is
	// not handle null yet.
	// } catch (Exception ex) {
	// fail("Fail" + ex.getLocalizedMessage());
	// }
	// }

	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2015-01-08
	 * @reviewer
	 * @since 2015-01-08
	 * @see none
	 * @objective Test 'getPrivateData' positively
	 * @target public byte[] getPrivateData
	 * @test_data none
	 * @pre_condition call addArgumentPrivateData() to set a valid value
	 * @procedure call getPrivateData()
	 * @post_condition none
	 * @expected getPrivateData() will be called without exception and return
	 *           expected value.
	 */

	@Test
	public void getPrivateData_P() {
		String byteValue = null;
		try {
			controlMessage.addArgumentPrivateData(OICHelper
					.hexStringToByteArray("6fa5e39969bc8eb4d76a412f7872267c"));
			byteValue = OICHelper.bytesToHex(controlMessage.getPrivateData());
			assertEquals(byteValue, "6fa5e39969bc8eb4d76a412f7872267c");
		} catch (Exception ex) {
			fail("Fail" + ex.getLocalizedMessage());
		}
	}

	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2015-01-08
	 * @reviewer
	 * @since 2015-01-08
	 * @see none
	 * @objective Test 'getPrivateData' negatively
	 * @target public byte[] getPrivateData
	 * @test_data none
	 * @pre_condition none
	 * @procedure call getPrivateData()
	 * @post_condition none
	 * @expected getPrivateData() will be called without exception and return
	 *           null.
	 */

	@Test
	public void getPrivateData_N() {
		try {
			assertNull(controlMessage.getPrivateData());
		} catch (Exception ex) {
			fail("Fail" + ex.getLocalizedMessage());
		}
	}

	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2015-01-11
	 * @reviewer
	 * @since 2015-01-11
	 * @see none
	 * @objective Test 'addArgumentIpAddress' positively
	 * @target public void addArgumentIpAddress
	 * @test_data none
	 * @pre_condition none
	 * @procedure call addArgumentIpAddress(String ipAddress)
	 * @post_condition none
	 * @expected addArgumentIpAddress() will be called without exception
	 */

	@Test
	public void addArgumentIpAddress_P() {
		try {
			controlMessage.addArgumentIpAddress(ConformanceTestHelper.ipv4);
		} catch (Exception ex) {
			fail("Fail" + ex.getLocalizedMessage());
		}
	}

	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2015-01-11
	 * @reviewer
	 * @since 2015-01-11
	 * @see none
	 * @objective Test 'addArgumentPort' positively
	 * @target public void addArgumentPort
	 * @test_data none
	 * @pre_condition none
	 * @procedure call addArgumentPort(int port)
	 * @post_condition none
	 * @expected addArgumentPort() will be called without exception
	 */

	@Test
	public void addArgumentPort_P() {
		try {
			controlMessage.addArgumentPort(ConformanceTestHelper.port);
		} catch (Exception ex) {
			fail("Fail" + ex.getLocalizedMessage());
		}
	}

	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2015-01-11
	 * @reviewer
	 * @since 2015-01-11
	 * @see none
	 * @objective Test 'addArgumentCipherSuite' positively
	 * @target public void addArgumentCipherSuite
	 * @test_data none
	 * @pre_condition none
	 * @procedure call addArgumentCipherSuite(CipherSuite cipherSuite)
	 * @post_condition none
	 * @expected addArgumentCipherSuite() will be called without exception
	 */

	@Test
	public void addArgumentCipherSuite_P() {
		try {
			controlMessage
					.addArgumentCipherSuite(CipherSuite.TLS_PSK_WITH_AES_128_CCM_8);
		} catch (Exception ex) {
			fail("Fail" + ex.getLocalizedMessage());
		}
	}

}

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
import oic.ctt.network.OICClient;
import oic.ctt.network.OICHelper;
import oic.ctt.network.control.ControlMessage;
import oic.ctt.network.control.MessageArgument;
import oic.ctt.network.control.ControlClient;
import oic.ctt.network.control.SecurityConstants.CipherSuite;
import oic.ctt.network.control.SecurityConstants.MessageCode;
import oic.ctt.network.OICResponseData;
import oic.ctt.network.OICClient;
import oic.ctt.network.OICClient.Protocol;
import oic.ctt.network.OICHelper.MessageType;
import oic.ctt.network.OICRequestData.Method;

import oic.ctt.provision.*;

import org.mockito.Mockito;
import org.mockito.internal.util.reflection.Whitebox;

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

import oic.ctt.provision.SecuredDeviceInfo.OTM;

public class ProvisioningClientTest extends ConformanceTestHelper {

	@Rule
	public ReportGenerator reportGenerator = new ReportGenerator();

	boolean useIp6 = false;
	String relayIp = null;
	String dutIp = null;
	String deviceIdEncoded = null;
	String clientDeviceId = "adminDeviceUUID0";
	String serverDeviceId = "serverDeviceUUID0";
	int relayControlPort = 12345;
	int relayDataPort = 1234;
	int dutSecuredPort = 1234;
	String pin = "1234";
	private ProvisioningClient provisioningClient;
	private Logger mlogger = CTLogger.getInstance();
	private SecuredDeviceInfo securedDevice;
	private ControlClient mCtlr;
	private OICClient oicCnt;

	@BeforeClass
	public static void setUpBeforeClass() throws Exception {
	}

	@AfterClass
	public static void tearDownAfterClass() throws Exception {
	}

	@Before
	public void setUp() throws Exception {
		provisioningClient = new ProvisioningClient();
		securedDevice = Mockito.mock(SecuredDeviceInfo.class);
	}

	@After
	public void tearDown() throws Exception {
	}

	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2015-01-12
	 * @reviewer
	 * @since 2015-01-12
	 * @see none
	 * @objective Test 'discoverOwnedDevice' positively
	 * @target public SecuredDeviceInfo discoverOwnedDevice
	 * @test_data none
	 * @pre_condition none
	 * @procedure call discoverOwnedDevice(String filterIp)
	 * @post_condition none
	 * @expected discoverOwnedDevice() will be called without exception
	 */

	@Test
	public void discoverOwnedDevice_P() {
		relayIp = provisioningClient.getCurrentSystemIpAddress(useIp6);
		dutIp = relayIp;
		try {
			provisioningClient.discoverOwnedDevice(dutIp);
		} catch (Exception ex) {
			fail("Fail" + ex.getLocalizedMessage());
		}
	}

	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2015-01-12
	 * @reviewer
	 * @since 2015-01-12
	 * @see none
	 * @objective Test 'discoverUnownedDevice' positively
	 * @target public SecuredDeviceInfo discoverUnownedDevice
	 * @test_data none
	 * @pre_condition none
	 * @procedure call discoverUnownedDevice(String filterIp)
	 * @post_condition none
	 * @expected discoverUnownedDevice() will be called without exception
	 */

	@Test
	public void discoverUnownedDevice_P() {
		relayIp = provisioningClient.getCurrentSystemIpAddress(useIp6);
		dutIp = relayIp;
		try {
			provisioningClient.discoverUnownedDevice(dutIp);
		} catch (Exception ex) {
			fail("Fail" + ex.getLocalizedMessage());
		}
	}

	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2015-01-12
	 * @reviewer
	 * @since 2015-01-12
	 * @see none
	 * @objective Test 'findPrivateData' positively
	 * @target public String findPrivateData
	 * @test_data none
	 * @pre_condition none
	 * @procedure call findPrivateData(String deviceId)
	 * @post_condition none
	 * @expected findPrivateData() will be called without exception
	 */

	@Test
	public void findPrivateData_P() {
		Mockito.when(securedDevice.getDeviceId())
				.thenReturn("adminDeviceUUID0");
		try {
			provisioningClient.findPrivateData(securedDevice.getDeviceId());
		} catch (Exception ex) {
			fail("Fail" + ex.getLocalizedMessage());
		}
	}

	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2015-01-12
	 * @reviewer
	 * @since 2015-01-12
	 * @see none
	 * @objective Test 'findPrivateData' negatively
	 * @target public String findPrivateData
	 * @test_data none
	 * @pre_condition none
	 * @procedure call findPrivateData(null)
	 * @post_condition none
	 * @expected findPrivateData() will return null
	 */

	@Test
	public void findPrivateData_N() {
		try {
			assertNull(provisioningClient.findPrivateData("hkda"));
		} catch (Exception ex) {
			fail("Fail" + ex.getLocalizedMessage());
		}
	}

	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2015-01-12
	 * @reviewer
	 * @since 2015-01-12
	 * @see none
	 * @objective Test 'storePrivateData' positively
	 * @target public void storePrivateData
	 * @test_data none
	 * @pre_condition none
	 * @procedure call storePrivateData(null)
	 * @post_condition none
	 * @expected storePrivateData() will be called without exception
	 */

	@Test
	public void storePrivateData_P() {
		try {
			provisioningClient.storePrivateData(securedDevice.getDeviceId(),
					"privateData");
		} catch (Exception ex) {
			fail("Fail" + ex.getLocalizedMessage());
		}
	}

	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2015-01-12
	 * @reviewer
	 * @since 2015-01-12
	 * @see none
	 * @objective Test 'dtlsConnectWithPSK' positively
	 * @target public boolean dtlsConnectWithPSK
	 * @test_data none
	 * @pre_condition none
	 * @procedure call dtlsConnectWithPSK(String privateData, String
	 *            clientDeviceId, String serverDeviceId, String relayIp, int
	 *            relayControlPort, String dutIp, int dutSecuredPort)
	 * @post_condition none
	 * @expected dtlsConnectWithPSK() will be called without exception
	 */

	@Test
	public void dtlsConnectWithPSK_P() {
		relayIp = provisioningClient.getCurrentSystemIpAddress(useIp6);
		dutIp = relayIp;
		try {
			provisioningClient.dtlsConnectWithPSK("private_data",
					clientDeviceId, serverDeviceId, relayIp, relayControlPort,
					dutIp, dutSecuredPort);
		} catch (Exception ex) {
			fail("Fail" + ex.getLocalizedMessage());
		}
	}

	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2015-01-13
	 * @reviewer
	 * @since 2015-01-13
	 * @see none
	 * @objective Test 'configureDeviceForProvisioning' positively
	 * @target public void configureDeviceForProvisioning
	 * @test_data none
	 * @pre_condition none
	 * @procedure call configureDeviceForProvisioning(OTM otm, String
	 *            clientDeviceId, String serverDeviceId)
	 * @post_condition none
	 * @expected configureDeviceForProvisioning() will be called without
	 *           exception
	 */

	@Test
	public void configureDeviceForProvisioning_P() {
		relayIp = provisioningClient.getCurrentSystemIpAddress(useIp6);
		dutIp = relayIp;
		OTM otm = null; // if private void checkAndConfirmProvisioningStatus()
						// don't check response with null then this TC will be
						// failed.
		try {
			provisioningClient.configureDeviceForProvisioning(OTM.RANDOM_PIN,
					clientDeviceId, serverDeviceId);
		} catch (Exception ex) {
			fail("Fail" + ex.getLocalizedMessage());
		}
	}

	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2015-01-13
	 * @reviewer
	 * @since 2015-01-13
	 * @see none
	 * @objective Test 'provisionWithJustWorks' positively
	 * @target public String provisionWithJustWorks
	 * @test_data none
	 * @pre_condition none
	 * @procedure call provisionWithJustWorks(String clientDeviceId, String
	 *            serverDeviceId, String relayIp, int relayControlPort, String
	 *            dutIp, int dutSecuredPort)
	 * @post_condition none
	 * @expected provisionWithJustWorks() will be called without exception
	 */

	@Test
	public void provisionWithJustWorks_P() {
		relayIp = provisioningClient.getCurrentSystemIpAddress(useIp6);
		ControlMessage response = new ControlMessage();
		response.setMessageCode(MessageCode.DTLS_HANDSHAKE_SUCCESS);
		dutIp = relayIp;
		mCtlr = Mockito.mock(ControlClient.class);
		Whitebox.setInternalState(provisioningClient, "mController", mCtlr);
		Mockito.when(
				mCtlr.initDtlsWithJustWorks(relayIp, relayControlPort, dutIp,
						dutSecuredPort,
						CipherSuite.TLS_ECDH_anon_WITH_AES_128_CBC_SHA_256
								.getValue(), clientDeviceId, serverDeviceId))
				.thenReturn(response); // need to change api by developer to
										// pass this tc
		try {
			provisioningClient.provisionWithJustWorks(clientDeviceId,
					serverDeviceId, relayIp, relayControlPort, dutIp,
					dutSecuredPort);
		} catch (Exception ex) {
			fail("Fail" + ex.getLocalizedMessage());
		}
	}

	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2015-01-13
	 * @reviewer
	 * @since 2015-01-13
	 * @see none
	 * @objective Test 'provisionWithRandomPin' positively
	 * @target public String provisionWithRandomPin
	 * @test_data none
	 * @pre_condition none
	 * @procedure call provisionWithRandomPin(String pin, String clientDeviceId,
	 *            String serverDeviceId, String relayIp, int relayControlPort,
	 *            String dutIp, int dutSecuredPort)
	 * @post_condition none
	 * @expected provisionWithRandomPin() will be called without exception
	 */

	@Test
	public void provisionWithRandomPin_P() {
		relayIp = provisioningClient.getCurrentSystemIpAddress(useIp6);
		ControlMessage response = new ControlMessage();
		response.setMessageCode(MessageCode.DTLS_HANDSHAKE_SUCCESS);
		dutIp = relayIp;
		mCtlr = Mockito.mock(ControlClient.class);
		Whitebox.setInternalState(provisioningClient, "mController", mCtlr);
		Mockito.when(
				mCtlr.initDtlsWithJustWorks(relayIp, relayControlPort, dutIp,
						dutSecuredPort,
						CipherSuite.TLS_ECDH_anon_WITH_AES_128_CBC_SHA_256
								.getValue(), clientDeviceId, serverDeviceId))
				.thenReturn(response); // need to change api by developer to
										// pass this tc
		try {
			provisioningClient.provisionWithRandomPin(pin, clientDeviceId,
					serverDeviceId, relayIp, relayControlPort, dutIp,
					dutSecuredPort);
		} catch (Exception ex) {
			fail("Fail" + ex.getLocalizedMessage());
		}
	}
	
	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2015-01-13
	 * @reviewer
	 * @since 2015-01-13
	 * @see none
	 * @objective Test 'getCurrentSystemIpAddress' positively
	 * @target String getCurrentSystemIpAddress(boolean useIp6)
	 * @test_data none
	 * @pre_condition none
	 * @procedure call getCurrentSystemIpAddress()
	 * @post_condition none
	 * @expected getCurrentSystemIpAddress() will be called without exception
	 */

	@Test
	public void getCurrentSystemIpAddress_P() {
		try {
			provisioningClient.getCurrentSystemIpAddress(useIp6);
		} catch (Exception ex) {
			fail("Fail" + ex.getLocalizedMessage());
		}
	}
	
	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2015-01-13
	 * @reviewer
	 * @since 2015-01-13
	 * @see none
	 * @objective Test 'transferOwnership' positively
	 * @target public SOICResponseData transferOwnership()
	 * @test_data none
	 * @pre_condition none
	 * @procedure call transferOwnership(String clientDeviceId,
            String serverDeviceId, String relayIp, int relayDataPort)
	 * @post_condition none
	 * @expected transferOwnership() will be called without exception
	 */

	@Test
	public void transferOwnership_P() {
		relayIp = provisioningClient.getCurrentSystemIpAddress(useIp6);
		OICResponseData response = new OICResponseData();
		//response.setMessageCode(MessageCode.DTLS_HANDSHAKE_SUCCESS);		
		oicCnt = Mockito.mock(OICClient.class);
		Whitebox.setInternalState(provisioningClient, "mOicClient", oicCnt);
		Mockito.when(
				oicCnt.sendRequest(Protocol.COAP,
		                MessageType.NON, Method.PUT,
		                OICHelper.getRandomMessageIdString(),
		                OICHelper.createTokenString(), relayIp, relayDataPort,
		                "/oic/sec/doxm", "", payload))
				.thenReturn(response); // need to change api by developer to
										// pass this tc
		try {
			provisioningClient.transferOwnership(clientDeviceId, serverDeviceId, relayIp, relayDataPort);
		} catch (Exception ex) {
			fail("Fail" + ex.getLocalizedMessage());
		}
	}
	
	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2015-01-13
	 * @reviewer
	 * @since 2015-01-13
	 * @see none
	 * @objective Test 'terminateDtls' positively
	 * @target public void terminateDtls()
	 * @test_data none
	 * @pre_condition none
	 * @procedure call terminateDtls()
	 * @post_condition none
	 * @expected terminateDtls() will be called without exception
	 */

	@Test
	public void terminateDtls_P() {
		ControlMessage response = new ControlMessage();
		response.setMessageCode(MessageCode.DTLS_HANDSHAKE_SUCCESS);
		mCtlr = Mockito.mock(ControlClient.class);
		Whitebox.setInternalState(provisioningClient, "mController", mCtlr);
		Mockito.when(
				mCtlr.terminateDtls())
				.thenReturn(response); 
		try {
			provisioningClient.terminateDtls();
		} catch (Exception ex) {
			fail("Fail" + ex.getLocalizedMessage());
		}
	}
}
// ******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
//
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

package src.test.java.oic.ctt.network.coap;

import static org.junit.Assert.*;
import oic.ctt.network.coap.OICCoapResource;
import oic.ctt.network.coap.OICCoapServer;
import oic.ctt.testhelper.*;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Rule;
import org.junit.Test;

//import src.main.java.oic.ctt.network.coap.String;

public class OICCoapResourceTest extends ConformanceTestHelper {

	@Rule
	public ReportGenerator reportGenerator = new ReportGenerator();

	private OICCoapResource oICCoapResource;

	@BeforeClass
	public static void setUpBeforeClass() throws Exception {
	}

	@AfterClass
	public static void tearDownAfterClass() throws Exception {
	}

	@Before
	public void setUp() throws Exception {
		oICCoapResource = new OICCoapResource("test");

	}

	@After
	public void tearDown() throws Exception {
	}

	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2015-12-31
	 * @reviewer
	 * @since 2015-12-31
	 * @see none
	 * @objective Test 'hasObserver' positively
	 * @target public void hasObserver
	 * @test_data none
	 * @pre_condition none
	 * @procedure Call hasObserver()
	 * @post_condition none
	 * @expected hasObserver() will be called without exception and return false
	 */

	@Test
	public void hasObserver_P() {
		try {
			assertFalse(oICCoapResource.hasObserver("test"));
		} catch (Exception ex) {
			fail("failed to call OICCoapResource method without exception");
		}
	}

	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2016-01-01
	 * @reviewer
	 * @since 2016-01-01
	 * @see none
	 * @objective Test 'updateResourceRespresentation' positively
	 * @target public void updateResourceRespresentation(String payloadString)
	 * @test_data none
	 * @pre_condition none
	 * @procedure Call updateResourceRespresentation(String payloadString)
	 * @post_condition none
	 * @expected updateResourceRespresentation() will be called without
	 *           exception
	 */

	@Test
	public void updateResourceRespresentation_P() {
		try {
			oICCoapResource
					.updateResourceRespresentation(ConformanceTestHelper.payload);
		} catch (Exception ex) {
			fail("failed to call OICCoapResource method without exception");
		}
	}
	
	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2016-01-01
	 * @reviewer
	 * @since 2016-01-01
	 * @see none
	 * @objective Test 'updateResourceRespresentation' Negatively
	 * @target public void updateResourceRespresentation(String payloadString)
	 * @test_data none
	 * @pre_condition none
	 * @procedure Call updateResourceRespresentation(null)
	 * @post_condition none
	 * @expected updateResourceRespresentation() will throw exception
	 */
	
	@Test
	public void updateResourceRespresentation_N() {
		try {
			oICCoapResource
					.updateResourceRespresentation(null);
		} catch (Exception ex) {
			//fail("failed to call OICCoapResource method without exception");
			assertEquals(null, ex.getMessage());
		}
	}
	
	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2016-01-01
	 * @reviewer
	 * @since 2016-01-01
	 * @see none
	 * @objective Test 'updateResourceRespresentationWithout' positively
	 * @target public void updateResourceRespresentation()
	 * @test_data none
	 * @pre_condition none
	 * @procedure Call updateResourceRespresentation()
	 * @post_condition none
	 * @expected updateResourceRespresentation() will be called without
	 *           exception
	 */

	@Test
	public void updateResourceRespresentationWithoutParam_P() {
		try {
			oICCoapResource
					.updateResourceRespresentation();
		} catch (Exception ex) {
			fail("failed to call OICCoapResource method without exception");
		}
	}
	
	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2016-01-01
	 * @reviewer
	 * @since 2016-01-01
	 * @see none
	 * @objective Test 'updateCompleteResourceRespresentation(String payloadString)' positively
	 * @target public void updateResourceRespresentation()
	 * @test_data none
	 * @pre_condition none
	 * @procedure Call updateCompleteResourceRespresentation(String payloadString)
	 * @post_condition none
	 * @expected updateCompleteResourceRespresentation(String payloadString) will be called without
	 *           exception
	 */

	@Test
	public void updateCompleteResourceRespresentation_P() {
		try {
			oICCoapResource
					.updateCompleteResourceRespresentation(ConformanceTestHelper.payload);
		} catch (Exception ex) {
			fail("failed to call OICCoapResource method without exception");
		}
	}
	
	/**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2016-01-01
	 * @reviewer
	 * @since 2016-01-01
	 * @see none
	 * @objective Test 'updateCompleteResourceRespresentation(String payloadString)' negatively
	 * @target public void updateResourceRespresentation()
	 * @test_data none
	 * @pre_condition none
	 * @procedure Call updateCompleteResourceRespresentation(null)
	 * @post_condition none
	 * @expected updateCompleteResourceRespresentation(null) will throw exception
	 */

	@Test
	public void updateCompleteResourceRespresentation_N() {
		try {
			oICCoapResource
					.updateCompleteResourceRespresentation(null);
		} catch (Exception ex) {
			//fail("failed to call OICCoapResource method without exception");
		}
	}
	

}

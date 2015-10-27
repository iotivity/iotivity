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

package oic.ctt.network.coap.test;

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

public class OICCoapServerTest extends ConformanceTestHelper {

    @Rule
    public ReportGenerator       reportGenerator   = new ReportGenerator();

    private OICCoapServer        oICCoapServer     = null;
    private OICCoapResource      oicCoapResource   = null;
    private final static String  resourceInterface = "oic.if.baseline";
    private final static boolean observable        = true;

    @BeforeClass
    public static void setUpBeforeClass() throws Exception {
    }

    @AfterClass
    public static void tearDownAfterClass() throws Exception {
    }

    @Before
    public void setUp() throws Exception {
        oICCoapServer = new OICCoapServer();
        oicCoapResource = new OICCoapResource(uriPath, resourceType,
                resourceInterface, observable);
    }

    @After
    public void tearDown() throws Exception {
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-13
     * @reviewer
     * @since 2015-08-13
     * @see none
     * @objective Test 'start' positively
     * @target public void start()
     * @test_data none
     * @pre_condition none
     * @procedure Call start()
     * @post_condition none
     * @expected discoverResource() will be called and return a response list of
     *           size 0
     */

    @Test
    public void start_P() {
        try {
            oICCoapServer.start();
        } catch (Exception ex) {
            fail("failed to call start method without exception");
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-17
     * @reviewer
     * @since 2015-08-17
     * @see none
     * @objective Test 'addResource' positively
     * @target public void addResource(OICCoapResource oicCoapResource)
     * @test_data oicCoapResource
     * @pre_condition add OICCoapResource by initializing OICCoapResource object
     * @procedure Call start()
     * @post_condition none
     * @expected addResource() will be called without exception
     */

    @Test
    public void addResource_P() {
        try {
            oICCoapServer.addResource(oicCoapResource);
        } catch (Exception ex) {
            fail("failed to call addResource method without exception");
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-17
     * @reviewer
     * @since 2015-08-17
     * @see none
     * @objective Test 'addResource' negatively
     * @target public void addResource(OICCoapResource oicCoapResource)
     * @test_data null
     * @pre_condition none
     * @procedure Call addResource()
     * @post_condition none
     * @expected addResource() will be called without exception
     */

    @Test
    public void addResource_N() {
        try {
            oICCoapServer.addResource(null);
        } catch (Exception ex) {
            fail("failed to call addResource method without exception");
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-17
     * @reviewer
     * @since 2015-08-17
     * @see none
     * @objective Test 'removeResource' positively
     * @target public void removeResource(String resourceUri)
     * @test_data uri
     * @pre_condition none
     * @procedure Call removeResource()
     * @post_condition none
     * @expected removeResource() will be called without exception
     */

    @Test
    public void removeResource_P() {
        try {
            oICCoapServer.removeResource(uriPath);
        } catch (Exception ex) {
            fail("failed to call addResource method without exception");
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-17
     * @reviewer
     * @since 2015-08-17
     * @see none
     * @objective Test 'removeResource' negatively
     * @target public void removeResource(String resourceUri)
     * @test_data null
     * @pre_condition none
     * @procedure Call removeResource()
     * @post_condition none
     * @expected removeResource() will be called without exception
     */

    @Test
    public void removeResource_N() {
        try {
            oICCoapServer.removeResource(null);
        } catch (Exception ex) {
            fail("failed to call addResource method without exception");
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-17
     * @reviewer
     * @since 2015-08-17
     * @see none
     * @objective Test 'clearDiscoveryRequests' positively
     * @target public void clearDiscoveryRequests()
     * @test_data none
     * @pre_condition none
     * @procedure Call clearDiscoveryRequests()
     * @post_condition none
     * @expected clearDiscoveryRequests() will be called without exception
     */

    @Test
    public void clearDiscoveryRequests_P() {
        try {
            oICCoapServer.clearDiscoveryRequests();
        } catch (Exception ex) {
            fail("failed to call addResource method without exception");
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-17
     * @reviewer
     * @since 2015-08-17
     * @see none
     * @objective Test 'getDiscoveryRequests' positively
     * @target public ArrayList<OICRequestData> getDiscoveryRequests()
     * @test_data none
     * @pre_condition none
     * @procedure Call getDiscoveryRequests()
     * @post_condition none
     * @expected getDiscoveryRequests() will be called without exception
     */

    @Test
    public void getDiscoveryRequests_P() {
        try {
            oICCoapServer.getDiscoveryRequests();
        } catch (Exception ex) {
            fail("failed to call addResource method without exception");
        }
    }

}

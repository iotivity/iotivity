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

package oic.ctt.network.test;

import static org.junit.Assert.*;
import oic.ctt.network.OICHelper;
import oic.ctt.testhelper.*;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Rule;
import org.junit.Test;

public class OICHelperTest extends ConformanceTestHelper {

    @Rule
    public ReportGenerator reportGenerator = new ReportGenerator();

    @BeforeClass
    public static void setUpBeforeClass() throws Exception {
    }

    @AfterClass
    public static void tearDownAfterClass() throws Exception {
    }

    @Before
    public void setUp() throws Exception {
    }

    @After
    public void tearDown() throws Exception {
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-17
     * @reviewer
     * @since 2015-08-17
     * @see none
     * @objective Test 'hexStringToByteArray' positively
     * @target public static byte[] hexStringToByteArray(String s)
     * @test_data 98
     * @pre_condition none
     * @procedure Call hexStringToByteArray()
     * @post_condition none
     * @expected hexStringToByteArray() will be called without exception
     */

    @Test
    public void hexStringToByteArray_P() {
        try {
            assertNotNull(OICHelper.hexStringToByteArray("98"));
        } catch (Exception ex) {
            fail("failed to call addResource method without exception :"
                    + ex.getMessage());
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-17
     * @reviewer
     * @since 2015-08-17
     * @see none
     * @objective Test 'hexStringToByteArray' positively
     * @target public static String bytesToHex(byte[] bytes)
     * @test_data 98
     * @pre_condition none
     * @procedure Call hexStringToByteArray()
     * @post_condition none
     * @expected hexStringToByteArray() will be called without exception
     */

    @Test
    public void bytesToHex_P() {
        try {
            String testString = "[B@17583b9";
            assertNotNull(OICHelper.bytesToHex(testString.getBytes()));
        } catch (Exception ex) {
            fail("failed to call addResource method without exception :"
                    + ex.getMessage());
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-17
     * @reviewer
     * @since 2015-08-17
     * @see none
     * @objective Test 'createToken' positively
     * @target public static byte[] hexStringToByteArray(String s)
     * @test_data
     * @pre_condition none
     * @procedure Call createToken()
     * @post_condition none
     * @expected createToken() will be called without exception
     */

    @Test
    public void createToken_P() {
        try {
            assertNotNull(OICHelper.createToken(2));
        } catch (Exception ex) {
            fail("failed to call createToken method without exception :"
                    + ex.getMessage());
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-17
     * @reviewer
     * @since 2015-08-17
     * @see none
     * @objective Test ' getQueryParts' positively
     * @target public static ArrayList<String> getQueryParts(String query)
     * @test_data query
     * @pre_condition none
     * @procedure Call getQueryParts()
     * @post_condition none
     * @expected getQueryParts() will be called without exception
     */

    @Test
    public void getQueryParts_P() {
        try {
            assertNotNull(OICHelper.getQueryParts(query));
            assertNotNull(OICHelper.getQueryParts("&query"));
        } catch (Exception ex) {
            fail("failed to call getQueryParts method without exception :"
                    + ex.getMessage());
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-17
     * @reviewer
     * @since 2015-08-17
     * @see none
     * @objective Test 'getUriParts' positively
     * @target public static ArrayList<String> getUriParts(String uriPath)
     * @test_data uriPath
     * @pre_condition none
     * @procedure Call getUriParts()
     * @post_condition none
     * @expected getUriParts() will be called without exception
     */

    @Test
    public void getUriParts_P() {
        try {
            assertNotNull(OICHelper.getUriParts(uriPath));
        } catch (Exception ex) {
            fail("failed to call getUriParts method without exception :"
                    + ex.getMessage());
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-17
     * @reviewer
     * @since 2015-08-17
     * @see none
     * @objective Test 'setDefaultOic' positively
     * @target public static void setDefaultOic(String oic)
     * @test_data oic
     * @pre_condition none
     * @procedure Call setDefaultOic()
     * @post_condition none
     * @expected setDefaultOic() will be called without exception
     */

    @Test
    public void setDefaultOic_P() {
        try {
            OICHelper.setDefaultOic("oic");
        } catch (Exception ex) {
            fail("failed to call setDefaultOic method without exception :"
                    + ex.getMessage());
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-17
     * @reviewer
     * @since 2015-08-17
     * @see none
     * @objective Test 'setDefaultUri' positively
     * @target public static void setDefaultUri(String oic)
     * @test_data uriPath
     * @pre_condition none
     * @procedure Call setDefaultUri()
     * @post_condition none
     * @expected setDefaultUri() will be called without exception
     */

    @Test
    public void setDefaultUri_P() {
        try {
            OICHelper.setDefaultUri(uriPath);
        } catch (Exception ex) {
            fail("failed to call setDefaultUri method without exception :"
                    + ex.getMessage());
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-17
     * @reviewer
     * @since 2015-08-25
     * @see none
     * @objective Test 'makeUriPath' positively
     * @target public static String makeUriPath(String prefix, String ip, int
     *         port)
     * @test_data coap:// ip port
     * @pre_condition none
     * @procedure Call makeUriPath()
     * @post_condition none
     * @expected makeUriPath() will be called without exception
     */

    @Test
    public void makeUriPath_P() {
        try {
            assertEquals("Failed to make URI path", "coap://224.0.1.187:5683",
                    OICHelper.makeUriPath("coap://", ipv4, port));
            OICHelper.makeUriPath(null, null, -1);

            System.out.println(OICHelper.makeUriPath(null, null, -1));
        } catch (Exception ex) {
            fail("failed to call makeUriPath method without exception :"
                    + ex.getMessage());
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-17
     * @reviewer
     * @since 2015-08-25
     * @see none
     * @objective Test 'makeUriPath' negatively
     * @target public static String makeUriPath(String prefix, String ip, int
     *         port)
     * @test_data null null -1
     * @pre_condition none
     * @procedure Call makeUriPath()
     * @post_condition none
     * @expected makeUriPath() will be called without exception
     */

    @Test
    public void makeUriPath_N() {
        try {
            OICHelper.makeUriPath(null, null, -1);
        } catch (Exception ex) {
            fail("failed to call makeUriPath method without exception :"
                    + ex.getMessage());
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-17
     * @reviewer
     * @since 2015-08-25
     * @see none
     * @objective Test 'validateIpAddress' positively
     * @target public static boolean validateIpAddress(final String ip)
     * @test_data ip
     * @pre_condition none
     * @procedure Call validateIpAddress()
     * @post_condition none
     * @expected validateIpAddress() will be called without exception
     */

    @Test
    public void validateIpAddress_P() {
        try {
            assertEquals(true, OICHelper.validateIpAddress(ipv4.toString()));
        } catch (Exception ex) {
            fail("failed to call validateIpAddress method without exception :"
                    + ex.getMessage());
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-17
     * @reviewer
     * @since 2015-08-25
     * @see none
     * @objective Test 'validateIpAddress' negatively
     * @target public static boolean validateIpAddress(final String ip)
     * @test_data invalidIPAddress
     * @pre_condition none
     * @procedure Call validateIpAddress()
     * @post_condition none
     * @expected validateIpAddress() will be called without exception
     */

    @Test
    public void validateIpAddress_N() {
        try {
            assertEquals(false, OICHelper.validateIpAddress("invalidIPAddress"));
            assertEquals(false, OICHelper.validateIpAddress(null));
        } catch (Exception ex) {
            fail("failed to call validateIpAddress method without exception :"
                    + ex.getMessage());
        }
    }

}

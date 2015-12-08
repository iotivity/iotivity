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

package oic.ctt.formatter;

import static org.junit.Assert.*;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Rule;
import org.junit.Test;

import oic.ctt.formatter.CborManager;
import oic.ctt.formatter.OCPayloadType;
import oic.ctt.testhelper.*;

public class CborManagerTest extends ConformanceTestHelper {

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
        // cborManager = new CborManager();
    }

    @After
    public void tearDown() throws Exception {
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-20
     * @reviewer
     * @since 2015-08-20
     * @see none
     * @objective Test 'convertToCbor' positively
     * @target public static byte[] convertToCbor(String payload, OCPayloadType
     *         payloadType)
     * @test_data PAYLOAD_TYPE_DISCOVERY
     * @test_data PAYLOAD_TYPE_DEVICE
     * @test_data PAYLOAD_TYPE_PLATFORM
     * @test_data PAYLOAD_TYPE_REPRESENTATION
     * @test_data PAYLOAD_TYPE_SECURITY
     * @pre_condition none
     * @procedure Call convertToCbor()
     * @post_condition none
     * @expected getValue() will be called without exception and convert payload
     *           to cbor
     */

    @Test
    public void convertToCbor_P() {
        try {
            assertNotNull("Failed to convert", CborManager.convertToCbor(
                    getMatchString(cborJsonDataPath),
                    OCPayloadType.PAYLOAD_TYPE_DISCOVERY));
            assertNotNull("Failed to convert", CborManager.convertToCbor(
                    getMatchString(cborJsonDataPath),
                    OCPayloadType.PAYLOAD_TYPE_DEVICE));
            assertNotNull("Failed to convert", CborManager.convertToCbor(
                    getMatchString(cborJsonDataPath),
                    OCPayloadType.PAYLOAD_TYPE_PLATFORM));
            assertNotNull("Failed to convert", CborManager.convertToCbor(
                    getMatchString(cborJsonDataPath),
                    OCPayloadType.PAYLOAD_TYPE_REPRESENTATION));
            assertNotNull("Failed to convert", CborManager.convertToCbor(
                    getMatchString(cborJsonDataPath),
                    OCPayloadType.PAYLOAD_TYPE_SECURITY));
            assertNotNull("Failed to convert", CborManager.convertToCbor(
                    getMatchString(cborJsonDataPath),
                    OCPayloadType.PAYLOAD_TYPE_NONE));

        } catch (Exception ex) {
            fail("failed to call convertToCbor method with the following exception :"
                    + ex.getMessage());
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-20
     * @reviewer
     * @since 2015-08-20
     * @see none
     * @objective Test 'convertToCbor' negatively
     * @target public static byte[] convertToCbor(String payload, OCPayloadType
     *         payloadType)
     * @test_data invalidJsonString PAYLOAD_TYPE_INVALID
     * @pre_condition none
     * @procedure Call convertToCbor()
     * @post_condition none
     * @expected getValue() will be called without exception and convert payload
     *           to cbor
     */

    @Test
    public void convertToCbor_N() {
        try {
            assertNull("Failed to convert", CborManager.convertToCbor(
                    "", OCPayloadType.PAYLOAD_TYPE_NONE));
        } catch (Exception ex) {
            fail("failed to call convertToCbor method with the following exception :"
                    + ex.getMessage());
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-20
     * @reviewer
     * @since 2015-08-20
     * @see none
     * @objective Test 'getPayloadAsJsonString' positively
     * @target public static String getPayloadAsJsonString(byte[] payload)
     * @test_data byteDataOfJsonString
     * @pre_condition none
     * @procedure Call getPayloadAsJsonString()
     * @post_condition none
     * @expected getPayloadAsJsonString() will be called without exception and
     *           can get Json string
     */

    @Test
    public void getPayloadAsJsonString_P() {
        try {
            assertNotNull(
                    "Failed to convert",
                    CborManager
                            .getPayloadAsJsonString(getCborByteData(OCPayloadType.PAYLOAD_TYPE_DISCOVERY)));
        } catch (Exception ex) {
            fail("failed to call getPayloadAsJsonString method with the following exception :"
                    + ex.getMessage());
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-20
     * @reviewer
     * @since 2015-08-20
     * @see none
     * @objective Test 'getPayloadAsJsonString' negatively
     * @target public static String getPayloadAsJsonString(byte[] payload)
     * @test_data byteDataOfJsonString
     * @pre_condition none
     * @procedure Call getPayloadAsJsonString()
     * @post_condition none
     * @expected getPayloadAsJsonString() will be called without exception and
     *           can get Json string
     */

    @Test
    public void getPayloadAsJsonString_N() {
        try {
            assertNotNull(
                    "Failed to convert",
                    CborManager
                            .getPayloadAsJsonString(getCborByteData(OCPayloadType.PAYLOAD_TYPE_DISCOVERY)));
        } catch (Exception ex) {
            fail("failed to call getPayloadAsJsonString method with the following exception :"
                    + ex.getMessage());
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-20
     * @reviewer
     * @since 2015-08-20
     * @see none
     * @objective Test 'getPayloadType' positively
     * @target public static OCPayloadType getPayloadType(byte[] payload)
     * @test_data byteDataOfJsonString
     * @pre_condition none
     * @procedure Call getPayloadType()
     * @post_condition none
     * @expected getPayloadType() will be called without exception and can get
     *           Json string
     */

    @Test
    public void getPayloadType_P() {
        try {
            assertEquals(
                    "payload type is not PAYLOAD_TYPE_DISCOVERY as expected",
                    OCPayloadType.PAYLOAD_TYPE_REPRESENTATION,
                    CborManager
                            .getPayloadType(getCborByteData(OCPayloadType.PAYLOAD_TYPE_REPRESENTATION)));
        } catch (Exception ex) {
            fail("failed to call getPayloadType method with the following exception :"
                    + ex.getMessage());
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-20
     * @reviewer
     * @since 2015-08-20
     * @see none
     * @objective Test 'getPayloadType' negatively
     * @target public static OCPayloadType getPayloadType(byte[] payload)
     * @test_data invalidJsonString
     * @pre_condition none
     * @procedure Call getPayloadType()
     * @post_condition none
     * @expected getPayloadType() will be called without exception and can get
     *           Json string
     */

    @Test
    public void getPayloadType_N() {
        try {
        	
            assertEquals(
                    "Payload type is not PAYLOAD_TYPE_DISCOVERY as expected",
                    OCPayloadType.PAYLOAD_TYPE_REPRESENTATION,
                    CborManager
                            .getPayloadType(getCborByteData(OCPayloadType.PAYLOAD_TYPE_INVALID)));
        } catch (Exception ex) {
            fail("failed to call getPayloadType method with the following exception :"
                    + ex.getMessage());
        }
    }

}

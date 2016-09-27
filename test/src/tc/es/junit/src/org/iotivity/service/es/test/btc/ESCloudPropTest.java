/******************************************************************
*
* Copyright 2016 Samsung Electronics All Rights Reserved.
*
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/

package org.iotivity.service.es.test.btc;

import org.iotivity.base.OcRepresentation;
import org.iotivity.service.easysetup.mediator.CloudProp;

import static org.iotivity.service.es.test.helper.ESUtility.*;
import static org.iotivity.service.es.test.helper.ESPropertiesHelper.*;

import android.test.AndroidTestCase;

public class ESCloudPropTest extends AndroidTestCase {

    private static final int DEFAULT_CRED_ID = 0;
    private CloudProp           cloudProp;
    private final static String AUTH_CODE     = "authcode";
    private final static String AUTH_PROVIDER = "authprovider";
    private final static String CI_SERVER     = "coap+tcp://52.69.149.85:5683";
    private static final String EMPTY_STRING  = "";
    private static final String CLOUD_ID      = "f002ae8b-c42c-40d3-8b8d-1927c17bd1b3";

    protected void setUp() throws Exception {
        super.setUp();
        cloudProp = new CloudProp();
    }

    protected void tearDown() throws Exception {
        super.tearDown();
        cloudProp = null;
    }

    /**
     * @since 2016-08-15
     * @see none
     * @objective test setCloudProp API positively
     * @target public void setCloudProp(String authCode, String authProvider,
     *         String ciServer)
     * @test_data AUTH_CODE, AUTH_PROVIDER, CI_SERVER
     * @pre_condition none
     * @procedure 1. Call setCloudProp API. 2. Get all values and check with set
     *            values.
     * @post_condition none
     * @expected setCloudProp set with specified values
     */
    public void testESSetCloudProp_P_ESV() {
        cloudProp.setCloudProp(AUTH_CODE, AUTH_PROVIDER, CI_SERVER);
        assertEquals("Fail to set auth code", cloudProp.getAuthCode(),
                AUTH_CODE);
        assertEquals("Fail to set auth provider", cloudProp.getAuthProvider(),
                AUTH_PROVIDER);
        assertEquals("Fail to set ciServer", cloudProp.getCiServer(),
                CI_SERVER);
    }

    /**
     * @since 2016-08-15
     * @see none
     * @objective test setCloudProp API negatively
     * @target public void setCloudProp(String authCode, String authProvider,
     *         String ciServer)
     * @test_data EMPTY_STRING, AUTH_PROVIDER, CI_SERVER
     * @pre_condition none
     * @procedure 1. Call setCloudProp API.
     * @post_condition none
     * @expected should throw exception
     */
    public void testESSetCloudPropWithInvalidArguments_ESV_ETC_N() {
        try {
            cloudProp.setCloudProp(EMPTY_STRING, AUTH_PROVIDER, CI_SERVER);
        } catch (Exception e) {
            fail(EXCEPTION_SHOULD_NOT_BE_THROWN);
        }

        try {
            cloudProp.setCloudProp(AUTH_CODE, EMPTY_STRING, CI_SERVER);
        } catch (Exception e) {
            fail(EXCEPTION_SHOULD_NOT_BE_THROWN);
        }

        try {
            cloudProp.setCloudProp(AUTH_CODE, AUTH_PROVIDER, EMPTY_STRING);
        } catch (Exception e) {
            fail(EXCEPTION_SHOULD_NOT_BE_THROWN);
        }

    }

    /**
     * @since 2016-08-15
     * @see none
     * @objective test setCloudProp API negatively
     * @target public void setCloudProp(String authCode, String authProvider,
     *         String ciServer)
     * @test_data NULL_VALUE, AUTH_PROVIDER, CI_SERVER
     * @pre_condition none
     * @procedure 1. Call setCloudProp API.
     * @post_condition none
     * @expected should throw exception
     */
    public void testESSetCloudPropWithNullArguments_ESV_ETC_N() {
        try {
            cloudProp.setCloudProp(NULL_VALUE, AUTH_PROVIDER, CI_SERVER);
        } catch (Exception e) {
        }

        try {
            cloudProp.setCloudProp(AUTH_CODE, NULL_VALUE, CI_SERVER);
        } catch (Exception e) {
            fail(EXCEPTION_SHOULD_NOT_BE_THROWN);
        }

        try {
            cloudProp.setCloudProp(AUTH_CODE, AUTH_PROVIDER, NULL_VALUE);
        } catch (Exception e) {
            fail(EXCEPTION_SHOULD_NOT_BE_THROWN);
        }

    }

    /**
     * @since 2016-08-12
     * @see none
     * @objective test getAuthCode API negatively
     * @target public String getAuthCode()
     * @test_data none
     * @pre_condition none
     * @procedure 1.Call getAuthCode API without setting auth code 2.Check
     *            return value
     * @post_condition none
     * @expected return null
     */
    public void testESGetAuthCode_SRC_DSCC_N() {
        assertNull("Should return null", cloudProp.getAuthCode());
    }

    /**
     * @since 2016-08-20
     * @see none
     * @objective test getAuthProvider API negatively
     * @target public getAuthProvider()
     * @test_data none
     * @pre_condition none
     * @procedure 1.Call getAuthProvider API without setting auth provider
     *            2.Check return value
     * @post_condition none
     * @expected return null
     */
    public void testESGetAuthProvider_SRC_DSCC_N() {
        assertNull("Should return null", cloudProp.getAuthProvider());
    }

    /**
     * @since 2016-08-20
     * @see none
     * @objective test getCiServer API negatively
     * @target public String getAuthCode()
     * @test_data none
     * @pre_condition none
     * @procedure 1.Call getCiServer API without setting auth code 2.Check
     *            return value
     * @post_condition none
     * @expected return null
     */
    public void testESGetCiServer_SRC_DSCC_N() {
        assertNull("Should return null", cloudProp.getCiServer());
    }

    /**
     * @since 2016-08-12
     * @see none
     * @objective test setCloudID API negatively
     * @target public void setCloudID(String cloudID)
     * @test_data empty cloud id
     * @pre_condition none
     * @procedure 1.Call setCloudID API with empty values
     * @post_condition none
     * @expected cloud id set with specified values
     */
    public void testESsetCloudIDWithEmptyCloudID_ESV_ETC_N() {

        try {
            cloudProp.setCloudID(EMPTY_STRING);
        } catch (Exception e) {
            fail(EXCEPTION_SHOULD_NOT_BE_THROWN);
        }
    }

    /**
     * @since 2016-08-20
     * @see none
     * @objective test setCloudID API positively
     * @target public void setCloudID(String cloudID)
     * @test_data null cloud ID
     * @pre_condition none
     * @procedure 1.Call setCloudID API 2.Get cloud id and check with set
     *            values.
     * @post_condition none
     * @expected exception should throw
     */
    public void testESsetCloudIDWithNullCloudID_NV_ETC_N() {

        try {
            cloudProp.setCloudID(NULL_VALUE);
        } catch (Exception e) {
            fail(EXCEPTION_SHOULD_NOT_BE_THROWN);
        }
    }

    /**
     * @since 2016-08-20
     * @see none
     * @objective test setCloudID API positively
     * @target public void setCloudID(String cloudID)
     * @test_data CLOUD_ID
     * @pre_condition none
     * @procedure 1.Call setCloudID API 2.Get cloud id and check with set
     *            values.
     * @post_condition none
     * @expected cloud id set with specified values
     */
    public void testESsetCloudID_FSV_P() {
        cloudProp.setCloudID(CLOUD_ID);
        assertEquals("Fail to set cloud id", cloudProp.getCloudID(), CLOUD_ID);
    }
    
    /**
     * @since 2016-08-20
     * @see none
     * @objective test getCloudID API negatively
     * @target public void setCloudID(String cloudID)
     * @test_data none
     * @pre_condition none
     * @procedure 1.Call getCloudID API without setCloudId
     * @post_condition none
     * @expected Return empty string
     */
    public void testESGetCloudID_ETC_N() {
        assertEquals("Should return empty string", EMPTY_STRING, cloudProp.getCloudID());
    }


    /**
     * @since 2016-08-20
     * @see none
     * @objective test OcRepresentation API positively
     * @target public OcRepresentation toOCRepresentation()
     * @test_data none
     * @pre_condition none
     * @procedure 1.Call toOCRepresentation API 2.check instance type
     * @post_condition none
     * @expected return OCRepresentation instance
     */
    public void testESToOCRepresentation_FSV_P() {
        assertTrue("Shoul instance of OcRepresentation",
                cloudProp.toOCRepresentation() instanceof OcRepresentation);
    }

    /**
     * @since 2016-09-05
     * @see none
     * @objective test setCloudID API positively
     * @target public void setCredID(int credID)
     * @test_data  CRED_ID
     * @pre_condition none
     * @procedure 1.Call setCredID API
     * @post_condition none
     * @expected cred id set with specified values
     */
    public void testESsetCredID_FSV_P() {
        cloudProp.setCredID(CRED_ID);
        assertEquals("Fail to set cred id",CRED_ID, cloudProp.getCredID());
    }
    
    /**
     * @since 2016-09-05
     * @see none
     * @objective test getCredID API negatively
     * @target public String getCredID()
     * @test_data none
     * @pre_condition none
     * @procedure 1.Call getCredID API without setCredID
     * @post_condition none
     * @expected return 0
     */
    public void testESsetGetCredID_ETC_N() {
        assertEquals("Fail to set cred id",DEFAULT_CRED_ID, cloudProp.getCredID());
    }
}

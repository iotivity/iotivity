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

import static org.iotivity.service.es.test.helper.ESUtility.*;

import org.iotivity.base.OcRepresentation;
import org.iotivity.service.easysetup.mediator.ESException;
import org.iotivity.service.easysetup.mediator.EnrolleeConf;
import org.iotivity.service.easysetup.mediator.enums.WIFI_FREQ;
import org.iotivity.service.easysetup.mediator.enums.WIFI_MODE;
import org.iotivity.service.es.test.helper.ESEnrolleeConfHelper;

import android.test.AndroidTestCase;

public class ESEnrolleeConfTest extends AndroidTestCase {

    private static final String EMPTY_STRING     = "";
    private int                 total_wifi_modes = 0;
    private EnrolleeConf        enrolleeConf;

    protected void setUp() throws Exception {
        super.setUp();
        enrolleeConf = new EnrolleeConf(
                ESEnrolleeConfHelper.cretaeOcRepresentation());
    }

    protected void tearDown() throws Exception {
        super.tearDown();
        enrolleeConf = null;
    }

    /**
     * @since 2016-08-21
     * @see none
     * @objective test EnrolleeConf constructor negatively
     * @target public EnrolleeConf(OcRepresentation rep)
     * @test_data null
     * @pre_condition none
     * @procedure 1. Call EnrolleeConf with null value
     * @post_condition none
     * @expected ES Exception should be thrown
     */
    public void testESEnrolleeConf_NV_ETC_N() {

        try {
            enrolleeConf = new EnrolleeConf(null);
            fail(EXCEPTION_SHOULD_BE_THROWN);
        } catch (Exception e) {
            assertTrue(ES_EXCEPTION_SHOULD_BE_THROWN, e instanceof ESException);
        }
    }

    /**
     * @since 2016-08-21
     * @see none
     * @objective test getDeviceName API positively
     * @target public String getDeviceName()
     * @test_data none
     * @pre_condition create oc representation for device configuration with
     *                device name attribute
     * @procedure 1. Call getDeviceName API. 2.check device name.
     * @post_condition none
     * @expected return device name as set through oc representation
     */
    public void testESGetDeviceName_SRC_P() {
        assertEquals("Fail to get device name",
                ESEnrolleeConfHelper.DEVICE_NAME, enrolleeConf.getDeviceName());
    }

    /**
     * @since 2016-08-21
     * @see none
     * @objective test isCloudAccessible API positively
     * @target public boolean isCloudAccessible()
     * @test_data none
     * @pre_condition create oc representation with cloud resource URI
     * @procedure 1. Call isCloudAccessible API. 2.check cloud is accessible
     * @post_condition none
     * @expected cloud is accessible as set through oc representation
     */
    public void testESIsCloudAccessible_SRC_P() {
        assertTrue("Cloud should accessible", enrolleeConf.isCloudAccessible());
    }

    /**
     * @since 2016-08-21
     * @see none
     * @objective test getWiFiModes API positively
     * @target public ArrayList<WIFI_MODE> getWiFiModes()
     * @test_data none
     * @pre_condition create oc representation for wifi configuration with wifi
     *                mode attribute
     * @procedure 1. Call getWiFiModes API. 2.check wifi modes
     * @post_condition none
     * @expected return WiFi mode as set through oc presentation
     */
    public void testESGetWiFiModes_SRC_P() {
        total_wifi_modes = 2;
        assertEquals("Total wifi modes should one", total_wifi_modes,
                enrolleeConf.getWiFiModes().size());
        assertEquals("Fail to get WiFi mode", WIFI_MODE.WIFI_11A,
                enrolleeConf.getWiFiModes().get(0));
        assertEquals("Fail to get WiFi mode", WIFI_MODE.WIFI_11AC,
                enrolleeConf.getWiFiModes().get(1));
    }

    /**
     * @since 2016-08-21
     * @see none
     * @objective test getWiFiFreq API positively
     * @target public WIFI_FREQ getWiFiFreq()
     * @test_data none
     * @pre_condition create oc representation for wifi configuration with
     *                frequency attribute
     * @procedure 1. Call getWiFiFreq API. 2.check frequency type
     * @post_condition none
     * @expected return WiFi frequency as set through oc presentation
     */
    public void testESGetWiFiFreq_SRC_P() {
        assertEquals("Fail to get WiFi frequency", WIFI_FREQ.WIFI_5G,
                enrolleeConf.getWiFiFreq());
    }

    /**
     * @since 2016-08-21
     * @see none
     * @objective test getDeviceName API negatively
     * @target public String getDeviceName()
     * @test_data none
     * @pre_condition create empty oc representation
     * @procedure 1. Call getDeviceName API. 2.check device name.
     * @post_condition none
     * @expected return empty device name
     */
    public void testESGetDeviceName_SRC_DSCC_N() {
        enrolleeConf = new EnrolleeConf(new OcRepresentation());
        assertEquals("Fail to get device name", EMPTY_STRING,
                enrolleeConf.getDeviceName());
    }

    /**
     * @since 2015-08-21
     * @see none
     * @objective test isCloudAccessible API negatively
     * @target public boolean isCloudAccessible()
     * @test_data none
     * @pre_condition create empty oc representation
     * @procedure 1. Call isCloudAccessible API. 2.check cloud is inaccessible
     * @post_condition none
     * @expected cloud is not accessible
     */
    public void testESIsCloudAccessible_SRC_DSCC_N() {
        enrolleeConf = new EnrolleeConf(new OcRepresentation());
        assertFalse("Cloud should accessible",
                enrolleeConf.isCloudAccessible());
    }

    /**
     * @since 2015-08-21
     * @see none
     * @objective test getWiFiModes API negatively
     * @target public ArrayList<WIFI_MODE> getWiFiModes()
     * @test_data none
     * @pre_condition create empty oc representation
     * @procedure 1. Call getWiFiModes API. 2.check wifi modes size
     * @post_condition none
     * @expected return 0 wifi modes
     */
    public void testESGetWiFiModes_SRC_DSCC_N() {
        enrolleeConf = new EnrolleeConf(new OcRepresentation());
        assertEquals("Fail to get WiFi mode", total_wifi_modes,
                enrolleeConf.getWiFiModes().size());
    }

    /**
     * @since 2016-08-21
     * @see none
     * @objective test getWiFiFreq API negatively
     * @target public WIFI_FREQ getWiFiFreq()
     * @test_data none
     * @pre_condition create empty oc representation
     * @procedure 1. Call getWiFiFreq API. 2.check frequency type
     * @post_condition none
     * @expected return WIFI_FREQ_NONE type WiFi frequency
     */
    public void testESGetWiFiFreq_SRC_DSCC_N() {
        enrolleeConf = new EnrolleeConf(new OcRepresentation());
        assertEquals("Fail to get WiFi frequency", WIFI_FREQ.WIFI_FREQ_NONE,
                enrolleeConf.getWiFiFreq());
    }

}
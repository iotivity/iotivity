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

import org.iotivity.service.easysetup.mediator.DeviceProp;
import org.iotivity.service.easysetup.mediator.ESException;
import org.iotivity.service.easysetup.mediator.enums.WIFI_AUTHTYPE;
import org.iotivity.service.easysetup.mediator.enums.WIFI_ENCTYPE;

import android.util.Log;

import static org.iotivity.service.es.test.helper.ESPropertiesHelper.*;
import static org.iotivity.service.es.test.helper.ESUtility.*;

import junit.framework.TestCase;

public class ESDevicePropTest extends TestCase {

    private DeviceProp          deviceProp;
    
    protected void setUp() throws Exception {
        super.setUp();
        deviceProp = new DeviceProp();
    }

    protected void tearDown() throws Exception {
        super.tearDown();
        deviceProp = null;
    }

    /**
     * @since 2016-08-12
     * @see none
     * @objective test setWiFiProp API positively
     * @target public void setWiFiProp(String ssid, String pwd, WIFI_AUTHTYPE
     *         authtype, WIFI_ENCTYPE encType)
     * @test_data ENROLLEE_SS_ID ENROLLEE_PASSWORD AUTH_TYPE
     *            WIFI_ENCTYPE.TKIP_AES
     * @pre_condition none
     * @procedure 1. Call setWiFiProp API. 2. Get all values and check with set
     *            values.
     * @post_condition none
     * @expected setWiFiProp set with specified values
     */
    public void testESSetWiFiProp_P_FSV() {
        deviceProp.setWiFiProp(ENROLLEE_SS_ID, ENROLLEE_PASSWORD,
                WIFI_AUTHTYPE.WPA2_PSK, WIFI_ENCTYPE.TKIP_AES);
        assertEquals("Fail to set WiFi ssid", deviceProp.getSsid(),
                ENROLLEE_SS_ID);
        assertEquals("Fail to set WiFi password", deviceProp.getPassword(),
                ENROLLEE_PASSWORD);
        assertEquals("Fail to set WiFi auth type", deviceProp.getAuthType(),
                WIFI_AUTHTYPE.WPA2_PSK);
        assertEquals("Fail to set WiFi enc type", deviceProp.getEncType(),
                WIFI_ENCTYPE.TKIP_AES);
    }

    /**
     * @since 2016-08-12
     * @see none
     * @objective test setWiFiProp API positively
     * @target public void setWiFiProp(String ssid, String pwd, WIFI_AUTHTYPE
     *         authtype, WIFI_ENCTYPE encType)
     * @test_data ENROLLEE_SS_ID ENROLLEE_PASSWORD AUTH_TYPE
     *            WIFI_ENCTYPE.TKIP_AES
     * @pre_condition none
     * @procedure 1.Call setWiFiProp API With all auth type enum values 2.Get
     *            all values and check with set values.
     * @post_condition none
     * @expected setWiFiProp auth type with specified values
     */
    public void testESSetWiFiPropWithAllAUTHType_P_CLU() {
        deviceProp.setWiFiProp(ENROLLEE_SS_ID, ENROLLEE_PASSWORD,
                WIFI_AUTHTYPE.NONE_AUTH, WIFI_ENCTYPE.TKIP_AES);
        assertEquals("Fail to set WiFi auth type", deviceProp.getAuthType(),
                WIFI_AUTHTYPE.NONE_AUTH);
        deviceProp.setWiFiProp(ENROLLEE_SS_ID, ENROLLEE_PASSWORD,
                WIFI_AUTHTYPE.WEP, WIFI_ENCTYPE.TKIP_AES);
        assertEquals("Fail to set WiFi auth type", deviceProp.getAuthType(),
                WIFI_AUTHTYPE.WEP);
        deviceProp.setWiFiProp(ENROLLEE_SS_ID, ENROLLEE_PASSWORD,
                WIFI_AUTHTYPE.WPA_PSK, WIFI_ENCTYPE.TKIP_AES);
        assertEquals("Fail to set WiFi auth type", deviceProp.getAuthType(),
                WIFI_AUTHTYPE.WPA_PSK);
        deviceProp.setWiFiProp(ENROLLEE_SS_ID, ENROLLEE_PASSWORD,
                WIFI_AUTHTYPE.WPA2_PSK, WIFI_ENCTYPE.TKIP_AES);
        assertEquals("Fail to set WiFi auth type", deviceProp.getAuthType(),
                WIFI_AUTHTYPE.WPA2_PSK);
    }

    /**
     * @since 2016-08-12
     * @see none
     * @objective test setWiFiProp API positively
     * @target public void setWiFiProp(String ssid, String pwd, WIFI_AUTHTYPE
     *         authtype, WIFI_ENCTYPE encType)
     * @test_data ENROLLEE_SS_ID ENROLLEE_PASSWORD AUTH_TYPE
     *            WIFI_ENCTYPE.TKIP_AES
     * @pre_condition none
     * @procedure Call setWiFiProp API With all encryptions type enum value
     * @post_condition none
     * @expected setWiFiProp encryption type with specified values
     */
    public void testESSetWiFiPropWithAllENCType_P_CLU() {
        deviceProp.setWiFiProp(ENROLLEE_SS_ID, ENROLLEE_PASSWORD,
                WIFI_AUTHTYPE.WPA2_PSK, WIFI_ENCTYPE.NONE_ENC);
        assertEquals("Fail to set WiFi NONE_ENC encryption type",
                deviceProp.getEncType(), WIFI_ENCTYPE.NONE_ENC);
        deviceProp.setWiFiProp(ENROLLEE_SS_ID, ENROLLEE_PASSWORD,
                WIFI_AUTHTYPE.WPA2_PSK, WIFI_ENCTYPE.WEP_64);
        assertEquals("Fail to set WiFi WEP_64 encryption type",
                deviceProp.getEncType(), WIFI_ENCTYPE.WEP_64);
        deviceProp.setWiFiProp(ENROLLEE_SS_ID, ENROLLEE_PASSWORD,
                WIFI_AUTHTYPE.WPA2_PSK, WIFI_ENCTYPE.WEP_128);
        assertEquals("Fail to set WiFi WEP_128 encryption type",
                deviceProp.getEncType(), WIFI_ENCTYPE.WEP_128);
        deviceProp.setWiFiProp(ENROLLEE_SS_ID, ENROLLEE_PASSWORD,
                WIFI_AUTHTYPE.WPA2_PSK, WIFI_ENCTYPE.TKIP);
        assertEquals("Fail to set WiFi TKIP encryption type",
                deviceProp.getEncType(), WIFI_ENCTYPE.TKIP);
        deviceProp.setWiFiProp(ENROLLEE_SS_ID, ENROLLEE_PASSWORD,
                WIFI_AUTHTYPE.WPA2_PSK, WIFI_ENCTYPE.AES);
        assertEquals("Fail to set WiFi AES encryption type",
                deviceProp.getEncType(), WIFI_ENCTYPE.AES);
        deviceProp.setWiFiProp(ENROLLEE_SS_ID, ENROLLEE_PASSWORD,
                WIFI_AUTHTYPE.WPA2_PSK, WIFI_ENCTYPE.TKIP_AES);
        assertEquals("Fail to set WiFi TKIP_AES encryption type",
                deviceProp.getEncType(), WIFI_ENCTYPE.TKIP_AES);
    }

    /**
     * @since 2016-08-12
     * @see none
     * @objective test values API negatively
     * @target public void setWiFiProp(String ssid, String pwd, WIFI_AUTHTYPE
     *         authtype, WIFI_ENCTYPE encType)
     * @test_data EMPTY_STRING, null
     * @pre_condition none
     * @procedure Call setWiFiProp API with empty string values
     * @post_condition none
     * @expected should throw exception
     */
    public void testESSetWiFiPropWithEmptyString_ESV_NV_ETC_N() {
        
        try {
            deviceProp.setWiFiProp(EMPTY_STRING, ENROLLEE_PASSWORD,
                    WIFI_AUTHTYPE.WPA2_PSK, WIFI_ENCTYPE.TKIP_AES);
        } catch (Exception e) {
            fail(EXCEPTION_SHOULD_NOT_BE_THROWN);
        }
        
        
        try {
            deviceProp.setWiFiProp(ENROLLEE_SS_ID, EMPTY_STRING,
                    WIFI_AUTHTYPE.WPA2_PSK, WIFI_ENCTYPE.TKIP_AES);
        } catch (Exception e) {
            fail(EXCEPTION_SHOULD_NOT_BE_THROWN);
        }
        
    }

    /**
     * @since 2016-08-17
     * @see none
     * @objective test values API negatively
     * @target public void setWiFiProp(String ssid, String pwd, WIFI_AUTHTYPE
     *         authtype, WIFI_ENCTYPE encType)
     * @test_data NULL_STRING, ENROLLEE_PASSWORD, WIFI_AUTHTYPE.WPA2_PSK,
     *            WIFI_ENCTYPE.TKIP_AES
     * @pre_condition none
     * @procedure Call setWiFiProp API with null values
     * @post_condition none
     * @expected should throw exception
     */
    public void testESSetWiFiPropWithNullArguments_NV_ETC_N() {
        try {
            deviceProp.setWiFiProp(NULL_VALUE, ENROLLEE_PASSWORD,
                    WIFI_AUTHTYPE.WPA2_PSK, WIFI_ENCTYPE.TKIP_AES);
        } catch (Exception e) {
            fail(EXCEPTION_SHOULD_NOT_BE_THROWN);
        }

        try {
            deviceProp.setWiFiProp(ENROLLEE_SS_ID, NULL_VALUE,
                    WIFI_AUTHTYPE.WPA2_PSK, WIFI_ENCTYPE.TKIP_AES);
        } catch (Exception e) {
            fail(EXCEPTION_SHOULD_NOT_BE_THROWN);
        }
        
        try {
            deviceProp.setWiFiProp(ENROLLEE_SS_ID, ENROLLEE_PASSWORD, null,
                    WIFI_ENCTYPE.TKIP_AES);
            fail(EXCEPTION_SHOULD_BE_THROWN);
        } catch (Exception e) {
            assertTrue(true);
        }
        
        try {
            deviceProp.setWiFiProp(ENROLLEE_SS_ID, ENROLLEE_PASSWORD,
                    WIFI_AUTHTYPE.WPA2_PSK, null);
            fail(EXCEPTION_SHOULD_BE_THROWN);
        } catch (Exception e) {
            assertTrue(true);
        }
    }

    /**
     * @since 2016-08-12
     * @see none
     * @objective test setDevConfProp API positively
     * @target public void setDevConfProp(String language, String country)
     * @test_data LANGUAGE COUNTRY
     * @pre_condition none
     * @procedure 1.Call setDevConfProp API 2.Get all values and check with set
     *            values.
     * @post_condition none
     * @expected setDevConfProp with specified values
     */
    public void testESSetDevConfProp_FSV_P() {
        deviceProp.setDevConfProp(LANGUAGE, COUNTRY, LOCATION);
        assertEquals("Fail to set WiFi ssid", deviceProp.getLanguage(),
                LANGUAGE);
        assertEquals("Fail to set WiFi password", deviceProp.getCountry(),
                COUNTRY);
        assertEquals("Fail to set location", deviceProp.getLocation(),
                LOCATION);
    }

    /**
     * @since 2016-08-12
     * @see none
     * @objective test setDevConfProp API negatively
     * @target public void setDevConfProp(String language, String country)
     * @test_data EMPTY_STRING, null
     * @pre_condition none
     * @procedure Call setDevConfProp API
     * @post_condition none
     * @expected setWiFiProp with specified values
     */
    public void testESSetDevConfPropWithInvalidArgument_ESV_NV_N() {
        
        try {
            deviceProp.setDevConfProp(EMPTY_STRING, LANGUAGE, LOCATION);
        } catch (Exception e) {
            fail(EXCEPTION_SHOULD_NOT_BE_THROWN);
        }

        try {
            deviceProp.setDevConfProp(LANGUAGE, EMPTY_STRING, LOCATION);
        } catch (Exception e) {
            fail(EXCEPTION_SHOULD_NOT_BE_THROWN);
        }
        
        try {
            deviceProp.setDevConfProp(NULL_VALUE, LANGUAGE, LOCATION);
        } catch (Exception e) {
            fail(EXCEPTION_SHOULD_NOT_BE_THROWN);
        }

        try {
            deviceProp.setDevConfProp(LANGUAGE, NULL_VALUE, LOCATION);
        } catch (Exception e) {
            fail(EXCEPTION_SHOULD_NOT_BE_THROWN);
        }
    }

    /**
     * @since 2016-08-12
     * @see none
     * @objective test getSsid API negatively
     * @target public String getSsid()
     * @test_data
     * @pre_condition none
     * @procedure 1.Call getSsid API without setting ssid 2.Check return value
     * @post_condition none
     * @expected return empty string
     */
    public void testESGetSsid_SRC_DSCC_N() {
        assertNull("Fail to get WiFi ssid", deviceProp.getSsid());
    }

    /**
     * @since 2016-08-12
     * @see none
     * @objective test getPassword API negatively
     * @target public String getPassword()
     * @test_data
     * @pre_condition none
     * @procedure 1.Call getPassword API without setting password 2.Check return
     *            value
     * @post_condition none
     * @expected return empty string
     */
    public void testESGetPassword_SRC_DSCC_N() {
        assertNull("Fail to get WiFi password", deviceProp.getPassword());
    }

    /**
     * @since 2016-08-12
     * @see none
     * @objective test getAuthType API negatively
     * @target public String getAuthType()
     * @test_data
     * @pre_condition none
     * @procedure 1.Call getAuthType API without setting auth type 2.Check
     *            return value
     * @post_condition none
     * @expected return NONE_AUTH type auth
     */
    public void testESGetAuthType_SRC_DSCC_N() {
        assertEquals("Fail to get auth type", deviceProp.getAuthType(),
                WIFI_AUTHTYPE.NONE_AUTH);
    }

    /**
     * @since 2016-08-12
     * @see none
     * @objective test getEncType API negatively
     * @target public String getEncType()
     * @test_data none
     * @pre_condition none
     * @procedure 1.Call getEncType API without setting encryption type 2.Check
     *            return value
     * @post_condition none
     * @expected return NONE_ENC type encryption
     */
    public void testESGetENCType_SRC_DSCC_N() {
        assertEquals("Fail to get encryption type", deviceProp.getEncType(),
                WIFI_ENCTYPE.NONE_ENC);
    }

    /**
     * @since 2016-08-12
     * @see none
     * @objective test getCountry API negatively
     * @target public String getCountry()
     * @test_data none
     * @pre_condition none
     * @procedure 1.Call getCountry API without setting country value 2.Check
     *            return value
     * @post_condition none
     * @expected return empty string value
     */
    public void testESGetCountry_SRC_DSCC_N() {
        assertNull("Fail to get WiFi ssid", deviceProp.getCountry());
    }

    /**
     * @since 2016-08-12
     * @see none
     * @objective test getLanguage API negatively
     * @target public String getLanguage()
     * @test_data none
     * @pre_condition none
     * @procedure 1.Call getLanguage API without setting language value 2.Check
     *            return value
     * @post_condition none
     * @expected return empty string value
     */
    public void testESGetLanguage_FSV_P() {
        assertNull("Fail to get WiFi password", deviceProp.getLanguage());
    }
}

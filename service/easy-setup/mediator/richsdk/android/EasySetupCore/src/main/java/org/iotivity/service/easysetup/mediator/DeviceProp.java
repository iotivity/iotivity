/**
 * ***************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
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
 * ****************************************************************
 */

package org.iotivity.service.easysetup.mediator;

import org.iotivity.service.easysetup.mediator.enums.WIFI_AUTHTYPE;
import org.iotivity.service.easysetup.mediator.enums.WIFI_ENCTYPE;

/**
 * This class contains device properties to be delivered to Enrollee
 */
public class DeviceProp {

    private final String mSsid;
    private final String mPwd;
    private final WIFI_AUTHTYPE mAuthType;
    private final WIFI_ENCTYPE mEncType;
    private final String mLanguage;
    private final String mCountry;

    /**
     * Constructor
     *
     * @param ssid WiFi AP's SSID
     * @param pwd WiFi AP's password
     * @param authType WiFi AP's authenticate type
     * @param encType WiFi AP's encryption type
     * @param language IETF language tag using ISO 639X
     * @param country ISO Country Code (ISO 3166-1 Alpha-2)
     */
    public DeviceProp(String ssid, String pwd, WIFI_AUTHTYPE authType, WIFI_ENCTYPE encType,
                                     String language, String country)
    {
        mSsid = ssid;
        mPwd = pwd;
        mAuthType = authType;
        mEncType = encType;
        mLanguage = language;
        mCountry = country;
    }

    /**
     * Get WiFi AP's SSID
     *
     * @return String WiFi AP's SSID
     */
    public String getSsid()
    {
        return mSsid;
    }

    /**
     * Get WiFi AP's password
     *
     * @return String WiFi AP's password
     */
    public String getPwd()
    {
        return mPwd;
    }

    /**
     * Get WiFi AP's authenticate type
     * NONE_AUTH(0), WEP(1), WPA_PSK(2), WPA2_PSK(3)
     *
     * @return WIFI_AUTHTYPE WiFi AP's authenticate type
     */
    public WIFI_AUTHTYPE getAuthType()
    {
        return mAuthType;
    }

    /**
     * Get WiFi AP's encryption type
     * NONE_ENC(0), WEP_64(1), WEP_128(2), TKIP(3), AES(4), TKIP_AES(5)
     *
     * @return WIFI_ENCTYPE WiFi AP's encryption type
     */
    public WIFI_ENCTYPE getEncType()
    {
        return mEncType;
    }

    /**
     * Get IETF language tag using ISO 639X
     *
     * @return String IETF language tag using ISO 639X
     */
    public String getLanguage()
    {
        return mLanguage;
    }

    /**
     * Get ISO Country Code (ISO 3166-1 Alpha-2)
     *
     * @return String ISO Country Code (ISO 3166-1 Alpha-2)
     */
    public String getCountry()
    {
        return mCountry;
    }
}

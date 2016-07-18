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
 * This class contains on data provisioning information.
 * It implements ProvisioningInfo interface and provide configuration object specific to data registration
 */
public class DeviceProp {

    private final String mSsid;
    private final String mPwd;
    private final WIFI_AUTHTYPE mAuthType;
    private final WIFI_ENCTYPE mEncType;
    private final String mLanguage;
    private final String mCountry;

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

    public String getSsid()
    {
        return mSsid;
    }

    public String getPwd()
    {
        return mPwd;
    }

    public WIFI_AUTHTYPE getAuthType()
    {
        return mAuthType;
    }

    public WIFI_ENCTYPE getEncType()
    {
        return mEncType;
    }

    public String getLanguage()
    {
        return mLanguage;
    }

    public String getCountry()
    {
        return mCountry;
    }
}

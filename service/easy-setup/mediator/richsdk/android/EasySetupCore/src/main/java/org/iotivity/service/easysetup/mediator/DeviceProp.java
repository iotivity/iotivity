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

import android.util.Log;

import org.iotivity.service.easysetup.mediator.enums.WIFI_AUTHTYPE;
import org.iotivity.service.easysetup.mediator.enums.WIFI_ENCTYPE;

import org.iotivity.base.OcException;
import org.iotivity.base.OcRepresentation;

/**
 * This class contains device properties to be delivered to Enrollee
 */
public class DeviceProp {
    private static final String TAG = DeviceProp.class.getName();
    protected OcRepresentation mRep;

    /**
     * Constructor
     */
    public DeviceProp() {
        mRep = new OcRepresentation();
    }

    public void setWiFiProp(String ssid, String pwd, WIFI_AUTHTYPE authtype, WIFI_ENCTYPE enctype)
    {
        try
        {
            mRep.setValue(ESConstants.OC_RSRVD_ES_SSID, ssid);
            mRep.setValue(ESConstants.OC_RSRVD_ES_CRED, pwd);
            mRep.setValue(ESConstants.OC_RSRVD_ES_AUTHTYPE, authtype.getValue());
            mRep.setValue(ESConstants.OC_RSRVD_ES_ENCTYPE, enctype.getValue());
        } catch (OcException e) {
            Log.e(TAG, "setWiFiProp is failed.");
        }
    }

    public void setDevConfProp(String language, String country, String location)
    {
        try {
            mRep.setValue(ESConstants.OC_RSRVD_ES_LANGUAGE, language);
            mRep.setValue(ESConstants.OC_RSRVD_ES_COUNTRY, country);
            mRep.setValue(ESConstants.OC_RSRVD_ES_LOCATION, location);
        } catch (OcException e) {
            Log.e(TAG, "setDevConfProp is failed.");
        }
    }

    /**
     * Get WiFi AP's SSID
     *
     * @return String WiFi AP's SSID
     */
    public String getSsid()
    {
        try {
            if(mRep.hasAttribute(ESConstants.OC_RSRVD_ES_SSID))
                return mRep.getValue(ESConstants.OC_RSRVD_ES_SSID);
        } catch (OcException e) {
            Log.e(TAG, "getSsid is failed.");
        }
        return new String("");
    }

    /**
     * Get WiFi AP's password
     *
     * @return String WiFi AP's password
     */
    public String getPassword()
    {
        try {
            if(mRep.hasAttribute(ESConstants.OC_RSRVD_ES_CRED))
                return mRep.getValue(ESConstants.OC_RSRVD_ES_CRED);
        } catch (OcException e) {
            Log.e(TAG, "getPassword is failed.");
        }
        return new String("");
    }

    /**
     * Get WiFi AP's authenticate type
     * NONE_AUTH(0), WEP(1), WPA_PSK(2), WPA2_PSK(3)
     *
     * @return WIFI_AUTHTYPE WiFi AP's authenticate type
     */
    public WIFI_AUTHTYPE getAuthType()
    {
        try {
            if (mRep.hasAttribute(ESConstants.OC_RSRVD_ES_AUTHTYPE))
                return WIFI_AUTHTYPE.fromInt((int)mRep.getValue(ESConstants.OC_RSRVD_ES_AUTHTYPE));
        } catch (OcException e) {
            Log.e(TAG, "getAuthType is failed.");
        }
        return WIFI_AUTHTYPE.NONE_AUTH;
    }

    /**
     * Get WiFi AP's encryption type
     * NONE_ENC(0), WEP_64(1), WEP_128(2), TKIP(3), AES(4), TKIP_AES(5)
     *
     * @return WIFI_ENCTYPE WiFi AP's encryption type
     */
    public WIFI_ENCTYPE getEncType()
    {
        try {
            if (mRep.hasAttribute(ESConstants.OC_RSRVD_ES_ENCTYPE))
                return WIFI_ENCTYPE.fromInt((int)mRep.getValue(ESConstants.OC_RSRVD_ES_ENCTYPE));
        } catch (OcException e) {
            Log.e(TAG, "getEncType is failed.");
        }
        return WIFI_ENCTYPE.NONE_ENC;
    }

    /**
     * Get IETF language tag using ISO 639X
     *
     * @return String IETF language tag using ISO 639X
     */
    public String getLanguage()
    {
        try {
            if(mRep.hasAttribute(ESConstants.OC_RSRVD_ES_LANGUAGE))
                return mRep.getValue(ESConstants.OC_RSRVD_ES_LANGUAGE);
        } catch (OcException e) {
            Log.e(TAG, "getLanguage is failed.");
        }
        return new String("");
    }

    /**
     * Get ISO Country Code (ISO 3166-1 Alpha-2)
     *
     * @return String ISO Country Code (ISO 3166-1 Alpha-2)
     */
    public String getCountry()
    {
        try {
            if (mRep.hasAttribute(ESConstants.OC_RSRVD_ES_COUNTRY))
                return mRep.getValue(ESConstants.OC_RSRVD_ES_COUNTRY);
        } catch (OcException e) {
            Log.e(TAG, "getCountry is failed.");
        }
        return new String("");
    }

    /**
     * Get location info
     *
     * @return String location info of GPS
     */
    public String getLocation()
    {
        try {
            if (mRep.hasAttribute(ESConstants.OC_RSRVD_ES_LOCATION))
                return mRep.getValue(ESConstants.OC_RSRVD_ES_LOCATION);
        } catch (OcException e) {
            Log.e(TAG, "getLocation is failed.");
        }
        return new String("");
    }

    public OcRepresentation toOCRepresentation()
    {
        return mRep;
    }
}

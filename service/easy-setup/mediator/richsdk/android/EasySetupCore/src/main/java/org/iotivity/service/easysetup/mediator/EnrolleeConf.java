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

import org.iotivity.base.OcException;
import org.iotivity.base.OcRepresentation;
import org.iotivity.service.easysetup.mediator.ESConstants;
import org.iotivity.service.easysetup.mediator.enums.WIFI_FREQ;
import org.iotivity.service.easysetup.mediator.enums.WIFI_MODE;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

/**
 * This class stores Enrollee's configuration including WiFi and Device configuration
 * including supported WiFi frequency and device name
 */
public class EnrolleeConf
{
    private static final String TAG = EnrolleeConf.class.getName();
    private OcRepresentation mProvRep = null, mWiFiRep = null, mDevConfRep = null, mCloudRep = null;
    /**
     * Constructor
     *
     * @param rep received properties in a form of OcRepresentation
     *
     */
    public EnrolleeConf(OcRepresentation rep)
    {
        mProvRep = rep;

        List<OcRepresentation> children = rep.getChildren();

        for (OcRepresentation child : children) {
            List<String> rts = child.getResourceTypes();

            if(child.getUri().indexOf(ESConstants.OC_RSRVD_ES_URI_WIFI) != -1)
            {
                mWiFiRep = child;
            }
            else if(child.getUri().indexOf(ESConstants.OC_RSRVD_ES_URI_DEVCONF) != -1)
            {
                mDevConfRep = child;
            }
            else if(child.getUri().indexOf(ESConstants.OC_RSRVD_ES_URI_CLOUDSERVER) != -1)
            {
                mCloudRep = child;
            }
        }
    }

    /**
     * Get Device Name property in DevConf resource
     *
     * @return deviceName
     */
    public String getDeviceName()
    {
        try
        {
            if(mDevConfRep != null && mDevConfRep.hasAttribute(ESConstants.OC_RSRVD_ES_DEVNAME)) {
                return (String) mDevConfRep.getValue(ESConstants.OC_RSRVD_ES_DEVNAME);
            }
        } catch (OcException e) {
                Log.e(TAG, "getWiFiModes is failed.");
        }
        return new String("");
    }

    /**
     * Get Supported WiFi Modes property in WiFi resource
     *
     * @return a list of WiFi modes
     */
    public ArrayList<WIFI_MODE> getWiFiModes()
    {
        ArrayList<WIFI_MODE> modes = new ArrayList<WIFI_MODE>();
        try {
            if (mWiFiRep != null && mWiFiRep.hasAttribute(ESConstants.OC_RSRVD_ES_SUPPORTEDWIFIMODE)) {
                int modes_int[] = mWiFiRep.getValue(ESConstants.OC_RSRVD_ES_SUPPORTEDWIFIMODE);
                for (int i = 0 ; i < modes_int.length ; ++i) {
                    modes.add(WIFI_MODE.fromInt(modes_int[i]));
                }
            }
        } catch (OcException e) {
            Log.e(TAG, "getWiFiModes is failed.");
        }
        return modes;
    }

    /**
     * Get Supported WiFi frequency property in WiFi resource
     *
     * @return WiFi frequency
     */
    public WIFI_FREQ getWiFiFreq()
    {
        try{
            if(mWiFiRep != null && mWiFiRep.hasAttribute(ESConstants.OC_RSRVD_ES_SUPPORTEDWIFIFREQ))
                return WIFI_FREQ.fromInt(
                        (int)mWiFiRep.getValue(ESConstants.OC_RSRVD_ES_SUPPORTEDWIFIFREQ));
        } catch (OcException e) {
            Log.e(TAG, "getWiFiFreq is failed.");
        }
        return WIFI_FREQ.WIFI_FREQ_NONE;
    }

    /**
     * To check if Enrollee can access to cloud. To decide its preference, we check if a cloudserver
     * resource is registered on Enrollee.
     *
     * @return boolean
     */
    public boolean isCloudAccessible()
    {
        if(mCloudRep != null && mCloudRep.getUri().indexOf(ESConstants.OC_RSRVD_ES_URI_CLOUDSERVER) != -1)
            return true;

        return false;
    }
}



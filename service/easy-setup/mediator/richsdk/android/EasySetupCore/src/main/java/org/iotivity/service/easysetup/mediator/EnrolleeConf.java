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
import java.util.List;

/**
 * This class stores Enrollee's configuration including WiFi and Device configuration
 * including supported WiFi frequency and device name
 */
public class EnrolleeConf
{
    private static final String TAG = EnrolleeConf.class.getName();
    protected OcRepresentation mEasySetupRep = null;
    protected int mSpecVersion = 0;

    /**
     * Constructor
     *
     * @param rep received properties in a form of OcRepresentation
     * @param specVersion Indicates the spec version of enrollee.
     *
     */
    public EnrolleeConf(OcRepresentation rep, int specVersion)
    {
        mEasySetupRep = rep;
        mSpecVersion = specVersion;
    }

    public EnrolleeConf(EnrolleeConf enrolleeConf)
    {
        mEasySetupRep = enrolleeConf.getEasySetupRep();
    }

    /**
     * Get Device Name property in DevConf resource
     *
     * @return deviceName
     */
    public String getDeviceName()
    {
        if(mEasySetupRep == null)
        {
            return null;
        }

        List<OcRepresentation> children = mEasySetupRep.getChildren();

        for (OcRepresentation child : children) {
            if(child.getUri().indexOf(ESConstants.OC_RSRVD_ES_URI_DEVCONF) != -1)
            {
                try
                {
                    OcRepresentation rep;
                    if(child.hasAttribute(ESConstants.OC_RSRVD_REPRESENTATION))
                    {
                        rep = (OcRepresentation)child.getValue(ESConstants.OC_RSRVD_REPRESENTATION);
                    }
                    else
                    {
                        return null;
                    }

                    if(rep.hasAttribute(ESConstants.OC_RSRVD_ES_DEVNAME)) {
                        return (String) rep.getValue(ESConstants.OC_RSRVD_ES_DEVNAME);
                    }
                } catch (OcException e) {
                    Log.e(TAG, "getWiFiModes is failed: " + e.toString());
                }
            }
        }
        return null;
    }

    /**
     * Get Model Number property in DevConf resource
     *
     * @return modelNumber
     */
    public String getModelNumber()
    {
        if(mEasySetupRep == null)
        {
            return null;
        }

        List<OcRepresentation> children = mEasySetupRep.getChildren();

        for (OcRepresentation child : children) {
            if(child.getUri().indexOf(ESConstants.OC_RSRVD_ES_URI_DEVCONF) != -1)
            {
                try
                {
                    OcRepresentation rep;
                    if(child.hasAttribute(ESConstants.OC_RSRVD_REPRESENTATION))
                    {
                        rep = (OcRepresentation)child.getValue(ESConstants.OC_RSRVD_REPRESENTATION);
                    }
                    else
                    {
                        return null;
                    }

                    if(rep.hasAttribute(ESConstants.OC_RSRVD_ES_MODELNUMBER)) {
                        return (String) rep.getValue(ESConstants.OC_RSRVD_ES_MODELNUMBER);
                    }
                } catch (OcException e) {
                    Log.e(TAG, "getModelNumber is failed: " + e.toString());
                }
            }
        }
        return null;
    }

    /**
     * Get Supported WiFi Modes property in WiFi resource
     *
     * @return a list of WiFi modes
     */
    public ArrayList<WIFI_MODE> getWiFiModes()
    {
        if(mEasySetupRep == null)
        {
            return null;
        }

        List<OcRepresentation> children = mEasySetupRep.getChildren();
        ArrayList<WIFI_MODE> modes = new ArrayList<WIFI_MODE>();
        for (OcRepresentation child : children) {
            if(child.getUri().indexOf(ESConstants.OC_RSRVD_ES_URI_WIFICONF) != -1)
            {
                try {
                    OcRepresentation rep;
                    if(child.hasAttribute(ESConstants.OC_RSRVD_REPRESENTATION))
                    {
                        rep = (OcRepresentation)child.getValue(ESConstants.OC_RSRVD_REPRESENTATION);
                    }
                    else
                    {
                        return null;
                    }

                    if (rep.hasAttribute(ESConstants.OC_RSRVD_ES_SUPPORTEDWIFIMODE)) {
                        // Response from OIC Server does not contain spec version in header, so assumed as 0.
                        if(0 == mSpecVersion) {
                            // Considering Representation as per OIC Core Spec.
                            int modes_int[] = rep.getValue(ESConstants.OC_RSRVD_ES_SUPPORTEDWIFIMODE);
                            for (int i = 0 ; i < modes_int.length ; ++i) {
                                modes.add(WIFI_MODE.fromInt(modes_int[i]));
                            }
                        }
                        else {
                            // Considering Representation as per OCF 1.0 / 1.x Core Spec.
                            String modes_str[] = rep.getValue(ESConstants.OC_RSRVD_ES_SUPPORTEDWIFIMODE);
                            for (int i = 0 ; i < modes_str.length ; ++i) {
                                for(WIFI_MODE mode : WIFI_MODE.values()) {
                                    if(mode.name().contains(modes_str[i])) {
                                       modes.add(mode);
                                    }
                                }
                            }
                        }
                    }
                } catch (OcException e) {
                    Log.e(TAG, "getWiFiModes is failed: " + e.toString());
                }
            }
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
        if(mEasySetupRep == null)
        {
            return WIFI_FREQ.WIFI_FREQ_NONE;
        }

        List<OcRepresentation> children = mEasySetupRep.getChildren();

        for (OcRepresentation child : children) {
            if(child.getUri().indexOf(ESConstants.OC_RSRVD_ES_URI_WIFICONF) != -1)
            {
                try{
                    OcRepresentation rep;
                    if(child.hasAttribute(ESConstants.OC_RSRVD_REPRESENTATION))
                    {
                        rep = (OcRepresentation)child.getValue(ESConstants.OC_RSRVD_REPRESENTATION);
                    }
                    else
                    {
                        return null;
                    }

                    if(rep.hasAttribute(ESConstants.OC_RSRVD_ES_SUPPORTEDWIFIFREQ)) {
                        // Response from OIC Server does not contain spec version in header, so assumed as 0.
                        if(0 == mSpecVersion) {
                            // Considering Representation as per OIC Core Spec.
                            return WIFI_FREQ.fromInt(
                                    (int)rep.getValue(ESConstants.OC_RSRVD_ES_SUPPORTEDWIFIFREQ));
                        }
                        else {
                            // Considering Representation as per OCF 1.0 / 1.x Core Spec.
                            String freqs[] = rep.getValue(ESConstants.OC_RSRVD_ES_SUPPORTEDWIFIFREQ);
                            return getWiFiFreqAsEnum(freqs);
                        }
                    }
                } catch (OcException e) {
                    Log.e(TAG, "getWiFiFreq is failed: " + e.toString());
                }
            }
        }
        return null;
    }

    /**
     * To check if Enrollee can access to cloud. To decide its preference, we check if a cloudserver
     * resource is registered on Enrollee.
     *
     * @return boolean
     */
    public boolean isCloudAccessible()
    {
        if(mEasySetupRep == null)
        {
            return false;
        }

        List<OcRepresentation> children = mEasySetupRep.getChildren();

        for (OcRepresentation child : children) {
            if(child.getUri().indexOf(ESConstants.OC_RSRVD_ES_URI_COAPCLOUDCONF) != -1)
            {
                return true;
            }
        }
        return false;
    }

    public  OcRepresentation getEasySetupRep()
    {
        return mEasySetupRep;
    }

    private WIFI_FREQ getWiFiFreqAsEnum(String[] freqs)
    {
        WIFI_FREQ freq_res = WIFI_FREQ.WIFI_FREQ_NONE;
        boolean b_2_4G = false;
        boolean b_5G = false;
        for (String freq : freqs)
        {
            if(freq.equals("2.4G"))
            {
                b_2_4G = true;
            }
            else if(freq.equals("5G"))
            {
                b_5G = true;
            }
        }

        if(b_2_4G && b_5G) {
            freq_res = WIFI_FREQ.WIFI_BOTH;
        }
        else if(b_2_4G) {
            freq_res = WIFI_FREQ.WIFI_24G;
        }
        else if(b_5G) {
            freq_res = WIFI_FREQ.WIFI_5G;
        }
        else {
            freq_res = WIFI_FREQ.WIFI_FREQ_NONE;
        }

        return freq_res;
    }
}

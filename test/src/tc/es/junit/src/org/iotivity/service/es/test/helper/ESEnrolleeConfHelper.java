
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

package org.iotivity.service.es.test.helper;

import org.iotivity.base.OcException;
import org.iotivity.base.OcRepresentation;
import org.iotivity.service.easysetup.mediator.ESConstants;
import org.iotivity.service.easysetup.mediator.enums.WIFI_FREQ;
import org.iotivity.service.easysetup.mediator.enums.WIFI_MODE;

public class ESEnrolleeConfHelper {

    public static final String DEVICE_NAME   = "9E09F4FE-978A-4BC3-B356-1F93BCA37829";
    public static final String REFRESH_TOKEN = "refreshtoken";
    public static final String USER_ID       = "samsung";
    public static final int    WIFI_MODES[]  = { WIFI_MODE.WIFI_11A.getValue(),
            WIFI_MODE.WIFI_11AC.getValue() };

    public static OcRepresentation cretaeOcRepresentation() throws OcException {
        OcRepresentation ocRepresentation = new OcRepresentation();
        OcRepresentation mDevConfRep = new OcRepresentation();
        OcRepresentation mWiFiRep = new OcRepresentation();
        OcRepresentation mCloudRep = new OcRepresentation();
        mDevConfRep.setUri(ESConstants.OC_RSRVD_ES_URI_DEVCONF);
        mWiFiRep.setUri(ESConstants.OC_RSRVD_ES_URI_WIFI);
        mCloudRep.setUri(ESConstants.OC_RSRVD_ES_URI_CLOUDSERVER);
        mDevConfRep.setValue(ESConstants.OC_RSRVD_ES_DEVNAME, DEVICE_NAME);
        mWiFiRep.setValue(ESConstants.OC_RSRVD_ES_SUPPORTEDWIFIMODE,
                WIFI_MODES);
        mWiFiRep.setValue(ESConstants.OC_RSRVD_ES_SUPPORTEDWIFIFREQ,
                (int) WIFI_FREQ.WIFI_5G.getValue());
        ocRepresentation.addChild(mDevConfRep);
        ocRepresentation.addChild(mWiFiRep);
        ocRepresentation.addChild(mCloudRep);
        return ocRepresentation;
    }
}
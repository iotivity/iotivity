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

import org.iotivity.service.easysetup.mediator.CloudProp;
import org.iotivity.service.easysetup.mediator.DeviceProp;
import org.iotivity.service.easysetup.mediator.enums.WIFI_AUTHTYPE;
import org.iotivity.service.easysetup.mediator.enums.WIFI_ENCTYPE;

public class ESPropertiesHelper {

    private final static String ENROLLEE_SS_ID    = "Iotivity_SSID";
    private final static String ENROLLEE_PASSWORD = "Iotivity_PWD";
    private final static String LANGUAGE          = "English";
    private final static String COUNTRY           = "America";
    private final static String AUTH_CODE         = "12";
    private final static String AUTH_PROVIDER     = "github";
    private final static String CI_SERVER         = "CI Server";

    public static DeviceProp createDeviceProperties() {
        DeviceProp deviceProp = new DeviceProp();
        deviceProp.setWiFiProp(ENROLLEE_SS_ID, ENROLLEE_PASSWORD,
                WIFI_AUTHTYPE.WPA2_PSK, WIFI_ENCTYPE.TKIP_AES);
        deviceProp.setDevConfProp(LANGUAGE, COUNTRY);
        return deviceProp;
    }

    public static CloudProp createCloudProperties() {
        CloudProp cloudProp = new CloudProp();
        cloudProp.setCloudProp(AUTH_CODE, AUTH_PROVIDER, CI_SERVER);
        return cloudProp;
    }
}
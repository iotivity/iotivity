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

    public final static String ENROLLEE_SS_ID    = "Iotivity_SSID";
    public final static String ENROLLEE_PASSWORD = "Iotivity_PWD";
    public final static String LANGUAGE          = "Bangla";
    public final static String COUNTRY           = "Bangladesh";
    public final static String LOCATION           = "Dhaka";
    public final static String AUTH_CODE         = "12";
    public final static String AUTH_PROVIDER     = "github";
    public final static String CI_SERVER         = "CI Server";
    public final static String CLOUD_ID        = "f002ae8b-c42c-40d3-8b8d-1927c17bd1b3";
    public final static int CRED_ID        =  1;


    public static DeviceProp createDeviceProperties() {
        DeviceProp deviceProp = new DeviceProp();
        deviceProp.setWiFiProp(ENROLLEE_SS_ID, ENROLLEE_PASSWORD,
                WIFI_AUTHTYPE.WPA2_PSK, WIFI_ENCTYPE.TKIP_AES);
        deviceProp.setDevConfProp(LANGUAGE, COUNTRY, LANGUAGE);
        return deviceProp;
    }

    public static CloudProp createCloudProperties() {
        CloudProp cloudProp = new CloudProp();
        cloudProp.setCloudProp(AUTH_CODE, AUTH_PROVIDER, CI_SERVER);
        cloudProp.setCloudID(CLOUD_ID);
        cloudProp.setCredID(CRED_ID);
        
        return cloudProp;
    }
}

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

import org.iotivity.service.easysetup.mediator.EnrolleeConf;
import org.iotivity.service.easysetup.mediator.GetConfigurationCallback;
import org.iotivity.service.easysetup.mediator.GetConfigurationStatus;
import org.iotivity.service.easysetup.mediator.enums.ESResult;

import android.util.Log;

public class ESGetConfigurationCallback extends GetConfigurationCallback {

    private static final String FREQUENCY   = "Frequency : ";
    private static final String WI_FI_MODES = "WiFi Modes : ";
    private static final String DEVICE_NAME = "Device Name : ";
    private static final String TAG         = "GetConfigurationCallback";
    public static boolean       isConfigurationCallbackSuccess;
    public static boolean       isConfigurationCallbackCalled;

    @Override
    public void onProgress(GetConfigurationStatus getConfigurationStatus) {
        isConfigurationCallbackCalled = true;
        if (getConfigurationStatus.getESResult() == ESResult.ES_OK) {
            final EnrolleeConf enrolleeConf = getConfigurationStatus
                    .getEnrolleeConf();
            Log.d(TAG, DEVICE_NAME + enrolleeConf.getDeviceName());
            Log.d(TAG, WI_FI_MODES + enrolleeConf.getWiFiModes());
            Log.d(TAG, FREQUENCY + enrolleeConf.getWiFiFreq());
            isConfigurationCallbackSuccess = true;
        }
    }
}
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

import org.iotivity.service.easysetup.mediator.CloudPropProvisioningCallback;
import org.iotivity.service.easysetup.mediator.CloudPropProvisioningStatus;
import org.iotivity.service.easysetup.mediator.enums.ESCloudProvState;
import org.iotivity.service.easysetup.mediator.enums.ESResult;

import android.util.Log;

public class ESCloudPropProvCallback extends CloudPropProvisioningCallback {

    public static boolean       isCloudPropSuccess            = false;
    public static boolean       isCloudPropProvCallbackCalled = false;
    public static ESResult      eSResult;
    public static boolean       isEnrolleeFound;
    private static final int    TIME_OUT                      = 10;
    private static final long   WAITING_TIME_IN_SECOND        = 1000;
    private static final String TAG                           = "DevicePropProvisioningStatus";
    public static boolean       isProvisionDevicePropSuccess;
    public static boolean       isDevicePropProvCallbackCalled;

    public static void waitUntilProvSuccess(boolean callbackStatus) {
        int count = 0;
        while (!callbackStatus) {

            Log.d(TAG, "\nWaiting For Provisioning..........\n");

            if (count++ == TIME_OUT) {
                Log.d(TAG, "Timeout for Provisioning response");
                break;
            }

            try {
                Thread.sleep(WAITING_TIME_IN_SECOND);
            } catch (InterruptedException ex) {
                Log.d(TAG, ex.getMessage());
            }
        }
    }

    @Override
    public void onProgress(CloudPropProvisioningStatus status) {
        if (status != null) {
            ESResult result = status.getESResult();
            if (!result.equals(ESResult.ES_ENROLLEE_DISCOVERY_FAILURE)) {
                isEnrolleeFound = true;
                Log.d(TAG, ESResult.ES_ENROLLEE_DISCOVERY_FAILURE.toString());

            } else if (result.equals(ESResult.ES_ENROLLEE_DISCOVERY_FAILURE)) {
                Log.d(TAG, ESResult.ES_ENROLLEE_DISCOVERY_FAILURE.toString());

            } else if (result.equals(ESResult.ES_OK)) {
                isCloudPropSuccess = true;
                Log.d(TAG, ESResult.ES_OK.toString());

            } else {
                Log.d(TAG, "Cloud Provisioning fails");
            }
        }
        isCloudPropProvCallbackCalled = true;
    }

    public static void initialCallbackValuse() {
        isCloudPropProvCallbackCalled = false;
        isCloudPropSuccess = false;
        isEnrolleeFound = false;
    }
}

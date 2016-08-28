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

import org.iotivity.service.easysetup.mediator.DevicePropProvisioningCallback;
import org.iotivity.service.easysetup.mediator.DevicePropProvisioningStatus;
import org.iotivity.service.easysetup.mediator.enums.ESResult;

import android.util.Log;

public class ESDevicePropProvCallback extends DevicePropProvisioningCallback {

    private static final String TAG = "DevicePropProvisioningStatus";
    public static boolean       isProvisionDevicePropSuccess;
    public static boolean       isDevicePropProvCallbackCalled;
    public static ESResult      eSResult;

    @Override
    public void onProgress(
            DevicePropProvisioningStatus devPropProvisioningStatus) {
        final ESResult result = devPropProvisioningStatus.getESResult();

        if (result.equals(ESResult.ES_OK)) {
            isProvisionDevicePropSuccess = true;
            eSResult = ESResult.ES_OK;
            Log.d(TAG, eSResult.toString());
        }

        else if (result.equals(ESResult.ES_ERROR)) {
            eSResult = ESResult.ES_ERROR;
            Log.d(TAG, eSResult.toString());
        } else if (result.equals(ESResult.ES_UNAUTHORIZED)) {
            eSResult = ESResult.ES_UNAUTHORIZED;
            Log.d(TAG, eSResult.toString());

        } else if (result.equals(ESResult.ES_NETWORKFOUND)) {
            eSResult = ESResult.ES_NETWORKFOUND;
            Log.d(TAG, eSResult.toString());

        } else if (result.equals(ESResult.ES_NETWORKNOTCONNECTED)) {
            eSResult = ESResult.ES_NETWORKNOTCONNECTED;
            Log.d(TAG, eSResult.toString());

        } else if (result.equals(ESResult.ES_RESOURCECREATED)) {
            eSResult = ESResult.ES_RESOURCECREATED;
            Log.d(TAG, eSResult.toString());

        } else if (result.equals(ESResult.ES_RECVREQOFPROVRES)) {
            eSResult = ESResult.ES_RECVREQOFPROVRES;
            Log.d(TAG, eSResult.toString());

        } else if (result.equals(ESResult.ES_RECVUPDATEOFPROVRES)) {
            eSResult = ESResult.ES_RECVUPDATEOFPROVRES;
            Log.d(TAG, eSResult.toString());

        } else if (result.equals(ESResult.ES_RECVTRIGGEROFPROVRES)) {
            eSResult = ESResult.ES_RECVTRIGGEROFPROVRES;
            Log.d(TAG, eSResult.toString());

        }
        isDevicePropProvCallbackCalled = true;
    }

    public static void initialCallbackValuse() {
        isDevicePropProvCallbackCalled = false;
        isProvisionDevicePropSuccess = false;
        eSResult = null;
        ESGetConfigurationCallback.isConfigurationCallbackCalled = false;
        ESGetConfigurationCallback.isConfigurationCallbackSuccess = false;
    }
}
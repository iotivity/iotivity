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

import org.iotivity.service.easysetup.mediator.SecurityProvisioningCallback;
import org.iotivity.service.easysetup.mediator.SecurityProvisioningStatus;
import org.iotivity.service.easysetup.mediator.enums.ESResult;

import android.util.Log;

public class ESSecurityProvisioningCallback
        extends SecurityProvisioningCallback {

    private static final String TAG = "SecurityProvisioningCallback";
    public static boolean       isSecurityProvPropSuccess;
    public static boolean       isSecurityProvCallbackCalled;
    public static ESResult      eSResult;

    @Override
    public void onProgress(SecurityProvisioningStatus status) {

        if (status.getESResult() == ESResult.ES_OK) {
            Log.d(TAG, status.getDevUUID());
            isSecurityProvPropSuccess = true;
        }
        isSecurityProvCallbackCalled = true;
    }

    public static void initialCallbackValuse() {
        isSecurityProvCallbackCalled = false;
        isSecurityProvPropSuccess = false;
    }
}

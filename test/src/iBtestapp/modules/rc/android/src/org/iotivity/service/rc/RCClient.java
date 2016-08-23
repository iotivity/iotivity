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

package org.iotivity.service.rc;

import org.iotivity.base.OcConnectivityType;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcException;

import android.util.Log;

import java.util.EnumSet;

import static org.iotivity.service.rc.RCTestAppUtilityFunctions.*;
import static org.iotivity.service.rc.RCTestAppStaticUtil.*;
import org.iotivity.service.rc.RCClientFindResourceAdapter;

import static org.iotivity.service.testapp.framework.Base.*;

public class RCClient {

    public static boolean isServerResponse = false;
    public static boolean isResourceFound  = false;

    RCClient() {

    }

    public void rcObserveResourceContainer() {
        RCClientFindResourceAdapter rcClientFindResourceAdapter = new RCClientFindResourceAdapter();

        try {
            isServerResponse = false;
            isResourceFound = false;

            showOutPut("Finding resources.Please wait...");
            OcPlatform.findResource("", OcPlatform.WELL_KNOWN_QUERY,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT),
                    rcClientFindResourceAdapter);

            waitInSecond(MAX_WAIT_IN_CALLBACK);

            if (!isServerResponse) {
                Log.d(TAG, "onResourceFoundListener Callback is not invoked...");
                showOutPut("onResourceFoundListener Callback is not invoked...");
            }

            if (isServerResponse && !isResourceFound)
                showOutPut("Sorry.No resource has been found...");

            if (isServerResponse && isResourceFound)
                showOutPut("Found resource operation has been completed successfully...");

        } catch (OcException ex) {
            showOutPut("OcException is occurred: " + ex.getMessage());
        }
    }

}

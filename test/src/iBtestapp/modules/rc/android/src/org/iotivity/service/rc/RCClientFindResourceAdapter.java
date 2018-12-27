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

import android.util.Log;
import org.iotivity.base.OcResource;
import org.iotivity.base.OcPlatform;
import org.iotivity.service.rc.RCClient;

import static org.iotivity.service.rc.RCTestAppStaticUtil.*;
import static org.iotivity.service.testapp.framework.Base.*;

public class RCClientFindResourceAdapter implements OcPlatform.OnResourceFoundListener {

    private static OcResource rcFoundResource;

    @Override
    public void onResourceFound(OcResource resource) {
        Log.d(TAG,
                "onResourceFound callback is invoked.");
        RCClient.isServerResponse = true;
        if (resource != null) {
            rcFoundResource = resource;
            try {
                String foundResourceDetailes = "";
                for (String resourceType : rcFoundResource
                        .getResourceTypes()) {
                    if ((resourceType
                            .compareTo(HUE_BUNDLE_RESOURCE_TYPE) == 0)
                            || (resourceType
                                    .compareTo(BMI_BUNDLE_RESOURCE_TYPE) == 0)
                            || (resourceType
                                    .compareTo(DI_RESOURCE_TYPE) == 0)
                            || (resourceType
                                    .compareTo(TEMPERATURE_SENSOR_TYPE) == 0)
                            || (resourceType
                                    .compareTo(HUMIDITY_SENSOR_TYPE) == 0)) {
                        RCClient.isResourceFound = true;
                        foundResourceDetailes = "\t Founded resource type : "
                                + resourceType
                                + "\n";
                        foundResourceDetailes = foundResourceDetailes
                                + "\t URI of the founded resource : "
                                + rcFoundResource
                                        .getUri()
                                + "\n";
                        foundResourceDetailes = foundResourceDetailes
                                + "\t Host address of the resource: "
                                + rcFoundResource
                                        .getHost()
                                + "\n";
                        showOutPut(foundResourceDetailes);
                    }
                }
            } catch (Exception ex) {
                ex.printStackTrace();
                Log.d(TAG,
                        ex.getLocalizedMessage()
                                .toString());
            }
        } else {
            showOutPut("Founded resource is invalid..");
        }        
    }
}

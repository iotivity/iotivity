//******************************************************************
//
// Copyright 2015 Duseok Kang (Seoul National University) All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

package org.iotivity.service.sample.container;

import android.content.Context;
import android.util.Log;

import org.iotivity.service.resourcecontainer.AndroidBundleResource;
import org.iotivity.service.resourcecontainer.RcsResourceAttributes;

/**
 * Created by duseok on 15. 12. 13.
 */
public class TrackingResource extends AndroidBundleResource {
    private final String LOG_TAG = "[" + MainActivity.class.getName() + "]";

    public TrackingResource(Context applicationContext) {
        super(applicationContext);

        this.setResourceType("oic.r.trackingsensor");
//        this.setName("TrackingSensor1");
    }

    @Override
    protected void initAttributes() {
        Log.i(LOG_TAG, "init Attributes.");

        this.m_attributes.put("accel_x", 0);
        this.m_attributes.put("accel_y", 0);
        this.m_attributes.put("accel_z", 0);

        this.m_attributes.put("linearaccel_x", 0);
        this.m_attributes.put("linearaccel_y", 0);
        this.m_attributes.put("linearaccel_z", 0);

        this.m_attributes.put("mag_x", 0);
        this.m_attributes.put("mag_y", 0);
        this.m_attributes.put("mag_z", 0);

        this.m_attributes.put("wifi_rssi", 0);
        this.m_attributes.put("wifi_frequency", 0);

        this.m_attributes.put("loc_x", 0);
        this.m_attributes.put("loc_y", 0);
    }

    @Override
    public void handleSetAttributesRequest(RcsResourceAttributes rcsResourceAttributes) {
        Log.i(LOG_TAG, "Set Attributes.");
    }

    @Override
    public RcsResourceAttributes handleGetAttributesRequest() {
        return this.m_attributes;
    }
}

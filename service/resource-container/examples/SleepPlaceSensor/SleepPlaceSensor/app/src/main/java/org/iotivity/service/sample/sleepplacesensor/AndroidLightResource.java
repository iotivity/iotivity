//******************************************************************
//
// Copyright 2015 Euiseok Kim (Seoul National University) All Rights Reserved.
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


package org.iotivity.service.sample.sleepplacesensor;

import android.content.Context;
        import android.util.Log;

        import org.iotivity.service.resourcecontainer.AndroidBundleResource;
        import org.iotivity.service.resourcecontainer.RcsResourceAttributes;

/**
 * Created by chjej202 on 15. 12. 13.
 */
public class AndroidLightResource extends AndroidBundleResource {
    private static final String LOG_TAG = AndroidLightResource.class.getSimpleName();
    public AndroidLightResource(Context context){
        super(context);
        this.setResourceType("oic.r.light");
        this.setName("androidLightResource");
    }
    @Override
    protected void initAttributes() {
        this.m_attributes.put("on-off", false);
    }

    @Override
    public void handleSetAttributesRequest(RcsResourceAttributes attrs) {

        for(String key: attrs.keySet()){
            Log.i(LOG_TAG, " " + key + ": " + attrs.get(key));
        }

        Log.i(LOG_TAG, "Set Attributes called with " + attrs);

        Log.i(LOG_TAG, "On-off value: " + attrs.get("on-off"));
    }

    @Override
    public RcsResourceAttributes handleGetAttributesRequest() {
        Log.i(LOG_TAG, "Get Attributes called");
        Log.i(LOG_TAG, "Returning: ");
        for(String key: m_attributes.keySet()){
            Log.i(LOG_TAG, " " + key + ": " + m_attributes.get(key));
        }
        return this.m_attributes;
    }
}
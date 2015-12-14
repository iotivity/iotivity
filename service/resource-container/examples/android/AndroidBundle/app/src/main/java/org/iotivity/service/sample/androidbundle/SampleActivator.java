//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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


package org.iotivity.service.sample.androidbundle;

import android.content.Context;
import android.util.Log;

import org.iotivity.service.resourcecontainer.AndroidBundleActivator;
import org.iotivity.service.resourcecontainer.AndroidBundleResource;
import org.iotivity.service.resourcecontainer.RcsResourceContainerBundleAPI;
import org.iotivity.service.resourcecontainer.ResourceConfig;
import org.iotivity.service.sample.androidbundle.resources.DiscomfortIndexResource;
import org.iotivity.service.sample.androidbundle.resources.HumidityResource;
import org.iotivity.service.sample.androidbundle.resources.LightIntensityResource;
import org.iotivity.service.sample.androidbundle.resources.TemperatureResource;

import java.util.List;

/**
 * Created by markus.jung on 11/7/2015.
 */
public class SampleActivator extends AndroidBundleActivator {

    public SampleActivator(RcsResourceContainerBundleAPI bundleAPI, Context appContext){
        super(bundleAPI, appContext);
        Log.d(SampleActivator.class.getName(), "Created activator instance");
    }

    @Override
    public void activateBundle() {
        Log.d(SampleActivator.class.getName(), "Activate bundle called");

        LightIntensityResource lightIntensityResource = new LightIntensityResource(this.appContext);
        lightIntensityResource.setURI("/lightsensor/1");
        bundleAPI.registerResource("oic.android.sample", lightIntensityResource);

        HumidityResource humidityResource = new HumidityResource(this.appContext);
        humidityResource.setURI("/humidity/1");
        bundleAPI.registerResource("oic.android.sample", humidityResource);

        TemperatureResource tempResource = new TemperatureResource(this.appContext);
        tempResource.setURI("/temperature/1");
        bundleAPI.registerResource("oic.android.sample", tempResource);

        DiscomfortIndexResource diResource = new DiscomfortIndexResource(this.appContext);
        diResource.setURI("/android/discomfortindex/1");
        bundleAPI.registerResource("oic.android.sample", diResource);
    }

    @Override
    public void deactivateBundle() {

    }

    @Override
    public void createResource(ResourceConfig resourceConfig) {

    }

    @Override
    public void destroyResource(AndroidBundleResource androidBundleResource) {

    }

    @Override
    public List<ResourceConfig> getConfiguredBundleResources() {
        return null;
    }
}

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
import java.util.Vector;

public class SampleActivator extends AndroidBundleActivator {
    private List<AndroidBundleResource> resources = new Vector<AndroidBundleResource>();

    public SampleActivator(RcsResourceContainerBundleAPI bundleAPI, Context appContext){
        super(bundleAPI, appContext);
        Log.d(SampleActivator.class.getName(), "Created activator instance");
    }

    @Override
    public void activateBundle() {
        Log.d(SampleActivator.class.getName(), "Activate bundle called");

        List<ResourceConfig> configuredBundleResources = this.bundleAPI.
                getConfiguredBundleResources("oic.android.sample");

        for(ResourceConfig config : configuredBundleResources){
            AndroidBundleResource resource = null;

            if("oic.r.lightsensor".equals(config.getResourceType())){
                resource =
                        new LightIntensityResource(this.appContext);
            } else if("oic.r.temperature".equals(config.getResourceType())){
                resource =
                        new TemperatureResource(this.appContext);
            } else if("oic.r.discomfortindex".equals(config.getResourceType())){
                resource =
                        new DiscomfortIndexResource(this.appContext);
            } else if ("oic.r.humidity".equals(config.getResourceType())) {
                resource =
                        new HumidityResource(this.appContext);
            }

            if(resource != null) {
                resource.setURI(config.getURI());
                resource.setName(config.getName());
                bundleAPI.registerResource("oic.android.sample", resource);
                resources.add(resource);
            }
        }
    }

    @Override
    public void deactivateBundle() {
        for(AndroidBundleResource resource : resources){
            bundleAPI.unregisterResource(resource);
        }
    }

    @Override
    public void createResource(ResourceConfig config) {
        AndroidBundleResource resource = null;

        if("oic.r.lightsensor".equals(config.getResourceType())){
            resource =
                    new LightIntensityResource(this.appContext);
        } else if("oic.r.temperature".equals(config.getResourceType())){
            resource =
                    new TemperatureResource(this.appContext);
        } else if("oic.r.discomfortindex".equals(config.getResourceType())){
            resource =
                    new DiscomfortIndexResource(this.appContext);
        } else if ("oic.r.humidity".equals(config.getResourceType())) {
            resource =
                    new HumidityResource(this.appContext);
        }

        if(resource != null) {
            resource.setURI(config.getURI());
            resource.setName(config.getName());
            bundleAPI.registerResource("oic.android.sample", resource);
            resources.add(resource);
        }
    }

    @Override
    public void destroyResource(AndroidBundleResource androidBundleResource) {
        bundleAPI.unregisterResource(androidBundleResource);
    }
}

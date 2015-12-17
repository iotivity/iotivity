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

import org.iotivity.service.resourcecontainer.AndroidBundleActivator;
import org.iotivity.service.resourcecontainer.AndroidBundleResource;
import org.iotivity.service.resourcecontainer.RcsResourceContainerBundleAPI;
import org.iotivity.service.resourcecontainer.ResourceConfig;

import java.util.List;

/**
 * Created by ikess on 15. 12. 13.
 */
public class SleepPlaceSensorActivator extends AndroidBundleActivator {
    private LightSensor lightSensor;
    private EMFSensor emfSensor;
    private SoundSensor soundSensor;
    private SleepPlaceSensorResource sleepPlaceResource;

    public SleepPlaceSensorActivator(RcsResourceContainerBundleAPI bundleAPI, Context appContext){
        super(bundleAPI, appContext);
        Log.d(SleepPlaceSensorActivator.class.getName(), "Created activator instance");
    }

    @Override
    public void activateBundle() {
        int numberOfConfiguredResources = 0;
        Log.d(SleepPlaceSensorActivator.class.getName(), "Activate bundle called");

        numberOfConfiguredResources = bundleAPI.getNumberOfConfiguredResources("oic.bundle.sleepplacesensor");

        // make a test resource
        lightSensor = new LightSensor(this.appContext);
        lightSensor.setURI("/android/lightsensor/1");
        lightSensor.startListener();
        bundleAPI.registerResource("oic.bundle.sleepplacesensor", lightSensor);

        emfSensor = new EMFSensor(this.appContext);
        emfSensor.setURI("/android/emfsensor/1");
        emfSensor.startListener();
        //bundleAPI.getConfiguredResourceParams()
        bundleAPI.registerResource("oic.bundle.sleepplacesensor", emfSensor);

        soundSensor = new SoundSensor(this.appContext);
        soundSensor.setURI("/android/soundsensor/1");
        soundSensor.startListener();
        bundleAPI.registerResource("oic.bundle.sleepplacesensor", soundSensor);

        sleepPlaceResource = new SleepPlaceSensorResource(this.appContext);
        sleepPlaceResource.setURI("/android/sleepplacesensor/1");
        bundleAPI.registerResource("oic.bundle.sleepplacesensor", sleepPlaceResource);
    }

    @Override
    public void deactivateBundle() {
        if(lightSensor != null)
        {
            lightSensor.stopListener();
            bundleAPI.unregisterResource(lightSensor);
        }

        if(emfSensor != null)
        {
            emfSensor.stopListener();
            bundleAPI.unregisterResource(emfSensor);
        }
        if(soundSensor != null)
        {
            soundSensor.stopListener();
            bundleAPI.unregisterResource(soundSensor);
        }

        if(sleepPlaceResource != null)
        {
            bundleAPI.unregisterResource(sleepPlaceResource);
        }
    }

    @Override
    public void createResource(ResourceConfig resourceConfig) {
        lightSensor = null;
        emfSensor = null;
        soundSensor = null;
        sleepPlaceResource = null;
        Log.d(SleepPlaceSensorActivator.class.getName(), "Create Resource bundle called");
        //resourceConfig.
    }

    @Override
    public void destroyResource(AndroidBundleResource androidBundleResource) {
        Log.d(SleepPlaceSensorActivator.class.getName(), "Destroyed Resource bundle called");
    }

    @Override
    public List<ResourceConfig> getConfiguredBundleResources() {
        return null;
    }
}

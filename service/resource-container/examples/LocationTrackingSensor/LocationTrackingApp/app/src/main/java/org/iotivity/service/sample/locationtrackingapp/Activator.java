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

package org.iotivity.service.sample.locationtrackingapp;

import android.content.Context;
import android.util.Log;

import org.iotivity.service.resourcecontainer.AndroidBundleActivator;
import org.iotivity.service.resourcecontainer.AndroidBundleResource;
import org.iotivity.service.resourcecontainer.RcsResourceContainerBundleAPI;
import org.iotivity.service.resourcecontainer.ResourceConfig;

import java.util.List;

/**
 * Created by duseok on 15. 12. 13.
 */
public class Activator extends AndroidBundleActivator {
    private final String LOG = "[" + Activator.class.getName() + "]";

    public Activator(RcsResourceContainerBundleAPI bundleAPI, Context appContext){
        super(bundleAPI, appContext);
        Log.i(LOG, "Create activator instance");
    }

    @Override
    public void activateBundle() {
        Log.i(LOG, "Activate bundle called");

        AndroidAccelerationSensor accelSensor = new AndroidAccelerationSensor(this.appContext);
        accelSensor.setURI("/android/accel/1");
        bundleAPI.registerResource("oic.android.sample", accelSensor);

        AndroidLinearAccelerationSensor linearaccelSensor = new AndroidLinearAccelerationSensor(this.appContext);
        linearaccelSensor.setURI("/android/linearaccel/1");
        bundleAPI.registerResource("oic.android.sample", linearaccelSensor);

        AndroidMagneticSensor magneticSensor = new AndroidMagneticSensor(this.appContext);
        magneticSensor.setURI("/android/magnetic/1");
        bundleAPI.registerResource("oic.android.smaple", magneticSensor);

        AndroidWifiPowerSensor wifiPowerSensor = new AndroidWifiPowerSensor(this.appContext);
        wifiPowerSensor.setURI("/android/wifi/1");
        bundleAPI.registerResource("oic.android.sample", wifiPowerSensor);

        TrackingSensorResource trackingSensorResource = new TrackingSensorResource(this.appContext);
        trackingSensorResource.setURI("/android/trackingsensor/1");
        bundleAPI.registerResource("oic.android.sample", trackingSensorResource);
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

//******************************************************************
////
//// Copyright 2015 Susang Park and Jake Choi (Seoul National University) All Rights Reserved.
////
////-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
////
//// Licensed under the Apache License, Version 2.0 (the "License");
//// you may not use this file except in compliance with the License.
//// You may obtain a copy of the License at
////
////      http://www.apache.org/licenses/LICENSE-2.0
////
//// Unless required by applicable law or agreed to in writing, software
//// distributed under the License is distributed on an "AS IS" BASIS,
//// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//// See the License for the specific language governing permissions and
//// limitations under the License.
////
////-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
package org.iotivity.service.sample.activitysensor;
import android.content.Context;

import org.iotivity.service.resourcecontainer.AndroidBundleActivator;
import org.iotivity.service.resourcecontainer.AndroidBundleResource;
import org.iotivity.service.resourcecontainer.RcsResourceContainerBundleAPI;
import org.iotivity.service.resourcecontainer.ResourceConfig;

import java.util.List;

public class ActivitySensorActivator extends AndroidBundleActivator {

    public ActivitySensorActivator(RcsResourceContainerBundleAPI bundleAPI, Context appContext) {
        super(bundleAPI, appContext);;
    }

    @Override
    public void activateBundle() {

        // make a test resource
        Accelerometer accelerometer = new Accelerometer(this.appContext);
        accelerometer.setURI("/sensor/accelerometer/1");
        bundleAPI.registerResource("oic.android.sample", accelerometer);

        HeartrateSensor heartrateSensor = new HeartrateSensor(this.appContext);
        heartrateSensor.setURI("/sensor/heartrate/1");
        bundleAPI.registerResource("oic.android.sample", heartrateSensor);

        GpsSensor gpsSensor = new GpsSensor(this.appContext);
        gpsSensor.setURI("/sensor/gps/1");
        bundleAPI.registerResource("oic.android.sample", gpsSensor);

        ActivitySensor activitySensor = new ActivitySensor(this.appContext);
        activitySensor.setURI("/sensor/activity/1");
        bundleAPI.registerResource("oic.android.sample", activitySensor);

        bundleAPI.unregisterResource(activitySensor);
    }

    @Override
    public void deactivateBundle() {
    }

    @Override
    public void createResource(ResourceConfig rcsConfig) {

    }

    @Override
    public void destroyResource(AndroidBundleResource bundleRcs) {

    }

    @Override
    public List<ResourceConfig> getConfiguredBundleResources() {
        return null;
    }
}

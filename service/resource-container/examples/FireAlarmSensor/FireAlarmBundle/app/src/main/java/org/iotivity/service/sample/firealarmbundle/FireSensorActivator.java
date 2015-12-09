//******************************************************************
////
//// Copyright 2015 Selin Chun (Seoul National University) All Rights Reserved.
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

package org.iotivity.service.sample.firealarmbundle;


import android.content.Context;

import org.iotivity.service.resourcecontainer.AndroidBundleActivator;
import org.iotivity.service.resourcecontainer.AndroidBundleResource;
import org.iotivity.service.resourcecontainer.RcsResourceContainerBundleAPI;
import org.iotivity.service.resourcecontainer.ResourceConfig;

import java.util.List;

public class FireSensorActivator extends AndroidBundleActivator {
    //private static int rcsCnt = 1;

    public FireSensorActivator(RcsResourceContainerBundleAPI bundleAPI, Context appContext) {
        super(bundleAPI, appContext);;
    }

    @Override
    public void activateBundle() {

        // make a test resource
        HumiditySensor humidSensor = new HumiditySensor(this.appContext);
        humidSensor.setURI("/sensor/humid/1");
        bundleAPI.registerResource("oic.android.sample", humidSensor);

        TemperatureSensor temperatureSensor = new TemperatureSensor(this.appContext);
        temperatureSensor.setURI("/sensor/temperature/1");
        bundleAPI.registerResource("oic.android.sample", temperatureSensor);

        LightSensor lightSensor = new LightSensor(this.appContext);
        lightSensor.setURI("/sensor/light/1");
        bundleAPI.registerResource("oic.android.sample", lightSensor);

        RunningSensor runningSensor = new RunningSensor(this.appContext);
        runningSensor.setURI("/sensor/running/1");
        bundleAPI.registerResource("oic.android.sample", runningSensor);

        FireAlarmSensor fireSensor = new FireAlarmSensor(this.appContext);
        fireSensor.setURI("/sensor/fire/1");
        bundleAPI.registerResource("oic.android.sample", fireSensor);

        bundleAPI.unregisterResource(fireSensor);
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

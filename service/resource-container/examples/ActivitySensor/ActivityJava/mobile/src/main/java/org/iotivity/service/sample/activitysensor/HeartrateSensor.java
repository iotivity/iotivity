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
import android.util.Log;

import com.squareup.otto.Subscribe;

import org.iotivity.service.resourcecontainer.AndroidBundleResource;
import org.iotivity.service.resourcecontainer.RcsResourceAttributes;
import org.iotivity.service.resourcecontainer.RcsValue;
import org.iotivity.service.sample.activitysensor.WearableSensor.RemoteSensorManager;
import org.iotivity.service.sample.activitysensor.events.SensorUpdatedEvent;

import java.util.Arrays;

public class HeartrateSensor extends AndroidBundleResource{
    private static final String TAG = "HeartrateSensor";
    private RemoteSensorManager remoteSensorManager;

    public HeartrateSensor(Context context){
        //super(context);
        this.setResourceType("oic.r.heartratesensor");
        this.setName("heartrateSensor");

        remoteSensorManager = RemoteSensorManager.getInstance(context);
    }

    @Override
    protected void initAttributes() {}

    @Override
    public void handleSetAttributesRequest(RcsResourceAttributes value) {}

    @Override
    public RcsResourceAttributes handleGetAttributesRequest() {
        Log.i(TAG, "Get Attributes called and return: ");
        for (String key : m_attributes.keySet()) {
            Log.i(TAG, " " + key + ": " + m_attributes.get(key));
        }
        return this.m_attributes;
    }

    @Subscribe
    public void onSensorUpdatedEvent(SensorUpdatedEvent event) {
        if(event.getSensorType() == android.hardware.Sensor.TYPE_HEART_RATE) {
            Log.i(TAG, "Heartrate Sensor event: " + Arrays.toString(event.getValues()));

            float[] values = event.getValues();
            setAttribute("heartrate", new RcsValue(values[0]), true);
        }
    }
}

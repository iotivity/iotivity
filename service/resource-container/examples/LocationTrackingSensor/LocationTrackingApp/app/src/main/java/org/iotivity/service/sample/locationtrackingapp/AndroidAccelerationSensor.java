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
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.util.Log;

import org.iotivity.service.resourcecontainer.AndroidBundleResource;
import org.iotivity.service.resourcecontainer.RcsResourceAttributes;
import org.iotivity.service.resourcecontainer.RcsValue;

public class AndroidAccelerationSensor extends AndroidBundleResource implements SensorEventListener {
    private final String LOG_TAG = "[" + this.getName() + "]";
    private final SensorManager mSensorManager;
    private final Sensor accelSensor;

    public AndroidAccelerationSensor(Context appContext) {
        super(appContext);
        this.setResourceType("oic.r.acceleration");
        this.setName("androidAccelerationSensor");

        mSensorManager = (SensorManager) appContext.getApplicationContext().getSystemService(Context.SENSOR_SERVICE);
        accelSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        mSensorManager.registerListener(this, accelSensor, SensorManager.SENSOR_DELAY_NORMAL);
    }

    @Override
    protected void initAttributes() {
        this.m_attributes.put("accel_x", 0.0);
        this.m_attributes.put("accel_y", 0.0);
        this.m_attributes.put("accel_z", 0.0);
    }

    @Override
    public void handleSetAttributesRequest(RcsResourceAttributes rcsResourceAttributes) {
        Log.i(LOG_TAG, "Set Attribute called with ");
        for (String key : rcsResourceAttributes.keySet()) {
            Log.i(LOG_TAG, " " + key + " : " + rcsResourceAttributes.get(key));
        }
    }

    @Override
    public RcsResourceAttributes handleGetAttributesRequest() {
        Log.i(LOG_TAG, "Get Attributes called");
        Log.i(LOG_TAG, "Returning: ");
        for (String key : m_attributes.keySet()) {
            Log.i(LOG_TAG, " " + key + " : " + m_attributes.get(key));
        }
        return this.m_attributes;
    }

    @Override
    public void onSensorChanged(SensorEvent event) {
        setAttribute("accel_x", new RcsValue(event.values[0]), true);
        setAttribute("accel_y", new RcsValue(event.values[1]), true);
        setAttribute("accel_z", new RcsValue(event.values[2]), true);
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {

    }
}

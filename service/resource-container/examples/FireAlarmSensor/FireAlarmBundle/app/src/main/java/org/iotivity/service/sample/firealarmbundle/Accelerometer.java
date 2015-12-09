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
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.util.Log;

import org.iotivity.service.resourcecontainer.AndroidBundleResource;
import org.iotivity.service.resourcecontainer.RcsResourceAttributes;
import org.iotivity.service.resourcecontainer.RcsValue;


public class Accelerometer extends AndroidBundleResource implements SensorEventListener {
    private static final String LOG_TAG = Accelerometer.class.getSimpleName();
    private final SensorManager mSensorManager;
    private final Sensor accelerometer;

    public Accelerometer(Context context) {
        super(context);
        this.setResourceType("oic.r.accelerometer");
        this.setName("accelerometer");
        mSensorManager = (SensorManager) context.getApplicationContext().getSystemService(Context.SENSOR_SERVICE);
        accelerometer = mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        mSensorManager.registerListener(this, accelerometer, SensorManager.SENSOR_DELAY_NORMAL);
    }

    @Override
    protected void initAttributes() {

    }

    @Override
    public void handleSetAttributesRequest(RcsResourceAttributes attrs) {

    }

    @Override
    public RcsResourceAttributes handleGetAttributesRequest() {
        Log.i(LOG_TAG, "Get Attributes called and return: ");
        for (String key : m_attributes.keySet()) {
            Log.i(LOG_TAG, " " + key + ": " + m_attributes.get(key));
        }
        return this.m_attributes;
    }

    @Override
    public void onSensorChanged(SensorEvent sensorEvent) {
        Log.i(LOG_TAG, "Sensor event: " + sensorEvent.values[0]);

        double accel = Math.sqrt(Math.pow(sensorEvent.values[0], 2) + Math.pow(sensorEvent.values[1], 2) + Math.pow(sensorEvent.values[2], 2));
        setAttribute("acceleration", new RcsValue(accel), true);
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int i) {

    }
}

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

import org.iotivity.service.resourcecontainer.AndroidBundleResource;
import org.iotivity.service.resourcecontainer.RcsResourceAttributes;
import org.iotivity.service.resourcecontainer.RcsValue;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.util.Log;

/**
 * Created by ikess on 15. 12. 14.
 */
public class SoundSensor extends AndroidBundleResource implements SensorEventListener {
    private static final String LOG_TAG = SoundSensor.class.getSimpleName();
    private final SensorManager mSensorManager;
    private final Sensor soundSensor;   // temporal sensor to use onChanged()
    SoundMeter mSoundMeter = null;

    public SoundSensor(Context context){
        super(context);
        this.setResourceType("oic.r.sound-intensity");
        this.setName("SoundSensor");

        mSensorManager = (SensorManager) context.getApplicationContext().getSystemService(Context.SENSOR_SERVICE);
        soundSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_ALL);
        mSoundMeter = new SoundMeter();

    }

    public void startListener()
    {
        mSensorManager.registerListener(this, soundSensor, SensorManager.SENSOR_DELAY_NORMAL);
        mSoundMeter.start();
    }

    public void stopListener()
    {
        mSoundMeter.stop();
        mSensorManager.unregisterListener(this);
    }

    @Override
    protected void initAttributes() {
        this.m_attributes.put("sound-intensity", 0);
    }

    @Override
    public void handleSetAttributesRequest(RcsResourceAttributes attrs) {
        Log.i(LOG_TAG, "Set Attributes called with ");
        for(String key: attrs.keySet()){
            Log.i(LOG_TAG, " " + key + ": " + attrs.get(key));
        }
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

    @Override
    public void onSensorChanged(SensorEvent sensorEvent) {
        double sound = mSoundMeter.getAmplitude();
        Log.i(LOG_TAG, "soundSensor event " + sound);

        this.setAttribute("sound-intensity", new RcsValue((sound)), true);
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int i) {

    }
}

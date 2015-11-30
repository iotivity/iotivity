package org.iotivity.service.sample.androidbundle;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.util.Log;

import org.iotivity.service.resourcecontainer.AndroidBundleResource;
import org.iotivity.service.resourcecontainer.RcsResourceAttributes;
import org.iotivity.service.resourcecontainer.RcsValue;

/**
 * Created by markus.jung on 11/26/2015.
 */
public class LightSensor extends AndroidBundleResource implements SensorEventListener {
    private static final String LOG_TAG = AndroidLightResource.class.getSimpleName();
    private final SensorManager mSensorManager;
    private final Sensor lightSensor;

    public LightSensor(Context context){
        super(context);
        this.setResourceType("oic.r.lightsensor");
        this.setName("lightSensor");
        mSensorManager = (SensorManager) context.getApplicationContext().getSystemService(Context.SENSOR_SERVICE);
        lightSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_LIGHT);
        mSensorManager.registerListener(this, lightSensor, SensorManager.SENSOR_DELAY_NORMAL);
    }

    @Override
    protected void initAttributes() {
        this.m_attributes.put("intensity", 0);
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
        Log.i(LOG_TAG, "Sensor event " + sensorEvent.values[0]);

        setAttribute("intensity", new RcsValue( (int) (sensorEvent.values[0]) ) , true);
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int i) {

    }
}

package org.iotivity.service.sample.sleepplacesensor;

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
 * Created by ikess on 15. 12. 13.
 */
public class EMCSensor extends AndroidBundleResource implements SensorEventListener {
    private static final String LOG_TAG = EMCSensor.class.getSimpleName();
    private final SensorManager mSensorManager;
    private final Sensor emcSensor;

    public EMCSensor(Context context){
        super(context);
        this.setResourceType("oic.r.emcsensor");
        this.setName("emcSensor");
        mSensorManager = (SensorManager) context.getApplicationContext().getSystemService(Context.SENSOR_SERVICE);
        emcSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD);
        mSensorManager.registerListener(this, emcSensor, SensorManager.SENSOR_DELAY_NORMAL);
    }

    @Override
    protected void initAttributes() {
        this.m_attributes.put("emc-intensity", 0);
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

        double emf = Math.sqrt(sensorEvent.values[0] * sensorEvent.values[0] + sensorEvent.values[1] * sensorEvent.values[1] + sensorEvent.values[2] * sensorEvent.values[2]) * 10;
        Log.i(LOG_TAG, "Sensor event " + emf);
        setAttribute("emc-intensity", new RcsValue( emf ) , true);
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int i) {

    }
}

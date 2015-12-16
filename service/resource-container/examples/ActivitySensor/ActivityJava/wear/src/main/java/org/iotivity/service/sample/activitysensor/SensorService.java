package org.iotivity.service.sample.activitysensor;

import android.app.Notification;
import android.app.Service;
import android.content.Intent;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.IBinder;
import android.util.Log;

import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

public class SensorService extends Service implements SensorEventListener {
    private static final String TAG = "z/SensorService";

    private final static int SENS_ACCELEROMETER = Sensor.TYPE_ACCELEROMETER;
    private final static int SENS_HEARTRATE = Sensor.TYPE_HEART_RATE;

    SensorManager mSensorManager;

    private Sensor mHeartrateSensor;

    private DeviceClient client;

    @Override
    public void onCreate() {
        super.onCreate();

        client = DeviceClient.getInstance(this);

        Notification.Builder builder = new Notification.Builder(this);
        builder.setContentTitle("Activity Sensor");
        builder.setContentText("Collecting sensor data..");
        builder.setSmallIcon(R.drawable.ic_launcher);

        startForeground(1, builder.build());

        startMeasurement();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();

        stopMeasurement();
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    protected void startMeasurement() {
        mSensorManager = ((SensorManager) getSystemService(SENSOR_SERVICE));

        Sensor accelerometerSensor = mSensorManager.getDefaultSensor(SENS_ACCELEROMETER);
        mHeartrateSensor = mSensorManager.getDefaultSensor(SENS_HEARTRATE);


        // Register the listener
        if (mSensorManager != null) {
            if (accelerometerSensor != null) {
                mSensorManager.registerListener(this, accelerometerSensor, SensorManager.SENSOR_DELAY_NORMAL);
            } else {
                Log.w(TAG, "No Accelerometer found");
            }

            if (mHeartrateSensor != null) {
                mSensorManager.registerListener(SensorService.this, mHeartrateSensor, SensorManager.SENSOR_DELAY_NORMAL);
            } else {
                Log.d(TAG, "No Heartrate Sensor found");
            }
        }
    }

    private void stopMeasurement() {
        if (mSensorManager != null) {
            mSensorManager.unregisterListener(this);
        }
    }

    @Override
    public void onSensorChanged(SensorEvent event) {
        client.sendSensorData(event.sensor.getType(), event.accuracy, event.timestamp, event.values);
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {

    }
}

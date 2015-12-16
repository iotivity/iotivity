//******************************************************************
////
//// Copyright 2015 Susang Park and Jake Czhoi (Seoul National University) All Rights Reserved.
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

import android.app.Activity;
import android.os.Bundle;
import android.widget.TextView;

import com.squareup.otto.Subscribe;

import org.iotivity.service.sample.activitysensor.WearableSensor.RemoteSensorManager;
import org.iotivity.service.sample.activitysensor.events.BusProvider;
import org.iotivity.service.sample.activitysensor.events.SensorUpdatedEvent;

import java.util.Arrays;


public class MainActivity extends Activity {
    private RemoteSensorManager remoteSensorManager;

    private TextView heartrateValue;
    private TextView accelerometerValue;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        accelerometerValue = (TextView)findViewById(R.id.accelerometer_value);
        heartrateValue = (TextView)findViewById(R.id.heartrate_value);

        remoteSensorManager = RemoteSensorManager.getInstance(this);
    }

    @Override
    protected void onResume() {
        super.onResume();
        BusProvider.getInstance().register(this);

        remoteSensorManager.startMeasurement();
    }

    @Override
    protected void onPause() {
        super.onPause();
        BusProvider.getInstance().unregister(this);

        remoteSensorManager.stopMeasurement();
    }

    //sensing heart rate and accelerometer from smart watch
    @Subscribe
    public void onSensorUpdatedEvent(SensorUpdatedEvent event) {
        if(event.getSensorType() == android.hardware.Sensor.TYPE_HEART_RATE) {
            heartrateValue.setText(Arrays.toString(event.getValues()));
        }
        if(event.getSensorType() == android.hardware.Sensor.TYPE_ACCELEROMETER){
            accelerometerValue.setText(Arrays.toString(event.getValues()));
        }
    }
}
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

import org.iotivity.service.resourcecontainer.AndroidBundleSoftSensorResource;
import org.iotivity.service.resourcecontainer.RcsResourceAttributes;
import org.iotivity.service.resourcecontainer.RcsValue;

import java.util.Vector;

public class FireAlarmSensor extends AndroidBundleSoftSensorResource{
    private static final String LOG_TAG = FireAlarmSensor.class.getSimpleName();
    private static final int FIRE_BASE_TEMPERATURE = 60;
    private static final int FIRE_TEMPERATURE_LEVEL_DELTA = 100;

    public FireAlarmSensor(Context context) {
        super(context);
        this.setResourceType("oic.r.firealarmsensor");
        this.setName("firealarmSensor");
    }

    @Override
    protected void initAttributes() {
        this.m_attributes.put("fire-level", 0);
        this.m_attributes.put("humidity", 0);
        this.m_attributes.put("temperature", 0);
        this.m_attributes.put("light-intensity", 0);
        this.m_attributes.put("is-running", false);
    }


    @Override
    protected void onUpdatedInputResource(String attributeName, Vector<RcsValue> values) {
        if (attributeName == "is-running") {
            for (RcsValue val : values) {
                if (val.asBoolean()) {
                    m_mapInputData.put(attributeName, val);
                }
            }
            m_mapInputData.put(attributeName, new RcsValue(false));
        }
        else {
            double sum = 0, avg = 0;
            int idx = 0;

            for (RcsValue val : values) {
                sum += val.asDouble();
                idx++;
            }
            avg = sum / idx;

            m_mapInputData.put(attributeName, new RcsValue(avg));
        }
    }

    @Override
    protected void executeLogic() {
        // Use sensors for fire detection
        // TODO: Implement proper method to refelect sensors for fire detection. Currently, mechanism is based on simple sensing results combination

        if (m_mapInputData.get("humidity") == null || m_mapInputData.get("temperature") == null
                || m_mapInputData.get("light-intensity") == null || m_mapInputData.get("is-running") == null)
            return;

        double temperature, humidity, light_intensity;
        boolean is_running;
        int fire_level = 0;

        temperature = m_mapInputData.get("temperature").asDouble();
        humidity = m_mapInputData.get("humidity").asDouble();
        light_intensity = m_mapInputData.get("light-intensity").asDouble();
        is_running = m_mapInputData.get("is-running").asBoolean();

        if (temperature < FIRE_BASE_TEMPERATURE) {
            this.setAttribute("fire-level", new RcsValue(0));
        }
        else if (temperature >= FIRE_BASE_TEMPERATURE) {
            fire_level += 1;
        }
        else if (temperature > FIRE_BASE_TEMPERATURE + FIRE_TEMPERATURE_LEVEL_DELTA) {
            fire_level += 2;
        }
        else if (temperature > FIRE_BASE_TEMPERATURE + 2 * FIRE_TEMPERATURE_LEVEL_DELTA) {
            fire_level += 3;
        }

        // When humidity is below 10, more probable for fire
        if (humidity < 10) {
            fire_level += 1;
        }

        // TODO: Implement proper light sensing mechanism for fire detection
        /*
        if (light_intensity < ?) {
            fire_level += 1
        }
        */

        // To use running sensor for fire level increment 3 to 4, fix fire level to 3 when fire level is higher than that
        if (fire_level > 3)
            fire_level = 3;

        // Reflect running sensor to fire probability level
        if (fire_level > 1 && is_running) {
            fire_level += 1;
        }

        this.setAttribute("fire-level", new RcsValue(fire_level));
    }

    @Override
    public void handleSetAttributesRequest(RcsResourceAttributes attrs) {
        this.setAttributes(attrs);
        executeLogic();
    }

    @Override
    public RcsResourceAttributes handleGetAttributesRequest() {
        return this.getAttributes();
    }
}

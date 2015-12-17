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

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorManager;
import android.util.Log;

import org.iotivity.service.resourcecontainer.AndroidBundleSoftSensorResource;
import org.iotivity.service.resourcecontainer.RcsResourceAttributes;
import org.iotivity.service.resourcecontainer.RcsValue;

import java.util.Calendar;
import java.util.HashMap;
import java.util.Vector;

/**
 * Created by chjej202 on 15. 12. 13.
 */
public class SleepPlaceSensorResource extends AndroidBundleSoftSensorResource {
    private static final String LOG_TAG = SleepPlaceSensorResource.class.getSimpleName();

    public SleepPlaceSensorResource(Context context){
        super(context);
        this.setResourceType("oic.r.sleepplacesensor");
        this.setName("SleepPlaceSensor");
        this.setURI("/android/sleepplacesensor/1");

        m_mapInputData = new HashMap<String, RcsValue>();
    }

    @Override
    protected void initAttributes() {
        this.m_attributes.put("humidity", 0);
        this.m_attributes.put("temperature", 0);
        this.m_attributes.put("light-intensity", 0);
        this.m_attributes.put("emf-intensity", 0);
        this.m_attributes.put("sound-intensity", 0);
        this.m_attributes.put("sleepplaceindex", 0);
    }

    @Override
    protected void onUpdatedInputResource(String attributeName, Vector<RcsValue> values) {
        Log.i(LOG_TAG, "Update SleepPlaceSensor Call!");
        m_mapInputData.put(attributeName, values.get(0));
    }

    int calcScore(double n, double min, double max) {
        double deviation_from_optimal = Math.abs(n - min) / max * 100;
        return (int)Math.pow(deviation_from_optimal, 2);
    }

    @Override
    protected void executeLogic() {
        if(/*m_mapInputData.get("humidity") != null && m_mapInputData.get("temperature") != null &&*/
                m_mapInputData.get("light-intensity") != null &&
                m_mapInputData.get("emf-intensity") != null &&
                m_mapInputData.get("sound-intensity") != null) {
            double dDI = 0.0;
            int dSPI = 0;

            double t = 20;//m_mapInputData.get("temperature").asDouble();
            double h = 60;//m_mapInputData.get("humidity").asDouble();
            double l = m_mapInputData.get("light-intensity").asDouble();
            double e = m_mapInputData.get("emf-intensity").asDouble();
            double s = m_mapInputData.get("sound-intensity").asDouble();

            double MIN_t = 0, MAX_t = 40;
            double MIN_h = 60, MAX_h = 85;

            Calendar oCalendar = Calendar.getInstance();
            int month = oCalendar.get(Calendar.MONTH) + 1;

            if (month == 12 || month == 1 || month == 2)    // winter
                MIN_t = 15;
            else if (month >= 6 && month <= 8)  // summer
                MIN_t = 25;
            else
                MIN_t = 20;

            double MIN_l = 30, MAX_l = 100;
            double MIN_e = 200, MAX_e = 3000;
            //double MIN_s = 30, MAX_s = 90;

            int t_score = calcScore(t, MIN_t, MAX_t);
            int h_score = calcScore(h, MIN_h, MAX_h);
            int l_score = calcScore(l, MIN_l, MAX_l);
            int e_score = calcScore(e, MIN_e, MAX_e);
            //int s_score = calcScore(s, MIN_s, MAX_s);

            int SP_score = 100 - (int)Math.pow(t_score * h_score * l_score * e_score /** s_score*/, 1.0/5.0);

            this.setAttribute("temperature", new RcsValue(t));
            this.setAttribute("humidity", new RcsValue(h));
            this.setAttribute("light-intensity", new RcsValue(l));
            this.setAttribute("emf-intensity", new RcsValue(e));
            this.setAttribute("sound-intensity", new RcsValue(s));
            this.setAttribute("sleepplaceindex", new RcsValue(SP_score));

            Log.i(LOG_TAG, "SleepPlace Index: " + SP_score);

            // something to add
            if (SP_score > 90)
                Log.i(LOG_TAG, "SP result : best place!");
            else if (SP_score > 80)
                Log.i(LOG_TAG, "SP result : good place :)");
            else if (SP_score > 70)
                Log.i(LOG_TAG, "SP result : normal place");
            else
                Log.i(LOG_TAG, "SP result : WARNING! DO NOT sleep here!");
        }
    }

    @Override
    public void handleSetAttributesRequest(RcsResourceAttributes rcsResourceAttributes) {
        Log.i(LOG_TAG, "Set SleepPlaceSensor Call!");
        //this.setAttributes(rcsResourceAttributes);

        for (String key : rcsResourceAttributes.keySet()) {
            m_mapInputData.put(key, rcsResourceAttributes.get(key));
            m_attributes.put(key, rcsResourceAttributes.get(key));

        }
        executeLogic();
    }

    @Override
    public RcsResourceAttributes handleGetAttributesRequest() {
        return this.getAttributes();
    }

}

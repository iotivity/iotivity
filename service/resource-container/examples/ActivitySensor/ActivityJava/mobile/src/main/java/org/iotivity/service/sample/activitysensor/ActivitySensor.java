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

import org.iotivity.service.resourcecontainer.AndroidBundleSoftSensorResource;
import org.iotivity.service.resourcecontainer.RcsResourceAttributes;
import org.iotivity.service.resourcecontainer.RcsValue;

import java.util.Vector;

public class ActivitySensor extends AndroidBundleSoftSensorResource {
    private static final String TAG = "ActivitySensor";
    private static final int HIGH_HEART_RATE = 100;
    private static final int LOW_HEART_RATE = 50;
    private static final int THRESHOLD_ACCELERATION = 100;

    private static final int NORMAL = 1;
    private static final int EXERCISE = 2;
    private static final int ABNORMAL = 3;


    public ActivitySensor(Context context) {
        super(context);
        this.setResourceType("oic.r.activitysensor");
        this.setName("activitySensor");
    }

    @Override
    protected void initAttributes() {
        this.m_attributes.put("heartrate", 0);
        this.m_attributes.put("acceleration", 0);
        this.m_attributes.put("latitude", 0);
        this.m_attributes.put("longitude", 0);
    }

    @Override
    protected void onUpdatedInputResource(String attributeName, Vector<RcsValue> values) {
        double sum = 0, avg = 0;
        int idx = 0;

        for (RcsValue val : values) {
            sum += val.asDouble();
            idx++;
        }
        avg = sum / idx;

        m_mapInputData.put(attributeName, new RcsValue(avg));
    }

    @Override
    protected void executeLogic() {
        // Use sensors for fire detection
        // TODO: Implement proper method to refelect sensors for fire detection. Currently, mechanism is based on simple sensing results combination

        if (m_mapInputData.get("heartrate") == null || m_mapInputData.get("acceleration") == null)
            return;

        double heartrate, acceleration;
        int activity_level = 0;

        heartrate = m_mapInputData.get("heartrate").asDouble();
        acceleration = m_mapInputData.get("acceleration").asDouble();

        if(heartrate > LOW_HEART_RATE && heartrate < HIGH_HEART_RATE){
            activity_level = NORMAL;
        }else if(heartrate > HIGH_HEART_RATE && acceleration > THRESHOLD_ACCELERATION){
            activity_level = EXERCISE;
        }else{
            activity_level = ABNORMAL;
        }

        this.setAttribute("activity-level", new RcsValue(activity_level));
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

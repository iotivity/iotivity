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

public class RunningSensor extends AndroidBundleSoftSensorResource {
    private static final String LOG_TAG = RunningSensor.class.getSimpleName();
    private Vector<Double> window;
    private long last_update_ts;

    public RunningSensor(Context context) {
        super(context);
        this.setResourceType("oic.r.runningsensor");
        this.setName("runningSensor");
    }

    @Override
     protected void initAttributes() {
        this.m_attributes.put("is-running", false);
        this.m_attributes.put("acceleration", 0);

        last_update_ts = System.currentTimeMillis();
        window = new Vector<Double>();
    }

    @Override
    protected void onUpdatedInputResource(String attributeName, Vector<RcsValue> values) {
        // Calculate acceleration speed and add it to window
        for (RcsValue val : values) {
            window.add(val.asDouble());
        }
    }

    @Override
    protected void executeLogic() {
        // TODO: Change algorithm to use step detection
        // Currently, consider user is running when user's acceleration exceeds 20m/s^2 in 100ms time window

        boolean is_walk = false;
        long current_ts = System.currentTimeMillis();
        int delta_cnt = (int) Math.floor( (current_ts - last_update_ts) / 100 );

        delta_cnt = delta_cnt < 1 ? 1 : delta_cnt;

        for (double val : window) {
            if (val > 20 && delta_cnt == 1) {
                is_walk = true;
                window.clear();
                break;
            }
            else if (val > 20 && delta_cnt > 1) {
                delta_cnt--;
            }
        }

        this.setAttribute("is-running", new RcsValue(is_walk));
        last_update_ts = System.currentTimeMillis();
    }

    @Override
    public void handleSetAttributesRequest(RcsResourceAttributes attrs) {
        this.setAttributes(attrs);
        executeLogic();
    }

    public RcsResourceAttributes handleGetAttributesRequest() {
        return this.getAttributes();
    }
}

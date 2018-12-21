/*
 *******************************************************************
 *
 * Copyright 2018 Intel Corporation.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

package org.iotivity.base.examples;

import org.iotivity.base.OcException;

/**
 * Light
 *
 * This class represents a light resource
 */
public class Light {
    static public final String RESOURCE_TYPE = "oic.d.light";
    static public final String DEVICE_RESOURCE_TYPE = "oic.wk.d";

    private Switch switchRes;
    private Dimming dimmingRes;

    private String deviceName;

    public Light(String name, String uuid, boolean powerOn, int dimmingValue, LightControlPanel ui) {
        deviceName = name;

        switchRes = new Switch(uuid);
        switchRes.setValue(powerOn);
        switchRes.addObserver(ui);
        ui.addObserver(switchRes);
        OcfLightServer.msg("Created switch resource: " + switchRes);

        dimmingRes = new Dimming(uuid);
        dimmingRes.setDimmingValue(dimmingValue);
        dimmingRes.addObserver(ui);
        ui.addObserver(dimmingRes);
        OcfLightServer.msg("Created dimming resource: " + dimmingRes);

        OcfLightServer.msg("Created light resource: " + this);
    }

    public void unregister() {
        try {
            switchRes.unregisterResource();
        } catch (OcException e) {
            OcfLightServer.msgError("Failed to unregister " + switchRes.getResourceUri());
            e.printStackTrace();
        }
        try {
            dimmingRes.unregisterResource();
        } catch (OcException e) {
            OcfLightServer.msgError("Failed to unregister " + dimmingRes.getResourceUri());
            e.printStackTrace();
        }
    }

    @Override
    public String toString() {
        return "[" + deviceName + ", "
                   + Switch.VALUE_KEY + ": " + switchRes.getValue() + ", "
                   + Dimming.DIMMING_SETTING_KEY + ": " + dimmingRes.getDimmingValue() + "]";
    }
}

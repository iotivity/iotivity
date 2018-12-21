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
import org.iotivity.base.OcRepresentation;

/**
 * Dimming
 *
 * This class represents a dimming resource
 */
public class Dimming extends Resource implements LightControlDimmingObserver {
    static public final String RESOURCE_TYPE = "oic.r.light.dimming";
    static public final String RESOURCE_INTERFACE = "oic.if.a";

    static public final String DIMMING_SETTING_KEY = "dimmingSetting";

    private int dimmingValue;

    public Dimming(String uuid) {
        super("/ocf/dimming", RESOURCE_TYPE, RESOURCE_INTERFACE);
    }

    public void setOcRepresentation(OcRepresentation rep) {
        try {
            if (rep.hasAttribute(DIMMING_SETTING_KEY)) {
                dimmingValue = rep.getValue(DIMMING_SETTING_KEY);
                dimmingValue = Math.max(0, dimmingValue);
                dimmingValue = Math.min(100, dimmingValue);
            }
        } catch (OcException e) {
            OcfLightServer.msgError(e.toString());
            OcfLightServer.msgError("Failed to get representation values");
        }
    }

    public OcRepresentation getOcRepresentation() {
        OcRepresentation rep = new OcRepresentation();
        try {
            dimmingValue = Math.max(0, dimmingValue);
            dimmingValue = Math.min(100, dimmingValue);
            rep.setValue(DIMMING_SETTING_KEY, dimmingValue);
            rep.setValue(RT_KEY, this.getResourceTypeArray());
            rep.setValue(IF_KEY, this.getResourceInterfaceArray());
        } catch (OcException e) {
            OcfLightServer.msgError(e.toString());
            OcfLightServer.msgError("Failed to set representation values");
        }
        return rep;
    }

    public int getDimmingValue() {
        return dimmingValue;
    }

    public void setDimmingValue(int dimmingValue) {
        dimmingValue = Math.max(0, dimmingValue);
        dimmingValue = Math.min(100, dimmingValue);
        this.dimmingValue = dimmingValue;
    }

    @Override
    public void update(int dimmingValue) {
        setDimmingValue(dimmingValue);
        notifyObservers(null);
    }

    @Override
    public String toString() {
        return "[" + super.toString() + ", " + DIMMING_SETTING_KEY + ": " + dimmingValue + "]";
    }
}

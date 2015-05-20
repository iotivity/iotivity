/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 *
 *
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
 ******************************************************************/
package com.example.con_server;

import java.util.EnumSet;

import org.iotivity.base.OcException;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.ResourceProperty;

import android.util.Log;

// For creating & deleting the FactorySet Resource
public class FactorySetResource extends ConfigurationResource {
    private final String LOG_TAG = "[CON-SERVER]"
                                         + this.getClass().getSimpleName();

    // constructor
    public FactorySetResource() {
        Log.i(LOG_TAG, "FactorySetCollection: enter");

        configurationUri = "/factorySet"; // uri of the resource
        configurationTypes.clear();
        configurationTypes.add("factorySet");
        configurationRep.setUri(configurationUri);
        configurationRep.setResourceTypes(configurationTypes);
    }

    // for creating FactoryReset Resource
    public void createResource(OcPlatform.EntityHandler listener)
            throws OcException {
        Log.i(LOG_TAG, "createResource(Factory Set): enter");
        EnumSet<ResourceProperty> propertySet = EnumSet.of(
                ResourceProperty.DISCOVERABLE, ResourceProperty.OBSERVABLE);
        if (null == listener) {
            Log.i(LOG_TAG, "CallBack Should be binded");
            return;
        }

        // Register factoryset resource
        configurationHandle = OcPlatform.registerResource(configurationUri,
                configurationTypes.elementAt(0),
                configurationInterfaces.elementAt(0), listener, propertySet);
        if (null == configurationHandle) {
            Log.e(LOG_TAG, "registerResource failed!");
            return;
        }
        Log.i(LOG_TAG, "createResource (Factory Set): exit");
    }

    // getters and Setters Methods for FacoryReset Resource
    public void setFactorySetRepresentation(OcRepresentation rep) {
        Log.i(LOG_TAG, "setFactorySetRepresentation: enter");

        String loc;
        String st;
        String cur;
        String reg;
        loc = rep.getValueString("loc");
        st = rep.getValueString("st");
        cur = rep.getValueString("c");
        reg = rep.getValueString("r");
        if (null != loc) {
            location = loc;
            Log.i(LOG_TAG, "setConfigurationRepresentation: New value: "
                    + location);
        }
        if (null != st) {
            // systemTime = st;
            Log.i(LOG_TAG, "setConfigurationRepresentation: New value: "
                    + systemTime);
        }
        if (null != cur) {
            currency = cur;
            Log.i(LOG_TAG, "setConfigurationRepresentation: New value: "
                    + currency);
        }
        if (null != reg) {
            region = reg;
            Log.i(LOG_TAG, "setConfigurationRepresentation: New value: "
                    + region);
        }
        Log.i(LOG_TAG, "setFactorySetRepresentation: exit");
    }

    OcRepresentation getFactorySetRepresentation() {

        configurationRep.setValueString("loc", location);
        configurationRep.setValueString("st", systemTime);
        configurationRep.setValueString("c", currency);
        configurationRep.setValueString("r", region);
        return configurationRep;
    }

    public String getUri() {
        return configurationUri;
    }

    // For deletingFactorySet Resource
    public void deleteResource() {
        try {
            if (null != configurationHandle) {
                // Unregister the collection resource
                OcPlatform.unregisterResource(configurationHandle);
            }
        } catch (OcException e) {
            Log.e(LOG_TAG, "OcException occured! " + e.toString());
        }
    }
}

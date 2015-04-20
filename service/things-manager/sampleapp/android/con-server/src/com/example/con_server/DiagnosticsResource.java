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
import java.util.Vector;

import org.iotivity.base.OcException;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcResourceHandle;
import org.iotivity.base.ResourceProperty;

import android.util.Log;

//For creating/deleting the Diagnostics Resource
public class DiagnosticsResource {
    private final String     LOG_TAG               = "[CON-SERVER]"
                                                           + this.getClass()
                                                                   .getSimpleName();
    // diagnostics members
    private String           diagnosticsUri;
    private String           factoryReset;
    private String           reboot;
    private String           startCollection;
    private Vector<String>   diagnosticsTypes      = new Vector<String>();
    private Vector<String>   diagnosticsInterfaces = new Vector<String>();
    private OcResourceHandle diagnosticsHandle;
    private OcRepresentation diagnosticsRep        = new OcRepresentation();

    // constructor
    public DiagnosticsResource() {
        Log.i(LOG_TAG, "DiagnosticsCollection: enter");

        factoryReset = ConfigurationDefaultValues.defaultFactoryReset;
        reboot = ConfigurationDefaultValues.defaultReboot;
        startCollection = ConfigurationDefaultValues.defaultStartCollection;

        diagnosticsUri = ConfigurationDefaultValues.diagURIPrefix;
        diagnosticsTypes.add(ConfigurationDefaultValues.diagResourceTypePrefix);
        diagnosticsInterfaces.add(OcPlatform.DEFAULT_INTERFACE);
        diagnosticsRep.setValueString("fr", factoryReset);
        diagnosticsRep.setValueString("rb", reboot);
        diagnosticsRep.setValueString("ssc", startCollection);
        diagnosticsRep.setUri(diagnosticsUri);
        diagnosticsRep.setResourceTypes(diagnosticsTypes);
        diagnosticsRep.setResourceInterfaces(diagnosticsInterfaces);

    }

    // for creating Diagnostic Resource
    public void createResource(OcPlatform.EntityHandler listener)
            throws OcException {
        Log.i(LOG_TAG, "createResource(Diagnostics): enter");
        EnumSet<ResourceProperty> propertySet = EnumSet.of(
                ResourceProperty.DISCOVERABLE, ResourceProperty.OBSERVABLE);
        if (null == listener) {
            Log.i(LOG_TAG, "CallBack Should be binded");
            return;
        }

        // Register diagnostic resource
        diagnosticsHandle = OcPlatform.registerResource(diagnosticsUri,
                diagnosticsTypes.get(0), diagnosticsInterfaces.get(0),
                listener, propertySet);
        if (null == diagnosticsHandle) {
            Log.e(LOG_TAG, "registerResource failed!");
            return;
        }

        Thread thread = new Thread() {
            @Override
            public void run() {
                try {
                    while (true) {
                        // Put this thread for sleep for 1 sec.
                        // Sleep value can be changed as per the developer
                        // convenience.
                        Thread.sleep(1000);
                        if (reboot.equalsIgnoreCase("true")) {
                            Log.i(LOG_TAG, "Reboot will be soon...");
                            MainActivity mainActivityObj = MainActivity
                                    .getMainActivityObject();
                            if (null == mainActivityObj) {
                                Log.e(LOG_TAG,
                                        "Mainactivity object is invalid!");
                                return;
                            }
                            try {
                                mainActivityObj.runOnUiThread(new Runnable() {
                                    @Override
                                    public void run() {
                                        try {
                                            MainActivity.reboot();
                                        } catch (InterruptedException e) {
                                            e.printStackTrace();
                                        }
                                    }
                                });
                            } catch (Exception e) {
                                Log.e(LOG_TAG, "InterruptedException occured: "
                                        + e.toString());
                                continue;
                            }
                            reboot = ConfigurationDefaultValues.defaultReboot;
                        }
                        if (factoryReset.equalsIgnoreCase("true")) {
                            factoryReset = ConfigurationDefaultValues.defaultFactoryReset;
                            factoryReset();
                        }
                    }
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        };
        thread.start();
        Log.i(LOG_TAG, "createResource(Diagnostics): exit");
    }

    // getters and Setters Methods for diagnostics Resource
    public void setDiagnosticsRepresentation(OcRepresentation rep) {
        Log.i(LOG_TAG, "setDiagnosticsRepresentation: enter");

        String fr = rep.getValueString("fr");
        String rb = rep.getValueString("rb");
        String ssc = rep.getValueString("ssc");

        if (!(fr.equalsIgnoreCase(""))) {
            factoryReset = fr;
            Log.i(LOG_TAG,
                    "setConfigurationRepresentation: New value(FactoryReset): "
                            + fr);
        }
        if (!(rb.equalsIgnoreCase(""))) {
            reboot = rb;
            Log.i(LOG_TAG, "setDiagnosticsRepresentation: new value:(reboot) "
                    + rb);
        }

        if (!(ssc.equalsIgnoreCase(""))) {
            startCollection = ssc;
            Log.i(LOG_TAG,
                    "setDiagnosticsRepresentation: new value:(startcollection) "
                            + ssc);
        }

        Log.i(LOG_TAG, "setDiagnosticsRepresentation: exit");
    }

    OcRepresentation getDiagnosticsRepresentation() {
        diagnosticsRep.setValueString("fr", factoryReset);
        diagnosticsRep.setValueString("rb", reboot);
        diagnosticsRep.setValueString("ssc", startCollection);
        return diagnosticsRep;
    }

    public String getUri() {
        return diagnosticsUri;
    }

    // For Resetting diagnostics Resource attributes to their default values
    public void factoryReset() {
        factoryReset = ConfigurationDefaultValues.defaultFactoryReset;
        reboot = ConfigurationDefaultValues.defaultReboot;
        startCollection = ConfigurationDefaultValues.defaultStartCollection;
    }

    // For Deleting diagnostic resource
    public void deleteResource() {
        try {
            if (null != diagnosticsHandle) {
                // Unregister the collection resource
                OcPlatform.unregisterResource(diagnosticsHandle);
            }
        } catch (OcException e) {
            Log.e(LOG_TAG, "OcException occured! " + e.toString());
        }
    }
}

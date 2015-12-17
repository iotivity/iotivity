//******************************************************************
//
// Copyright 2015 Duseok Kang (Seoul National University) All Rights Reserved.
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

package org.iotivity.service.sample.container;

import android.os.Message;
import android.util.Log;

import org.iotivity.service.resourcecontainer.RcsBundleInfo;
import org.iotivity.service.resourcecontainer.RcsResourceContainer;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class ResourceContainer {
    private final String LOG_TAG = "[" + "ResourceContainer" + "]";
    private static MainActivity mainActivityInstance;

    public static String logMessage;
    private static Message msg;

    private RcsResourceContainer containerInstance;
    private TrackingResource trackingResource;

    public boolean startBundleFlag = false;
    private static boolean isStarted = false;

    public ResourceContainer() {
        mainActivityInstance = MainActivity.getActivity();
        containerInstance = new RcsResourceContainer(mainActivityInstance.getApplicationContext());
//        trackingResource = new TrackingResource(mainActivityInstance.getApplicationContext());

//        trackingResource.setURI("/boy/1");
//        trackingResource.setName("boy1");
    }

    public void startContainer(String sdCardPath) {
        String configFile = sdCardPath + "/ResourceContainerConfig.xml";
        Log.i(LOG_TAG, "config path : " + configFile);

        if (!isStarted) {
            containerInstance.startContainer(configFile);
            isStarted = true;
            logMessage = "Containter Started ";

            MainActivity.setMessageLog(logMessage);
            msg = Message.obtain();
            msg.what = 1;
            mainActivityInstance.getHandler().sendMessage(msg);

            msg = Message.obtain();
            msg.what = 0;
            mainActivityInstance.getHandler().sendMessage(msg);
        }
    }

    public void startTracking() {
        List<RcsBundleInfo> bundleList = containerInstance.listBundles();

        if (1 == bundleList.size()) {
            logMessage = "Bundle not added\n";
        } else if (startBundleFlag) {
            logMessage = "Bundle already started\n";
        } else {
            containerInstance.startBundle("oic.bundle.TrackingSensor");
            startBundleFlag = true;

            logMessage = "bundle to start\n";
            logMessage = logMessage + "ID : " + "oic.bundle.TrackingSensor\n";
            logMessage = logMessage + "Uri : " + "/android/trackingsensor/1\n\n";
            logMessage = logMessage + "bundle started successfully\n";
        }

        MainActivity.setMessageLog(logMessage);
        msg = Message.obtain();
        msg.what = 1;
        mainActivityInstance.getHandler().sendMessage(msg);
    }

    public void addTracking() {
//        Map<String,String> bundleParams = null;
        List<RcsBundleInfo> bundleList = containerInstance.listBundles();

        if (1 < bundleList.size()) {
            logMessage = "Bundle already added\n";
        } else {
            containerInstance.addBundle("oic.bundle.TrackingSensor","oic.r.trackingsensor",
                    "org.iotivity.service.sample.locationtrackingapp.apk","Activator",
                    null);

            logMessage = "bundle to add : \n";
            logMessage = logMessage + "ID : " + "oic.bundle.TrackingSensor\n";
            logMessage = logMessage + "Uri : " + "/android/trackingsensor/1\n\n";
            logMessage = logMessage + "bundle added successfully\n";
        }

        MainActivity.setMessageLog(logMessage);
        msg = Message.obtain();
        msg.what = 1;
        mainActivityInstance.getHandler().sendMessage(msg);
    }

    public void addResourceConfig() {
        if (!startBundleFlag) {
            logMessage = "Bundle is not started" + "\n";
        } else {
            List<RcsBundleInfo> bundleList = containerInstance.listBundles();
            List<String> bundleResources = containerInstance.listBundleResources("oic.bundle.TrackingSensor");

            if (0 == bundleList.size()) {
                logMessage = "No bundle found" + "\n";
            } else if ((0 == bundleList.size())
                    && (0 == bundleResources.size())) {
                logMessage = "No bundle found" + "\n";
            } else {
                Map<String, String> params = new HashMap<>();
                params.put("resourceType", "oic.r.trackingsensor");
                params.put("address",
                        "http://192.168.2.40/api/duseok/sensor/7");
                containerInstance.addResourceConfig("oic.bundle.TrackingSensor", "", params);
                logMessage = "resource added successfully" + "\n";
                Log.i(LOG_TAG, logMessage);
            }
        }
        MainActivity.setMessageLog(logMessage);
        msg = Message.obtain();
        msg.what = 1;
        mainActivityInstance.getHandler().sendMessage(msg);
    }

    public void addAndroidResource() {
        logMessage = "Add android resource\n";
        msg = Message.obtain();
        msg.what = 1;
        mainActivityInstance.getHandler().sendMessage(msg);

        containerInstance.registerResource("oic.bundle.TrackingSensor", trackingResource);
    }
}

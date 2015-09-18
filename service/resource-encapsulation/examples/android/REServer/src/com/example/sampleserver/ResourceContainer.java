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

package com.example.sampleserver;

import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import org.iotivity.ResourceEncapsulation.server.RCSBundleInfo;
import org.iotivity.ResourceEncapsulation.server.RCSResourceContainer;

import android.content.Context;
import android.os.Message;

/**
 * For calling the Resource Container APIs as per user selection on UI and for
 * updating the UI
 *
 * It contains all the Resource Container APIs.
 */
public class ResourceContainer {

    private RCSResourceContainer             containerInstance;
    public static String                     logMessage;
    private static ResourceContainerActivity resourceContainerActivityInstance;
    private static Message                   msg;
    private static boolean                   startBundleFlag;
    private static boolean                   isStarted = false;

    // constructor
    public ResourceContainer() {
        resourceContainerActivityInstance = ResourceContainerActivity
                .getResourceContainerActivityObj();
        containerInstance = RCSResourceContainer.getInstance();
    }

    // Start Container
    public void startContainer(String sdCardPath) {

        startBundleFlag = true;
        String configFile = sdCardPath + "/ResourceContainerConfig.xml";

        if (!isStarted) {
            for (int i = 0; i < 2; i++) {
                containerInstance.startContainer(configFile);
                isStarted = true;
            }
        } else {
            containerInstance.startContainer(configFile);
        }
        logMessage = "Container Started";
        ResourceContainerActivity.setMessageLog(logMessage);
        msg = Message.obtain();
        msg.what = 1;
        resourceContainerActivityInstance.getHandler().sendMessage(msg);

        msg = Message.obtain();
        msg.what = 0;
        resourceContainerActivityInstance.getHandler().sendMessage(msg);
    }

    // Stop Container
    public void stopContainer() {

        containerInstance.stopContainer();
        logMessage = "Container stopped";
        ResourceContainerActivity.setMessageLog(logMessage);
        msg = Message.obtain();
        msg.what = 1;
        resourceContainerActivityInstance.getHandler().sendMessage(msg);
    }

    // List Bundle Resources
    public void listBundleResources() {

        List<String> bundleResources = containerInstance
                .listBundleResources("oic.bundle.discomfortIndexSensor");
        Iterator<String> it = bundleResources.iterator();
        logMessage = "";

        if (0 == bundleResources.size()) {
            logMessage = logMessage + "No resource found in the bundle" + "\n";
        } else {
            while (it.hasNext()) {
                String element = (String) it.next();
                logMessage = logMessage + element + "\n";
            }
        }
        ResourceContainerActivity.setMessageLog(logMessage);
        msg = Message.obtain();
        msg.what = 1;
        resourceContainerActivityInstance.getHandler().sendMessage(msg);
    }

    // List Bundles
    public void listBundles() {

        List<RCSBundleInfo> bundleList = containerInstance.listBundles();
        Iterator<RCSBundleInfo> it = bundleList.iterator();
        logMessage = "";
        logMessage = logMessage + "size of bundleList : " + bundleList.size()
                + "\n";

        while (it.hasNext()) {

            RCSBundleInfo object = (RCSBundleInfo) it.next();
            logMessage = logMessage + "ID :" + object.getID() + "\n";
            logMessage = logMessage + "Path: " + object.getPath() + "\n";
            logMessage = logMessage + "version : " + object.getVersion() + "\n";
        }
        ResourceContainerActivity.setMessageLog(logMessage);
        msg = Message.obtain();
        msg.what = 1;
        resourceContainerActivityInstance.getHandler().sendMessage(msg);
    }

    // Add Bundles
    public void addBundle() {

        Map<String, String> bundleParams = null;
        List<RCSBundleInfo> bundleList = containerInstance.listBundles();
        if (0 < bundleList.size()) {
            logMessage = "Bundle already added" + "\n";

        } else {
            for (int i = 0; i < 2; i++) {
                containerInstance
                        .addBundle(
                                "oic.bundle.discomfortIndexSensor",
                                "xyz",
                                "data/data/com.example.sampleresourceserver/files/libSoftSensorBundle.so",
                                bundleParams);
            }

            logMessage = "bundle to add : " + "\n";
            logMessage = logMessage + "ID :"
                    + "oic.bundle.discomfortIndexSensor" + "\n";
            logMessage = logMessage + "Uri: " + "xyz" + "\n";
            logMessage = logMessage
                    + "Path : "
                    + "data/data/com.re.sampleclient/files/libSoftSensorBundle.so"
                    + "\n";
            logMessage = logMessage + "bundle added successfully" + "\n";
        }

        ResourceContainerActivity.setMessageLog(logMessage);
        msg = Message.obtain();
        msg.what = 1;
        resourceContainerActivityInstance.getHandler().sendMessage(msg);
    }

    // Remove Bundle
    public void removeBundle() {

        List<RCSBundleInfo> bundleList = containerInstance.listBundles();
        if (0 == bundleList.size()) {
            logMessage = "No bundle to remove" + "\n";

        } else {

            for (int i = 0; i < 2; i++) {
                containerInstance
                        .removeBundle("oic.bundle.discomfortIndexSensor");
            }
            startBundleFlag = false;
            logMessage = "bundle to remove : " + "\n";
            logMessage = logMessage + "ID :"
                    + "oic.bundle.discomfortIndexSensor" + "\n";
            logMessage = logMessage + " bundle removed  successfully" + "\n";
        }
        ResourceContainerActivity.setMessageLog(logMessage);
        msg = Message.obtain();
        msg.what = 1;
        resourceContainerActivityInstance.getHandler().sendMessage(msg);
    }

    // Start Bundle
    public void startBundle() {

        List<RCSBundleInfo> bundleList = containerInstance.listBundles();
        if (0 == bundleList.size()) {
            logMessage = "No bundle to Start" + "\n";
        } else if (true == startBundleFlag) {
            logMessage = "Bundle already started" + "\n";
        } else {
            startBundleFlag = true;
            containerInstance.startBundle("oic.bundle.discomfortIndexSensor");

            logMessage = " bundle to start" + "\n";
            logMessage = logMessage + " ID : oic.bundle.discomfortIndexSensor"
                    + "\n";
            logMessage = logMessage + " bundle started successfully" + "\n";
        }
        ResourceContainerActivity.setMessageLog(logMessage);
        msg = Message.obtain();
        msg.what = 1;
        resourceContainerActivityInstance.getHandler().sendMessage(msg);
    }

    // Stop Bundle
    public void stopBundle() {

        if (false == startBundleFlag) {
            logMessage = "Bundle already stopped" + "\n";
        } else {

            List<RCSBundleInfo> bundleList = containerInstance.listBundles();
            if (0 == bundleList.size()) {
                logMessage = "No bundle to Stop" + "\n";

            } else {
                containerInstance
                        .stopBundle("oic.bundle.discomfortIndexSensor");
                startBundleFlag = false;
                logMessage = " bundle to stop" + "\n";
                logMessage = logMessage
                        + " ID : oic.bundle.discomfortIndexSensor" + "\n";
                logMessage = logMessage + " bundle stopped successfully" + "\n";
            }
        }

        ResourceContainerActivity.setMessageLog(logMessage);
        msg = Message.obtain();
        msg.what = 1;
        resourceContainerActivityInstance.getHandler().sendMessage(msg);
    }

    // Add Resource Configuration
    public void addResourceConfig() {

        if (false == startBundleFlag) {
            logMessage = "Bundle is not started" + "\n";

        } else {

            List<RCSBundleInfo> bundleList = containerInstance.listBundles();
            List<String> bundleResources = containerInstance
                    .listBundleResources("oic.bundle.discomfortIndexSensor");

            if (0 == bundleList.size()) {

                logMessage = "No bundle found" + "\n";

            } else if ((0 == bundleList.size())
                    && (0 == bundleResources.size())) {

                logMessage = "No bundle found" + "\n";
            } else {

                Map<String, String> params = new HashMap<String, String>();
                params.put("resourceType", "oic.softsensor");
                params.put("address",
                        "http://192.168.0.2/api/newdeveloper/sensor/22");
                containerInstance.addResourceConfig(
                        "oic.bundle.discomfortIndexSensor", "", params);
                logMessage = "resource added successfully" + "\n";
            }
        }
        ResourceContainerActivity.setMessageLog(logMessage);
        msg = Message.obtain();
        msg.what = 1;
        resourceContainerActivityInstance.getHandler().sendMessage(msg);
    }

    // Remove Resource Configuration
    public void removeResourceConfig() {

        List<String> bundleResources = containerInstance
                .listBundleResources("oic.bundle.discomfortIndexSensor");
        if (bundleResources.size() >= 1) {
            String element = bundleResources.get(0);
            containerInstance.removeResourceConfig(
                    "oic.bundle.discomfortIndexSensor", element);
            Message msg;
            logMessage = "resource removed successfully: \n" + element + "\n";
            ResourceContainerActivity.setMessageLog(logMessage);
            msg = Message.obtain();
            msg.what = 1;
            resourceContainerActivityInstance.getHandler().sendMessage(msg);
        } else {
            logMessage = "No resource to remove" + "\n";
            ResourceContainerActivity.setMessageLog(logMessage);
            msg = Message.obtain();
            msg.what = 1;
            resourceContainerActivityInstance.getHandler().sendMessage(msg);
        }
    }
}

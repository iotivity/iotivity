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
    public static boolean                    startBundleFlag;
    public static boolean                    isStarted     = false;
    public static boolean                    isInitialized = false;

    // constructor
    public ResourceContainer() {
        resourceContainerActivityInstance = ResourceContainerActivity
                .getResourceContainerActivityObj();
        containerInstance = RCSResourceContainer.getInstance();
    }

    // Start Container
    public void startContainer(String sdCardPath) {

        String configFile = sdCardPath + "/ResourceContainerConfig.xml";
        if (!isStarted && !isInitialized) {
            for (int i = 0; i < 2; i++) {
                containerInstance.startContainer(configFile);
            }
            isStarted = true;
            isInitialized = true;
        } else {
            containerInstance.startContainer(configFile);
            isStarted = true;
        }

        logMessage = "Container Started";
        logMessage += "with one Bundle" + "\n";
        logMessage += "ID : oic.bundle.discomfortIndexSensor";

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

        if (isStarted) {
            containerInstance.stopContainer();
            logMessage = "Container stopped";
            isStarted = false;
            ResourceContainerActivity.setMessageLog(logMessage);
            msg = Message.obtain();
            msg.what = 1;
            resourceContainerActivityInstance.getHandler().sendMessage(msg);
        }
    }

    // List Bundle Resources DI
    public void listBundleResourcesDI() {

        List<String> bundleResources = containerInstance
                .listBundleResources("oic.bundle.discomfortIndexSensor");

        Iterator<String> it = bundleResources.iterator();
        logMessage = "";

        if (0 == bundleResources.size()) {
            logMessage += "No resource found in the bundle" + "\n";
        } else {
            while (it.hasNext()) {
                String element = (String) it.next();
                logMessage += element + "\n";
            }
        }
        ResourceContainerActivity.setMessageLog(logMessage);
        msg = Message.obtain();
        msg.what = 1;
        resourceContainerActivityInstance.getHandler().sendMessage(msg);
    }

    // List Bundle Resources BMI
    public void listBundleResourcesBMI() {

        List<String> bundleResources = containerInstance
                .listBundleResources("oic.bundle.BMISensor");
        Iterator<String> it = bundleResources.iterator();
        logMessage = "";

        if (0 == bundleResources.size()) {
            logMessage += "No resource found in the bundle" + "\n";
        } else {
            while (it.hasNext()) {
                String element = (String) it.next();
                logMessage += element + "\n";
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
        int i = 0;
        logMessage = "";
        logMessage += "size of bundleList : " + bundleList.size() + "\n\n";

        while (it.hasNext()) {
            i++;
            RCSBundleInfo object = (RCSBundleInfo) it.next();
            logMessage += "Bundle : " + i + " -: \n";
            logMessage += "ID : " + object.getID() + "\n";
            logMessage += "Lib Path: " + object.getPath() + "\n";
            if (!(object.getVersion().equalsIgnoreCase("")))
                logMessage += "version : " + object.getVersion() + "\n\n";
            else
                logMessage += "\n";
        }
        ResourceContainerActivity.setMessageLog(logMessage);
        msg = Message.obtain();
        msg.what = 1;
        resourceContainerActivityInstance.getHandler().sendMessage(msg);
    }

    // Add Bundle BMI
    public void addBundleBMI() {

        Map<String, String> bundleParams = null;
        List<RCSBundleInfo> bundleList = containerInstance.listBundles();
        if (1 < bundleList.size()) {
            logMessage = "Bundle already added" + "\n";
        } else {
            for (int i = 0; i < 2; i++) {
                containerInstance
                        .addBundle(
                                "oic.bundle.BMISensor",
                                "xyz",
                                "data/data/com.example.sampleresourceserver/files/libBMISensorBundle.so",
                                bundleParams);
            }

            logMessage = "bundle to add : " + "\n";
            logMessage += "ID :" + "oic.bundle.BMISensor" + "\n";
            logMessage += "Uri: " + "xyz" + "\n";
            logMessage += "Path : "
                    + "data/data/com.re.sampleclient/files/libBMISensorBundle.so"
                    + "\n";
            logMessage += "bundle added successfully" + "\n";
        }

        ResourceContainerActivity.setMessageLog(logMessage);
        msg = Message.obtain();
        msg.what = 1;
        resourceContainerActivityInstance.getHandler().sendMessage(msg);
    }

    // Remove Bundle BMI
    public void removeBundleBMI() {

        List<RCSBundleInfo> bundleList = containerInstance.listBundles();
        if (1 == bundleList.size()) {
            logMessage = "Bundle already removed" + "\n";
        } else {

            for (int i = 0; i < 2; i++) {
                containerInstance.removeBundle("oic.bundle.BMISensor");
            }
            startBundleFlag = false;
            logMessage = "bundle to remove : " + "\n";
            logMessage += "ID :" + "oic.bundle.BMISensor" + "\n";
            logMessage += " bundle removed  successfully" + "\n";
        }
        ResourceContainerActivity.setMessageLog(logMessage);
        msg = Message.obtain();
        msg.what = 1;
        resourceContainerActivityInstance.getHandler().sendMessage(msg);
    }

    // Start Bundle BMI
    public void startBundleBMI() {

        List<RCSBundleInfo> bundleList = containerInstance.listBundles();
        if (1 == bundleList.size()) {
            logMessage = "BMI bundle not added" + "\n";
        } else if (true == startBundleFlag) {
            logMessage = "Bundle already started" + "\n";
        } else {
            startBundleFlag = true;
            containerInstance.startBundle("oic.bundle.BMISensor");

            logMessage = " bundle to start" + "\n";
            logMessage += " ID : oic.bundle.BMISensor" + "\n";
            logMessage += " bundle started successfully" + "\n";
        }
        ResourceContainerActivity.setMessageLog(logMessage);
        msg = Message.obtain();
        msg.what = 1;
        resourceContainerActivityInstance.getHandler().sendMessage(msg);
    }

    // Stop Bundle BMI
    public void stopBundleBMI() {

        if (false == startBundleFlag) {
            logMessage = "Bundle is not Started" + "\n";
        } else {
            List<RCSBundleInfo> bundleList = containerInstance.listBundles();
            if (0 == bundleList.size()) {
                logMessage = "No bundle to Stop" + "\n";
            } else {
                containerInstance.stopBundle("oic.bundle.BMISensor");
                startBundleFlag = false;
                logMessage = " bundle to stop" + "\n";
                logMessage += " ID : oic.bundle.BMISensor" + "\n";
                logMessage += " bundle stopped successfully" + "\n";
            }
        }

        ResourceContainerActivity.setMessageLog(logMessage);
        msg = Message.obtain();
        msg.what = 1;
        resourceContainerActivityInstance.getHandler().sendMessage(msg);
    }

    // Add Resource Configuration to DI bundle
    public void addResourceConfigDI() {

        List<RCSBundleInfo> bundleList = containerInstance.listBundles();
        List<String> bundleResources = containerInstance
                .listBundleResources("oic.bundle.discomfortIndexSensor");

        if (0 == bundleList.size()) {
            logMessage = "No bundle found" + "\n";
        } else if ((0 == bundleList.size()) && (0 == bundleResources.size())) {
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
        ResourceContainerActivity.setMessageLog(logMessage);
        msg = Message.obtain();
        msg.what = 1;
        resourceContainerActivityInstance.getHandler().sendMessage(msg);
    }

    // Add Resource Configuration to BMI bundle
    public void addResourceConfigBMI() {

        if (false == startBundleFlag) {
            logMessage = "Bundle is not started" + "\n";
        } else {
            List<RCSBundleInfo> bundleList = containerInstance.listBundles();
            List<String> bundleResources = containerInstance
                    .listBundleResources("oic.bundle.BMISensor");

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
                containerInstance.addResourceConfig("oic.bundle.BMISensor", "",
                        params);
                logMessage = "resource added successfully" + "\n";
            }
        }
        ResourceContainerActivity.setMessageLog(logMessage);
        msg = Message.obtain();
        msg.what = 1;
        resourceContainerActivityInstance.getHandler().sendMessage(msg);
    }

    // Remove Resource Configuration from DI Bundle
    public void removeResourceConfigDI() {

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

    // Remove Resource Configuration from BMI Bundle
    public void removeResourceConfigBMI() {

        List<String> bundleResources = containerInstance
                .listBundleResources("oic.bundle.BMISensor");
        if (bundleResources.size() >= 1) {
            String element = bundleResources.get(0);
            containerInstance.removeResourceConfig("oic.bundle.BMISensor",
                    element);
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

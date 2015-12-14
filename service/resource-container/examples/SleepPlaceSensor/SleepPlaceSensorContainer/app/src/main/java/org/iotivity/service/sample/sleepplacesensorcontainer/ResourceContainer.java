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

package org.iotivity.service.sample.sleepplacesensorcontainer;

import android.os.Message;

import android.util.Log;

import org.iotivity.service.resourcecontainer.AndroidBundleSoftSensorResource;
import org.iotivity.service.resourcecontainer.RcsBundleInfo;
import org.iotivity.service.resourcecontainer.RcsResourceContainer;

import java.util.Iterator;
import java.util.List;

/**
 * For calling the Resource Container APIs as per user selection on UI and for
 * updating the UI
 *
 * It contains all the Resource Container APIs.
 */
public class ResourceContainer {

    private RcsResourceContainer             containerInstance;
    public static String                     logMessage;

    private static ResourceContainerActivity resourceContainerActivityInstance;
    private static Message                   msg;
    public static boolean                    startBundleFlag;
    private static boolean                   isStarted     = false;
    public static boolean                    isInitialized = false;

    //private FlashLightResource testResource;
    private AndroidBundleSoftSensorResource SPSensor;

    // constructor
    public ResourceContainer() {
        resourceContainerActivityInstance = ResourceContainerActivity
                .getResourceContainerActivityObj();
        containerInstance = new RcsResourceContainer(resourceContainerActivityInstance.getApplicationContext());
        //SPSensor = new SleepPlaceSensor(resourceContainerActivityInstance.getApplicationContext());

        //testResource = new FlashLightResource(resourceContainerActivityInstance.getApplicationContext());
        //testResource.setURI("/light/1");
        //testResource.setName("light1");
    }

    // Start Container
    public void startContainer(String sdCardPath) {

        String configFile = sdCardPath + "/ResourceContainerConfig.xml";
        Log.i("startContainer : config path : ", configFile);

        if (!isStarted) {
            containerInstance.startContainer(configFile);
            isStarted = true;
            logMessage = "Container Started ";

            ResourceContainerActivity.setMessageLog(logMessage);
            msg = Message.obtain();
            msg.what = 1;
            resourceContainerActivityInstance.getHandler().sendMessage(msg);

            msg = Message.obtain();
            msg.what = 0;
            resourceContainerActivityInstance.getHandler().sendMessage(msg);
        }
    }

    // Stop Container
    public void stopContainer() {

        if (isStarted) {
            containerInstance.stopContainer();
            logMessage = "Container stopped";
            isStarted = false;
        } else {
            logMessage = "Container not started";
        }

        ResourceContainerActivity.setMessageLog(logMessage);
        msg = Message.obtain();
        msg.what = 1;
        resourceContainerActivityInstance.getHandler().sendMessage(msg);
    }

    // List Bundles
    public void listBundles() {

        List<RcsBundleInfo> bundleList = containerInstance.listBundles();
        Iterator<RcsBundleInfo> it = bundleList.iterator();
        int i = 0;
        logMessage = "";
        logMessage = logMessage + "size of bundleList : " + bundleList.size()
                + "\n\n";

        while (it.hasNext()) {
            i++;
            RcsBundleInfo object = (RcsBundleInfo) it.next();
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

    public void addAndroidResource(){
        logMessage = "Add android resource " + "\n";
        ResourceContainerActivity.setMessageLog(logMessage);
        msg = Message.obtain();
        msg.what = 1;
        resourceContainerActivityInstance.getHandler().sendMessage(msg);
        //containerInstance.registerResource("SleepPlaceSensor", SPSensor);
        //containerInstance.registerAndroidResource("testBundle", testResource);
    }
}

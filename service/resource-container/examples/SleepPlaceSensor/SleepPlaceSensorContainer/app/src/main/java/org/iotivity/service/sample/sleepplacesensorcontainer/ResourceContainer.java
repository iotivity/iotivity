//******************************************************************
//
// Copyright 2015 Euiseok Kim (Seoul National University) All Rights Reserved.
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

package org.iotivity.service.sample.sleepplacesensorcontainer;

import android.os.Message;
import android.util.Log;

import org.iotivity.service.resourcecontainer.RcsResourceContainer;

/**
 * Created by chjej202 on 15. 12. 13.
 */
public class ResourceContainer {
    private final String LOG_TAG = "[" + ResourceContainerActivity.class.getName() + "]";
    private static ResourceContainerActivity resourceContainerActivityInstance;

    public static String logMessage;
    private static Message msg;

    private RcsResourceContainer containerInstance;

    public boolean startBundleFlag = false;
    private static boolean isStarted = false;

    public ResourceContainer() {
        resourceContainerActivityInstance = ResourceContainerActivity.getActivity();
        containerInstance = new RcsResourceContainer(resourceContainerActivityInstance.getApplicationContext());
    }

    public void startContainer(String sdCardPath) {
        String configFile = sdCardPath + "/ResourceContainerConfig.xml";
        Log.i(LOG_TAG, "config path : " + configFile);

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

    public void stopContainer() {

    }
}

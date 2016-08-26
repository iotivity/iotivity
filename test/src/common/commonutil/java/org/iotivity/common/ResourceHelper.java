/******************************************************************
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 * <p/>
 * <p/>
 * <p/>
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * <p/>
 * http://www.apache.org/licenses/LICENSE-2.0
 * <p/>
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************/

package org.iotivity.common;

import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcResource;

import java.util.Vector;

public class ResourceHelper {

    private static ResourceHelper s_resourceHelperInstance;

    private ResourceHelper() {
    }

    public synchronized static ResourceHelper getInstance() {
        if (s_resourceHelperInstance == null) {
            if (s_resourceHelperInstance == null) {
                s_resourceHelperInstance = new ResourceHelper();
            }
        }

        return s_resourceHelperInstance;
    }

    public boolean waitForResourceFound(Vector<OcResource> foundResourceList,
                                        int timeOut) {
        boolean isTimeOut = false;
        int passedTime = 0;
        while (foundResourceList.isEmpty()) {
            waitInSecond(ResourceConstants.CALLBACK_WAIT_MIN);
            passedTime++;
            System.out.println(" seconds for finding resources");

            if (passedTime > timeOut) {
                isTimeOut = true;
                break;
            }
        }

        return isTimeOut;
    }

    public boolean isCallbackTimeoutOccured(int callbackMonitor, int timeOut) {
        boolean isTimeOut = false;
        long startTime = System.nanoTime();
        while (callbackMonitor == 0) {
            waitInSecond(1);

            if ((System.nanoTime() - startTime) / 1000000000 > timeOut) {
                isTimeOut = true;
                break;
            }
        }

        return isTimeOut;
    }

    public void printRepresentation(OcRepresentation rep) {
        String uri = rep.getUri();
        if (uri.equals("")) {
            System.out.println("The representation has no uri.");
        } else {
            System.out.println("The representation has uri: " + uri);
        }

        System.out.println("The representation has following value: ");
    }

    public void waitInSecond(long seconds) {
        try {
            Thread.sleep(seconds * 1000);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}

/*
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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
 */

package org.iotivity.simulator;

import java.util.Vector;

class SimulatorManagerCallback {

    private IResourceModelChangedListener   resourceModelChangeListener;
    private ILogMessageListener             logMessageListener;
    private static SimulatorManagerCallback simulatorManagerCallbackObj;

    private SimulatorManagerCallback() {
    }

    public static synchronized SimulatorManagerCallback getInstance() {
        if (null == simulatorManagerCallbackObj) {
            simulatorManagerCallbackObj = new SimulatorManagerCallback();
        }
        return simulatorManagerCallbackObj;
    }

    public void registerResourceModelChangeListener(
            IResourceModelChangedListener listener) {
        resourceModelChangeListener = listener;
    }

    public void registerLogMessageListener(ILogMessageListener listener) {
        logMessageListener = listener;
    }

    public void unregisterResourceModelChangeListener() {
        resourceModelChangeListener = null;
    }

    public void unregisterLogMessageListener() {
        logMessageListener = null;
    }

    public void onResourceModelChanged(String resourceURI,
            Vector<SimulatorResourceAttribute> attributes) {
        if (null != resourceModelChangeListener) {
            resourceModelChangeListener.onResourceModelChanged(resourceURI,
                    attributes);
        }
    }

    public void onLogMessageHandler(int level, String tag, String location,
            int lineNumber, String log) {
        if (null != logMessageListener) {
            logMessageListener.onLogMessageHandler(level, tag, location,
                    lineNumber, log);
        }
    }
}
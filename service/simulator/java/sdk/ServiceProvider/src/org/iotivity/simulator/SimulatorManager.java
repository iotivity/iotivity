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

public class SimulatorManager {

    static {
        // TODO: Load all native libraries here
    }

    public static void setLogMessageHandler(ILogMessageListener listener) {
        SimulatorManagerCallback.getInstance().registerLogMessageListener(
                listener);
    }

    public static void setResourceModelChangeHandler(
            IResourceModelChangedListener listener) {
        SimulatorManagerCallback.getInstance()
                .registerResourceModelChangeListener(listener);
    }

    public static void initialize() {
        SimulatorManagerNativeInterface.initialize();
    }

    public static void terminate() {
        SimulatorManagerNativeInterface.terminate();

        // Unregister the listeners
        SimulatorManagerCallback.getInstance().unregisterLogMessageListener();
        SimulatorManagerCallback.getInstance()
                .unregisterResourceModelChangeListener();
    }

    public static SimulatorResourceServer createResource(String configPath) {
        SimulatorResourceServer simulatorResourceServerObj;
        simulatorResourceServerObj = SimulatorManagerNativeInterface
                .createResource(configPath);
        return simulatorResourceServerObj;
    }

    public static Vector<SimulatorResourceServer> createResource(
            String configPath, int count) {
        Vector<SimulatorResourceServer> simulatorResourceServerVector;
        simulatorResourceServerVector = SimulatorManagerNativeInterface
                .createResources(configPath, count);
        return simulatorResourceServerVector;
    }

    public static Vector<SimulatorResourceServer> getLocalResources() {
        Vector<SimulatorResourceServer> simulatorResourceServerVector = null;
        simulatorResourceServerVector = SimulatorManagerNativeInterface
                .getResources();
        return simulatorResourceServerVector;
    }

    public static void deleteResource(SimulatorResourceServer resource) {
        SimulatorManagerNativeInterface.deleteResource(resource);
    }

    public static void deleteAllResources(String resourceType) {
        SimulatorManagerNativeInterface.deleteAllResources(resourceType);
    }

    public static void deleteAllResources() {
        SimulatorManagerNativeInterface.deleteAllResources();
    }
}
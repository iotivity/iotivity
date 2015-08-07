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

/**
 * This file contains a class which has the methods
 * for creation and deletion of resources.
 */
package org.oic.simulator;

import java.util.ArrayList;
import java.util.Vector;

import org.oic.simulator.clientcontroller.IFindResourceListener;
import org.oic.simulator.clientcontroller.SimulatorRemoteResource;
import org.oic.simulator.serviceprovider.IResourceModelChangedListener;
import org.oic.simulator.serviceprovider.SimulatorResourceServer;

/**
 * This class provides a set of methods for creation and deletion of resources.
 */
public class SimulatorManager {

    /**
     * Set listener for receiving log messages.
     *
     * @param logger
     *            {@link ILogger} to receive the log messages.
     */
    public static void setLogger(ILogger logger) {
        SimulatorManagerNativeInterface.setLogger(logger);
    }

    /**
     * API for creating a resource from a RAML configuration file whose path is
     * given as a parameter.
     *
     * @param configPath
     *            Path to RAML configuration file.
     * @param listener
     *            Listener for receiving notifications whenever there is a
     *            change in the resource model.
     * 
     * @return {@link SimulatorResourceServer} - Created resource on success,
     *         otherwise null.
     */
    public static SimulatorResourceServer createResource(String configPath,
            IResourceModelChangedListener listener) {
        SimulatorResourceServer simulatorResourceServerObj;
        simulatorResourceServerObj = SimulatorManagerNativeInterface
                .createResource(configPath, listener);
        return simulatorResourceServerObj;
    }

    /**
     * API for creating a set of resources from a RAML configuration file whose
     * path is given as a parameter.
     *
     * @param configPath
     *            Path to RAML configuration file.
     * @param count
     *            Number of resources to be created.
     * @param listener
     *            Listener for receiving notifications whenever there is a
     *            change in the resource model.
     * 
     * @return Returns an array of {@link SimulatorResourceServer} objects one
     *         for each created resource on success, otherwise null.
     */
    public static SimulatorResourceServer[] createResource(String configPath,
            int count, IResourceModelChangedListener listener) {
        SimulatorResourceServer[] simulatorResourceServers;
        simulatorResourceServers = SimulatorManagerNativeInterface
                .createResources(configPath, count, listener);
        return simulatorResourceServers;
    }

    /**
     * API for getting all locally created resources.
     *
     * @return Returns a list of {@link SimulatorResourceServer} objects on
     *         success, otherwise null.
     */
    public static Vector<SimulatorResourceServer> getLocalResources() {
        Vector<SimulatorResourceServer> simulatorResourceServerVector = null;
        simulatorResourceServerVector = SimulatorManagerNativeInterface
                .getResources();
        return simulatorResourceServerVector;
    }

    /**
     * API for deleting a specific resource.
     *
     * @param resource
     *            {@link SimulatorResourceServer} object of the resource to be
     *            deleted.
     */
    public static void deleteResource(SimulatorResourceServer resource) {
        SimulatorManagerNativeInterface.deleteResource(resource);
    }

    /**
     * API for deleting either all the resources or resources of a specific
     * type. Ex: If resourceType is oic.light, all resources of oic.light type
     * will be deleted. If resourceType is null, then all of the resources will
     * be deleted.
     *
     * @param resourceType
     *            Type of resource to be deleted.
     */
    public static void deleteResources(String resourceType) {
        SimulatorManagerNativeInterface.deleteResources(resourceType);
    }

    /**
     * API for discovering resources in the network. Callback is called whenever
     * a resource is discovered in the network.
     *
     * @param resourceType
     *            Required resource type
     *
     * @return OCSimulatorResult - return value of this API. It returns
     *         OC_STACK_OK if success.
     *
     */
    public OCSimulatorResult findResource(String resourceType,
            IFindResourceListener listener) {
        OCSimulatorResult result;
        int ordinal = SimulatorManagerNativeInterface.findResource(
                resourceType, listener);
        result = OCSimulatorResult.conversion(ordinal);
        return result;
    }

    /**
     * API for getting the list of previously discovered resources in the
     * network.
     *
     * @param resourceType
     *            Required resource type
     *
     * @return ArrayList<SimulatorRemoteResource> - returns list of
     *         SimulatorRemoteResource
     *
     */
    public ArrayList<SimulatorRemoteResource> getFoundResources(
            String resourceType) {
        ArrayList<SimulatorRemoteResource> resourceList = SimulatorManagerNativeInterface
                .getFoundResources(resourceType);
        return resourceList;
    }
}
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
 * This file contains a class which provides a set of native methods
 * for creation and deletion of resources.
 */
package org.oic.simulator;

import java.util.List;
import java.util.ArrayList;
import java.util.Vector;

import org.oic.simulator.clientcontroller.IFindResourceListener;
import org.oic.simulator.clientcontroller.SimulatorRemoteResource;
import org.oic.simulator.ILogger;
import org.oic.simulator.serviceprovider.IResourceModelChangedListener;
import org.oic.simulator.serviceprovider.SimulatorResourceServer;

/**
 * This class provides a set of native functions for creation and deletion of
 * resources.
 */
class SimulatorManagerNativeInterface {

    /**
     * Native function for creating a resource.
     *
     * @param configPath
     *            Path to RAML configuration file.
     * @param listener
     *            Listener for receiving notifications whenever there is a
     *            change in the resource model.
     *
     * @return {@link SimulatorResourceServer} object on success, otherwise
     *         null.
     */
    public static native SimulatorResourceServer createResource(
            String configPath, IResourceModelChangedListener listener);

    /**
     * Native function for creating several resources.
     *
     * @param configPath
     *            Path to RAML configuration file.
     * @param listener
     *            Listener for receiving notifications whenever there is a
     *            change in the resource model.
     *
     * @return An array of {@link SimulatorResourceServer} objects on success,
     *         otherwise null.
     */
    public static native SimulatorResourceServer[] createResources(
            String configPath, int count, IResourceModelChangedListener listener);

    /**
     * Native function to get the list of locally created resources.
     *
     * @return A list of {@link SimulatorResourceServer} objects on success,
     *         otherwise null.
     */
    public static native Vector<SimulatorResourceServer> getResources();

    /**
     * Native function to delete a specific resource
     *
     * @param resource
     *            {@link SimulatorResourceServer} object of the resource to be
     *            deleted.
     */
    public static native void deleteResource(SimulatorResourceServer resource);

    /**
     * Native function to delete all resources or resources of a specific type.
     *
     * @param resourceType
     *            Type of the resource.
     */
    public static native void deleteResources(String resourceType);

    /**
     * Native function to set the logger listener for receiving the log messages
     * from native layer.
     */
    public static native void setLogger(ILogger logger);

    /**
     * Native function for discovering resources.
     *
     * @param resourceType
     *            - required resource type
     *
     * @return OCSimulatorResult - return value of this API. It returns
     *         OC_STACK_OK if success.
     *
     */
    public static native int findResource(String resourceType,
            IFindResourceListener listener);

    /**
     * Native function for getting the list of previously discovered resources
     * in the network.
     *
     * @param resourceType
     *            - required resource type
     *
     * @return ArrayList<SimulatorRemoteResource> - returns list of
     *         SimulatorRemoteResource
     *
     */
    public static native ArrayList<SimulatorRemoteResource> getFoundResources(
            String resourceType);

    /**
     * Method to get the URI for this resource
     *
     * @return resource URI
     */
    public static native String getUri();

    /**
     * Method to get the list of resource types
     *
     * @return List of resource types
     */
    public static native List<String> getResourceTypes();

    /**
     * Method to get the list of resource interfaces
     *
     * @return List of resource interface
     */
    public static native List<String> getResourceInterfaces();

    /**
     * Method to get a string representation of the resource's server ID. This
     * is unique per-server independent on how it was discovered.
     *
     * @return server ID
     */
    public static native String getServerId();
}
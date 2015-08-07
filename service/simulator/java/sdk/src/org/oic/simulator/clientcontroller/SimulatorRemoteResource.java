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
 * This file contains a class which has a set of native methods for 
 * communicating with a remote resource.  
 */
package org.oic.simulator.clientcontroller;

import java.util.List;
import java.util.Map;

import org.oic.simulator.serviceprovider.SimulatorResourceModel;

/**
 * SimulatorRemoteResource represents a Resource running in the remote Simulator
 * Server. It comes with a well-defined contract or interface onto which you can
 * perform different operations or subscribe for event notifications.
 */
public class SimulatorRemoteResource {

    private SimulatorRemoteResource(long nativeHandle) {
        this.mNativeHandle = nativeHandle;
    }

    /**
     * Method to get the URI for this resource
     *
     * @return resource URI
     */
    public native String getUri();

    /**
     * Method to get the list of resource types
     *
     * @return List of resource types
     */
    public native List<String> getResourceTypes();

    /**
     * Method to get the list of resource interfaces
     *
     * @return List of resource interface
     */
    public native List<String> getResourceInterfaces();

    /**
     * Method to get a string representation of the resource's server ID. This
     * is unique per-server independent on how it was discovered.
     *
     * @return server ID
     */
    public native String getServerId();

    /**
     * Method to set observation on the resource
     *
     * @param observeType
     *            allows the client to specify how it wants to observe
     * @param queryParamsMap
     *            map which can have the query parameter name and value
     * @param onObserveListener
     *            event handler The handler method will be invoked with a map of
     *            attribute name and values.
     *
     */
    public native void observe(SimulatorObserveType observeType,
            Map<String, String> queryParamsMap,
            IObserveListener onObserveListener);

    /**
     * Method to cancel the observation on the resource
     *
     */
    public native void cancelObserve();

    /**
     * Method to get the attributes of a resource.
     *
     * @param queryParamsMap
     *            map which can have the query parameter name and value
     * @param onGetListener
     *            The event handler will be invoked with a map of attribute name
     *            and values. The event handler will also have the result from
     *            this Get operation This will have error codes
     */
    public native void get(Map<String, String> queryParamsMap,
            IGetListener onGetListener);

    /**
     * Method to get the attributes of a resource.
     *
     * @param resourceType
     *            resourceType of the resource to operate on
     * @param resourceInterface
     *            interface type of the resource to operate on
     * @param queryParamsMap
     *            map which can have the query parameter name and value
     * @param onGetListener
     *            The event handler will be invoked with a map of attribute name
     *            and values. The event handler will also have the result from
     *            this Get operation This will have error codes
     */
    public void get(String resourceType, String resourceInterface,
            Map<String, String> queryParamsMap, IGetListener onGetListener) {
        this.get2(resourceType, resourceInterface, queryParamsMap,
                onGetListener);
    }

    private native void get2(String resourceType, String resourceInterface,
            Map<String, String> queryParamsMap, IGetListener onGetListener);

    /**
     * Method to set the representation of a resource (via PUT)
     *
     * @param representation
     *            representation of the resource
     * @param queryParamsMap
     *            Map which can have the query parameter name and value
     * @param onPutListener
     *            event handler The event handler will be invoked with a map of
     *            attribute name and values.
     */
    public native void put(SimulatorResourceModel representation,
            Map<String, String> queryParamsMap, IPutListener onPutListener);

    /**
     * Method to set the representation of a resource (via PUT)
     *
     * @param resourceType
     *            resource type of the resource to operate on
     * @param resourceInterface
     *            interface type of the resource to operate on
     * @param representation
     *            representation of the resource
     * @param queryParamsMap
     *            Map which can have the query parameter name and value
     * @param onPutListener
     *            event handler The event handler will be invoked with a map of
     *            attribute name and values.
     */
    public void put(String resourceType, String resourceInterface,
            SimulatorResourceModel representation,
            Map<String, String> queryParamsMap, IPutListener onPutListener) {
        this.put2(resourceType, resourceInterface, representation,
                queryParamsMap, onPutListener);
    }

    private native void put2(String resourceType, String resourceInterface,
            SimulatorResourceModel representation,
            Map<String, String> queryParamsMap, IPutListener onPutListener);

    /**
     * Method to POST on a resource
     *
     * @param representation
     *            representation of the resource
     * @param queryParamsMap
     *            Map which can have the query parameter name and value
     * @param onPostListener
     *            event handler The event handler will be invoked with a map of
     *            attribute name and values.
     */
    public native void post(SimulatorResourceModel representation,
            Map<String, String> queryParamsMap, IPostListener onPostListener);

    /**
     * Method to POST on a resource
     *
     * @param resourceType
     *            resource type of the resource to operate on
     * @param resourceInterface
     *            interface type of the resource to operate on
     * @param representation
     *            representation of the resource
     * @param queryParamsMap
     *            Map which can have the query parameter name and value
     * @param onPostListener
     *            event handler The event handler will be invoked with a map of
     *            attribute name and values.
     */
    public void post(String resourceType, String resourceInterface,
            SimulatorResourceModel representation,
            Map<String, String> queryParamsMap, IPostListener onPostListener) {
        this.post2(resourceType, resourceInterface, representation,
                queryParamsMap, onPostListener);
    }

    private native void post2(String resourceType, String resourceInterface,
            SimulatorResourceModel representation,
            Map<String, String> queryParamsMap, IPostListener onPostListener);

    @Override
    protected void finalize() throws Throwable {
        super.finalize();

        dispose();
    }

    private native void dispose();

    private long mNativeHandle;
}

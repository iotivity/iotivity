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

import java.util.LinkedList;
import java.util.Map;

import org.oic.simulator.SimulatorResourceModel;

/**
 * SimulatorRemoteResource represents a Resource running in the remote Simulator
 * Server. It comes with a well-defined contract or interface onto which you can
 * perform different operations or subscribe for event notifications.
 */
public class SimulatorRemoteResource {

    private SimulatorRemoteResource(long nativeHandle) {
        this.nativeHandle = nativeHandle;
    }

    /**
     * Method to get the URI for this resource
     * 
     * @return resource URI
     */
    public String getUri() {
        return mUri;
    }

    /**
     * Method to get the observe capability of this resource
     * 
     * @return true if the resource is observable, otherwise false.
     */
    public boolean getIsObservable() {
        return mIsObservable;
    }

    /**
     * Method to get the connectivity type for this resource
     * 
     * @return Connectivity type.
     */
    public String getConnectivityType() {
        return mConnType;
    }

    /**
     * Method to get the list of resource types
     * 
     * @return List of resource types
     */
    public LinkedList<String> getResourceTypes() {
        return mResTypes;
    }

    /**
     * Method to get the list of resource interfaces
     * 
     * @return List of resource interface
     */
    public LinkedList<String> getResourceInterfaces() {
        return mResInterfaces;
    }

    /**
     * Method to get a string representation of the resource's server ID. This
     * is unique per-server independent on how it was discovered.
     * 
     * @return server ID
     */
    public String getServerId() {
        return mSid;
    }

    /**
     * Method to get a unique Id of the resource.
     * 
     * @return Unique ID.
     */
    public String getUid() {
        return mUid;
    }

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
    public void get(Map<String, String> queryParamsMap,
            IGetListener onGetListener) {
        this.get(null, queryParamsMap, onGetListener);
    }

    /**
     * Method to get the attributes of a resource.
     * 
     * @param resourceInterface
     *            interface type of the resource to operate on
     * @param queryParamsMap
     *            map which can have the query parameter name and value
     * @param onGetListener
     *            The event handler will be invoked with a map of attribute name
     *            and values. The event handler will also have the result from
     *            this Get operation This will have error codes
     */
    public native void get(String resourceInterface,
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
    public void put(SimulatorResourceModel representation,
            Map<String, String> queryParamsMap, IPutListener onPutListener) {
        this.put(null, representation, queryParamsMap, onPutListener);
    }

    /**
     * Method to set the representation of a resource (via PUT)
     * 
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
    private native int put(String resourceInterface,
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
    public void post(SimulatorResourceModel representation,
            Map<String, String> queryParamsMap, IPostListener onPostListener) {
        this.post(null, representation, queryParamsMap, onPostListener);
    }

    /**
     * Method to POST on a resource
     * 
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
    private native int post(String resourceInterface,
            SimulatorResourceModel representation,
            Map<String, String> queryParamsMap, IPostListener onPostListener);

    /**
     * Method to set the RAML file path from application
     * 
     * @param ramlPath
     *            RAML configuration file path
     */
    public native void configureRAMLPath(String ramlPath);

    /**
     * Method to start verification of a resource using automation
     * 
     * @param requestType
     *            request type to verify
     * 
     * @param onVerifyListener
     *            event handler The event handler will be invoked with the
     *            automation ID.
     * 
     * @return Automation ID.
     * 
     */
    public native int startVerification(int requestType,
            IVerificationListener onVerifyListener);

    /**
     * Method to stop verification of a resource previously started.
     * 
     * @param id
     *            Automation ID.
     * 
     */
    public native void stopVerification(int id);

    @Override
    protected void finalize() throws Throwable {
        super.finalize();

        dispose();
    }

    private native void dispose();

    private long               nativeHandle;
    private String             mUri;
    private String             mConnType;
    private String             mSid;
    private String             mUid;
    private boolean            mIsObservable;
    private LinkedList<String> mResTypes;
    private LinkedList<String> mResInterfaces;
}

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

package org.oic.simulator.clientcontroller;

import java.util.LinkedList;
import java.util.Map;

import org.oic.simulator.InvalidArgsException;
import org.oic.simulator.NoSupportException;
import org.oic.simulator.OperationInProgressException;
import org.oic.simulator.SimulatorException;
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
     * Method to get the URI for this resource.
     * 
     * @return Resource URI
     */
    public String getUri() {
        return mUri;
    }

    /**
     * Method to get the observe capability of this resource.
     * 
     * @return True if the resource is observable, otherwise false.
     */
    public boolean getIsObservable() {
        return mIsObservable;
    }

    /**
     * Method to get the connectivity type for this resource.
     * 
     * @return Connectivity type.
     */
    public SimulatorConnectivityType getConnectivityType() {
        return SimulatorConnectivityType.getConnectivityType(mConnType);
    }

    /**
     * Method to get the list of resource types.
     * 
     * @return List of resource types.
     */
    public LinkedList<String> getResourceTypes() {
        return mResTypes;
    }

    /**
     * Method to get the list of resource interfaces.
     * 
     * @return List of resource interfaces.
     */
    public LinkedList<String> getResourceInterfaces() {
        return mResInterfaces;
    }

    /**
     * Method to get a string representation of the host address of the remote
     * resource.
     * 
     * @return Host address.
     */
    public String getHost() {
        return mHost;
    }

    /**
     * Method to get a unique Id for the resource.
     * 
     * @return Unique ID.
     */
    public String getId() {
        return mId;
    }

    /**
     * Method to set observation on the remote resource.
     * 
     * @param observeType
     *            Allows the client to specify how it wants to observe.
     * @param queryParamsMap
     *            Map which can have the query parameter names and values.
     * @param onObserveListener
     *            The handler method which will be invoked with a map of
     *            attribute names and values whenever there is a change in
     *            resource model of the remote resource.
     * 
     * @throws InvalidArgsException
     *             This exception will be thrown if any parameter has invalid
     *             values.
     * @throws SimulatorException
     *             This exception will be thrown for other errors.
     */
    public native void observe(SimulatorObserveType observeType,
            Map<String, String> queryParamsMap,
            IObserveListener onObserveListener) throws InvalidArgsException,
            SimulatorException;

    /**
     * Method to cancel the observation on the resource.
     * 
     * @throws InvalidArgsException
     *             This exception will be thrown if the native remote resource
     *             object is unavailable.
     * @throws SimulatorException
     *             This exception will be thrown for other errors.
     */
    public native void cancelObserve() throws InvalidArgsException,
            SimulatorException;

    /**
     * Method to get the attributes of a resource.
     * 
     * @param queryParamsMap
     *            Map which can have the query parameter name and value.
     * @param onGetListener
     *            Event handler which will be invoked with the response for GET
     *            request with a map of attribute name and values.
     * 
     * @throws InvalidArgsException
     *             This exception will be thrown if any parameter has invalid
     *             values.
     * @throws NoSupportException
     *             This exception will be thrown if we cannot send GET request
     *             to the remote resource.
     * @throws SimulatorException
     *             This exception will be thrown for other errors.
     */
    public void get(Map<String, String> queryParamsMap,
            IGetListener onGetListener) throws InvalidArgsException,
            NoSupportException, SimulatorException {
        this.get(null, queryParamsMap, onGetListener);
    }

    /**
     * Method to get the attributes of a resource.
     * 
     * @param resourceInterface
     *            Interface type of the resource to operate on.
     * @param queryParamsMap
     *            Map which can have the query parameter name and value.
     * @param onGetListener
     *            Event handler which will be invoked with the response for GET
     *            request with a map of attribute name and values.
     * 
     * @throws InvalidArgsException
     *             This exception will be thrown if any parameter has invalid
     *             values.
     * @throws NoSupportException
     *             This exception will be thrown if we cannot send GET request
     *             to the remote resource.
     * @throws SimulatorException
     *             This exception will be thrown for other errors.
     */
    public native void get(String resourceInterface,
            Map<String, String> queryParamsMap, IGetListener onGetListener)
            throws InvalidArgsException, NoSupportException, SimulatorException;

    /**
     * Method to set the representation of a resource (via PUT)
     * 
     * @param representation
     *            {@link SimulatorResourceModel} holding the representation of
     *            the resource.
     * @param queryParamsMap
     *            Map which can have the query parameter name and value.
     * @param onPutListener
     *            Event handler which will be invoked with the response for PUT
     *            request with a map of attribute name and values.
     * 
     * @throws InvalidArgsException
     *             This exception will be thrown if any parameter has invalid
     *             value.
     * @throws NoSupportException
     *             This exception will be thrown if we cannot send PUT request
     *             to the remote resource.
     * @throws SimulatorException
     *             This exception will be thrown for other errors.
     */
    public void put(SimulatorResourceModel representation,
            Map<String, String> queryParamsMap, IPutListener onPutListener)
            throws InvalidArgsException, NoSupportException, SimulatorException {
        this.put(null, representation, queryParamsMap, onPutListener);
    }

    /**
     * Method to set the representation of a resource (via PUT).
     * 
     * @param resourceInterface
     *            Interface type of the resource to operate on.
     * @param representation
     *            {@link SimulatorResourceModel} holding the representation of
     *            the resource.
     * @param queryParamsMap
     *            Map which can have the query parameter name and value.
     * @param onPutListener
     *            Event handler which will be invoked with the response for PUT
     *            request with a map of attribute name and values.
     * 
     * @throws InvalidArgsException
     *             This exception will be thrown if any parameter has invalid
     *             value.
     * @throws NoSupportException
     *             This exception will be thrown if we cannot send PUT request
     *             to the remote resource.
     * @throws SimulatorException
     *             This exception will be thrown for other errors.
     */
    private native void put(String resourceInterface,
            SimulatorResourceModel representation,
            Map<String, String> queryParamsMap, IPutListener onPutListener)
            throws InvalidArgsException, NoSupportException, SimulatorException;

    /**
     * Method to POST on a resource.
     * 
     * @param representation
     *            {@link SimulatorResourceModel} holding the representation of
     *            the resource
     * @param queryParamsMap
     *            Map which can have the query parameter name and value
     * @param onPostListener
     *            Event handler which will be invoked with the response for POST
     *            request with a map of attribute name and values.
     * 
     * @throws InvalidArgsException
     *             This exception will be thrown if any parameter has invalid
     *             value.
     * @throws NoSupportException
     *             This exception will be thrown if we cannot send POST request
     *             on the remote resource.
     * @throws SimulatorException
     *             This exception will be thrown for other errors.
     */
    public void post(SimulatorResourceModel representation,
            Map<String, String> queryParamsMap, IPostListener onPostListener)
            throws InvalidArgsException, NoSupportException, SimulatorException {
        this.post(null, representation, queryParamsMap, onPostListener);
    }

    /**
     * Method to POST on a resource.
     * 
     * @param resourceInterface
     *            Interface type of the resource to operate on.
     * @param representation
     *            {@link SimulatorResourceModel} holding the representation of
     *            the resource.
     * @param queryParamsMap
     *            Map which can have the query parameter name and value.
     * @param onPostListener
     *            Event handler which will be invoked with the response for POST
     *            request with a map of attribute name and values.
     * 
     * @throws InvalidArgsException
     *             This exception will be thrown if any parameter has invalid
     *             value.
     * @throws NoSupportException
     *             This exception will be thrown if we cannot send POST request
     *             on the remote resource.
     * @throws SimulatorException
     *             This exception will be thrown for other errors.
     */
    public native void post(String resourceInterface,
            SimulatorResourceModel representation,
            Map<String, String> queryParamsMap, IPostListener onPostListener)
            throws InvalidArgsException, NoSupportException, SimulatorException;

    /**
     * Method to set the RAML file path from application
     * 
     * @param ramlPath
     *            RAML configuration file path
     * 
     * @throws InvalidArgsException
     *             Thrown if the RAML configuration file path is invalid.
     * @throws SimulatorException
     *             Thrown for other errors.
     */
    public native void configureRAMLPath(String ramlPath)
            throws InvalidArgsException, SimulatorException;

    /**
     * Method to start verification of a resource using automation.
     * 
     * @param requestType
     *            Request type to verify.
     * @param onVerifyListener
     *            This event handler will be invoked with the current status of
     *            the automation.
     * 
     * @return Automation ID.
     * 
     * @throws InvalidArgsException
     *             This exception will be thrown if any parameter has invalid
     *             value.
     * @throws NoSupportException
     *             Thrown either if the resource does not support the request
     *             type or the resource is not configured with RAML.
     * @throws OperationInProgressException
     *             Thrown if another request generation session is already in
     *             progress.
     * @throws SimulatorException
     *             This exception will be thrown for other errors.
     */
    public int startVerification(SimulatorVerificationType requestType,
            IVerificationListener onVerifyListener)
            throws InvalidArgsException, NoSupportException,
            OperationInProgressException, SimulatorException {
        return startVerification(requestType.ordinal(), onVerifyListener);
    }

    private native int startVerification(int requestType,
            IVerificationListener onVerifyListener)
            throws InvalidArgsException, NoSupportException,
            OperationInProgressException, SimulatorException;

    /**
     * Method to stop verification of a resource previously started.
     * 
     * @param id
     *            Automation ID.
     * 
     * @throws InvalidArgsException
     *             Thrown if the automation ID is invalid.
     * @throws NoSupportException
     *             Thrown if the resource is not configured with RAML.
     * @throws SimulatorException
     *             Thrown for other errors.
     */
    public native void stopVerification(int id) throws InvalidArgsException,
            NoSupportException, SimulatorException;

    @Override
    protected void finalize() throws Throwable {
        super.finalize();

        dispose();
    }

    private native void dispose();

    private long               nativeHandle;
    private String             mUri;
    private int                mConnType;
    private String             mHost;
    private String             mId;
    private LinkedList<String> mResTypes;
    private LinkedList<String> mResInterfaces;
    private boolean            mIsObservable;
}

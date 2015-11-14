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

package org.oic.simulator.server;

import java.util.Vector;

import org.oic.simulator.InvalidArgsException;
import org.oic.simulator.SimulatorException;
import org.oic.simulator.SimulatorResourceModel;

public class SimulatorResource {

    private native void dispose();

    protected long mNativeHandle;

    protected SimulatorResource() {
    }

    @Override
    protected void finalize() throws Throwable {
        try {
            dispose();
        } catch (Throwable t) {
            throw t;
        } finally {
            super.finalize();
        }
    }

    public enum Type {
        SINGLE, COLLECTION
    }

    public enum AutoUpdateType {
        ONE_TIME, REPEAT
    }

    /**
     * API to get the name of the resource.
     *
     * @return Name of the resource.
     *
     * @throws SimulatorException
     *             This exception will be thrown if the native resource object
     *             does not exist or for some general errors.
     */
    public native String getName() throws SimulatorException;

    /**
     * API to get the type which indicates whether resource is single or
     * collection resource.
     *
     * @return Type of resource.
     *
     * @throws SimulatorException
     *             This exception will be thrown if the native resource object
     *             does not exist or for some general errors.
     */
    public native Type getType() throws SimulatorException;

    /**
     * API to get the resource URI.
     *
     * @return Resource URI.
     *
     * @throws SimulatorException
     *             This exception will be thrown if the native resource object
     *             does not exist or for some general errors.
     */
    public native String getURI() throws SimulatorException;

    /**
     * API to get the resource type.
     *
     * @return Resource type.
     *
     * @throws SimulatorException
     *             This exception will be thrown if the native resource object
     *             does not exist or for some general errors.
     */
    public native String getResourceType() throws SimulatorException;

    /**
     * API to get the interfaces resource is bound with.
     *
     * @return Interface type.
     *
     * @throws SimulatorException
     *             This exception will be thrown if the native resource object
     *             does not exist or for some general errors.
     */
    public native Vector<String> getInterface() throws SimulatorException;

    /**
     * API to get the observable state of resource.
     *
     * @return Observable state - true if resource is observable, otherwise
     *         false.
     *
     * @throws SimulatorException
     *             This exception will be thrown if the native resource object
     *             does not exist or for some general errors.
     */
    public native boolean isObservable() throws SimulatorException;

    /**
     * API to get the start state of resource.
     *
     * @return Start state - true if resource is started, otherwise false.
     *
     * @throws SimulatorException
     *             This exception will be thrown if the native resource object
     *             does not exist or for some general errors.
     */
    public native boolean isStarted() throws SimulatorException;

    /**
     * API to get the {@link SimulatorResourceModel} of the simulated resource.
     *
     * @return {@link SimulatorResourceModel} object on success, otherwise null.
     *
     * @throws SimulatorException
     *             This exception will be thrown if simulated resource is not
     *             proper.
     */
    public native SimulatorResourceModel getResourceModel()
            throws SimulatorException;

    /**
     * API to set the name of the resource.
     *
     * @param name
     *            - Name to be set.
     *
     * @throws InvalidArgsException
     *             This exception will be thrown if the resource name is
     *             invalid.
     * @throws SimulatorException
     *             This exception will be thrown if the native resource object
     *             does not exist or for some general errors.
     */
    public native void setName(String name) throws InvalidArgsException,
            SimulatorException;

    /**
     * API to set the resource URI.
     *
     * @param uri
     *            - URI to be set.
     *
     * @throws InvalidArgsException
     *             This exception will be thrown if the resource URI is invalid.
     * @throws SimulatorException
     *             This exception will be thrown if the native resource object
     *             does not exist or for some general errors.
     */
    public native void setURI(String uri) throws InvalidArgsException,
            SimulatorException;

    /**
     * API to set the resource type.
     *
     * @param resourceType
     *            - resource type string.
     *
     * @throws InvalidArgsException
     *             This exception will be thrown if the resource type is
     *             invalid.
     * @throws SimulatorException
     *             This exception will be thrown if the native resource object
     *             does not exist or for some general errors.
     */
    public native void setResourceType(String resourceType)
            throws InvalidArgsException, SimulatorException;

    /**
     * API to add interface type for resource.
     *
     * @param interfaceType
     *            - interface to be added for resource.
     *
     * @throws InvalidArgsException
     *             This exception will be thrown if the interface type is
     *             invalid.
     * @throws SimulatorException
     *             This exception will be thrown if the native resource object
     *             does not exist or for some general errors.
     */
    public native void addInterface(String interfaceType)
            throws InvalidArgsException, SimulatorException;

    /**
     * API to make the resource observable or not.
     *
     * @param state
     *            - true make the resource observable, otherwise non-observable.
     *
     * @throws SimulatorException
     *             This exception will be thrown if the native resource object
     *             does not exist or for some general errors.
     */
    public native void setObservable(boolean state) throws SimulatorException;

    /**
     * API to set the listener for receiving the notifications when observer is
     * registered or unregistered with resource.
     *
     * @param listener
     *            - Callback to be set for receiving the notifications.
     *
     * @throws InvalidArgsException
     *             This exception will be thrown if the listener is invalid.
     * @throws SimulatorException
     *             This exception will be thrown if the native resource object
     *             does not exist or for some general errors.
     */
    public native void setObserverListener(ObserverListener listener)
            throws InvalidArgsException, SimulatorException;

    /**
     * API to set listener for receiving notifications when resource's model
     * gets changed.
     *
     * @param listener
     *            {@link ResourceModelChangeListener}.
     *
     * @throws InvalidArgsException
     *             This exception will be thrown on invalid input.
     * @throws SimulatorException
     *             This exception will be thrown for other errors.
     */
    public native void setResourceModelChangeListener(
            ResourceModelChangeListener listener) throws InvalidArgsException,
            SimulatorException;

    /**
     * API to start the resource.
     *
     * @throws SimulatorException
     *             This exception will be thrown if the native resource object
     *             does not exist or for some general errors.
     */
    public native void start() throws SimulatorException;

    /**
     * API to stop the resource.
     *
     * @throws SimulatorException
     *             This exception will be thrown if the native resource object
     *             does not exist or for some general errors.
     */
    public native void stop() throws SimulatorException;

    /**
     * API to get observers which are registered with resource.
     *
     * @return observers as an array of {@link Observer}.
     *
     * @throws SimulatorException
     *             This exception will be thrown if the native resource object
     *             does not exist or for some general errors.
     */
    public native Vector<Observer> getObservers() throws SimulatorException;

    /**
     * API to notify current resource model to specific observer.
     *
     * @param observerId
     *            - Observer ID to notify.
     *
     * @throws SimulatorException
     *             This exception will be thrown if the native resource object
     *             does not exist or for some general errors.
     */
    public native void notifyObserver(int observerId) throws SimulatorException;

    /**
     * API to notify all registered observers.
     *
     * @throws SimulatorException
     *             This exception will be thrown if the native resource object
     *             does not exist or for some general errors.
     */
    public native void notifyAllObservers() throws SimulatorException;

    /**
     * Listener for receiving notification when observer is registered or
     * unregistered with the resource.
     */
    public interface ObserverListener {
        /**
         * Method will be invoked when a observer is registered with resource.
         *
         * @param uri
         *            URI of the resource.
         * @param observer
         *            {@link ObserverInfo} object containing the details of
         *            observer.
         */
        public void onObserverAdded(String resourceURI, Observer observer);

        /**
         * Method will be invoked when a observer is Unregistered with resource.
         *
         * @param resourceURI
         *            URI of the resource.
         * @param observer
         *            {@link ObserverInfo} object containing the details of
         *            observer.
         */
        public void onObserverRemoved(String resourceURI, Observer observer);
    }

    /**
     * Listener for receiving notification on completion of automatically
     * updating attribute value from its range or value set property.
     */
    public interface AutoUpdateListener {
        /**
         * Method for receiving automation complete notifications.
         *
         * @param uri
         *            URI of resource.
         * @param id
         *            Identifier for auto resource/attribute update session.
         */
        public void onUpdateComplete(String uri, int id);
    }

    /**
     * Listener for receiving notifications whenever there is a change in the
     * resource model.
     */
    public interface ResourceModelChangeListener {
        /**
         * Method will be invoked to notify about the changes in the resource
         * model.
         *
         * @param uri
         *            URI of resource.
         * @param resourceModel
         *            {@link SimulatorResourceModel} of the resource.
         */
        public void onResourceModelChanged(String uri,
                SimulatorResourceModel resourceModel);
    }
}

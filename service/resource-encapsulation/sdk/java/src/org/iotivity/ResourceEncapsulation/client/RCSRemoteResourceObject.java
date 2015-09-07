/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 *
 *
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
 *
 ******************************************************************/
/**
 * @file    RCSRemoteResourceObject.java
 *
 * @brief     This file provides a class which provides a set of native functions and API's
 *        of resource client, provided to the developers.
 */

package org.iotivity.ResourceEncapsulation.client;

import java.util.List;
import java.util.Vector;

import org.iotivity.ResourceEncapsulation.common.RCSException;
import org.iotivity.ResourceEncapsulation.common.RCSResourceAttributes;

import android.util.Log;

/**
 *
 * The resource can be discovered with discoverResource. On successful resource
 * discovery an instance of this class is returned. This class is an interaction
 * point between Resource and the developers. Developer will get the
 * RCSRemoteResourceObject by calling RCSDiscoveryManager's discoverResource
 * API.
 *
 * {@link RCSDiscoveryManager}
 *
 */
public class RCSRemoteResourceObject {

    private IStateChangedCallbackListener m_stateChangedListener;
    private ICacheUpdateListener          m_cacheUpdateListener;
    private IRemoteAttributeListener      m_remoteAttributeListener;
    private long                          m_nativeHandle;
    private String                        LOG_TAG = this.getClass()
                                                          .getSimpleName();

    // native methods
    private native boolean nativeIsMonitoring();

    private native boolean nativeIsCaching();

    private native boolean nativeIsObservable();

    private native void nativeStartMonitoring(
            IStateChangedCallbackListener listener);

    private native void nativeStopMonitoring();

    private native int nativeGetState();

    private native void nativeStartCaching(ICacheUpdateListener listener);

    private native void nativeStopCaching();

    private native int nativeGetCacheState();

    private native boolean nativeIsCachedAvailable();

    private native RCSResourceAttributes nativeGetCachedAttributes();

    private native String nativeGetCachedAttribute(String key);

    private native void nativeGetRemoteAttributes(
            IRemoteAttributeListener listener);

    private native void nativeSetRemoteAttributes(
            RCSResourceAttributes attributes, IRemoteAttributeListener listener);

    private native String nativeGetUri();

    private native String nativeGetAddress();

    private native List<String> nativeGetTypes();

    private native List<String> nativeGetInterFaces();

    private RCSRemoteResourceObject(long nativeHandle) {
        this.m_nativeHandle = nativeHandle;
    }

    /**
     * This Enum contains Status codes for resource state.
     *
     * <p>
     * Developer will get the ResoureState as a result of getState API of
     * RCSRemoteResourceObject class and as a callback result of startMonitoring
     * API
     *
     * {@link RCSRemoteResourceObject}
     */
    public enum ResourceState {

        NONE, /** < Monitoring is not started. */
        REQUESTED, /**
         * < Monitoring is started and checking state is in progress.
         * This is the default state after startMonitoring.
         */
        ALIVE, /** < The resource is alive. */
        LOST_SIGNAL, /** < Failed to reach the resource. */
        DESTROYED;
        /** < The resource is deleted. */

        /**
         * Utility API for integer to enum conversion.
         */
        public static ResourceState getResourceState(int ordinal) {

            if (ordinal == 0)
                return ResourceState.values()[0];
            else if (ordinal == 1)
                return ResourceState.values()[1];
            else if (ordinal == 2)
                return ResourceState.values()[2];
            else if (ordinal == 3)
                return ResourceState.values()[3];

            return null;
        }
    }

    /**
     * This Enum contains Status codes for cache state.
     *
     * <p>
     * Application will get this CacheState as a result of getCacheState API of
     * RCSRemoteResourceObject class
     *
     * {@link RCSRemoteResourceObject}
     */
    public enum CacheState {

        NONE, /** < Caching is not started. */
        UNREADY, /**
         * < Caching is started, but the data is not ready yet. This is
         * the default state after startCaching.
         */
        READY, /** < The data is ready. */
        LOST_SIGNAL;
        /** < Failed to reach the resource. */

        /**
         * Utility API for int to enum conversion.
         */
        public static CacheState getCacheState(int ordinal) {

            CacheState result = CacheState.values()[0];

            if (ordinal == 1)
                result = CacheState.values()[1];
            else if (ordinal == 2)
                result = CacheState.values()[2];
            else if (ordinal == 3)
                result = CacheState.values()[3];

            return result;
        }
    }

    /**
     * Provides interface for receiving the callback on cache update.
     */
    public interface ICacheUpdateListener {

        /**
         * This callback method is called when a asynchronous response for
         * startCaching request is received. Listener should be set to receive
         * this callback.
         *
         * @param attributes
         *            It is the updated resource attributes.
         *
         *            {@link RCSResourceAttributes}
         */
        public void onCacheUpdatedCallback(RCSResourceAttributes attributes);

    }

    /**
     * Provides interface for receiving the callback for the getRemoteAttributes
     * and setRemoteAttributes requested actions.
     */
    public interface IRemoteAttributeListener {

        /**
         * This callback method is called when a asynchronous response for the
         * getRemoteAttributes request or setRemoteAttributes request is
         * received.
         *
         * @param attributes
         *            It is the updated resource attributes value received from
         *            the remote resource.
         *
         *            {@link RCSResourceAttributes}
         */
        public void onRemoteAttributesGetCallback(
                RCSResourceAttributes attributes);

    }

    /**
     * Provides interface for receiving the callback for the startMonitoring
     * This callback is received on remote resource state change.
     */
    public interface IStateChangedCallbackListener {

        /**
         * This callback method is called when a asynchronous response for the
         * startMonitoring request is received.
         *
         * @param resourceState
         *            It gives the current remote resource state.
         *
         *            {@link ResourceState}
         */
        public void onStateChanged(ResourceState resourceState);
    }

    /**
     * Set listener for receiving asynchronous response for cache updates.
     *
     * @param listener
     *            ICacheUpdateListener to receive asynchronous response for
     *            cache updates.
     */
    public void setCacheUpdateListener(ICacheUpdateListener listener) {
        m_cacheUpdateListener = listener;
    }

    /**
     * Set listener for receiving asynchronous response for remote resource
     * state changes.
     *
     * @param listener
     *            IStateChangedCallbackListener to receive asynchronous response
     *            for remote resource state changes.
     */
    public void setStateChangedListener(IStateChangedCallbackListener listener) {
        m_stateChangedListener = listener;
    }

    /**
     * Set listener for receiving asynchronous response for resource attributes
     * SET and GET request.
     *
     * @param listener
     *            IRemoteAttributeListener to receive asynchronous response for
     *            resource attributes SET and GET request.
     */
    public void setRemoteAttributesListener(IRemoteAttributeListener listener) {
        m_remoteAttributeListener = listener;
    }

    /**
     * Returns whether monitoring is enabled.
     *
     * @return boolean - true if monitoring the resource.
     *
     *         {@link #startMonitoring()}
     */
    public boolean isMonitoring() {

        Log.i(LOG_TAG, "isMonitoring called");
        return this.nativeIsMonitoring();
    }

    /**
     * Returns whether caching is enabled.
     *
     * @return boolean - true if caching the resource.
     *
     *         {@link #startCaching()}
     */
    public boolean isCaching() {

        Log.i(LOG_TAG, "isCaching called");
        return this.nativeIsCaching();
    }

    /**
     * Returns whether resource is observable.
     *
     * @return boolean - true if resource is observable.
     */
    public boolean isObservable() {

        Log.i(LOG_TAG, "isObservable called");
        return this.nativeIsObservable();
    }

    /**
     * Starts monitoring the resource.
     * <p>
     * Monitoring provides a feature to check the presence of a resource, even
     * when the server is not announcing Presence using startPresnece.
     * <p>
     * StateChangedListener should be set before calling this API
     *
     * @throws RCSException
     *             If monitoring is already started.
     *
     *             {@link IStateChangedCallbackListener} {@link ResourceState}
     *             {@link #isMonitoring()} {@link #stopMonitoring()}
     */
    public void startMonitoring() throws RCSException {

        Log.i(LOG_TAG, "startMonitoring called");
        this.nativeStartMonitoring(m_stateChangedListener);
    }

    /**
     * Stop monitoring the resource.
     * <p>
     * It does nothing if monitoring is not started.
     * <p>
     * {@link #startMonitoring()}
     */
    public void stopMonitoring() {

        Log.i(LOG_TAG, "stopMonitoring called");
        this.nativeStopMonitoring();
    }

    /**
     * Returns the current state of the resource.
     *
     * @return ResourceState - current resource state
     *
     *         {@link #startMonitoring()} {@link ResourceState}
     */
    public ResourceState getState() {

        Log.i(LOG_TAG, "getState called");
        int state = this.nativeGetState();

        return ResourceState.getResourceState(state);
    }

    /**
     * Starts caching attributes of the resource.
     *
     * This will start data caching for the resource. Once caching started it
     * will look for the data updation on the resource and updates the cache
     * data accordingly.
     *
     * @throws RCSException
     *
     *             <p>
     *             If this method is called with null listener it will not
     *             notify the updated value to caller. It it is called with a
     *             valid listener updated cached values will be notified to
     *             caller.
     *
     *             {@link #getCacheState()} {@link #getCachedAttributes()}
     */
    public void startCaching() throws RCSException {

        if (null == m_cacheUpdateListener) {
            Log.i(LOG_TAG, "startCaching WITHOUT callback");
        } else {
            Log.i(LOG_TAG, "startCaching WITH callback");
        }

        this.nativeStartCaching(m_cacheUpdateListener);
    }

    /**
     * Stops caching.
     *
     * It does nothing if caching is not started.
     *
     * {@link #startCaching()}
     */
    public void stopCaching() {

        Log.i(LOG_TAG, "stopCaching called");
        this.nativeStopCaching();
    }

    /**
     * Returns the current cache state.
     *
     * @return CacheState - current cache state.
     *
     *         {@link CacheState}
     */
    public CacheState getCacheState() {

        Log.i(LOG_TAG, "getCacheState called");
        return CacheState.getCacheState(this.nativeGetCacheState());
    }

    /**
     * Returns whether cached data is available.
     *
     * Cache will be available always after cache state READY even if current
     * state is LOST_SIGNAL.
     *
     * @return boolean - true if cache data is available.
     *
     *         {@link #startCaching()} {@link CacheState}
     */
    public boolean isCachedAvailable() {

        Log.i(LOG_TAG, "isCachedAvailable called");
        return this.nativeIsCachedAvailable();
    }

    /**
     * Gets the cached RCSResourceAttributes data.
     *
     * <p>
     * Cache should be available.
     *
     * @return RCSResourceAttributes
     *
     * @throws RCSException
     *             If the Cache is not started.
     *
     *             {@link RCSResourceAttributes} {@link #isCachedAvailable()}
     *             {@link #startCaching()}
     */
    public RCSResourceAttributes getCachedAttributes() throws RCSException {

        Log.i(LOG_TAG, "getCachedAttributes called");
        return this.nativeGetCachedAttributes();
    }

    /**
     * Gets a particular cached ResourceAttribute Value.
     *
     * <p>
     * Cache should be available.
     *
     * @param key
     *            particular attribute name as a string to get the value for.
     *
     * @return String - Requested attribute value in string.
     *
     * @throws RCSException
     *             If the Cache is not started or InvalidKeyException If @a key
     *             doesn't match the key of any value.
     *
     *             {@link RCSResourceAttributes} {@link #isCachedAvailable()}
     *             {@link #startCaching()}
     */
    public String getCachedAttribute(String key) throws RCSException {

        Log.i(LOG_TAG, "getCachedAttribute called");
        return this.nativeGetCachedAttribute(key);
    }

    /**
     * Gets resource attributes directly from the server.
     *
     * This API send a get request to the resource of interest and provides the
     * attributes to the caller in the RemoteAttributesReceivedCallback.
     *
     * <p>
     * IRemoteAttributeListener should be set to get the attributes
     *
     * {@link IRemoteAttributeListener}
     */
    public void getRemoteAttributes() throws RCSException {

        Log.i(LOG_TAG, "getRemoteAttributes called");
        this.nativeGetRemoteAttributes(m_remoteAttributeListener);
    }

    /**
     * Sends a set request with resource attributes to the server.
     *
     * The SetRequest behavior depends on the server, whether updating its
     * attributes or not.
     *
     * @param attributes
     *            Attribute value to set for the remote resource.
     *
     *            <p>
     *            IRemoteAttributeListener should be set to call this API
     *
     *            {@link IRemoteAttributeListener}
     */
    public void setRemoteAttributes(RCSResourceAttributes attributes)
            throws RCSException {

        Log.i(LOG_TAG, "getRemoteAttributes called");
        this.nativeSetRemoteAttributes(attributes, m_remoteAttributeListener);
    }

    /**
     * Returns the uri of the resource.
     *
     * @return Uri of the resource
     */
    public String getUri() {

        Log.i(LOG_TAG, "getUri called");
        return this.nativeGetUri();
    }

    /**
     * Returns the address of the resource .
     *
     * @return Address of the resource
     */
    public String getAddress() {

        Log.i(LOG_TAG, "getAddress called");
        return this.nativeGetAddress();
    }

    /**
     * Returns the resource types of the resource.
     *
     * @return vector containing resource types
     */
    public Vector<String> getTypes() {

        Log.i(LOG_TAG, "getTypes called");
        List<String> types = this.nativeGetTypes();
        Vector<String> resourceType = new Vector<String>();
        resourceType.addAll(types);
        return resourceType;
    }

    /**
     * Returns the resource interfaces of the resource.
     *
     * @return vector containing resource Interfaces
     */
    public Vector<String> getInterFaces() {

        Log.i(LOG_TAG, "getInterFaces called");
        List<String> interfaces = this.nativeGetInterFaces();
        Vector<String> resourceInterfaces = new Vector<String>();
        resourceInterfaces.addAll(interfaces);
        return resourceInterfaces;
    }
}

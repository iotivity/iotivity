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
 * @file   RCSResourceObject.java
 *
 *  This file contains the resource object APIs provided to the developers.
 *        RCSResourceObject is a part of the server builder module.
 *
 */

package org.iotivity.ResourceEncapsulation.server;

import org.iotivity.ResourceEncapsulation.common.RCSResourceAttributes;

import android.util.Log;

/**
 * RCSResourceObject represents a resource. It handles any requests from clients
 * automatically with attributes.
 * <p>
 * It also provides an auto notification mechanism that notifies to the
 * observers. Requests are handled automatically by defaultAction of
 * RCSGetResponse and RCSSetResponse. Developer can override them and send your
 * own response.
 * <p>
 * For simple resources, developer may want to know whenever attributes are
 * changed by a set request. In this case, add an AttributeUpdatedListener with
 * a key interested in instead of overriding SetRequestHandler.
 *
 * <p>
 * Developer will get the object of this when they call the build() API of
 * Builder class
 *
 * {@link Builder}
 */
public class RCSResourceObject {

    private long                      m_nativeHandle;
    private IGetRequestHandler        m_getRequestListener;
    private ISetRequestHandler        m_setRequestListener;
    private IAttributeUpdatedListener m_attributeUpdatedListener;
    private final String              LOG_TAG = this.getClass().getSimpleName();

    // native methods
    private native void nativeSetAttributeInteger(String key, int value);

    private native void nativeSetAttributeDouble(String key, double value);

    private native void nativeSetAttributeBool(String key, boolean value);

    private native void nativeSetAttributeString(String key, String value);

    private native String nativeGetAttributeValue(String key);

    private native boolean nativeRemoveAttribute(String key);

    private native boolean nativeContainsAttribute(String key);

    private native RCSResourceAttributes nativeGetAttributes();

    private native boolean nativeIsObservable();

    private native boolean nativeIsDiscoverable();

    private native void nativeNotify();

    private native void nativeSetAutoNotifyPolicy(int policy);

    private native int nativeGetAutoNotifyPolicy();

    private native void nativeSetSetRequestHandlerPolicy(int policy);

    private native int nativeGetSetRequestHandlerPolicy();

    private native void nativeSetGetRequestHandler(IGetRequestHandler handler);

    private native void nativeSetSetRequestHandler(ISetRequestHandler handler);

    private native void nativeAddAttributeUpdatedListener(String key,
            IAttributeUpdatedListener listener);

    private native boolean nativeRemoveAttributeUpdatedListener(String key);

    private RCSResourceObject(long nativeHandle) {
        m_nativeHandle = nativeHandle;
    }

    /**
     * Represents the policy of AutoNotify function of RCSResourceObject class
     * In accord with this policy, observers are notified of attributes that are
     * changed or updated.
     *
     * <p>
     * Attributes are changed or updated according to execution of some
     * functions or receipt of 'set-request'.
     *
     * {@link RCSResourceObject}
     */
    public enum AutoNotifyPolicy {
        NEVER, /** < Never notify. */
        ALWAYS, /** < Always notify. */
        UPDATED;
        /** < When attributes are changed, notify. */

        /**
         * Utility API for integer to enum conversion.
         */
        public static AutoNotifyPolicy getAutoNotifyPolicy(int ordinal) {
            if (ordinal == 0)
                return AutoNotifyPolicy.values()[0];
            else if (ordinal == 1)
                return AutoNotifyPolicy.values()[1];
            else if (ordinal == 2)
                return AutoNotifyPolicy.values()[2];
            return null;
        }
    }

    public enum SetRequestHandlerPolicy {

        NEVER, /**
         * < Server ignore when server is received set-request of
         * attributes of the new key.
         */
        ACCEPTANCE;
        /**
         * < Server creates attributes of the new key When server is received
         * set-request of attributes of the new key.
         */

        /**
         * Utility API for integer to enum conversion.
         */
        public static SetRequestHandlerPolicy getSetRequestHandlerPolicy(
                int ordinal) {

            if (ordinal == 0)
                return SetRequestHandlerPolicy.values()[0];
            else if (ordinal == 1)
                return SetRequestHandlerPolicy.values()[1];
            return null;
        }
    }

    /**
     * Provides interface for receiving the callback on receiving a Get request
     * from the client
     */
    public interface IGetRequestHandler {

        /**
         * This callback method is called when received a get request from the
         * client. Listener should be set to receive this callback.
         *
         * @param request
         *            Request for the resource
         * @param attributes
         *            Resource Attributes
         *
         * @return RCSGetResponse Response to be sent.
         *
         *         {@link RCSResourceObject} {@link RCSGetResponse}
         *         {@link RCSResourceAttributes}
         */
        RCSGetResponse onGetRequest(RCSRequest request,
                RCSResourceAttributes attributes);

    }

    /**
     * Provides interface for receiving the callback on receiving a Set request
     * from the client
     */
    public interface ISetRequestHandler {

        /**
         * This callback method is called when received a set request from the
         * client. Listener should be set to receive this callback.
         *
         * @param request
         *            Request for the resource
         * @param attributes
         *            Resource Attributes to set
         *
         * @return RCSSetResponse Response to be sent.
         *
         *         {@link RCSResourceObject} {{@link RCSSetResponse}
         *         {@link RCSResourceAttributes}
         */
        RCSSetResponse onSetRequest(RCSRequest request,
                RCSResourceAttributes attributes);

    }

    /**
     * Provides interface for receiving the callback on update of requested
     * attribute.
     */
    public interface IAttributeUpdatedListener {

        /**
         * This callback method is called when a requested attribute value is
         * updated. Listener should be set to receive this callback.
         *
         * @param oldValue
         *            It is the attribute value before updation.
         * @param newValue
         *            It is the current resource attribute value.
         *
         *            {@link RCSResourceAttributes}
         */
        void onAttributeUpdate(String oldValue, String newValue);
    }

    /**
     * Set listener for receiving get request from the client asynchronously.
     *
     * @param listener
     *            IGetRequestHandler to receive asynchronous get request from
     *            client.
     *
     *            {@link IGetRequestHandler}
     */
    public void setGetRequestListener(IGetRequestHandler listener) {
        m_getRequestListener = listener;
    }

    /**
     * Set listener for receiving set request from the client asynchronously.
     *
     * @param listener
     *            ISetRequestHandler to receive asynchronous set request from
     *            client.
     *
     *            {@link ISetRequestHandler}
     */
    public void setSetRequestListener(ISetRequestHandler listener) {
        m_setRequestListener = listener;
    }

    /**
     * Set listener for receiving the updated attribute value asynchronously.
     *
     * @param listener
     *            IAttributeUpdatedListener to receive updated attribute value
     *            asynchronously.
     *
     *            {@link IAttributeUpdatedListener}
     */
    public void setAttributeUpdatedListener(IAttributeUpdatedListener listener) {
        m_attributeUpdatedListener = listener;
    }

    /**
     * API for setting a particular attribute value as a integer
     *
     * @param key
     *            name of attribute(used to map the attribute value).
     * @param value
     *            integer value to be mapped against the key.
     *
     *            The thread-safety for attributes is taken care internally.
     */
    public void setAttribute(String key, int value) {
        Log.e(LOG_TAG, "setAttribute (integer) called");
        this.nativeSetAttributeInteger(key, value);
    }

    /**
     * API for setting a particular attribute value as a double
     *
     * @param key
     *            name of attribute(used to map the attribute value).
     * @param value
     *            Double value to be mapped against the key.
     *
     *            The thread-safety for attributes is taken care internally.
     */
    public void setAttribute(String key, double value) {
        Log.e(LOG_TAG, "setAttribute (float) called");
        this.nativeSetAttributeDouble(key, value);
    }

    /**
     * API for setting a particular attribute value as a boolean
     *
     * @param key
     *            name of attribute(used to map the attribute value).
     * @param value
     *            boolean value to be mapped against the key.
     *
     *            The thread-safety for attributes is taken care internally.
     */
    public void setAttribute(String key, boolean value) {
        Log.e(LOG_TAG, "setAttribute (double) called ");
        this.nativeSetAttributeBool(key, value);
    }

    /**
     * API for setting a particular attribute value as a String
     *
     * @param key
     *            name of attribute(used to map the attribute value).
     * @param value
     *            String value to be mapped against the key.
     *
     *            The thread-safety for attributes is taken care internally.
     */
    public void setAttribute(String key, String value) {
        Log.e(LOG_TAG, "setAttribute (String) called ");
        this.nativeSetAttributeString(key, value);
    }

    /**
     * API for retrieving the attribute value associated with the supplied key.
     *
     * @param key
     *            Name of the attribute
     *
     * @return resource attributes value as a string
     *
     *         The thread-safety for attributes is taken care internally.
     */
    public String getAttributeValue(String key) {
        Log.e(LOG_TAG, "getAttributeValue called");
        return this.nativeGetAttributeValue(key);
    }

    /**
     * API for removing a particular attribute of the resource.
     *
     * @param key
     *            Name of the attribute.
     *
     * @return If the key exist and matched attribute is deleted, return true.
     *
     *         The thread-safety for attributes is taken care internally.
     */
    public boolean removeAttribute(String key) {
        Log.e(LOG_TAG, "removeAttribute called");
        return this.nativeRemoveAttribute(key);
    }

    /**
     * API for checking whether a particular attribute is there for a resource
     * or not.
     *
     * @param key
     *            Name of the attribute.
     *
     * @return If the key exist, return true.
     *
     *         The thread-safety for attributes is taken care internally.
     */
    public boolean containsAttribute(String key) {
        Log.e(LOG_TAG, "containsAttribute called");
        return this.nativeContainsAttribute(key);
    }

    /**
     * API for getting all the attributes of the RCSResourceObject.
     *
     * @return RCSResourceAttributes
     *
     *         {@link RCSResourceAttributes}
     */
    public RCSResourceAttributes getAttributes() {
        Log.e(LOG_TAG, "getAttributes called");
        return this.nativeGetAttributes();
    }

    /**
     * API for checking whether the particular resource is observable or not
     *
     * @return true if observable otherwise false
     */
    public boolean isObservable() {
        Log.e(LOG_TAG, "isObservable called");
        return this.nativeIsObservable();
    }

    /**
     * API for checking whether the particular resource is discoverable or not
     *
     * @return true if discoverable else false
     */
    public boolean isDiscoverable() {
        Log.e(LOG_TAG, "isDiscoverable called");
        return this.nativeIsDiscoverable();
    }

    /**
     * API for setting the resource's get request handler by the
     * developer/application. If developer set this handler then all get request
     * will come to the application and developer can send the response to the
     * client using APIs of RCSGetResponse class.
     *
     * <p>
     * IGetRequestHandler should be set to call this API. For setting the
     * listener call setGetRequestListener() API
     *
     * {@link RCSGetResponse} {@link #setGetRequestListener(IGetRequestHandler)}
     *
     */
    public void setGetRequestHandler() {
        if (null == m_getRequestListener) {
            Log.e(LOG_TAG, "getRequestListener is not set");
            return;
        } else {
            this.nativeSetGetRequestHandler(m_getRequestListener);
        }
    }

    /**
     * API for setting the resource's set request handler by the
     * developer/application. If developer set this handler then all set request
     * will come to the application and developer can send the response to the
     * client using APIs of RCSSetResponse class.
     *
     * <p>
     * ISetRequestHandler should be set to call this API. For setting the
     * listener call setSetRequestListener() API
     *
     * {@link RCSSetResponse} {@link #setSetRequestListener(ISetRequestHandler)}
     *
     */
    public void setSetRequestHandler() {
        if (null == m_setRequestListener) {
            Log.e(LOG_TAG, "setRequestListener is not set");
            return;
        } else {
            this.nativeSetSetRequestHandler(m_setRequestListener);
        }
    }

    /**
     * API for setting the Listener for a particular attribute update.
     *
     * @param key
     *            The interested attribute's key
     *
     *            <p>
     *            IAttributeUpdatedListener should be set to call this API. For
     *            setting the listener call setAttributeUpdatedListener() API
     */
    public void addAttributeUpdatedListener(String key) {
        if (null == m_attributeUpdatedListener) {
            Log.e(LOG_TAG, "addAttributeUpdatedListener is not set");
            return;
        } else {
            this.nativeAddAttributeUpdatedListener(key,
                    m_attributeUpdatedListener);
        }
    }

    /**
     * API for removing the handler for a particular attribute update.
     *
     * @param key
     *            The interested attribute's key
     *
     * @return true if requested attribute is removed successfully else false
     */
    public boolean removeAttributeUpdatedListener(String key) {
        return this.nativeRemoveAttributeUpdatedListener(key);
    }

    /**
     * API for notifying all observers of the RCSResourceObject with the updated
     * attributes value
     */
    public void notifyObservers() {
        Log.e(LOG_TAG, "notifyObservers called");
        this.nativeNotify();
    }

    /**
     * API for setting Auto notify policy
     *
     * @param policy
     *            policy to be set
     *
     *            {@link AutoNotifyPolicy}
     *
     */
    public void setAutoNotifyPolicy(AutoNotifyPolicy policy) {
        Log.e(LOG_TAG, "setAutoNotifyPolicy called");
        this.nativeSetAutoNotifyPolicy(policy.ordinal());
    }

    /**
     * API for getting auto notify policy
     *
     * @return AntoNotify policy
     *
     *         {@link AutoNotifyPolicy}
     *
     */
    public AutoNotifyPolicy getAutoNotifyPolicy() {
        Log.e(LOG_TAG, "getAutoNotifyPolicy called");
        return AutoNotifyPolicy.getAutoNotifyPolicy(this
                .nativeGetAutoNotifyPolicy());
    }

    /**
     * API for setting the policy for a setRequestHandler.
     *
     * @param policy
     *            policy to be set
     *
     *            {@link SetRequestHandlerPolicy}
     *
     */
    public void setSetRequestHandlerPolicy(SetRequestHandlerPolicy policy) {
        Log.e(LOG_TAG, "setSetRequestHandlerPolicy called");
        this.nativeSetSetRequestHandlerPolicy(policy.ordinal());
    }

    /**
     * API for getting the SetRequestHandler Policy.
     *
     * @return Property of setRequesthandler
     *
     *         {@link SetRequestHandlerPolicy}
     *
     */
    public SetRequestHandlerPolicy getSetRequestHandlerPolicy() {
        Log.e(LOG_TAG, "getSetRequestHandlerPolicy called");
        return SetRequestHandlerPolicy.getSetRequestHandlerPolicy(this
                .nativeGetSetRequestHandlerPolicy());
    }
}

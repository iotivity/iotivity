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

package org.iotivity.service.server;

import java.lang.ref.WeakReference;

import org.iotivity.service.RcsDestroyedObjectException;
import org.iotivity.service.RcsException;
import org.iotivity.service.RcsIllegalStateException;
import org.iotivity.service.RcsObject;
import org.iotivity.service.RcsPlatformException;
import org.iotivity.service.RcsResourceAttributes;
import org.iotivity.service.RcsValue;

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
 * @see Builder
 */
public final class RcsResourceObject extends RcsObject {
    /**
     * This class provides APIs for resource creation, setting properties and
     * attributes for the constructed resource. It provides the build() API
     * which
     * builds a resource and returns RCSResourceObject.
     *
     * {@link RcsResourceObject}
     */
    public static class Builder {
        private final String          mUri;
        private final String          mType;
        private final String          mInterface;
        private boolean               mIsObservable    = true;
        private boolean               mIsDiscovervable = true;
        private RcsResourceAttributes mAttributes;

        /**
         * Constructor
         *
         * @param uri
         *            Resource URI value to be set
         * @param resourceType
         *            Resource type value to be set
         * @param resourceInterface
         *            Interface value to be set
         *
         * @throws NullPointerException
         *             If any parameter is null.
         */
        public Builder(String uri, String resourceType,
                String resourceInterface) {

            if (uri == null) {
                throw new NullPointerException("uri is null.");
            }
            if (resourceType == null) {
                throw new NullPointerException("resourceType is null.");
            }
            if (resourceInterface == null) {
                throw new NullPointerException("resourceInterface is null.");
            }

            mUri = uri;
            mType = resourceType;
            mInterface = resourceInterface;
        }

        /**
         * Sets the discoverable(OC_DISCOVERABLE) property for the resource.
         *
         * @param isDiscoverable
         *            Whether to be discovered or not
         *
         */
        public Builder setDiscoverable(boolean isDiscoverable) {
            mIsDiscovervable = isDiscoverable;
            return this;
        }

        /**
         * Sets the observable(OC_OBSERVABLE) property of the resource.
         *
         * @param isObservable
         *            Whether to be observed or not
         *
         */
        public Builder setObservable(boolean isObservable) {
            mIsObservable = isObservable;
            return this;
        }

        /**
         * API for setting attributes of the resource.
         *
         * @param attributes
         *            Attributes to set
         *
         */
        public Builder setAttributes(RcsResourceAttributes attributes) {
            mAttributes = attributes;
            return this;
        }

        /**
         * Creates a RCSResourceObject.
         *
         * @throws RcsPlatformException
         *             If registering a resource is failed.
         *
         */
        public RcsResourceObject build() {
            return nativeBuild(mUri, mType, mInterface, mIsObservable,
                    mIsDiscovervable, mAttributes);
        }
    }

    /**
     * This provides the way to get the attributes of RcsResourceObject with
     * lock.
     * When a thread holds the lock, the other threads will be pending until the
     * lock is released by unlock.
     *
     * Here is the standard idiom for AttributesLock:
     *
     * <pre>
     * {@code
     * AttributesLock lock = rcsResourceObject.getAttributesLock();
     *
     * try
     * {
     *     lock.lock();
     *
     *     ....
     *
     *     lock.apply();
     * } finally {
     *     lock.unlock();
     * }
     * </pre>
     */
    public static class AttributesLock {

        private final WeakReference<RcsResourceObject> mResourceObjectRef;

        private RcsLockedAttributes mCurrentAttributes;

        private AttributesLock(RcsResourceObject resourceObj) {
            mResourceObjectRef = new WeakReference<RcsResourceObject>(
                    resourceObj);
        }

        private RcsResourceObject ensureResourceObject() throws RcsException {

            final RcsResourceObject object = mResourceObjectRef.get();
            if (object == null || object.isDestroyed()) {
                throw new RcsDestroyedObjectException(
                        "The object is already destroyed!");
            }

            return object;
        }

        /**
         * Locks the attributes of the RcsResourceObject and returns locked
         * attributes that can be modified until unlocked.
         *
         * @return Locked attributes.
         *
         * @throws RcsException
         *             If the RcsResourceObject is destroyed.
         */
        public RcsLockedAttributes lock() throws RcsException {
            return mCurrentAttributes = new RcsLockedAttributes(
                    ensureResourceObject());
        }

        /**
         * Changes the state to unlock of the attributes of the
         * RcsResourceObject.
         *
         */
        public void unlock() {
            if (mCurrentAttributes == null) return;

            mCurrentAttributes.setUnlockState();
            mCurrentAttributes = null;
        }

        /**
         * Applies the modified attributes to the RcsResourceObject.
         *
         * @throws RcsIllegalStateException
         *             If not in locked state.
         */
        public void apply() throws RcsIllegalStateException {
            if (mCurrentAttributes == null) {
                throw new RcsIllegalStateException("it is not locked state.");
            }
            mCurrentAttributes.apply();
        }
    }

    private static native RcsResourceObject nativeBuild(String uri,
            String resourceType, String resourceInterface, boolean isObservable,
            boolean isDiscoverable, RcsResourceAttributes attributes);

    private native void nativeSetAttribute(String key, RcsValue value);

    private native RcsValue nativeGetAttributeValue(String key);

    private native boolean nativeRemoveAttribute(String key);

    private native boolean nativeContainsAttribute(String key);

    private native RcsResourceAttributes nativeGetAttributes();

    private native boolean nativeIsObservable();

    private native boolean nativeIsDiscoverable();

    private native void nativeNotify();

    private native void nativeSetAutoNotifyPolicy(AutoNotifyPolicy policy);

    private native AutoNotifyPolicy nativeGetAutoNotifyPolicy();

    private native void nativeSetSetRequestHandlerPolicy(
            SetRequestHandlerPolicy policy);

    private native SetRequestHandlerPolicy nativeGetSetRequestHandlerPolicy();

    private native void nativeSetGetRequestHandler(GetRequestHandler handler);

    private native void nativeSetSetRequestHandler(SetRequestHandler handler);

    private native void nativeAddAttributeUpdatedListener(String key,
            OnAttributeUpdatedListener listener);

    private native boolean nativeRemoveAttributeUpdatedListener(String key);

    private RcsResourceObject() {
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
     * {@link RcsResourceObject}
     */
    public enum AutoNotifyPolicy {
        NEVER, /** < Never notify. */
        ALWAYS, /** < Always notify. */
        UPDATED;
        /** < When attributes are changed, notify. */
    }

    public enum SetRequestHandlerPolicy {

        NEVER, /**
                * < Server ignore when server is received set-request of
                * attributes of the new key.
                */
        ACCEPT;
        /**
         * < Server creates attributes of the new key When server is received
         * set-request of attributes of the new key.
         */
    }

    /**
     * Interface definition for a handler to be invoked when a get request is
     * received.
     */
    public interface GetRequestHandler {

        /**
         * Called when received a get request from the client.
         *
         * @param request
         *            Request information.
         * @param attributes
         *            The attributes of the request.
         *
         * @return A response to be sent.
         *
         * @see RcsGetResponse
         */
        RcsGetResponse onGetRequested(RcsRequest request,
                RcsResourceAttributes attributes);

    }

    /**
     * Interface definition for a handler to be invoked when a set request is
     * received.
     */
    public interface SetRequestHandler {

        /**
         * Called when received a set request from the client.
         *
         * @param request
         *            Request information.
         * @param attributes
         *            The attributes of the request.
         *            It will be applied to the RcsResourceObject.
         *
         * @return A response indicating how to handle this request.
         *
         * @see RcsSetResponse
         */
        RcsSetResponse onSetRequested(RcsRequest request,
                RcsResourceAttributes attributes);

    }

    /**
     * Interface definition for a callback to be invoked when an attribute is
     * updated.
     */
    public interface OnAttributeUpdatedListener {

        /**
         * Called when an attribute value is updated.
         *
         * @param oldValue
         *            The attribute value before updated.
         * @param newValue
         *            The current resource attribute value.
         */
        void onAttributeUpdated(RcsValue oldValue, RcsValue newValue);
    }

    private void assertAlive() throws RcsException {
        if (!hasHandle()) {
            throw new RcsDestroyedObjectException(
                    "The object is already destroyed!");
        }
    }

    /**
     * Sets a particular attribute value as a integer.
     * The thread-safety for attributes is taken care internally.
     *
     * @param key
     *            name of attribute(used to map the attribute value).
     * @param value
     *            value to be mapped against the key.
     *
     * @throws RcsException
     *             If the object is destroyed.
     * @throws NullPointerException
     *             If key or value is null.
     *
     */
    public void setAttribute(String key, RcsValue value) throws RcsException {
        assertAlive();

        if (key == null) throw new NullPointerException("key is null");
        if (value == null) throw new NullPointerException("value is null");

        nativeSetAttribute(key, value);
    }

    /**
     * Returns a copied attribute value associated with the supplied key.
     * The thread-safety for attributes is taken care internally.
     *
     * @param key
     *            key of an attribute.
     *
     * @return An attributes value.
     *
     * @throws RcsException
     *             If the object is destroyed.
     * @throws NullPointerException
     *             If key is null.
     */
    public RcsValue getAttributeValue(String key) throws RcsException {
        assertAlive();

        if (key == null) throw new NullPointerException("key is null");
        return nativeGetAttributeValue(key);
    }

    /**
     * API for removing a particular attribute of the resource.
     * The thread-safety for attributes is taken care internally.
     *
     * @param key
     *            Name of the attribute.
     *
     * @return If the key exist and matched attribute is deleted, return true.
     *
     * @throws RcsException
     *             If the object is destroyed.
     * @throws NullPointerException
     *             If key is null.
     */
    public boolean removeAttribute(String key) throws RcsException {
        assertAlive();

        if (key == null) throw new NullPointerException("key is null");
        return nativeRemoveAttribute(key);
    }

    /**
     * API for checking whether a particular attribute is there for a resource
     * or not.
     * The thread-safety for attributes is taken care internally.
     *
     * @param key
     *            Name of the attribute.
     *
     * @return If the key exist, return true.
     *
     * @throws RcsException
     *             If the object is destroyed.
     * @throws NullPointerException
     *             If key is null.
     */
    public boolean containsAttribute(String key) throws RcsException {
        assertAlive();

        if (key == null) throw new NullPointerException("key is null");
        return nativeContainsAttribute(key);
    }

    /**
     * Returns a copied attributes of the RCSResourceObject.
     * To modify the attributes, use AttrbutesLock.
     *
     * @throws RcsException
     *             If the object is destroyed.
     *
     * @see getAttributesLock
     */
    public RcsResourceAttributes getAttributes() throws RcsException {
        assertAlive();

        return nativeGetAttributes();
    }

    /**
     * Returns the AttributesLock for this RcsResourceObject.
     *
     * @throws RcsException
     *             If the object is destroyed.
     */
    public AttributesLock getAttributesLock() throws RcsException {
        assertAlive();

        return new AttributesLock(this);
    }

    /**
     * API for checking whether the particular resource is observable or not.
     *
     * @return true if this is observable. Otherwise false.
     *
     * @throws RcsException
     *             If the object is destroyed.
     */
    public boolean isObservable() throws RcsException {
        assertAlive();

        return nativeIsObservable();
    }

    /**
     * API for checking whether the particular resource is discoverable or not.
     *
     * @return true if this is discoverable. Otherwise false.
     *
     * @throws RcsException
     *             If the object is destroyed.
     */
    public boolean isDiscoverable() throws RcsException {
        assertAlive();

        return nativeIsDiscoverable();
    }

    /**
     * API for setting the resource's get request handler by the
     * developer/application. If developer set this handler then all get request
     * will come to the application and developer can send the response to the
     * client using APIs of RCSGetResponse class.
     *
     * @throws RcsException
     *             If the object is destroyed.
     * @throws NullPointerException
     *             If handler is null.
     *
     */
    public void setGetRequestHandler(GetRequestHandler handler)
            throws RcsException {
        assertAlive();

        if (handler == null) throw new NullPointerException("handler is null.");
        nativeSetGetRequestHandler(handler);
    }

    /**
     * API for setting the resource's set request handler by the
     * developer/application. If developer set this handler then all set request
     * will come to the application and developer can send the response to the
     * client using APIs of RCSSetResponse class.
     *
     * @throws RcsException
     *             If the object is destroyed.
     * @throws NullPointerException
     *             If handler is null.
     *
     */
    public void setSetRequestHandler(SetRequestHandler handler)
            throws RcsException {
        assertAlive();

        if (handler == null) throw new NullPointerException("handler is null.");

        nativeSetSetRequestHandler(handler);
    }

    /**
     * API for setting the Listener for a particular attribute update.
     *
     * @param key
     *            The interested attribute's key
     *
     * @throws NullPointerException
     *             If key or listener is null.
     *
     * @throws RcsException
     *             If the object is destroyed.
     */
    public void addAttributeUpdatedListener(String key,
            OnAttributeUpdatedListener listener) throws RcsException {
        assertAlive();

        if (key == null) {
            throw new NullPointerException("key is null.");
        }
        if (listener == null) {
            throw new NullPointerException("listener is null.");
        }

        nativeAddAttributeUpdatedListener(key, listener);
    }

    /**
     * API for removing the handler for a particular attribute update.
     *
     * @param key
     *            The interested attribute's key
     *
     * @return true if the requested attribute is removed successfully.
     *         Otherwise false.
     *
     * @throws RcsException
     *             If the object is destroyed.
     * @throws NullPointerException
     *             If key is null.
     */
    public boolean removeAttributeUpdatedListener(String key)
            throws RcsException {
        assertAlive();

        if (key == null) throw new NullPointerException("key is null");
        return nativeRemoveAttributeUpdatedListener(key);
    }

    /**
     * API for notifying all observers of the RCSResourceObject with the updated
     * attributes value
     *
     * @throws RcsException
     *             If the object is destroyed.
     * @throws RcsPlatformException
     *             If the operation failed.
     */
    public void notifyObservers() throws RcsException {
        assertAlive();

        nativeNotify();
    }

    /**
     * API for setting Auto notify policy
     *
     * @param policy
     *            policy to be set
     *
     * @throws RcsException
     *             If the object is destroyed.
     *
     */
    public void setAutoNotifyPolicy(AutoNotifyPolicy policy)
            throws RcsException {
        assertAlive();

        if (policy == null) throw new NullPointerException("policy is null");
        nativeSetAutoNotifyPolicy(policy);
    }

    /**
     * API for getting auto notify policy
     *
     * @return AntoNotify policy
     *
     * @throws RcsException
     *             If the object is destroyed.
     *
     */
    public AutoNotifyPolicy getAutoNotifyPolicy() throws RcsException {
        assertAlive();

        return nativeGetAutoNotifyPolicy();
    }

    /**
     * API for setting the policy for a setRequestHandler.
     *
     * @param policy
     *            policy to be set
     *
     * @throws RcsException
     *             If the object is destroyed.
     *
     */
    public void setSetRequestHandlerPolicy(SetRequestHandlerPolicy policy)
            throws RcsException {
        assertAlive();

        if (policy == null) throw new NullPointerException("policy is null");
        nativeSetSetRequestHandlerPolicy(policy);
    }

    /**
     * API for getting the SetRequestHandler Policy.
     *
     * @return Property of setRequesthandler
     *
     * @throws RcsException
     *             If the object is destroyed.
     *
     */
    public SetRequestHandlerPolicy getSetRequestHandlerPolicy()
            throws RcsException {
        assertAlive();

        return nativeGetSetRequestHandlerPolicy();
    }

    private boolean isDestroyed() {
        return !hasHandle();
    }

    /**
     * Unregister the resource and reclaims all resources used by this object.
     * This must be called if the resource is not used any longer.
     *
     */
    public void destroy() {
        super.dispose();
    }
}

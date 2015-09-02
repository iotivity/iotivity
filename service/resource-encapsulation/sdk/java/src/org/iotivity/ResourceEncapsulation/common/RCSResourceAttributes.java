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
 * @file    RCSResourceAttributes.java
 *
 * @brief     This file provides a class which represents the attributes for a resource.
 */

package org.iotivity.ResourceEncapsulation.common;

import org.iotivity.ResourceEncapsulation.client.RCSDiscoveryManager;
import org.iotivity.ResourceEncapsulation.client.RCSRemoteResourceObject;
import org.iotivity.ResourceEncapsulation.server.RCSResourceObject;

/**
 * RCSResourceAttributes represents the attributes for a resource.
 *
 * An attribute value can be one of various types : integer, double, boolean,
 * String<br/>
 *
 * <p>
 * If developer on client side wants to get the RCSResourceAttributes for the
 * resource of interest following are the steps: - first call the discover API
 * of DiscoveryManager class. - After getting the RemoteResourceObject, call
 * getRemoteAttributes() API of RemoteResourceObject class
 *
 * If the developer on server side wants to get the attributes following are the
 * steps: Call the getAttributes() API of RCSResourceObject class.
 *
 * {@link RCSDiscoveryManager} {@link RCSRemoteResourceObject}
 * {@link RCSResourceObject}
 */
public class RCSResourceAttributes {

    private long    m_nativeHandle;
    private boolean m_nativeNeedsDelete;

    // native methods
    private native Object nativeGetValueN(String key);

    private native void nativeSetValueInteger(String key, int value);

    private native void nativeSetValueDouble(String key, double value);

    private native void nativeSetValueBoolean(String key, boolean value);

    private native void nativeSetValueString(String key, String value);

    private native boolean nativeIsEmpty();

    private native int nativeSize();

    private native boolean nativeRemove(String key);

    private native boolean nativeHasAttribute(String key);

    private native void create();

    private native void dispose(boolean needsDelete);

    /**
     * Constructor for RCSResourceAttributes. Invokes create() which internally
     * constructs a new RCSResourceAttributes and sets its handle.
     *
     * <p>
     * Native RCSResourceAttributes object was created using "new" and needs to
     * be deleted
     */
    public RCSResourceAttributes() {
        create();
        this.m_nativeNeedsDelete = true;
    }

    private RCSResourceAttributes(long nativeHandle) {
        this.m_nativeHandle = nativeHandle;
        this.m_nativeNeedsDelete = false;
    }

    private RCSResourceAttributes(long nativeHandle, boolean nativeNeedsDelete) {
        this.m_nativeHandle = nativeHandle;
        this.m_nativeNeedsDelete = nativeNeedsDelete;
    }

    private <T> T getValue(String key) throws RCSException {
        Object obj = this.nativeGetValueN(key);
        T t = (T) obj;
        return t;
    }

    /**
     * get attribute value as an integer
     *
     * @param key
     *            - Key of the element.
     *
     * @return int - integer value of key provided.
     *
     */
    public int getValueInt(String key) {
        Integer value = 0;
        try {
            value = this.getValue(key);
        } catch (RCSException e) {
            e.printStackTrace();
        }
        return value;
    }

    /**
     * get attribute value as a double
     *
     * @param key
     *            - Key of the element.
     *
     * @return boolean - boolean value of key provided.
     *
     */
    public double getValueDouble(String key) {
        double value = 0;
        try {
            value = this.getValue(key);
        } catch (RCSException e) {
            e.printStackTrace();
        }
        return value;
    }

    /**
     * get attribute value as a boolean
     *
     * @param key
     *            - Key of the element.
     *
     * @return boolean - boolean value of key provided.
     *
     */
    public boolean getValueBool(String key) {
        Boolean value = false;
        try {
            value = this.getValue(key);
        } catch (RCSException e) {
            e.printStackTrace();
        }
        return value;
    }

    /**
     * get attribute value as a string
     *
     * @param key
     *            - Key of the element.
     *
     * @return String - String value of key provided.
     *
     */
    public String getValueString(String key) {
        String value = "";
        try {
            value = this.getValue(key);
        } catch (RCSException e) {
            e.printStackTrace();
        }
        return value;
    }

    /**
     * set an integer type attribute value.
     *
     * @param key
     *            - Key of the element to be added.
     *
     * @param value
     *            - integer value of key to be set.
     *
     */
    public void setValueInt(String key, int value) throws RCSException {
        try {
            this.nativeSetValueInteger(key, value);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * set a double attribute value.
     *
     * @param key
     *            - Key of the element to be added.
     *
     * @param value
     *            - double value of key to be set.
     *
     */
    public void setValueDouble(String key, double value) throws RCSException {
        try {
            this.nativeSetValueDouble(key, value);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * set a boolean attribute value.
     *
     * @param key
     *            - Key of the element to be added.
     *
     * @param value
     *            - integer value of key to be set.
     *
     */
    public void setValueBool(String key, boolean value) throws RCSException {
        try {
            this.nativeSetValueBoolean(key, value);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * set a string attribute value.
     *
     * @param key
     *            - Key of the element to be added.
     *
     * @param value
     *            - integer value of key to be set.
     *
     */
    public void setValueString(String key, String value) throws RCSException {
        try {
            this.nativeSetValueString(key, value);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * Returns whether attribute is empty.
     *
     * @return boolean
     */
    public boolean isEmpty() {
        return this.nativeIsEmpty();
    }

    /**
     * Returns the number of elements.
     *
     * @return boolean
     */
    public int size() {
        return this.nativeSize();
    }

    /**
     * Removes a single attribute
     *
     * @param key
     *            Key to be removed.
     *
     * @return true if an attribute is removed, false otherwise.
     */
    public boolean remove(String key) {
        return this.nativeRemove(key);
    }

    /**
     * Checks the container has an attribute with a Key equivalent to the
     * provided key.
     *
     * @param key
     *            Key to check.
     *
     * @return true if an attribute with requests key exists, false otherwise.
     */
    public boolean hasAttribute(String key) {
        return this.nativeHasAttribute(key);
    }

    @Override
    protected void finalize() throws Throwable {
        super.finalize();
        dispose(this.m_nativeNeedsDelete);
    }
}

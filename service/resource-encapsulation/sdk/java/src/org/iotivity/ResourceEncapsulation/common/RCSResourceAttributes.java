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
 * @file
 * This file provides a class which represents the attributes for a resource.
 */

package org.iotivity.ResourceEncapsulation.common;

import java.util.Vector;

import org.iotivity.ResourceEncapsulation.client.RCSDiscoveryManager;
import org.iotivity.ResourceEncapsulation.client.RCSRemoteResourceObject;
import org.iotivity.ResourceEncapsulation.server.RCSResourceObject;

/**
 * RCSResourceAttributes represents the attributes for a resource.
 *
 * An attribute value can be one of types specified in enum
 * "AttriubtesGetValueTypes"
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
 * {@link RCSResourceObject} {@link AttriubtesGetValueTypes}
 */
public class RCSResourceAttributes {

    private long    m_nativeHandle;
    private boolean m_nativeNeedsDelete;

    // native methods
    private native Object nativeGetValueN(String key, int type);

    private native void nativeSetValueInt(String key, int value);

    private native void nativeSetValueVectorOfInt(String key,
            Vector<Integer> values);

    private native void nativeSetValueVectorOfVectorOfInt(String key,
            Vector<Vector<Integer>> values);

    private native void nativeSetValueVectorOfVectorOfVectorOfInt(String key,
            Vector<Vector<Vector<Integer>>> values);

    private native void nativeSetValueDouble(String key, double value);

    private native void nativeSetValueVectorOfDouble(String key,
            Vector<Double> values);

    private native void nativeSetValueVectorOfVectorOfDouble(String key,
            Vector<Vector<Double>> values);

    private native void nativeSetValueVectorOfVectorOfVectorOfDouble(
            String key, Vector<Vector<Vector<Double>>> values);

    private native void nativeSetValueBoolean(String key, boolean value);

    private native void nativeSetValueVectorOfBoolean(String key,
            Vector<Boolean> values);

    private native void nativeSetValueVectorOfVectorOfBoolean(String key,
            Vector<Vector<Boolean>> values);

    private native void nativeSetValueVectorOfVectorOfVectorOfBoolean(
            String key, Vector<Vector<Vector<Boolean>>> values);

    private native void nativeSetValueString(String key, String value);

    private native void nativeSetValueVectorOfString(String key,
            Vector<String> values);

    private native void nativeSetValueVectorOfVectorOfString(String key,
            Vector<Vector<String>> values);

    private native void nativeSetValueVectorOfVectorOfVectorOfString(
            String key, Vector<Vector<Vector<String>>> values);

    private native void nativeSetValueAttributes(String key,
            RCSResourceAttributes value);

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

    /**
     * Get the Resource Attribute value.
     *
     * @param key
     *            Key for which value is requested
     *
     * @param type
     *            AttriubtesGetValueTypes
     *
     * @return Value of the attribute.
     *
     * @throws RCSException
     *             In case of bad Request
     *
     *             {@link AttriubtesGetValueTypes}
     */
    public <T> T getValue(String key, AttriubtesGetValueTypes type)
            throws RCSException {
        Object obj = this.nativeGetValueN(key, type.ordinal());
        T t = (T) obj;
        return t;
    }

    /**
     * Set Integer as Attribute value.
     *
     * @param key
     *            Key of the element to be added.
     *
     * @param value
     *            integer value to be set.
     *
     * @throws RCSException
     *             In case of bad Request
     */
    public void setValueInt(String key, int value) throws RCSException {
        try {
            this.nativeSetValueInt(key, value);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * Set Vector of Integer as Attribute Value.
     *
     * @param key
     *            Key of the element to be added.
     *
     * @param values
     *            Vector of integer to be set.
     *
     * @throws RCSException
     *             In case of bad Request
     */
    public void setValueVectorOfInt(String key, Vector<Integer> values) {
        try {
            this.nativeSetValueVectorOfInt(key, values);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * Set Vector of Vector of Integer as Attribute Value.
     *
     * @param key
     *            Key of the element to be added.
     *
     * @param values
     *            Vector of Vector of integer to be set.
     *
     * @throws RCSException
     *             In case of bad Request
     */
    public void setValueVectorOfVectorInt(String key,
            Vector<Vector<Integer>> values) {
        try {
            this.nativeSetValueVectorOfVectorOfInt(key, values);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * Set Vector Vector of Vector of Integer as Attribute Value.
     *
     * @param key
     *            Key of the element to be added.
     *
     * @param values
     *            Vector of Vector of Vector of integer to be set.
     *
     * @throws RCSException
     *             In case of bad Request
     */
    public void setValueVectorOfVectorOfVectorInt(String key,
            Vector<Vector<Vector<Integer>>> values) {
        try {
            this.nativeSetValueVectorOfVectorOfVectorOfInt(key, values);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * set a double attribute value.
     *
     * @param key
     *            Key of the element to be added.
     *
     * @param value
     *            double value of key to be set.
     *
     * @throws RCSException
     *             In case of bad Request
     */
    public void setValueDouble(String key, double value) throws RCSException {
        try {
            this.nativeSetValueDouble(key, value);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * Set Vector of double as Attribute Value.
     *
     * @param key
     *            Key of the element to be added.
     *
     * @param values
     *            Vector of double value of key to be set.
     *
     * @throws RCSException
     *             In case of bad Request
     */
    public void setValueVectorOfDouble(String key, Vector<Double> values) {
        try {
            this.nativeSetValueVectorOfDouble(key, values);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * Set Vector of Vector of double as Attribute Value.
     *
     * @param key
     *            Key of the element to be added.
     *
     * @param values
     *            Vector of vector of double value of key to be set.
     *
     * @throws RCSException
     *             In case of bad Request
     */
    public void setValueVectorOfVectorOfDouble(String key,
            Vector<Vector<Double>> values) {
        try {
            this.nativeSetValueVectorOfVectorOfDouble(key, values);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * Set Vector Vector of Vector of double as Attribute Value.
     *
     * @param key
     *            Key of the element to be added.
     *
     * @param values
     *            Vector of Vector of Vector of double value of key to be set.
     *
     * @throws RCSException
     *             In case of bad Request
     */
    public void setValueVectorOfVectorOfVectorOfDouble(String key,
            Vector<Vector<Vector<Double>>> values) {
        try {
            this.nativeSetValueVectorOfVectorOfVectorOfDouble(key, values);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * set a boolean attribute value.
     *
     * @param key
     *            Key of the element to be added.
     *
     * @param values
     *            boolean value of key to be set.
     *
     * @throws RCSException
     *             In case of bad Request
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
     * Set Vector of boolean as Attribute Value.
     *
     * @param key
     *            Key of the element to be added.
     *
     * @param values
     *            Vector of boolean value of key to be set.
     *
     * @throws RCSException
     *             In case of bad Request
     */
    public void setValueVectorOfBool(String key, Vector<Boolean> values) {
        try {
            this.nativeSetValueVectorOfBoolean(key, values);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * Set Vector of Vector of boolean as Attribute Value.
     *
     * @param key
     *            Key of the element to be added.
     *
     * @param values
     *            Vector of vector of boolean value of key to be set.
     *
     * @throws RCSException
     *             In case of bad Request
     */
    public void setValueVectorOfVectorOfBool(String key,
            Vector<Vector<Boolean>> values) {
        try {
            this.nativeSetValueVectorOfVectorOfBoolean(key, values);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * Set Vector Vector of Vector of boolean as Attribute Value.
     *
     * @param key
     *            Key of the element to be added.
     *
     * @param values
     *            Vector of Vector of Vector of boolean value of key to be set.
     *
     * @throws RCSException
     *             In case of bad Request
     */
    public void setValueVectorOfVectorOfVectorOfBool(String key,
            Vector<Vector<Vector<Boolean>>> values) {
        try {
            this.nativeSetValueVectorOfVectorOfVectorOfBoolean(key, values);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * set a string attribute value.
     *
     * @param key
     *            Key of the element to be added.
     *
     * @param value
     *            String value of key to be set.
     *
     * @throws RCSException
     *             In case of bad Request
     */
    public void setValueString(String key, String value) throws RCSException {
        try {
            this.nativeSetValueString(key, value);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * Set Vector of String as Attribute Value.
     *
     * @param key
     *            Key of the element to be added.
     *
     * @param values
     *            Vector of String value of key to be set.
     *
     * @throws RCSException
     *             In case of bad Request
     */
    public void setValueVectorOfString(String key, Vector<String> values) {
        try {
            this.nativeSetValueVectorOfString(key, values);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * Set Vector of Vector of String as Attribute Value.
     *
     * @param key
     *            Key of the element to be added.
     *
     * @param values
     *            Vector of vector of String value of key to be set.
     *
     * @throws RCSException
     *             In case of bad Request
     */
    public void setValueVectorOfVectorOfString(String key,
            Vector<Vector<String>> values) {
        try {
            this.nativeSetValueVectorOfVectorOfString(key, values);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * Set Vector Vector of Vector of String as Attribute Value.
     *
     * @param key
     *            Key of the element to be added.
     *
     * @param values
     *            Vector of Vector of Vector of String value of key to be set.
     *
     * @throws RCSException
     *             In case of bad Request
     */
    public void setValueVectorOfVectorOfVectorOfString(String key,
            Vector<Vector<Vector<String>>> values) {
        try {
            this.nativeSetValueVectorOfVectorOfVectorOfString(key, values);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * set a Attribute as attribute value.
     *
     * @param key
     *            Key of the element to be added.
     *
     * @param values
     *            RCSResourceAttributes to be set.
     *
     * @throws RCSException
     *             In case of bad Request
     */
    public void setValueAttributes(String key, RCSResourceAttributes value)
            throws RCSException {
        try {
            this.nativeSetValueAttributes(key, value);
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

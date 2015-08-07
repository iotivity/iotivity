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
 * getting the information associated with a particular attribute.
 */
package org.oic.simulator;

/**
 * This class represents an attribute of a resource. It has a set of native
 * methods for getting the attribute's information.
 */
public class SimulatorResourceAttribute {

    /**
     * Handle to the native object of the simulator resource attribute.
     */
    private long nativeHandle;

    public SimulatorResourceAttribute() {
    }

    /**
     * Constructor for SimulatorResourceAttribute.
     *
     * @param nativeHandle
     *            Handle to the native SimulatorResourceAttribute object.
     */
    private SimulatorResourceAttribute(long handle) {
        nativeHandle = handle;
    }

    /**
     * This constructor is used to create a new attribute.
     *
     * @param attrName
     *            Name of the attribute
     */
    public SimulatorResourceAttribute(String attrName) {
        create(attrName);
    }

    @Override
    protected void finalize() throws Throwable {
        dispose();
    }

    /**
     * This generic API is used to get the value of an attribute whose type is
     * given by the caller of the method.
     *
     * @param <T>
     *            This specifies the type in which the value has to be returned.
     *
     * @return The attribute's value in a specified type.
     */
    public <T> T getAttributeValue() {
        Object obj = getValue();
        @SuppressWarnings("unchecked")
        T t = (T) obj;
        return t;
    }

    /**
     * Native method to create a new attribute with the given name.
     *
     * @param attrName
     *            Name of the attribute.
     */
    public native void create(String attrName);

    /**
     * Native method for getting the attribute's name.
     *
     * @return Attribute's name
     */
    public native String getName();

    /**
     * Native method for getting the attribute's value.
     *
     * @return Attribute's value represented as {@link Object}.
     */
    public native Object getValue();

    /**
     * Native method for getting the number of values in the allowed values
     * list.
     *
     * @return Count of allowed values
     */
    public native int allowedValuesSize();

    /**
     * Native method for returning the string representation of the attribute's
     * value.
     *
     * @return Attribute's value as {@link String}.
     */
    public native String valueToString();

    /**
     * Native method for returning the string representation of the attribute's
     * allowed values.
     *
     * @return Attribute's allowed values as {@link String}.
     */
    public native String allowedValuesToString();

    /**
     * Native function to release the memory allocated to the native object for
     * SimulatorResourceAttribute.
     */
    private native void dispose();

}
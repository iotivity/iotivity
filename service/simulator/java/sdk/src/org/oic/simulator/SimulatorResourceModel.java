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
 * This file contains a class which has a set of native methods for accessing
 * the resource model.
 */
package org.oic.simulator;

import org.oic.simulator.ResourceAttribute;
import java.util.Map;

/**
 * This class represents the resource model of a resource and it provides a set
 * of native methods for accessing the resource model.
 */
public class SimulatorResourceModel {

    /**
     * Constructor.
     */
    public SimulatorResourceModel() {
        create();
    }

    /**
     * Method for adding an attribute whose value is of type int.
     * 
     * @param name
     *            Name of the attribute
     * @param value
     *            Value of the attribute
     */
    public void addAttribute(String name, int value) {
        addAttributeInt(name, value);
    }

    /**
     * Method for adding an attribute whose value is of type double.
     * 
     * @param name
     *            Name of the attribute
     * @param value
     *            Value of the attribute
     */
    public void addAttribute(String name, double value) {
        addAttributeDouble(name, value);
    }

    /**
     * Method for adding an attribute whose value is of type boolean.
     * 
     * @param name
     *            Name of the attribute
     * @param value
     *            Value of the attribute
     */
    public void addAttribute(String name, boolean value) {
        addAttributeBoolean(name, value);
    }

    /**
     * Method for adding an attribute whose value is of type string.
     * 
     * @param name
     *            Name of the attribute
     * @param value
     *            Value of the attribute
     */
    public void addAttribute(String name, String value) {
        addAttributeString(name, value);
    }

    /**
     * Method for getting the number of attributes this model comprised of.
     *
     * @return Number of attributes.
     */
    public native int size();

    /**
     * Method for getting all attributes.
     *
     * @return Map of attributes with attribute name as the key and its
     *         corresponding {@link ResourceAttribute} object as the value.
     */
    public native Map<String, ResourceAttribute> getAttributes();

    /**
     * Method for getting a specific attribute by its name.
     *
     * @param attrName
     *            Name of the attribute
     *
     * @return An object of {@link ResourceAttribute}.
     */
    public native ResourceAttribute getAttribute(String attrName);

    private native void addAttributeInt(String name, int value);

    private native void addAttributeDouble(String name, double value);

    private native void addAttributeBoolean(String name, boolean value);

    private native void addAttributeString(String name, String value);

    private SimulatorResourceModel(long nativeHandle) {
        this.nativeHandle = nativeHandle;
    }

    @Override
    protected void finalize() throws Throwable {
        dispose();
    }

    private native void create();

    private native void dispose();

    private long nativeHandle;
}
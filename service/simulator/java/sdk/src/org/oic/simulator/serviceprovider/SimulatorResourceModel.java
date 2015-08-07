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
package org.oic.simulator.serviceprovider;

import org.oic.simulator.SimulatorResourceAttribute;
import java.util.Map;

/**
 * This class represents the resource model of a resource and it provides a set
 * of native methods for accessing the resource model.
 */
public class SimulatorResourceModel {

    /**
     * Handle to the native object of the simulator resource model.
     */
    private long nativeHandle;

    /**
     * Constructor for SimulatorResourceModel.
     *
     * @param nativeHandle
     *            Handle to the native SimulatorResourceModel object.
     */
    private SimulatorResourceModel(long nativeHandle) {
        this.nativeHandle = nativeHandle;
    }

    @Override
    protected void finalize() throws Throwable {
        dispose();
    }

    /**
     * Native function for getting the total count of attributes in the resource
     * model.
     *
     * @return Total number of attributes.
     */
    public native int size();

    /**
     * Native function for getting all attributes of a resource.
     *
     * @return Map of attributes with attribute name as the key and its
     *         corresponding {@link SimulatorResourceAttribute} object as the
     *         value.
     */
    public native Map<String, SimulatorResourceAttribute> getAttributes();

    /**
     * Native function to get a specific attribute of a resource. It takes the
     * attribute name and returns an object of
     * {@link SimulatorResourceAttribute}.
     *
     * @param attrName
     *            Name of the attribute
     *
     * @return An object of SimulatorResourceAttribute.
     */
    public native SimulatorResourceAttribute getAttribute(String attrName);

    /**
     * Native function to get all the allowed values of a particular attribute.
     *
     * @param key
     *            Name of the attribute
     *
     * @return An array of all possible values as strings.
     */
    public native String[] getAllowedValues(String key);

    /**
     * Native function to release the memory allocated to the native object for
     * SimulatorResourceModel.
     */
    public native void dispose();
}
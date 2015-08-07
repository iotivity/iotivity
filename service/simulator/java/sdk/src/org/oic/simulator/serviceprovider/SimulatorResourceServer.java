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
 * This file contains a class which has a set of native methods for manipulating
 * the simulated resource by adding and removing attributes to its model,
 * automating the attribute value updates, and changing the range of
 * acceptable values of the attributes.
 */
package org.oic.simulator.serviceprovider;

import java.util.Vector;

import org.oic.simulator.IAutomation;

/**
 * This class represents a resource in the simulator. It provides a set of
 * native methods for manipulating the attributes.
 */
public class SimulatorResourceServer {

    private String resourceName;
    private String resourceURI;
    private String resourceType;
    private String interfaceType;

    private long   nativeHandle;

    /**
     * Constructor for SimulatorResourceServer.
     *
     * @param nativeHandle
     *            Handle to the native {@link SimulatorResourceServer} object.
     */
    private SimulatorResourceServer(long nativeHandle) {
        this.nativeHandle = nativeHandle;
    }

    /**
     * API to set the name of the resource. Example: Light, Fan, etc.
     *
     * @param resourceName
     *            Name of the resource.
     */
    public void setName(String resourceName) {
        this.resourceName = resourceName;
    }

    /**
     * API to set the URI of the resource. Example: /oic/light, /oic/fan, etc.
     *
     * @param resourceURI
     *            URI of the resource.
     */
    private void setURI(String resourceURI) {
        this.resourceURI = resourceURI;
    }

    /**
     * API to set the type of the resource. Example: oic.light, oic.fan, etc.
     *
     * @param resourceType
     *            Type of the resource.
     */
    private void setResourceType(String resourceType) {
        this.resourceType = resourceType;
    }

    /**
     * API to set the interface type of the resource. Example: oic.if.baseline,
     * oic.if.b, etc.
     *
     * @param interfaceType
     *            Interface type of the resource.
     */
    private void setInterfaceType(String interfaceType) {
        this.interfaceType = interfaceType;
    }

    /**
     * API to get the resource name. Example: Light, Fan, etc.
     *
     * @return Resource name.
     */
    public String getName() {
        return resourceName;
    }

    /**
     * API to get the resource URI. Example: /oic/light, /oic/fan, etc.
     *
     * @return Resource URI.
     */
    public String getURI() {
        return resourceURI;
    }

    /**
     * API to get the resource Type. Example: oic.light, oic.fan, etc.
     *
     * @return Resource type.
     */
    public String getResourceType() {
        return resourceType;
    }

    /**
     * API to get the interface type of the resource. Example: oic.if.baseline,
     * oic.if.b, etc.
     *
     * @return Interface type of the resource.
     */
    public String getInterfaceType() {
        return interfaceType;
    }

    @Override
    protected void finalize() throws Throwable {
        dispose();
    }

    /**
     * Native function to get the {@link SimulatorResourceModel} of the
     * corresponding resource.
     *
     * @return {@link SimulatorResourceModel} object on success, otherwise null.
     */
    public native SimulatorResourceModel getModel();

    /**
     * Native function to automatically update the value of an attribute from
     * its allowed values.
     *
     * @param attrName
     *            Name of the attribute
     *
     * @param index
     *            Index of the value in the allowed values.
     */
    public native void updateAttributeFromAllowedValues(String attrName,
            int index);

    /**
     * Native function to set the range of allowed values. This function is
     * intended to be used for integral type attributes.
     *
     * @param attrName
     *            Name of the attribute
     * @param min
     *            Minimum value in the range.
     * @param max
     *            Maximum value in the range.
     */
    public native void setRange(String attrName, int min, int max);

    /**
     * Native function to add an attribute whose value is of type int.
     *
     * @param key
     *            Name of the attribute
     * @param value
     *            Initial value of the attribute
     */
    public native void addAttributeInteger(String key, int value);

    /**
     * Native function to add an attribute whose value is of type double.
     *
     * @param key
     *            Name of the attribute
     * @param value
     *            Initial value of the attribute
     */
    public native void addAttributeDouble(String key, double value);

    /**
     * Native function to add an attribute whose value is of type Boolean.
     *
     * @param key
     *            Name of the attribute
     * @param value
     *            Initial value of the attribute
     */
    public native void addAttributeBoolean(String key, Boolean value);

    /**
     * Native function to add an attribute whose value is of type String.
     *
     * @param key
     *            Name of the attribute
     * @param value
     *            Initial value of the attribute
     */
    public native void addAttributeStringN(String key, String value);

    /**
     * Native function to update the value of an attribute whose value is of
     * type int.
     *
     * @param key
     *            Name of the attribute
     * @param value
     *            New value of the attribute
     */
    public native void updateAttributeInteger(String key, int value);

    /**
     * Native function to update the value of an attribute whose value is of
     * type double.
     *
     * @param key
     *            Name of the attribute
     * @param value
     *            New value of the attribute
     */
    public native void updateAttributeDouble(String key, double value);

    /**
     * Native function to update the value of an attribute whose value is of
     * type boolean.
     *
     * @param key
     *            Name of the attribute
     * @param value
     *            New value of the attribute
     */
    public native void updateAttributeBoolean(String key, Boolean value);

    /**
     * Native function to update the value of an attribute whose value is of
     * type String.
     *
     * @param key
     *            Name of the attribute
     * @param value
     *            New value of the attribute
     */
    public native void updateAttributeStringN(String key, String value);

    /**
     * Native function to set the allowed values of attribute whose value is of
     * type int.
     *
     * @param key
     *            Name of the attribute
     * @param allowedValues
     *            Allowed values of the attribute
     */
    public native void setAllowedValuesInteger(String key,
            Vector<Integer> allowedValues);

    /**
     * Native function to set the allowed values of attribute whose value is of
     * type double.
     *
     * @param key
     *            Name of the attribute
     * @param allowedValues
     *            Allowed values of the attribute
     */
    public native void setAllowedValuesDouble(String key,
            Vector<Double> allowedValues);

    /**
     * Native function to set the allowed values of attribute whose value is of
     * type String.
     *
     * @param key
     *            Name of the attribute
     * @param allowedValues
     *            Allowed values of the attribute
     */
    public native void setAllowedValuesStringN(String key,
            Vector<String> allowedValues);

    /**
     * Native function to start the resource level automation. This automation
     * involves automatically updating all the possible values for all the
     * attributes sequentially.
     *
     * @param typeOfAutomation
     *            Indicates whether the automation is one-time or recursive.
     * @param listener
     *            Listener to be notified when automation ends.
     *
     * @return Automation ID using which the automation can be stopped.
     */
    public native int startResourceAutomation(int typeOfAutomation,
            IAutomation listener);

    /**
     * Native function to start the attribute level automation. This automation
     * involves automatically updating all the possible values for a given
     * attribute sequentially.
     *
     * @param attrName
     *            Name of the attribute to be automated.
     * @param typeOfAutomation
     *            Indicates whether the automation is one-time or recursive.
     * @param listener
     *            Listener to be notified when automation ends.
     *
     * @return Automation ID using which the automation can be stopped.
     */
    public native int startAttributeAutomation(String attrName,
            int typeOfAutomation, IAutomation listener);

    /**
     * Native function to stop the automation.
     *
     * @param automationId
     *            Using which a specific automation can be stopped.
     */
    public native void stopAutomation(int automationId);

    /**
     * Native function to remove an attribute from the resource model.
     *
     * @param key
     *            Name of the attribute to be deleted
     */
    public native void removeAttribute(String key);

    /**
     * Native function to release the memory allocated to the native object for
     * SimulatorResourceServer.
     */
    private native void dispose();
}

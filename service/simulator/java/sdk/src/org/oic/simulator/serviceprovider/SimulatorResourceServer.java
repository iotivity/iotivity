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

package org.oic.simulator.serviceprovider;

import java.util.Vector;

import org.oic.simulator.IAutomation;
import org.oic.simulator.InvalidArgsException;
import org.oic.simulator.SimulatorException;
import org.oic.simulator.SimulatorResourceModel;

/**
 * This class represents a resource in the simulator. It provides a set of
 * native methods for manipulating the simulated resource by adding and removing
 * attributes to its model, automating the attribute value updates, and changing
 * the range of acceptable values of the attributes.
 */
public class SimulatorResourceServer {

    private String resourceName;
    private String resourceURI;
    private String resourceType;
    private String interfaceType;

    private long   nativeHandle;

    private SimulatorResourceServer(long nativeHandle) {
        this.nativeHandle = nativeHandle;
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
        try {
            dispose();
        } catch(Throwable t){
            throw t;
        } finally{
            System.out.println("Calling finalize of Super Class");
            super.finalize();
        }
    }

    /**
     * Native function to get the {@link SimulatorResourceModel} of the
     * corresponding resource.
     *
     * @return {@link SimulatorResourceModel} object on success, otherwise null.
     * 
     * @throws InvalidArgsException
     *             This exception will be thrown if the native resource object
     *             is invalid. values.
     * @throws SimulatorException
     *             This exception will be thrown for other errors.
     */
    public native SimulatorResourceModel getModel()
            throws InvalidArgsException, SimulatorException;

    /**
     * Native function to add an attribute whose value is of type int.
     *
     * @param key
     *            Name of the attribute.
     * @param value
     *            Initial value of the attribute.
     * 
     * @throws InvalidArgsException
     *             This exception will be thrown if any parameter has invalid
     *             values.
     * @throws SimulatorException
     *             This exception will be thrown for other errors.
     */
    public native void addAttributeInteger(String key, int value)
            throws InvalidArgsException, SimulatorException;

    /**
     * Native function to add an attribute whose value is of type double.
     *
     * @param key
     *            Name of the attribute.
     * @param value
     *            Initial value of the attribute.
     * 
     * @throws InvalidArgsException
     *             This exception will be thrown if any parameter has invalid
     *             values.
     * @throws SimulatorException
     *             This exception will be thrown for other errors.
     */
    public native void addAttributeDouble(String key, double value)
            throws InvalidArgsException, SimulatorException;

    /**
     * Native function to add an attribute whose value is of type boolean.
     *
     * @param key
     *            Name of the attribute.
     * @param value
     *            Initial value of the attribute.
     * 
     * @throws InvalidArgsException
     *             This exception will be thrown if any parameter has invalid
     *             values.
     * @throws SimulatorException
     *             This exception will be thrown for other errors.
     */
    public native void addAttributeBoolean(String key, boolean value)
            throws InvalidArgsException, SimulatorException;

    /**
     * Native function to add an attribute whose value is of type String.
     *
     * @param key
     *            Name of the attribute.
     * @param value
     *            Initial value of the attribute.
     * 
     * @throws InvalidArgsException
     *             This exception will be thrown if any parameter has invalid
     *             values.
     * @throws SimulatorException
     *             This exception will be thrown for other errors.
     */
    public native void addAttributeString(String key, String value)
            throws InvalidArgsException, SimulatorException;

    /**
     * Native function to update the value of an attribute whose value is of
     * type int.
     *
     * @param key
     *            Name of the attribute.
     * @param value
     *            New value of the attribute.
     * 
     * @throws InvalidArgsException
     *             This exception will be thrown if any parameter has invalid
     *             values.
     * @throws SimulatorException
     *             This exception will be thrown for other errors.
     */
    public native void updateAttributeInteger(String key, int value)
            throws InvalidArgsException, SimulatorException;

    /**
     * Native function to update the value of an attribute whose value is of
     * type double.
     *
     * @param key
     *            Name of the attribute.
     * @param value
     *            New value of the attribute.
     * 
     * @throws InvalidArgsException
     *             This exception will be thrown if any parameter has invalid
     *             values.
     * @throws SimulatorException
     *             This exception will be thrown for other errors.
     */
    public native void updateAttributeDouble(String key, double value)
            throws InvalidArgsException, SimulatorException;

    /**
     * Native function to update the value of an attribute whose value is of
     * type boolean.
     *
     * @param key
     *            Name of the attribute.
     * @param value
     *            New value of the attribute.
     * 
     * @throws InvalidArgsException
     *             This exception will be thrown if any parameter has invalid
     *             values.
     * @throws SimulatorException
     *             This exception will be thrown for other errors.
     */
    public native void updateAttributeBoolean(String key, boolean value)
            throws InvalidArgsException, SimulatorException;

    /**
     * Native function to update the value of an attribute whose value is of
     * type String.
     *
     * @param key
     *            Name of the attribute.
     * @param value
     *            New value of the attribute.
     * 
     * @throws InvalidArgsException
     *             This exception will be thrown if any parameter has invalid
     *             values.
     * @throws SimulatorException
     *             This exception will be thrown for other errors.
     */
    public native void updateAttributeString(String key, String value)
            throws InvalidArgsException, SimulatorException;

    /**
     * Native function to automatically update the value of an attribute from
     * its allowed values.
     *
     * @param attrName
     *            Name of the attribute.
     * @param index
     *            Index of the value in the allowed values.
     * 
     * @throws InvalidArgsException
     *             This exception will be thrown if any parameter has invalid
     *             values.
     * @throws SimulatorException
     *             This exception will be thrown for other errors.
     */
    public native void updateAttributeFromAllowedValues(String attrName,
            int index) throws InvalidArgsException, SimulatorException;

    /**
     * Native function to set the range of allowed values. This function is
     * intended to be used for integral type attributes.
     *
     * @param attrName
     *            Name of the attribute.
     * @param min
     *            Minimum value in the range.
     * @param max
     *            Maximum value in the range.
     * 
     * @throws InvalidArgsException
     *             This exception will be thrown if any parameter has invalid
     *             values.
     * @throws SimulatorException
     *             This exception will be thrown for other errors.
     */
    public native void setRange(String attrName, int min, int max)
            throws InvalidArgsException, SimulatorException;

    /**
     * Native function to set the allowed values of attribute whose value is of
     * type int.
     *
     * @param key
     *            Name of the attribute.
     * @param allowedValues
     *            Allowed values of the attribute.
     * 
     * @throws InvalidArgsException
     *             This exception will be thrown if any parameter has invalid
     *             values.
     * @throws SimulatorException
     *             This exception will be thrown for other errors.
     */
    public native void setAllowedValuesInteger(String key,
            Vector<Integer> allowedValues) throws InvalidArgsException,
            SimulatorException;

    /**
     * Native function to set the allowed values of attribute whose value is of
     * type double.
     *
     * @param key
     *            Name of the attribute.
     * @param allowedValues
     *            Allowed values of the attribute.
     * 
     * @throws InvalidArgsException
     *             This exception will be thrown if any parameter has invalid
     *             values.
     * @throws SimulatorException
     *             This exception will be thrown for other errors.
     */
    public native void setAllowedValuesDouble(String key,
            Vector<Double> allowedValues) throws InvalidArgsException,
            SimulatorException;

    /**
     * Native function to set the allowed values of attribute whose value is of
     * type String.
     *
     * @param key
     *            Name of the attribute.
     * @param allowedValues
     *            Allowed values of the attribute.
     * 
     * @throws InvalidArgsException
     *             This exception will be thrown if any parameter has invalid
     *             values.
     * @throws SimulatorException
     *             This exception will be thrown for other errors.
     */
    public native void setAllowedValuesString(String key,
            Vector<String> allowedValues) throws InvalidArgsException,
            SimulatorException;

    /**
     * Native function to start the resource level automation. This automation
     * involves automatically updating all the possible values for all the
     * attributes sequentially.
     *
     * @param typeOfAutomation
     *            {@link AutomationType} indicating whether the automation is
     *            one-time or recursive.
     * @param listener
     *            Listener to be notified when automation ends.
     *
     * @return Automation ID using which the automation can be stopped.
     * 
     * @throws InvalidArgsException
     *             This exception will be thrown if any parameter has invalid
     *             values.
     * @throws SimulatorException
     *             This exception will be thrown for other errors.
     */
    public int startResourceAutomation(AutomationType typeOfAutomation,
            IAutomation listener) throws InvalidArgsException,
            SimulatorException {
        return startResourceAutomation(typeOfAutomation.getValue(), listener);
    }

    private native int startResourceAutomation(int typeOfAutomation,
            IAutomation listener) throws InvalidArgsException,
            SimulatorException;

    /**
     * Native function to start the attribute level automation. This automation
     * involves automatically updating all the possible values for a given
     * attribute sequentially.
     *
     * @param attrName
     *            Name of the attribute to be automated.
     * @param typeOfAutomation
     *            {@link AutomationType} indicating whether the automation is
     *            one-time or recursive.
     * @param listener
     *            Listener to be notified when automation ends.
     *
     * @return Automation ID using which the automation can be stopped.
     * 
     * @throws InvalidArgsException
     *             This exception will be thrown if any parameter has invalid
     *             values.
     * @throws SimulatorException
     *             This exception will be thrown for other errors.
     */
    public int startAttributeAutomation(String attrName,
            AutomationType typeOfAutomation, IAutomation listener)
            throws InvalidArgsException, SimulatorException {
        return startAttributeAutomation(attrName, typeOfAutomation.getValue(),
                listener);
    }

    private native int startAttributeAutomation(String attrName,
            int typeOfAutomation, IAutomation listener)
            throws InvalidArgsException, SimulatorException;

    /**
     * Native function to stop the automation.
     *
     * @param automationId
     *            Using which a specific automation can be stopped.
     * 
     * @throws InvalidArgsException
     *             This exception will be thrown if the native resource object
     *             does not exist.
     * @throws SimulatorException
     *             This exception will be thrown for other errors.
     */
    public native void stopAutomation(int automationId)
            throws InvalidArgsException, SimulatorException;

    /**
     * Native function to remove an attribute from the resource model.
     *
     * @param key
     *            Name of the attribute to be deleted.
     * 
     * @throws InvalidArgsException
     *             This exception will be thrown either if the parameter has
     *             invalid value or the native resource object does not exist.
     * @throws SimulatorException
     *             This exception will be thrown for other errors.
     */
    public native void removeAttribute(String key) throws InvalidArgsException,
            SimulatorException;

    /**
     * Native function to get the details of a list of observers.
     * 
     * @return An array of {@link ObserverInfo} objects.
     * 
     * @throws InvalidArgsException
     *             This exception will be thrown if the native resource object
     *             does not exist.
     * @throws SimulatorException
     *             This exception will be thrown for other errors.
     */
    public native ObserverInfo[] getObserversList()
            throws InvalidArgsException, SimulatorException;

    /**
     * Native function to set callback to receive the clients information who are started or stopped observing the simulated resource.
     * 
     * @param observer
     *            Listener to be notified when clients start/stop observing.
     * 
     * @throws InvalidArgsException
     *             This exception will be thrown either if the parameter has
     *             invalid value or the native resource object does not exist.
     * @throws SimulatorException
     *             This exception will be thrown for other errors.
     */
    public native void setObserverCallback(IObserver observer)
            throws InvalidArgsException, SimulatorException;

    /**
     * Native function which sends notification to a specific observer.
     * 
     * @param id
     *            Observer's Id.
     * 
     * @throws InvalidArgsException
     *             This exception will be thrown if the native resource object
     *             does not exist.
     * @throws SimulatorException
     *             This exception will be thrown for other errors.
     */
    public native void notifyObserver(int id) throws InvalidArgsException,
            SimulatorException;

    /**
     * Native function which sends notification to all observers.
     * 
     * @throws InvalidArgsException
     *             This exception will be thrown if the native resource object
     *             does not exist.
     * @throws SimulatorException
     *             This exception will be thrown for other errors.
     */
    public native void notifyAllObservers() throws InvalidArgsException,
            SimulatorException;

    /**
     * Native function to release the memory allocated to the native object for
     * SimulatorResourceServer.
     */
    private native void dispose();
}

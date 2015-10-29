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

package org.oic.simulator.server;

import org.oic.simulator.AttributeValue;
import org.oic.simulator.InvalidArgsException;
import org.oic.simulator.SimulatorException;
import org.oic.simulator.SimulatorResourceAttribute;
import org.oic.simulator.SimulatorResourceModel;

public final class SimulatorSingleResource extends SimulatorResource {

    private SimulatorSingleResource(long nativeHandle) {
        mNativeHandle = nativeHandle;
    }

    /**
     * API to get the {@link SimulatorResourceModel} of the simulated resource.
     *
     * @return {@link SimulatorResourceModel} object on success, otherwise null.
     *
     * @throws SimulatorException
     *             This exception will be thrown if simulated resource is not
     *             proper.
     */
    public native SimulatorResourceModel getResourceModel() throws SimulatorException;

    /**
     * API to get attribute of resource.
     *
     * @param attrName
     *            Name of the attribute
     *
     * @return An object of {@link SimulatorResourceAttribute},
     *             or null if resource doest not have attribute of this name.
     *
     * @throws InvalidArgsException
     *             This exception will be thrown if the attribute name is
     *             invalid.
     * @throws SimulatorException
     *             This exception will be thrown either if the resource model is
     *             not found or for some general errors.
     */
    public native SimulatorResourceAttribute getAttribute(String attrName)
            throws InvalidArgsException, SimulatorException;

    /**
     * API to add an attribute to resource's representation model.
     *
     * @param attrName
     *            Name of the attribute.
     * @param attribute
     *            Attribute to be added to resource's representation model.
     *
     * @throws InvalidArgsException
     *             This exception will be thrown on invalid input.
     * @throws SimulatorException
     *             This exception will be thrown for other errors.
     */
    public native void addAttribute(SimulatorResourceAttribute attribute)
            throws InvalidArgsException, SimulatorException;

    /**
     * API to update the value of an attribute.
     *
     * @param attrName
     *            Name of the attribute.
     * @param value
     *            New value of the attribute.
     *
     * @throws InvalidArgsException
     *             This exception will be thrown on invalid input.
     * @throws SimulatorException
     *             This exception will be thrown for other errors.
     */
    public native void updateAttribute(String attrName, AttributeValue value)
            throws InvalidArgsException, SimulatorException;

    /**
     * API to remove an attribute from the simulated resource.
     *
     * @param attrName
     *            Name of the attribute to be deleted.
     *
     * @throws InvalidArgsException
     *             This exception will be thrown on invalid input.
     * @throws SimulatorException
     *             This exception will be thrown for other errors.
     */
    public native void removeAttribute(String attrName)
            throws InvalidArgsException, SimulatorException;

    /**
     * API to start the resource level automation. This automation involves
     * automatically updating all the possible values for all the attributes
     * sequentially.
     *
     * @param type
     *            {@link AutomationType} indicating whether the automation is
     *            one-time or recursive.
     * @param interval
     *            Interval time in milliseconds.
     * @param listener
     *            Listener to be notified when value updation ends.
     *
     * @return Automation ID using which the automation can be stopped.
     *
     * @throws InvalidArgsException
     *             This exception will be thrown on invalid input.
     * @throws SimulatorException
     *             This exception will be thrown for other errors.
     */
    public native int startResourceUpdation(AutoUpdateType type,
            int interval, AutoUpdateListener listener)
            throws InvalidArgsException, SimulatorException;

    /**
     * API to start the attribute level automation. This automation involves
     * automatically updating all the possible values for a given attribute
     * sequentially.
     *
     * @param attrName
     *            Name of the attribute to be automated.
     * @param type
     *            {@link AutomationType} indicating whether the automation is
     *            one-time or recursive.
     * @param interval
     *            Interval time in milliseconds.
     * @param listener
     *            Listener to be notified when automation ends.
     *
     * @return Automation ID using which the automation can be stopped.
     *
     * @throws InvalidArgsException
     *             This exception will be thrown on invalid input.
     * @throws SimulatorException
     *             This exception will be thrown for other errors.
     */
    public native int startAttributeUpdation(String attrName,
            AutoUpdateType type, int interval, AutoUpdateListener listener)
            throws InvalidArgsException, SimulatorException;

    /**
     * API to stop the automation based on automation id.
     *
     * @param id
     *            Using which a specific automation can be stopped.
     *
     * @throws SimulatorException
     *             This exception will be thrown for general errors.
     */
    public native void stopUpdation(int id)
            throws SimulatorException;

    /**
     * API to set listener for receiving notifications when resource's model
     * gets changed.
     *
     * @param listener
     *            {@link ResourceModelChangeListener}.
     *
     * @throws InvalidArgsException
     *             This exception will be thrown on invalid input.
     * @throws SimulatorException
     *             This exception will be thrown for other errors.
     */
    public native void setResourceModelChangeListener(ResourceModelChangeListener listener)
            throws InvalidArgsException, SimulatorException;
}

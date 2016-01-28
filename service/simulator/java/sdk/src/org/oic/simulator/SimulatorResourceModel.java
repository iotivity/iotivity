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

package org.oic.simulator;

import java.util.HashMap;
import java.util.Map;

/**
 * This class represents the resource model of a resource. A resource model
 * contains a set of attributes and their properties. It represents the complex
 * value type of an attribute.
 */
public class SimulatorResourceModel {

    private Map<String, AttributeValue>    mValues     = null;
    private Map<String, AttributeProperty> mProperties = null;

    /**
     * Constructs new {@link SimulatorResourceModel} object.
     */
    public SimulatorResourceModel() {
        mValues = new HashMap<>();
        mProperties = new HashMap<>();
    }

    /**
     * API to add an attribute to resource model.
     *
     * @param attrName
     *            Name of the attribute.
     * @param value
     *            Value of the attribute.
     *
     * @throws InvalidArgsException
     *             This exception will be thrown on invalid input.
     */
    public void addAttribute(String attrName, AttributeValue value)
            throws InvalidArgsException {
        if (null == attrName || attrName.isEmpty())
            throw new InvalidArgsException("Invalid attribute name!");

        if (null == value)
            throw new InvalidArgsException("Attribute value is null!");

        mValues.put(attrName, value);
    }

    /**
     * API to add an attribute to resource model.
     *
     * @param attribute
     *            {@link SimulatorResourceAttribute} to be added to resource
     *            model.
     *
     * @throws InvalidArgsException
     *             This exception will be thrown on invalid input.
     */
    public void addAttribute(SimulatorResourceAttribute attribute)
            throws InvalidArgsException {
        if (null == attribute || null == attribute.name()
                || attribute.name().isEmpty() || null == attribute.value())
            throw new InvalidArgsException("Invalid attribute!");

        mValues.put(attribute.name(), attribute.value());
    }

    /**
     * API to set attribute's property.
     *
     * @param attrName
     *            Name of the attribute.
     * @param property
     *            {@link AttributeProperty} to be set for attribute.
     *
     * @throws InvalidArgsException
     *             This exception will be thrown on invalid input.
     */
    public void setAttributeProperty(String attrName, AttributeProperty property)
            throws InvalidArgsException {
        if (null == attrName || attrName.isEmpty())
            throw new InvalidArgsException("Invalid attribute!");

        if (null == property)
            throw new InvalidArgsException("Invalid attribute property!");

        mProperties.put(attrName, property);
    }

    /**
     * API to set attribute's value.
     *
     * @param attrName
     *            Name of the attribute.
     * @param value
     *            {@link AttributeValue} to be set for attribute.
     *
     * @throws InvalidArgsException
     *             This exception will be thrown on invalid input.
     */
    public void setAttributeValue(String attrName, AttributeValue value)
            throws InvalidArgsException {
        if (null == attrName || attrName.isEmpty())
            throw new InvalidArgsException("Invalid attribute name!");

        if (null == value)
            throw new InvalidArgsException("Attribute value is null!");

        mValues.put(attrName, value);
    }

    /**
     * API to get all the attributes of resource model.
     *
     * @return Map of attributes with attribute name as key and its
     *         corresponding {@link SimulatorResourceAttribute} as value.
     */
    public Map<String, SimulatorResourceAttribute> getAttributes() {
        Map<String, SimulatorResourceAttribute> attributes = new HashMap<>();
        for (Map.Entry<String, AttributeValue> entry : mValues.entrySet()) {
            SimulatorResourceAttribute attribute = new SimulatorResourceAttribute(
                    entry.getKey(), entry.getValue(), mProperties.get(entry
                            .getKey()));
            attributes.put(entry.getKey(), attribute);
        }

        return attributes;
    }

    /**
     * API to get attribute by name.
     *
     * @param attrName
     *            Name of the attribute.
     *
     * @return {@link SimulatorResourceAttribute}.
     */
    public SimulatorResourceAttribute getAttribute(String attrName) {
        if (mValues.containsKey(attrName)) {
            SimulatorResourceAttribute attribute = new SimulatorResourceAttribute(
                    attrName, mValues.get(attrName), mProperties.get(attrName));
            return attribute;
        }

        return null;
    }

    /**
     * API to check whether resource model has an attribute with given name.
     *
     * @param attrName
     *            Name of the attribute.
     *
     * @return true if resource model has an attribute with given name,
     *         otherwise false.
     */
    public boolean containsAttribute(String attrName) {
        if (mValues.containsKey(attrName))
            return true;
        return false;
    }

    /**
     * API to get value type information of attribute.
     *
     * @param attrName
     *            Name of the attribute.
     *
     * @return Attribute value type information {@link AttributeValue.TypeInfo
     * 
     * }.
     */
    public AttributeValue.TypeInfo getAttributeType(String attrName) {
        if (mValues.containsKey(attrName))
            return mValues.get(attrName).typeInfo();
        return null;
    }

    /**
     * API to remove attribute from resource model.
     *
     * @param attrName
     *            Name of the attribute.
     *
     * @return true if resource model has an attribute with the given name,
     *         otherwise false.
     */
    public boolean removeAttribute(String attrName) {
        if (mValues.containsKey(attrName)) {
            mValues.remove(attrName);
            if (mProperties.containsKey(attrName))
                mProperties.remove(attrName);
            return true;
        }

        return false;
    }

    /**
     * API to get number of attributes present in resource model.
     *
     * @return Number of attributes present in resource model.
     */
    public int size() {
        return mValues.size();
    }

    /**
     * API to update the attribute values from given
     * {@link SimulatorResourceModel}.
     * 
     * @param resourceModel
     *            {@link SimulatorResourceModel} holding the new attribute
     *            values.
     */
    public void update(SimulatorResourceModel resourceModel) {
        if (null == resourceModel || 0 == resourceModel.size())
            return;

        for (Map.Entry<String, SimulatorResourceAttribute> entry : resourceModel
                .getAttributes().entrySet()) {
            SimulatorResourceAttribute newAttribute = entry.getValue();
            SimulatorResourceAttribute attribute = getAttribute(entry.getKey());
            if (null != newAttribute && null != attribute) {
                if (null != attribute.property()) {
                    AttributeValueValidation validation = new AttributeValueValidation(
                            attribute.property());
                    if (!validation.validate(newAttribute.value())) {
                        mValues.put(entry.getKey(), newAttribute.value());
                    }
                } else {
                    mValues.put(entry.getKey(), newAttribute.value());
                }
            }
        }
    }

    private SimulatorResourceModel(Map<String, AttributeValue> values,
            Map<String, AttributeProperty> properties) {
        mValues = values;
        mProperties = properties;
    }
}

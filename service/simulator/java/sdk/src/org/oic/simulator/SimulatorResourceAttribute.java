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

/**
 * This class represents the resource attribute which contains
 * attribute value and its property.
 */
public class SimulatorResourceAttribute {

    private String mName = null;
    private AttributeValue mValue = null;
    private AttributeProperty mProperty = null;

    /**
     * Constructs {@SimulatorResourceAttribute} with attribute name, value and its property.
     *
     * @param name
     *            Name of the attribute.
     * @param value
     *            Value of the attribute.
     * @param property
     *            Property of attribute value.
     */
    public SimulatorResourceAttribute(String name, AttributeValue value, AttributeProperty property) {
        mName = new String(name);
        mValue = value;
        mProperty = property;
    }

    /**
     * Constructs {@SimulatorResourceAttribute} with attribute name, value.
     *
     * @param name
     *            Name of the attribute.
     * @param value
     *            Value of the attribute.
     */
    public SimulatorResourceAttribute(String name, AttributeValue value) {
        mName = new String(name);
        mValue = value;
        mProperty = null;
    }

    /**
     * API to get name of attribute.
     *
     * @return Attribute's name.
     */
    public String name() {
        return mName;
    }

    /**
     * API to get value of attribute.
     *
     * @return Attribute's value {@AttributeValue}.
     */
    public AttributeValue value() {
        return mValue;
    }

    /**
     * API to get propety of attribute's value.
     *
     * @return Attribute's value property{@AttributeProperty}.
     */
    public AttributeProperty property() {
        return mProperty;
    }
}

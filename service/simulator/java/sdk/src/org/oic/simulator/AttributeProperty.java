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
 * This class represents the resource attribute's vaule property.
 */
public class AttributeProperty {

    private Type mType = Type.UNKNOWN;
    private double mMin = -1;
    private double mMax = -1;
    private AttributeValue[] mValueSet = null;

    /**
     * Enum to represent propety type.
     */
    public enum Type {
        UNKNOWN, RANGE, VALUESET
    }

    /**
     * Constructs {@AttributeProperty} of type {@AttributeProperty.Type.Range}
     * with min and max values.
     *
     * @param min
     *            Minimun value the attribute can have.
     * @param max
     *            Maximum value the attribute can have.
     */
    public AttributeProperty(double min, double max) {
        mType = Type.RANGE;
        mMin = min;
        mMax = max;
    }

    /**
     * Constructs {@AttributeProperty} of type {@AttributeProperty.Type.VALUESET}
     * with array of integers.
     *
     * @param values
     *            Attribute will be having one of the value from this array at any time.
     */
    public AttributeProperty(int[] values) {
        mType = Type.VALUESET;
        mValueSet = new AttributeValue[values.length];
        for (int i = 0; i < values.length; i++)
            mValueSet[i] = new AttributeValue(values[i]);
    }

    /**
     * Constructs {@AttributeProperty} of type {@AttributeProperty.Type.VALUESET}
     * with array of doubles.
     *
     * @param values
     *            Attribute will be having one of the value from this array at any time.
     */
    public AttributeProperty(double[] values) {
        mType = Type.VALUESET;
        mValueSet = new AttributeValue[values.length];
        for (int i = 0; i < values.length; i++)
            mValueSet[i] = new AttributeValue(values[i]);
    }

    /**
     * Constructs {@AttributeProperty} of type {@AttributeProperty.Type.VALUESET}
     * with array of Strings.
     *
     * @param values
     *            Attribute will be having one of the value from this array at any time.
     */
    public AttributeProperty(String[] values) {
        mType = Type.VALUESET;
        mValueSet = new AttributeValue[values.length];
        for (int i = 0; i < values.length; i++)
            mValueSet[i] = new AttributeValue(values[i]);
    }

    /**
     * API to get type of property.
     *
     * @return {@AttributeProperty.Type}.
     */
    public Type type() {
        return mType;
    }

    /**
     * API to get minimum value which was set as property.
     */
    public double min() {
        return mMin;
    }

    /**
     * API to get maximum value which was set as property.
     */
    public double max() {
        return mMax;
    }

    /**
     * API to get array of values which was set as property.
     *
     * @return array of {@AttributeValue}.
     */
    public AttributeValue[] valueSet() {
        return mValueSet;
    }

    private AttributeProperty(AttributeValue[] values) {
        mType = Type.VALUESET;
        mValueSet = values;
    }
}

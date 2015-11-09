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

package oic.simulator.clientcontroller.remoteresource;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import org.oic.simulator.AttributeProperty.Type;
import org.oic.simulator.AttributeValue;
import org.oic.simulator.AttributeValue.ValueType;
import org.oic.simulator.SimulatorResourceAttribute;

/**
 * This class represents an attribute in the remote resource.
 */
public class RemoteResourceAttribute {

    // Native object reference
    private SimulatorResourceAttribute resourceAttributeRef;

    private String                     attributeName;
    private Object                     attributeValue;

    private ValueType                  attValType;
    private ValueType                  attValBaseType;
    private int                        depth;

    private Type                       valuesType;
    private List<Object>               allowedValues;
    private Object                     minValue;
    private Object                     maxValue;

    public SimulatorResourceAttribute getResourceAttributeRef() {
        return resourceAttributeRef;
    }

    public void setResourceAttributeRef(
            SimulatorResourceAttribute resourceAttribute) {
        this.resourceAttributeRef = resourceAttribute;
    }

    public String getAttributeName() {
        return attributeName;
    }

    public void setAttributeName(String attributeName) {
        this.attributeName = attributeName;
    }

    public Object getAttributeValue() {
        return attributeValue;
    }

    public void setAttributeValue(Object attributeValue) {
        this.attributeValue = attributeValue;
    }

    public List<Object> getAllowedValues() {
        return allowedValues;
    }

    public void setAllowedValues(List<Object> allowedValues) {
        this.allowedValues = allowedValues;
    }

    public void setAllowedValues(String[] allowedValues) {
        List<Object> allowedValueList = null;
        if (null != allowedValues && allowedValues.length > 0) {
            allowedValueList = new ArrayList<Object>();
            for (String value : allowedValues) {
                allowedValueList.add(value);
            }
        }
        this.allowedValues = allowedValueList;
    }

    public Object getMinValue() {
        return minValue;
    }

    public void setMinValue(Object minValue) {
        this.minValue = minValue;
    }

    public Object getMaxValue() {
        return maxValue;
    }

    public void setMaxValue(Object maxValue) {
        this.maxValue = maxValue;
    }

    public static RemoteResourceAttribute clone(
            RemoteResourceAttribute attribute) {
        RemoteResourceAttribute clone = null;
        if (null != attribute) {
            clone = new RemoteResourceAttribute();
            clone.setAttributeName(attribute.getAttributeName());
            clone.setAttributeValue(attribute.getAttributeValue());
            clone.setAllowedValues(attribute.getAllowedValues());
            clone.setAttValBaseType(attribute.getAttValBaseType());
            clone.setAttValType(attribute.getAttValType());
            clone.setMinValue(attribute.getMinValue());
            clone.setMaxValue(attribute.getMaxValue());
            clone.setResourceAttributeRef(null);
        }
        return clone;
    }

    // This method gives all known possible values of the attribute in string
    // format.
    // It takes allowed values or range of values whichever is available
    public List<String> getAllValues() {
        List<String> valueList = new ArrayList<String>();
        if (null != allowedValues) {
            Iterator<Object> values = allowedValues.iterator();
            Object value;
            while (values.hasNext()) {
                value = values.next();
                if (null != value) {
                    valueList.add(String.valueOf(value));
                }
            }
        } else if (null != minValue && null != maxValue) {
            double min = (Double) minValue;
            double max = (Double) maxValue;
            for (double value = min; value <= max; value++) {
                valueList.add(String.valueOf(value));
            }
        }
        if (valueList.size() < 1 && null != attributeValue) {
            valueList.add(String.valueOf(attributeValue));
        }
        return valueList;
    }

    // Method added for debugging purpose
    public static void printAttributes(
            Map<String, RemoteResourceAttribute> attributeMap) {
        Iterator<String> itr = attributeMap.keySet().iterator();
        String attName;
        RemoteResourceAttribute att;
        while (itr.hasNext()) {
            attName = itr.next();
            att = attributeMap.get(attName);
            System.out.println("AttributeName:" + attName);
            System.out.println("AttributeValue:"
                    + att.getAttributeValue().toString());
            System.out.println("Allowed Values:" + att.getAllValues());
        }
    }

    public ValueType getAttValType() {
        return attValType;
    }

    public void setAttValType(ValueType attValType) {
        this.attValType = attValType;
    }

    public ValueType getAttValBaseType() {
        return attValBaseType;
    }

    public void setAttValBaseType(ValueType attValBaseType) {
        this.attValBaseType = attValBaseType;
    }

    public Type getValuesType() {
        return valuesType;
    }

    public void setValuesType(Type valuesType) {
        this.valuesType = valuesType;
    }

    public int getDepth() {
        return depth;
    }

    public void setDepth(int depth) {
        this.depth = depth;
    }
}
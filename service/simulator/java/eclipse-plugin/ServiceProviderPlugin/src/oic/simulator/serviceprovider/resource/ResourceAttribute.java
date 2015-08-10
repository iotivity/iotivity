package oic.simulator.serviceprovider.resource;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Set;

import org.oic.simulator.AutomationType;
import org.oic.simulator.SimulatorResourceAttribute;

public class ResourceAttribute {

    // Native object reference
    private SimulatorResourceAttribute resourceAttribute;

    private String                     attributeName;
    private Object                     attributeValue;
    private AttributeValueType         attributeType;
    private List<Object>               allowedValues;

    private Object                     minValue;
    private Object                     maxValue;

    private int                        automationId;

    private boolean                    automationInProgress;

    private int                        automationUpdateInterval;

    private AutomationType             automationType;

    public SimulatorResourceAttribute getResourceAttribute() {
        return resourceAttribute;
    }

    public void setResourceAttribute(
            SimulatorResourceAttribute resourceAttribute) {
        this.resourceAttribute = resourceAttribute;
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

    public AttributeValueType getAttributeType() {
        return attributeType;
    }

    public void setAttributeType(AttributeValueType attributeType) {
        this.attributeType = attributeType;
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

    public boolean isAutomationInProgress() {
        return automationInProgress;
    }

    public void setAutomationInProgress(boolean automationInProgress) {
        this.automationInProgress = automationInProgress;
    }

    public int getAutomationUpdateInterval() {
        return automationUpdateInterval;
    }

    public void setAutomationUpdateInterval(int automationUpdateInterval) {
        this.automationUpdateInterval = automationUpdateInterval;
    }

    public AutomationType getAutomationType() {
        return automationType;
    }

    public void setAutomationType(AutomationType automationType) {
        this.automationType = automationType;
    }

    public int getAutomationId() {
        return automationId;
    }

    public void setAutomationId(int automationId) {
        this.automationId = automationId;
    }

    public static ResourceAttribute clone(ResourceAttribute attribute) {
        ResourceAttribute clone = null;
        if (null != attribute) {
            clone = new ResourceAttribute();
            clone.setAttributeName(attribute.getAttributeName());
            clone.setAttributeValue(attribute.getAttributeValue());
            clone.setAllowedValues(attribute.getAllowedValues());
            clone.setAttributeType(attribute.getAttributeType());
            clone.setMinValue(attribute.getMinValue());
            clone.setMaxValue(attribute.getMaxValue());
            clone.setAutomationInProgress(attribute.isAutomationInProgress());
            clone.setAutomationType(attribute.getAutomationType());
            clone.setAutomationUpdateInterval(attribute
                    .getAutomationUpdateInterval());
            clone.setResourceAttribute(null);
        }
        return clone;
    }

    // This method gives all known possible values of the attribute
    // It takes allowed values or range of values whichever is available
    public Set<Object> getValues() {
        Set<Object> valueList = new HashSet<Object>();
        if (null != allowedValues) {
            Iterator<Object> values = allowedValues.iterator();
            while (values.hasNext()) {
                valueList.add(values.next());
            }
        } else if (null != minValue && null != maxValue) {
            if (attributeValue.getClass() == Integer.class) {
                int min = (Integer) minValue;
                int max = (Integer) maxValue;
                for (int value = min; value <= max; value++) {
                    valueList.add(value);
                }
            } else if (attributeValue.getClass() == Double.class) {
                double min = (Double) minValue;
                double max = (Double) maxValue;
                for (double value = min; value <= max; value++) {
                    valueList.add(value);
                }
            }
        }
        if (valueList.size() < 1) {
            valueList.add(attributeValue);
        }
        return valueList;
    }
}
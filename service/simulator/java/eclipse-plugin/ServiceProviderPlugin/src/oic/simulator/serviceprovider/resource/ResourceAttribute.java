package oic.simulator.serviceprovider.resource;

import java.util.List;

import org.iotivity.simulator.SimulatorResourceAttribute;

public class ResourceAttribute {

    // Native object reference
    private SimulatorResourceAttribute resourceAttribute;

    private String                     attributeName;
    private Object                     attributeValue;
    private AttributeValueType         attributeType;
    private List<Object>               allowedValues;

    private Object                     minValue;
    private Object                     maxValue;

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
}

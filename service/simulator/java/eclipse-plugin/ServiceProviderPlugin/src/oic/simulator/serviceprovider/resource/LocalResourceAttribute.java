package oic.simulator.serviceprovider.resource;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.oic.simulator.AutomationType;
import org.oic.simulator.ResourceAttribute;
import org.oic.simulator.ResourceAttribute.Type;

public class LocalResourceAttribute {

    // Native object reference
    private ResourceAttribute resourceAttribute;

    private String            attributeName;
    private Object            attributeValue;
    private Type              attValType;
    private Type              attValBaseType;
    private List<Object>      allowedValues;

    private Object            minValue;
    private Object            maxValue;

    private int               automationId;

    private boolean           automationInProgress;

    private int               automationUpdateInterval;

    private AutomationType    automationType;

    public ResourceAttribute getResourceAttribute() {
        return resourceAttribute;
    }

    public void setResourceAttribute(ResourceAttribute resourceAttribute) {
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

    public Type getAttValType() {
        return attValType;
    }

    public void setAttValType(Type attValType) {
        this.attValType = attValType;
    }

    public Type getAttValBaseType() {
        return attValBaseType;
    }

    public void setAttValBaseType(Type attValBaseType) {
        this.attValBaseType = attValBaseType;
    }

    public static LocalResourceAttribute clone(LocalResourceAttribute attribute) {
        LocalResourceAttribute clone = null;
        if (null != attribute) {
            clone = new LocalResourceAttribute();
            clone.setAttributeName(attribute.getAttributeName());
            clone.setAttributeValue(attribute.getAttributeValue());
            clone.setAllowedValues(attribute.getAllowedValues());
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

    // This method gives all known possible values of the attribute in string
    // format. It takes allowed values or range of values whichever is available
    public List<String> getAllValues() {
        List<String> valueList = new ArrayList<String>();
        if (null != allowedValues) {
            System.out.println("In getAllValues() - AllowedValues available");
            Iterator<Object> values = allowedValues.iterator();
            Object value;
            while (values.hasNext()) {
                value = values.next();
                if (null != value) {
                    valueList.add(String.valueOf(value));
                }
            }
        } else if (null != minValue && null != maxValue) {
            System.out.println("In getAllValues() - Range available");
            if (attValBaseType == Type.INT) {
                int min = (Integer) minValue;
                int max = (Integer) maxValue;
                for (int value = min; value <= max; value++) {
                    valueList.add(String.valueOf(value));
                }
            } else if (attValBaseType == Type.DOUBLE) {
                double min = (Double) minValue;
                double max = (Double) maxValue;
                for (double value = min; value <= max; value++) {
                    valueList.add(String.valueOf(value));
                }
            }
        }
        if (valueList.size() < 1 && null != attributeValue) {
            valueList.add(String.valueOf(attributeValue));
        }
        return valueList;
    }

    public void printAttributeDetails() {
        System.out.println("Attribute Name:" + attributeName);
        System.out.println("Attribute Value:" + attributeValue);
        System.out.println("Attribute Base Type:" + attValBaseType);
        System.out.println("Attribute Type:" + attValType);
        System.out.println("Allowed Values:" + allowedValues);
        System.out.println("Range:" + minValue + " to " + maxValue);
    }
}
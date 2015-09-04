package oic.simulator.serviceprovider.resource;

import java.util.List;

import org.oic.simulator.AutomationType;
import org.oic.simulator.ResourceAttribute;
import org.oic.simulator.ResourceAttribute.Range;
import org.oic.simulator.ResourceAttribute.Type;

public class LocalResourceAttribute {

    // Native object reference
    private ResourceAttribute resourceAttribute;

    private Object            attributeValue;
    private List<String>      attValues;

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
        return resourceAttribute.getName();
    }

    public Object getAttributeValue() {
        return attributeValue;
    }

    public void setAttributeValue(Object attributeValue) {
        this.attributeValue = attributeValue;
    }

    public Object[] getAllowedValues() {
        return resourceAttribute.getAllowedValues();
    }

    public Object getMinValue() {
        return resourceAttribute.getRange().getMin();
    }

    public Object getMaxValue() {
        return resourceAttribute.getRange().getMax();
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
        return resourceAttribute.getType();
    }

    public Type getAttValBaseType() {
        return resourceAttribute.getBaseType();
    }

    public List<String> getAttValues() {
        return attValues;
    }

    public void setAttValues(List<String> attValues) {
        this.attValues = attValues;
    }

    public void printAttributeDetails() {
        System.out.println("Attribute Name:" + resourceAttribute.getName());
        System.out.println("Attribute Value:" + resourceAttribute.getValue());
        System.out.println("Attribute Base Type:"
                + resourceAttribute.getBaseType());
        System.out.println("Attribute Type:" + resourceAttribute.getType());
        System.out.print("Allowed Values:");
        Object[] values = getAllowedValues();
        for (Object obj : values) {
            System.out.print(obj);
        }
        Range range = resourceAttribute.getRange();
        if (null != range) {
            System.out.println("Range:" + range.getMin() + " to "
                    + range.getMax());
        }
    }
}
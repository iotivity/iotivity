package org.oic.simulator.serviceprovider.test;

public class AutomationObject
{

    private String resourceURI;
    private int automationId;

    public void setResourceURI(String resourceURI)
    {
        this.resourceURI = resourceURI;
    }

    public String getResourceURI()
    {
        return resourceURI;
    }

    public void setAutomationId(int automationId)
    {
        this.automationId = automationId;
    }

    public int getAutomationId()
    {
        return automationId;
    }
}

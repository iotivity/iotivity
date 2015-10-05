package org.oic.simulator.test;

import org.oic.simulator.SimulatorResourceModel;

public class ResourceModelObject
{
    private String resourceURI;
    private SimulatorResourceModel resourceModel;

    public void setResourceURI(String resourceURI)
    {
        this.resourceURI = resourceURI;
    }

    public String getResourceURI()
    {
        return resourceURI;
    }

    public void setResourceModel(SimulatorResourceModel resourceModel)
    {
        this.resourceModel = resourceModel;
    }

    public SimulatorResourceModel getResourceModel()
    {
        return resourceModel;
    }
}

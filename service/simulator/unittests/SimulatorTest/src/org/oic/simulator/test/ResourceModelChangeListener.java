package org.oic.simulator.test;

import org.oic.simulator.SimulatorResourceModel;
import org.oic.simulator.serviceprovider.IResourceModelChangedListener;

public class ResourceModelChangeListener implements IResourceModelChangedListener
{

    private ResourceModelObject toReturn;

    public ResourceModelChangeListener(Object toReturn)
    {
        this.toReturn = (ResourceModelObject)toReturn;
    }

    @Override
    public void onResourceModelChanged(String resourceURI, SimulatorResourceModel resourceModel)
    {
        toReturn.setResourceURI(resourceURI);
        toReturn.setResourceModel(resourceModel);
    }
}

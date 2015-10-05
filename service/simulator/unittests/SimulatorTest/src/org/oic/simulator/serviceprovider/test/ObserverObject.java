package org.oic.simulator.serviceprovider.test;

import org.oic.simulator.serviceprovider.ObserverInfo;

public class ObserverObject
{

    private String resourceURI;
    private int state;
    private ObserverInfo observer;

    public void setResourceURI(String resourceURI)
    {
        this.resourceURI = resourceURI;
    }

    public String getResourceURI()
    {
        return resourceURI;
    }

    public void setState(int state)
    {
        this.state = state;
    }

    public int getState()
    {
        return state;
    }

    public void setObserver(ObserverInfo observer)
    {
        this.observer = observer;
    }

    public ObserverInfo getObserver()
    {
        return observer;
    }
}

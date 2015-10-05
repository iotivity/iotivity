package org.oic.simulator.clientcontroller.test;

import org.oic.simulator.SimulatorResourceModel;

public class ListenerObject
{

    private String uId;
    private SimulatorResourceModel representation;
    private Throwable ex;

    public void setuId(String uId)
    {
        this.uId = uId;
    }

    public String getuId()
    {
        return uId;
    }

    public void setRepresentation(SimulatorResourceModel representation)
    {
        this.representation = representation;
    }

    public SimulatorResourceModel getRepresentation()
    {
        return representation;
    }

    public void setEx(Throwable ex)
    {
        this.ex = ex;
    }

    public Throwable getEx()
    {
        return ex;
    }
}

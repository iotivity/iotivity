package org.oic.simulator.clientcontroller.test;

import org.oic.simulator.SimulatorResourceModel;

public class ObserveListenerObject
{

    private String uId;
    private SimulatorResourceModel representation;
    private int sequenceNumber;
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

    public void setSequenceNumber(int sequenceNumber)
    {
        this.sequenceNumber = sequenceNumber;
    }

    public int getSequenceNumber()
    {
        return sequenceNumber;
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

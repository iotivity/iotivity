package org.oic.simulator.clientcontroller.test;

import java.util.concurrent.CountDownLatch;

import org.oic.simulator.SimulatorResourceModel;
import org.oic.simulator.clientcontroller.IPutListener;

public class PutListener implements IPutListener
{

    private CountDownLatch lockObject;
    private ListenerObject listenerObject;

    public PutListener(CountDownLatch lockObject, ListenerObject listenerObject)
    {
        this.lockObject = lockObject;
        this.listenerObject = listenerObject;
    }

    @Override
    public void onPutCompleted(String uId, SimulatorResourceModel representation)
    {
        listenerObject.setuId(uId);
        listenerObject.setRepresentation(representation);

        lockObject.countDown();
    }

    @Override
    public void onPutFailed(Throwable ex)
    {
        listenerObject.setEx(ex);

        lockObject.countDown();
    }
}

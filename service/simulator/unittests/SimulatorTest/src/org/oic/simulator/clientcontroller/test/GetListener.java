package org.oic.simulator.clientcontroller.test;

import java.util.concurrent.CountDownLatch;

import org.oic.simulator.SimulatorResourceModel;
import org.oic.simulator.clientcontroller.IGetListener;

public class GetListener implements IGetListener
{

    private CountDownLatch lockObject;
    private ListenerObject getListenerObject;

    public GetListener(CountDownLatch lockObject, ListenerObject getListenerObject)
    {
        this.lockObject = lockObject;
        this.getListenerObject = getListenerObject;
    }

    @Override
    public void onGetCompleted(String uId, SimulatorResourceModel representation)
    {
        getListenerObject.setuId(uId);
        getListenerObject.setRepresentation(representation);

        lockObject.countDown();
    }

    @Override
    public void onGetFailed(Throwable ex)
    {
        getListenerObject.setEx(ex);

        lockObject.countDown();
    }
}

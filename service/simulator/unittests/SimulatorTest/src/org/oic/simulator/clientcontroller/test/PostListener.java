package org.oic.simulator.clientcontroller.test;

import java.util.concurrent.CountDownLatch;

import org.oic.simulator.SimulatorResourceModel;
import org.oic.simulator.clientcontroller.IPostListener;

public class PostListener implements IPostListener
{

    private CountDownLatch lockObject;
    private ListenerObject getListenerObject;

    public PostListener(CountDownLatch lockObject, ListenerObject getListenerObject)
    {
        this.lockObject = lockObject;
        this.getListenerObject = getListenerObject;
    }

    @Override
    public void onPostCompleted(String uId, SimulatorResourceModel representation)
    {
        getListenerObject.setuId(uId);
        getListenerObject.setRepresentation(representation);

        lockObject.countDown();
    }

    @Override
    public void onPostFailed(Throwable ex)
    {
        getListenerObject.setEx(ex);

        lockObject.countDown();
    }
}

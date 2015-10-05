package org.oic.simulator.clientcontroller.test;

import java.util.concurrent.CountDownLatch;

import org.oic.simulator.SimulatorResourceModel;
import org.oic.simulator.clientcontroller.IObserveListener;

public class ObserveListener implements IObserveListener
{

    private CountDownLatch lockObject;
    private ObserveListenerObject observeListenerObject;

    public ObserveListener(CountDownLatch lockObject, ObserveListenerObject observeListenerObject)
    {
        this.lockObject = lockObject;
        this.observeListenerObject = observeListenerObject;
    }

    @Override
    public void onObserveCompleted(String uId, SimulatorResourceModel representation, int sequenceNumber)
    {
        observeListenerObject.setuId(uId);
        observeListenerObject.setRepresentation(representation);
        observeListenerObject.setSequenceNumber(sequenceNumber);

        lockObject.countDown();
    }

    @Override
    public void onObserveFailed(Throwable ex)
    {
        observeListenerObject.setEx(ex);
        lockObject.countDown();
    }
}

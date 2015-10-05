package org.oic.simulator.serviceprovider.test;

import java.util.concurrent.CountDownLatch;

import org.oic.simulator.serviceprovider.IObserver;
import org.oic.simulator.serviceprovider.ObserverInfo;

public class Observer implements IObserver
{

    private CountDownLatch lockObject;
    private ObserverObject observerObject;

    public Observer(CountDownLatch lockObject, ObserverObject observerObject)
    {
        this.lockObject = lockObject;
        this.observerObject = observerObject;
    }

    @Override
    public void onObserverChanged(String resourceURI, int state, ObserverInfo observer)
    {
        observerObject.setState(state);
        observerObject.setResourceURI(resourceURI);
        observerObject.setObserver(observer);

        lockObject.countDown();
    }
}

package org.oic.simulator.test;

import java.util.concurrent.CountDownLatch;

import org.oic.simulator.clientcontroller.IFindResourceListener;
import org.oic.simulator.clientcontroller.SimulatorRemoteResource;

public class FindResourceListener implements IFindResourceListener
{

    private CountDownLatch lockObject;
    private SimulatorRemoteResourceObject simulatorRemoteResource;

    public FindResourceListener(CountDownLatch lockObject, SimulatorRemoteResourceObject simulatorRemoteResource)
    {
        this.lockObject = lockObject;
        this.simulatorRemoteResource = simulatorRemoteResource;
    }

    @Override
    public void onResourceCallback(SimulatorRemoteResource resource)
    {
        simulatorRemoteResource.setSimulatorRemoteResource(resource);

        lockObject.countDown();
    }
}

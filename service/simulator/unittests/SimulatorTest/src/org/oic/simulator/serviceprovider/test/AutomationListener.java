package org.oic.simulator.serviceprovider.test;

import java.util.concurrent.CountDownLatch;

import org.oic.simulator.IAutomation;

public class AutomationListener implements IAutomation
{

    private CountDownLatch lockObject;
    private AutomationObject automationObject;

    public AutomationListener(CountDownLatch lockObject, AutomationObject automationObject)
    {
        this.lockObject = lockObject;
        this.automationObject = automationObject;
    }

    @Override
    public void onAutomationComplete(String resourceURI, int automationId)
    {
        automationObject.setAutomationId(automationId);
        automationObject.setResourceURI(resourceURI);

        lockObject.countDown();
    }
}

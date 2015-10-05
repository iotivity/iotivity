package org.oic.simulator.clientcontroller.test;

import java.util.concurrent.CountDownLatch;

import org.oic.simulator.clientcontroller.IVerificationListener;

public class VerifyListener implements IVerificationListener
{

    private CountDownLatch lockObject;
    private VerifyListenerObject verifyListenerObject;

    public VerifyListener(CountDownLatch lockObject, VerifyListenerObject verifyListenerObject)
    {
        this.lockObject = lockObject;
        this.verifyListenerObject = verifyListenerObject;
    }

    @Override
    public void onVerificationStarted(String uId, int id)
    {
        verifyListenerObject.setId(id);
        verifyListenerObject.setuId(uId);
        verifyListenerObject.setWhichOne("started");

        lockObject.countDown();
    }

    @Override
    public void onVerificationAborted(String uId, int id)
    {
        verifyListenerObject.setId(id);
        verifyListenerObject.setuId(uId);
        verifyListenerObject.setWhichOne("aborted");

        lockObject.countDown();
    }

    @Override
    public void onVerificationCompleted(String uId, int id)
    {
        verifyListenerObject.setId(id);
        verifyListenerObject.setuId(uId);
        verifyListenerObject.setWhichOne("completed");

        lockObject.countDown();
    }
}

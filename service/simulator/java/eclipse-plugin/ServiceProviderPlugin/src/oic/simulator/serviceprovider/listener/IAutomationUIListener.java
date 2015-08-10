package oic.simulator.serviceprovider.listener;

public interface IAutomationUIListener {
    public void onResourceAutomationStart(String resourceURI);

    public void onAutomationComplete(String resourceURI, String attName);
}

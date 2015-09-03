package oic.simulator.serviceprovider.resource;

import org.oic.simulator.serviceprovider.ObserverInfo;

public class ObserverDetail {

    private ObserverInfo observerInfo;
    private boolean      isClicked;

    public ObserverInfo getObserverInfo() {
        return observerInfo;
    }

    public void setObserverInfo(ObserverInfo observerInfo) {
        this.observerInfo = observerInfo;
    }

    public boolean isClicked() {
        return isClicked;
    }

    public void setClicked(boolean isClicked) {
        this.isClicked = isClicked;
    }
}

package oic.simulator.serviceprovider.listener;

import oic.simulator.serviceprovider.resource.ModelChangeNotificationType;

public interface IResourceModelChangedUIListener {
    public void onResourceModelChange(
            ModelChangeNotificationType notificationType, String resourceURI);
}

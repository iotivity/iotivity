package oic.simulator.serviceprovider.listener;

import java.util.Set;

import oic.simulator.serviceprovider.resource.ModelChangeNotificationType;
import oic.simulator.serviceprovider.resource.LocalResourceAttribute;

public interface IResourceModelChangedUIListener {
    public void onResourceModelChange(
            ModelChangeNotificationType notificationType, String resourceURI,
            Set<LocalResourceAttribute> changeSet);
}

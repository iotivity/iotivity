package oic.simulator.serviceprovider.listener;

import java.util.List;

public interface IResourceListChangedListener {
    public void onResourceCreation(String resourceType, List<String> resourceURI);

    public void onResourceDeletion(String resourceType, String resourceURI);

    public void onResourceDeletionByType(String resourceType);

    public void onAllResourceDeletion();
}

package org.iotivity.resourcecontainer.bundle.api;


import java.util.List;

public interface BundleActivator 
{
    public void activateBundle();
    public void deactivateBundle();
    public void registerResource(BundleResource resource);
    public void unregisterResource(BundleResource resource);
    public List<ResourceConfig> getConfiguredBundleResources();
}

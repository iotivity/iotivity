package org.iotivity.resourcecontainer.bundle.api;

import java.util.List;
import java.util.Vector;

public class BaseActivator implements BundleActivator {
    private String bundleId;
    private Vector<BundleResource> bundleResources = new Vector<BundleResource>();

    public BaseActivator(String bundleId) {
        this.bundleId = bundleId;
        
    }

    static {
        try {
            System.loadLibrary("ResContainerLib");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void activateBundle() {

    }

    public void deactivateBundle() {
        System.out.println("Deactivating bundle (Base Activator).");
        for(BundleResource bundleResource : bundleResources){
            unregisterResource(bundleResource);
        }
    }

    public void registerResource(BundleResource resource) {
        bundleResources.add(resource);
        registerJavaResource(resource, resource.getAttributeKeys(), bundleId,
                resource.getURI(), resource.getResourceType(),
                resource.getName());
    }

    public List<ResourceConfig> getConfiguredBundleResources() {
        int configuredResources = getNumberOfConfiguredResources(bundleId);

        Vector<ResourceConfig> configs = new Vector<ResourceConfig>();

        for (int i = 0; i < configuredResources; i++) {
            String[] resourceParams = getConfiguredResourceParams(bundleId, i);
            ResourceConfig config = new ResourceConfig(resourceParams);
            configs.add(config);

        }
        return configs;
    }

    public void unregisterResource(BundleResource resource) {
        System.out.println("Making native call.");
        unregisterJavaResource(resource, resource.getURI());
    }

    public native void registerJavaResource(BundleResource resource,
            String[] attributes, String bundleId, String uri,
            String resourceType, String name);

    public native void unregisterJavaResource(BundleResource resource, String uri);

    public native int getNumberOfConfiguredResources(String bundleId);

    public native String[] getConfiguredResourceParams(String bundleId,
            int resId);

}

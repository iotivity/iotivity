package org.iotivity.bundle.hue;

import java.util.List;

import org.iotivity.resourcecontainer.bundle.api.BaseActivator;
import org.iotivity.resourcecontainer.bundle.api.ResourceConfig;

public class HueBundleActivator extends BaseActivator {
    private HueConnector connector;

    public HueBundleActivator(String bundleId) {
        super(bundleId);
    }

    public void activateBundle() {
        super.activateBundle();
        connector = new HueConnector();
        List<ResourceConfig> resourceConfig = getConfiguredBundleResources();

        for (ResourceConfig config : resourceConfig) {

            HueLightResource hueLightResource = new HueLightResource(connector,
                    config.getM_name(), config.getM_uri(),
                    config.getM_resourceType(), config.getM_address());
            registerResource(hueLightResource);
        }
    }

    public void deactivateBundle() {
        System.out.println("Deactivate bundle called.");
    }

    // test call
    public static void main(String[] args) {
        HueBundleActivator activator = new HueBundleActivator("oic.hue.bundle");
    }
}

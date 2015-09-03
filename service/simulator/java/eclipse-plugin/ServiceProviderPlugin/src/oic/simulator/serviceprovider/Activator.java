package oic.simulator.serviceprovider;

import java.net.URL;

import oic.simulator.serviceprovider.manager.LogManager;
import oic.simulator.serviceprovider.manager.ResourceManager;

import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.swt.graphics.Image;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.osgi.framework.BundleContext;

/**
 * The activator class controls the plug-in life cycle
 */
public class Activator extends AbstractUIPlugin {

    // The plug-in ID
    public static final String     PLUGIN_ID = "ServiceProviderPlugin"; //$NON-NLS-1$

    // The shared instance
    private static Activator       plugin;

    private static ResourceManager resourceManager;

    private static LogManager      logManager;

    public Activator() {
    }

    public void start(BundleContext context) throws Exception {
        super.start(context);
        plugin = this;
        setResourceManager(new ResourceManager());
        setLogManager(new LogManager());
    }

    public void stop(BundleContext context) throws Exception {
        plugin = null;

        // Stopping Resource Manager
        if (null != resourceManager) {
            resourceManager.shutdown();
            resourceManager = null;
        }
        // Stopping Log Manager
        if (null != logManager) {
            logManager.shutdown();
            logManager = null;
        }
        super.stop(context);
    }

    public static Activator getDefault() {
        return plugin;
    }

    public ResourceManager getResourceManager() {
        return resourceManager;
    }

    private static void setResourceManager(ResourceManager manager) {
        Activator.resourceManager = manager;
    }

    public LogManager getLogManager() {
        return logManager;
    }

    private static void setLogManager(LogManager logManager) {
        Activator.logManager = logManager;
    }

    public Image getImage(String imagePath) {
        Image img = null;
        if (null == imagePath || imagePath.length() < 1) {
            return img;
        }
        URL imageURL = getBundle().getEntry(imagePath);
        ImageDescriptor descriptor = ImageDescriptor.createFromURL(imageURL);
        return descriptor.createImage();
    }
}
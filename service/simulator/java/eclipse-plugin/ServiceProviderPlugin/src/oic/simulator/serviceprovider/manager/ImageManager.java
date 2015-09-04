package oic.simulator.serviceprovider.manager;

import java.net.URL;

import oic.simulator.serviceprovider.Activator;
import oic.simulator.serviceprovider.utils.Constants;

import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.resource.ImageRegistry;
import org.eclipse.swt.graphics.Image;
import org.osgi.framework.Bundle;

public class ImageManager {

    private static ImageManager imageManager;

    public static ImageManager getInstance() {
        if (null == imageManager) {
            imageManager = new ImageManager();
        }
        return imageManager;
    }

    static {
        ImageRegistry r = Activator.getDefault().getImageRegistry();
        Bundle bundle = Activator.getDefault().getBundle();

        r.put(Constants.CHECKED, ImageDescriptor.createFromURL(bundle
                .getEntry("icons/checked.gif")));
        r.put(Constants.UNCHECKED, ImageDescriptor.createFromURL(bundle
                .getEntry("icons/unchecked.gif")));

        r.put(Constants.NOTIFY_BUTTON_UNSELECTED, ImageDescriptor
                .createFromURL(bundle.getEntry("icons/button_free.PNG")));
        r.put(Constants.NOTIFY_BUTTON_SELECTED, ImageDescriptor
                .createFromURL(bundle.getEntry("icons/button_pressed.PNG")));

        // Resource icons based on the resource type
        r.put(Constants.OIC_LIGHT, ImageDescriptor.createFromURL(bundle
                .getEntry("/icons/light_16x16.png")));

        // Log View related icons
        r.put(Constants.DEBUG_LOG, ImageDescriptor.createFromURL(bundle
                .getEntry("/icons/debug_log.gif")));
        r.put(Constants.INFO_LOG, ImageDescriptor.createFromURL(bundle
                .getEntry("/icons/info_log.gif")));
        r.put(Constants.WARNING_LOG, ImageDescriptor.createFromURL(bundle
                .getEntry("/icons/warning_log.gif")));
        r.put(Constants.ERROR_LOG, ImageDescriptor.createFromURL(bundle
                .getEntry("/icons/error_log.gif")));
        r.put(Constants.UNKNOWN_LOG, ImageDescriptor.createFromURL(bundle
                .getEntry("/icons/unknown_log.gif")));
    }

    public static Image getImage(String imagePath) {
        Image img = null;
        if (null == imagePath || imagePath.length() < 1) {
            return img;
        }
        URL imageURL = Activator.getDefault().getBundle().getEntry(imagePath);
        ImageDescriptor descriptor = ImageDescriptor.createFromURL(imageURL);
        return descriptor.createImage();
    }
}

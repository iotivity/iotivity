package org.iotivity.service.sample.androidbundle;

import android.content.Context;
import android.util.Log;

import org.iotivity.service.resourcecontainer.AndroidBundleActivator;
import org.iotivity.service.resourcecontainer.AndroidBundleResource;
import org.iotivity.service.resourcecontainer.RcsResourceContainerBundleAPI;
import org.iotivity.service.resourcecontainer.ResourceConfig;

import java.util.List;

/**
 * Created by markus.jung on 11/7/2015.
 */
public class AndroidSampleActivator extends AndroidBundleActivator {

    public AndroidSampleActivator(RcsResourceContainerBundleAPI bundleAPI, Context appContext){
        super(bundleAPI, appContext);
        Log.d(AndroidSampleActivator.class.getName(), "Created activator instance");
    }

    @Override
    public void activateBundle() {
        Log.d(AndroidSampleActivator.class.getName(), "Activate bundle called");

        // make a test resource
        AndroidLightResource lightRes = new AndroidLightResource(this.appContext);
        lightRes.setURI("/android/light/1");
        bundleAPI.registerResource("oic.android.sample", lightRes);
    }

    @Override
    public void deactivateBundle() {

    }

    @Override
    public void createResource(ResourceConfig resourceConfig) {

    }

    @Override
    public void destroyResource(AndroidBundleResource androidBundleResource) {

    }

    @Override
    public List<ResourceConfig> getConfiguredBundleResources() {
        return null;
    }
}

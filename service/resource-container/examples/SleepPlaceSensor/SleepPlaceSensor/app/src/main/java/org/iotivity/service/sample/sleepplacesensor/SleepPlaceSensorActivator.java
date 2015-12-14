package org.iotivity.service.sample.sleepplacesensor;

import android.content.Context;
import android.util.Log;

import org.iotivity.service.resourcecontainer.AndroidBundleActivator;
import org.iotivity.service.resourcecontainer.AndroidBundleResource;
import org.iotivity.service.resourcecontainer.RcsResourceContainerBundleAPI;
import org.iotivity.service.resourcecontainer.ResourceConfig;

import java.util.List;

/**
 * Created by ikess on 15. 12. 13.
 */
public class SleepPlaceSensorActivator extends AndroidBundleActivator {
    public SleepPlaceSensorActivator(RcsResourceContainerBundleAPI bundleAPI, Context appContext){
        super(bundleAPI, appContext);
        Log.d(SleepPlaceSensorActivator.class.getName(), "Created activator instance");
    }

    @Override
    public void activateBundle() {
        Log.d(SleepPlaceSensorActivator.class.getName(), "Activate bundle called");

        // make a test resource
        AndroidLightResource humidityRes = new AndroidLightResource(this.appContext);
        humidityRes.setURI("/android/humidity/1");
        bundleAPI.registerResource("oic.sleepplace.sensor", humidityRes);

        AndroidLightResource temperatureRes = new AndroidLightResource(this.appContext);
        temperatureRes.setURI("/android/temperature/1");
        bundleAPI.registerResource("oic.sleepplace.sensor", temperatureRes);

        AndroidLightResource lightRes = new AndroidLightResource(this.appContext);
        lightRes.setURI("/android/light-intensity/1");
        bundleAPI.registerResource("oic.sleepplace.sensor", lightRes);

        LightSensor lightSensor = new LightSensor(this.appContext);
        lightSensor.setURI("/android/lightsensor/1");
        bundleAPI.registerResource("oic.sleepplace.sensor", lightSensor);

        AndroidEMCResource emcRes = new AndroidEMCResource(this.appContext);
        emcRes.setURI("/android/emc-intensity/1");
        bundleAPI.registerResource("oic.sleepplace.sensor", emcRes);

        EMCSensor emcSensor = new EMCSensor(this.appContext);
        emcSensor.setURI("/android/emcsensor/1");
        bundleAPI.registerResource("oic.sleepplace.sensor", emcSensor);

        /*AndroidSoundResource soundRes = new AndroidSoundResource(this.appContext);
        emcRes.setURI("/android/sound-dB/1");
        bundleAPI.registerResource("oic.sleepplace.sensor", emcRes);

        SoundSensor soundSensor = new SoundSensor(this.appContext);
        soundSensor.setURI("/android/soundsensor/1");
        bundleAPI.registerResource("oic.sleepplace.sensor", soundSensor);*/

        SleepPlaceSensorResource sleepPlaceResource = new SleepPlaceSensorResource(this.appContext);
        sleepPlaceResource.setURI("/android/sleepplaceindex/1");
        bundleAPI.registerResource("oic.sleepplace.sensor", sleepPlaceResource);
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

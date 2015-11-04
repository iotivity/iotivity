package org.iotivity.service.sample.androidbundle;

import android.content.Context;
import android.util.Log;

import org.iotivity.service.resourcecontainer.AndroidBundleResource;
import org.iotivity.service.resourcecontainer.RcsResourceAttributes;

/**
 * Created by markus.jung on 11/8/2015.
 */
public class AndroidLightResource extends AndroidBundleResource {
    private static final String LOG_TAG = AndroidBundleResource.class.getSimpleName();
    public AndroidLightResource(Context context){
        super(context);
        this.setResourceType("oic.r.light");
        this.setName("androidLightResource");
    }
    @Override
    protected void initAttributes() {
        this.m_attributes.put("on-off", true);
    }

    @Override
    public void handleSetAttributesRequest(RcsResourceAttributes attrs) {
        Log.i(LOG_TAG, "Set Attributes called with " + attrs);

        Log.i(LOG_TAG, "On-off value: " + attrs.get("on-off"));
    }

    @Override
    public RcsResourceAttributes handleGetAttributesRequest() {
        return null;
    }
}

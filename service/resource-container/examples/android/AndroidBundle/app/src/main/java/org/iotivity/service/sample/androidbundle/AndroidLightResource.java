package org.iotivity.service.sample.androidbundle;

import android.content.Context;
import android.util.Log;

import org.iotivity.service.resourcecontainer.AndroidBundleResource;
import org.iotivity.service.resourcecontainer.RcsResourceAttributes;

/**
 * Created by markus.jung on 11/8/2015.
 */
public class AndroidLightResource extends AndroidBundleResource {
    private static final String LOG_TAG = AndroidLightResource.class.getSimpleName();
    public AndroidLightResource(Context context){
        super(context);
        this.setResourceType("oic.r.light");
        this.setName("androidLightResource");
    }
    @Override
    protected void initAttributes() {
        this.m_attributes.put("on-off", false);
    }

    @Override
    public void handleSetAttributesRequest(RcsResourceAttributes attrs) {

        for(String key: attrs.keySet()){
            Log.i(LOG_TAG, " " + key + ": " + attrs.get(key));
        }

        Log.i(LOG_TAG, "Set Attributes called with " + attrs);

        Log.i(LOG_TAG, "On-off value: " + attrs.get("on-off"));
    }

    @Override
    public RcsResourceAttributes handleGetAttributesRequest() {
        Log.i(LOG_TAG, "Get Attributes called");
        Log.i(LOG_TAG, "Returning: ");
        for(String key: m_attributes.keySet()){
            Log.i(LOG_TAG, " " + key + ": " + m_attributes.get(key));
        }
        return this.m_attributes;
    }
}

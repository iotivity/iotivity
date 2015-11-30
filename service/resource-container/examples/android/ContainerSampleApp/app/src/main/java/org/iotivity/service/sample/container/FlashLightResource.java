package org.iotivity.service.sample.container;

import android.content.Context;
import android.util.Log;


import org.iotivity.service.resourcecontainer.AndroidBundleResource;
import org.iotivity.service.resourcecontainer.RcsResourceAttributes;

/**
 * Created by markus.jung on 11/2/2015.
 */
public class FlashLightResource extends AndroidBundleResource {
    private static final String LOG_TAG = FlashLightResource.class.getSimpleName();

    public FlashLightResource(Context context){
        super(context);
        this.setResourceType("oic.r.light");
        this.setName("flashLightResource");
    }

    @Override
    protected void initAttributes() {
        this.m_attributes.put("on-off", false);
    }

    @Override
    public void handleSetAttributesRequest(RcsResourceAttributes attrs) {
        Log.i(LOG_TAG, "Set Attributes called with " + attrs);

        Log.i(LOG_TAG, "New attributes value: " + attrs.get("on-off"));
        /*super.setAttribute(attribute ,value);
        if("on-off".equals(attribute)){
            if("true".equals(value)){
                Log.i(LOG_TAG, "Turn on");
            }
            else{
                Log.i(LOG_TAG, "Turn off");
            }
        }*/

    }

    @Override
    public RcsResourceAttributes handleGetAttributesRequest() {
        return this.m_attributes;
    }
}

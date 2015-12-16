package org.iotivity.service.resourcecontainer;

import java.util.HashMap;
import java.util.Set;
import java.util.Vector;
import android.content.Context;
import android.util.Log;


public abstract class AndroidBundleSoftSensorResource extends AndroidBundleResource {
    protected HashMap<String, RcsValue> m_mapInputData;
    
    public AndroidBundleSoftSensorResource(Context context) {
        super(context);
    }
    /**
     * Initialize the internal attribute structure.
     */
    protected abstract void onUpdatedInputResource(String attributeName,  Vector<RcsValue> values);

    /**
     * SoftSensor logic. Has to be provided by the soft sensor developer.
     * This function will be executed if an input attribute is updated.
     *
     * @return void
     */
    protected abstract void executeLogic();
}
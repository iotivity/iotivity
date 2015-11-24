package org.iotivity.service.sample.androidbundle;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorManager;
import android.util.Log;

import org.iotivity.service.resourcecontainer.AndroidBundleSoftSensorResource;
import org.iotivity.service.resourcecontainer.RcsResourceAttributes;
import org.iotivity.service.resourcecontainer.RcsValue;

import java.util.Vector;

/**
 * Created by markus.jung on 11/27/2015.
 */
public class DiscomfortIndexSensorResource extends AndroidBundleSoftSensorResource {
    private static final String LOG_TAG = DiscomfortIndexSensorResource.class.getSimpleName();

    public DiscomfortIndexSensorResource(Context context){
        super(context);
        this.setResourceType("oic.r.discomfortindex");
        this.setName("discomfortIndex");
    }

    @Override
    protected void initAttributes() {
        this.m_attributes.put("discomfortIndex", 0);
        this.m_attributes.put("humidity", 0);
        this.m_attributes.put("temperature",0);
    }

    @Override
    protected void onUpdatedInputResource(String attributeName, Vector<RcsValue> values) {
        m_mapInputData.put(attributeName, values.get(0));
    }

    @Override
    protected void executeLogic() {
        if(m_mapInputData.get("humidity") != null && m_mapInputData.get("temperature") != null){
            double dDI = 0.0;

            int t = m_mapInputData.get("temperature").asInt();
            int h = m_mapInputData.get("humidity").asInt();
            double F = (9.0 * (double) t) / 5.0 + 32.0;

            // calculation of discomfortIndex
            dDI = F - (F - 58.0) * (double)((100 - h) * 55) / 10000.0;

            this.setAttribute("temperature", new RcsValue(t));
            this.setAttribute("humidity", new RcsValue(h));
            this.setAttribute("discomfortIndex", new RcsValue(dDI));

            Log.i(LOG_TAG, "Discomfort Index" + dDI);
        }
    }

    @Override
    public void handleSetAttributesRequest(RcsResourceAttributes rcsResourceAttributes) {
        this.setAttributes(rcsResourceAttributes);
        executeLogic();
    }

    @Override
    public RcsResourceAttributes handleGetAttributesRequest() {
        return this.getAttributes();
    }
}

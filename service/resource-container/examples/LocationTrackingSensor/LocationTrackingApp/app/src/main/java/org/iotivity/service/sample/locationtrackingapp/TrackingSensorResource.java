//******************************************************************
//
// Copyright 2015 Duseok Kang (Seoul National University) All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

package org.iotivity.service.sample.locationtrackingapp;

import android.content.Context;
import android.hardware.SensorManager;
import android.util.Log;

import org.iotivity.service.resourcecontainer.AndroidBundleSoftSensorResource;
import org.iotivity.service.resourcecontainer.RcsResourceAttributes;
import org.iotivity.service.resourcecontainer.RcsValue;

import biz.source_code.dsp.filter.FilterPassType;
import biz.source_code.dsp.filter.IirFilter;
import biz.source_code.dsp.filter.IirFilterDesignExstrom;
import biz.source_code.dsp.filter.IirFilterCoefficients;

import java.util.HashMap;
import java.util.Vector;

public class TrackingSensorResource extends AndroidBundleSoftSensorResource {
    private final String LOG_TAG = "[" + Activator.class.getName() + "]";

    private float[] accel = new float[3];
    private float[] LinearAccel = new float[3];
    private float[] prevLinearAccel = new float[3];
    private float[] mag = new float[3];
    private float wifi_rssi = 0f;
    private float wifi_freq = 0f;

    private float[] gravity = new float[3];
    private float[] velocity = new float[3];
    private float[] loc = new float[3];
    private float[] rdisthistory = new float[10];
    private int rdistidx = 0;

    private float azimuth = 0f;
    private float rdist = 0f;

    private long prevTime = 0;
    private long time = System.currentTimeMillis();

    public TrackingSensorResource(Context appContext) {
        super(appContext);
        this.setResourceType("oic.r.trackingsensor");
        this.setName("TrackingSensor1");

        m_mapInputData = new HashMap<>();
    }

    @Override
    protected void onUpdatedInputResource(String attribute, Vector<RcsValue> values) {
        Log.i(LOG_TAG, "attribute - " + attribute + " value - " + values);

        m_mapInputData.put(attribute, values.get(0));
    }

    public float calculateDistance(float levelInDb, float freqInMHz)    {
        double exp = (27.55 - (20 * Math.log10(freqInMHz)) + Math.abs(levelInDb)) / 20.0;
        return (float)Math.pow(10.0, exp);
    }

    private void getSensorValue() {
        float alpha = 0.97f;

        IirFilterCoefficients filtercoeffx1 = IirFilterDesignExstrom.design(FilterPassType.lowpass, 2, 0.03, 0.1);
        final IirFilter lpfilterx1 = new IirFilter(filtercoeffx1);
        IirFilterCoefficients filtercoeffx2 = IirFilterDesignExstrom.design(FilterPassType.lowpass, 2, 0.03, 0.1);
        final IirFilter lpfilterx2 = new IirFilter(filtercoeffx2);
        IirFilterCoefficients filtercoeffy1 = IirFilterDesignExstrom.design(FilterPassType.lowpass, 2, 0.03, 0.1);
        final IirFilter lpfiltery1 = new IirFilter(filtercoeffy1);
        IirFilterCoefficients filtercoeffy2 = IirFilterDesignExstrom.design(FilterPassType.lowpass, 2, 0.03, 0.1);
        final IirFilter lpfiltery2 = new IirFilter(filtercoeffy2);
        IirFilterCoefficients filtercoeffz1 = IirFilterDesignExstrom.design(FilterPassType.lowpass, 2, 0.03, 0.1);
        final IirFilter lpfilterz1 = new IirFilter(filtercoeffz1);
        IirFilterCoefficients filtercoeffz2 = IirFilterDesignExstrom.design(FilterPassType.lowpass, 2, 0.03, 0.1);
        final IirFilter lpfilterz2 = new IirFilter(filtercoeffz2);

        accel[0]       = (float) m_mapInputData.get("accel_x").asDouble();
        accel[1]       = (float) m_mapInputData.get("accel_y").asDouble();
        accel[2]       = (float) m_mapInputData.get("accel_z").asDouble();
        accel[0]       = (float) lpfilterx1.step(accel[0]);
        accel[1]       = (float) lpfiltery1.step(accel[1]);
        accel[2]       = (float) lpfilterz1.step(accel[2]);

        LinearAccel[0] = (float) m_mapInputData.get("linearaccel_x").asDouble();
        LinearAccel[1] = (float) m_mapInputData.get("linearaccel_y").asDouble();
        LinearAccel[2] = (float) m_mapInputData.get("linearaccel_z").asDouble();
        LinearAccel[0] = (float) lpfilterx2.step(LinearAccel[0]);
        LinearAccel[1] = (float) lpfiltery2.step(LinearAccel[1]);
        LinearAccel[2] = (float) lpfilterz2.step(LinearAccel[2]);

        mag[0]         = alpha * mag[0] + (1 -alpha) * ((float) m_mapInputData.get("mag_x").asDouble());
        mag[1]         = alpha * mag[1] + (1 -alpha) * ((float) m_mapInputData.get("mag_y").asDouble());
        mag[2]         = alpha * mag[2] + (1 -alpha) * ((float) m_mapInputData.get("mag_z").asDouble());

        wifi_rssi    = (float) m_mapInputData.get("wifi_rssi").asDouble();
        wifi_freq    = (float) m_mapInputData.get("wifi_frequency").asDouble();

        gravity[0]     = accel[0] - LinearAccel[0];
        gravity[1]     = accel[1] - LinearAccel[1];
        gravity[2]     = accel[2] - LinearAccel[2];
    }

    protected float[] rotateByAzimuth(float x, float y, float azimuth){
        float realazi = 0.0f - azimuth;
        float[] result = new float[2];
        result[0] = x*(float)Math.cos(realazi) - y*(float)Math.sin(realazi);
        result[1] = x*(float)Math.sin(realazi) - y*(float)Math.cos(realazi);
        return result;
    }

    private void setAzimuth() {
        float R[] = new float[9];
        float I[] = new float[9];
        boolean success = SensorManager.getRotationMatrix(R, I, gravity, mag);
        if (success) {
            float orientation[] = new float[3];

            SensorManager.getOrientation(R, orientation);
            azimuth = orientation[0];
        }
    }

    private void removeNoise() {
        if (LinearAccel[0] < 0.05f && LinearAccel[0]> -0.05f) LinearAccel[0] = 0;
        if (LinearAccel[1] < 0.05f && LinearAccel[1]> -0.05f)
            LinearAccel[1] = 0;
        if (LinearAccel[2] < 0.05f && LinearAccel[2]> -0.05f)
            LinearAccel[2] = 0;
    }

    //calculating velocity of the device
    private void calcVelandLoc() {
        IirFilterCoefficients filtercoeffvelx = IirFilterDesignExstrom.design(FilterPassType.lowpass, 1, 0.2, 0.1);
        final IirFilter lpfiltervelx = new IirFilter(filtercoeffvelx);
        IirFilterCoefficients filtercoeffvely = IirFilterDesignExstrom.design(FilterPassType.lowpass, 1, 0.2, 0.1);
        final IirFilter lpfiltervely = new IirFilter(filtercoeffvely);
        IirFilterCoefficients filtercoeffvelz = IirFilterDesignExstrom.design(FilterPassType.lowpass, 1, 0.2, 0.1);
        final IirFilter lpfiltervelz = new IirFilter(filtercoeffvelz);
        time = System.currentTimeMillis();

        float time_slot = (time - prevTime) * 0.001f;

        float zLinearAccel = 0;
        float prevzLinearAccel = 0;
        if (LinearAccel[2] < 0.55f && LinearAccel[2]> -0.55f) zLinearAccel = 0;
        else if(LinearAccel[2] > 0.55f) zLinearAccel -= 0.55f;
        else zLinearAccel += 0.55f;

        if (prevLinearAccel[2] < 0.55f && prevLinearAccel[2]> -0.55f) prevzLinearAccel = 0;
        else if(prevLinearAccel[2] > 0.55f) prevzLinearAccel -= 0.55f;
        else prevzLinearAccel += 0.55f;

        if(LinearAccel[0] == prevLinearAccel[0] && LinearAccel[0] == 0) velocity[0] = 0;

        //second filtering: velocity value of X axis itself is filtered
        velocity[0] += (float)lpfiltervelx.step(time_slot *
                (rotateByAzimuth(LinearAccel[0], LinearAccel[1], azimuth)[0]
                 + rotateByAzimuth(prevLinearAccel[0], prevLinearAccel[1], azimuth)[0])/2.0);
        velocity[1] += (float)lpfiltervely.step(time_slot *
                (rotateByAzimuth(LinearAccel[0], LinearAccel[1], azimuth)[1]
                 + rotateByAzimuth(prevLinearAccel[0], prevLinearAccel[1], azimuth)[1])/2.0);
        velocity[2] += (float)lpfiltervelz.step(time_slot *
                (zLinearAccel + prevzLinearAccel)/2.0);

        float diff = 0;
        for(int i = 0; i < 9; i++){
            diff = (rdisthistory[i+1] - rdisthistory[i] > diff)
                ? (rdisthistory[i+1] - rdisthistory[i]) : diff;
        }
        if(diff < 0.7 && diff>-0.7){
            velocity[0] = 0;
            velocity[1] = 0;
        }

        loc[0] += (float)lpfiltervelx.step(time_slot * velocity[0]);
        loc[1] += (float)lpfiltervelx.step(time_slot * velocity[1]);
        float rsqare = loc[0]*loc[0] + loc[1]*loc[1];
        float beta = 0.35f;
        if(rsqare != 0){
            float adjustingfactor = (float)Math.sqrt(rdist*rdist/rsqare);
            loc[0] = (1 - beta)*loc[0] + beta*adjustingfactor*loc[0];
            loc[1] = (1 - beta)*loc[1] + beta*adjustingfactor*loc[1];
        }
        loc[2] += (float)lpfiltervelx.step(time_slot * velocity[2]);

        prevTime = time;
        prevLinearAccel = LinearAccel;
    }

    private void getDistenceFromWifi() {
        rdist = calculateDistance(wifi_rssi, wifi_freq);
        rdisthistory[(rdistidx++) % 10] = rdist;
    }

    protected void executeLogic() {
        Log.d(LOG_TAG, "in accel - " + accel.toString() + " linear - " + LinearAccel.toString() +
                " mag - " + mag.toString() + " wifi - " + wifi_rssi);
        Log.d(LOG_TAG, "accel - " + m_mapInputData.get("accel_x") + " linear - "
                + m_mapInputData.get("linearaccel_x") + " mag - " + m_mapInputData.get("mag_x"));
        if (m_mapInputData.get("accel_x") != null &&
                m_mapInputData.get("linearaccel_x") != null &&
                m_mapInputData.get("mag_x") != null) {
            getSensorValue();

            setAzimuth();

            removeNoise();

            getDistenceFromWifi();

            calcVelandLoc();

            Log.d(LOG_TAG, " loc_x - " + loc[0] + " loc_y - " + loc[1]);
            this.setAttribute("loc_x", new RcsValue(loc[0]));
            this.setAttribute("loc_y", new RcsValue(loc[1]));
        }
    }

    @Override
    protected void initAttributes() {
        this.m_attributes.put("accel_x", 0);
        this.m_attributes.put("accel_y", 0);
        this.m_attributes.put("accel_z", 0);

        this.m_attributes.put("linearaccel_x", 0);
        this.m_attributes.put("linearaccel_y", 0);
        this.m_attributes.put("linearaccel_z", 0);

        this.m_attributes.put("mag_x", 0);
        this.m_attributes.put("mag_y", 0);
        this.m_attributes.put("mag_z", 0);

        this.m_attributes.put("wifi_rssi", 0);
        this.m_attributes.put("wifi_frequency", 0);

        this.m_attributes.put("loc_x", 0);
        this.m_attributes.put("loc_y", 0);
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

package org.iotivity.service.sample.sleepplacesensor;

import org.iotivity.service.resourcecontainer.AndroidBundleResource;
import org.iotivity.service.resourcecontainer.RcsResourceAttributes;
import org.iotivity.service.resourcecontainer.RcsValue;

import android.content.Context;
import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.util.Log;

/**
 * Created by ikess on 15. 12. 14.
 */
public class SoundSensor extends AndroidBundleResource {
    private static final String LOG_TAG = SoundSensor.class.getSimpleName();
    private int minSize = AudioRecord.getMinBufferSize(8000, AudioFormat.CHANNEL_IN_MONO, AudioFormat.ENCODING_PCM_16BIT);
    private AudioRecord ar = new AudioRecord(MediaRecorder.AudioSource.MIC, 8000, AudioFormat.CHANNEL_IN_MONO, AudioFormat.ENCODING_PCM_16BIT, minSize);

    short[] buffer = new short[minSize];

    public SoundSensor(Context context){
        super(context);
        this.setResourceType("oic.r.soundsensor");
        this.setName("soundSensor");

        while (true) {
            ar.startRecording();
            ar.read(buffer, 0, minSize);
            int max = 0;
            for (short s : buffer) {
                if (Math.abs(s) > max) {
                    max = Math.abs(s);
                }
            }
            setAttribute("sound-dB", new RcsValue(max) , true);
            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }


    @Override
    protected void initAttributes() {
        this.m_attributes.put("emc-intensity", 0);
    }

    @Override
    public void handleSetAttributesRequest(RcsResourceAttributes attrs) {
        Log.i(LOG_TAG, "Set Attributes called with ");
        for(String key: attrs.keySet()){
            Log.i(LOG_TAG, " " + key + ": " + attrs.get(key));
        }
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

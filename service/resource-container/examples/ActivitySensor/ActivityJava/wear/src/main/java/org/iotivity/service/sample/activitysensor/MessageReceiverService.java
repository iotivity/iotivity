package org.iotivity.service.sample.activitysensor;

import android.content.Intent;

import com.google.android.gms.wearable.MessageEvent;
import com.google.android.gms.wearable.WearableListenerService;

public class MessageReceiverService extends WearableListenerService {
    @Override
    public void onMessageReceived(MessageEvent messageEvent) {
        if (messageEvent.getPath().equals("START"))
            startService(new Intent(this, SensorService.class));

        if (messageEvent.getPath().equals("STOP"))
            stopService(new Intent(this, SensorService.class));
    }
}

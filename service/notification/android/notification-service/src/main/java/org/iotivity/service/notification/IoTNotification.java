package org.iotivity.service.notification;

import android.util.Log;
import org.iotivity.service.notification.NSMessage;
import org.iotivity.service.notification.NSConsumer;
import org.iotivity.service.notification.NSSync;

public class IoTNotification {
    public IoTNotification(){}

    static {
        System.loadLibrary("notification_provider_jni");
    }

    public void onLogPrinted(String log)
    {
        Log.i("JNI_NS_CALLBACK", "[C->J] Log: " + log);
    }

    public void onSubscribeRequest(String consumer)
    {
        Log.i("JNI_NS_CALLBACK", "[C->J] Consumer IP: " + consumer);
    }

    public void onMessageSync(String sync)
    {
        Log.i("JNI_NS_CALLBACK", "[C->J] Sync Status" + sync);
    }
      
    public native int NSStartProvider(boolean access);
    public native int NSStopProvider();
    public native int NSSendNotification(NSMessage message);
    public native int NSProviderReadCheck(NSMessage message);
    public native int NSAccept(NSConsumer consumer, boolean accepted);
}


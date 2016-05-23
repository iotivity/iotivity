package org.iotivity.service.notification;

import android.util.Log;
import org.iotivity.service.notification.NSMessage;
import org.iotivity.service.notification.NSConsumer;
import org.iotivity.service.notification.NSSync;

public class IoTNotification
{
    public IoTNotification()
    {
    }

    static
    {
        System.loadLibrary("notification_provider_jni");
    }

    public native int NSStartProvider(boolean access,
        NSSubscriptionListner subscriptionListener,
        NSSynchListner syncListener);
    public native int NSStopProvider();
    public native int NSSendNotification(NSMessage message);
    public native int NSProviderReadCheck(NSMessage message);
    public native int NSAccept(NSConsumer consumer, boolean accepted);

    public interface NSSubscriptionListner {
        public void OnNSSubscribedEvent(String consumerId);
    }

    public interface NSSynchListner {
        public void OnNSSynchronizedEvent(String messageId, int syncState);
    }
}


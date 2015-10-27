package org.iotivity.service.notificationservice;

import android.test.AndroidTestCase;
import android.util.Log;

import org.iotivity.service.notificationservice.impl.ImageNotification;
import org.iotivity.service.notificationservice.impl.TextNotification;
import org.iotivity.service.notificationservice.impl.VideoNotification;

public class NotificationConsumerTest extends AndroidTestCase {

    private NotificationService notificationService;
    private NotificationServiceCallbackHandler callbackHandler;
    boolean resourceDiscovered = false;
    Object lock=new Object();
    String TAG="NSConsumerTest";

    public void testStartSetupWithConsumer() {

        notificationService = NotificationService.getInstance();
        if (notificationService == null) {
            assertTrue(false);
            return;
        }


        final NotificationCallback notificationCallback = new NotificationCallback() {
            @Override
            public void onNotificationReceived(NotificationObject notificationObject) {
                Utility.toNotify(lock);
                Log.d(TAG, "onNotificationReceived Inside+lock released");


                String sender = notificationObject.getNotifcationSender();
                if (sender.equals(""))
                    assertTrue(false);
                final int id = notificationObject.getNotifcationId();
                if (id == 0)
                    assertTrue(false);
                final int ttl = notificationObject.getNotifcationTTL();
                if (ttl == 0)
                    assertTrue(false);
                if (notificationObject.getNotificationType() == NotificationType.TYPE_TEXT) {

                    TextNotification textNotification = (TextNotification) notificationObject;
                    if (textNotification.getNotificationMessage().equals(""))
                        assertTrue(false);

                } else if (notificationObject.getNotificationType() == NotificationType.TYPE_IMAGE) {
                    ImageNotification imageNotification = (ImageNotification) notificationObject;
                    if (imageNotification.getNotificationMessage().equals(""))
                        assertTrue(false);
                    if (imageNotification.getNotificationImageUrl().equals(""))
                        assertTrue(false);

                } else if (notificationObject.getNotificationType() == NotificationType.TYPE_VIDEO) {
                    VideoNotification videoNotification = (VideoNotification) notificationObject;
                    if (videoNotification.getNotificationVideoUrl().equals(""))
                        assertTrue(false);
                }

                assertTrue(true);
            }

            @Override
            public void onResourceDiscoveredCallback(String resourceURI, int statuscode) {

                assertTrue(true);
                Log.d(TAG,"Resource Discovered,Auto subscribing");
                notificationService.subscribesnotifications(0);

            }

            @Override
            public void showMessage(String message) {

            }
        };

        if (notificationCallback == null) {
            assertTrue(false);
            return;
        }

        callbackHandler = NotificationServiceCallbackHandler.getInstance();

        if (callbackHandler == null) {
            assertTrue(false);
            return;
        }

        callbackHandler.setNotificationCallback(notificationCallback);

        notificationService.discover();


        Thread t = new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    Thread.sleep(5000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                if (!resourceDiscovered) {
                    Log.d("NSConsumerTest", "Resource not discovered after 10 seconds,");
                    assertTrue(false);
                }

            }
        });
        t.start();

        Utility.toWait(lock);
    }
}

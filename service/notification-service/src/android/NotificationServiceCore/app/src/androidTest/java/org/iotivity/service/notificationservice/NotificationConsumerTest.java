package org.iotivity.service.notificationservice;

import android.test.AndroidTestCase;

public class NotificationConsumerTest extends AndroidTestCase {

    private NotificationService notificationService;
    private  NotificationServiceCallbackHandler callbackHandler;
    boolean resourceDiscovered=false;
    public void testStartSetupWithConsumer(){

        notificationService=NotificationService.getInstance();
        if(notificationService==null) {
            assertTrue(false);
            return;
        }



        NotificationCallback notificationCallback=new NotificationCallback() {
            @Override
            public void onNotificationReceived(NotificationObject notificationObject) {

            }

            @Override
            public void onResourceDiscoveredCallback(String resourceURI, int statuscode) {

                assertTrue(true);
                notificationService.subscribesnotifications(0);

            }

            @Override
            public void showMessage(String message) {

            }
        };

        if(notificationCallback==null)
            {
                assertTrue(false);
                return;
            }

        callbackHandler=NotificationServiceCallbackHandler.getInstance();

        if(callbackHandler==null)
        {
            assertTrue(false);
            return;
        }

        callbackHandler.setNotificationCallback(notificationCallback);

        notificationService.discover();

        Thread t=new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    Thread.sleep(5000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                if(resourceDiscovered)
                {}

            }
        });
    }
}

package org.iotivity.service.notificationservice;

import android.test.AndroidTestCase;
import android.util.Log;

import org.iotivity.service.notificationservice.impl.ImageNotification;
import org.iotivity.service.notificationservice.impl.TextNotification;
import org.iotivity.service.notificationservice.impl.VideoNotification;

public class NotificationProducerTest extends AndroidTestCase {

    NotificationService notificationService;
    NotificationServiceCallbackHandler callbackHandler;
    String TestDeviceName="TestDevice";
    Object lock=new Object();
    final String TAG="NSProducerTest";

    NotificationProducerCallback callback=new NotificationProducerCallback() {
        @Override
        public void onNotificationAcknowledgementReceieved(int id, int read_status) {
            Log.d(TAG,"Notification with id= "+ id+" read");
        }
    };
    public void testStartSetupWithProducer()
    {
        notificationService=NotificationService.getInstance();

        if(notificationService==null)
            assertTrue(false);

        callbackHandler=NotificationServiceCallbackHandler.getInstance();

        if(callbackHandler==null)
            assertTrue(false);

        callbackHandler.setNotificationCallback(callback);
        try {
            notificationService.startnotificationproducer(TestDeviceName);
        }
        catch (Exception e){assertTrue(false);}


        Log.d(TAG,"Waiting for Subscribers");
        //Utility.toWait(lock, 5000);

        TextNotification textNotification=new TextNotification();
        textNotification.setNotifcationSender(TestDeviceName);
        textNotification.setNotificationMessage("Welcome");

        try {
            notificationService.sendnotification(textNotification);
        }
        catch (Exception e){assertTrue(false);}
        Utility.toWait(lock, 5000);
        Log.d(TAG, "Text notification sent");
        assertTrue(true);

        ImageNotification imageNotification=new ImageNotification();
        imageNotification.setNotifcationSender(TestDeviceName);
        imageNotification.setNotificationMessage("Image Test");
        imageNotification.setNotificationImageUrl("http://theselby.com/media/2_28_11_SupremeCoffee7598.jpg");

        try {
            notificationService.sendnotification(imageNotification);
        }
        catch(Exception e){assertTrue(false);}

        Utility.toWait(lock, 2000);
        Log.d(TAG, "Image notification sent");
        assertTrue(true);

        VideoNotification videoNotification=new VideoNotification();
        videoNotification.setNotifcationSender(TestDeviceName);
        videoNotification.setNotificationVideoUrl("https://www.youtube.com/watch?v=CppgLnNM1PE");

        try {
            notificationService.sendnotification(videoNotification);
        }
        catch(Exception e){ assertTrue(false);}

        Utility.toWait(lock, 2000);
        Log.d(TAG, "Video notification sent");
        assertTrue(true);

        Utility.toWait(lock,5000);
    }
}

package org.iotivity.service.notificationservice.impl;

import android.app.IntentService;
import android.content.Intent;
import android.util.Log;

import org.iotivity.service.notificationservice.NotificationService;

public class WifiIntentService extends IntentService {

    public static String WifiIntentServiceAction="org.iotivity.service.notificationservice.WIFI_STATUS";
    public WifiIntentService() {
        super(WifiIntentService.class.getName());
    }

    @Override
    protected void onHandleIntent(Intent intent) {

        Log.d("WifiIntentService", "Wifi connected,discovering again");
        boolean wifiStatus=intent.getBooleanExtra("WIFISTATUS", false);
        if(wifiStatus==false)
            {
                Intent broadcastIntent = new Intent();
                broadcastIntent.setAction(WifiIntentServiceAction);
                broadcastIntent.addCategory(Intent.CATEGORY_DEFAULT);
                sendBroadcast(broadcastIntent);
            }
        final NotificationService notificationService=NotificationService.getInstance();

        new Thread(new Runnable() {
            @Override
            public void run() {
                try {

                    notificationService.stopSubscribeNotifications();
                    Thread.sleep(2000);
                    notificationService.discover();


                } catch (InterruptedException e) {
                    e.printStackTrace();
                }



            }
        }).start();
    }
}

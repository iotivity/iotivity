/**
 * ***************************************************************
 * <p>
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 * <p>
 * <p>
 * <p>
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * <p>
 * http://www.apache.org/licenses/LICENSE-2.0
 * <p>
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * <p>
 * ****************************************************************
 */

package org.iotivity.service.sampleconsumer;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.media.RingtoneManager;
import android.net.Uri;
import android.os.Bundle;

import android.os.Handler;
import android.support.v4.app.NotificationCompat;
import android.util.Log;
import android.view.Gravity;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;
import android.widget.Toast;

import org.iotivity.service.notificationservice.NotificationCallback;
import org.iotivity.service.notificationservice.NotificationService;
import org.iotivity.service.notificationservice.NotificationServiceCallbackHandler;
import org.iotivity.service.notificationservice.NotificationObject;
import org.iotivity.service.notificationservice.NotificationType;
import org.iotivity.service.notificationservice.impl.ImageNotification;
import org.iotivity.service.notificationservice.impl.TextNotification;
import org.iotivity.service.notificationservice.impl.VideoNotification;

import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.HashMap;



public class SampleConsumer extends Activity implements NotificationCallback {


    NotificationService notificationservice = NotificationService.getInstance();
    NotificationServiceCallbackHandler callbackHandler = NotificationServiceCallbackHandler.getInstance();
    int selected;
    TextView infomesssage;
    boolean discovered_from_activity = false;
    WifiReceiver wifiReceiver;


    Handler clearNotification = new Handler();
    Button subscribe, unsubscribe, discover;
    LinearLayout listResource;
    RadioGroup radioGroupResources;
    HashMap<String, Integer> icons = new HashMap<>();
    HashMap<String, Integer> actions = new HashMap<>();
    ArrayList<RadioButton> radioButtons = new ArrayList<>();
    HashMap<String,Integer> resourceList=new HashMap<>();

    long discoverTime,notificationtime;

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_sample_consumer);
        setupiconMap();
        setupActionMap();
        registerReceiver();
        ConsumerApplication consumerApplication = (ConsumerApplication) getApplicationContext();
        consumerApplication.consumer = this;
        notificationservice = NotificationService.getInstance();
        callbackHandler = NotificationServiceCallbackHandler.getInstance();


        callbackHandler.setNotificationCallback(SampleConsumer.this);


        infomesssage = (TextView) findViewById(R.id.infomessage);
        subscribe = (Button) findViewById(R.id.subscribe);
        unsubscribe = (Button) findViewById(R.id.unsubscribe);
        discover = (Button) findViewById(R.id.discover);
        listResource = (LinearLayout) findViewById(R.id.resourcelist);
        unsubscribe.setEnabled(false);
        radioGroupResources = (RadioGroup) findViewById(R.id.radioresources);

        subscribe.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                selected = -1;
                for (RadioButton r : radioButtons)
                    if (r.isChecked()) {

                        selected = resourceList.get(r.getText().toString());//radioButtons.indexOf(r);
                        break;
                    }
                Log.d("Selected=", selected + "");

                subscribe.setEnabled(false);
                unsubscribe.setEnabled(true);
                discover.setEnabled(false);

                Runnable r=new Runnable() {
                    @Override
                    public void run() {
                        notificationservice.subscribesnotifications(selected);
                    }
                };
                new Thread(r).start();


            }
        });

        unsubscribe.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                subscribe.setEnabled(true);
                unsubscribe.setEnabled(false);
                discover.setEnabled(true);
                notificationservice.stopSubscribeNotifications();

            }
        });

        discover.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {




                discoverTime=System.currentTimeMillis();
                discovered_from_activity = true;
                notificationservice.discover();


            }
        });
    }


    @Override
    protected void onDestroy() {
        super.onDestroy();
        unregisterReceiver(wifiReceiver);
    }

    @Override
    public void onNotificationReceived(NotificationObject notificationObject) {
        Log.d("onNotificationReceived", "Inside" + notificationObject.getNotifcationSender() + notificationObject.getNotificationType().getValue() + notificationObject.getNotifcationId());
        Calendar cal = Calendar.getInstance();
        SimpleDateFormat sdf = new SimpleDateFormat("HH:mm:ss:SS");

        Log.d("Notification TIme",sdf.format(cal.getTime())+"...."+notificationObject.getNotifcationTime());
        final NotificationCompat.Builder mBuilder = new NotificationCompat.Builder(SampleConsumer.this);
        String sender = notificationObject.getNotifcationSender();

        final int id = notificationObject.getNotifcationId();
        final int ttl = notificationObject.getNotifcationTTL();
        if (id == 0) return;

        if (sender == null || sender.equals("")) {
            Log.d("SampleComsumer", "sender is null,using default sender");
            sender = "Light";
        }
        int iconId;
        try {
            iconId = icons.get(sender);
        } catch (Exception e) {
            iconId = R.drawable.logo;
        }


        mBuilder.setSmallIcon(iconId);

        mBuilder.setContentTitle(sender);

        ;
        if (notificationObject.getNotificationType() == NotificationType.TYPE_TEXT) {

            TextNotification textNotification = (TextNotification) notificationObject;
            mBuilder.setContentText(textNotification.getNotificationMessage() + ":" + textNotification.getNotifcationTime());
            final android.app.NotificationManager service = (android.app.NotificationManager)
                    getSystemService(Context.NOTIFICATION_SERVICE);
            mBuilder.setAutoCancel(true);
            Uri notificationSound = RingtoneManager.getDefaultUri(RingtoneManager.TYPE_NOTIFICATION);
            mBuilder.setSound(notificationSound);
            service.notify(id, mBuilder.build());

        } else if (notificationObject.getNotificationType() == NotificationType.TYPE_IMAGE) {
            ImageNotification imageNotification = (ImageNotification) notificationObject;
            mBuilder.setContentText(imageNotification.getNotificationMessage() + ":" + imageNotification.getNotifcationTime());
            final String url = imageNotification.getNotificationImageUrl();
            if (sender == null || sender.equals("")) {
                Log.d("SampleComsumer", "sender is null,using default sender");
                sender = "Light";
            }

            Bitmap myBitmap = BitmapFactory.decodeResource(getResources(),R.drawable.coffee_action);
            if (myBitmap == null)
                Log.d("Bitmap is", "NULl");

            mBuilder.setStyle(new NotificationCompat.BigPictureStyle().bigPicture(myBitmap));
            final android.app.NotificationManager service = (android.app.NotificationManager)
                    getSystemService(Context.NOTIFICATION_SERVICE);
            mBuilder.setAutoCancel(true);
            Uri notificationSound = RingtoneManager.getDefaultUri(RingtoneManager.TYPE_NOTIFICATION);
            mBuilder.setSound(notificationSound);
            service.notify(id, mBuilder.build());

            Runnable r = new Runnable() {
                @Override
                public void run() {
                    try {
                        Log.d("ImageUrl=", url);
                        URL url_object = new URL(url);
                        HttpURLConnection connection = (HttpURLConnection) url_object.openConnection();
                        connection.setDoInput(true);
                        connection.connect();
                        InputStream input = connection.getInputStream();
                        Bitmap myBitmap = BitmapFactory.decodeStream(input);
                        if (myBitmap == null)
                            Log.d("Bitmap is", "NULl");

                        mBuilder.setStyle(new NotificationCompat.BigPictureStyle().bigPicture(myBitmap));
                        final android.app.NotificationManager service = (android.app.NotificationManager)
                                getSystemService(Context.NOTIFICATION_SERVICE);
                        mBuilder.setAutoCancel(true);
                        Uri notificationSound = RingtoneManager.getDefaultUri(RingtoneManager.TYPE_NOTIFICATION);
                        mBuilder.setSound(notificationSound);
                        service.notify(id, mBuilder.build());

                    } catch (MalformedURLException e) {
                        e.printStackTrace();
                    } catch (IOException e) {
                        e.printStackTrace();

                        //playvideo("http://www.sample-videos.com/video/mp4/480/big_buck_bunny_480p_1mb.mp4");
                    }
                }
            };
            //new Thread(r).start();


        } else if (notificationObject.getNotificationType() == NotificationType.TYPE_VIDEO) {
            VideoNotification videoNotification = (VideoNotification) notificationObject;
            mBuilder.setContentText("video received" + ":" + videoNotification.getNotifcationTime());
            String url = videoNotification.getNotificationVideoUrl();
            String vid = url.substring(url.indexOf("?v=", 0) + 3);
            final String vid2 = "http://img.youtube.com/vi/" + vid + "/0.jpg";
            Log.d("video thumbnail url", vid2);
            // TODO: 10/9/2015 :show video in aother application
            Runnable r = new Runnable() {
                @Override
                public void run() {
                    try {

                        URL url_object = new URL(vid2);
                        HttpURLConnection connection = (HttpURLConnection) url_object.openConnection();
                        connection.setDoInput(true);
                        connection.connect();
                        InputStream input = connection.getInputStream();
                        Bitmap myBitmap = BitmapFactory.decodeStream(input);
                        if (myBitmap == null)
                            Log.d("Bitmap is", "NULl");

                        mBuilder.setStyle(new NotificationCompat.BigPictureStyle().bigPicture(myBitmap));
                        final android.app.NotificationManager service = (android.app.NotificationManager)
                                getSystemService(Context.NOTIFICATION_SERVICE);
                        mBuilder.setAutoCancel(true);
                        mBuilder.addAction(R.drawable.play_icon, "Play Video", null);
                        Uri notificationSound = RingtoneManager.getDefaultUri(RingtoneManager.TYPE_NOTIFICATION);
                        mBuilder.setSound(notificationSound);
                        service.notify(id, mBuilder.build());

                    } catch (MalformedURLException e) {
                        e.printStackTrace();
                    } catch (IOException e) {
                        e.printStackTrace();


                    }
                }
            };
            new Thread(r).start();


        }


        Runnable r = new Runnable() {
            @Override
            public void run() {
                clearNotification.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        final android.app.NotificationManager service = (android.app.NotificationManager)
                                getSystemService(Context.NOTIFICATION_SERVICE);
                        service.cancel(id);
                    }
                }, ttl * 1000);
            }
        };
        new Thread(r).start();

    }

    @Override
    public void onResourceDiscoveredCallback(final String resourceName, int resourceIndex) {

        resourceList.put(resourceName,resourceIndex);
        Log.d("Discover time=", (System.currentTimeMillis()-discoverTime)+"");
        runOnUiThread(new Runnable() {
            @Override
            public void run() {

                if (radioButtons.size() != 0) {
                    radioGroupResources.removeAllViewsInLayout();
                }

                Toast.makeText(SampleConsumer.this, "Notification Rescource discovered", Toast.LENGTH_SHORT).show();

                if (!discovered_from_activity) {
                    subscribe.setEnabled(true);
                    unsubscribe.setEnabled(false);


                }

                RadioButton radioButtonView = new RadioButton(SampleConsumer.this);
                radioButtonView.setText(resourceName);

                LinearLayout.LayoutParams p = new LinearLayout.LayoutParams(
                        LinearLayout.LayoutParams.FILL_PARENT,
                        LinearLayout.LayoutParams.WRAP_CONTENT

                );
                p.gravity = Gravity.CENTER_HORIZONTAL;
                p.setMarginStart(50);

                radioGroupResources.addView(radioButtonView, p);
                radioButtons.add(radioButtonView);
                discovered_from_activity = false;
            }
        });

    }

    @Override
    public void showMessage(String s) {

    }


    void setupiconMap() {
        String device[] = getResources().getStringArray(R.array.Devices);
        icons.put(device[0], R.drawable.light_icon);
        icons.put(device[1], R.drawable.coffee_machine_icon);
        icons.put(device[2], R.drawable.ac_icon);
        icons.put(device[3], R.drawable.refrigerator_icon);
        icons.put(device[4], R.drawable.oven_icon);
        Log.d("icon hsize", icons.size() + "");

    }

    void setupActionMap() {
        String device[] = getResources().getStringArray(R.array.Devices);
        actions.put(device[0], R.drawable.light_action);
        actions.put(device[1], R.drawable.coffee_action);
        actions.put(device[2], R.drawable.ac_action);
        actions.put(device[3], R.drawable.refrigerator_action);
        actions.put(device[4], R.drawable.oven_action);

    }

    void registerReceiver(){
        IntentFilter filter = new IntentFilter("org.iotivity.service.notificationservice.WIFI_STATUS");
        filter.addCategory(Intent.CATEGORY_DEFAULT);
       wifiReceiver=new WifiReceiver();
        registerReceiver(wifiReceiver, filter);
    }
    public class WifiReceiver extends BroadcastReceiver{

        @Override
        public void onReceive(Context context, Intent intent) {
            Log.d("WifiReceiver","Wifi disconnected");
            infomesssage.setText("Wifi disconnected");
        }
    }
}


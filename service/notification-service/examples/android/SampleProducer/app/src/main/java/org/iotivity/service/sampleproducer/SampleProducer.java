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

package org.iotivity.service.sampleproducer;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import org.iotivity.service.notificationservice.NotificationProducerCallback;
import org.iotivity.service.notificationservice.NotificationService;
import org.iotivity.service.notificationservice.NotificationObject;
import org.iotivity.service.notificationservice.NotificationServiceCallbackHandler;
import org.iotivity.service.notificationservice.NotificationType;
import org.iotivity.service.notificationservice.impl.ImageNotification;
import org.iotivity.service.notificationservice.impl.TextNotification;
import org.iotivity.service.notificationservice.impl.VideoNotification;

import java.util.ArrayList;
import java.util.HashMap;


public class SampleProducer extends Activity {

    RadioGroup radioGroupType;
    RadioButton radioButtonText, radioButtonImage, radioButtonVideo;
    LinearLayout messageLL, urlLL;
    EditText editTextMessage, editTextUrl;
    NotificationType notificationType = NotificationType.TYPE_TEXT;
    TextView textViewSender;
    String sender;
    ListView listView;
    NotificationProducerCallback notificationProducerCallback;
    NotificationAdapter notificationlistAdapter;
    ArrayList<NotificationObject> notifications = new ArrayList<NotificationObject>();
    HashMap<Integer,Integer> notificationRead=new HashMap<>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_sample_producer);
        sender = getIntent().getStringExtra("DEVICENAME");
        Log.d("Device name", sender);
        final NotificationService notificationservice = NotificationService.getInstance();
        notificationProducerCallback =new NotificationProducerCallback() {
            @Override
            public void onNotificationAcknowledgementReceieved(int id, int i1) {
                Log.d("SampleProducer", "onNotificationAcknowledgementReceieved" + id);

                notificationRead.put(id, notificationRead.get(id) + 1);

                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        notificationlistAdapter.notifyDataSetChanged();
                        Toast.makeText(getApplicationContext(), "onNotificationAcknowledgementReceieved", Toast.LENGTH_SHORT).show();
                    }
                });

            }
        };
        final NotificationServiceCallbackHandler callbackHandler=NotificationServiceCallbackHandler.getInstance();
        callbackHandler.setNotificationCallback(notificationProducerCallback);
        notificationservice.startnotificationproducer(sender);
        messageLL = (LinearLayout) findViewById(R.id.messageLL);
        urlLL = (LinearLayout) findViewById(R.id.URLLL);

        radioGroupType = (RadioGroup) findViewById(R.id.radiogrouptype);

        radioButtonImage = (RadioButton) findViewById(R.id.radioimage);
        radioButtonText = (RadioButton) findViewById(R.id.radiotext);
        radioButtonVideo = (RadioButton) findViewById(R.id.radiovideo);

        editTextMessage = (EditText) findViewById(R.id.message);
        textViewSender = (TextView) findViewById(R.id.sender);
        textViewSender.setText(sender);


        editTextUrl = (EditText) findViewById(R.id.url);
        radioGroupType.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(RadioGroup radioGroup, int i) {
                if (radioButtonText.isChecked()) {
                    messageLL.setVisibility(View.VISIBLE);

                    urlLL.setVisibility(View.INVISIBLE);
                    notificationType = NotificationType.TYPE_TEXT;
                } else if (radioButtonImage.isChecked()) {
                    messageLL.setVisibility(View.VISIBLE);
                    editTextUrl.setText("http://theselby.com/media/2_28_11_SupremeCoffee7598.jpg");
                    urlLL.setVisibility(View.VISIBLE);
                    notificationType = NotificationType.TYPE_IMAGE;
                } else if (radioButtonVideo.isChecked()) {
                    messageLL.setVisibility(View.INVISIBLE);
                    urlLL.setVisibility(View.VISIBLE);
                    editTextUrl.setText("https://www.youtube.com/watch?v=CppgLnNM1PE");
                    notificationType = NotificationType.TYPE_VIDEO;
                }
            }
        });
        Button b = (Button) findViewById(R.id.button);
        b.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (sender == null || sender.equals("")) {
                    Toast.makeText(SampleProducer.this, "Sender is empty", Toast.LENGTH_SHORT).show();
                    return;
                }

                String message = editTextMessage.getText().toString();
                String url = editTextUrl.getText().toString();
                Log.d("sending:", sender + message);
                NotificationObject notificationObject = null;
                if (notificationType == NotificationType.TYPE_TEXT) {
                    notificationObject = new TextNotification();
                    notificationObject.setNotifcationSender(sender);
                    ((TextNotification) notificationObject).setNotificationMessage(message);




                } else if (notificationType == NotificationType.TYPE_IMAGE) {

                    notificationObject = new ImageNotification();
                    notificationObject.setNotifcationSender(sender);
                    ((ImageNotification) notificationObject).setNotificationImageUrl(url);
                    ((ImageNotification) notificationObject).setNotificationImageUrl("http://weknowyourdreams.com/images/coffee/coffee-01.jpg");
                    ((ImageNotification) notificationObject).setNotificationMessage(message);


                } else if (notificationType == NotificationType.TYPE_VIDEO) {

                    notificationObject = new VideoNotification();
                    notificationObject.setNotifcationSender(sender);
                    String url2 = "https://www.youtube.com/watch?v=CppgLnNM1PE";
                    ((VideoNotification) notificationObject).setNotificationVideoUrl(url2);



                }
                int id=notificationservice.sendnotification(notificationObject);
                notificationObject.setNotifcationId(id);
                //update List and UI
                notifications.add(notificationObject);
                notificationRead.put(id, 0);
                notificationlistAdapter.notifyDataSetChanged();


                editTextMessage.setText("");

            }
        });

        listView = (ListView) findViewById(R.id.notification_list);
       // notificationlistAdapter = new ArrayAdapter<String>(getApplicationContext(), android.R.layout.simple_dropdown_item_1line, notifications);
        notificationlistAdapter=new NotificationAdapter(this,notifications,notificationRead);
        listView.setAdapter(notificationlistAdapter);

        sendWelcomeNotification();

    }

    void sendWelcomeNotification() {
        TextNotification textNotification = new TextNotification();
        textNotification.setNotificationMessage("Welcome");
        textNotification.setNotifcationSender(sender);
        NotificationService notificationService = NotificationService.getInstance();
        int id=notificationService.sendnotification(textNotification);
        textNotification.setNotifcationId(id);
        //update List and UI
        notifications.add(textNotification);
        notificationRead.put(id,0);
        notificationlistAdapter.notifyDataSetChanged();
    }



    public void onItemClick(int mPosition){

        Log.d("Clicked",notifications.get(mPosition).getNotificationType()+"");
    }
}

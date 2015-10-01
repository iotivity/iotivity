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
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.Spinner;
import android.widget.Toast;

import org.iotivity.service.notificationservice.NotificationService;
import org.iotivity.service.notificationservice.NotificationObject;
import org.iotivity.service.notificationservice.NotificationType;
import org.iotivity.service.notificationservice.impl.ImageNotification;
import org.iotivity.service.notificationservice.impl.TextNotification;
import org.iotivity.service.notificationservice.impl.VideoNotification;


public class SampleProducer extends Activity {

    RadioGroup radioGroupType;
    RadioButton radioButtonText, radioButtonImage, radioButtonVideo;
    LinearLayout messageLL, urlLL;
    EditText editTextMessage, editTextUrl;
    NotificationType notificationType = NotificationType.TYPE_TEXT;
    Spinner spinnerSender;
    String sender;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_sample_producer);
        sender = getIntent().getStringExtra("DEVICENAME");
        Log.d("Device name", sender);
        final NotificationService notificationservice = NotificationService.getInstance();
        notificationservice.startnotificationproducer(sender);
        messageLL = (LinearLayout) findViewById(R.id.messageLL);
        urlLL = (LinearLayout) findViewById(R.id.URLLL);

        radioGroupType = (RadioGroup) findViewById(R.id.radiogrouptype);

        radioButtonImage = (RadioButton) findViewById(R.id.radioimage);
        radioButtonText = (RadioButton) findViewById(R.id.radiotext);
        radioButtonVideo = (RadioButton) findViewById(R.id.radiovideo);

        editTextMessage = (EditText) findViewById(R.id.message);
        spinnerSender = (Spinner) findViewById(R.id.sender);
        String[] devices = {sender};
        ArrayAdapter<String> senderAdapter = new ArrayAdapter<String>(SampleProducer.this, android.R.layout.simple_spinner_item, devices);
        senderAdapter.setDropDownViewResource(android.R.layout.simple_dropdown_item_1line);
        spinnerSender.setAdapter(senderAdapter);
        spinnerSender.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {

                Log.d("sender", sender);
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {

            }
        });

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
                    notificationObject.setNotificationType(notificationType);
                    ((TextNotification) notificationObject).setNotificationMessage(message);

                } else if (notificationType == NotificationType.TYPE_IMAGE) {

                    notificationObject = new ImageNotification();
                    notificationObject.setNotifcationSender(sender);
                    notificationObject.setNotificationType(notificationType);
                    ((ImageNotification) notificationObject).setNotificationImageUrl(url);
                    ((ImageNotification) notificationObject).setNotificationImageUrl("http://weknowyourdreams.com/images/coffee/coffee-01.jpg");
                    ((ImageNotification) notificationObject).setNotificationMessage(message);
                } else if (notificationType == NotificationType.TYPE_VIDEO) {

                    notificationObject = new VideoNotification();
                    notificationObject.setNotificationType(notificationType);
                    notificationObject.setNotifcationSender(sender);
                    String url2 = "https://www.youtube.com/watch?v=CppgLnNM1PE";
                    ((VideoNotification) notificationObject).setNotificationVideoUrl(url2);

                }
                notificationservice.sendnotification(notificationObject);
                editTextMessage.setText("");

            }
        });
        sendWelcomeNotification();
    }

    void sendWelcomeNotification()
    {
        TextNotification textNotification=new TextNotification();
        textNotification.setNotificationMessage("Welcome");
        textNotification.setNotifcationSender(sender);
        textNotification.setNotificationType(NotificationType.TYPE_TEXT);
        NotificationService notificationService=NotificationService.getInstance();
        notificationService.sendnotification(textNotification);
    }

}

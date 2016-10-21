/*
//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

package com.sec.notiproviderexample;

import android.app.Notification;
import android.app.NotificationManager;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.RadioButton;
import android.widget.TextView;
import android.widget.Toast;
import android.app.Activity;

import org.iotivity.service.ns.common.MediaContents;

import java.text.DateFormat;
import java.util.Date;

public class MainActivity extends Activity {

    private final String TAG = "NS_MAIN_ACTIVITY";
    private static final int CONSUMER_SUBSCRIBED = 1;
    private static final int MESSAGE_SYNC = 2;
    private static final int MESSAGE_NOTIFICATION = 3;

    private Button btnTitle;
    private Button btnBody;
    private Button btnTopic;
    private Button btnSend;
    private Button btnStart;
    private Button btnRegister;
    private Button btnSet;
    private Button btnStop;
    private Button btnLog;
    private EditText editTextTitle;
    private EditText editTextBody;
    private EditText editTextTopic;
    private RadioButton radioProvider;
    private RadioButton radioConsumer;
    private static TextView TvLog;

    private static int notiId = 100;
    private static int subCnt = 0;
    private boolean isStarted = false;
    private String consumerId;
    private boolean gAcceptor = true;

    private NotiListener mNotiListener = null;
    private ProviderSample mProviderSample = null;

    public static Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case CONSUMER_SUBSCRIBED:
                    String ConsumerId = (String) msg.obj;
                    if(ConsumerId != null)
                        TvLog.append("Consumer Subscibed: " + ConsumerId + "\n");
                    break;

                case MESSAGE_SYNC:
                    String sync = (String) msg.obj;
                    if(sync != null)
                        TvLog.append("SyncInfo Received :" + sync + "\n");
                    break;

                default:
                    break;
            }
        }
    };

    public void showToast(final String toast)
    {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(getApplicationContext(), toast, Toast.LENGTH_SHORT).show();
            }
        });
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        btnTitle = (Button) findViewById(R.id.BtnTitle);
        btnBody = (Button) findViewById(R.id.BtnBody);
        btnTopic = (Button) findViewById(R.id.BtnTopic);
        btnSend = (Button) findViewById(R.id.BtnCreateNoti);

        btnStart = (Button) findViewById(R.id.BtnStart);
        btnRegister = (Button) findViewById(R.id.BtnRegister);
        btnSet = (Button) findViewById(R.id.BtnSet);
        btnLog = (Button) findViewById(R.id.BtnLog);
        btnStop = (Button) findViewById(R.id.BtnStop);

        editTextTitle = (EditText) findViewById(R.id.EditTextTitle);
        editTextBody = (EditText) findViewById(R.id.EditTextBody);
        editTextTopic = (EditText) findViewById(R.id.EditTextTopic);

        radioProvider = (RadioButton) findViewById(R.id.RadioProvider);
        radioConsumer = (RadioButton) findViewById(R.id.RadioConsumer);

        TvLog = (TextView) findViewById(R.id.TvLog);

        btnTitle.setEnabled(false);
        btnBody.setEnabled(false);
        btnTopic.setEnabled(false);

        btnSend.setOnClickListener(mClickListener);
        btnStart.setOnClickListener(mClickListener);
        btnRegister.setOnClickListener(mClickListener);
        btnSet.setOnClickListener(mClickListener);
        btnLog.setOnClickListener(mClickListener);
        btnStop.setOnClickListener(mClickListener);
        radioProvider.setOnClickListener(mClickListener);
        radioConsumer.setOnClickListener(mClickListener);

        mProviderSample = new ProviderSample(getApplicationContext());
        mProviderSample.setHandler(mHandler);

        mNotiListener = new NotiListener(this);

        Intent intent = new Intent("android.settings.ACTION_NOTIFICATION_LISTENER_SETTINGS");
        startActivity(intent);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    public ProviderSample getProviderSample()
    {
        return mProviderSample;
    }

    Button.OnClickListener mClickListener = new View.OnClickListener() {
        public void onClick(View v) {
            switch (v.getId()) {
                case R.id.RadioProvider: {
                    if (isStarted == false) {
                        gAcceptor = true;
                        showToast("Provider as acceptor is " + gAcceptor);
                    }
                    else
                        showToast("Start ProviderService again to change acceptor as provider");
                }
                break;

                case R.id.RadioConsumer: {
                    if (isStarted == false) {
                        gAcceptor = false;
                        showToast("Provider as acceptor is "+ gAcceptor);
                    }
                    else
                        showToast("Start ProviderService again to change acceptor as consumer");
                }
                break;

                case R.id.BtnStart: {
                    if (isStarted == false) {
                        Log.i(TAG, "Start  Provider Service");
                        TvLog.setText("Start Provider Service\n");
                        mProviderSample.Start(gAcceptor);
                        isStarted = true;
                        radioProvider.setEnabled(false);
                        radioConsumer.setEnabled(false);
                    } else {
                        Log.e(TAG, " Provider Service had already started");
                        showToast(" Provider Service had already started");
                    }
                }
                break;

                case R.id.BtnRegister: {
                    if (isStarted == false) {
                        Log.i(TAG, "Start  Provider Service");
                        TvLog.append("Register Topic : OCF_TOPIC1\n");
                        TvLog.append("Register Topic : OCF_TOPIC2\n");
                        TvLog.append("Register Topic : OCF_TOPIC3\n");
                        TvLog.append("Register Topic : OCF_TOPIC4\n");
                        showToast("Start Provider Service First");
                        break;
                    }
                    mProviderSample.RegisterTopic();

                }
                break;

                case R.id.BtnSet: {
                    if (isStarted == false) {
                        Log.i(TAG, "Start Provider Service");
                        TvLog.append("Set Topic : OCF_TOPIC1\n");
                        TvLog.append("Set Topic : OCF_TOPIC2\n");
                        TvLog.append("Set Topic : OCF_TOPIC3\n");
                        TvLog.append("Set Topic : OCF_TOPIC4\n");
                        showToast("Start Provider Service First");
                        break;
                    }
                    if(gAcceptor == false){
                        showToast("Operation Not Permitted: \nStart Provider Service with provider as acceptor");
                        break;
                    }
                    mProviderSample.SetTopic();

                }
                break;

                case R.id.BtnCreateNoti: {

                    String id = Integer.toString(notiId); // generate notificaion ID
                    String title = editTextTitle.getText().toString();
                    String body = editTextBody.getText().toString();
                    String topic  = editTextTopic.getText().toString();

                    if(isStarted == false)
                    {
                        Log.e(TAG, "Fail to send NSMessage");
                        showToast("Start ProviderService First");
                        break;
                    }

                    // Build android noti object and send it to Notification service receiver
                    Notification.Builder notiBuilder = new Notification.Builder(getApplicationContext());
                    notiBuilder.setContentTitle(title);
                    notiBuilder.setContentText(body);
                    notiBuilder.setPriority(Notification.PRIORITY_MAX);
                    notiBuilder.setDefaults(Notification.DEFAULT_ALL);
                    notiBuilder.setSmallIcon(R.mipmap.ic_launcher);
                    NotificationManager notiMgr = (NotificationManager) getSystemService(NOTIFICATION_SERVICE);
                    notiMgr.notify(notiId, notiBuilder.build());

                    Log.i(TAG, "#" + notiId + " notified ..");
                    TvLog.append("Send Notitication(Msg ID: " + notiId + ")\n");
                    notiId++;
                }
                break;

                case R.id.BtnStop: {
                    if(isStarted == false) {
                        Log.e(TAG, "Fail to stop service");
                        showToast("Already Stopped");
                        break;
                    }

                    mProviderSample.Stop();
                    isStarted = false;
                    radioProvider.setEnabled(true);
                    radioConsumer.setEnabled(true);
                    showToast("Stopped ProviderService"+ isStarted);
                    TvLog.append("Stop Provider Service\n");
                }
                break;

                case R.id.BtnLog: {

                    TvLog.setText("");
                }
                break;  }
            }
    };
}


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
import android.content.SyncInfo;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import org.iotivity.service.ns.common.MediaContents;
public class MainActivity extends AppCompatActivity {

    private final String TAG = "NS_MAIN_ACTIVITY";
    private static final int MESSAGE_SUBSCRIPTION = 1;
    private static final int MESSAGE_SYNC = 2;
    private static final int MESSAGE_NOTIFICATION = 3;

    private Button btnTitle;
    private Button btnBody;
    private Button btnSend;
    private Button btnStart;
    private Button btnStop;
    private Button btnAccept;
    private Button btnSync;
    private EditText editTextTitle;
    private EditText editTextBody;
    private static TextView TvLog;

    private static int notiId = 100;
    private static int subCnt = 0;
    private boolean isStarted = false;
    private String consumerId;

    private NotiListener mNotiListener = null;
    private ProviderProxy mProviderProxy = null;

    public static Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MESSAGE_SUBSCRIPTION:
                    String subscriber = (String) msg.obj;
                    if(subscriber != null)
                        TvLog.append("Subscriber IP(" + ++subCnt + "): " + subscriber + "\n");
                    break;

                case MESSAGE_SYNC:
                    String sync = (String) msg.obj;
                    if(sync != null)
                        TvLog.append("Sync-Read(Msg ID: " + sync + ")\n");
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
        btnSend = (Button) findViewById(R.id.BtnCreateNoti);

        btnStart = (Button) findViewById(R.id.BtnStart);
        btnAccept = (Button) findViewById(R.id.BtnAccept);
        btnSync = (Button) findViewById(R.id.BtnSync);
        btnStop = (Button) findViewById(R.id.BtnStop);

        editTextTitle = (EditText) findViewById(R.id.EditTextTitle);
        editTextBody = (EditText) findViewById(R.id.EditTextBody);

        TvLog = (TextView) findViewById(R.id.TvLog);

        btnTitle.setEnabled(false);
        btnBody.setEnabled(false);

        btnSend.setOnClickListener(mClickListener);

        btnStart.setOnClickListener(mClickListener);

        btnAccept.setOnClickListener(mClickListener);
        btnAccept.setVisibility(View.INVISIBLE);

        btnSync.setOnClickListener(mClickListener);
        btnSync.setVisibility(View.INVISIBLE);

        btnStop.setOnClickListener(mClickListener);

        mProviderProxy = new ProviderProxy(getApplicationContext());
        mProviderProxy.setHandler(mHandler);

        mNotiListener = new NotiListener(this);

        Intent intent = new Intent("android.settings.ACTION_NOTIFICATION_LISTENER_SETTINGS");
        startActivity(intent);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    public ProviderProxy getProviderProxy()
    {
        return mProviderProxy;
    }

    Button.OnClickListener mClickListener = new View.OnClickListener() {
        public void onClick(View v) {
            switch (v.getId()) {

                case R.id.BtnStart: {
                    if (isStarted == false) {
                        Log.i(TAG, "Start NS Provider Service");

                        TvLog.setText("Start NS-Provider\n");

                        boolean policy = true; // provider controls the acceptance of consumers
                        mProviderProxy.Start(policy);
                        isStarted = true;
                    } else {
                        Log.e(TAG, "NS Provider Service had already started");
                    }
                }
                break;

                case R.id.BtnAccept: {
                    if(isStarted == false)
                    {
                        Log.e(TAG, "Fail to request Accept");
                        break;
                    }
                }
                break;

                case R.id.BtnCreateNoti: {

                    String id = Integer.toString(notiId); // generate notificaion ID
                    String title = editTextTitle.getText().toString();
                    String body = editTextBody.getText().toString();

                    if(isStarted == false)
                    {
                        Log.e(TAG, "Fail to send NSMessage");
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
                    org.iotivity.service.ns.common.Message notiMessage = new org.iotivity.service.ns.common.Message(title,body,"dss");
                    notiMessage.setTTL(10);
                    notiMessage.setTime("12:10");
                    MediaContents media = new MediaContents("daasd");
                    notiMessage.setMediaContents(media);
                    mProviderProxy.SendMessage(notiMessage);
                }
                break;

                case R.id.BtnSync: {
                    if(isStarted == false)
                    {
                        Log.e(TAG, "Fail to send sync");
                        break;
                    }
                    org.iotivity.service.ns.common.SyncInfo.SyncType syncType =  org.iotivity.service.ns.common.SyncInfo.SyncType.READ;
                    mProviderProxy.SendSyncInfo(1,syncType);
                }
                break;

                case R.id.BtnStop: {
                    if(isStarted == false)
                    {
                        Log.e(TAG, "Fail to stop service");
                        break;
                    }

                    mProviderProxy.Stop();
                    isStarted = false;

                    TvLog.append("Stop NS-Provider\n");
                }
                break;
            }
        }
    };
}


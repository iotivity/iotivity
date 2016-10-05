/******************************************************************
 * Copyright 2016 Samsung Electronics All Rights Reserved.
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
 ******************************************************************/

package com.sec.noticonsumerexample;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.app.Activity;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import org.iotivity.service.ns.common.TopicsList;
import org.iotivity.service.ns.common.Topic;

public class MainActivity extends Activity
{
    private final String TAG = "NS_MAIN_ACTIVITY";

    private Button btnStart;
    private Button btnStop;
    private Button btnRescan;
    private Button btnEnableRemoteService;
    private Button btnGetTopicList;
    private Button btnUpdateTopicList;
    private Button btnClearLog;
    private static TextView TvLog;

    private boolean isStarted = false;

    private ConsumerSample mConsumerSample = null;

    private static final int PROVIDER_DISCOVERED = 1;
    private static final int STATE_CHANGED = 2;
    private static final int MESSAGE_RECEIVED = 3;
    private static final int SYNCINFO_RECEIVED = 4;
    private static final int TOPICS_RECEIVED = 5;

    public static Handler mHandler = new Handler()
    {
        @Override
        public void handleMessage(Message msg)
        {
            switch (msg.what)
            {
                case PROVIDER_DISCOVERED:
                    {
                        String providerId = (String) msg.obj;
                        if (providerId != null)
                        {
                            TvLog.append( providerId + "\n");
                        }
                        break;
                    }
                case STATE_CHANGED:
                    {
                        String state = (String) msg.obj;
                        if (state != null)
                        {
                            TvLog.append( state + "\n");
                        }
                        break;
                    }
                case MESSAGE_RECEIVED:
                    {
                        String message = (String) msg.obj;
                        if (message != null)
                        {
                            TvLog.append( message + "\n");
                        }
                        break;
                    }
                case SYNCINFO_RECEIVED:
                    {
                        String sync = (String) msg.obj;
                        if (sync != null)
                        {
                            TvLog.append( sync + "\n");
                        }
                        break;
                    }
                case TOPICS_RECEIVED:
                    {
                        String topicList = (String) msg.obj;
                        if (topicList != null)
                        {
                            TvLog.append( topicList + "\n");
                        }
                        break;
                    }
                default:
                    break;
            }
        }
    };

    public void showToast(final String toast)
    {
        runOnUiThread(new Runnable()
        {
            @Override
            public void run()
            {
                Toast.makeText(getApplicationContext(), toast, Toast.LENGTH_SHORT).show();
            }
        });
    }

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        btnStart = (Button) findViewById(R.id.BtnStart);
        btnStop = (Button) findViewById(R.id.BtnStop);
        btnRescan = (Button) findViewById(R.id.BtnRescan);
        btnEnableRemoteService = (Button) findViewById(R.id.BtnEnableRemoteService);
        btnGetTopicList = (Button) findViewById(R.id.BtnGetTopicList);
        btnUpdateTopicList = (Button) findViewById(R.id.BtnUpdateTopicList);
        btnClearLog = (Button) findViewById(R.id.BtnClearLog);

        TvLog = (TextView) findViewById(R.id.TvLog);

        btnStart.setOnClickListener(mClickListener);
        btnStop.setOnClickListener(mClickListener);
        btnRescan.setOnClickListener(mClickListener);
        btnEnableRemoteService.setOnClickListener(mClickListener);
        btnGetTopicList.setOnClickListener(mClickListener);
        btnUpdateTopicList.setOnClickListener(mClickListener);
        btnClearLog.setOnClickListener(mClickListener);

        mConsumerSample = new ConsumerSample(getApplicationContext());
        mConsumerSample.setHandler(mHandler);
    }

    @Override
    protected void onDestroy()
    {
        if (isStarted)
            mConsumerSample.stopNotificationConsumer();
        super.onDestroy();
    }

    Button.OnClickListener mClickListener = new View.OnClickListener()
    {
        public void onClick(View v)
        {
            switch (v.getId())
            {
                case R.id.BtnStart:
                    {
                        if (!isStarted)
                        {
                            Log.i(TAG, "Start NS Consumer Service");

                            TvLog.setText("Start NS-Consumer\n");
                            mConsumerSample.startNotificationConsumer();
                            isStarted = true;
                        }
                        else
                        {
                            Log.e(TAG, "NS Consumer Service has already started");
                            showToast("Error : Service has already started");
                        }
                    }
                    break;

                case R.id.BtnStop:
                    {
                        if (!isStarted)
                        {
                            Log.e(TAG, "Fail to stop service. Service has not been started");
                            showToast("Error : Service has not been started");
                            break;
                        }
                        TvLog.append("Stop NS-Consumer\n");
                        mConsumerSample.stopNotificationConsumer();
                        isStarted = false;
                    }
                    break;
                case R.id.BtnRescan:
                    {
                        if (!isStarted)
                        {
                            Log.e(TAG, "Fail to rescan. Service has not been started");
                            showToast("Error : Service has not been started");
                            break;
                        }
                        TvLog.append("Rescan NS-Consumer\n");
                        mConsumerSample.rescanProvider();
                    }
                    break;
                case R.id.BtnEnableRemoteService:
                    {
                        if (!isStarted)
                        {
                            Log.e(TAG, "Fail to Enable RemoteService. Service has not been started");
                            showToast("Error : Service has not been started");
                            break;
                        }
                        TvLog.append("EnableRemoteService NS-Consumer\n");

                        //TODO: Update to read the serverAddress from UI
                        String serverAddress = new String();
                        mConsumerSample.enableRemoteService(serverAddress);
                    }
                    break;
                case R.id.BtnGetTopicList:
                    {
                        if (!isStarted)
                        {
                            Log.e(TAG, "Fail to GetTopicList. Service has not been started");
                            showToast("Error : Service has not been started");
                            break;
                        }
                        TvLog.append("GetTopicList NS-Consumer\n");
                        mConsumerSample.getTopicsList();
                    }
                    break;
                case R.id.BtnUpdateTopicList:
                    {
                        if (!isStarted)
                        {
                            Log.e(TAG, "Fail to UpdateTopicList. Service has not been started");
                            showToast("Error : Service has not been started");
                            break;
                        }
                        if(mConsumerSample.getAcceptor())
                        {
                            Log.e(TAG, "Operation Not Allowed. ProviderService Acceptor is not Consumer");
                            showToast("Operation Not Allowed. ProviderService Acceptor is not Consumer");
                            break;
                        }
                        TvLog.append("UpdateTopicList NS-Consumer\n");

                        TopicsList topicList = new TopicsList();
                        topicList.addTopic("OCF_TOPIC1", Topic.TopicState.SUBSCRIBED);
                        topicList.addTopic("OCF_TOPIC2", Topic.TopicState.SUBSCRIBED);
                        topicList.addTopic("OCF_TOPIC3", Topic.TopicState.UNSUBSCRIBED);

                        mConsumerSample.updateTopicList(topicList);
                    }
                    break;
                case R.id.BtnClearLog:
                {
                    TvLog.setText("");
                }
                break;
            }
        }
    };
}

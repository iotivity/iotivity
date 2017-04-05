/******************************************************************
 * Copyright 2017 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************/

package org.iotivity.tcpandroidclient;

import java.util.ArrayList;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;

import org.iotivity.configuration.IConfiguration;
import org.iotivity.tcpandroidclient.MyCustomAdapter;

public class MainActivity extends Activity implements IConfiguration {
    private ListView          mList;
    private ArrayList<String> mArrayList;
    private MyCustomAdapter   mAdapter;
    private static TCPClient  mTcpClient = null;
    private connectTask       mConctTask  = null;
    private final String            TAG        = "TCP Client";

    private static void delay(int second) {
        try {
            Thread.sleep(second * 1000);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    public BroadcastReceiver tcpClientBroadcastReciever = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {

            Log.d(TAG, "BroadcastReceiver Invoked");
            Log.d(TAG, "Intent Flag : " + intent.getFlags());

            if (mTcpClient != null) {
                if (17 == intent.getFlags()) {
                    mTcpClient.sendMessage(KILL_ALL_SERVER);
                    delay(10);
                    mTcpClient.sendMessage(START_JUSTWORK_SERVER_01);
                    delay(10);
                    mTcpClient.sendMessage(START_JUSTWORK_SERVER_02);
                    delay(10);
                    mTcpClient.sendMessage(START_PRECONFIGPIN_SERVER);
                    delay(10);
                } else {

                    mTcpClient.sendMessage(KILL_ALL_SERVER);

                }
            } else {
                Log.e(TAG, "TCP Client is not initialized");
            }
        }
    };

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Window window = getWindow();
        window.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        setContentView(R.layout.activity_main);
        registerReceiver(tcpClientBroadcastReciever,
                new IntentFilter(INTENT_START_SERVER_JUSTWORK));
        registerReceiver(tcpClientBroadcastReciever,
                new IntentFilter(INTENT_KILL_SERVER));
        mArrayList = new ArrayList<String>();

        final EditText editText = (EditText) findViewById(R.id.editText);
        Button send = (Button) findViewById(R.id.send_button);

        // relate the listView from java to the one created in xml
        mList = (ListView) findViewById(R.id.list);
        mAdapter = new MyCustomAdapter(this, mArrayList);
        mList.setAdapter(mAdapter);

        mTcpClient = null;
        mConctTask = new connectTask();
        mConctTask.executeOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR);

        send.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String message = editText.getText().toString();
                mArrayList.add(message);
                if (mTcpClient != null) {
                    mTcpClient.sendMessage(message);
                }
                // refresh the list
                mAdapter.notifyDataSetChanged();
                editText.setText("");
            }
        });
    }

    public class connectTask extends AsyncTask<String, String, TCPClient> {
        @Override
        protected TCPClient doInBackground(String... message) {
            mTcpClient = new TCPClient(new TCPClient.OnMessageReceived() {
                @Override
                public void messageReceived(String message) {
                    try {
                        publishProgress(message);
                        if (message != null) {
                            System.out.println("Recieved Message " + message);
                        }
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
            });
            mTcpClient.run();
            if (mTcpClient != null) {
                mTcpClient.sendMessage("TCP Server and Client connected");
            }
            return null;
        }

        @Override
        protected void onProgressUpdate(String... values) {
            super.onProgressUpdate(values);
            mArrayList.add(values[0]);
            mAdapter.notifyDataSetChanged();
        }
    }

    @Override
    protected void onDestroy() {
        try {
            System.out.println("onDestroy.");
            mTcpClient.sendMessage("Stopping Client");
            mTcpClient.stopClient();
            mConctTask.cancel(true);
            mConctTask = null;
        } catch (Exception e) {
            e.printStackTrace();
        }
        super.onDestroy();
    }
}

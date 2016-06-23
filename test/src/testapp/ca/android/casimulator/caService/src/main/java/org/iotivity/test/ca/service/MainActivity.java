/******************************************************************
*
* Copyright 2016 Samsung Electronics All Rights Reserved.
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
*
******************************************************************/

package org.iotivity.test.ca.service;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;
import java.io.File;

import org.iotivity.ca.simulator.R;

public class MainActivity extends Activity {

    private static boolean isHandlerAlive = false;
    static RMInterface RM = new RMInterface();

    private final static String TAG = "CA_ANDROID_SIMULATOR : MainActivity";

    private final CharSequence[] mNetworkCheckBoxItems = { Network.IP.name(),
            Network.LE.name(), Network.EDR.name()};

    private enum Mode {
        SERVER, CLIENT, BOTH, UNKNOWN
    };

    private enum Network {
        IP, LE, EDR
    };

    private boolean mCheckedItems[] = {
            false, false, false, false
    };

    private int mSelectedItems[] = { 0, 0, 0 };

    private int mUnSelectedItems[] = { 0, 0, 0 };

    private Mode mCurrentMode = Mode.UNKNOWN;

    private RelativeLayout mReceiveLayout = null;

    private TextView mMode_tv = null;

    private TextView mNetwork_tv = null;

    private Button mGetNetworkInfo_btn = null;

    private Handler mLogHandler = null;

    private int CA_IP = (1 << 0);
    private int CA_LE = (1 << 1);
    private int CA_EDR = (1 << 2);
    private int selectedNetworkType = -1;
    private int selectedResponseValue = 0;
    int selectedNetwork = -1;
    int interestedNetwork = 0;
    int uninterestedNetwork = 0;

    private Thread responseThread = null;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_main);

        //RM.setResponseListener(this);

        mLogHandler = new Handler();
        TextView logView = (TextView) findViewById(R.id.tv_result);
        DLog.setTextView(mLogHandler, logView);

        mReceiveLayout = (RelativeLayout) findViewById(R.id.layout_receive);

        mMode_tv = (TextView) findViewById(R.id.tv_mode);
        mNetwork_tv = (TextView) findViewById(R.id.tv_network);

        mGetNetworkInfo_btn = (Button) findViewById(R.id.btn_get_network_info);
        mGetNetworkInfo_btn.setOnClickListener(mGetNetworkInfoHandler);

        showSelectModeView();

        RM.RMInitialize(getApplicationContext(), this);

        RM.RMRegisterHandler();
    }

    private void showSelectModeView()
    {

        mReceiveLayout.setVisibility(View.INVISIBLE);

        mMode_tv.setText("Select Mode (Server or Client)");
        Log.i(TAG, "Select Mode (Server or Client)");
    }

    private void showNetworkView()
    {

        mNetwork_tv.setText("Select Network Type");
        Log.i(TAG, "Select Network Type");
    }

    private void showModeView()
    {

        if (mCurrentMode == Mode.SERVER)
        {

            mReceiveLayout.setVisibility(View.VISIBLE);
            mNetwork_tv.setText("");

        }
        else if (mCurrentMode == Mode.CLIENT)
        {

            mReceiveLayout.setVisibility(View.VISIBLE);
            mNetwork_tv.setText("");
        }
    }

    @Override
    protected void onDestroy()
    {
        super.onDestroy();

        isHandlerAlive = false;

        RM.RMTerminate();
        android.os.Process.killProcess(android.os.Process.myPid());
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu)
    {
        menu.add(0, 1, Menu.NONE, "Start Server");
        menu.add(0, 2, Menu.NONE, "Start Client");
        menu.add(0, 3, Menu.NONE, "Select Network");

        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item)
    {
        switch (item.getItemId())
        {

        case 1:

            RM.RMStartListeningServer();

            if (interestedNetwork == 0) {
                mCurrentMode = Mode.SERVER;
                mMode_tv.setText("MODE: " + mCurrentMode.toString());
                Log.i(TAG, "MODE: " + mCurrentMode.toString());
                showNetworkView();

            }
            else
            {
                mCurrentMode = Mode.SERVER;
                mMode_tv.setText("MODE: " + mCurrentMode.toString());
                Log.i(TAG, "MODE: " + mCurrentMode.toString());
                showModeView();
            }

            if (responseThread == null)
            {
                handleResponse();
            }

            break;

        case 2:

            RM.RMStartDiscoveryServer();

            if (interestedNetwork == 0)
            {
                mCurrentMode = Mode.CLIENT;
                mMode_tv.setText("MODE: " + mCurrentMode.toString());
                Log.i(TAG, "MODE: " + mCurrentMode.toString());
                showNetworkView();

            }
            else
            {
                mCurrentMode = Mode.CLIENT;
                mMode_tv.setText("MODE: " + mCurrentMode.toString());
                Log.i(TAG, "MODE: " + mCurrentMode.toString());
                showModeView();
            }

            if (responseThread == null)
            {
                handleResponse();
            }

            break;

        case 3:

            checkInterestedNetwork("Select Network");

            break;
        }

        return super.onOptionsItemSelected(item);
    }

    private OnClickListener mGetNetworkInfoHandler = new OnClickListener() {
        @Override
        public void onClick(View v) {

            RM.RMGetNetworkInfomation();
        }
    };


    private void checkInterestedNetwork(String title)
    {
        AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);
        builder.setTitle(title)
                .setMultiChoiceItems(mNetworkCheckBoxItems, mCheckedItems,
                        new DialogInterface.OnMultiChoiceClickListener()
                        {
                            @Override
                            public void onClick(DialogInterface dialog,
                                                int which, boolean isChecked)
                            {
                                if (isChecked)
                                {

                                    mSelectedItems[which] = 1;
                                    mUnSelectedItems[which] = 0;

                                }
                                else if (mSelectedItems[which] == 1)
                                {
                                    mSelectedItems[which] = 0;
                                    mUnSelectedItems[which] = 1;
                                }
                            }
                        })
                .setPositiveButton("OK", new DialogInterface.OnClickListener() {

                    @Override
                    public void onClick(DialogInterface dialog, int which) {

                        interestedNetwork = 0;

                        for (int i = 0; i < mSelectedItems.length; i++) {
                            if (mSelectedItems[i] == 1) {
                                interestedNetwork |= (1 << i);
                            }
                        }
                        if (0 != interestedNetwork) {
                            RM.RMSelectNetwork(interestedNetwork);
                        }

                        uninterestedNetwork = 0;

                        for (int i = 0; i < mUnSelectedItems.length; i++)
                        {
                            if (mUnSelectedItems[i] == 1)
                            {
                                uninterestedNetwork |= (1 << i);
                            }
                        }
                        if (0 != uninterestedNetwork) {
                            RM.RMUnSelectNetwork(uninterestedNetwork);
                        }

                    }
                }).show();
    }

    private void checkSendNetworkType(String title)
    {
        selectedNetworkType = -1;
        AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);

        builder.setTitle(title)
                .setSingleChoiceItems(mNetworkCheckBoxItems, -1,
                        new DialogInterface.OnClickListener() {

                            @Override
                            public void onClick(DialogInterface dialog,
                                                int which) {
                                selectedNetworkType = which;
                            }
                        })
                .setPositiveButton("OK", new DialogInterface.OnClickListener() {

                    @Override
                    public void onClick(DialogInterface dialog, int which) {

                        if (selectedNetworkType == Network.IP.ordinal())
                        {
                            selectedNetwork = CA_IP;
                            DLog.v(TAG, "Selected Network is IP");
                        } else if (selectedNetworkType == Network.LE.ordinal())
                        {
                            selectedNetwork = CA_LE;
                            DLog.v(TAG, "Selected Network is LE");
                        } else if (selectedNetworkType == Network.EDR.ordinal())
                        {
                            selectedNetwork = CA_EDR;
                            DLog.v(TAG, "Selected Network is EDR");
                        } else
                        {
                            DLog.v(TAG, "Selected Network is NULL");
                            selectedNetwork = -1;
                        }
                    }
                }).show();
    }

    public void OnResponseReceived(String subject, String receivedData)
    {
        String callbackData = subject + receivedData;
        DLog.v(TAG, callbackData);
    }

    public void handleResponse()
    {
        isHandlerAlive = true;

        this.responseThread = new Thread()
        {
            public void run()
            {
                try
                {
                    while (isHandlerAlive)
                    {
                        sleep(2000);
                        RM.RMHandleRequestResponse();
                    }
                }
                catch (InterruptedException localInterruptedException)
                {
                    localInterruptedException.printStackTrace();
                }
            }
        };
        responseThread.start();
    }
}

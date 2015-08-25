/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/
package com.re.sampleclient;

import java.util.ArrayList;
import java.util.Calendar;

import android.app.Activity;
import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.Toast;

/*
 * Activity for handling user's selection on UI for Resource Client APIs.
 * & for updating UI.
 */
public class ResourceClientActivity extends Activity {

    private ListView                      list;
    private Button                        discoverResourceButton;
    private ArrayAdapter<String>          groupApis;
    private ArrayList<String>             clientApisList;
    private static ResourceClientActivity clientApiActivityObj;
    private ResourceClient                resourceClientObj;
    private static Handler                mHandler;

    private static EditText               logs;
    private static String                 logMessage;

    public static Context                 mcontext;
    public static Calendar                scheduleTime;

    private final String                  LOG_TAG = "[ReSample] "
                                                          + this.getClass()
                                                                  .getSimpleName(); ;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.resourceclientapis);

        clientApiActivityObj = this;
        mcontext = this;

        resourceClientObj = new ResourceClient();
        clientApisList = new ArrayList<String>();
        list = (ListView) findViewById(R.id.groupaApiList);
        discoverResourceButton = (Button) findViewById(R.id.button1);
        logs = (EditText) findViewById(R.id.EditText);

        // adding the item to list that will be displayed on the UI.
        clientApisList.add("1. Start Monitoring"); // 0
        clientApisList.add("2. Stop Monitoring"); // 1

        clientApisList.add("3. Get Remote Attributes"); // 2
        clientApisList.add("4. Set Remote Attributes"); // 3

        clientApisList.add("5. Start Caching without callback"); // 4
        clientApisList.add("6. Start Caching with callback"); // 5

        clientApisList.add("7. Get Cache State"); // 6
        clientApisList.add("8. Get All Cached Attributes"); // 7
        clientApisList.add("9. Get Cached Attribute"); // 8

        clientApisList.add("10. Stop Caching"); // 9

        // handler for updating the UI i.e. MessageLog (TextBox) & ListView
        mHandler = new Handler() {
            @Override
            public void handleMessage(Message msg) {
                switch (msg.what) {
                    case 0:
                        groupApis = new ArrayAdapter<String>(
                                clientApiActivityObj,
                                android.R.layout.simple_list_item_1,
                                clientApisList);
                        list.setAdapter(groupApis);
                        list.bringToFront();
                        break;
                    case 1:
                        logs.setText("");
                        logs.setText(logMessage);
                        Log.i(LOG_TAG, logMessage);
                }
            }
        };
        setHandler(mHandler);

        // Discover ResourceButton Listener
        discoverResourceButton.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                resourceClientObj.disocverResources();
            }
        });

        // Listener for item clicked by the user on the UI
        list.setOnItemClickListener(new OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view,
                    int position, long id) {
                if (position == 0) {
                    resourceClientObj.startMonitoring();
                } else if (position == 1) {
                    resourceClientObj.stopMonitoring();
                } else if (position == 2) {
                    resourceClientObj.getRemoteAttributes();
                } else if (position == 3) {
                    userInputDialog();
                } else if (position == 4) {
                    resourceClientObj.startCaching(0);
                } else if (position == 5) {
                    resourceClientObj.startCaching(1);
                } else if (position == 6) {
                    resourceClientObj.getCacheState();
                } else if (position == 7) {
                    resourceClientObj.getCachedAttributes();
                } else if (position == 8) {
                    resourceClientObj.getCachedAttribute();
                } else if (position == 9) {
                    resourceClientObj.stopCaching();
                }
            }
        });
    }

    // Dialog for taking user input for setting the attribute [Temperature]
    // value
    private void userInputDialog() {

        if (null == resourceClientObj) {
            displayToastMessage("Resource Object is NULL");
        } else {
            final Dialog dialog = new Dialog(mcontext);
            dialog.setContentView(R.layout.userinputforattributevalue);
            dialog.setTitle("Enter the Temperature Value");

            dialog.setCancelable(false);
            dialog.show();
            Button ok = (Button) dialog.findViewById(R.id.ok);
            Button cancel = (Button) dialog.findViewById(R.id.cancel);

            ok.setOnClickListener(new OnClickListener() {
                @Override
                public void onClick(View v) {

                    EditText regionValue = (EditText) dialog
                            .findViewById(R.id.attributeValue);
                    String value = regionValue.getText().toString();
                    if (value.equalsIgnoreCase("")) {
                        String toastmessage = "Please enter the Temperature Value";
                        displayToastMessage(toastmessage);
                    } else {
                        dialog.dismiss();
                        int val = Integer.parseInt(value);
                        resourceClientObj.setRemoteAttributes(val);
                    }
                }
            });
            cancel.setOnClickListener(new OnClickListener() {
                @Override
                public void onClick(View v) {
                    dialog.dismiss();
                }
            });
        }
    }

    public static void setMessageLog(String message) {
        logMessage = message;
    }

    @Override
    public void onBackPressed() {
        super.onBackPressed();
    }

    public static ResourceClientActivity getResourceClientActivityObj() {
        return clientApiActivityObj;
    }

    public Handler getHandler() {
        return mHandler;
    }

    public void setHandler(Handler mHandler) {
        ResourceClientActivity.mHandler = mHandler;
    }

    public void displayToastMessage(String message) {
        Toast toast = Toast.makeText(this, message, Toast.LENGTH_SHORT);
        toast.show();
        Log.i(LOG_TAG, message);
    }
}

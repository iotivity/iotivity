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

package com.example.sampleserver;

import java.util.ArrayList;

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
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.Toast;
import android.widget.AdapterView.OnItemClickListener;

import com.example.sampleresourceserver.R;

/**
 * Activity for handling user's selection on UI & for updating UI.
 */
public class ServerBuilderDevActivity extends Activity {

    private static ServerBuilderDevActivity activityObj;
    private ArrayAdapter<String>            apis;
    private ArrayList<String>               apisList;
    private final String                    LOG_TAG                         = "[ReSample] "
                                                                                    + this.getClass()
                                                                                            .getSimpleName();
    private ListView                        list;
    private EditText                        logs;
    private static Handler                  mHandler;
    public static Context                   mcontext;
    private static String                   logMessage;
    private ServerBuilder                   builderObj;

    public static boolean                   getHandlerFlag                  = false;
    public static boolean                   setHandlerFlag                  = false;
    public static boolean                   setAttributeUpdatedListenerFlag = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.server_builder_dev);

        logs = (EditText) findViewById(R.id.devLog);
        logs.setText("Resource Created Successfully");

        activityObj = this;
        mcontext = this;
        builderObj = new ServerBuilder();

        list = (ListView) findViewById(R.id.serverBuilderDevList);
        apisList = new ArrayList<String>();

        // adding the item to list that will be displayed on the UI.
        apisList.add("Set Temperature");
        apisList.add("Set Get Request Handler");
        apisList.add("Set Set Request Handler");
        apisList.add("Set Attribute Updated Listener");
        apisList.add("Remove Attribute Updated Listener");

        apis = new ArrayAdapter<String>(activityObj,
                android.R.layout.simple_list_item_1, apisList);
        list.setAdapter(apis);

        // handling user's selection on the UI
        list.setOnItemClickListener(new OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view,
                    int position, long id) {

                if (position == 0) {
                    userInputDialog();
                }
                if (position == 1) {
                    if (getHandlerFlag == true) {
                        displayToastMessage("GetRequest Handle already set");
                    } else {
                        builderObj.setGetRequestListener();
                        getHandlerFlag = true;
                    }
                }
                if (position == 2) {
                    if (setHandlerFlag == true) {
                        displayToastMessage("GetRequest Handle already set");
                    } else {
                        builderObj.setSetRequestListener();
                        setHandlerFlag = true;
                    }
                }
                if (position == 3) {
                    if (setAttributeUpdatedListenerFlag == true) {
                        displayToastMessage("Attribute Updated Listener already set");
                    } else {
                        builderObj.setAttributeUpdatedListener();
                        setAttributeUpdatedListenerFlag = true;
                    }
                }
                if (position == 4) {
                    if (setAttributeUpdatedListenerFlag == false) {
                        displayToastMessage("Attribute Updated Listener is not set");
                    } else {
                        builderObj.removeAttributeUpdatedListener();
                        setAttributeUpdatedListenerFlag = false;
                    }
                }
            }
        });

        // handler for updating the UI i.e. MessageLog (TextBox) & ListView
        mHandler = new Handler() {
            @Override
            public void handleMessage(Message msg) {
                switch (msg.what) {
                    case 0:
                        logs.setText("");
                        logs.setText(logMessage);
                        Log.i(LOG_TAG, logMessage);
                        break;
                }
            }
        };
        setHandler(mHandler);
    }

    // Dialog for taking user input for setting the attribute [Temperature]
    // value
    private void userInputDialog() {

        if (null == builderObj) {
            displayToastMessage("Resource Object is NULL");
        } else {
            final Dialog dialog = new Dialog(mcontext);
            dialog.setContentView(R.layout.userinputfortemperaturevalue);
            dialog.setTitle("Enter the Temperature Value");

            dialog.setCancelable(false);
            dialog.show();
            Button ok = (Button) dialog.findViewById(R.id.ok);
            Button cancel = (Button) dialog.findViewById(R.id.cancel);

            ok.setOnClickListener(new OnClickListener() {
                @Override
                public void onClick(View v) {

                    EditText temperatureValue = (EditText) dialog
                            .findViewById(R.id.attributeValue);
                    String value = temperatureValue.getText().toString();
                    if (value.equalsIgnoreCase("")) {
                        String toastmessage = "Please enter the Temperature Value";
                        displayToastMessage(toastmessage);
                    } else {
                        dialog.dismiss();
                        int val = Integer.parseInt(value);

                        builderObj.setAttribute(val);
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

    @Override
    public void onBackPressed() {
        super.onBackPressed();
    }

    // ****Utility Functions*****//

    public void setMessageLog(String message) {
        logMessage = message;
    }

    public static ServerBuilderDevActivity getServerBuilderDevActivity() {
        return activityObj;
    }

    public Handler getHandler() {
        return mHandler;
    }

    public void setHandler(Handler mHandler) {
        ServerBuilderDevActivity.mHandler = mHandler;
    }

    public void displayToastMessage(String message) {
        Toast toast = Toast.makeText(this, message, Toast.LENGTH_SHORT);
        toast.show();
        Log.i(LOG_TAG, message);
    }
}

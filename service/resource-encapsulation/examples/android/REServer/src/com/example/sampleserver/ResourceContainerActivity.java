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

import com.example.sampleresourceserver.*;
import android.app.Activity;
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

/**
 * Activity for handling user's selection on UI for Resource container APIs. &
 * for updating UI.
 */
public class ResourceContainerActivity extends Activity {

    private final String                     LOG_TAG = "[ReSample] "
                                                             + this.getClass()
                                                                     .getSimpleName();
    private static ResourceContainerActivity resourceContainerActivityInstance;
    private ResourceContainer                resourceContainerInstance;
    private static String                    logMessage;

    private ArrayAdapter<String>             containerApis;
    private ArrayList<String>                containerApiList;
    private static Handler                   mHandler;
    private ListView                         list;
    private Button                           startContainer;
    private Button                           stopContainer;
    private static EditText                  logs;
    private static String                    sdCardPath;

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.resource_container);

        resourceContainerActivityInstance = this;
        resourceContainerInstance = new ResourceContainer();

        containerApiList = new ArrayList<String>();

        list = (ListView) findViewById(R.id.containerApiList);
        startContainer = (Button) findViewById(R.id.startContainer);
        stopContainer = (Button) findViewById(R.id.stopContainer);
        stopContainer.setEnabled(false);
        logs = (EditText) findViewById(R.id.log);

        sdCardPath = this.getFilesDir().getPath();
        // adding the item to list that will be displayed on the UI.
        containerApiList.add("1. List Bundles");
        containerApiList.add("2. List bundle resources");
        containerApiList.add("3. Add Resource Config");
        containerApiList.add("4. Remove Resource Config");
        containerApiList.add("5. Remove Bundle");
        containerApiList.add("6. Add Bundle");
        containerApiList.add("7. Start Bundle");
        containerApiList.add("8. Stop Bundle");

        // handler for updating the UI i.e. MessageLog (TextBox) & ListView
        mHandler = new Handler() {
            @Override
            public void handleMessage(Message msg) {
                switch (msg.what) {
                    case 0:
                        containerApis = new ArrayAdapter<String>(
                                resourceContainerActivityInstance,
                                android.R.layout.simple_list_item_1,
                                containerApiList);
                        list.setAdapter(containerApis);
                        list.bringToFront();
                        break;
                    case 1:
                        logs.setText("");
                        logs.setText(logMessage);
                        Log.i(LOG_TAG, logMessage);
                        break;
                    case 2:
                        list.setAdapter(null);
                        break;
                }
            }
        };
        setHandler(mHandler);

        // StartContainer Button Listener
        startContainer.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {

                resourceContainerInstance.startContainer(sdCardPath);
                startContainer.setEnabled(false);
                stopContainer.setEnabled(true);

            }
        });

        // StopContainer Button Listener
        stopContainer.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                resourceContainerInstance.stopContainer();
                startContainer.setEnabled(true);
                stopContainer.setEnabled(false);
                Message msg;
                msg = Message.obtain();
                msg.what = 2;
                resourceContainerActivityInstance.getHandler().sendMessage(msg);
            }
        });

        // Listener for item clicked by the user on the UI
        list.setOnItemClickListener(new OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view,
                    int position, long id) {
                if (position == 0) {
                    resourceContainerInstance.listBundles();
                } else if (position == 1) {
                    resourceContainerInstance.listBundleResources();
                } else if (position == 2) {
                    resourceContainerInstance.addResourceConfig();
                } else if (position == 3) {
                    resourceContainerInstance.removeResourceConfig();
                } else if (position == 4) {
                    resourceContainerInstance.removeBundle();
                } else if (position == 5) {
                    resourceContainerInstance.addBundle();
                } else if (position == 6) {
                    resourceContainerInstance.startBundle();
                } else if (position == 7) {
                    resourceContainerInstance.stopBundle();
                }
            }
        });
    }

    @Override
    public void onBackPressed() {
        super.onBackPressed();
    }

    // ****Utility Functions*****//

    public static void setMessageLog(String message) {
        logMessage = message;
    }

    public static ResourceContainerActivity getResourceContainerActivityObj() {
        return resourceContainerActivityInstance;
    }

    public Handler getHandler() {
        return mHandler;
    }

    public void setHandler(Handler mHandler) {
        ResourceContainerActivity.mHandler = mHandler;
    }

    public void displayToastMessage(String message) {
        Toast toast = Toast.makeText(this, message, Toast.LENGTH_SHORT);
        toast.show();
        Log.i(LOG_TAG, message);
    }
}

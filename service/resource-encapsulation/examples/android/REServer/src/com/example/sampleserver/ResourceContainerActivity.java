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
import java.util.HashMap;
import java.util.List;

import com.example.sampleresourceserver.*;

import android.app.Activity;
import android.content.Context;
import android.graphics.Typeface;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.View.OnClickListener;
import android.widget.BaseExpandableListAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ExpandableListAdapter;
import android.widget.ExpandableListView;
import android.widget.ExpandableListView.OnChildClickListener;
import android.widget.TextView;
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

    public ExpandableListAdapter             listAdapter;
    public ExpandableListView                expListView;
    public List<String>                      sensors;
    public List<String>                      diApiList;
    public List<String>                      bmiApiList;
    HashMap<String, List<String>>            listChild;

    private static Handler                   mHandler;
    private Button                           startContainer;
    private Button                           listBundles;
    private static EditText                  logs;
    private static String                    sdCardPath;

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.resource_container);
        resourceContainerActivityInstance = this;
        resourceContainerInstance = new ResourceContainer();

        expListView = (ExpandableListView) findViewById(R.id.lvExp);
        startContainer = (Button) findViewById(R.id.startContainer);
        listBundles = (Button) findViewById(R.id.listBundles);
        listBundles.setEnabled(false);
        logs = (EditText) findViewById(R.id.log);

        sensors = new ArrayList<String>();
        diApiList = new ArrayList<String>();
        bmiApiList = new ArrayList<String>();
        listChild = new HashMap<String, List<String>>();

        // Adding list items (header)
        sensors.add("Discomfort Index Sensor");
        sensors.add("BMI Sensor");

        // Adding child data [discomfort Index sensor]
        diApiList.add("1. List bundle resources");
        diApiList.add("2. Add Resource");
        diApiList.add("3. Remove Resource");

        // Adding child data [BMI sensor]
        bmiApiList.add("1. Add Bundle");
        bmiApiList.add("2. Start Bundle");
        bmiApiList.add("3. List bundle resources");
        bmiApiList.add("4. Add Resource ");
        bmiApiList.add("5. Remove Resource");
        bmiApiList.add("6. Stop Bundle");
        bmiApiList.add("7. Remove Bundle");

        listChild.put(sensors.get(0), diApiList);
        listChild.put(sensors.get(1), bmiApiList);
        listAdapter = new ExpandableList(this, sensors, listChild);

        // getting the sd card path
        sdCardPath = this.getFilesDir().getPath();

        // handler for updating the UI i.e. MessageLog (TextBox) & ListView
        mHandler = new Handler() {
            @Override
            public void handleMessage(Message msg) {
                switch (msg.what) {
                    case 0:
                        expListView.setAdapter(listAdapter);
                        expListView.bringToFront();
                        break;
                    case 1:
                        logs.setText("");
                        logs.setText(logMessage);
                        Log.i(LOG_TAG, logMessage);
                        break;
                    case 2:
                        listAdapter = null;
                        expListView.setAdapter(listAdapter);
                        break;
                }
            }
        };
        setHandler(mHandler);

        // StartContainer/stopContainer Button Listener
        startContainer.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {

                String text = (String) startContainer.getText();
                if (text.contains("Start")) {
                    resourceContainerInstance.startContainer(sdCardPath);
                    listAdapter = new ExpandableList(ResourceContainerActivity
                            .getResourceContainerActivityObj(), sensors,
                            listChild);
                    listBundles.setEnabled(true);
                    startContainer.setText("Stop Container");
                } else {
                    resourceContainerInstance.stopContainer();
                    startContainer.setText("Start Container");
                    listBundles.setEnabled(false);
                    Message msg;
                    msg = Message.obtain();
                    msg.what = 2;
                    resourceContainerActivityInstance.getHandler().sendMessage(
                            msg);
                }
            }
        });

        // List Bundles Button Listener
        listBundles.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                resourceContainerInstance.listBundles();
            }
        });

        // Listener for the expandable list
        expListView.setOnChildClickListener(new OnChildClickListener() {

            @Override
            public boolean onChildClick(ExpandableListView parent, View v,
                    int groupPosition, int childPosition, long id) {

                if (0 == groupPosition) {
                    if (childPosition == 0) {
                        resourceContainerInstance.listBundleResourcesDI();
                    } else if (childPosition == 1) {
                        resourceContainerInstance.addResourceConfigDI();
                    } else if (childPosition == 2) {
                        resourceContainerInstance.removeResourceConfigDI();
                    }
                } else {
                    if (childPosition == 0) {
                        resourceContainerInstance.addBundleBMI();
                    } else if (childPosition == 1) {
                        resourceContainerInstance.startBundleBMI();
                    } else if (childPosition == 2) {
                        resourceContainerInstance.listBundleResourcesBMI();
                    } else if (childPosition == 3) {
                        resourceContainerInstance.addResourceConfigBMI();
                    } else if (childPosition == 4) {
                        resourceContainerInstance.removeResourceConfigBMI();
                    } else if (childPosition == 5) {
                        resourceContainerInstance.stopBundleBMI();
                    } else if (childPosition == 6) {
                        resourceContainerInstance.removeBundleBMI();
                    }
                }
                return false;
            }
        });
    }

    @Override
    public void onBackPressed() {
        listAdapter = null;
        expListView.setAdapter(listAdapter);
        resourceContainerInstance.stopContainer();
        ResourceContainer.startBundleFlag = false;
        super.onBackPressed();
    }

    // class for handling expandable list items
    public class ExpandableList extends BaseExpandableListAdapter {

        private Context                       mContext;
        private HashMap<String, List<String>> mListDataChild;
        private List<String>                  mListDataHeader;

        // constructor
        public ExpandableList(Context context, List<String> dataHeader,
                HashMap<String, List<String>> childData) {
            this.mContext = context;
            this.mListDataHeader = dataHeader;
            this.mListDataChild = childData;
        }

        // get the child ID
        @Override
        public long getChildId(int grpPosition, int childPosition) {
            return childPosition;
        }

        // get the child
        @Override
        public Object getChild(int grpPosition, int childPosititon) {
            return this.mListDataChild.get(
                    this.mListDataHeader.get(grpPosition)).get(childPosititon);
        }

        // get Group View
        @Override
        public View getGroupView(int grpPosition, boolean isExpandable,
                View changeView, ViewGroup head) {
            String mainHeading = (String) getGroup(grpPosition);
            if (changeView == null) {
                LayoutInflater flater;
                flater = (LayoutInflater) this.mContext
                        .getSystemService(Context.LAYOUT_INFLATER_SERVICE);
                changeView = flater.inflate(R.layout.group, null);
            }

            TextView listHeader = (TextView) changeView
                    .findViewById(R.id.ListHead);
            listHeader.setTypeface(null, Typeface.BOLD);
            listHeader.setText(mainHeading);
            return changeView;
        }

        // get Children count
        @Override
        public int getChildrenCount(int grpPosition) {
            int count = this.mListDataChild.get(
                    this.mListDataHeader.get(grpPosition)).size();
            return count;
        }

        // Get Group
        @Override
        public Object getGroup(int grpPosition) {
            return this.mListDataHeader.get(grpPosition);
        }

        // get Group size
        @Override
        public int getGroupCount() {
            int size = this.mListDataHeader.size();
            return size;
        }

        // get Group ID
        @Override
        public long getGroupId(int grpPosition) {
            return grpPosition;
        }

        // get Group View
        @Override
        public View getChildView(int grpPosition, final int childPosition,
                boolean isLastItem, View changeView, ViewGroup head) {

            final String innerText = (String) getChild(grpPosition,
                    childPosition);

            if (changeView == null) {
                LayoutInflater flater = (LayoutInflater) this.mContext
                        .getSystemService(Context.LAYOUT_INFLATER_SERVICE);
                changeView = flater.inflate(R.layout.list_item, null);
            }

            TextView textListChild = (TextView) changeView
                    .findViewById(R.id.listItem);

            textListChild.setText(innerText);
            return changeView;
        }

        // To check whether child is selectable or not
        @Override
        public boolean isChildSelectable(int grpPosition, int childPosition) {
            return true;
        }

        // To check the stable IDs
        @Override
        public boolean hasStableIds() {
            return false;
        }
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

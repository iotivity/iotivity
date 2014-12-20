//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
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

package com.sec.controller.ui;

import gen.cm.MyCMDevice;
import OC.Cm.ControlManager;
import OC.Cm.Device;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.ListView;

import com.sec.controller.R;

public class DeviceListView extends Activity {

    ListView              listView;
    static Device[]       mDevices = null;
    static public Context mContext = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_list_view_android_example);

        // Get ListView object from xml
        listView = (ListView) findViewById(R.id.devicelist);

        mContext = this;
        showDevices();
    }

    void showDevices() {

        String[] values = null;

        try {
            mDevices = ControlManager.getInstance(MyCMDevice.APP_ID)
                    .getDeviceFinder().getDevices();

            if (mDevices.length > 0) {

                values = new String[mDevices.length];
                for (int i = 0; i < mDevices.length; i++) {
                    values[i] = " Device : " + i;
                }
            }

        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }

        ArrayAdapter<String> adapter = new ArrayAdapter<String>(this,
                android.R.layout.simple_list_item_1, android.R.id.text1, values);

        // Assign adapter to ListView
        listView.setAdapter(adapter);

        // ListView Item Click Listener
        listView.setOnItemClickListener(new OnItemClickListener() {

            @Override
            public void onItemClick(AdapterView<?> parent, View view,
                    int position, long id) {
                // ListView Clicked item value
                // String itemValue = (String)
                // listView.getItemAtPosition(position);

                // Show Alert
                // Toast.makeText(getApplicationContext(),"Position :"+position+"  ListItem : "
                // +itemValue , Toast.LENGTH_SHORT).show();
                ResourceListView.mSelectedDevice = mDevices[position];

                Intent intent = new Intent(DeviceListView.mContext,
                        ResourceListView.class);
                intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                DeviceListView.mContext.startActivity(intent);

            }

        });
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {

        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();
        if (id == R.id.action_settings) {

            showDevices();
            return true;
        }
        return super.onOptionsItemSelected(item);
    }
}

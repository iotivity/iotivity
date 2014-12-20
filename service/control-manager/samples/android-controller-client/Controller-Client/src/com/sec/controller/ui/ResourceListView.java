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

import gen.cm.CMDevice;

import java.util.List;

import OC.Cm.Device;
import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.ListView;

import com.sec.controller.CMController;
import com.sec.controller.R;
import com.sec.controller.handler.HumidityResourceHandler;
import com.sec.controller.handler.InformationResourceHandler;
import com.sec.controller.handler.LightResourceHandler;
import com.sec.controller.handler.ModeResourceHandler;
import com.sec.controller.handler.NetworkResourceHandler;
import com.sec.controller.handler.TemperatureResourceHandler;
import com.sec.controller.handler.interfaces.IResourceHandler;

public class ResourceListView extends Activity {

    ListView              listView;
    static public Device  mSelectedDevice = null;
    static public Context mContext        = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_list_view_android_example);

        mContext = this;

        // Get ListView object from xml
        listView = (ListView) findViewById(R.id.devicelist);

        String[] values = null;

        try {
            List<String> resources = ((CMDevice) mSelectedDevice)
                    .getSupportedResourceTypes();

            if (resources != null && resources.size() > 0) {
                values = new String[resources.size()];
                for (int i = 0; i < resources.size(); i++) {
                    values[i] = resources.get(i);
                }
            }

            if (values != null && values.length > 0) {
                ArrayAdapter<String> adapter = new ArrayAdapter<String>(this,
                        android.R.layout.simple_list_item_1,
                        android.R.id.text1, values);

                // Assign adapter to ListView
                listView.setAdapter(adapter);

                // ListView Item Click Listener
                listView.setOnItemClickListener(new OnItemClickListener() {

                    @Override
                    public void onItemClick(AdapterView<?> parent, View view,
                            int position, long id) {

                        // ListView Clicked item value
                        String resourceType = (String) listView
                                .getItemAtPosition(position);
                        IResourceHandler rHandler = null;

                        if (resourceType.equals("Light")) {
                            rHandler = new LightResourceHandler(mSelectedDevice);
                        } else if (resourceType.equals("Humidity")) {
                            rHandler = new HumidityResourceHandler(
                                    mSelectedDevice);
                        } else if (resourceType.equals("Network")) {
                            rHandler = new NetworkResourceHandler(
                                    mSelectedDevice);
                        }
                        /*
                         * else if (resourceType.equals("WiFi")){ rHandler = new
                         * WifiResourceHandler(mSelectedDevice); }
                         */
                        else if (resourceType.equals("Temperaturs")) {
                            rHandler = new TemperatureResourceHandler(
                                    mSelectedDevice);
                        } else if (resourceType.equals("Information")) {
                            rHandler = new InformationResourceHandler(
                                    mSelectedDevice);
                        } else if (resourceType.equals("Mode")) {
                            rHandler = new ModeResourceHandler(mSelectedDevice);
                        } else if (resourceType.equals("Operation")) {
                            rHandler = new ModeResourceHandler(mSelectedDevice);
                        } else {
                            CMController
                                    .displayToastMessage("Currently not supported : "
                                            + resourceType);
                        }

                        if (null != rHandler) {
                            rHandler.get();
                        }

                    }

                });
            }

        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }

    }
}

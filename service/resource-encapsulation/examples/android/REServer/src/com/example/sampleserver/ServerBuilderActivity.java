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

import com.example.sampleresourceserver.R;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.AdapterView.OnItemClickListener;

/**
 * Starting Activity for ServerBuilder and redirect to ServerBuilderAutoActivity
 * or ServerBuilderDevActivity as per user's selection
 */
public class ServerBuilderActivity extends Activity {

    private static ServerBuilderActivity activityObj;
    private ArrayAdapter<String>         apis;
    private ArrayList<String>            apisList;
    private ListView                     list;

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.server_builder);
        activityObj = this;
        list = (ListView) findViewById(R.id.buildeList);
        apisList = new ArrayList<String>();

        // adding the item to list that will be displayed on the UI.
        apisList.add("Auto Control for created Resource");
        apisList.add("Developer Control for created Resource");
        apis = new ArrayAdapter<String>(activityObj,
                android.R.layout.simple_list_item_1, apisList);
        list.setAdapter(apis);

        // handling user's selection on the UI
        list.setOnItemClickListener(new OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view,
                    int position, long id) {

                if (position == 0) {

                    Intent intent = new Intent(activityObj,
                            ServerBuilderAutoActivity.class);
                    startActivity(intent);
                }
                if (position == 1) {

                    Intent intent = new Intent(activityObj,
                            ServerBuilderDevActivity.class);
                    startActivity(intent);
                }
            }
        });
    }

    @Override
    public void onBackPressed() {
        super.onBackPressed();
    }
}

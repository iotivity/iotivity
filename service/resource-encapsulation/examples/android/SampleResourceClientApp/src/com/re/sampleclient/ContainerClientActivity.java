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

import java.util.Vector;

import org.iotivity.ResourceEncapsulation.client.RCSAddress;
import org.iotivity.ResourceEncapsulation.client.RCSDiscoveryManager;
import org.iotivity.ResourceEncapsulation.client.RCSDiscoveryManager.IDiscoverResourceListener;
import org.iotivity.ResourceEncapsulation.client.RCSRemoteResourceObject;
import org.iotivity.ResourceEncapsulation.common.RCSException;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;

/**
 * It contains the discover resource API for Discovering Container Resource
 */
public class ContainerClientActivity extends Activity {

    private final RCSDiscoveryManager     discoveyManagerObj = RCSDiscoveryManager
                                                                     .getInstance();
    private final String                  LOG_TAG            = "[ReSample] "
                                                                     + this.getClass()
                                                                             .getSimpleName();
    private Button                        discoverResourceButton;
    private EditText                      logs;
    public static String                  logMessage;
    private DiscoverResourceListener      discoverResourceListener;
    public static ContainerClientActivity containerClientActivityObj;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.resource_container);

        containerClientActivityObj = this;
        discoverResourceButton = (Button) findViewById(R.id.discoverResource);
        logs = (EditText) findViewById(R.id.log);

        discoverResourceListener = new DiscoverResourceListener();
        discoveyManagerObj
                .setDiscoverResourceListener(discoverResourceListener);

        discoverResourceButton.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                RCSAddress address = RCSAddress.multicast();
                try {
                    discoveyManagerObj.discoverResourceByType(address,
                            "oic.softsensor");
                    logMessage = "";
                    logs.setText(logMessage);
                } catch (RCSException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    /**
     * Listener for receiving Resource discovered in the network.
     */
    private class DiscoverResourceListener implements IDiscoverResourceListener {

        @Override
        public void onResourceDiscovered(RCSRemoteResourceObject foundResource) {
            Log.i(LOG_TAG, "onResourceDiscovered invoked");

            String resourceURI = foundResource.getUri();
            String hostAddress = foundResource.getAddress();
            boolean isObservableflag = foundResource.isObservable();

            Vector<String> resourceTypes = foundResource.getTypes();
            Vector<String> resourceInterfaces = foundResource.getInterFaces();

            logMessage = "URI : " + resourceURI + "\n";
            logMessage = logMessage + "Host : " + hostAddress + "\n";
            for (int i = 0; i < resourceTypes.size(); i++) {
                logMessage = logMessage + "resourceType : "
                        + resourceTypes.get(i) + "\n";
            }

            for (int i = 0; i < resourceInterfaces.size(); i++) {
                logMessage = logMessage + "resourceInterfaces : "
                        + resourceInterfaces.get(i) + "\n";
            }
            logMessage = logMessage + "isObservable : " + isObservableflag
                    + "\n";

            ContainerClientActivity.this.runOnUiThread(new Runnable() {
                public void run() {
                    logs.setText(logMessage);
                }
            });
        }
    };
}

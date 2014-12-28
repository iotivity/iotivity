//******************************************************************
//
// Copyright 2014 MediaTek All Rights Reserved.
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

package org.iotivity.simpleclient;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;

import org.iotivity.base.OCPlatform;
import org.iotivity.base.OCRepresentation;
import org.iotivity.base.OCResource;
import org.iotivity.base.ObserveType;
import org.iotivity.base.PlatformConfig;


public class SimpleClient extends Activity {
    final private static String TAG = "SimpleClient";

    class Light {
        public boolean m_state;
        public int m_power;
        public String m_name;

        public Light() {
            m_state = false;
            m_power = 0;
            m_name = "vincent";
        }
    }

    static int OBSERVE_TYPE_TO_USE = ObserveType.Observe;

    public static Light mylight;
    public static OCResource curResource;
    static int oc = 0;
    static Activity mActivity;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mActivity = this;

        mylight = new Light();

		PlatformConfig cfg = new PlatformConfig(PlatformConfig.ServiceType.INPROC,
				                                PlatformConfig.ModeType.CLIENT,
				                                "0.0.0.0",
				                                0,
				                                PlatformConfig.QualityOfService.LO_QOS);

        OCPlatform.configure(cfg);
        FoundResource foundResource = new FoundResource();
        OCPlatform.findResource("", "coap://224.0.1.187/oc/core?rt=core.light", foundResource);

        setContentView(R.layout.activity_simple_client);
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.simple_client, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();
        if (id == R.id.action_settings) {
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    protected static void getLightRepresentation(OCResource resource) {
        if(resource != null) {
            Log.e(TAG, "Getting Light Representation ...");

            OnGet onGet = new OnGet();
            resource.get(onGet);
        }
    }

    protected static void putLightRepresentation(OCResource resource) {
        if(resource != null) {

            OCRepresentation rep = new OCRepresentation();

            Log.e(TAG, "Putting Light Representation ...");

            mylight.m_state = true;
            mylight.m_power = 15;

            Log.i(TAG, "setValueBool : " + SimpleClient.mylight.m_state);
            Log.i(TAG, "setValueInt  : " + SimpleClient.mylight.m_power);
            rep.setValueBool("state", mylight.m_state);
            rep.setValueInt("power", mylight.m_power);

            OnPut onPut = new OnPut();
            resource.put(rep, onPut);
        }
    }

    protected static void postLightRepresentation(OCResource resource) {
        if(resource != null) {

            OCRepresentation rep = new OCRepresentation();

            Log.e(TAG, "Posting Light Representation ...");

            mylight.m_state = false;
            mylight.m_power = 105;

            Log.i(TAG, "setValueBool : " + SimpleClient.mylight.m_state);
            Log.i(TAG, "setValueInt  : " + SimpleClient.mylight.m_power);
            rep.setValueBool("state", mylight.m_state);
            rep.setValueInt("power", mylight.m_power);

            OnPost onPost = new OnPost();
            resource.post(rep, onPost);
        }
    }

    protected static int observe_count() {
        return ++oc;
    }

}

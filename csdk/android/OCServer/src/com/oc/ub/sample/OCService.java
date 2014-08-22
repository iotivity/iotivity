//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
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

package com.oc.ub.sample;

import com.oc.OCSecurityModel;
import com.oc.OCServer;
import com.oc.ub.sampleclient.R;

import android.app.Activity;
import android.app.Fragment;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;

public class OCService extends Activity {
  private static String TAG = "OCCLIENT";
  private OCServer service = null;
  private MyLight resource = null;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_occlient);

    if (savedInstanceState == null) {
      getFragmentManager().beginTransaction().add(R.id.container, new PlaceholderFragment())
          .commit();
    }

	// NOTE: We throw exceptions for the bad errors... so no return codes
    try {
    	// In a complete example, the MyLight class should have callback to
    	// update the UI
	    resource = new MyLight();
	    service = new OCServer();
	    service.setSecurityModel(new OCSecurityModel());
	    // TODO: Mats, will we allow one resource to be mapped to two URLs???
	    service.registerResource(resource, "/light/a");
	    // TODO: Mats, will we allow one resource to be mapped to two URLs???
	    // service.registerResource(resource, "/light/b", new OCAccessHandler());
	    service.start();
    } catch (Exception exception) {
    	finish();
    }
  }

  @Override
  public boolean onCreateOptionsMenu(Menu menu) {

    // Inflate the menu; this adds items to the action bar if it is present.
    getMenuInflater().inflate(R.menu.occlient, menu);
    return true;
  }

  @Override
  public boolean onOptionsItemSelected(MenuItem item) {
    // Handle action bar item clicks here. The action bar will automatically handle clicks on the
    // Home/Up button, so long as you specify a parent activity in AndroidManifest.xml.
    int id = item.getItemId();
    if (id == R.id.action_settings) {
      return true;
    }
    return super.onOptionsItemSelected(item);
  }

  @Override
  protected void onDestroy() {
      super.onDestroy();

      try {
	      service.unregisterResource(resource);
	      service.stop();
      } catch (Exception exception) {
    	  // TODO: Handle exception
      }
  }
  
  /**
   * A placeholder fragment containing a simple view.
   */
  public static class PlaceholderFragment extends Fragment {

    public PlaceholderFragment() {}

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
      View rootView = inflater.inflate(R.layout.fragment_occlient, container, false);
      return rootView;
    }
  }
}

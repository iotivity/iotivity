//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
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

import com.oc.OCDiscovery;
import com.oc.OCObserver;
import com.oc.OCResource;
import com.oc.OCResourceResultHandler;
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

public class OCClient extends Activity {
  private static String TAG = "OCCLIENT";
  private OCServer ocServer = null;
  private OCDiscovery ocDiscovery = null;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_occlient);

    if (savedInstanceState == null) {
      getFragmentManager().beginTransaction().add(R.id.container, new PlaceholderFragment())
          .commit();
    }

    ocServer = new OCServer();
    ocServer.start();
    ocDiscovery = ocServer.getDiscoverySingleton();

    /*
     * APPLICATION NOTE: All callbacks will come through on a new thread and will not interfere with
     * the UI thread or the Framework Thread (i.e. Networking Thread) which means that you need to
     * use a Handler() to post back to the UI.
     */
    ocDiscovery.findResourceByType(null, null, new OCResourceResultHandler() {
      @Override
      public void onResourcesFound(String serviceURL, OCResource[] resources) {
        // NOTE: This means that a service is reporting back zero or more resources
        // During the callback, lets check to see if the resource supports the
        // light control interface and turn the light on and then subscribe to any changes in the
        // light's power state.
        for (OCResource resource : resources) {
          try {
            LightControl control = resource.getInterface(LightControl.class);

            if (control != null) {
              control.setPowered(true);
              control.isPowered();
              /*
               * APPLICATION NOTE: All callbacks will come through on a new thread and will not
               * interfere with the UI thread or the Framework Thread (i.e. Networking Thread) which
               * means that you need to use a Handler() to post back to the UI.
               */
              control.addPoweredObserver(new OCObserver<Boolean>() {
                @Override
                public void onChanged(Boolean data) {
                  // TODO: Tell the UI! The state of the Light has changed.

                }

                @Override
                public void onFailed(Throwable throwable) {
                  // TODO: Tell the UI! We are no longer subscribed to events for the Light's power
                  // state.
                }
              });
            }
          } catch (Exception exception) {
            // TODO: Ooops
          }
        }

      }

      @Override
      public void onFailed(Throwable throwable) {
        // TODO: Handle the error. The request to find resources has failed.
      }

      @Override
      public void onCompleted() {
        // Nothing to do here... we are just done finding resources is all.
      }
    });
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

    ocServer.stop();
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

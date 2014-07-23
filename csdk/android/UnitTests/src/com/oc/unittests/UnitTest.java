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

package com.oc.unittests;

import static org.junit.Assert.*;
import junit.framework.Assert;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import com.oc.OCDiscovery;
import com.oc.OCObserver;
import com.oc.OCResource;
import com.oc.OCResourceResultHandler;
import com.oc.OCSecurityModel;
import com.oc.OCServer;

public class UnitTest {
  private OCServer clientService = null;
  private OCServer serverService = null;
  private OCDiscovery ocDiscovery = null;
  private UnitTestResource serverResource = null;

  @Before
  public void init() throws Exception {
    clientService = new OCServer();
    clientService.start();
    ocDiscovery = clientService.getDiscoverySingleton();
  }

  @After
  public void destroy() throws Exception {
    try {
      clientService.stop();
      clientService = null;
      serverService.unregisterResource(serverResource);
      serverService.stop();
    } catch (Exception exception) {
      // TODO: Handle exception
    }
  }

  @Test
  public void findMyself() {
    client();
    server();
  }

  @SuppressWarnings("serial")
  public void client() {
    assertNotNull(clientService);

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
            UnitTestControlItf control = resource.getInterface(UnitTestControlItf.class);
            assertNotNull(control);
            if (control != null) {
              control.setState(true);
              control.isTrue();

              Assert.assertEquals(true, control.isTrue());

              /*
               * APPLICATION NOTE: All callbacks will come through on a new thread and will not
               * interfere with the UI thread or the Framework Thread (i.e. Networking Thread) which
               * means that you need to use a Handler() to post back to the UI.
               */
              control.addMyUnitTestObserver(new OCObserver<Boolean>() {
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

  public void server() {
    // NOTE: We throw exceptions for the bad errors... so no return codes
    try {
      // In a complete example, the MyLight class should have callback to
      // update the UI
      serverResource = new UnitTestResource();
      serverService = new OCServer();
      serverService.setSecurityModel(new OCSecurityModel());
      // TODO: Mats, will we allow one resource to be mapped to two URLs???
      serverService.registerResource(serverResource, "/ResourceTypeGoesHere/a");
      // TODO: Mats, will we allow one resource to be mapped to two URLs???
      // service.registerResource(resource, "/light/b", new OCAccessHandler());
      serverService.start();
    } catch (Exception exception) {
      // finish();
      return; // End the application.
    }
  }
}

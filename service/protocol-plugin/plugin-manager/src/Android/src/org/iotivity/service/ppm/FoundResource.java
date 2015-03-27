//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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

/// @file FoundResource.java

package org.iotivity.service.ppm;

import java.util.HashMap;

import org.iotivity.base.OcException;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcResource;

import android.util.Log;

public class FoundResource implements OcPlatform.OnResourceFoundListener {
    final private static String TAG = "FoundResource";

    public void onResourceFound(OcResource resource) {

        String resourceURI;
        String hostAddress;

        if (resource != null) {
            Log.i(TAG, "DISCOVERED Resource");

            resourceURI = resource.getUri();

            Log.i(TAG, "URI of the resource: " + resourceURI);

            hostAddress = resource.getHost();
            Log.i(TAG, "Host address of the resource: " + hostAddress);

            if (resourceURI.equals("/a/wemo")) {
                MainActivity.belkinResource = resource;
                OnGetBelkinplug onGet = new OnGetBelkinplug();
                try {
                    resource.get(new HashMap<String, String>(), onGet);
                } catch (OcException e) {
                    e.printStackTrace();
                }
            } else if (resourceURI.equals("/a/galaxy/gear")) {
                MainActivity.gearResource = resource;
                OnGetGear onGet = new OnGetGear();
                try {
                    resource.get(new HashMap<String, String>(), onGet);
                } catch (OcException e) {
                    e.printStackTrace();
                }
            } else if (resourceURI.equals("/a/huebulb")) {
                MainActivity.hueResource = resource;
                OnGetHuebulb onGet = new OnGetHuebulb();
                try {
                    resource.get(new HashMap<String, String>(), onGet);
                } catch (OcException e) {
                    e.printStackTrace();
                }
            } else {
                Log.e(TAG, "ResourceURI is invalid");
            }
        } else {
            Log.e(TAG, "Resource is invalid");
        }
    }
}

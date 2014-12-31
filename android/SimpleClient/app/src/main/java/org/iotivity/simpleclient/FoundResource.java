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

import android.util.Log;

import org.iotivity.base.AbstractFindCallback;
import org.iotivity.base.OCResource;

public class FoundResource extends AbstractFindCallback {
    final private static String TAG = "FoundResource";


    public void Callback(OCResource resource) {

        if(SimpleClient.curResource != null) {
            Log.e(TAG, "Found another resource, ignoring");
        }

        String resourceURI;
        String hostAddress;

        if(resource != null) {
            Log.i(TAG, "DISCOVERED Resource");

            resourceURI = resource.uri();
            Log.i(TAG, "URI of the resource: " + resourceURI);

            hostAddress = resource.host();
            Log.i(TAG, "Host address of the resource: " + hostAddress);

            new SimpleToast().execute("URI of the resource: " + resourceURI + "\nHost address of the resource: " + hostAddress);

            String[] resourceTypes = resource.getResourceTypes();
            for(int i=0; i<resourceTypes.length; i++) {
                Log.i(TAG, "[" + i + "] : " + resourceTypes[i]);
            }

            String[] resourceInterfaces = resource.getResourceInterfaces();
            for(int i=0; i<resourceInterfaces.length; i++) {
                Log.i(TAG, "[" + i + "] : " + resourceInterfaces[i]);
            }

            if(resourceURI.equals("/a/light")) {
                SimpleClient.curResource = resource;
                SimpleClient.getLightRepresentation(resource);
            }
        }
        else {
            Log.e(TAG, "Resource is invalid");
        }
    }

}

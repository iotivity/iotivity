//******************************************************************
////
//// Copyright 2015 Susang Park and Jake Choi (Seoul National University) All Rights Reserved.
////
////-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
////
//// Licensed under the Apache License, Version 2.0 (the "License");
//// you may not use this file except in compliance with the License.
//// You may obtain a copy of the License at
////
////      http://www.apache.org/licenses/LICENSE-2.0
////
//// Unless required by applicable law or agreed to in writing, software
//// distributed under the License is distributed on an "AS IS" BASIS,
//// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//// See the License for the specific language governing permissions and
//// limitations under the License.
////
////-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

package org.iotivity.service.sample.activitysensor;

import android.content.Context;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.Bundle;
import android.util.Log;

import org.iotivity.service.resourcecontainer.AndroidBundleResource;
import org.iotivity.service.resourcecontainer.RcsResourceAttributes;
import org.iotivity.service.resourcecontainer.RcsValue;

public class GpsSensor extends AndroidBundleResource {
    private static final String TAG = "GpsSensor";
    private LocationManager locationManager;

    public GpsSensor(Context context){
        super(context);
        this.setResourceType("oic.r.accelerometer");
        this.setName("accelerometer");

        locationManager = (LocationManager) context.getApplicationContext().getSystemService(Context.LOCATION_SERVICE);

        getMyLocation();
    }

    @Override
    protected void initAttributes() {}

    @Override
    public void handleSetAttributesRequest(RcsResourceAttributes value) {}

    @Override
    public RcsResourceAttributes handleGetAttributesRequest() {
        Log.i(TAG, "Get Attributes called and return: ");
        for (String key : m_attributes.keySet()) {
            Log.i(TAG, " " + key + ": " + m_attributes.get(key));
        }
        return this.m_attributes;
    }


    //sensing gps from mobile
    private void getMyLocation() {
        LocationListener locationListener = new LocationListener() {
            public void onLocationChanged(Location location) {
                double lat = location.getLatitude();
                double lng = location.getLongitude();
                setAttribute("latitude", new RcsValue( lat ), true);
                setAttribute("longitude", new RcsValue( lng ), true);

            }

            public void onStatusChanged(String provider, int status, Bundle extras) {}

            public void onProviderEnabled(String provider) {}

            public void onProviderDisabled(String provider) {}
        };
        locationManager.requestLocationUpdates(LocationManager.NETWORK_PROVIDER, 0, 0, locationListener);
        locationManager.requestLocationUpdates(LocationManager.GPS_PROVIDER, 0, 0, locationListener);
    }

}

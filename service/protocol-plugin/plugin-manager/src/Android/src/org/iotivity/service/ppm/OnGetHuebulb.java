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

/// @file OnGetHueblub.java

/// @brief This class is OnGetListener for Huebulb.

package org.iotivity.service.ppm;

import java.util.List;

import org.iotivity.base.ErrorCode;
import org.iotivity.base.OcException;
import org.iotivity.base.OcHeaderOption;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcResource;

import android.util.Log;

public class OnGetHuebulb implements OcResource.OnGetListener {
    final private static String TAG = "OnGetHuebulb";

    @Override
    public void onGetCompleted(List<OcHeaderOption> headerOptions,
            OcRepresentation rep) {
        Log.i(TAG, "GET request Hue was successful");
        Log.i(TAG, "Resource URI : " + rep.getUri());

        try{
            MainActivity.hueplug.m_name = rep.getValue("name");
            MainActivity.hueplug.m_power = rep.getValue("power");
            MainActivity.hueplug.m_bright = rep.getValue("brightness");
            MainActivity.hueplug.m_color = rep.getValue("color");
            MainActivity.hueplug.m_uri = rep.getValue("uri");
        } catch (OcException e) {
            Log.e(TAG, e.getMessage());
        }

        Log.i(TAG, "name : " + MainActivity.hueplug.m_name);
        Log.i(TAG, "power : " + MainActivity.hueplug.m_power);
        Log.i(TAG, "brightness : " + MainActivity.hueplug.m_bright);
        Log.i(TAG, "color : " + MainActivity.hueplug.m_color);
        Log.i(TAG, "uri : " + MainActivity.hueplug.m_uri);

        Log.e(TAG, "updating display from thread");
        MainActivity.mActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                MainActivity.updateHuebulbStatus();
            }
        });
    }

    @Override
    public void onGetFailed(Throwable ex) {
        if (ex instanceof OcException) {
            OcException ocEx = (OcException) ex;
            ErrorCode errCode = ocEx.getErrorCode();
        }
        Log.e(TAG, ex.toString());
    }
}

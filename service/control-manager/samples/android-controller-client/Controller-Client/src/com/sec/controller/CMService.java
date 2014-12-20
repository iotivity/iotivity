//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
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

package com.sec.controller;

import OC.Cm.ControlManager;
import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;

public class CMService extends Service {

    static final String TAG = "CMService";

    public CMService() {

    }

    @Override
    public void onCreate() {
        super.onCreate();

        Log.i(TAG, "CMService onCreate");

        try {

            ControlManager.loadNativeLibaries();
            CMController controller = CMController
                    .getCMControllerInstance(getApplicationContext());
            controller.startController();

        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }

        Log.i(TAG, "CMService onCreate END");

    }

    @Override
    public IBinder onBind(Intent arg0) {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public void onDestroy() {

        Log.i(TAG, "CMService onDestroy");
        CMController.getCMControllerInstance(getApplicationContext())
                .stopController();
    }

}

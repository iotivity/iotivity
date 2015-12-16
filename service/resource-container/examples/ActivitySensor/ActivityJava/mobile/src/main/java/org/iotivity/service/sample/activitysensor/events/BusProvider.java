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

package org.iotivity.service.sample.activitysensor.events;

import android.os.Handler;
import android.os.Looper;

import com.squareup.otto.Bus;

public final class BusProvider
{
    private static final Bus BUS = new Bus();

    public static Bus getInstance() {
        return BUS;
    }

    public static void postOnMainThread(final Object event) {
        Handler handler = new Handler(Looper.getMainLooper());

        handler.post(new Runnable() {
            public void run() {
                BUS.post(event);
            }
        });
    }

    private BusProvider() {
    }
}

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

import org.iotivity.base.AbstractObserveCallback;
import org.iotivity.base.OCHeaderOption;
import org.iotivity.base.OCRepresentation;
import org.iotivity.base.OCStackResult;

public class OnObserve extends AbstractObserveCallback {
    final private static String TAG = "OnObserve";

    public void Callback(OCHeaderOption[] options, OCRepresentation rep, int eCode, int seqNum) {
        if(eCode == OCStackResult.OC_STACK_OK) {
            {
                Log.i(TAG, "OBSERVE RESULT :");
                Log.i(TAG, "SequenceNumber : " + seqNum);

                SimpleClient.mylight.m_state = rep.getValueBool("state");
                Log.i(TAG, "state : " + SimpleClient.mylight.m_state);
                SimpleClient.mylight.m_power = rep.getValueInt("power");
                Log.i(TAG, "power : " + SimpleClient.mylight.m_power);
                SimpleClient.mylight.m_name = rep.getValueString("name");
                Log.i(TAG, "name  : " + SimpleClient.mylight.m_name);

                new SimpleToast().execute(
                        "state : " + SimpleClient.mylight.m_state +
                                "\npower : " + SimpleClient.mylight.m_power +
                                "\nname  : " + SimpleClient.mylight.m_name
                );

                if(SimpleClient.observe_count() > 5) {
                    Log.i(TAG, "Cancelling Observe ...");
                    OCStackResult result = SimpleClient.curResource.cancelObserve();
                    Log.i(TAG, "Cancel result : " + result);
                    try {
                        Thread.sleep(10000); // 10 sec
                    } catch (InterruptedException e) {
                        // TODO Auto-generated catch block
                        e.printStackTrace();
                    }
                    Log.i(TAG, "DONE");
                    new SimpleToast().execute(
                            "DONE"
                    );
                }

            }
        }
        else {
            Log.e(TAG, "onPut Response error : " + eCode);
        }

    }
}

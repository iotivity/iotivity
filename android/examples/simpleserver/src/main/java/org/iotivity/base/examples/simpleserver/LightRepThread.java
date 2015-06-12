/*
 * //******************************************************************
 * //
 * // Copyright 2015 Intel Corporation.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 * //
 * // Licensed under the Apache License, Version 2.0 (the "License");
 * // you may not use this file except in compliance with the License.
 * // You may obtain a copy of the License at
 * //
 * //      http://www.apache.org/licenses/LICENSE-2.0
 * //
 * // Unless required by applicable law or agreed to in writing, software
 * // distributed under the License is distributed on an "AS IS" BASIS,
 * // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * // See the License for the specific language governing permissions and
 * // limitations under the License.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

/*
 * // Starts a new thread for OBSERVE. It increments the value of mPower by 10 every 3 seconds
 */
package org.iotivity.base.examples.simpleserver;

import android.util.Log;

import org.iotivity.base.EntityHandlerResult;
import org.iotivity.base.OcException;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcResourceResponse;

import java.util.List;

/**
 * LightRepThread
 *
 * Spawn a thread for OBSERVE. This increments the resource's power value by 10 every 3 seconds.
 * If there is an observationList, call notifyListOfObservers(). Otherwise, call notifyAllObservers()
 */
public class LightRepThread extends Thread {
    private static String TAG = "LightRepThread: ";

    private LightResource mLightResource;
    private List<Byte> mObservationList;
    private static int count = 0;

    LightRepThread(Object lr, List<Byte> observationList) {
        mLightResource = (LightResource)lr;
        mObservationList = observationList;
    }

    public void run() {
        while(count < 20) {
            try {
                sleep(3000);
            } catch (InterruptedException e) {
                Log.e(TAG, e.getMessage());
            }

            // increment current power value by 10 every 3 seconds
            mLightResource.setPower(mLightResource.getPower() + 10);
            try {
                // if observationList is not empty, call notifyListOfObservers
                if (mObservationList.size() > 0) {
                    OcResourceResponse ocResourceResponse = new OcResourceResponse();
                    ocResourceResponse.setErrorCode(StringConstants.ERROR_CODE);
                    ocResourceResponse.setResponseResult(EntityHandlerResult.OK);
                    OcRepresentation r = mLightResource.get();
                    ocResourceResponse.setResourceRepresentation
                            (mLightResource.get(), OcPlatform.DEFAULT_INTERFACE);
                    OcPlatform.notifyListOfObservers(mLightResource.getHandle(), mObservationList, ocResourceResponse);
                } else {
                    // notify all observers if mObservationList is empty
                    OcPlatform.notifyAllObservers(mLightResource.getHandle());
                }
            } catch (OcException e) {
                Log.e(TAG, e.getMessage());
            }
            ++count;
        }
    }
}

/******************************************************************
*
* Copyright 2016 Samsung Electronics All Rights Reserved.
*
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/

package org.iotivity.service.es.test.helper;

import android.util.Log;

public class ESUtility {

    private static final String TIMEOUT_MESSAGE                = "Timeout for Enrollee response";
    private static final int    TIME_OUT                       = 10;
    private static final int    WAITING_TIME_IN_SECOND         = 1000;
    private static final String TAG                            = "Enrollee Response : ";
    private static final String RESPONSE_MESSAGE               = "\nWaiting For Enrollee Repsonse..........\n";
    public static final String  ES_EXCEPTION_SHOULD_BE_THROWN  = "ES Exception should be thrown";
    public static final String  EXCEPTION_SHOULD_BE_THROWN     = "Exception should be thrown";
    public static final String  EXCEPTION_SHOULD_NOT_BE_THROWN = "Exception should not be thrown";
    public final static String  EMPTY_STRING                   = "";
    public final static String  NULL_STRING                    = null;

    public static void waitUntilEnrolleeResponse(boolean callbackStatus) {
        int count = 0;
        while (!callbackStatus) {

            Log.d(TAG, RESPONSE_MESSAGE);

            if (count++ == TIME_OUT) {
                Log.d(TAG, TIMEOUT_MESSAGE);
                break;
            }

            try {
                Thread.sleep(WAITING_TIME_IN_SECOND);
            } catch (InterruptedException ex) {
                Log.d(TAG, ex.getMessage());
            }
        }
    }
}
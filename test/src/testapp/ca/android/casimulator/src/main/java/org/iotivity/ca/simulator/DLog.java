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

package org.iotivity.ca.simulator;

import android.os.Handler;
import android.widget.TextView;

public final class DLog {

    private final static String MAIN_TAG = "CASimulator : DLog";

    private static TextView mLogView = null;

    private static Handler mHandler = null;

    public static void setTextView(Handler handler, TextView logView) {
        mHandler = handler;
        mLogView = logView;
    }

    private static void addLogText(final String msg) {

        mHandler.post(new Runnable() {

            @Override
            public void run() {

                if (mLogView == null)
                    return;

                StringBuilder builder = new StringBuilder(mLogView.getText());
                // add front
                builder.append(msg + "\n");

                mLogView.setText(builder.toString());
            }

        });

    }

    public static final void v(String className, String msg) {
        android.util.Log.v(MAIN_TAG, className + "." + msg);

        if (mLogView != null) {
            addLogText(msg);
        }
    }

    public static final void d(String className, String msg) {
        android.util.Log.d(MAIN_TAG, className + "." + msg);

        if (mLogView != null) {
            addLogText(msg);
        }
    }

    public static final void i(String className, String msg) {
        android.util.Log.i(MAIN_TAG, className + "." + msg);

        if (mLogView != null) {
            addLogText(msg);
        }
    }

    public static final void w(String className, String msg) {
        android.util.Log.w(MAIN_TAG, className + "." + msg);

        if (mLogView != null) {
            addLogText(msg);
        }
    }

    public static final void w(String className, String msg, Exception ex) {
        android.util.Log.w(MAIN_TAG, className + "." + msg + ":" + ex.getMessage());

        if (mLogView != null) {
            addLogText(msg);
        }
    }

    public static final void w(String className, String msg, Error e) {
        android.util.Log.w(MAIN_TAG, className + "." + msg + ":" + e.getMessage());

        if (mLogView != null) {
            addLogText(msg);
        }
    }

}


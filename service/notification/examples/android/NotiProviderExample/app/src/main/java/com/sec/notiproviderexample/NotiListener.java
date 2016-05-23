/*
 *******************************************************************
 *
 * Copyright 2015 Intel Corporation.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

package com.sec.notiproviderexample;

import android.app.Notification;
import android.os.Bundle;
import android.service.notification.NotificationListenerService;
import android.service.notification.StatusBarNotification;
import android.util.Log;

public class NotiListener extends NotificationListenerService {

    private final String TAG = "NS_JNI_NOTI_LISTENER";
    private static ProviderProxy mProviderProxy = null;
    private MainActivity mActivity = null;

    public NotiListener() {

        Log.i(TAG, "Create NotiListener");
    }

    public NotiListener(MainActivity activity) {

        Log.i(TAG, "Create NotiListener with MainActivity");

        this.mActivity = activity;
        this.mProviderProxy = mActivity.getProviderExample();

        if(mProviderProxy == null) {
            Log.i(TAG, "Fail to get providerProxy instance");
        }
    }

    @Override
    public void onNotificationPosted(StatusBarNotification sbn) {
        super.onNotificationPosted(sbn);

        Bundle bundle = sbn.getNotification().extras;
        //Log.i("PACKAGENAME : ", sbn.getPackageName());

        if (sbn.getPackageName().equals("android"))
            return;

        String id = Integer.toString(sbn.getId());
        String title = bundle.getString(Notification.EXTRA_TITLE, "");
        String body = bundle.getString(Notification.EXTRA_TEXT, "");

        Log.i(TAG, "onNotificationPosted .. ");
        Log.i(TAG, "Id : " + id);
        Log.i(TAG, "Title : " + title);
        Log.i(TAG, "Body : " + body);

        if (mProviderProxy != null) {

            NotificationObject notiObject = new NotificationObject(id, title);
            notiObject.setContentText(body);
            mProviderProxy.sendNSMessage(id, title, body);

        } else {
            Log.i(TAG, "providerExample is NULL");
        }
    }

    @Override
    public void onNotificationRemoved(StatusBarNotification sbn) {
        super.onNotificationRemoved(sbn);

        Log.i(TAG, "onNotificationRemoved .. ");
    }
}

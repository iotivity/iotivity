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

import android.content.Context;
import android.util.Log;

import org.iotivity.base.ModeType;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcResourceHandle;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;
import org.iotivity.service.notification.IoTNotification;
import org.iotivity.service.notification.NSConsumer;
import org.iotivity.service.notification.NSMessage;

public class ProviderProxy {

    private static final String TAG = "NS_JNI_PROVIDER_PROXY";

    private Context mContext = null;
    private OcResourceHandle mResourceHandle;   //resource handle
    private IoTNotification ioTNotification = null;

    private final int SUCCESS = 200;

    public ProviderProxy(Context context) {
        Log.i(TAG, "Create providerProxy Instance");

        this.mContext = context;
        ioTNotification = new IoTNotification();
    }

    private void configurePlatform() {

        PlatformConfig platformConfig = new PlatformConfig(
                mContext,
                ServiceType.IN_PROC,
                ModeType.CLIENT_SERVER,
                "0.0.0.0", // By setting to "0.0.0.0", it binds to all available interfaces
                0,         // Uses randomly available port
                QualityOfService.LOW
        );

        Log.i(TAG, "Configuring platform.");
        OcPlatform.Configure(platformConfig);
        try {
            OcPlatform.stopPresence(); // Initialize OcPlatform
        } catch(Exception e) {
            Log.e(TAG, "Exception during sropPresence: " + e);
        }
        Log.i(TAG, "Configuration done Successfully");
    }

    public void startNotificationServer(boolean access)
    {
        configurePlatform();
        ioTNotification.NSStartProvider(access);
    }

    public void stopNotificationServer() {

        try {
            OcPlatform.stopPresence();
        } catch (Exception e) {
            Log.e(TAG, "Exception during stopPresence to terminate ns: " + e);
        }

        ioTNotification.NSStopProvider();
    }

    public void sendNSMessage(String id, String title, String body) {

        NSMessage notiMessage = new NSMessage(id);
        notiMessage.setTitle(title);
        notiMessage.setBody(body);
        ioTNotification.NSSendNotification(notiMessage);
    }

    public void readCheck(String messageId) {

        NSMessage notiMessage = new NSMessage(messageId);
        ioTNotification.NSProviderReadCheck(notiMessage);
    }

    public void accept(String consumerId, boolean accepted)
    {
        NSConsumer consumer = new NSConsumer(consumerId);
        ioTNotification.NSAccept(consumer, accepted);
    }
}

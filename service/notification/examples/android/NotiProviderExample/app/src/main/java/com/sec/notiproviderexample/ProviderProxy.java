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

import android.app.NotificationManager;
import android.content.Context;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.widget.Toast;

import org.iotivity.base.ModeType;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcResourceHandle;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;
import org.iotivity.service.ns.IoTNotification;
import org.iotivity.service.ns.NSConsumer;
import org.iotivity.service.ns.NSMessage;

import java.util.HashMap;

public class ProviderProxy
        implements IoTNotification.NSSubscriptionListner, IoTNotification.NSSynchListner{

    private static final String TAG = "NS_PROVIDER_PROXY";

    private Context mContext = null;
    private OcResourceHandle mResourceHandle;   //resource handle
    private IoTNotification ioTNotification = null;
    private HashMap<String, Integer> msgMap;

    private Handler mHandler = null;

    private static final int MESSAGE_SUBSCRIPTION = 1;
    private static final int MESSAGE_SYNC = 2;

    private static final int SYNC_READ = 0;
    private static final int SYNC_DISMISS = 1;
    private static final int SYNC_UNREAD = 2;

    public ProviderProxy(Context context) {
        Log.i(TAG, "Create providerProxy Instance");

        this.msgMap = new HashMap<>();
        this.mContext = context;
        ioTNotification = new IoTNotification();
    }

    public void setHandler(Handler handler)
    {
        this.mHandler = handler;
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
            Log.e(TAG, "Exception: stopping presence when configuration step: " + e);
        }
        Log.i(TAG, "Configuration done Successfully");
    }

    public void startNotificationServer(boolean access)
    {
        configurePlatform();
        ioTNotification.NSStartProvider(access, this, this);
    }

    public void stopNotificationServer() {

        try {
            OcPlatform.stopPresence();
        } catch (Exception e) {
            Log.e(TAG, "Exception: stopping presence when terminating NS server: " + e);
        }

        ioTNotification.NSStopProvider();
    }

    public void sendNSMessage(String id, String title, String body, String source) {

        NSMessage notiMessage = new NSMessage(id);
        notiMessage.setTitle(title);
        notiMessage.setBody(body);
        notiMessage.setSource(source);
        msgMap.put(id, SYNC_UNREAD);
        ioTNotification.NSSendNotification(notiMessage);

        mHandler.post(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(mContext, "Notification sent", Toast.LENGTH_SHORT).show();
            }
        });
    }

    public void readCheck(String messageId) {
        if(msgMap.containsKey(messageId)) {
            if(msgMap.get(messageId) == SYNC_UNREAD)
            {
                NSMessage notiMessage = new NSMessage(messageId);
                ioTNotification.NSProviderReadCheck(notiMessage);
                msgMap.put(messageId, SYNC_READ);
            }
        }
    }

    public void accept(String consumerId, boolean accepted)
    {
        NSConsumer consumer = new NSConsumer(consumerId);
        ioTNotification.NSAccept(consumer, accepted);
    }

    @Override
    public void OnNSSubscribedEvent(String consumerId) {
        Log.i(TAG, "OnNSSubscribedEvent");

        Log.i(TAG, "Consumer: " + consumerId);
        Message msg = mHandler.obtainMessage(MESSAGE_SUBSCRIPTION, consumerId);
        mHandler.sendMessage(msg);
    }

    @Override
    public void OnNSSynchronizedEvent(String messageId, int syncState) {
        Log.i(TAG, "OnNSSynchronizedEvent");

        Log.i(TAG, "Message Id: " + messageId);
        Log.i(TAG, "Sync state: " + syncState);

        Message msg = mHandler.obtainMessage(MESSAGE_SYNC, messageId + " / Sync State: " + syncState);
        mHandler.sendMessage(msg);

        NotificationManager manager = (NotificationManager)mContext
                .getSystemService(Context.NOTIFICATION_SERVICE);

        if(messageId != null)
            try
            {
                manager.cancel(Integer.valueOf(messageId));
            }
            catch (Exception e)
            {
                Log.e(TAG, "Handle exception for invalid message id" + e);
            }
        else
            Log.i(TAG, "message id is null");
    }

    public HashMap<String, Integer> getMsgMap() {
        return msgMap;
    }
}

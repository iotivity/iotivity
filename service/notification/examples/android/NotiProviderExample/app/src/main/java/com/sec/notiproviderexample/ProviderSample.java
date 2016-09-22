/*
//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
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
 */

package com.sec.notiproviderexample;

import android.content.Context;
import android.os.*;
import android.util.Log;
import android.widget.Toast;

import org.iotivity.base.ModeType;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcResourceHandle;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;
import org.iotivity.service.ns.common.Message;
import org.iotivity.service.ns.provider.*;
import org.iotivity.service.ns.common.*;


import java.util.HashMap;

public class ProviderSample
        implements ProviderService.OnConsumerSubscribedListener, ProviderService.OnMessageSynchronizedListener{

    private static final String TAG = "NS_PROVIDER_PROXY";

    private Context mContext = null;
    private OcResourceHandle mResourceHandle;   //resource handle
    private ProviderService ioTNotification = null;
    private HashMap<String, Integer> msgMap;

    private Handler mHandler = null;

    private static final int CONSUMER_SUBSCRIBED = 1;
    private static final int MESSAGE_SYNC = 2;

    private static final int SYNC_READ = 0;
    private static final int SYNC_DISMISS = 1;
    private static final int SYNC_UNREAD = 2;
    private boolean gAcceptor;
    private Consumer gConsumer;

    public ProviderSample(Context context) {
        Log.i(TAG, "Create providerSample Instance");

        this.msgMap = new HashMap<>();
        this.mContext = context;
        ioTNotification =  ProviderService.getInstance();
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

    public void Start(boolean policy)
    {
        Log.i(TAG, "Start ProviderService -IN");
        configurePlatform();
        gAcceptor = policy;
        try{
            int result =  ioTNotification.start(this, this, policy, "Info", false);
            Log.i(TAG, "Notification Start: " + result);
        }
        catch(Exception e){

        }

        Log.i(TAG, "Start ProviderService - OUT");
    }

    public void RegisterTopic()
    {
        Log.i(TAG, "Register Topics -IN");
        try{
            int result =  ioTNotification.registerTopic("OCF_TOPIC1");
            Log.i(TAG, " RegisterTopic: " + result);
            result =  ioTNotification.registerTopic("OCF_TOPIC2");
            Log.i(TAG, " RegisterTopic: " + result);
            result =  ioTNotification.registerTopic("OCF_TOPIC3");
            Log.i(TAG, " RegisterTopic: " + result);
            result =  ioTNotification.registerTopic("OCF_TOPIC4");
            Log.i(TAG, " RegisterTopic: " + result);
        }
        catch(Exception e){

        }

        Log.i(TAG, "Start ProviderService - OUT");
    }

    public int SetTopic()
    {
        Log.i(TAG, "Set Topic -IN");
        if(gConsumer == null){
            return 0;
        }
        try{
            int result =  gConsumer.setTopic("OCF_TOPIC1");
            Log.i(TAG, " Set Topic : " + result);
            result =  gConsumer.setTopic("OCF_TOPIC2");
            Log.i(TAG, " Set Topic : " + result);
            result =  gConsumer.setTopic("OCF_TOPIC3");
            Log.i(TAG, " Set Topic : " + result);
            result =  gConsumer.setTopic("OCF_TOPIC4");
            Log.i(TAG, " Set Topic : " + result);
        }
        catch(Exception e){

        }

        Log.i(TAG, "Start ProviderService - OUT");
        return 1;
    }

    public void Stop() {
        Log.i(TAG, "Stop ProviderService - IN");
        try {
            OcPlatform.stopPresence();
        } catch (Exception e) {
            Log.e(TAG, "Exception: stopping presence when terminating NS server: " + e);
        }
        try{
            int result =  ioTNotification.stop();
            Log.i(TAG, "Notification Stop: " + result);
        }
        catch(Exception e){

        }

        Log.i(TAG, "Stop ProviderService - OUT");
    }


    public void SendMessage(Message notiMessage) {
        Log.i(TAG, "SendMessage ProviderService - IN");

        try{
            int result =  ioTNotification.sendMessage(notiMessage);
            Log.i(TAG, "Notification Send Message: " + result);
        }
        catch(Exception e){

        }

        Log.i(TAG, "SendMessage ProviderService - OUT");
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(mContext, "Notification sent", Toast.LENGTH_SHORT).show();
            }
        });
    }

    public void SendSyncInfo(long messageId, SyncInfo.SyncType syncType) {
        Log.i(TAG, "SendSyncInfo ProviderService - IN");
        if(msgMap.containsKey(messageId)) {
            if(msgMap.get(messageId) == SYNC_UNREAD)
            {
                try{
                    ioTNotification.sendSyncInfo(messageId, syncType);
                    Log.i(TAG, "Notification Sync " );
                }
                catch(Exception e) {

                }
                Log.i(TAG, "SendSyncInfo ProviderService - OUT");
                msgMap.put("ID: "+messageId, SYNC_READ);
            }
        }
    }

    public void EnableRemoteService(String servAdd) {
        Log.i(TAG, "EnableRemoteService ProviderService - IN");
        try{
            int result = ioTNotification.enableRemoteService(servAdd);
            Log.i(TAG, "Notification EnableRemoteService: "+ result );
        }
        catch(Exception e) {

        }
        Log.i(TAG, "EnableRemoteService ProviderService - OUT");
    }

    public void DisableRemoteService(String servAdd) {
        Log.i(TAG, "DisableRemoteService ProviderService - IN");
        try{
            int result = ioTNotification.disableRemoteService(servAdd);
            Log.i(TAG, "Notification DisableRemoteService: "+ result );
        }
        catch(Exception e) {

        }
        Log.i(TAG, "DisableRemoteService ProviderService - OUT");
    }

    public void AcceptSubscription(Consumer consumer, boolean accepted)
    {
        Log.i(TAG,"AcceptSubscription ProviderService - IN");
        try{
            int result = consumer.acceptSubscription(accepted);
            Log.i(TAG, "Notification AcceptSubscription: "+result );
        }
        catch(Exception e) {

        }
        Log.i(TAG, "AcceptSubscription ProviderService - OUT");
    }

    @Override
    public void onConsumerSubscribed(Consumer consumer) {
        Log.i(TAG, "onConsumerSubscribed - IN");
        gConsumer = consumer;
        AcceptSubscription(consumer, true);
        android.os.Message msg = mHandler.obtainMessage(CONSUMER_SUBSCRIBED,
                "Consumer Id: " + consumer.getConsumerId()  );
        mHandler.sendMessage(msg);
        Log.i(TAG, "onConsumerSubscribed - OUT");
    }

    @Override
    public void onMessageSynchronized(SyncInfo syncInfo) {
        Log.i(TAG, "Received SyncInfo with messageID: "+syncInfo.getMessageId());
        android.os.Message msg = mHandler.obtainMessage(MESSAGE_SYNC,
                "Message Id: " + syncInfo.getMessageId()  );
        mHandler.sendMessage(msg);
    }

    public HashMap<String, Integer> getMsgMap() {
        return msgMap;
    }
}

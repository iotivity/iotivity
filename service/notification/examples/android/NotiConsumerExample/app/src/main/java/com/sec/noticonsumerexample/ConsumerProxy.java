/******************************************************************
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 * <p>
 * <p>
 * <p>
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * <p>
 * http://www.apache.org/licenses/LICENSE-2.0
 * <p>
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************/

package com.sec.noticonsumerexample;

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
import org.iotivity.service.ns.common.SyncInfo;
import org.iotivity.service.ns.consumer.ConsumerService;
import org.iotivity.service.ns.consumer.Provider;

import java.util.HashMap;

public class ConsumerProxy
    implements ConsumerService.OnProviderDiscoveredListner,
    ConsumerService.OnSubscriptionAcceptedListener,
    Provider.OnMessageReceivedListner, Provider.OnSyncInfoReceivedListner
{

    private static final String TAG = "NS_CONSUMER_PROXY";

    private Context mContext = null;
    private ConsumerService consumerService = null;

    private Handler mHandler = null;

    private static final int PROVIDER_DISCOVERED = 1;
    private static final int SUBSCRIPTION_ACCEPTED = 2;
    private static final int MESSAGE_RECEIVED = 3;
    private static final int SYNCINFO_RECEIVED = 4;

    public ConsumerProxy(Context context)
    {
        Log.i(TAG, "Create consumerProxy Instance");

        this.mContext = context;
        consumerService = new ConsumerService();
    }

    public void setHandler(Handler handler)
    {
        this.mHandler = handler;
    }

    private void configurePlatform()
    {

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
        try
        {
            OcPlatform.stopPresence(); // Initialize OcPlatform
        }
        catch (Exception e)
        {
            Log.e(TAG, "Exception: stopping presence when configuration step: " + e);
        }
        Log.i(TAG, "Configuration done Successfully");
    }

    public void startNotificationConsumer()
    {
        configurePlatform();
        try
        {
            consumerService.Start(this, this);
        }
        catch (Exception e)
        {
            Log.e(TAG, "Exception: startNotificationConsumer : " + e);
        }
    }

    public void stopNotificationConsumer()
    {
        try
        {
            consumerService.Stop();
        }
        catch (Exception e)
        {
            Log.e(TAG, "Exception: stopNotificationConsumer : " + e);
        }
    }

    public void enableRemoteService(String serverAddress)
    {
        try
        {
            consumerService.EnableRemoteService(serverAddress);
        }
        catch (Exception e)
        {
            Log.e(TAG, "Exception: enableRemoteService : " + e);
        }
    }

    public void rescanProvider()
    {
        try
        {
            consumerService.RescanProvider();
        }
        catch (Exception e)
        {
            Log.e(TAG, "Exception: rescanProvider : " + e);
        }
    }

    public Provider getProvider(String providerId)
    {
        Provider provider = null;
        try
        {
            provider = consumerService.GetProvider(providerId);
        }
        catch (Exception e)
        {
            Log.e(TAG, "Exception: getProvider : " + e);
        }
        return provider;
    }

    public void subscribe(String providerId)
    {
        try
        {
            Provider provider = getProvider(providerId);
            if (provider != null)
                provider.Subscribe();
            else
                Log.e(TAG, "providerID is Null : ");
        }
        catch (Exception e)
        {
            Log.e(TAG, "Exception: Subscribe : " + e);
        }
    }

    public void unsubscribe(String providerId)
    {
        try
        {
            Provider provider = getProvider(providerId);
            if (provider != null)
                provider.Unsubscribe();
            else
                Log.e(TAG, "providerID is Null : ");
        }
        catch (Exception e)
        {
            Log.e(TAG, "Exception: Unsubscribe : " + e);
        }
    }

    public void sendSyncInfo(String providerId, long messageId, SyncInfo.SyncType syncType)
    {
        try
        {
            Provider provider = getProvider(providerId);
            if (provider != null)
                provider.SendSyncInfo(messageId, syncType);
            else
                Log.e(TAG, "providerID is Null : ");
        }
        catch (Exception e)
        {
            Log.e(TAG, "Exception: SendSyncInfo : " + e);
        }
    }

    public void setListener(String providerId)
    {
        try
        {
            Provider provider = getProvider(providerId);
            if (provider != null)
                provider.SetListener(this, this);
            else
                Log.e(TAG, "providerID is Null : ");
        }
        catch (Exception e)
        {
            Log.e(TAG, "Exception: SetListener : " + e);
        }
    }

    @Override
    public void onProviderDiscovered(Provider provider)
    {
        Log.i(TAG, "onProviderDiscovered");

        Log.i(TAG, "Provider ID: " + provider.getProviderId() );
        Message msg = mHandler.obtainMessage(PROVIDER_DISCOVERED, provider.getProviderId());
        mHandler.sendMessage(msg);
        subscribe(provider.getProviderId());
    }

    @Override
    public void onSubscriptionAccepted(Provider provider)
    {
        Log.i(TAG, "onSubscriptionAccepted");

        Log.i(TAG, "Provider ID: " + provider.getProviderId() );
        Message msg = mHandler.obtainMessage(SUBSCRIPTION_ACCEPTED, provider.getProviderId());
        mHandler.sendMessage(msg);
        setListener(provider.getProviderId());
    }

    @Override
    public void onMessageReceived(org.iotivity.service.ns.common.Message message)
    {
        Log.i(TAG, "onMessageReceived");

        Log.i(TAG, "Message Id: " + message.getMessageId());
        Log.i(TAG, "Message title: " + message.getTitle());
        Log.i(TAG, "Message Content: " + message.getContentText());
        Log.i(TAG, "Message Source: " + message.getSourceName());

        Message msg = mHandler.obtainMessage(MESSAGE_RECEIVED,
                                             "Message Id: " + message.getMessageId() +
                                             " / Message title: " + message.getTitle() +
                                             " / Message Content: " + message.getContentText() +
                                             " / Message Source: " + message.getSourceName() );
        mHandler.sendMessage(msg);
    }

    @Override
    public void onSyncInfoReceived(SyncInfo sync)
    {
        Log.i(TAG, "onSyncInfoReceived");

        Log.i(TAG, "Sync Id: " + sync.getMessageId());
        Log.i(TAG, "Sync STATE: " + sync.getState());
        Message msg = mHandler.obtainMessage(SYNCINFO_RECEIVED,
                                             "Sync Id: " + sync.getMessageId() +
                                             " / Sync STATE: " + sync.getState());
        mHandler.sendMessage(msg);
    }
}

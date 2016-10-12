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

package org.iotivity.service.ns.consumer;

import android.util.Log;
import org.iotivity.service.ns.common.*;
import java.util.Vector;

/**
  * @class   Provider
  * @brief   This class provides implementation of Notification Provider object.
  */
public class Provider
{
    private static final String LOG_TAG = "ConsumerService_Provider";

    /**
      * ProviderState of Notification resource
      */
    public enum ProviderState
    {
        ALLOW(1),
        DENY(2),
        TOPIC(3),
        STOPPED(12);
        private int state;

        private ProviderState(int state)
        {
            this.state = state;
        }

        public int getProviderState()
        {
            return this.state;
        }
    };

    public String mProviderId        = null;
    private long mNativeHandle       = 0;

    /**
      * Constructor of Provider.
      * @param providerId - providerId of Provider.
      */
    public Provider(String providerId)
    {
        Log.i (LOG_TAG, "Provider()");

        mProviderId = providerId;
    }

    /**
      * API for getting providerId.
      * @return ConsumerId as string.
      */
    public String getProviderId()
    {
        return mProviderId ;
    }

    /**
      * API for getting for getting Topic List.
      * @return TopicsList.
      */
    public TopicsList getTopicList() throws NSException
    {
        return nativeGetTopicList();
    }

    /**
      * API for getting for getting ProviderState.
      * @return ProviderState.
      */
    public ProviderState getProviderState() throws NSException
    {
        return nativeGetProviderState();
    }

    /**
      * API for for requesting subscription of Notification service.
      */
    public void subscribe() throws NSException
    {
        nativeSubscribe();
    }

    /**
      * API for for requesting subscription status from Provider of Notification service.
      */
    public boolean isSubscribed () throws NSException
    {
        return nativeIsSubscribed();
    }

    /**
      * This method is for Sending SyncInfo of Notification service.
      * @param messageId - id of  message.
      * @param syncType - SyncType of Notification service.
      */
    public void sendSyncInfo(long messageId, SyncInfo.SyncType syncType) throws NSException
    {
        nativeSendSyncInfo(messageId, syncType.ordinal());
    }

    /**
      * This method is for registering for listeners of Notification .
      * @param onProviderStateListener - OnProviderStateListener callback Interface.
      * @param onMessageReceivedListner - OnMessageReceivedListner callback Interface.
      * @param onSyncInfoReceivedListner - OnSyncInfoReceivedListner callback Interface.
      */
    public void setListener(OnProviderStateListener onProviderStateListener,
                            OnMessageReceivedListner onMessageReceivedListner,
                            OnSyncInfoReceivedListner onSyncInfoReceivedListner) throws NSException
    {
        nativeSetListener(onProviderStateListener, onMessageReceivedListner, onSyncInfoReceivedListner);
    }

    /**
      * Update Topic list that is wanted to be subscribed from provider
      * @param topicsList - TopicsList of interested Topics.
      * @return :: result code  100 = OK , 200 = ERROR , 300 = SUCCESS , 400 = FAIL
      */
    public int updateTopicList(TopicsList topicsList) throws NSException
    {
        return nativeUpdateTopicList(topicsList);
    }

    /**
      * Interface to implement callback function to receive provider state information
      */
    public interface OnProviderStateListener
    {
        /**
          * Callback function to receive provider state information
          * @param state - ProviderState
          */
        public void onProviderStateReceived(ProviderState state);
    }

    /**
      * Interface to implement callback function to receive Notification Message
      */
    public interface OnMessageReceivedListner
    {
        /**
          * Callback function to receive Notification Message
          * @param message - Notification Message
          */
        public void onMessageReceived(Message message);
    }

    /**
      * Interface to implement callback function to receive message read synchronization
      */
    public interface OnSyncInfoReceivedListner
    {
        /**
          * Callback function to receive message read synchronization
          * @param sync - SyncInfo
          */
        public void onSyncInfoReceived(SyncInfo sync);
    }

    private native void nativeSubscribe() throws NSException;
    private native void nativeSendSyncInfo(long messageId, int syncType) throws NSException;
    private native void nativeSetListener(OnProviderStateListener onProviderStateListener,
                                      OnMessageReceivedListner onMessageReceivedListner,
                                      OnSyncInfoReceivedListner onSyncInfoReceivedListner) throws NSException;
    public native TopicsList  nativeGetTopicList() throws NSException;
    private native int nativeUpdateTopicList(TopicsList topicsList) throws NSException;
    private native ProviderState nativeGetProviderState() throws NSException;
    public native boolean  nativeIsSubscribed() throws NSException;

}

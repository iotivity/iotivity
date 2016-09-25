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

    public Provider(String providerId)
    {
        Log.i (LOG_TAG, "Provider()");

        mProviderId = providerId;
    }

    public String getProviderId()
    {
        return mProviderId ;
    }

    public TopicsList getTopicList() throws NSException
    {
        return nativeGetTopicList();
    }

    public ProviderState getProviderState() throws NSException
    {
        return nativeGetProviderState();
    }

    public void subscribe() throws NSException
    {
        nativeSubscribe();
    }

    public boolean isSubscribed () throws NSException
    {
        return nativeIsSubscribed();
    }

    public void sendSyncInfo(long messageId, SyncInfo.SyncType syncType) throws NSException
    {
        nativeSendSyncInfo(messageId, syncType.ordinal());
    }

    public void setListener(OnProviderStateListener onProviderStateListener,
                            OnMessageReceivedListner onMessageReceivedListner,
                            OnSyncInfoReceivedListner onSyncInfoReceivedListner) throws NSException
    {
        nativeSetListener(onProviderStateListener, onMessageReceivedListner, onSyncInfoReceivedListner);
    }

    public int updateTopicList(TopicsList topicsList) throws NSException
    {
        return nativeUpdateTopicList(topicsList);
    }

    public interface OnProviderStateListener
    {
        public void onProviderStateReceived(ProviderState state);
    }

    public interface OnMessageReceivedListner
    {
        public void onMessageReceived(Message message);
    }

    public interface OnSyncInfoReceivedListner
    {
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

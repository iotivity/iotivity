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

    public String mProviderId        = null;
    TopicsList mTopicsList = new TopicsList();
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

    public TopicsList  getTopicsList()
    {
        return mTopicsList ;
    }

    public void Subscribe() throws NSException
    {
        nativeSubscribe();
    }

    public void Unsubscribe() throws NSException
    {
        nativeUnsubscribe();
    }

    public void SendSyncInfo(long messageId, SyncInfo.SyncType syncType) throws NSException
    {
        nativeSendSyncInfo(messageId, syncType.ordinal());
    }

    public void SetListener(OnMessageReceivedListner onMessageReceivedListner,
                            OnSyncInfoReceivedListner onSyncInfoReceivedListner) throws NSException
    {
        nativeSetListener(onMessageReceivedListner, onSyncInfoReceivedListner);
    }

    public int SelectInterestTopics(Vector<Topic> topicsList) throws NSException
    {
        return nativeSelectInterestTopics(topicsList);
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
    private native void nativeUnsubscribe() throws NSException;
    private native void nativeSendSyncInfo(long messageId, int syncType) throws NSException;
    private native void nativeSetListener(
        OnMessageReceivedListner onMessageReceivedListner,
        OnSyncInfoReceivedListner onSyncInfoReceivedListner
    ) throws NSException;
    private native int nativeSelectInterestTopics(Vector<Topic> topicsList) throws NSException;
}

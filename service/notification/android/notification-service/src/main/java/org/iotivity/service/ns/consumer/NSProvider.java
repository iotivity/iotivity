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

public class NSProvider {

    private static final String LOG_TAG = "ConsumerService_NSProvider";

    String providerId                = null;

    private long mNativeHandle       = 0;

    public NSProvider(String providerId, long mNativeHandle) {
        Log.i (LOG_TAG, "NSProvider()");

        this.providerId = providerId;

        this.mNativeHandle = mNativeHandle;
    }

    private native void Subscribe(long provider) throws NSException;
    private native void Unsubscribe(long provider) throws NSException;
    private native void SendSyncInfo(String providerId, long messageId, int syncType) throws NSException;
    public native void SetListener(
        ConsumerService.OnMessageReceivedListner onMessageReceivedListner,
        ConsumerService.OnSyncInfoReceivedListner onSyncInfoReceivedListner
        ) throws NSException;

    public String getProviderId () {
        return providerId ;
    }

    public void Subscribe() throws NSException {
        this.Subscribe(mNativeHandle);
    }

    public void Unsubscribe() throws NSException {
        this.Unsubscribe(mNativeHandle);
    }

    public void SendSyncInfo(long messageId, int syncType) throws NSException {
        this.SendSyncInfo(this.providerId, messageId, syncType);
    }
}
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

public class ConsumerService {

    private static final String LOG_TAG = "ConsumerService";

    static {
        System.loadLibrary("notification_consumer_jni");
    }

    public ConsumerService() {
        Log.i (LOG_TAG, "ConsumerService()");
    }

    public native void Start (
            OnProviderDiscoveredListner onProviderDiscoveredListner,
            OnSubscriptionAcceptedListener onSubscriptionAcceptedListener
            ) throws NSException;
    public native void Stop () throws NSException;
    public native void RescanProvider() throws NSException;
    public native NSProvider GetProvider(String providerId) throws NSException;

    public interface OnProviderDiscoveredListner {
        public void onProviderDiscovered(NSProvider provider);
    }

    public interface OnSubscriptionAcceptedListener {
        public void onSubscriptionAccepted(NSProvider provider);
    }

    public interface OnMessageReceivedListner {
        public void onMessageReceived(NSMessage message);
    }

    public interface OnSyncInfoReceivedListner {
        public void onSyncInfoReceived(NSSyncInfo sync);
    }
}


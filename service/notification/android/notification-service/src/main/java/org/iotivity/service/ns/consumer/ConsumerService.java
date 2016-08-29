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
  * @class   ConsumerService
  * @brief   This class provides a set of Java APIs for Notification Consumer.
  */
public class ConsumerService
{
    private static final String LOG_TAG = "ConsumerService";

    static
    {
        System.loadLibrary("gnustl_shared");
        System.loadLibrary("oc_logger");
        System.loadLibrary("connectivity_abstraction");
        System.loadLibrary("ca-interface");
        System.loadLibrary("octbstack");
        System.loadLibrary("oc");
        System.loadLibrary("ocstack-jni");
        System.loadLibrary("notification_consumer");
        System.loadLibrary("notification_consumer_wrapper");
        System.loadLibrary("notification_consumer_jni");
    }

    public enum Response
    {
        ALLOW(1),
        DENY(2),
        TOPIC(3);
        private int type;

        private Response(int type)
        {
            this.type = type;
        }

        public int getResponseType()
        {
            return this.type;
        }
    };

    private static ConsumerService instance;
    static
    {
        instance = new ConsumerService();
    }
    public static ConsumerService getInstance()
    {
        return instance;
    }

    public void Start(
        OnProviderDiscoveredListner onProviderDiscoveredListner,
        OnProviderChangedListener onProviderChangedListener
    ) throws NSException
    {
        nativeStart(onProviderDiscoveredListner, onProviderChangedListener);
    }

    public void Stop() throws NSException
    {
        nativeStop();
    }

    public void EnableRemoteService(String serverAddress) throws NSException
    {
        nativeEnableRemoteService(serverAddress);
    }

    public void RescanProvider() throws NSException
    {
        nativeRescanProvider();
    }

    public Provider GetProvider(String providerId) throws NSException
    {
        return nativeGetProvider(providerId);
    }

    public Message GetMessage(long  messageId) throws NSException
    {
        return nativeGetMessage(messageId);
    }

    public interface OnProviderDiscoveredListner
    {
        public void onProviderDiscovered(Provider provider);
    }

    public interface OnProviderChangedListener
    {
        public void onProviderChanged(Provider provider , Response response);
    }

    private native void nativeStart (
        OnProviderDiscoveredListner onProviderDiscoveredListner,
        OnProviderChangedListener onProviderChangedListener
    ) throws NSException;

    private native void nativeStop() throws NSException;
    private native void nativeEnableRemoteService(String serverAddress) throws NSException;
    private native void nativeRescanProvider() throws NSException;
    private native Provider nativeGetProvider(String providerId) throws NSException;
    private native Message nativeGetMessage(long messageId) throws NSException;
}

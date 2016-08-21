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
package org.iotivity.service.ns.provider;
import org.iotivity.service.ns.common.*;
/**
  * @class   ProviderService
  * @brief   This class provides a set of Java APIs for Notification ProviderService.
  */
public class ProviderService
{

    static
    {
        System.loadLibrary("gnustl_shared");
        System.loadLibrary("oc_logger");
        System.loadLibrary("connectivity_abstraction");
        System.loadLibrary("ca-interface");
        System.loadLibrary("octbstack");
        System.loadLibrary("oc");
        System.loadLibrary("ocstack-jni");
        System.loadLibrary("notification_provider");
        System.loadLibrary("notification_provider_wrapper");
        System.loadLibrary("notification_provider_jni");
    }
    public native int  nativeStart(boolean policy,
                                   OnSubscriptionListener   subscriptionListener,
                                   OnSyncInfoListener   syncInfoListener);
    public native int  nativeStop();
    public native int  nativeSendMessage(Message message);
    public native void  nativeSendSyncInfo( long messageId , int type);
    public native int  nativeEnableRemoteService(String servAdd);
    public native int  nativeDisableRemoteService(String servAdd);

    private static ProviderService instance;

    static
    {
        instance = new ProviderService();
    }
    public interface OnSubscriptionListener
    {
        public void onConsumerSubscribed(Consumer consumer);
    }
    public interface OnSyncInfoListener
    {
        public void onMessageSynchronized(SyncInfo syncInfo);
    }

    public static ProviderService getInstance()
    {
        return instance;
    }
    public int Start(boolean policy,
                     OnSubscriptionListener  subscriptionListener,
                     OnSyncInfoListener  syncInfoListener) throws NSException
    {

    int result = nativeStart(policy, subscriptionListener, syncInfoListener);
        return result;
    }

    public int Stop() throws NSException
    {
        int result = nativeStop();
        return result;
    }

    public int   SendMessage(Message message) throws NSException
    {
        int result = nativeSendMessage(message);
        return result;
    }

    public void SendSyncInfo ( long messageId , SyncInfo.SyncType syncType) throws NSException
    {
        nativeSendSyncInfo(messageId, syncType.ordinal());
        return ;
    }

    public int   EnableRemoteService(String servAdd) throws NSException
    {
        int result = nativeEnableRemoteService(servAdd);
        return result;
    }

    public int  DisableRemoteService(String servAdd) throws NSException
    {
        int result = nativeDisableRemoteService(servAdd);
        return result;
    }

}

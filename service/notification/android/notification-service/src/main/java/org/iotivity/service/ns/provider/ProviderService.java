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
import java.util.Vector;
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

    private static ProviderService instance;

    static
    {
        instance = new ProviderService();
    }

    public static ProviderService getInstance()
    {
        return instance;
    }

    public int start(OnConsumerSubscribedListener  subscribedListener,
                     OnMessageSynchronizedListener  messageSynchronized,
                     boolean subControllability, String userInfo, 
                     boolean resourceSecurity) throws NSException
    {
        return nativeStart(subscribedListener, messageSynchronized,
                            subControllability, userInfo, resourceSecurity);
    }

    public int stop() throws NSException
    {
        return nativeStop();
    }

    public int   sendMessage(Message message) throws NSException
    {
        return nativeSendMessage(message);
    }

    public void sendSyncInfo ( long messageId , SyncInfo.SyncType syncType) throws NSException
    {
        nativeSendSyncInfo(messageId, syncType.ordinal());
    }

    public Message createMessage () throws NSException
    {
        return nativeCreateMessage();
    }

    public int   enableRemoteService(String servAdd) throws NSException
    {
        return nativeEnableRemoteService(servAdd);
    }

    public int  disableRemoteService(String servAdd) throws NSException
    {
        return nativeDisableRemoteService(servAdd);
    }

    public int registerTopic(String topicName) throws NSException
    {
        return nativeRegisterTopic(topicName);
    }

    public int unregisterTopic(String topicName) throws NSException
    {
        return nativeUnregisterTopic(topicName);
    }

    public TopicsList getRegisteredTopicList() throws NSException
    {
        return nativeGetRegisteredTopicList();
    }

    public interface OnConsumerSubscribedListener
    {
        public void onConsumerSubscribed(Consumer consumer);
    }

    public interface OnMessageSynchronizedListener
    {
        public void onMessageSynchronized(SyncInfo syncInfo);
    }

    public native int  nativeStart(OnConsumerSubscribedListener subscribedListener,
                                                 OnMessageSynchronizedListener messageSynchronized,
                                                 boolean subControllability, String userInfo, 
                                                 boolean resourceSecurity) throws NSException;
    public native int  nativeStop() throws NSException;
    public native int  nativeSendMessage(Message message) throws NSException;
    public native void  nativeSendSyncInfo( long messageId , int type) throws NSException;
    public native Message nativeCreateMessage() throws NSException;
    public native int  nativeEnableRemoteService(String servAdd) throws NSException;
    public native int  nativeDisableRemoteService(String servAdd) throws NSException;
    public native int  nativeRegisterTopic(String topicName) throws NSException;
    public native int  nativeUnregisterTopic(String topicName) throws NSException;
    public native TopicsList  nativeGetRegisteredTopicList() throws NSException;
}

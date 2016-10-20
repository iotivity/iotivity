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

    /**
      * API for getting instance of ProviderService
      * @return ProviderService singleton instance created
      */
    public static ProviderService getInstance()
    {
        return instance;
    }

    /**
      * Start ProviderService
      * @param subscribedListener - OnConsumerSubscribedListener Callback
      * @param messageSynchronized - OnMessageSynchronizedListener Callback
      * @param subControllability - Set the policy for notification servcie which checks whether
      * provider is capable of denying the subscription of notification message from consumer and
      * getting controllabliity to set consumer topic list.
      * If true, provider is able to control subscription request and consumer topic list.
      * Otherwise(policy is false), consumer can do the same.
      * @param userInfo -  User defined information such as device friendly name
      * @param resourceSecurity -  Set on/off for secure resource channel setting
      * @return :: result code  100 = OK , 200 = ERROR , 300 = SUCCESS , 400 = FAIL
      */
    public int start(OnConsumerSubscribedListener  subscribedListener,
                     OnMessageSynchronizedListener  messageSynchronized,
                     boolean subControllability, String userInfo,
                     boolean resourceSecurity) throws NSException
    {
        return nativeStart(subscribedListener, messageSynchronized,
                            subControllability, userInfo, resourceSecurity);
    }

    /**
      * Stop ProviderService
      * @return :: result code
      */
    public int stop() throws NSException
    {
        return nativeStop();
    }

    /**
      * Send notification message to all subscribers
      * @param  message - Notification message including id, title, contentText
      * @return :: result code
      */
    public int   sendMessage(Message message) throws NSException
    {
        return nativeSendMessage(message);
    }

    /**
      * Send read-check to provider in order to synchronize notification status with other consumers
      * @param  messageId -  ID of Notification message to synchronize the status
      * @param  syncType - SyncType of the SyncInfo message
      */
    public void sendSyncInfo ( long messageId , SyncInfo.SyncType syncType) throws NSException
    {
        nativeSendSyncInfo(messageId, syncType.ordinal());
    }

    /**
      * Initialize Message class, Mandatory fields which are messge id and provider(device) id are filled with.
      * @return Message
      */
    public Message createMessage () throws NSException
    {
        return nativeCreateMessage();
    }

    /**
      * Request to publish resource to cloud server
      * @param[in]  servAdd combined with IP address and port number using delimiter :
      * @return  result code
      */
    public int   enableRemoteService(String servAdd) throws NSException
    {
        return nativeEnableRemoteService(servAdd);
    }

    /**
      * Request to cancel remote service using cloud server
      * @param[in]  servAdd combined with IP address and port number using delimiter :
      * @return  result code
      */
    public int  disableRemoteService(String servAdd) throws NSException
    {
        return nativeDisableRemoteService(servAdd);
    }

    /**
      * Add topic to topic list
      * @param  topicName - Topic name to add
      * @return :: result code
      */
    public int registerTopic(String topicName) throws NSException
    {
        return nativeRegisterTopic(topicName);
    }

    /**
      * Delete topic from topic list
      * @param  topicName - Topic name to add
      * @return :: result code
      */
    public int unregisterTopic(String topicName) throws NSException
    {
        return nativeUnregisterTopic(topicName);
    }

    /**
      * Request topics list already registered by provider user
      * @return :: Topic list
      */
    public TopicsList getRegisteredTopicList() throws NSException
    {
        return nativeGetRegisteredTopicList();
    }

    /**
      * Interface to implement callback function to receive subscription request of consumer
      */
    public interface OnConsumerSubscribedListener
    {

        /**
          * Callback function to receive subscription request of consumer
          * @param consumer - Consumer who subscribes the notification message resource
          */
        public void onConsumerSubscribed(Consumer consumer);
    }

    /**
      * Interface to implement callback function to receive the status of the message synchronization
      */
    public interface OnMessageSynchronizedListener
    {

        /**
          * Callback function to receive the status of the message synchronization
          * @param syncInfo - Synchronization information of the notification message
          */
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

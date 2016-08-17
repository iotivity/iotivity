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

/**
 * @file
 *
 * This file provides APIs of Notification Service for Provider.
 */

#ifndef _NS_PROVIDER_INTERFACE_H_
#define _NS_PROVIDER_INTERFACE_H_

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#include "NSCommon.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * Provider uses this callback function to receive subscription request of consumer
 * @param[in] consumer        Consumer who subscribes the resource
 */
typedef void (*NSSubscribeRequestCallback)(NSConsumer *);

/**
 * Provider use this callback function to receive the status of the message
 * synchronization
 * @param[in] sync        Synchronization information of the notification message
 */
typedef void (*NSProviderSyncInfoCallback)(NSSyncInfo *);

/**
 *  Set provider service with the following configuration
 */
typedef struct
{
    /* Invoked when the subscription request from consumer is received */
    NSSubscribeRequestCallback subRequestCallback;
    /* Invoked when the synchronization data, read and deleted, is sent by consumer is received */
    NSProviderSyncInfoCallback syncInfoCallback;
    /* Set the policy for notification servcie refering to following
     * if policy is true, provider decides to allow or deny for all the subscribing consumers.
     * Otherwise(policy is false) consumer decides to request subscription to discovered providers.
     */
    bool policy;
    /* User Information */
    char * userInfo;

} NSProviderConfig;

/**
 * Initialize notification service for provider service
 * @param[in]  config   Refer to NSProviderConfig
 * @return ::NS_OK or result code of NSResult
 */
NSResult NSStartProvider(NSProviderConfig config);

/**
 * Terminate notification service for provider
 * @return ::NS_OK or result code of NSResult
 */
NSResult NSStopProvider();

/**
 * Request to publish resource to cloud server
 * @param[in]  server address combined with IP address and port number using delimiter :
 * @return ::NS_OK or result code of NSResult
 */
NSResult NSProviderEnableRemoteService(char *serverAddress);

/**
 * Request to cancel remote service using cloud server
 * @param[in]  server address combined with IP address and port number using delimiter :
 * @return ::NS_OK or result code of NSResult
 */
NSResult NSProviderDisableRemoteService(char *serverAddress);

/**
 * Send notification message to all subscribers
 * @param[in]  message  Notification message including id, title, contentText
 * @return ::NS_OK or result code of NSResult
 */
NSResult NSSendMessage(NSMessage *msg);

/**
 * Send acceptance to consumer who subscribes the resource of notification message
 * @param[in]  consumer  Consumer who subscribes the resource
 * @param[in]  accepted  the result of acceptance; Allow or Deny
 * @return ::NS_OK or result code of NSResult
 */
NSResult NSAcceptSubscription(NSConsumer *consumer, bool accepted);

/**
 * Get consumer list that is stored in the cache of notification service
 * @param[in]  list  Consumer list
 * @param[in]  size  the number of consumers stored in the cache
 * @return ::NS_OK or result code of NSResult
 */
//TODO will use Function.
// NSResult NSGetConsumerList(uint8_t *list, uint32_t size);

/**
 * Send read-check to provider in order to synchronize notification status with other consumers
 * @param[in]  message  Notification message to synchronize the status
 * @return ::NS_OK or result code of NSResult
 */
NSResult NSProviderSendSyncInfo(uint64_t messageId, NSSyncType type);

/**
 * Initialize NSMessage struct, our service set message id and provider(device) id
 * @return ::NSMessage *
 */
NSMessage * NSCreateMessage();

/**
 * Add topic to topic list which is located in provider service storage
 * @param[in]  topicName Topic name to add
 * @return ::NS_OK or result code of NSResult
 */
NSResult NSProviderAddTopic(char* topicName);

/**
 * Delete topic from topic list
 * @param[in]  topicName Topic name to delete
 * @return ::NS_OK or result code of NSResult
 */
NSResult NSProviderDeleteTopic(char* topicName);

/**
 * Select a topic name for a consumer
 * @param[in]  consumerId  consumer id for which the user on provider selects a topic
 * @param[in]  topicName Topic name to select
 * @return ::NS_OK or result code of NSResult
 */
NSResult NSProviderSelectTopic(char* consumerId, char* topicName);

/**
 * Unselect a topic from the topic list for consumer
 * @param[in]  consumerId  consumer id for which the user on provider unselects a topic
 * @param[in]  topicName Topic name to unselect
 * @return ::NS_OK or result code of NSResult
 */
NSResult NSProviderUnselectTopic(char* consumerId, char* topicName);

/**
 * Request topic list with selection state for the consumer
 * @param[in] consumerid  the id of consumer which topic list is subscribed for
 * @return :: Topic list
 */
NSTopicLL * NSProviderGetConsumerTopics(char *consumerId);

/**
 * Request topics list already registered by provider user
 * @return :: Topic list
 */
NSTopicLL * NSProviderGetTopics();

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* _NS_PROVIDER_INTERFACE_H_ */


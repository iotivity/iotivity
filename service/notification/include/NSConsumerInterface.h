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
 * This file provides APIs of Notification Service for Consumer.
 */

#ifndef _NS_CONSUMER_INTERFACE_H_
#define _NS_CONSUMER_INTERFACE_H_

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#include "NSCommon.h"

/**
 * Provider and consumer use this callback function to receive the status of the message
 * synchronization
 * @param[in] provider    Provider who requests synchronization with the status
 * @param[in] sync        Synchronization information of the notification message
 */
typedef void (*NSSyncCallback)(NSProvider *, NSSyncInfo *);

/**
 * Consumer uses this callback function to receive the discovered providers
 * @param[in] provider        Provider who has the notification resource
 */
typedef void (*NSProviderDiscoveredCallback)(NSProvider *);

/**
 * Consumer use this callback function to receive notification message from provider
 * synchronization
 * @param[in] provider    Provider who sends notification message
 * @param[in] message     Notification message
 */
typedef void (*NSNotificationReceivedCallback)(NSProvider *, NSMessage *);

/**
 * Initialize notification service for consumer
 * @param[in]  providerDiscoveredCallback   Callback function to discover notification providers
 * @param[in]  notificationReceivedCallback   Callback function to receive notification messages
 * @param[in]  syncCallback   Callback function to receive synchronization status of notification
 * @return ::NS_OK or result code of NSResult
 */
NSResult NSStartConsumer(NSProviderDiscoveredCallback discoverCb,
        NSNotificationReceivedCallback postCb, NSSyncCallback syncCb);

/**
 * Terminate notification service for consumer
 * @return ::NS_OK or result code of NSResult
 */
NSResult NSStopConsumer();

/**
 * Request discovery manually
 * @return ::NS_OK or result code of NSResult
 */
NSResult NSRescanProvider();

/**
 * Request to subscribe notification message resource of provider
 * @param[in]  provider  Provider who send the notification message
 * @return ::NS_OK or result code of NSResult
 */
NSResult NSSubscribe(NSProvider *provider);

/**
 * Request to unsubscribe in order not to receive notification message from provider
 * @param[in]  provider  Provider who send the notification message
 * @return ::NS_OK or result code of NSResult
 */
NSResult NSUnsubscribe(NSProvider *provider);

NSResult NSConsumerReadCheck(NSMessage *);

NSResult NSConsumerDismissCheck(NSMessage *);

//** Remove below functions **//
NSResult NSDropNSObject(NSMessage *);
//** end of functions to be removed **//

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _NS_CONSUMER_INTERFACE_H_

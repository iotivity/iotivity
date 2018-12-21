/******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#ifndef _NS_CONSUMER_INTERNAL_TASK_CONTROLLER_H_
#define _NS_CONSUMER_INTERNAL_TASK_CONTROLLER_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "NSStructs.h"
#include "NSConsumerMemoryCache.h"
#include "NSConsumerCommunication.h"

/**
 * Get message cache list.
 *
 * @return message cache list.
 */
NSCacheList ** NSGetMessageCacheList(void);

/**
 * Set message cache list.
 *
 * @param cache list to set message
 */
void NSSetMessageCacheList(NSCacheList *);

/**
 * Get provider cache list.
 *
 * @return provider cache list.
 */
NSCacheList ** NSGetProviderCacheList(void);

/**
 * Cancel all subscription.
 */
void NSCancelAllSubscription(void);

/**
 * Set provider cache list.
 *
 * @param cache  cache list
 */
void NSSetProviderCacheList(NSCacheList *);

/**
 * Destroy message cache list.
 */
void NSDestroyMessageCacheList(void);

/**
 * Destory Internal cache list.
 */
void NSDestroyInternalCachedList(void);

/**
 * Find message using Id.
 *
 * @param message Id.
 *
 * @return notification message
 */
NSMessage * NSMessageCacheFind(const char *);

/**
 * Find provider using provider ID.
 *
 * @param provider Id
 *
 * @return provider information
 */
NSProvider_internal * NSProviderCacheFind(const char *);

/**
 * Find provider from address.
 *
 * @param addr   address to find provider
 *
 * @return provider information
 */
NSProvider_internal * NSFindProviderFromAddr(OCDevAddr * addr);

/**
 * Consumer internal task processing.
 *
 * @param notification task information
 */
void NSConsumerInternalTaskProcessing(NSTask *);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _NS_CONSUMER_INTERNAL_TASK_CONTROLLER_H_

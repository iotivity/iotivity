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

#ifndef _NS_CONSUMER_MEMORY_CACHE_H_
#define _NS_CONSUMER_MEMORY_CACHE_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include "NSConsumerCommon.h"

/**
 * Creates consumer storage list.
 *
 * @return new cache list.
 */
NSCacheList * NSConsumerStorageCreate(void);

/**
 * Read list.
 *
 * @param list    cache list.
 * @param findId  find element by given Id.
 *
 * @return cache element for given Id, otherwise NULL.
 */
NSCacheElement * NSConsumerStorageRead(NSCacheList * list, const char * findId);

/**
 * Write list.
 *
 * @param list     cache list.
 * @param newObj   new cache element object.
 *
 * @return OK if success, otherwise ERROR.
 */
NSResult NSConsumerStorageWrite(NSCacheList * list, NSCacheElement * newObj);

/**
 * Delete list for given Id.
 *
 * @param list    cache list.
 * @param delId   delete element by given Id.
 *
 * @return OK if success, otherwise ERROR.
 */
NSResult NSConsumerStorageDelete(NSCacheList * list, const char * delId);

/**
 * Destroy list.
 *
 * @param list cache list.
 *
 * @return OK success.
 */
NSResult NSConsumerStorageDestroy(NSCacheList * list);

/**
 * Get cache mutex.
 *
 * @return reference mutex, otherwise NULL.
 */
pthread_mutex_t * NSGetCacheMutex(void);

/**
 * Compare data with given Id
 *
 * @param type   cache type.
 * @param data   cache data.
 * @param id     Id to comapre.
 *
 * @return true if success, otherwise false.
 */
bool NSConsumerCompareIdCacheData(NSCacheType type, void * data, const char * id);

/**
 * Write provide to cache.
 *
 * @param list     cache list.
 * @param newObj   new cache object.
 *
 * @return OK if success, otherwise ERROR.
 */
NSResult NSConsumerCacheWriteProvider(NSCacheList * list, NSCacheElement * newObj);

/**
 * Pop provider cache list.
 *
 * @param list cache list.
 *
 * @return head , otherwise null.
 */
NSCacheElement * NSPopProviderCacheList(NSCacheList * list);

/**
 * Get provider from addr.
 *
 * @param list   cache list.
 * @param addr   device address.
 * @param port   port.
 *
 * @return cache element for addr & port, otherwise NULL.
 */
NSCacheElement * NSGetProviderFromAddr(NSCacheList * list, const char * addr, uint16_t port);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _NS_CONSUMER_MEMORY_CACHE_H_

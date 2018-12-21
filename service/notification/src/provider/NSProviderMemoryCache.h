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

#ifndef _NS_PROVIDER_CACHEADAPTER__H_
#define _NS_PROVIDER_CACHEADAPTER__H_

#include <pthread.h>
#include <stdbool.h>

#include "NSCommon.h"
#include "NSConstants.h"
#include "NSStructs.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "NSUtil.h"

/**
 * Create NS provider storage.
 *
 * @return new list.
 */
NSCacheList * NSProviderStorageCreate(void);

/**
 * Read NS provider storage for the given Id.
 *
 * @param list         NS cache list.
 * @param findId       Id to find in list.
 *
 * @return new list.
 */
NSCacheElement * NSProviderStorageRead(NSCacheList * list, const char * findId);

/**
 * Add new object in list.
 *
 * @param list         NS cache list.
 * @param newObj       new object to add.
 *
 * @return OK if write success , otherwise ERROR.
 */
NSResult NSProviderStorageWrite(NSCacheList * list, NSCacheElement * newObj);

/**
 * Delete cache list for the given id.
 *
 * @param list        NS cache list.
 * @param delId       Id to delete in list.
 *
 * @return OK if state updated , otherwise FAIL.
 */
NSResult NSProviderStorageDelete(NSCacheList * list, const char * delId);

/**
 * Destroy the list.
 *
 * @param list NS cache list.
 *
 * @return OK after destroy.
 */
NSResult NSProviderStorageDestroy(NSCacheList * list);

/**
 * Delete cache data.
 *
 * @param NSCacheType  cache type to delete data.
 * @param cache data.
 *
 * @return OK if data deleted.
 */
NSResult NSProviderDeleteCacheData(NSCacheType, void *);

/**
 * Comapre Id in cache data.
 *
 * @param cache type.
 * @param cache data.
 * @param Id to comapre.
 *
 * @return True if id available, otherwise false.
 */
bool NSProviderCompareIdCacheData(NSCacheType, void *, const char *);

/**
 * Checks whether data is found.
 *
 * @param cache type.
 * @param cache data.
 * @param Id to comapre.
 *
 * @return True if id available, otherwise false.
 */
bool NSProviderIsFoundCacheData(NSCacheType, void *, void*);

/**
 * Update sub scription state for the given id.
 *
 * @param NS cache list.
 * @param Id to update in list.
 * @param value to update in list.
 *
 * @return OK if state updated , otherwise ERROR.
 */
NSResult NSCacheUpdateSubScriptionState(NSCacheList *, char *, bool);


/**
 * Delete data using observation Id.
 *
 * @param NSCacheList   NS cache list.
 * @param id            observation id.
 *
 * @return OK if state updated , otherwise ERROR.
 */
NSResult NSProviderDeleteSubDataFromObId(NSCacheList * list, OCObservationId id);

/**
 * Get topics from cache data
 *
 * @param regTopicList register topic list.
 *
 * @return topics.
 */
NSTopicLL * NSProviderGetTopicsCacheData(NSCacheList * regTopicList);

/**
 * Get consumer topics for the given id.
 *
 * @param regTopicList register topic list.
 * @param conTopicList consumer topic list.
 * @param consumerId   consumer Id.
 *
 * @return topics.
 */
NSTopicLL * NSProviderGetConsumerTopicsCacheData(NSCacheList * regTopicList,
        NSCacheList * conTopicList, const char * consumerId);

/**
 * Checks whether topic is subscribed.
 *
 * @param conTopicList  Consumer topic list.
 * @param cId           consumer Id.
 * @param topicName     topic name.
 *
 * @return True if topic subscribed, otherwise false.
 */
bool NSProviderIsTopicSubScribed(NSCacheElement * conTopicList, char * cId, char * topicName);

/**
 * Delete consumer topic from consumer topic list.
 *
 * @param conTopicList Consumer topic list.
 * @param topicSubData topic subscribed data.
 *
 * @return OK if deleted , otherwise ERROR.
 */
NSResult NSProviderDeleteConsumerTopic(NSCacheList * conTopicList,
        NSCacheTopicSubData * topicSubData);

pthread_mutex_t NSCacheMutex;
pthread_mutexattr_t NSCacheMutexAttr;

#endif /* _NS_PROVIDER_CACHEADAPTER__H_ */

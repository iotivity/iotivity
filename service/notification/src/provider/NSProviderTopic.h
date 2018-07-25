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

#ifndef _NS_PROVIDER_TOPIC_H_
#define _NS_PROVIDER_TOPIC_H_

#include "NSCommon.h"
#include "NSStructs.h"
#include "NSConstants.h"
#include "NSProviderScheduler.h"
#include "NSProviderResource.h"
#include "NSProviderSubscription.h"

/** Notification topic synchronize structure */
typedef struct {
    pthread_cond_t * condition; /**< conditional variable */
    pthread_mutex_t * mutex;    /**< mutex variable */
    char * consumerId;          /**< consumer ID */
    NSTopicLL * topics;         /**< pointer to topic linked list */
} NSTopicSync;

/** Notification topic synchronize result structure */
typedef struct {
    pthread_cond_t * condition; /**< conditional variable */
    pthread_mutex_t * mutex;    /**< mutex variable */
    void * topicData;           /**< topic data */
    NSResult result;            /**< notification result */
} NSTopicSyncResult;

/** consumer topic list */
NSCacheList * consumerTopicList;
/** registered topic list */
NSCacheList * registeredTopicList;

/**
 * Initialize the topic list.
 *
 * @return NS_OK if intialized, otherwise NS_FAIL.
 */
NSResult NSInitTopicList(void);

/**
 * Get the topic with given consumer Id.
 *
 * @param consumerId     Id to get topic.
 *
 * @return topic.
 */
NSTopicList * NSGetTopics(char *consumerId);

/**
 * Check topic is subscribed or not.
 *
 * @param consumerId         Id to find topic.
 * @param topic              topic name.
 *
 * @return true if subscribed, otherwise fail.
 */
bool NSIsTopicSubscribed(char *consumerId, char *topic);

#endif /* _NS_PROVIDER_TOPIC_H_ */

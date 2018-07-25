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

#ifndef _NS_STRUCTS_H_
#define _NS_STRUCTS_H_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <octypes.h>
#include "NSCommon.h"
#include "NSConstants.h"
#include "ocstack.h"

/** ns task structure */
typedef struct _nsTask
{
    NSTaskType taskType;        /**< task type */
    void * taskData;            /**< task data */
    struct _nsTask * nextTask;  /**< pointer to next task */

} NSTask;

/** ns topic list structure */
typedef struct
{
    NSTopicLL * head;                       /**< head node of list */
    NSTopicLL * tail;                       /**< tail node of list */
    char consumerId[NS_UUID_STRING_SIZE];   /**< consumer Id */
    NSTopicLL ** topics;                    /**< poniter to structure */

} NSTopicList;

typedef void * NSCacheData; /**< ns cache data */

/** ns cache element structure */
typedef struct _NSCacheElement
{
    NSCacheData * data;                 /**< cache data */
    struct _NSCacheElement * next;      /**< pointer to next element */

} NSCacheElement;

/** ns cache list */
typedef struct
{
    NSCacheType cacheType;         /**< cache type */
    NSCacheElement * head;         /**< head node of list */
    NSCacheElement * tail;         /**< tail node of list */

} NSCacheList;

/** ns cache sub data structure */
typedef struct
{
    char id[NS_UUID_STRING_SIZE]; /**< id */
    int syncObId;                 /**< sync resource observer ID for local consumer */
    int messageObId;              /**< message resource observer ID for local consumer */
    bool isWhite;                 /**< access state -> True: allowed / False: blocked */

} NSCacheSubData;

/** ns cache message data structure */
typedef struct
{
    char * id; /**< id */
    int messageType; /**< noti = 1, read = 2, dismiss = 3 */
    NSMessage * nsMessage; /**< message */

} NSCacheMsgData;

/** ns cache topic data structure */
typedef struct
{
    char * topicName;    /**< topic name */
    NSTopicState state;  /**< topic state */

} NSCacheTopicData;

/** ns cache topic sub data structure */
typedef struct
{
    char id[NS_UUID_STRING_SIZE];  /**< id */
    char * topicName;              /**< topic name */

} NSCacheTopicSubData;

/** ns notification resource structure */
typedef struct
{
    OCResourceHandle handle;               /**< resource handle */
    char providerId[NS_UUID_STRING_SIZE];  /**< provider Id */
    char * version;                        /**< version */
    bool policy;                           /**< policy */
    char * message_uri;                    /**< message uri */
    char * sync_uri;                       /**< sync uri */

    /** optional */
    char * topic_uri;                     /**< topic uri */

} NSNotificationResource;

/** ns message resource */
typedef struct
{
    OCResourceHandle handle;              /**< resource handle */

    uint64_t messageId;                   /**< message Id */
    char providerId[NS_UUID_STRING_SIZE]; /**< provider Id */

    //optional
    NSMessageType type;                   /**< type */
    char * dateTime;                      /**< date time */
    uint64_t ttl;                         /**< ttl */
    char * title;                         /**< title */
    char * contentText;                   /**< content text */
    char * sourceName;                    /**< source name */
    char * topicName;                     /**< topic name */
    NSMediaContents * mediaContents;      /**< media contents */

} NSMessageResource;

/** ns sync resource structure */
typedef struct
{
    OCResourceHandle handle;              /**< resource handle */
    uint64_t messageId;                   /**< message Id */
    char providerId[NS_UUID_STRING_SIZE]; /**< provider Id */
    char * state;                         /**< resource state */

} NSSyncResource;

/** ns topic resource structure */
typedef struct
{
    OCResourceHandle handle;              /**< resource handle */
    char providerId[NS_UUID_STRING_SIZE]; /**< provider Id */
    char consumerId[NS_UUID_STRING_SIZE]; /**< consumer Id */
    NSTopicList ** TopicList;             /**< pointer to topic list */

} NSTopicResource;

/** ns provider information structure */
typedef struct
{
    char providerId[NS_UUID_STRING_SIZE];  /**< provider Id */
    char * providerName;                   /**< provider name */
    char * userInfo;                       /**< user information */

} NSProviderInfo;

#ifdef WITH_MQ
/** ns MQ topic address structure */
typedef struct
{
    char * serverAddr;                     /**< server address */
    char * topicName;                      /**< topic name */

} NSMQTopicAddress;

/** ns MQ server information structure */
typedef struct
{
    char * serverUri;                   /**< server uri */
    OCDevAddr * devAddr;                /**< device address */

} NSMQServerInfo;
#endif

#endif /* _NS_STRUCTS_H_ */

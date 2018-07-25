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

/**
 * @file
 *
 * This file provides APIs of Notification Service for common functions.
 */

#ifndef _NS_COMMON_H_
#define _NS_COMMON_H_

#include <stdint.h>
#include <octypes.h>

#define NS_UUID_STRING_SIZE 37 /**< UUID string size */
//#define WITH_MQ
/**
 * Result code of notification service
 */
typedef enum eResult
{
    NS_OK = 100,        /**< success result */
    NS_ERROR = 200,     /**< error result */
    NS_SUCCESS = 300,   /**< success result */
    NS_FAIL = 400,      /**< failure result */

} NSResult;

/**
 * Provider state of notification consumer service
 */
typedef enum
{
    NS_ALLOW = 1,        /**< allow state */
    NS_DENY = 2,         /**< deny state */
    NS_TOPIC = 3,        /**< topic state */
    NS_DISCOVERED = 11,  /**< discovered state */
    NS_STOPPED = 12      /**< stopped state */

} NSProviderState;

/**
 * Notification message status to synchronize
 */
typedef enum
{
    NS_SYNC_UNREAD = 0,       /**< synchronize unread status */
    NS_SYNC_READ = 1,         /**< synchronize read status */
    NS_SYNC_DELETED = 2,      /**< synchronize deleted status */

} NSSyncType;

/**
 * Notification Message Type
 * Alert mean is High / critical
 * Notice mean is low / critical
 * Event mean is High / Normal
 * Information mean is Low / Normal
 */
typedef enum
{
    NS_MESSAGE_ALERT = 1,        /**< alert message type */
    NS_MESSAGE_NOTICE = 2,       /**< notice message type */
    NS_MESSAGE_EVENT = 3,        /**< event message type */
    NS_MESSAGE_INFO = 4,         /**< info message type */
    NS_MESSAGE_WARNING = 5,      /**< warning message type */
    NS_MESSAGE_READ = 11,        /**< read message type */
    NS_MESSAGE_DELETED = 12      /**< deleted message type */

} NSMessageType;

/**
 *  Notification topic state
 */
typedef enum
{
    NS_TOPIC_UNSUBSCRIBED = 0,   /**< topic unsubscribed notification */
    NS_TOPIC_SUBSCRIBED = 1,     /**< topic subscribed notification */

} NSTopicState;

/**
 * Topic linked list
 */
typedef struct _nsTopic
{
    char * topicName;         /**< topic name */
    NSTopicState state;       /**< topic state */
    struct _nsTopic * next;   /**< link to other node */

} NSTopicLL;

/**
 *  Consumer information
 */
typedef struct
{
    char consumerId[NS_UUID_STRING_SIZE];  /**< consumer Id */

} NSConsumer;

/**
 *  Provider information
 */
typedef struct
{
    char providerId[NS_UUID_STRING_SIZE];   /**< provider Id */

} NSProvider;

/**
 *  Media Contents of Notification Message (Optional)
 */
typedef struct
{
    char * iconImage;    /**< icon image */

} NSMediaContents;

/**
 *  Notification Message
 */
typedef struct
{
    // Mandatory
    uint64_t messageId;                   /**< the notification of index message */
    char providerId[NS_UUID_STRING_SIZE]; /**< provider Id */

    // optional
    NSMessageType type;                   /**< message type */
    char * dateTime;                      /**< date time */
    uint64_t ttl;                         /**< time duaration in which the notification is meaningful */
    char * title;                         /**< title of the notification */
    char * contentText;                   /**< description of the notification */
    char * sourceName;                    /**< source name which produce the notification message */
    NSMediaContents * mediaContents;      /**< notification icon image */
    char * topic;                         /**< interest topic */
    OCRepPayload * extraInfo;             /**< additional information which is user defined data */

} NSMessage;

/**
 *  Synchronization information of the notification message
 */
typedef struct
{
    uint64_t messageId;                   /**< index of notification message */
    char providerId[NS_UUID_STRING_SIZE]; /**< provider Id */
    NSSyncType state;                     /**< status of notification message */

} NSSyncInfo;

#endif /* _NS_COMMON_H_ */


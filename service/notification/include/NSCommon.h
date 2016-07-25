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
 * This file provides APIs of Notification Service for common functions.
 */

#ifndef _NS_COMMON_H_
#define _NS_COMMON_H_

#include <stdint.h>

#define NS_ATTRIBUTE_POLICY "ACCEPTER"
#define NS_ATTRIBUTE_MESSAGE "MESSAGE_URI"
#define NS_ATTRIBUTE_SYNC "SYNC_URI"
#define NS_ATTRIBUTE_ACCPETANCE "ACCEPTANCE"
#define NS_ATTRIBUTE_MESSAGE_ID "MESSAGE_ID"
#define NS_ATTRIBUTE_PROVIDER_ID "PROVIDER_ID"
#define NS_ATTRIBUTE_TITLE "TITLE"
#define NS_ATTRIBUTE_TEXT "CONTENTTEXT"
#define NS_ATTRIBUTE_SOURCE "SOURCE"
#define NS_ATTRIBUTE_STATE "STATE"
#define NS_ATTRIBUTE_DEVICE "DEVICE"
#define NS_ATTRIBUTE_TYPE "TYPE"
#define NS_ATTRIBUTE_DATETIME "DATE_TIME"
#define NS_ATTRIBUTE_TTL "TTL"

/**
 * Result code of notification service
 */
typedef enum eResult
{
    NS_OK = 100,
    NS_ERROR = 200,
    NS_SUCCESS = 300,
    NS_FAIL = 400,
    NS_ALLOW = 500,
    NS_DENY = 600,

} NSResult;

/**
 * Access policy exchanged between provider and consumer during subscription process
 */
typedef enum eAccessPolicy
{
    NS_ACCESS_ALLOW = 0,
    NS_ACCESS_DENY = 1,
} NSAccessPolicy;

/**
 * Notification message status to synchronize
 */
typedef enum
{
    NS_SYNC_UNREAD = 0,
    NS_SYNC_READ = 1,
    NS_SYNC_DELETED = 2,
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
    NS_MESSAGE_ALERT = 1,
    NS_MESSAGE_NOTICE = 2,
    NS_MESSAGE_EVENT = 3,
    NS_MESSAGE_INFO = 4,

} NSMessageType;

/**
 *  Consumer information
 */
typedef struct
{
    char consumerId[37];

} NSConsumer;

/**
 *  Provider information
 */
typedef struct
{
    char providerId[37];

} NSProvider;

/**
 *  Media Contents of Notification Message (Optional)
 */
typedef struct
{
    char * iconImage;

} NSMediaContents;

/**
 *  Notification Message
 */
typedef struct
{
    //Mandatory
    uint64_t messageId;
    char providerId[37];

    //optional
    NSMessageType type;
    char * dateTime;
    uint64_t ttl;
    char * title;
    char * contentText;
    char * sourceName;
    NSMediaContents * mediaContents;

} NSMessage;

/**
 *  Synchronization information of the notification message
 */
typedef struct
{
    uint64_t messageId;
    char providerId[37];
    NSSyncType state;

} NSSyncInfo;

#endif /* _NS_COMMON_H_ */


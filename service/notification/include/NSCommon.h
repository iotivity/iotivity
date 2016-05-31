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

#include <ocstack.h>

#define NS_ATTRIBUTE_POLICY "ACCEPTER"
#define NS_ATTRIBUTE_MESSAGE "MESSAGE_URI"
#define NS_ATTRIBUTE_SYNC "SYNC_URI"
#define NS_ATTRIBUTE_ACCPETANCE "ACCEPTANCE"
#define NS_ATTRIBUTE_ID "ID"
#define NS_ATTRIBUTE_TITLE "TITLE"
#define NS_ATTRIBUTE_TEXT "CONTENTTEXT"
#define NS_ATTRIBUTE_SOURCE "SOURCE"
#define NS_ATTRIBUTE_STATE "STATE"
#define NS_ATTRIBUTE_DEVICE "DEVICE"

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
    NS_ACCEPTER_PROVIDER = 0,
    NS_ACCEPTER_CONSUMER = 1,

} NSAccessPolicy;

/**
 * Notification message status to synchronize
 */
typedef enum
{
    Notification_Read = 0,
    Notification_Dismiss = 1,
    Notification_Unread = 2,

} NSSyncTypes;

/**
 *  Consumer information
 */
typedef struct
{
    char * mDeviceId;
    char * mAddress;

} NSConsumer;

/**
 *  Provider information
 */
typedef struct
{
    char * mId;
    char * mUserData;
    char * messageUri;
    char * syncUri;
    OCDoHandle messageHandle;
    OCDoHandle syncHandle;

} NSProvider;

/**
 *  Notification Message
 */
typedef struct
{
    char * mId;
    char * mTitle;
    char * mContentText;
    char * mSource;

} NSMessage;

/**
 *  Synchronization information of the notification message
 */
typedef struct
{
    // Mandatory
    char * mMessageId;
    char * mSourceId; // TO-DO from whom
    NSSyncTypes mState;

} NSSync;

#endif /* _NS_COMMON_H_ */


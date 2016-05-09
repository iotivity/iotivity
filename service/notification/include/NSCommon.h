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

#ifndef _NS_COMMON_H_
#define _NS_COMMON_H_

#define NS_ATTRIBUTE_POLICY "ACCEPTER"
#define NS_ATTRIBUTE_MESSAGE "MESSAGE_URI"
#define NS_ATTRIBUTE_SYNC "SYNC_URI"
#define NS_ATTRIBUTE_ACCPETANCE "ACCEPTANCE"
#define NS_ATTRIBUTE_ID "ID"
#define NS_ATTRIBUTE_TITLE "TITLE"
#define NS_ATTRIBUTE_TEXT "CONTENTTEXT"
#define NS_ATTRIBUTE_STATE "STATE"
#define NS_ATTRIBUTE_DEVICE "DEVICE"

typedef enum eResult
{
    NS_OK = 100,
    NS_ERROR = 200,
    NS_SUCCESS = 300,
    NS_FAIL = 400,
    NS_ALLOW = 500,
    NS_DENY = 600,

} NSResult;

typedef enum eAccessPolicy
{
    NS_ACCEPTER_PROVIDER = 0,
    NS_ACCEPTER_CONSUMER = 1,

} NSAccessPolicy;

typedef struct
{
    char * mId;
    void * mUserData;
} NSDevice;

typedef NSDevice NSConsumer;

typedef struct
{
    char * mId;
    char * mUserData;
    char * messageUri;
    char * syncUri;
} NSProvider;

typedef struct
{
    // Mandatory
    char * mId;
    char * mTitle;

    //Optional
    char * mContentText;

} NSMessage;

typedef enum
{
    Notification_Read,
    Notification_Dismiss,
} NSSyncTypes;

typedef struct
{
    // Mandatory
    char * mMessageId;
    NSSyncTypes mState;

    //Optional
    NSDevice * mDevice;

} NSSync;

typedef void (* NSSubscribeRequestCallback)(NSConsumer *);
typedef void (* NSSyncCallback)(NSProvider *, NSSync *);

typedef void (* NSProviderDiscoveredCallback)(NSProvider *);
// NSMessage should deleted as user.
typedef void (* NSNotificationReceivedCallback)(NSProvider *, NSMessage *);

#endif /* _NS_COMMON_H_ */


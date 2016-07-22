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

#ifndef _NS_STRUCTS_H_
#define _NS_STRUCTS_H_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <octypes.h>
#include "NSCommon.h"
#include "NSConstants.h"

typedef struct _nsTask
{
    NSTaskType taskType;
    void * taskData;
    struct _nsTask * nextTask;
} NSTask;

typedef void * NSCacheData;

typedef struct _NSCacheElement
{
    NSCacheData * data;
    struct _NSCacheElement * next;
} NSCacheElement;

typedef struct
{
    NSCacheType cacheType;
    NSCacheElement * head;
    NSCacheElement * tail;
} NSCacheList;

typedef struct
{
    char id[37];
    int syncObId; // sync resource observer ID for local consumer
    int messageObId; // message resource observer ID for local consumer
    int remote_syncObId; //sync observer ID subscribed through remote server
    int remote_messageObId; //message observer ID subscribed through remote server
    bool isWhite; // access state -> True: allowed / False: blocked
} NSCacheSubData;

typedef struct
{
    char * id;
    int messageType; // noti = 1, read = 2, dismiss = 3
    NSMessage * nsMessage;
} NSCacheMsgData;

typedef struct
{
    OCResourceHandle handle;
    int accepter;
    char * message_uri;
    char * sync_uri;
} NSNotificationResource;

typedef struct
{
    OCResourceHandle handle;

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
} NSMessageResource;

typedef struct
{
    OCResourceHandle handle;
    char * id;
    char * state;
} NSSyncResource;

typedef struct
{
    char providerId[37];
    char * providerName;
} NSProviderInfo;

#endif /* _NS_STRUCTS_H_ */

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
#include "NSCommon.h"
#include "NSConstants.h"

typedef struct _nsTask
{
    NSTaskType taskType;
    void* taskData;
    struct _nsTask* nextTask;
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
    char * id; // ip
    int syncObId;
    int messageObId;
    bool isWhite;
}NSCacheSubData;

typedef struct
{
    char * id; // ip ? ? ?
    int messageType; // noti = 1, read = 2, dismiss = 3
    NSMessage * nsMessage;
}NSCacheMsgData;

#endif /* _NS_STRUCTS_H_ */

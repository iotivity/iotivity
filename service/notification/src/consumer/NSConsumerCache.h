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

#ifndef _NS_CONSUMER_CACHE_H_
#define _NS_CONSUMER_CACHE_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdlib.h>
#include <stdbool.h>

#include "NSCommon.h"
#include "NSConsumerCommon.h"


typedef void * NSCacheData;

typedef struct _NSCacheObject
{
    NSCacheData * data;
    struct _NSCacheObject * next;
} NSCacheObject;

NSCacheList * NSConsumerCacheInit();
NSResult NSConsumerCacheDestroy(NSCacheList * list);

NSResult NSConsumerCacheInsert(NSCacheList * list, NSCacheObject * newObj);
NSResult NSConsumerCacheDelete(NSCacheList * list, NSCacheObject * delObj);

NSCacheObject * NSConsumerCacheFind(NSCacheList * list, char * findId);

NSResult NSConsumerCacheWrite(NSCacheList * list, NSCacheObject * writeObj);
NSCacheObject * NSConsumerCacheRead(NSCacheList * list, NSCacheObject * readObj);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _NS_CONSUMER_CACHE_H_

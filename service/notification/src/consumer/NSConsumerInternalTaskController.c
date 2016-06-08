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

#include "NSConstants.h"
#include "NSConsumerCommon.h"
#include "NSConsumerInternalTaskController.h"
#include "NSStructs.h"

#include "oic_malloc.h"

NSCacheList ** NSGetCacheList()
{
    static NSCacheList * cache = NULL;
    return & cache;
}

void NSSetCacheList(NSCacheList * cache)
{
    *(NSGetCacheList()) = cache;
}

void NSDestroyCacheList()
{
    NSCacheList * cache;
    cache = *(NSGetCacheList());
    if (cache)
    {
        NSStorageDestroy(cache);
    }
}

NSResult NSCacheUpdate(NSCacheList * cache, NSTask * task, NSConsumerMessageTypes type)
{
    NSMessage_consumer * noti = (NSMessage_consumer *) task->taskData;
    noti->type = type;

    NSCacheElement * obj = (NSCacheElement *)OICMalloc(sizeof(NSCacheElement));
    NS_VERTIFY_NOT_NULL(obj, NS_ERROR);

    obj->data = (NSCacheData *) noti;
    obj->next = NULL;

    NS_LOG(DEBUG, "try to write to storage");
    NSResult ret = NSStorageWrite(cache, obj);
    NS_VERTIFY_NOT_NULL_WITH_POST_CLEANING(ret == NS_OK ? (void *) 1 : NULL,
            NS_ERROR, NSRemoveMessage(noti));

    NSRemoveMessage(noti);
    OICFree(obj);

    return NS_OK;
}

void NSConsumerSubscriptionTaskProcessing(NSTask * task)
{
    NS_VERTIFY_NOT_NULL_V(task);

    NSCacheList * cache;
    if (!*(NSGetCacheList()))
    {
        NS_LOG(DEBUG, "Cache Init");
        cache = NSStorageCreate();
        NS_VERTIFY_NOT_NULL_V(cache);

        cache->cacheType = NS_CONSUMER_CACHE_MESSAGE;
        NSSetCacheList(cache);
    }
    cache = *(NSGetCacheList());

    NSResult ret = NS_ERROR;
    NS_LOG_V(DEBUG, "Receive Event : %d", (int)task->taskType);
    switch (task->taskType)
    {
        case TASK_CONSUMER_RECV_MESSAGE:
        {
            NS_LOG(DEBUG, "Receive New Notification");

            ret = NSCacheUpdate(cache, task, Notification);
            NS_VERTIFY_NOT_NULL_V(ret == NS_OK ? (void *) 1 : NULL);
            break;
        }
        case TASK_RECV_READ:
        {
            NS_LOG(DEBUG, "Receive Read Notification");

            ret = NSCacheUpdate(cache, task, Read);
            NS_VERTIFY_NOT_NULL_V(ret == NS_OK ? (void *) 1 : NULL);
            break;
        }
        case TASK_RECV_DISMISS:
        {
            NS_LOG(DEBUG, "Receive Dismiss Notification");

            ret = NSCacheUpdate(cache, task, Dismiss);
            NS_VERTIFY_NOT_NULL_V(ret == NS_OK ? (void *) 1 : NULL);
            break;
        }
        default :
        {
            NS_LOG(ERROR, "Unknown TASK Type");
            return ;
        }
    }
}

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

#include "NSConsumerSubsription.h"

#include "NSConstants.h"
#include "NSConsumerCommon.h"
#include "NSConsumerCache.h"
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

NSResult NSCacheUpdate(NSCacheList * cache, NSTask * task, NSConsumerMessageTypes type)
{
    NSMessage_consumer * noti = (NSMessage_consumer *) task->taskData;
    noti->type = type;

    NSCacheObject * obj = (NSCacheObject *)OICMalloc(sizeof(NSCacheObject));
    if (!obj)
    {
        NS_LOG(ERROR, "Cache allocation is failed");
    }
    obj->data = (NSCacheData *) noti;
    obj->next = NULL;

    if (NS_OK != NSConsumerCacheWrite(cache, obj))
    {
        NS_LOG(DEBUG, "CacheUpdate - NSConsumerCacheInsert");
        if (NS_OK != NSConsumerCacheInsert(cache, (NSCacheObject *) obj))
        {
            NS_LOG(ERROR, "Cache insert fail");
            return NS_ERROR;
        }
    }

    NSRemoveMessage(noti);
    OICFree(obj);

    return NS_OK;
}

void NSConsumerSubscriptionHandleMsg(NSTask * task)
{
    if (!task)
    {
        NS_LOG(ERROR, "task is null");
        return;
    }

    NSCacheList * cache;
    if (!*(NSGetCacheList()))
    {
        NS_LOG(DEBUG, "Cache Init");
        cache = NSConsumerCacheInit();
        if (!cache)
        {
            NS_LOG(ERROR, "Cache create fail");
            return;
        }
        NSSetCacheList(cache);
    }
    cache = *(NSGetCacheList());
    if (!cache->head)
    {
        NS_LOG(DEBUG, "Cache Head is null 2");
    }
    else{
        NS_LOG(DEBUG, "Cache Head is not null 2");
    }


    NS_LOG_V(DEBUG, "Receive Event : %d", (int)task->taskType);

    switch (task->taskType)
    {
        case TASK_CONSUMER_RECV_NOTIFICATION:
        {
            NS_LOG(DEBUG, "Receive New Notification");

            if (NS_OK != NSCacheUpdate(cache, task, Notification))
            {
                NS_LOG(ERROR, "Cache Update fail");
                return;
            }
            break;
        }
        case TASK_RECV_READ:
        {
            NS_LOG(DEBUG, "Receive Read Notification");
            // TODO update Cache.
            if (NS_OK != NSCacheUpdate(cache, task, Read))
            {
                NS_LOG(ERROR, "Cache Update fail");
                return;
            }
            break;
        }
        case TASK_RECV_DISMISS:
        {
            NS_LOG(DEBUG, "Receive Dismiss Notification");
            if (NS_OK != NSCacheUpdate(cache, task, Dismiss))
            {
                NS_LOG(ERROR, "Cache Update fail");
                return;
            }
            break;
        }
        default :
        {
            NS_LOG(ERROR, "Unknown TASK Type");
            return ;
        }
    }
}

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

#include "NSConsumerMemoryCache.h"
#include "oic_malloc.h"
#include "oic_string.h"

pthread_mutex_t * NSGetCacheMutex()
{
    static pthread_mutex_t NSCacheMutex;
    return & NSCacheMutex;
}

void NSSetCacheMutex(pthread_mutex_t mutex)
{
    *(NSGetCacheMutex()) = mutex;
}

NSCacheList * NSStorageCreate()
{
    pthread_mutex_t * mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(mutex, NULL);
    NSSetCacheMutex(*mutex);
    mutex = NSGetCacheMutex();


    pthread_mutex_lock(mutex);

    NSCacheList * newList = (NSCacheList *) OICMalloc(sizeof(NSCacheList));
    if (!newList)
    {
        pthread_mutex_unlock(mutex);
        NS_LOG (ERROR, "Failed to Create Cache");
        return NULL;
    }

    newList->head = newList->tail = NULL;

    pthread_mutex_unlock(mutex);

    return newList;
}

NSCacheElement * NSStorageRead(NSCacheList * list, const char * findId)
{
    pthread_mutex_t * mutex = NSGetCacheMutex();

    pthread_mutex_lock(mutex);

    NSCacheElement * iter = list->head;
    NSCacheElement * next = NULL;
    NSCacheType type = list->cacheType;

    while (iter)
    {
        next = iter->next;

        pthread_mutex_unlock(mutex);

        if (NSConsumerCompareIdCacheData(type, iter->data, findId))
        {
            pthread_mutex_unlock(mutex);

            return iter;
        }

        iter = next;
    }

    pthread_mutex_unlock(mutex);
    NS_LOG (DEBUG, "No Cache Element");
    return NULL;
}

NSResult NSStorageWrite(NSCacheList * list, NSCacheElement * newObj)
{
    pthread_mutex_t * mutex = NSGetCacheMutex();

    pthread_mutex_lock(mutex);

    NSCacheType type = list->cacheType;

    if (!newObj)
    {
        pthread_mutex_unlock(mutex);
        NS_LOG (ERROR, "Failed to Write Cache");
        return NS_ERROR;
    }

    if (type == NS_CONSUMER_CACHE_MESSAGE)
    {
        pthread_mutex_unlock(mutex);

        return NSConsumerCacheWriteMessage(list, newObj);
    }

    NS_LOG (ERROR, "Not Supported Type");
    pthread_mutex_unlock(mutex);

    return NS_ERROR;
}

NSResult NSStorageDelete(NSCacheList * list, const char * delId)
{
    pthread_mutex_t * mutex = NSGetCacheMutex();

    pthread_mutex_lock(mutex);

    if (!delId)
    {
        pthread_mutex_unlock(mutex);
        NS_LOG (ERROR, "Failed to Delete Cache");
        return NS_ERROR;
    }

    NSCacheElement * prev = list->head;
    NSCacheElement * del = list->head;

    NSCacheType type = list->cacheType;

    if (NSConsumerCompareIdCacheData(type, del->data, delId))
    {
        if (del == list->head)
        {
            if (del == list->tail)
                list->tail = del->next;
            list->head = del->next;

            NSRemoveMessage((NSMessage_consumer *) del->data);
            OICFree(del);
            pthread_mutex_unlock(mutex);

            return NS_OK;
        }
    }

    del = del->next;
    while (del)
    {
        if (NSConsumerCompareIdCacheData(type, del->data, delId))
        {
            if (del == list->tail)
                list->tail = prev;

            prev->next = del->next;
            NSRemoveMessage((NSMessage_consumer *) del->data);
            OICFree(del);
            pthread_mutex_unlock(mutex);

            return NS_OK;
        }

        prev = del;
        del = del->next;
    }
    pthread_mutex_unlock(mutex);
    return NS_OK;
}

NSResult NSConsumerCacheWriteMessage(NSCacheList * list, NSCacheElement * newObj)
{
    pthread_mutex_t * mutex = NSGetCacheMutex();

    pthread_mutex_lock(mutex);

    if (!newObj)
    {
        pthread_mutex_unlock(mutex);
        NS_LOG (ERROR, "Failed to Write Message Cache");
        return NS_ERROR;
    }

    NSMessage_consumer * newMsgObj = (NSMessage_consumer *) newObj->data;

    pthread_mutex_unlock(mutex);
    char msgId[] = {0, };
    sprintf(msgId, "%ld", newMsgObj->messageId);
    NSCacheElement * it = NSStorageRead(list, msgId);
    pthread_mutex_lock(mutex);

    if (it)
    {
        NSMessage_consumer * msgObj = (NSMessage_consumer *) it->data;
        it->data = (void *) NSCopyMessage(newMsgObj);
        if (!it->data)
        {
            NS_LOG (ERROR, "Failed to CopyMessage");
            it->data = (void *) msgObj;
            pthread_mutex_unlock(mutex);

            return NS_ERROR;
        }
        NSRemoveMessage(msgObj);
        pthread_mutex_unlock(mutex);

        return NS_OK;
    }

    NSCacheElement * obj = (NSCacheElement *) OICMalloc(sizeof(NSCacheElement));
    if (!obj)
    {
        NS_LOG(ERROR, "Fail to Create New Object");
        pthread_mutex_unlock(mutex);

        return NS_ERROR;
    }
    obj->data = (void *) NSCopyMessage(newMsgObj);
    if (!obj->data)
    {
        NS_LOG (ERROR, "Failed to CopyMessage");
        pthread_mutex_unlock(mutex);

        return NS_ERROR;
    }
    obj->next = NULL;

    if (!list->head)
    {
        list->head = obj;
        list->tail = obj;
        pthread_mutex_unlock(mutex);

        return NS_OK;
    }

    (list->tail)->next = obj;
    list->tail = obj;
    pthread_mutex_unlock(mutex);

    return NS_OK;
}

NSResult NSStorageDestroy(NSCacheList * list)
{
    pthread_mutex_t * mutex = NSGetCacheMutex();

    pthread_mutex_lock(mutex);

    NSCacheElement * iter = list->head;
    NSCacheElement * next = NULL;

    NSCacheType type = list->cacheType;

    if (type == NS_CONSUMER_CACHE_MESSAGE)
    {

        while (iter)
        {
            next = (NSCacheElement *) iter->next;

            NSRemoveMessage((NSMessage_consumer *) iter->data);
            OICFree(iter);

            iter = next;
        }

        OICFree(list);
    }

    pthread_mutex_unlock(mutex);

    return NS_OK;
}

bool NSConsumerCompareIdCacheData(NSCacheType type, void * data, const char * id)
{
    if (data == NULL)
    {
        return false;
    }

    if (type == NS_CONSUMER_CACHE_MESSAGE)
    {
        NSMessage_consumer * msg = (NSMessage_consumer *) data;

        char msgId[] = {0, };
        sprintf(msgId, "%ld", msg->messageId);
        if (!strcmp(msgId, id))
        {
            return true;
        }

        return false;
    }

    return false;
}

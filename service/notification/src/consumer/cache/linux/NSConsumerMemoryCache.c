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
    pthread_mutex_t * mutex = (pthread_mutex_t *) OICMalloc(sizeof(pthread_mutex_t));
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

    NS_LOG_V(DEBUG, "cache type : %d", type);
    if (type == NS_CONSUMER_CACHE_MESSAGE)
    {
        pthread_mutex_unlock(mutex);

        return NSConsumerCacheWriteMessage(list, newObj);
    }
    else if (type == NS_CONSUMER_CACHE_PROVIDER)
    {
        pthread_mutex_unlock(mutex);

        return NSConsumerCacheWriteProvider(list, newObj);
    }

    NS_LOG (ERROR, "Not Supported Type");
    pthread_mutex_unlock(mutex);

    return NS_ERROR;
}

NSResult NSStorageDelete(NSCacheList * list, const char * delId)
{
    pthread_mutex_t * mutex = NSGetCacheMutex();

    pthread_mutex_lock(mutex);

    NSCacheType type = list->cacheType;

    if (!delId)
    {
        pthread_mutex_unlock(mutex);
        NS_LOG (ERROR, "Failed to Delete Cache");
        return NS_ERROR;
    }

    NSCacheElement * prev = list->head;
    NSCacheElement * del = list->head;

    if (NSConsumerCompareIdCacheData(type, del->data, delId))
    {
        if (del == list->head)
        {
            if (del == list->tail)
                list->tail = del->next;
            list->head = del->next;

            if (type == NS_CONSUMER_CACHE_MESSAGE)
            {
                NSRemoveMessage((NSMessage_consumer *) del->data);
            }
            else if (type == NS_CONSUMER_CACHE_PROVIDER)
            {
                NSRemoveProvider((NSProvider_internal *) del->data);
            }
            NSOICFree(del);
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
            if (type == NS_CONSUMER_CACHE_MESSAGE)
            {
                NSRemoveMessage((NSMessage_consumer *) del->data);
            }
            else if (type == NS_CONSUMER_CACHE_PROVIDER)
            {
                NSRemoveProvider((NSProvider_internal *) del->data);
            }
            NSOICFree(del);
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
    char msgId[NS_DEVICE_ID_LENGTH] = {0, };
    snprintf(msgId, NS_DEVICE_ID_LENGTH, "%lld", (long long int)newMsgObj->messageId);
    NSCacheElement * it = NSStorageRead(list, msgId);
    pthread_mutex_lock(mutex);

    if (it)
    {
        NSMessage_consumer * msgObj = (NSMessage_consumer *) it->data;
        if(msgObj->type == newMsgObj->type)
        {
            NS_LOG (DEBUG, "Already receive message");
            pthread_mutex_unlock(mutex);
            return NS_ERROR;
        }


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

NSResult NSConsumerCacheWriteProvider(NSCacheList * list, NSCacheElement * newObj)
{
    pthread_mutex_t * mutex = NSGetCacheMutex();

    pthread_mutex_lock(mutex);

    NS_LOG (DEBUG, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1");
    NSProvider_internal * prov = (NSProvider_internal *)newObj->data;
    NS_LOG_V (DEBUG, "%s", prov->providerId);
    NS_LOG (DEBUG, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1");

    if (!newObj)
    {
        pthread_mutex_unlock(mutex);
        NS_LOG (ERROR, "Failed to Write Provider Cache");
        return NS_ERROR;
    }

    NSProvider_internal * newProvObj = (NSProvider_internal *) newObj->data;

    pthread_mutex_unlock(mutex);
    NSCacheElement * it = NSStorageRead(list, newProvObj->providerId);
    pthread_mutex_lock(mutex);

    if (it)
    {
        NSProvider_internal * provObj = (NSProvider_internal *) it->data;

        NSProviderConnectionInfo * infos = provObj->connection;
        NSProviderConnectionInfo * lastConn = infos->next;
        while(lastConn)
        {
            infos = lastConn;
            lastConn = lastConn->next;
        }
        infos->next = NSCopyProviderConnections(newProvObj->connection);

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
    NS_LOG_V(DEBUG, "New Object address : %s:%d", newProvObj->connection->addr->addr, newProvObj->connection->addr->port);
    obj->data = (void *) NSCopyProvider(newProvObj);

    NS_LOG (DEBUG, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!2");
    prov = (NSProvider_internal *)obj->data;
    NS_LOG_V (DEBUG, "%s", prov->providerId);
    NS_LOG (DEBUG, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!2");

    if (!obj->data)
    {
        NS_LOG (ERROR, "Failed to CopyProvider");
        pthread_mutex_unlock(mutex);

        return NS_ERROR;
    }
    obj->next = NULL;

    if (!list->head)
    {
        list->head = obj;
        list->tail = obj;

        NS_LOG (DEBUG, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!3");
        prov = (NSProvider_internal *)list->tail->data;
        NS_LOG_V (DEBUG, "%s", prov->providerId);
        NS_LOG (DEBUG, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!3");

        pthread_mutex_unlock(mutex);

        return NS_OK;
    }

    (list->tail)->next = obj;
    list->tail = obj;

    NS_LOG (DEBUG, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!4");
    prov = (NSProvider_internal *)list->tail->data;
    NS_LOG_V (DEBUG, "%s", prov->providerId);
    NS_LOG (DEBUG, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!4");

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
            NSOICFree(iter);

            iter = next;
        }

        NSOICFree(list);
    }
    else if (type == NS_CONSUMER_CACHE_PROVIDER)
    {
        while (iter)
        {
            next = (NSCacheElement *) iter->next;

            NSRemoveProvider((NSProvider_internal *) iter->data);
            NSOICFree(iter);

            iter = next;
        }

        NSOICFree(list);
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

        char msgId[NS_DEVICE_ID_LENGTH] = {0, };
        snprintf(msgId, NS_DEVICE_ID_LENGTH, "%lld", (long long int)msg->messageId);
        if (!strcmp(msgId, id))
        {
            return true;
        }

        return false;
    }
    else if (type == NS_CONSUMER_CACHE_PROVIDER)
    {
        NSProvider_internal * prov = (NSProvider_internal *) data;

        if (!strcmp(prov->providerId, id))
        {
            return true;
        }

        return false;
    }

    return false;
}

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
#include "NSProviderMemoryCache.h"

NSCacheList * NSCacheCreate()
{
    pthread_mutex_lock(&NSCacheMutex);
    NSCacheList * newList = (NSCacheList *) OICMalloc(sizeof(NSCacheList));
    if (!newList)
    {
        pthread_mutex_unlock(&NSCacheMutex);
        return NULL;
    }

    newList->head = newList->tail = NULL;

    pthread_mutex_unlock(&NSCacheMutex);

    return newList;
}

NSCacheElement * NSCacheRead(NSCacheList * list, const char * findId)
{
    pthread_mutex_lock(&NSCacheMutex);
    NSCacheElement * iter = list->head;
    NSCacheElement * next = NULL;
    NSCacheType type = list->cacheType;

    printf("NS_ findId = %s\n", findId);

    while (iter)
    {
        next = iter->next;

        printf("NS_ findId2 = %s\n", findId);

        if (NSProviderCompareIdCacheData(type, iter->data, findId))
        {
            pthread_mutex_unlock(&NSCacheMutex);
            return iter;
        }

        iter = next;
    }

    pthread_mutex_unlock(&NSCacheMutex);

    return NULL;
}

NSResult NSCacheUpdateSubScriptionState(NSCacheList * list, NSCacheSubData * subData)
{
    pthread_mutex_lock(&NSCacheMutex);

    printf("NS_ NSCacheUpdateSubScriptionState\n");

    if (subData == NULL)
    {
        printf("NS_ NSCacheWrite - newObj is NULL\n");
        pthread_mutex_unlock(&NSCacheMutex);
        return NS_ERROR;
    }

    printf("NS_ findId0 - 0 = %s\n", subData->id);
    pthread_mutex_unlock(&NSCacheMutex);
    NSCacheElement * it = NSCacheRead(list, subData->id);
    pthread_mutex_lock(&NSCacheMutex);
    printf("NS_ findId0 -1 = %s\n", subData->id);
    if (it)
    {
        printf("NS_ findId1 = %s\n", subData->id);
        NSCacheSubData * itData = (NSCacheSubData *) it->data;
        printf("NS_ findId2 = %s\n", subData->id);
        if (strcmp(itData->id, subData->id) == 0)
        {
            printf("NS_ findId3_subdata_id = %s\n", subData->id);
            printf("NS_ findId3_subdata_messageobId = %d\n", subData->messageObId);
            printf("NS_ findId3_subdata_syncobId = %d\n", subData->syncObId);
            printf("NS_ findId3_subdata_isWhite = %d\n", subData->isWhite);

            printf("NS_ findId3_itdata_id = %s\n", itData->id);
            printf("NS_ findId3_itdata_messageobId = %d\n", itData->messageObId);
            printf("NS_ findId3_itdata_isWhite = %d\n", itData->isWhite);

            itData->isWhite = subData->isWhite;

            printf("_NS : PROVIDER_CACHE_SUBSCRIBER UPDATE\n");

            pthread_mutex_unlock(&NSCacheMutex);
            return NS_OK;

        }
    }
    pthread_mutex_unlock(&NSCacheMutex);
    return NS_OK;
}

NSResult NSCacheWrite(NSCacheList * list, NSCacheElement * newObj)
{
    pthread_mutex_lock(&NSCacheMutex);

    NSCacheType type = list->cacheType;

    printf("NS_ NSCacheWrite\n");

    if (newObj == NULL)
    {
        printf("NS_ NSCacheWrite - newObj is NULL\n");
        pthread_mutex_unlock(&NSCacheMutex);
        return NS_ERROR;
    }

    if (type == NS_PROVIDER_CACHE_SUBSCRIBER)
    {
        NSCacheSubData * subData = (NSCacheSubData *) newObj->data;

        printf("NS_ findId0 - 0 = %s\n", subData->id);
        pthread_mutex_unlock(&NSCacheMutex);
        NSCacheElement * it = NSCacheRead(list, subData->id);
        pthread_mutex_lock(&NSCacheMutex);
        printf("NS_ findId0 -1 = %s\n", subData->id);
        if (it)
        {
            printf("NS_ findId1 = %s\n", subData->id);
            NSCacheSubData * itData = (NSCacheSubData *) it->data;
            printf("NS_ findId2 = %s\n", subData->id);
            if (strcmp(itData->id, subData->id) == 0)
            {
                printf("NS_ findId3_subdata_id = %s\n", subData->id);
                printf("NS_ findId3_subdata_messageobId = %d\n", subData->messageObId);
                printf("NS_ findId3_subdata_syncobId = %d\n", subData->syncObId);
                printf("NS_ findId3_subdata_isWhite = %d\n", subData->isWhite);

                printf("NS_ findId3_itdata_id = %s\n", itData->id);
                printf("NS_ findId3_itdata_messageobId = %d\n", itData->messageObId);
                printf("NS_ findId3_itdata_isWhite = %d\n", itData->isWhite);
                if (itData->messageObId == 0)
                {
                    itData->messageObId = subData->messageObId;
                }

                if (itData->syncObId == 0)
                {
                    itData->syncObId = subData->syncObId;
                }

                //itData->isWhite = subData->isWhite;

                printf("_NS : PROVIDER_CACHE_SUBSCRIBER UPDATE\n");

                pthread_mutex_unlock(&NSCacheMutex);
                return NS_OK;

            }
        }

    }
    else if (type == NS_PROVIDER_CACHE_MESSAGE)
    {
        NSCacheMsgData * msgData = (NSCacheMsgData *) newObj->data;

        NSCacheElement * it = NSCacheRead(list, msgData->id);
        if (it)
        {
            NSCacheMsgData * itData = (NSCacheMsgData *) it->data;

            if (strcmp(itData->id, msgData->id) == 0)
            {

                itData->messageType = msgData->messageType;
                printf("_NS : PROVIDER_CACHE_MESSAGE UPDATE\n");
                pthread_mutex_unlock(&NSCacheMutex);
                return NS_OK;

            }
        }
    }

    printf("NS_ berfore list->head == NULL\n");
    if (list->head == NULL)
    {
        list->head = list->tail = newObj;
        printf("NS_ list->head == NULL\n");
        pthread_mutex_unlock(&NSCacheMutex);
        return NS_OK;
    }

    list->tail = list->tail->next = newObj;
    printf("NS_ list->head == not NULL\n");
    pthread_mutex_unlock(&NSCacheMutex);
    return NS_OK;
}

NSResult NSCacheDelete(NSCacheList * list, const char * delId)
{
    pthread_mutex_lock(&NSCacheMutex);
    NSCacheElement * prev = list->head;
    NSCacheElement * del = list->head;

    NSCacheType type = list->cacheType;

    if (NSProviderCompareIdCacheData(type, del->data, delId))
    {
        if (del == list->head) // first object
        {
            if (del == list->tail) // first object (one object)
                list->tail = del->next;

            list->head = del->next;

            NSProviderDeleteCacheData(type, del->data);
            OICFree(del);
            pthread_mutex_unlock(&NSCacheMutex);
            return NS_OK;
        }
    }

    del = del->next;
    while (del)
    {
        if (NSProviderCompareIdCacheData(type, del->data, delId))
        {
            if (del == list->tail) // delete object same to last object
                list->tail = prev;

            prev->next = del->next;
            NSProviderDeleteCacheData(type, del->data);
            OICFree(del);
            pthread_mutex_unlock(&NSCacheMutex);
            return NS_OK;
        }

        prev = del;
        del = del->next;
    }
    pthread_mutex_unlock(&NSCacheMutex);
    return NS_OK;
}

NSResult NSCacheDestroy(NSCacheList * list)
{
    NSCacheElement * iter = list->head;
    NSCacheElement * next = NULL;

    NSCacheType type = list->cacheType;

    while (iter)
    {
        next = (NSCacheElement *) iter->next;

        NSProviderDeleteCacheData(type, iter->data);
        OICFree(iter);

        iter = next;
    }

    OICFree(list);

    return NS_OK;
}

bool NSProviderCompareIdCacheData(NSCacheType type, void * data, const char * id)
{
    if (data == NULL)
    {
        return false;
    }

    if (type == NS_PROVIDER_CACHE_SUBSCRIBER)
    {
        NSCacheSubData * subData = (NSCacheSubData *) data;

        printf("NS_ subdata->id = %s\n", subData->id);
        printf("NS_ id = %s\n", id);

        if (strcmp(subData->id, id) == 0)
        {
            return true;
        }
        return false;

    }
    else if (type == NS_PROVIDER_CACHE_MESSAGE)
    {
        NSCacheMsgData * msgData = (NSCacheMsgData *) data;

        if (strcmp(msgData->id, id) == 0)
        {
            return true;
        }

        return false;
    }

    return false;
}

NSResult NSProviderDeleteCacheData(NSCacheType type, void * data)
{
    if (data == NULL)
    {
        return NS_OK;
    }

    if (type == NS_PROVIDER_CACHE_SUBSCRIBER)
    {
        NSCacheSubData * subData = (NSCacheSubData *) data;
        if (subData->id)
        {
            OICFree(subData->id);
            subData->id = NULL;
        }

        OICFree(subData);

        return NS_OK;
    }
    else if (type == NS_PROVIDER_CACHE_MESSAGE)
    {
        NSCacheMsgData * msgData = (NSCacheMsgData *) data;

        if (msgData->id)
        {
            OICFree(msgData->id);
            msgData->id = NULL;
        }

        if (msgData->nsMessage)
        {
            NSFreeMessage(msgData->nsMessage);
        }

        OICFree(msgData);

        return NS_OK;
    }

    return NS_OK;
}


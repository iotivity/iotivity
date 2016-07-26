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
#include <string.h>

NSCacheList * NSStorageCreate()
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
    NS_LOG(DEBUG, "NSCacheCreate");

    return newList;
}

NSCacheElement * NSStorageRead(NSCacheList * list, const char * findId)
{
    pthread_mutex_lock(&NSCacheMutex);

    NS_LOG(DEBUG, "NSCacheRead - IN");

    NSCacheElement * iter = list->head;
    NSCacheElement * next = NULL;
    NSCacheType type = list->cacheType;

    NS_LOG_V(DEBUG, "Find ID - %s", findId);

    while (iter)
    {
        next = iter->next;

        if (NSProviderCompareIdCacheData(type, iter->data, findId))
        {
            NS_LOG(DEBUG, "Found in Cache");
            pthread_mutex_unlock(&NSCacheMutex);
            return iter;
        }

        iter = next;
    }

    NS_LOG(DEBUG, "Not found in Cache");
    NS_LOG(DEBUG, "NSCacheRead - OUT");
    pthread_mutex_unlock(&NSCacheMutex);

    return NULL;
}

NSResult NSCacheUpdateSubScriptionState(NSCacheList * list, char * id, bool state)
{
    pthread_mutex_lock(&NSCacheMutex);

    NS_LOG(DEBUG, "NSCacheUpdateSubScriptionState - IN");

    if (id == NULL)
    {
        NS_LOG(DEBUG, "id is NULL");
        pthread_mutex_unlock(&NSCacheMutex);
        return NS_ERROR;
    }

    NSCacheElement * it = NSStorageRead(list, id);

    if (it)
    {
        NSCacheSubData * itData = (NSCacheSubData *) it->data;
        if (strcmp(itData->id, id) == 0)
        {
            NS_LOG(DEBUG, "Update Data - IN");

            NS_LOG_V(DEBUG, "currData_ID = %s", itData->id);
            NS_LOG_V(DEBUG, "currData_MsgObID = %d", itData->messageObId);
            NS_LOG_V(DEBUG, "currData_SyncObID = %d", itData->syncObId);
            NS_LOG_V(DEBUG, "currData_Cloud_MsgObID = %d", itData->remote_messageObId);
            NS_LOG_V(DEBUG, "currData_Cloud_SyncObID = %d", itData->remote_syncObId);
            NS_LOG_V(DEBUG, "currData_IsWhite = %d", itData->isWhite);

            NS_LOG_V(DEBUG, "update state = %d", state);

            itData->isWhite = state;

            NS_LOG(DEBUG, "Update Data - OUT");
            pthread_mutex_unlock(&NSCacheMutex);
            return NS_OK;
        }
    }
    else
    {
        NS_LOG(DEBUG, "Not Found Data");
    }

    NS_LOG(DEBUG, "NSCacheUpdateSubScriptionState - OUT");
    pthread_mutex_unlock(&NSCacheMutex);
    return NS_ERROR;
}

NSResult NSStorageWrite(NSCacheList * list, NSCacheElement * newObj)
{
    pthread_mutex_lock(&NSCacheMutex);

    NSCacheType type = list->cacheType;

    NS_LOG(DEBUG, "NSCacheWrite - IN");

    if (newObj == NULL)
    {
        NS_LOG(DEBUG, "newObj is NULL - IN");
        pthread_mutex_unlock(&NSCacheMutex);
        return NS_ERROR;
    }

    if (type == NS_PROVIDER_CACHE_SUBSCRIBER)
    {
        NS_LOG(DEBUG, "Type is SUBSCRIBER");

        NSCacheSubData * subData = (NSCacheSubData *) newObj->data;
        NSCacheElement * it = NSStorageRead(list, subData->id);

        if (it)
        {
            NSCacheSubData * itData = (NSCacheSubData *) it->data;

            if (strcmp(itData->id, subData->id) == 0)
            {
                NS_LOG(DEBUG, "Update Data - IN");

                NS_LOG_V(DEBUG, "currData_ID = %s", itData->id);
                NS_LOG_V(DEBUG, "currData_MsgObID = %d", itData->messageObId);
                NS_LOG_V(DEBUG, "currData_SyncObID = %d", itData->syncObId);
                NS_LOG_V(DEBUG, "currData_Cloud_MsgObID = %d", itData->remote_messageObId);
                NS_LOG_V(DEBUG, "currData_Cloud_SyncObID = %d", itData->remote_syncObId);
                NS_LOG_V(DEBUG, "currData_IsWhite = %d", itData->isWhite);

                NS_LOG_V(DEBUG, "subData_ID = %s", subData->id);
                NS_LOG_V(DEBUG, "subData_MsgObID = %d", subData->messageObId);
                NS_LOG_V(DEBUG, "subData_SyncObID = %d", subData->syncObId);
                NS_LOG_V(DEBUG, "subData_Cloud_MsgObID = %d", subData->remote_messageObId);
                NS_LOG_V(DEBUG, "subData_Cloud_SyncObID = %d", subData->remote_syncObId);
                NS_LOG_V(DEBUG, "subData_IsWhite = %d", subData->isWhite);

                if (subData->messageObId != 0)
                {
                    itData->messageObId = subData->messageObId;
                }

                if (subData->syncObId != 0)
                {
                    itData->syncObId = subData->syncObId;
                }

                if (subData->remote_messageObId != 0)
                {
                    itData->remote_messageObId = subData->remote_messageObId;
                }

                if (subData->remote_syncObId != 0)
                {
                    itData->remote_syncObId = subData->remote_syncObId;
                    NS_LOG_V(DEBUG, "sync id cached: %d", itData->remote_syncObId);
                }

                NS_LOG(DEBUG, "Update Data - OUT");

                pthread_mutex_unlock(&NSCacheMutex);
                return NS_OK;
            }
        }

    }
    else if (type == NS_PROVIDER_CACHE_MESSAGE)
    {
        NS_LOG(DEBUG, "Type is MESSAGE");

        NSCacheMsgData * msgData = (NSCacheMsgData *) newObj->data;
        NSCacheElement * it = NSStorageRead(list, msgData->id);

        if (it)
        {
            NSCacheMsgData * itData = (NSCacheMsgData *) it->data;

            if (strcmp(itData->id, msgData->id) == 0)
            {

                itData->messageType = msgData->messageType;
                NS_LOG(DEBUG, "Updated messageType");
                pthread_mutex_unlock(&NSCacheMutex);
                return NS_OK;

            }
        }
    }

    if (list->head == NULL)
    {
        NS_LOG(DEBUG, "list->head is NULL, Insert First Data");
        list->head = list->tail = newObj;
        pthread_mutex_unlock(&NSCacheMutex);
        return NS_OK;
    }

    list->tail = list->tail->next = newObj;
    NS_LOG(DEBUG, "list->head is not NULL");
    pthread_mutex_unlock(&NSCacheMutex);
    return NS_OK;
}

NSResult NSStorageDelete(NSCacheList * list, const char * delId)
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
            {
                list->tail = del->next;
            }

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
            {
                list->tail = prev;
            }

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

NSResult NSStorageDestroy(NSCacheList * list)
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
    NS_LOG(DEBUG, "NSProviderCompareIdCacheData - IN");

    if (data == NULL)
    {
        return false;
    }

    if (type == NS_PROVIDER_CACHE_SUBSCRIBER)
    {
        NSCacheSubData * subData = (NSCacheSubData *) data;

        NS_LOG_V(DEBUG, "Data(subData) = [%s]", subData->id);
        NS_LOG_V(DEBUG, "Data(compData) = [%s]", id);

        if (strcmp(subData->id, id) == 0)
        {
            NS_LOG(DEBUG, "SubData is Same");
            return true;
        }

        NS_LOG(DEBUG, "Message Data is Not Same");
        return false;

    }
    else if (type == NS_PROVIDER_CACHE_MESSAGE)
    {
        NSCacheMsgData * msgData = (NSCacheMsgData *) data;

        if (strcmp(msgData->id, id) == 0)
        {
            NS_LOG(DEBUG, "Message Data is Same");
            return true;
        }

        NS_LOG(DEBUG, "Message Data is Not Same");
        return false;
    }

    NS_LOG(DEBUG, "NSProviderCompareIdCacheData - OUT");

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

        (subData->id)[0] = '\0';
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

bool NSIsSameObId(NSCacheSubData * data, OCObservationId id)
{
    if (id == data->messageObId || id == data->syncObId || id == data->remote_messageObId ||
                id == data->remote_syncObId)
    {
        return true;
    }

    return false;
}

NSResult NSProviderDeleteSubDataFromObId(NSCacheList * list, OCObservationId id)
{
    pthread_mutex_lock(&NSCacheMutex);
    NSCacheElement * prev = list->head;
    NSCacheElement * del = list->head;

    NSCacheType type = list->cacheType;

    bool isDelete = true;

    while(isDelete)
    {
        NSCacheSubData * curr = (NSCacheSubData *)del->data;

        isDelete = false;

        if (NSIsSameObId(curr, id))
        {
            if (del == list->head) // first object
            {
                if (del == list->tail) // first object (one object)
                {
                    list->tail = del->next;
                }

                list->head = del->next;

                NSProviderDeleteCacheData(type, del->data);
                OICFree(del);
                isDelete = true;
            }
        }
        else
        {
            del = del->next;
            while (del)
            {
                if (NSIsSameObId(curr, id))
                {
                    if (del == list->tail) // delete object same to last object
                    {
                        list->tail = prev;
                    }

                    prev->next = del->next;
                    NSProviderDeleteCacheData(type, del->data);
                    OICFree(del);
                    isDelete = true;
                    break;
                }

                prev = del;
                del = del->next;
            }
        }
    }
    pthread_mutex_unlock(&NSCacheMutex);
    return NS_OK;
}

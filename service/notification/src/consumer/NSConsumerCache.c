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

#include "NSConsumerCache.h"
#include "NSConsumerCommon.h"
#include "NSConstants.h"

#include "stdlib.h"
#include "oic_malloc.h"
#include "oic_string.h"

NSCacheList * NSConsumerCacheInit()
{
    NSCacheList * retNSCacheList = (NSCacheList *)OICMalloc(sizeof(NSCacheList));
    if (!retNSCacheList)
    {
        return NULL;
    }

    retNSCacheList->head = NULL;
    retNSCacheList->tail = NULL;

    return retNSCacheList;
}

NSResult NSConsumerCacheDestroy(NSCacheList * list)
{
    NSCacheObject * iter = list->head;

    NSCacheObject * next = NULL;
    while (iter)
    {
        next = (NSCacheObject *)iter->next;
        if (NS_OK != NSConsumerCacheDelete(list, iter))
        {
            return NS_ERROR;
        }
        iter = next;
    }

    OICFree((NSCacheList *) list);

    return NS_OK;
}

NSResult NSConsumerCacheInsert(NSCacheList * list, NSCacheObject * newObj)
{
    NSCacheObject * obj = (NSCacheObject *)OICMalloc(sizeof(NSCacheObject));
    if (!obj)
    {
        NS_LOG(ERROR, "Fail to Create New Object");
        return NS_ERROR;
    }

    NSMessage_consumer * msgObj = (NSMessage_consumer *) OICMalloc(sizeof(NSMessage_consumer));
    NSMessage_consumer * newMsgObj = (NSMessage_consumer *) newObj->data;

    NS_LOG_V(DEBUG, "Title: %s", newMsgObj->mTitle);
    NS_LOG_V(DEBUG, "ID: %s", newMsgObj->mId);
    NS_LOG_V(DEBUG, "TEXT: %s", newMsgObj->mContentText);

    msgObj->mTitle = OICStrdup(newMsgObj->mTitle);
    msgObj->mId = OICStrdup(newMsgObj->mId);
    msgObj->mContentText = OICStrdup(newMsgObj->mContentText);

    if (!msgObj->mId)
    {
        NS_LOG(ERROR, "Notification Attributes copy fail");
    }

    msgObj->addr = (OCDevAddr *)OICMalloc(sizeof(OCDevAddr));
    if (!msgObj->addr)
    {
        NS_LOG(ERROR, "OCDevAddr allocation is failed.");
    }

    memcpy(msgObj->addr, newMsgObj->addr, sizeof(OCDevAddr));
    msgObj->type = newMsgObj->type;

    //obj->data = (NSCacheData *)OICMalloc(sizeof(NSCacheData));
    obj->data = (NSCacheData *) msgObj;
    //memcpy(obj->data, (NSCacheData *)msgObj, sizeof(NSCacheData));
    obj->next = NULL;

    msgObj = NULL;
    msgObj = (NSMessage_consumer *) obj->data;

    if (!list->head)
    {
        list->head = obj;
        return NS_OK;
    }

    NSCacheObject * iter = list->head;
    while (iter)
    {
        if (iter == obj)
        {
            return NS_ERROR;
        }
        iter = (NSCacheObject *) iter->next;
    }

    if (list->tail)
    {
        (list->tail)->next = obj;
    }

    list->tail = obj;

    return NS_OK;
}

NSResult NSConsumerCacheDelete(NSCacheList * list, NSCacheObject * delObj) // Free?
{
    NSCacheObject * beDelete;
    if (list->head == delObj)
    {
        beDelete = list->head;
        list->head = (NSCacheObject *) list->head->next;
        return NS_OK;
    }

    NSCacheObject * pre = list->head;
    beDelete = (NSCacheObject *) pre->next;
    while (beDelete)
    {
        if (beDelete == delObj)
        {
            if (beDelete == list->tail)
            {
                pre->next = NULL;
                list->tail = pre;
            }
            else
            {
                pre->next = beDelete->next;
            }
            OICFree((NSCacheObject *) beDelete);
            break;
        }
        pre = beDelete;
        beDelete = (NSCacheObject *) beDelete->next;
    }

    return NS_OK;
}

NSCacheObject * NSConsumerCacheFind(NSCacheList * list, char * findId)
{

    NS_LOG_V(DEBUG, "findID: %s", findId);
    NSCacheObject * retObj = list->head;
    if (!retObj)
    {
        NS_LOG(DEBUG, "findcache is null");
        return retObj;
    }

    NSMessage_consumer * obj = (NSMessage_consumer *) retObj->data;
    NS_LOG_V(DEBUG, "objmID: %s", obj->mId);

    while (retObj)
    {
        NSMessage_consumer * obj = (NSMessage_consumer *) retObj->data;
        NS_LOG_V(DEBUG, "objmID: %s", obj->mId);
        if (!strcmp(obj->mId, findId))
        {
            return retObj;
        }
        retObj = (NSCacheObject *) retObj->next;
    }

    return retObj;
}

NSResult NSConsumerCacheWrite(NSCacheList * list, NSCacheObject * writeObj)
{
    NSCacheObject * beModify = NSConsumerCacheFind(list, ((NSMessage_consumer *) writeObj->data)->mId);
    if (!beModify)
    {
        return NS_ERROR;
    }

    NSMessage_consumer * msgObj = (NSMessage_consumer *) beModify->data;
    NSMessage_consumer * msgWriteObj = (NSMessage_consumer *) writeObj->data;

    if (msgWriteObj->mId)
    {
        strcpy(msgObj->mId, msgWriteObj->mId);
    }
    if (msgWriteObj->mTitle)
    {
        strcpy(msgObj->mTitle, msgWriteObj->mTitle);
    }
    if (msgWriteObj->mContentText)
    {
        strcpy(msgObj->mContentText, msgWriteObj->mContentText);
    }

    memcpy(msgObj->addr, msgWriteObj->addr, sizeof(OCDevAddr));
    msgObj->type = msgWriteObj->type;

    return NS_OK;
}

NSCacheObject * NSConsumerCacheRead(NSCacheList * list, NSCacheObject * readObj)
{
    NSCacheObject * retObj = NSConsumerCacheFind(list, ((NSMessage_consumer *) readObj->data)->mId);
    if (!retObj)
    {
        return NULL;
    }

    return retObj;
}




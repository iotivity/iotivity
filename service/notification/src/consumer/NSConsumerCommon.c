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

#include "NSConsumerCommon.h"
#include "NSConstants.h"
#include "NSThread.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "ocpayload.h"

#include <pthread.h>

pthread_mutex_t ** NSGetStackMutex()
{
    static pthread_mutex_t * g_stackMutext = NULL;
    if (g_stackMutext == NULL)
    {
        g_stackMutext = (pthread_mutex_t *)OICMalloc(sizeof(pthread_mutex_t));
        NS_VERIFY_NOT_NULL(g_stackMutext, NULL);
        pthread_mutex_init(g_stackMutext, NULL);
    }

    return & g_stackMutext;
}

char ** NSGetConsumerId()
{
    static char * g_consumerId = NULL;
    return & g_consumerId;
}

void NSSetConsumerId(char * cId)
{
    NS_VERIFY_NOT_NULL_V(cId);
    char ** consumerId = NSGetConsumerId();
    NSOICFree(*consumerId);
    *consumerId = (char *)OICMalloc(sizeof(char) * NS_DEVICE_ID_LENGTH);
    NS_VERIFY_NOT_NULL_V(*consumerId);

    OICStrcpy(*consumerId, sizeof(char) * NS_DEVICE_ID_LENGTH, cId);
}

char * NSMakeRequestUriWithConsumerId(const char * uri)
{
    NS_VERIFY_NOT_NULL(uri, NULL);

    char * consumerId = OICStrdup(*NSGetConsumerId());
    NS_VERIFY_NOT_NULL(consumerId, NULL);

    size_t uriLen = strlen(uri) + 1;
    size_t qKeyLen = sizeof(NS_QUERY_CONSUMER_ID);
    size_t queryLen = NS_DEVICE_ID_LENGTH + uriLen + qKeyLen + 2;

    char * retQuery = (char *)OICMalloc(sizeof(char) * queryLen);
    NS_VERIFY_NOT_NULL(retQuery, NULL);

    size_t index = 0;
    OICStrcpy((retQuery + index), uriLen, uri);
    index += uriLen - 1;
    OICStrcpy((retQuery + index), 2, "?");
    index += 1;
    OICStrcpy((retQuery + index), qKeyLen, NS_QUERY_CONSUMER_ID);
    index += qKeyLen - 1;
    OICStrcpy((retQuery + index), 2, "=");
    index += 1;
    OICStrcpy((retQuery + index), NS_DEVICE_ID_LENGTH, consumerId);

    NSOICFree(consumerId);

    return retQuery;
}

bool * NSGetBoneIsStartedConsumer()
{
    static bool g_isStartedConsumer = false;

    return & g_isStartedConsumer;
}

void NSSetIsStartedConsumer(bool setValue)
{
    * NSGetBoneIsStartedConsumer() = setValue;

    if (setValue == false)
    {
        pthread_mutex_destroy(*NSGetStackMutex());
        NSOICFree(*NSGetStackMutex());
        *NSGetStackMutex() = NULL;

        NSOICFree(*NSGetConsumerId());
        *NSGetConsumerId() = NULL;
    }
}

bool NSIsStartedConsumer()
{
    return * NSGetBoneIsStartedConsumer();
}

NSProviderStateCallback * NSGetProviderChangedCb()
{
    static NSProviderStateCallback g_changedCb = NULL;

    return & g_changedCb;
}

void NSSetProviderChangedCb(NSProviderStateCallback cb)
{
    *(NSGetProviderChangedCb()) = cb;
}

typedef struct
{
    NSProvider * provider;
    NSProviderState state;
} NSProviderChangedData;

void * NSProviderChangedFunc(void * obj)
{
    NSProviderChangedData * data = (NSProviderChangedData *) obj;
    (*(NSGetProviderChangedCb()))(data->provider, data->state);
    NSOICFree(data);
    return NULL;
}

void NSProviderChanged(NSProvider * provider, NSProviderState response)
{
    NS_VERIFY_NOT_NULL_V(provider);

    NSProvider * retProvider = NSCopyProvider((NSProvider_internal *) provider);
    NS_VERIFY_NOT_NULL_V(retProvider);

    NSProviderChangedData * data =
            (NSProviderChangedData *)OICMalloc(sizeof(NSProviderChangedData));
    NS_VERIFY_NOT_NULL_WITH_POST_CLEANING_V(data, NSRemoveProvider(retProvider));

    data->provider = retProvider;
    data->state = response;

    NSConsumerThread * thread = NSThreadInit(NSProviderChangedFunc, (void *) data);
    NS_VERIFY_NOT_NULL_WITH_POST_CLEANING_V(thread,
    {
        NSRemoveProvider(retProvider);
        NSOICFree(data);
    });

    NSDestroyThreadHandle(thread);
    NSOICFree(thread);
}

NSSyncInfoReceivedCallback * NSGetBoneNotificationSyncCb()
{
    static NSSyncInfoReceivedCallback g_syncCb = NULL;

    return & g_syncCb;
}

void NSSetNotificationSyncCb(NSSyncInfoReceivedCallback cb)
{
    * NSGetBoneNotificationSyncCb() = cb;
}

void * NSNotificationSyncFunc(void * obj)
{
    (* NSGetBoneNotificationSyncCb())((NSSyncInfo *) obj);
    return NULL;
}

void NSNotificationSync(NSSyncInfo * sync)
{
    NS_VERIFY_NOT_NULL_V(sync);

    NSSyncInfo * retSync = (NSSyncInfo *)OICMalloc(sizeof(NSSyncInfo));
    NS_VERIFY_NOT_NULL_V(retSync);
    memcpy(retSync, sync, sizeof(NSSyncInfo));

    NSConsumerThread * thread = NSThreadInit(NSNotificationSyncFunc, (void *) retSync);
    NS_VERIFY_NOT_NULL_V(thread);

    NSDestroyThreadHandle(thread);
    NSOICFree(thread);
}

NSMessageReceivedCallback  * NSGetBoneMessagePostedCb()
{
    static NSMessageReceivedCallback  g_postCb = NULL;

    return & g_postCb;
}

void NSSetMessagePostedCb(NSMessageReceivedCallback  cb)
{
    * NSGetBoneMessagePostedCb() = cb;
}

NSMessageReceivedCallback  NSGetMessagePostedCb()
{
    return * NSGetBoneMessagePostedCb();
}

void * NSMessagePostFunc(void * obj)
{
    NSGetMessagePostedCb()((NSMessage *) obj);
    return NULL;
}

void NSMessagePost(NSMessage * msg)
{
    NS_VERIFY_NOT_NULL_V(msg);

    NSMessage * retMsg = NSCopyMessage(msg);
    NS_VERIFY_NOT_NULL_V(retMsg);

    NSConsumerThread * thread = NSThreadInit(NSMessagePostFunc, (void *) retMsg);
    NS_VERIFY_NOT_NULL_V(thread);

    NSDestroyThreadHandle(thread);
    NSOICFree(thread);
}

NSTask * NSMakeTask(NSTaskType type, void * data)
{
    NSTask * retTask = OICMalloc(sizeof(NSTask));
    NS_VERIFY_NOT_NULL(retTask, NULL);

    retTask->taskType = type;
    retTask->taskData = data;
    retTask->nextTask = NULL;

    return retTask;
}

NSMessage * NSCopyMessage(NSMessage * msg)
{
    NS_VERIFY_NOT_NULL(msg, NULL);

    NSMessage * newMsg = (NSMessage *)OICMalloc(sizeof(NSMessage));
    NS_VERIFY_NOT_NULL(newMsg, NULL);

    OICStrcpy(newMsg->providerId, NS_DEVICE_ID_LENGTH, msg->providerId);

    newMsg->messageId = msg->messageId;
    newMsg->title = OICStrdup(msg->title);
    newMsg->contentText = OICStrdup(msg->contentText);
    newMsg->sourceName = OICStrdup(msg->sourceName);
    newMsg->dateTime = OICStrdup(msg->dateTime);
    newMsg->type = msg->type;
    newMsg->ttl= msg->ttl;

    newMsg->topic = NULL;
    if (msg->topic && strlen(msg->topic) > 0)
    {
        newMsg->topic = OICStrdup(msg->topic);
    }

    newMsg->mediaContents = NULL;
    if (msg->mediaContents)
    {
        newMsg->mediaContents = (NSMediaContents *)OICMalloc(sizeof(NSMediaContents));
        NS_VERIFY_NOT_NULL_WITH_POST_CLEANING(
                newMsg->mediaContents, NULL, NSRemoveMessage(newMsg));
        newMsg->mediaContents->iconImage =
                (char *)OICMalloc(sizeof(char)*strlen(msg->mediaContents->iconImage) + 1);
        NS_VERIFY_NOT_NULL_WITH_POST_CLEANING(
                newMsg->mediaContents->iconImage, NULL, NSRemoveMessage(newMsg));
        memcpy(newMsg->mediaContents->iconImage, msg->mediaContents->iconImage,
               strlen(msg->mediaContents->iconImage) + 1);
    }

    newMsg->extraInfo = NULL;
    if (msg->extraInfo)
    {
        newMsg->extraInfo = OCRepPayloadClone(msg->extraInfo);
    }

    return newMsg;
}
void NSRemoveMessage(NSMessage * msg)
{
    NS_VERIFY_NOT_NULL_V(msg);

    msg->messageId = 0;
    NSOICFree(msg->title);
    NSOICFree(msg->contentText);
    NSOICFree(msg->sourceName);
    NSOICFree(msg->dateTime);
    NSOICFree(msg->topic);

    if (msg->mediaContents)
    {
        NSOICFree(msg->mediaContents->iconImage);
    }
    NSOICFree(msg->mediaContents);

    if (msg->extraInfo)
    {
        OCRepPayloadDestroy(msg->extraInfo);
        msg->extraInfo = NULL;
    }

    NSOICFree(msg);
}

void NSRemoveConnections(NSProviderConnectionInfo * connections)
{
    NS_VERIFY_NOT_NULL_V(connections);

    NSProviderConnectionInfo * tmp = connections;

    while(tmp)
    {
        tmp->messageHandle = NULL;
        tmp->syncHandle = NULL;
        NSOICFree(tmp->addr);
        NSProviderConnectionInfo * next = tmp->next;
        NSOICFree(tmp);
        tmp = next;
    }
}

NSProviderConnectionInfo * NSCreateProviderConnections(OCDevAddr * inAddr)
{
    NSProviderConnectionInfo * connections
        = (NSProviderConnectionInfo *)OICMalloc(sizeof(NSProviderConnectionInfo));
    NS_VERIFY_NOT_NULL(connections, NULL);

    connections->addr = NULL;
    connections->messageHandle = NULL;
    connections->syncHandle = NULL;
    connections->isCloudConnection = false;
    connections->isSubscribing = false;
    connections->next = NULL;

    if (inAddr)
    {
        connections->addr = (OCDevAddr *)OICMalloc(sizeof(OCDevAddr));
        NS_VERIFY_NOT_NULL_WITH_POST_CLEANING(connections->addr, NULL, NSRemoveConnections(connections));
        memcpy(connections->addr, inAddr, sizeof(OCDevAddr));
    }

    return connections;
}

NSProviderConnectionInfo * NSCopyProviderConnections(NSProviderConnectionInfo * conn)
{
    NS_VERIFY_NOT_NULL(conn, NULL);
    NSProviderConnectionInfo * tmp = conn;

    NSProviderConnectionInfo * retInfo = NSCreateProviderConnections(tmp->addr);
    NS_VERIFY_NOT_NULL(retInfo, NULL);
    retInfo->messageHandle = tmp->messageHandle;
    retInfo->syncHandle = tmp->syncHandle;
    retInfo->isCloudConnection = tmp->isCloudConnection;
    retInfo->isSubscribing = tmp->isSubscribing;

    tmp = tmp->next;
    NSProviderConnectionInfo * copyInfo = retInfo;

    while(tmp)
    {
        NSProviderConnectionInfo * tmpInfo = NSCreateProviderConnections(tmp->addr);
        NS_VERIFY_NOT_NULL(tmpInfo, NULL);

        tmpInfo->messageHandle = tmp->messageHandle;
        tmpInfo->syncHandle = tmp->syncHandle;
        tmpInfo->isCloudConnection = tmp->isCloudConnection;
        tmpInfo->isSubscribing = tmp->isSubscribing;
        tmp = tmp->next;
        copyInfo->next = tmpInfo;
        copyInfo = tmpInfo;
    }

    return retInfo;
}

void NSRemoveTopicNode(NSTopicLL * topicNode)
{
    NS_VERIFY_NOT_NULL_V(topicNode);

    NSOICFree(topicNode->topicName);
    topicNode->next = NULL;

    NSOICFree(topicNode);
}

NSTopicLL * NSCopyTopicNode(NSTopicLL * topicNode)
{
    NS_VERIFY_NOT_NULL(topicNode, NULL);

    NSTopicLL * newTopicNode = (NSTopicLL *)OICMalloc(sizeof(NSTopicLL));
    NS_VERIFY_NOT_NULL(newTopicNode, NULL);

    newTopicNode->topicName = OICStrdup(topicNode->topicName);
    newTopicNode->state = topicNode->state;
    newTopicNode->next = NULL;

    return newTopicNode;
}

NSResult NSInsertTopicNode(NSTopicLL * topicHead, NSTopicLL * topicNode)
{
    NS_VERIFY_NOT_NULL(topicHead, NS_ERROR);
    NS_VERIFY_NOT_NULL(topicNode, NS_ERROR);

    NSTopicLL * iter = topicHead;
    NSTopicLL * prev = NULL;

    while (iter)
    {
        prev = iter;
        iter = (NSTopicLL *) iter->next;
    }

    prev->next = topicNode;
    topicNode->next = NULL;

    return NS_OK;
}

void NSRemoveTopicLL(NSTopicLL * topicHead)
{
    NS_VERIFY_NOT_NULL_V(topicHead);

    NSTopicLL * iter = topicHead;
    NSTopicLL * following = NULL;

    while (iter)
    {
        following = iter->next;

        NSRemoveTopicNode(iter);

        iter = following;
    }
}

NSTopicLL * NSCopyTopicLL(NSTopicLL * topicHead)
{
    NS_VERIFY_NOT_NULL(topicHead, NULL);

    NSTopicLL * iter = topicHead;

    NS_LOG_V(DEBUG, "[NSCopyTopicLL] Name:%s\t State:%d", iter->topicName, iter->state);
    NSTopicLL * newTopicHead = NSCopyTopicNode(iter);
    NS_VERIFY_NOT_NULL_WITH_POST_CLEANING(newTopicHead, NULL, NSRemoveTopicLL(newTopicHead));

    iter = (NSTopicLL *) iter->next;

    while (iter)
    {
        NS_LOG_V(DEBUG, "[NSCopyTopicLL] Name:%s\t State:%d", iter->topicName, iter->state);
        NSTopicLL * newTopicNode = NSCopyTopicNode(iter);
        NS_VERIFY_NOT_NULL_WITH_POST_CLEANING(newTopicNode, NULL, NSRemoveTopicLL(newTopicHead));

        NSResult ret = NSInsertTopicNode(newTopicHead, newTopicNode);
        NS_VERIFY_NOT_NULL_WITH_POST_CLEANING(ret == NS_OK ? (void *)1 : NULL,
                                                    NULL, NSRemoveTopicLL(newTopicHead));

        iter = (NSTopicLL *) iter->next;
    }

    return newTopicHead;
}

void NSCopyProviderPostClean(
        NSProviderConnectionInfo * connections, NSProvider_internal * provider)
{
    NSRemoveConnections(connections);
    NSOICFree(provider);
}

NSProvider_internal * NSCopyProvider_internal(NSProvider_internal * prov)
{
    NS_VERIFY_NOT_NULL(prov, NULL);

    NSProviderConnectionInfo * connections = NSCopyProviderConnections(prov->connection);
    NS_VERIFY_NOT_NULL(connections, NULL);

    NSProvider_internal * newProv = (NSProvider_internal *) OICMalloc(sizeof(NSProvider_internal));
    NS_VERIFY_NOT_NULL_WITH_POST_CLEANING(newProv, NULL, NSRemoveConnections(connections));

    newProv->topicLL = NULL;

    if (prov->topicLL)
    {
        NSTopicLL * newTopicLL = NSCopyTopicLL(prov->topicLL);
        NS_VERIFY_NOT_NULL_WITH_POST_CLEANING(newTopicLL, NULL,
                    NSCopyProviderPostClean(connections, newProv));

        newProv->topicLL = newTopicLL;
    }

    newProv->connection = connections;
    OICStrcpy(newProv->providerId, NS_DEVICE_ID_LENGTH, prov->providerId);
    newProv->messageUri = OICStrdup(prov->messageUri);
    newProv->syncUri = OICStrdup(prov->syncUri);
    newProv->topicUri = OICStrdup(prov->topicUri);
    newProv->accessPolicy = prov->accessPolicy;
    newProv->state = prov->state;

    return newProv;
}

NSProvider * NSCopyProvider(NSProvider_internal * prov)
{
    NS_VERIFY_NOT_NULL(prov, NULL);

    NSProvider * newProv = (NSProvider *) OICMalloc(sizeof(NSProvider));
    NS_VERIFY_NOT_NULL(newProv, NULL);

    OICStrcpy(newProv->providerId, NS_DEVICE_ID_LENGTH, prov->providerId);

    return newProv;
}

void NSRemoveProvider_internal(void * data)
{
    NS_VERIFY_NOT_NULL_V(data);

    NSProvider_internal * prov = (NSProvider_internal *) data;

    NSOICFree(prov->messageUri);
    NSOICFree(prov->syncUri);
    NSOICFree(prov->topicUri);
    NSRemoveConnections(prov->connection);
    if (prov->topicLL)
    {
        NSRemoveTopicLL(prov->topicLL);
    }

    NSOICFree(prov);
}

void NSRemoveProvider(NSProvider * prov)
{
    NS_VERIFY_NOT_NULL_V(prov);
    NSOICFree(prov);
}

OCStackResult NSInvokeRequest(OCDoHandle * handle,
        OCMethod method, const OCDevAddr * addr,
        const char * queryUrl, OCPayload * payload,
        void * callbackFunc, void * callbackData,
        OCClientContextDeleter cd, OCConnectivityType type)
{
    int mutexRet = pthread_mutex_lock(*(NSGetStackMutex()));
    NS_VERIFY_NOT_NULL(mutexRet != 0 ? NULL : (void *)1, OC_STACK_ERROR);

    OCCallbackData cbdata = { NULL, NULL, NULL };

    cbdata.cb = callbackFunc;
    cbdata.context = callbackData;
    cbdata.cd = cd;

    OCStackResult ret = OCDoResource(handle, method, queryUrl, addr,
                                     payload, type, NS_QOS, &cbdata, NULL, 0);

    mutexRet = pthread_mutex_unlock(*(NSGetStackMutex()));
    NS_VERIFY_NOT_NULL(mutexRet != 0 ? NULL : (void *)1, OC_STACK_ERROR);

    return ret;
}

bool NSOCResultToSuccess(OCStackResult ret)
{
    switch (ret)
    {
        case OC_STACK_OK:
        case OC_STACK_RESOURCE_CREATED:
        case OC_STACK_RESOURCE_DELETED:
        case OC_STACK_PRESENCE_STOPPED:
        case OC_STACK_CONTINUE:
        case OC_STACK_RESOURCE_CHANGED:
            return true;
        default:
            NS_LOG_V(DEBUG, "OCStackResult : %d", (int)ret);
            return false;
    }
}

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

#include <pthread.h>

#define NS_QUERY_CONSUMER_ID "consumerid"

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
}

bool NSIsStartedConsumer()
{
    return * NSGetBoneIsStartedConsumer();
}

NSProviderDiscoveredCallback * NSGetBoneDiscoverCb()
{
    static NSProviderDiscoveredCallback g_discoverCb = NULL;

    return & g_discoverCb;
}

void NSSetDiscoverProviderCb(NSProviderDiscoveredCallback cb)
{
    * NSGetBoneDiscoverCb() = cb;
}

NSProviderDiscoveredCallback NSGetDiscoverCb()
{
    return * NSGetBoneDiscoverCb();
}

void * NSDiscoveredProviderFunc(void * provider)
{
    NSGetDiscoverCb()((NSProvider *) provider);

    return NULL;
}

void NSDiscoveredProvider(NSProvider * provider)
{
    NS_VERIFY_NOT_NULL_V(provider);

    NSProvider * retProvider = (NSProvider *)NSCopyProvider((NSProvider_internal *)provider);
    NS_VERIFY_NOT_NULL_V(retProvider);

    NSConsumerThread * thread = NSThreadInit(NSDiscoveredProviderFunc, (void *) retProvider);
    NS_VERIFY_NOT_NULL_V(thread);
}

NSSubscriptionAcceptedCallback * NSGetSubscriptionAcceptedCb()
{
    static NSSubscriptionAcceptedCallback g_acceptCb = NULL;

    return & g_acceptCb;
}

void NSSetSubscriptionAcceptedCb(NSSubscriptionAcceptedCallback cb)
{
    *(NSGetSubscriptionAcceptedCb()) = cb;
}

void NSSubscriptionAccepted(NSProvider * provider)
{
    (*(NSGetSubscriptionAcceptedCb()))(provider);
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

    // TODO change to copy function.
    newMsg->mediaContents = msg->mediaContents;

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

    // TODO change to remove function.
    NSOICFree(msg->mediaContents);

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
        tmp = tmp->next;
    }

    NSOICFree(connections);
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
    tmp = tmp->next;
    NSProviderConnectionInfo * copyInfo = retInfo;

    while(tmp)
    {
        copyInfo = NSCreateProviderConnections(tmp->addr);
        NS_VERIFY_NOT_NULL(copyInfo, NULL);

        copyInfo->messageHandle = tmp->messageHandle;
        copyInfo->syncHandle = tmp->syncHandle;
        copyInfo->isCloudConnection = tmp->isCloudConnection;
        copyInfo->isSubscribing = tmp->isSubscribing;
        tmp = tmp->next;
        copyInfo = copyInfo->next;
    }

    return retInfo;
}

NSProvider_internal * NSCopyProvider(NSProvider_internal * prov)
{
    NS_VERIFY_NOT_NULL(prov, NULL);

    NSProviderConnectionInfo * connections = NSCopyProviderConnections(prov->connection);
    NS_VERIFY_NOT_NULL(connections, NULL);

    NSProvider_internal * newProv = (NSProvider_internal *)OICMalloc(sizeof(NSProvider_internal));
    NS_VERIFY_NOT_NULL_WITH_POST_CLEANING(newProv, NULL, NSRemoveConnections(connections));

    newProv->connection = connections;
    OICStrcpy(newProv->providerId, NS_DEVICE_ID_LENGTH, prov->providerId);
    newProv->messageUri = OICStrdup(prov->messageUri);
    newProv->syncUri = OICStrdup(prov->syncUri);
    newProv->accessPolicy = prov->accessPolicy;

    return newProv;
}
void NSRemoveProvider(NSProvider_internal * prov)
{
    NS_VERIFY_NOT_NULL_V(prov);

    NSOICFree(prov->messageUri);
    NSOICFree(prov->syncUri);
    NSRemoveConnections(prov->connection);

    NSOICFree(prov);
}

OCStackResult NSInvokeRequest(OCDoHandle * handle,
        OCMethod method, const OCDevAddr * addr,
        const char * queryUrl, OCPayload * payload,
        void * callbackFunc, void * callbackData, OCConnectivityType type)
{
    int mutexRet = pthread_mutex_lock(*(NSGetStackMutex()));
    NS_VERIFY_NOT_NULL(mutexRet != 0 ? NULL : (void *)1, OC_STACK_ERROR);

    OCCallbackData cbdata = { 0, };

    cbdata.cb = callbackFunc;
    cbdata.context = callbackData;
    cbdata.cd = NULL;

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
        case OC_STACK_CONTINUE:
        case OC_STACK_RESOURCE_CHANGED:
            return true;
        default:
            return false;
    }
}

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

#include "NSProviderTopic.h"

static bool isTopicList = false;

NSResult NSInitTopicStorage()
{
    NS_LOG(DEBUG, "NSInitTopicList - IN");

    if(isTopicList)
    {
        NS_LOG(DEBUG, "topic list has already initiated");
        return NS_FAIL;
    }

    topicStorage = NSStorageCreate();
    topicStorage->cacheType = NS_PROVIDER_CACHE_TOPIC;
    isTopicList = true;

    NS_LOG(DEBUG, "NSInitTopicList - OUT");
    return NS_OK;
}

NSTopicList * NSGetTopics(char *consumerId)
{
    NS_LOG(DEBUG, "NSGetTopics()");

    NSTopicList  * topicList;

    if(consumerId == NULL)
    {
        NS_LOG(DEBUG, "All registered topic list");
    }
    else
    {
        NS_LOG_V(DEBUG, "Subscribed topic list for consumerId(%s)", consumerId);
    }

    NS_LOG(DEBUG, "NSGetTopics() NS_OK");
    return topicList;
}

//TODO: update parameter
NSResult NSStoreTopics(char * consumerId, NSTopic** topics)
{
    NS_LOG(DEBUG, "NSWriteTopicsToStorage()");

    NSCacheElement * element = (NSCacheElement *) OICMalloc(sizeof(NSCacheElement));
    NSCacheTopicData * topicData = (NSCacheTopicData *) OICMalloc(sizeof(NSCacheTopicData));

    OICStrcpy(topicData->consumerId, UUID_STRING_SIZE, consumerId);
    NS_LOG_V(DEBUG, "consumer id: %s", topicData->consumerId);

    // TODO: print topic list
    topicData->topics = topics;
    NS_LOG(DEBUG, "print topic list");

    element->data = (void*) topicData;
    element->next = NULL;

    if(NSStorageWrite(topicStorage, element) != NS_OK)
    {
        NS_LOG(DEBUG, "fail to write cache");
    }

    NS_LOG(DEBUG, "NSWriteTopicsToStorage() NS_OK");
    return NS_OK;
}

NSResult NSRegisterTopicList(NSTopicList *topicList)
{
    NS_LOG(DEBUG, "NSRegisterTopicList()");

    if(!topicList)
    {
        NS_LOG(ERROR, "no topics");
        return NS_ERROR;
    }

    OCResourceHandle rHandle = NULL;
    if(NSPutTopicResource(topicList, &rHandle) != NS_OK)
    {
        NS_LOG(ERROR, "Fail to put topic resource");
        return NS_ERROR;
    }

    if(topicList->consumerId != NULL)
    {
        // id should be null to register topic list
        NS_LOG(ERROR, "invalid consumer id");
        return NS_ERROR;
    }

    NSStoreTopics(topicList->consumerId, topicList->head);

    NS_LOG(DEBUG, "NSRegisterTopicList() NS_OK");
    return NS_OK;
}

NSResult NSSendTopicUpdation()
{
    NS_LOG(DEBUG, "NSSendTopicUpdation - IN");

    OCRepPayload* payload = OCRepPayloadCreate();

    if (!payload)
    {
        NS_LOG(ERROR, "fail to create playload");
        return NS_ERROR;
    }

    OCResourceHandle rHandle = NULL;
    if (NSPutMessageResource(NULL, &rHandle) != NS_OK)
    {
        NS_LOG(ERROR, "Fail to put message resource");
        return NS_ERROR;
    }

    OCRepPayloadSetUri(payload, NS_COLLECTION_MESSAGE_URI);
    OCRepPayloadSetPropInt(payload, NS_ATTRIBUTE_MESSAGE_ID, NS_TOPIC);
    OCRepPayloadSetPropString(payload, NS_ATTRIBUTE_PROVIDER_ID, NSGetProviderInfo()->providerId);

    OCObservationId obArray[255] = { 0, };
    int obCount = 0;

    NSCacheElement * it = consumerSubList->head;

    while (it)
    {
        NSCacheSubData * subData = (NSCacheSubData *) it->data;

        if (subData->isWhite)
        {
            if(subData->messageObId != 0)
            {
                obArray[obCount++] = subData->messageObId;
            }

#ifdef WITH_CLOUD
            if(subData->remote_messageObId != 0)
            {
                obArray[obCount++] = subData->remote_messageObId;
            }
#endif

        }
        it = it->next;
    }

    if(!obCount)
    {
        NS_LOG(ERROR, "observer count is zero");
        return NS_ERROR;
    }

    if (OCNotifyListOfObservers(rHandle, obArray, obCount, payload, OC_HIGH_QOS)
            != OC_STACK_OK)
    {
        NS_LOG(ERROR, "fail to send topic updation");
        OCRepPayloadDestroy(payload);
        return NS_ERROR;

    }
    OCRepPayloadDestroy(payload);

    NS_LOG(DEBUG, "NSSendTopicUpdation - OUT");
    return NS_OK;
}

NSResult NSSendTopicUpdationToConsumer(char *consumerId)
{
    NS_LOG(DEBUG, "NSSendTopicUpdationToConsumer - IN");

    OCRepPayload* payload = OCRepPayloadCreate();

    if (!payload)
    {
        NS_LOG(ERROR, "fail to create playload");
        return NS_ERROR;
    }

    OCResourceHandle rHandle = NULL;
    if (NSPutMessageResource(NULL, &rHandle) != NS_OK)
    {
        NS_LOG(ERROR, "Fail to put message resource");
        return NS_ERROR;
    }

    OCRepPayloadSetUri(payload, NS_COLLECTION_MESSAGE_URI);
    OCRepPayloadSetPropInt(payload, NS_ATTRIBUTE_MESSAGE_ID, NS_TOPIC);
    OCRepPayloadSetPropString(payload, NS_ATTRIBUTE_PROVIDER_ID, NSGetProviderInfo()->providerId);

    NSCacheElement * element = NSStorageRead(consumerSubList, consumerId);

    if(element == NULL)
    {
        NS_LOG(ERROR, "element is NULL");
        return NS_ERROR;
    }

    NSCacheSubData * subData = (NSCacheSubData*) element->data;

    if (OCNotifyListOfObservers(rHandle, (OCObservationId*)&subData->messageObId, 1, payload, OC_HIGH_QOS)
            != OC_STACK_OK)
    {
        NS_LOG(ERROR, "fail to send topic updation");
        OCRepPayloadDestroy(payload);
        return NS_ERROR;

    }

    OCRepPayloadDestroy(payload);

    NS_LOG(DEBUG, "NSSendTopicUpdationToConsumer - OUT");
    return NS_OK;
}

bool NSIsTopicSubscribed(char * consumerId, char * topic)
{
    //TODO: implement function
    return true;
}

void * NSTopicSchedule(void * ptr)
{
    if (ptr == NULL)
    {
        NS_LOG(DEBUG, "Create NSTopicSchedule");
    }

    while (NSIsRunning[TOPIC_SCHEDULER])
    {
        sem_wait(&NSSemaphore[TOPIC_SCHEDULER]);
        pthread_mutex_lock(&NSMutex[TOPIC_SCHEDULER]);

        if (NSHeadMsg[TOPIC_SCHEDULER] != NULL)
        {
            NSTask *node = NSHeadMsg[TOPIC_SCHEDULER];
            NSHeadMsg[TOPIC_SCHEDULER] = node->nextTask;

            switch (node->taskType)
            {
                case TASK_SEND_TOPICS:
                    NS_LOG(DEBUG, "CASE TASK_SEND_TOPICS : ");
                    break;
                case TASK_SUBSCRIBE_TOPICS:
                    NS_LOG(DEBUG, "CASE TASK_SUBSCRIBE_TOPICS : ");
                    break;
                case TASK_REGISTER_TOPICS:
                    NS_LOG(DEBUG, "CASE TASK_REGISTER_TOPICS : ");
                    NSTopicList * topicList = (NSTopicList *) node->taskData;
                    NSRegisterTopicList(topicList);
                    NSSendTopicUpdation();
                    // TODO : free NSTopic
                    // NSFreeTopicList(topicList);
                    break;
                default:
                    break;
            }

            OICFree(node);
        }

        pthread_mutex_unlock(&NSMutex[TOPIC_SCHEDULER]);
    }

    NS_LOG(DEBUG, "Destroy NSTopicSchedule");
    return NULL;
}

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

#include "NSProviderScheduler.h"

pthread_t NSThread[THREAD_COUNT];
pthread_mutex_t NSMutex[THREAD_COUNT];
sem_t NSSemaphore[THREAD_COUNT];
bool NSIsRunning[THREAD_COUNT] = { false, };

NSTask* NSHeadMsg[THREAD_COUNT];
NSTask* NSTailMsg[THREAD_COUNT];

void * NSCallbackResponseSchedule(void *ptr);
void * NSDiscoverySchedule(void *ptr);
void * NSSubScriptionSchedule(void *ptr);
void * NSNotificationSchedule(void *ptr);
void * NSTopicSchedule(void * ptr);

bool NSInitScheduler()
{
    NS_LOG(DEBUG, "NSInitScheduler - IN");

    int i = 0;

    for (i = 0; i < THREAD_COUNT; i++)
    {
        pthread_mutex_init(&NSMutex[i], NULL);
        NSIsRunning[i] = true;
        sem_init(&(NSSemaphore[i]), 0, 0);
    }

    NS_LOG(DEBUG, "NSInitScheduler - OUT");

    return true;
}

bool NSStartScheduler()
{
    int i = 0;

    for (i = 0; i < THREAD_COUNT; i++)
    {
        pthread_mutex_lock(&NSMutex[i]);

        switch (i)
        {
            case CALLBACK_RESPONSE_SCHEDULER:
            {
                NS_LOG(DEBUG, "CASE RESPONSE_SCHEDULER :");
                pthread_create(&NSThread[i], NULL, NSCallbackResponseSchedule, NULL);
            }
                break;

            case DISCOVERY_SCHEDULER:
            {
                NS_LOG(DEBUG, "CASE DISCOVERY_SCHEDULER :");
                pthread_create(&NSThread[i], NULL, NSDiscoverySchedule, NULL);
            }
                break;

            case SUBSCRIPTION_SCHEDULER:
            {
                NS_LOG(DEBUG, "CASE SUBSCRIPTION_SCHEDULER :");
                pthread_create(&NSThread[i], NULL, NSSubScriptionSchedule, NULL);
            }
                break;

            case NOTIFICATION_SCHEDULER:
            {
                NS_LOG(DEBUG, "CASE NOTIFICATION_SCHEDULER :");
                pthread_create(&NSThread[i], NULL, NSNotificationSchedule, NULL);
            }
                break;

            case TOPIC_SCHEDULER:
            {
                NS_LOG(DEBUG, "CASE TOPIC_SCHEDULER :");
                pthread_create(&NSThread[i], NULL, NSTopicSchedule, NULL);
            }
                break;
            default:
                break;

        }

        NSHeadMsg[i] = NSTailMsg[i] = NULL;

        pthread_mutex_unlock(&NSMutex[i]);

    }

    return true;
}

bool NSStopScheduler()
{
    NS_LOG(DEBUG, "NSStopScheduler - IN");
    int i = 0;

    for (i = THREAD_COUNT - 1; i >= 0; --i)
    {
        int status = -1;

        NSIsRunning[i] = false;

        sem_post(&(NSSemaphore[i]));
        pthread_join(NSThread[i], (void **) &status);

        NSThread[i] = 0;

        pthread_mutex_lock(&NSMutex[i]);

        while (NSHeadMsg[i] != NULL)
        {
            NSTask* temp = NSHeadMsg[i];
            NSHeadMsg[i] = NSHeadMsg[i]->nextTask;
            NSFreeData(i, temp);
            NSOICFree(temp);
        }

        NSTailMsg[i] = NSHeadMsg[i] = NULL;

        pthread_mutex_unlock(&NSMutex[i]);
        pthread_mutex_destroy(&NSMutex[i]);
    }

    NS_LOG(DEBUG, "NSStopScheduler - OUT");

    return true;
}

void NSPushQueue(NSSchedulerType schedulerType, NSTaskType taskType, void* data)
{

    if (!NSIsRunning[schedulerType])
    {
        return;
    }

    pthread_mutex_lock(&NSMutex[schedulerType]);

    NS_LOG(DEBUG, "NSPushQueue - IN");
    NS_LOG_V(DEBUG, "NSSchedulerType = %d", schedulerType);
    NS_LOG_V(DEBUG, "NSTaskType = %d", taskType);

    if (NSHeadMsg[schedulerType] == NULL)
    {
        NSHeadMsg[schedulerType] = (NSTask*) OICMalloc(sizeof(NSTask));

        if (NSHeadMsg[schedulerType])
        {
            NSHeadMsg[schedulerType]->taskType = taskType;
            NSHeadMsg[schedulerType]->taskData = data;
            NSHeadMsg[schedulerType]->nextTask = NULL;
            NSTailMsg[schedulerType] = NSHeadMsg[schedulerType];
        }
    }
    else
    {
        NSTask* newNode = (NSTask*) OICMalloc(sizeof(NSTask));
        if (newNode)
        {
            newNode->taskType = taskType;
            newNode->taskData = data;
            newNode->nextTask = NULL;

            NSTailMsg[schedulerType]->nextTask = newNode;
            NSTailMsg[schedulerType] = newNode;
        }
    }

    sem_post(&(NSSemaphore[schedulerType]));
    NS_LOG(DEBUG, "NSPushQueue - OUT");
    pthread_mutex_unlock(&NSMutex[schedulerType]);
}

void NSFreeData(NSSchedulerType type, NSTask * task)
{
    NS_LOG(DEBUG, "NSFreeData - IN");

    if (type == CALLBACK_RESPONSE_SCHEDULER)
    {
        switch (task->taskType)
        {
            case TASK_CB_SUBSCRIPTION:
                NS_LOG(DEBUG, "CASE TASK_CB_SUBSCRIPTION : Free");
                NSFreeOCEntityHandlerRequest((OCEntityHandlerRequest*) task->taskData);
                break;
            case TASK_CB_SYNC:
                NS_LOG(DEBUG, "CASE TASK_CB_SYNC : Free");
                NSFreeSync((NSSyncInfo*) task->taskData);
                break;
            default:
                NS_LOG(DEBUG, "No Task Type");
                break;
        }
    }
    else if (type == DISCOVERY_SCHEDULER)
    {
        switch (task->taskType)
        {
            case TASK_START_PRESENCE:
            case TASK_STOP_PRESENCE:
            case TASK_REGISTER_RESOURCE:
                NS_LOG(DEBUG, "Not required Free");
                break;
            default:
                NS_LOG(DEBUG, "No Task Type");
                break;
        }
    }
    else if (type == SUBSCRIPTION_SCHEDULER)
    {
        switch (task->taskType)
        {
            case TASK_SEND_POLICY:
            case TASK_RECV_SUBSCRIPTION:
            case TASK_RECV_UNSUBSCRIPTION:
            case TASK_SYNC_SUBSCRIPTION:
                NS_LOG(DEBUG, "NSFreeOCEntityHandlerRequest : Free ");
                NSFreeOCEntityHandlerRequest((OCEntityHandlerRequest*) task->taskData);
                break;
            case TASK_SEND_ALLOW:
            case TASK_SEND_DENY:
                NS_LOG(DEBUG, "NSFreeConsumer : Free ");
                NSFreeConsumer((NSConsumer *) task->taskData);
                break;
            default:
                NS_LOG(DEBUG, "No Task Type");
                break;
        }
    }
    else if (type == NOTIFICATION_SCHEDULER)
    {
        switch (task->taskType)
        {
            case TASK_SEND_NOTIFICATION:
            {
                NS_LOG(DEBUG, "NSFreeMessage : Free ");
                NSFreeMessage((NSMessage *)task->taskData);
                break;
            }
            case TASK_SEND_READ:
            case TASK_RECV_READ:
                NS_LOG(DEBUG, "NSFreeSync : Free ");
                NSFreeSync((NSSyncInfo*) task->taskData);
                break;

            default:
                NS_LOG(DEBUG, "No Task Type");
                break;
        }
    }
    else if (type == TOPIC_SCHEDULER)
    {
        switch (task->taskType)
        {
            case TASK_SUBSCRIBE_TOPIC:
            case TASK_UNSUBSCRIBE_TOPIC:
            {
                NSCacheTopicSubData * data = task->taskData;
                NSOICFree(data->topicName);
                NSOICFree(data);
            }
                break;
            case TASK_REGISTER_TOPIC:
            case TASK_UNREGISTER_TOPIC:
            {
                NSOICFree(task->taskData);
            }
                break;
            case TASK_SEND_TOPICS:
            case TASK_POST_TOPIC:
            {
                NS_LOG(DEBUG, "TASK_POST_TOPIC : ");
                NSFreeOCEntityHandlerRequest((OCEntityHandlerRequest*) task->taskData);
            }
                break;
            default:
                break;
        }
    }
    NS_LOG(DEBUG, "NSFreeData - OUT");
}

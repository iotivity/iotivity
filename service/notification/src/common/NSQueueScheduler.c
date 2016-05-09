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

#include "NSQueueScheduler.h"

pthread_t NSThread[THREAD_COUNT];
pthread_mutex_t NSMutex[THREAD_COUNT];
sem_t NSSemaphore[THREAD_COUNT];
bool NSIsRunning[THREAD_COUNT] =
{ false, };

NSTask* NSHeadMsg[THREAD_COUNT] =
{ 0, };
NSTask* NSTailMsg[THREAD_COUNT] =
{ 0, };

void * NSResponseSchedule(void *ptr);
void * NSDiscoverySchedule(void *ptr);
void * NSSubScriptionSchedule(void *ptr);
void * NSNotificationSchedule(void *ptr);

bool NSInitScheduler()
{
    OIC_LOG(INFO, SCHEDULER_TAG, "NSInitScheduler()");

    int i = 0;
    //	NsQsStopScheduler(ALL_SCHEDULER);

    for (i = 0; i < THREAD_COUNT; i++)
    {
        pthread_mutex_init(&NSMutex[i], NULL);
        NSIsRunning[i] = true;
        sem_init(&(NSSemaphore[i]), 0, 0);
    }

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
            case RESPONSE_SCHEDULER:
            {
                pthread_create(&NSThread[i], NULL, NSResponseSchedule, NULL);
            }
                break;

            case DISCOVERY_SCHEDULER:
            {
                pthread_create(&NSThread[i], NULL, NSDiscoverySchedule, NULL);
            }
                break;

            case SUBSCRIPTION_SCHEDULER:
            {
                pthread_create(&NSThread[i], NULL, NSSubScriptionSchedule, NULL);
            }
                break;

            case NOTIFICATION_SCHEDULER:
            {
                pthread_create(&NSThread[i], NULL, NSNotificationSchedule, NULL);
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
    int i = 0;

    for (i = THREAD_COUNT - 1; i >= 0; --i)
    {

        NSIsRunning[i] = false;

        pthread_join(NSThread[i], (void *) NULL);
        NSThread[i] = 0;

        pthread_mutex_lock(&NSMutex[i]);

        while (NSHeadMsg[i]->nextTask != NULL)
        {
            NSTask* temp = NSHeadMsg[i];
            NSHeadMsg[i] = NSHeadMsg[i]->nextTask;
            OICFree(temp);
        }

        OICFree(NSHeadMsg[i]);

        pthread_mutex_unlock(&NSMutex[i]);
    }

    return true;
}

void NSPushQueue(NSSchedulerType schedulerType, NSTaskType taskType, void* data)
{
    pthread_mutex_lock(&NSMutex[schedulerType]);
    // mutext Lock
    if (NSHeadMsg[schedulerType] == NULL)
    {
        printf("first schedule");
        NSHeadMsg[schedulerType] = (NSTask*) malloc(sizeof(NSTask));
        memset(NSHeadMsg[schedulerType], 0, sizeof(NSTask));
        NSHeadMsg[schedulerType]->taskType = taskType;
        NSHeadMsg[schedulerType]->taskData = data;
        NSHeadMsg[schedulerType]->nextTask = NULL;
        NSTailMsg[schedulerType] = NSHeadMsg[schedulerType];
    }
    else
    {
        printf("after first schedule");
        NSTask* newNode = (NSTask*) malloc(sizeof(NSTask));
        memset(newNode, 0, sizeof(NSTask));
        newNode->taskType = taskType;
        newNode->taskData = data;
        newNode->nextTask = NULL;

        NSTailMsg[schedulerType]->nextTask = newNode;
        NSTailMsg[schedulerType] = newNode;
    }

    sem_post(&(NSSemaphore[schedulerType]));
    pthread_mutex_unlock(&NSMutex[schedulerType]);

    // mutext UnLock
}


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

#include "NSProviderDiscovery.h"

NSResult NSStartPresence()
{
    OIC_LOG(INFO, DISCOVERY_TAG, "NSStartPresence()");
    NS_LOG(INFO, "NSStartPresence()");

    if (OCStartPresence(0) != OC_STACK_OK)
    {
        OIC_LOG(INFO, DISCOVERY_TAG, "NSStartPresence() NS_ERROR");
        NS_LOG(INFO, "NSStartPresence() NS_ERROR");
        return NS_ERROR;
    }

    OIC_LOG(INFO, DISCOVERY_TAG, "NSStartPresence() NS_OK");
    NS_LOG(INFO, "NSStartPresence() NS_OK");
    return NS_OK;
}

NSResult NSStopPresence()
{
    OIC_LOG(INFO, DISCOVERY_TAG, "NSStopPresence()");
    NS_LOG(INFO, "NSStopPresence()");

    if (OCStopPresence() != OC_STACK_OK)
    {
        OIC_LOG(INFO, DISCOVERY_TAG, "NSStopPresence() NS_ERROR");
        NS_LOG(INFO, "NSStopPresence() NS_ERROR");
        return NS_ERROR;
    }

    OIC_LOG(INFO, DISCOVERY_TAG, "NSStopPresence() NS_OK");
    NS_LOG(INFO, "NSStopPresence() NS_OK");
    return NS_OK;
}

void * NSDiscoverySchedule(void * ptr)
{
    if (ptr == NULL)
    {
        OIC_LOG(INFO, DISCOVERY_TAG, "Create NSDiscoverySchedule");
        NS_LOG(INFO, "Create NSDiscoverySchedule");
    }

    while (NSIsRunning[DISCOVERY_SCHEDULER])
    {
        sem_wait(&NSSemaphore[DISCOVERY_SCHEDULER]);
        pthread_mutex_lock(&NSMutex[DISCOVERY_SCHEDULER]);

        if (NSHeadMsg[DISCOVERY_SCHEDULER] != NULL)
        {
            NSTask *node = NSHeadMsg[DISCOVERY_SCHEDULER];
            NSHeadMsg[DISCOVERY_SCHEDULER] = node->nextTask;

            switch (node->taskType)
            {
                case TASK_START_PRESENCE:
                    NS_LOG(INFO, "CASE TASK_START_PRESENCE : ");
                    NSStartPresence();
                    break;
                case TASK_STOP_PRESENCE:
                    NS_LOG(INFO, "CASE TASK_STOP_PRESENCE : ");
                    NSStopPresence();
                    break;
                case TASK_REGISTER_RESOURCE:
                    NS_LOG(INFO, "CASE TASK_REGISTER_RESOURCE : ");
                    NSRegisterResource();
                    break;
                default:
                    break;
            }

            OICFree(node);
        }

        pthread_mutex_unlock(&NSMutex[DISCOVERY_SCHEDULER]);

    }
    return NULL;
}

/******************************************************************
 *
 * Copyright 2014 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#ifndef _CA_MESSAGE_QUEUE_H_
#define _CA_MESSAGE_QUEUE_H_

#include "umutex.h"
#include "cacommon.h"

/**
 @brief Adapter message information
 */
typedef struct CAAdapterMessage_t
{
    CARemoteEndpoint_t *remoteEndpoint;
    void *data;
    uint32_t dataLen;
    struct CAAdapterMessage_t *next;
} CAAdapterMessage_t;

/**
 @brief Adapter message queue
 */
typedef struct
{
    u_mutex queueMutex;
    CAAdapterMessage_t *begin;
    CAAdapterMessage_t *end;
} CAAdapterMessageQueue_t;

/**
 * @brief API to initialize Message queue.
 * @param queueHandle will be set on successfull initialization of message queue.
 *             This param to be passed in all subsequent calls to queue handling api's.
 * @return Error Code
 */
CAResult_t  CAAdapterInitializeMessageQueue(CAAdapterMessageQueue_t **queueHandle);

/**
 * @brief API to terminate Message queue.
 *          This api will delete entire message queue and will take care of freeing all memory.
 *          queueHandle will become invalid after this api call.
 * @param queueHandle handle of queue obtained via CAAdapterInitializeMessageQueue()
 */
void CAAdapterTerminateMessageQueue(CAAdapterMessageQueue_t *queueHandle);

/**
 * @brief API to add Message to queue.
 *          This api will duplicate all passed arguments and add entry to end of message queue.
 * @param queueHandle handle of queue obtained via CAAdapterInitializeMessageQueue()
 * @param remoteEndpoint remoteEndpoint from which data is received or is supposed to be sent.
 *                                  Would be NULL for outgoing multicast data.
 * @param data Data that is received or supposed to be sent.
 * @param dataLen Data length.
 * @return Error Code
 */
CAResult_t CAAdapterEnqueueMessage(CAAdapterMessageQueue_t *queueHandle,
                                   CARemoteEndpoint_t *remoteEndpoint,
                                   void *data, uint32_t dataLen);

/**
 * @brief API to get oldest message and remove it from the queue
 * @param queueHandle handle of queue obtained via CAAdapterInitializeMessageQueue()
 * @param message oldest message in queue. To be freed by caller using CAAdapterFreeMessage() api.
 * @return Error Code
 */
CAResult_t CAAdapterDequeueMessage(CAAdapterMessageQueue_t *queueHandle,
                                   CAAdapterMessage_t **message);

/**
 * @brief API to help caller free the message obtained via CAAdapterDequeueMessage() api.
 * @param message to be freed.
 */
void CAAdapterFreeMessage(CAAdapterMessage_t *message);

#endif //#ifndef _CA_MESSAGE_QUEUE_H_


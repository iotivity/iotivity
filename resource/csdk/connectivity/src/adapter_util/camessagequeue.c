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

#include "camessagequeue.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "logger.h"
#include "caadapterutils.h"
#include "oic_malloc.h"

#define TAG "CAAdapterUtil"

CAResult_t CAAdapterInitializeMessageQueue(CAAdapterMessageQueue_t **queueHandle)
{
    OIC_LOG(DEBUG, TAG, "IN");
    VERIFY_NON_NULL(queueHandle, TAG, "Invalid queue handle container");

    CAAdapterMessageQueue_t *queuePtr = (CAAdapterMessageQueue_t *) OICMalloc(
            sizeof(CAAdapterMessageQueue_t));
    if (!queuePtr)
    {
        OIC_LOG_V(ERROR, TAG, "Out of memory!");
        return CA_MEMORY_ALLOC_FAILED;
    }

    memset((void *) queuePtr, 0, sizeof(CAAdapterMessageQueue_t));
    // Initialize mutex utility
    u_mutex_init();
    queuePtr->queueMutex = u_mutex_new();
    if (NULL == queuePtr->queueMutex)
    {
        OICFree(queuePtr);
        return CA_MEMORY_ALLOC_FAILED;
    }
    queuePtr->begin = NULL;
    queuePtr->end = NULL;
    *queueHandle = queuePtr;

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

void CAAdapterFreeMessage(CAAdapterMessage_t *message)
{
    OIC_LOG(DEBUG, TAG, "IN");
    VERIFY_NON_NULL_VOID(message, TAG, "Invalid message");

    if (message->remoteEndpoint)
    {
        CAAdapterFreeRemoteEndpoint(message->remoteEndpoint);
    }

    if (message->data)
    {
        OICFree(message->data);
    }
    OICFree(message);
    OIC_LOG(DEBUG, TAG, "OUT");
}

void CAAdapterTerminateMessageQueue(CAAdapterMessageQueue_t *queueHandle)
{
    OIC_LOG(DEBUG, TAG, "IN");
    VERIFY_NON_NULL_VOID(queueHandle, TAG, "Invalid queue handle");
    VERIFY_NON_NULL_VOID(queueHandle->queueMutex, TAG, "Invalid queue mutex");
    CAAdapterMessage_t *messagePtr = NULL;
    CAAdapterMessage_t *toRemove = NULL;

    u_mutex_lock(queueHandle->queueMutex);
    messagePtr = queueHandle->begin;
    while (messagePtr)
    {
        toRemove = messagePtr;
        messagePtr = messagePtr->next;
        CAAdapterFreeMessage(toRemove);
    }

    queueHandle->begin = NULL;
    queueHandle->end = NULL;
    u_mutex_unlock(queueHandle->queueMutex);
    u_mutex_free(queueHandle->queueMutex);
    queueHandle->queueMutex = NULL;
    OICFree(queueHandle);

    OIC_LOG(DEBUG, TAG, "OUT");
}

CAResult_t CAAdapterEnqueueMessage(CAAdapterMessageQueue_t *queueHandle,
        const CARemoteEndpoint_t *remoteEndpoint, void *data, uint32_t dataLen)
{
    OIC_LOG(DEBUG, TAG, "IN");
    VERIFY_NON_NULL(queueHandle, TAG, "Invalid queue handle");
    VERIFY_NON_NULL(queueHandle->queueMutex, TAG, "Invalid queue mutex");
    VERIFY_NON_NULL(data, TAG, "Invalid data");

    CAAdapterMessage_t *adapterMessage = (CAAdapterMessage_t *) OICMalloc(
            sizeof(CAAdapterMessage_t));
    if (!adapterMessage)
    {
        OIC_LOG_V(ERROR, TAG, "Out of memory!");
        return CA_MEMORY_ALLOC_FAILED;
    }
    memset((void *) adapterMessage, 0, sizeof(CAAdapterMessage_t));

    // Copy data
    adapterMessage->data = (void *) OICMalloc(dataLen);
    if (adapterMessage->data == NULL)
    {
        OIC_LOG_V(ERROR, TAG, "Out of memory!!!");
        CAAdapterFreeMessage(adapterMessage);
        return CA_MEMORY_ALLOC_FAILED;
    }
    memcpy(adapterMessage->data, data, dataLen);
    adapterMessage->dataLen = dataLen;

    // Copy remote endpoint info
    adapterMessage->remoteEndpoint = NULL;
    if (remoteEndpoint)
    {
        adapterMessage->remoteEndpoint = CAAdapterCopyRemoteEndpoint(remoteEndpoint);
        if (NULL == adapterMessage->remoteEndpoint)
        {
            OIC_LOG_V(ERROR, TAG, "Out of memory.!!");
            OICFree(adapterMessage->data);
            CAAdapterFreeMessage(adapterMessage);
            return CA_MEMORY_ALLOC_FAILED;
        }
    }
    //Insert at end of queue
    u_mutex_lock(queueHandle->queueMutex);
    if (queueHandle->end)
    {
        queueHandle->end->next = adapterMessage;
        queueHandle->end = adapterMessage;
    }
    else
    {
        queueHandle->begin = adapterMessage;
        queueHandle->end = adapterMessage;
    }
    u_mutex_unlock(queueHandle->queueMutex);
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAAdapterDequeueMessage(CAAdapterMessageQueue_t *queueHandle,
        CAAdapterMessage_t **message)
{
    OIC_LOG(DEBUG, TAG, "IN");
    VERIFY_NON_NULL(queueHandle, TAG, "Invalid queue handle");
    VERIFY_NON_NULL(queueHandle->queueMutex, TAG, "Invalid queue mutex");
    VERIFY_NON_NULL(message, TAG, "Invalid message handle");
    u_mutex_lock(queueHandle->queueMutex);
    if (queueHandle->begin)
    {
        *message = queueHandle->begin;
        queueHandle->begin = queueHandle->begin->next;
        if (queueHandle->begin == NULL)
        {
            queueHandle->end = NULL;
        }
        // keep next as NULL for upper layer
        (*message)->next = NULL;
    }
    else
    {
        u_mutex_unlock(queueHandle->queueMutex);
        return CA_STATUS_FAILED;
    }
    u_mutex_unlock(queueHandle->queueMutex);
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}


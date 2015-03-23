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

// Defining _BSD_SOURCE or _DEFAULT_SOURCE causes header files to expose
// definitions that may otherwise be skipped. Skipping can cause implicit
// declaration warnings and/or bugs and subtle problems in code execution.
// For glibc information on feature test macros,
// Refer http://www.gnu.org/software/libc/manual/html_node/Feature-Test-Macros.html
//
// This file requires #define use due to random()
// For details on compatibility and glibc support,
// Refer http://www.gnu.org/software/libc/manual/html_node/BSD-Random.html
#define _DEFAULT_SOURCE
#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <glib.h>

#ifdef __ANDROID__
#include <linux/time.h>
#endif

#include "caretransmission.h"
#include "caremotehandler.h"
#include "caprotocolmessage.h"
#include "oic_malloc.h"
#include "logger.h"

#define TAG PCF("RET")

typedef struct
{
    uint64_t timeStamp;                 /**< last sent time. microseconds */
    uint64_t timeout;                   /**< timeout value. microseconds */
    uint8_t triedCount;                 /**< retransmission count */
    uint16_t messageId;                 /**< coap PDU message id */
    CARemoteEndpoint_t *endpoint;       /**< remote endpoint */
    void *pdu;                          /**< coap PDU */
    uint32_t size;                      /**< coap PDU size */
} CARetransmissionData_t;

/**
 * @brief   getCurrent monotonic time
 * @return  current time in microseconds
 */
uint64_t getCurrentTimeInMicroSeconds();

/**
 * @brief   check timeout routine
 * @param   currentTime     [IN]microseconds
 * @param   timeStamp       [IN]microseconds
 * @param   timeoutValue    [IN]microseconds
 * @param   triedCount      [IN]
 * @return  true if the timeout period has elapsed, false otherwise
 */
static bool CACheckTimeout(uint64_t currentTime, uint64_t timeStamp, uint64_t timeoutValue,
        uint8_t triedCount)
{
    // #1. calculate timeout
    uint32_t milliTimeoutValue = timeoutValue * 0.001;
    uint64_t timeout = (milliTimeoutValue << triedCount) * (uint64_t) 1000;

    if (currentTime >= timeStamp + timeout)
    {
        OIC_LOG_V(DEBUG, TAG, "%d microseconds time out!!, tried count(%d)", timeout, triedCount);
        return true;
    }

    return false;
}

/**
 * @brief   timeout value is
 *          between DEFAULT_ACK_TIMEOUT_SEC and
 *          (DEFAULT_ACK_TIMEOUT_SEC * DEFAULT_RANDOM_FACTOR) second.
 *          DEFAULT_RANDOM_FACTOR       1.5 (CoAP)
 * @return  microseconds.
 */
static uint64_t CAGetTimeoutValue()
{
    return ((DEFAULT_ACK_TIMEOUT_SEC * 1000) + ((1000 * (random() & 0xFF)) >> 8)) *
            (uint64_t) 1000;
}

static void CACheckRetransmissionList(CARetransmission_t *context)
{
    // mutex lock
    u_mutex_lock(context->threadMutex);

    int32_t i = 0;
    uint32_t len = u_arraylist_length(context->dataList);

    for (i = 0; i < len; i++)
    {
        CARetransmissionData_t *retData = u_arraylist_get(context->dataList, i);

        if (retData == NULL)
        {
            continue;
        }

        uint64_t currentTime = getCurrentTimeInMicroSeconds();

        if (CACheckTimeout(currentTime, retData->timeStamp, retData->timeout, retData->triedCount))
        {
            // #2. if time's up, send the data.
            if (context->dataSendMethod != NULL)
            {
                OIC_LOG_V(DEBUG, TAG, "retransmission CON data!!, message id(%d)",
                          retData->messageId);
                context->dataSendMethod(retData->endpoint, retData->pdu, retData->size);
            }

            // #3. increase the retransmission count and update timestamp.
            retData->timeStamp = currentTime;
            retData->triedCount++;
        }

        // #4. if tried count is max, remove the retransmission data from list.
        if (retData->triedCount >= context->config.tryingCount)
        {
            CARetransmissionData_t *removedData = u_arraylist_remove(context->dataList, i);

            if (removedData != NULL)
            {
                OIC_LOG_V(DEBUG, TAG, "max trying count, remove retransmission CON data!!,\
                        message id(%d)", removedData->messageId);

                // callback for retransmit timeout
                if (context->timeoutCallback != NULL)
                {
                    context->timeoutCallback(removedData->endpoint, removedData->pdu,
                            removedData->size);
                }

                CADestroyRemoteEndpointInternal(removedData->endpoint);
                OICFree(removedData->pdu);

                OICFree(removedData);

                // modify loop value.
                len = u_arraylist_length(context->dataList);

                --i;
            }
            else
            {
                OIC_LOG(ERROR, TAG, "arraylist remove error");
            }

        }
    }

    // mutex unlock
    u_mutex_unlock(context->threadMutex);
}

static void CARetransmissionBaseRoutine(void *threadValue)
{
    OIC_LOG(DEBUG, TAG, "retransmission main thread start..");

    CARetransmission_t *context = (CARetransmission_t *) threadValue;

    if (context == NULL)
    {
        OIC_LOG(ERROR, TAG, "thread data passing error!!");

        return;
    }

    while (!context->isStop)
    {
        // mutex lock
        u_mutex_lock(context->threadMutex);

        if (u_arraylist_length(context->dataList) <= 0)
        {
            // if list is empty, thread will wait
            OIC_LOG(DEBUG, TAG, "wait..there is no retransmission data.");

            // wait
            u_cond_wait(context->threadCond, context->threadMutex);

            OIC_LOG(DEBUG, TAG, "wake up..");
        }
        else
        {
            // check each RETRANSMISSION_CHECK_PERIOD_SEC time.
            OIC_LOG_V(DEBUG, TAG, "wait..(%d)microseconds",
                        RETRANSMISSION_CHECK_PERIOD_SEC * (uint64_t) 1000000);

            // wait
            u_cond_wait_until(context->threadCond, context->threadMutex,
                              RETRANSMISSION_CHECK_PERIOD_SEC * (uint64_t) 1000000);
        }

        // mutex unlock
        u_mutex_unlock(context->threadMutex);

        // check stop flag
        if (context->isStop)
        {
            continue;
        }

        CACheckRetransmissionList(context);
    }

    u_cond_signal(context->threadCond);

    OIC_LOG(DEBUG, TAG, "retransmission main thread end..");

}

CAResult_t CARetransmissionInitialize(CARetransmission_t *context, u_thread_pool_t handle,
        CADataSendMethod_t retransmissionSendMethod, CATimeoutCallback_t timeoutCallback,
        CARetransmissionConfig_t* config)
{
    if (context == NULL)
    {
        OIC_LOG(ERROR, TAG, "thread instance is empty..");
        return CA_STATUS_FAILED;
    }

    if (handle == NULL)
    {
        OIC_LOG(ERROR, TAG, "thread pool handle is empty..");
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TAG, "thread initialize..");

    memset(context, 0, sizeof(CARetransmission_t));

    CARetransmissionConfig_t cfg = { 0 };

    if (config == NULL)
    {
        // setDefault
        cfg.supportType = DEFAULT_RETRANSMISSION_TYPE;
        cfg.tryingCount = DEFAULT_MAX_RETRANSMIT;
    }
    else
    {
        cfg = *config;
    }

    // set send thread data
    context->threadPool = handle;
    context->threadMutex = u_mutex_new();
    context->threadCond = u_cond_new();
    context->dataSendMethod = retransmissionSendMethod;
    context->timeoutCallback = timeoutCallback;
    context->config = cfg;
    context->isStop = false;
    context->dataList = u_arraylist_create();

    return CA_STATUS_OK;
}

CAResult_t CARetransmissionStart(CARetransmission_t *context)
{
    if (context == NULL)
    {
        OIC_LOG(ERROR, TAG, "context is empty..");
        return CA_STATUS_FAILED;
    }

    if (context->threadPool == NULL)
    {
        OIC_LOG(ERROR, TAG, "thread pool handle is empty..");
        return CA_STATUS_FAILED;
    }

    CAResult_t res = u_thread_pool_add_task(context->threadPool, CARetransmissionBaseRoutine,
                                            context);

    if (res != CA_STATUS_OK)
    {
        OIC_LOG(ERROR, TAG, "thread pool add task error(send thread).");
        return res;
    }

    return res;
}

CAResult_t CARetransmissionSentData(CARetransmission_t *context,
    const CARemoteEndpoint_t* endpoint, const void* pdu, uint32_t size)
{
    if (context == NULL || endpoint == NULL || pdu == NULL)
    {
        OIC_LOG(ERROR, TAG, "invalid parameter..");
        return CA_STATUS_INVALID_PARAM;
    }

    // #0. check support connectivity type
    if (!(context->config.supportType & endpoint->connectivityType))
    {
        OIC_LOG_V(DEBUG, TAG, "not supported connectivity type for retransmission..(%d)",
                  endpoint->connectivityType);
        return CA_STATUS_OK;
    }

    // #1. check PDU method type and get message id.
    CAMessageType_t type = CAGetMessageTypeFromPduBinaryData(pdu, size);
    uint16_t messageId = CAGetMessageIdFromPduBinaryData(pdu, size);

    OIC_LOG_V(DEBUG, TAG, "sent pdu, message type(%d), message id(%d)", type, messageId);

    if (type != CA_MSG_CONFIRM)
    {
        return CA_STATUS_OK;
    }

    // create retransmission data
    CARetransmissionData_t *retData = (CARetransmissionData_t *) OICCalloc(
                                          1, sizeof(CARetransmissionData_t));

    if (retData == NULL)
    {
        OIC_LOG(ERROR, TAG, "memory error!!");
        return CA_MEMORY_ALLOC_FAILED;
    }

    // copy PDU data
    void *pduData = (void *) OICMalloc(sizeof(int8_t) * size);
    if (pduData == NULL)
    {
        OICFree(retData);
        OIC_LOG(ERROR, TAG, "memory error!!");
        return CA_MEMORY_ALLOC_FAILED;
    }
    memcpy(pduData, pdu, sizeof(int8_t) * size);

    // clone remote endpoint
    CARemoteEndpoint_t *remoteEndpoint = CACloneRemoteEndpoint(endpoint);
    if (remoteEndpoint == NULL)
    {
        OICFree(retData);
        OICFree(pduData);
        OIC_LOG(ERROR, TAG, "memory error!!");
        return CA_MEMORY_ALLOC_FAILED;
    }

    // #2. add additional information. (time stamp, retransmission count...)
    retData->timeStamp = getCurrentTimeInMicroSeconds();
    retData->timeout = CAGetTimeoutValue();
    retData->triedCount = 0;
    retData->messageId = messageId;
    retData->endpoint = remoteEndpoint;
    retData->pdu = pduData;
    retData->size = size;

    // mutex lock
    u_mutex_lock(context->threadMutex);

    uint32_t i = 0;
    uint32_t len = u_arraylist_length(context->dataList);

    // #3. add data into list
    for (i = 0; i < len; i++)
    {
        CARetransmissionData_t *currData = u_arraylist_get(context->dataList, i);

        if (currData == NULL)
        {
            continue;
        }

        // found index
        if (retData != NULL && (currData->endpoint->connectivityType == endpoint->connectivityType)
            && currData->messageId == messageId)
        {
            OIC_LOG(ERROR, TAG, "Duplicate message ID");

            // mutex unlock
            u_mutex_unlock(context->threadMutex);

            OICFree(retData);
            OICFree(pduData);
            OICFree(remoteEndpoint);
            return CA_STATUS_FAILED;
        }
    }

    u_arraylist_add(context->dataList, (void *) retData);

    // notity the thread
    u_cond_signal(context->threadCond);

    // mutex unlock
    u_mutex_unlock(context->threadMutex);

    return CA_STATUS_OK;
}

CAResult_t CARetransmissionReceivedData(CARetransmission_t *context,
                                        const CARemoteEndpoint_t *endpoint, const void *pdu,
                                        uint32_t size)
{
    if (context == NULL || endpoint == NULL || pdu == NULL)
    {
        OIC_LOG(ERROR, TAG, "invalid parameter..");
        return CA_STATUS_INVALID_PARAM;
    }

    // #0. check support connectivity type
    if (!(context->config.supportType & endpoint->connectivityType))
    {
        OIC_LOG_V(DEBUG, TAG, "not supported connectivity type for retransmission..(%d)",
                  endpoint->connectivityType);
        return CA_STATUS_OK;
    }

    // #1. check PDU method type and get message id.
    // ACK, RST --> remove the CON data
    CAMessageType_t type = CAGetMessageTypeFromPduBinaryData(pdu, size);
    uint16_t messageId = CAGetMessageIdFromPduBinaryData(pdu, size);

    OIC_LOG_V(DEBUG, TAG, "received pdu, message type(%d), message id(%d)", type, messageId);

    if (type != CA_MSG_ACKNOWLEDGE && type != CA_MSG_RESET)
    {
        return CA_STATUS_OK;
    }

    // mutex lock
    u_mutex_lock(context->threadMutex);

    uint32_t i = 0;
    uint32_t len = u_arraylist_length(context->dataList);

    // find index
    for (i = 0; i < len; i++)
    {
        CARetransmissionData_t *retData = u_arraylist_get(context->dataList, i);

        // found index
        if (retData != NULL && (retData->endpoint->connectivityType == endpoint->connectivityType)
            && retData->messageId == messageId)
        {
            // #2. remove data from list
            CARetransmissionData_t *removedData = u_arraylist_remove(context->dataList, i);

            if (removedData != NULL)
            {
                OIC_LOG_V(DEBUG, TAG, "remove retransmission CON data!!, message id(%d)",
                        messageId);

                CADestroyRemoteEndpointInternal(removedData->endpoint);
                OICFree(removedData->pdu);

                OICFree(removedData);
            }
            break;
        }
    }

    // mutex unlock
    u_mutex_unlock(context->threadMutex);

    return CA_STATUS_OK;
}

CAResult_t CARetransmissionStop(CARetransmission_t *context)
{
    if (context == NULL)
    {
        OIC_LOG(ERROR, TAG, "context is empty..");
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TAG, "retransmission stop request!!");

    // mutex lock
    u_mutex_lock(context->threadMutex);

    // set stop flag
    context->isStop = true;

    // notity the thread
    u_cond_signal(context->threadCond);

    u_cond_wait(context->threadCond, context->threadMutex);

    // mutex unlock
    u_mutex_unlock(context->threadMutex);

    return CA_STATUS_OK;
}

CAResult_t CARetransmissionDestroy(CARetransmission_t *context)
{
    if (context == NULL)
    {
        OIC_LOG(ERROR, TAG, "context is empty..");
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TAG, "retransmission context destroy..");

    u_mutex_free(context->threadMutex);
    context->threadMutex = NULL;
    u_cond_free(context->threadCond);
    u_arraylist_free(&context->dataList);

    return CA_STATUS_OK;
}

uint64_t getCurrentTimeInMicroSeconds()
{
    uint64_t currentTime = 0;

#ifdef __ANDROID__
    struct timespec getTs;

    clock_gettime(CLOCK_MONOTONIC, &getTs);

    currentTime = (getTs.tv_sec * (uint64_t)1000000000 + getTs.tv_nsec)/1000;
    OIC_LOG_V(DEBUG, TAG, "current time = %d", currentTime);
#else
    currentTime = g_get_monotonic_time();
#endif
    return currentTime;
}


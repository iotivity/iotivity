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

// Defining _POSIX_C_SOURCE macro with 199309L (or greater) as value
// causes header files to expose definitions
// corresponding to the POSIX.1b, Real-time extensions
// (IEEE Std 1003.1b-1993) specification
//
// For this specific file, see use of clock_gettime,
// Refer to http://pubs.opengroup.org/stage7tc1/functions/clock_gettime.html
// and to http://man7.org/linux/man-pages/man2/clock_gettime.2.html
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#if defined(__ANDROID__)
#include <linux/time.h>
#endif

#include "caretransmission.h"
#include "caremotehandler.h"
#include "caprotocolmessage.h"
#include "oic_malloc.h"
#include "logger.h"

#define TAG PCF("CA")

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

static const uint64_t USECS_PER_SEC = 1000000;

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
 * @param   triedCount      [IN]Number of retransmission tried
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
    if (NULL == context)
    {
        OIC_LOG(ERROR, TAG, "context is null..");
        return;
    }

    // mutex lock
    ca_mutex_lock(context->threadMutex);

    uint32_t i = 0;
    uint32_t len = u_arraylist_length(context->dataList);

    for (i = 0; i < len; i++)
    {
        CARetransmissionData_t *retData = u_arraylist_get(context->dataList, i);

        if (NULL == retData)
        {
            continue;
        }

        uint64_t currentTime = getCurrentTimeInMicroSeconds();

        if (CACheckTimeout(currentTime, retData->timeStamp, retData->timeout, retData->triedCount))
        {
            // #2. if time's up, send the data.
            if (NULL != context->dataSendMethod)
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

            if (NULL != removedData)
            {
                OIC_LOG_V(DEBUG, TAG, "max trying count, remove retransmission CON data!!,\
                          message id(%d)", removedData->messageId);

                // callback for retransmit timeout
                if (NULL != context->timeoutCallback)
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
    ca_mutex_unlock(context->threadMutex);
}

static void CARetransmissionBaseRoutine(void *threadValue)
{
    OIC_LOG(DEBUG, TAG, "retransmission main thread start..");

    CARetransmission_t *context = (CARetransmission_t *) threadValue;

    if (NULL == context)
    {
        OIC_LOG(ERROR, TAG, "thread data passing error!!");

        return;
    }

    while (!context->isStop)
    {
        // mutex lock
        ca_mutex_lock(context->threadMutex);

        if (!context->isStop && u_arraylist_length(context->dataList) <= 0)
        {
            // if list is empty, thread will wait
            OIC_LOG(DEBUG, TAG, "wait..there is no retransmission data.");

            // wait
            ca_cond_wait(context->threadCond, context->threadMutex);

            OIC_LOG(DEBUG, TAG, "wake up..");
        }
        else if (!context->isStop)
        {
            // check each RETRANSMISSION_CHECK_PERIOD_SEC time.
            OIC_LOG_V(DEBUG, TAG, "wait..(%d)microseconds",
                      RETRANSMISSION_CHECK_PERIOD_SEC * (uint64_t) USECS_PER_SEC);

            // wait
            uint64_t absTime = RETRANSMISSION_CHECK_PERIOD_SEC * (uint64_t) USECS_PER_SEC;
            ca_cond_wait_for(context->threadCond, context->threadMutex, absTime );
        }
        else
        {
            // we are stopping, so we want to unlock and finish stopping
        }

        // mutex unlock
        ca_mutex_unlock(context->threadMutex);

        // check stop flag
        if (context->isStop)
        {
            continue;
        }

        CACheckRetransmissionList(context);
    }

    ca_mutex_lock(context->threadMutex);
    ca_cond_signal(context->threadCond);
    ca_mutex_unlock(context->threadMutex);

    OIC_LOG(DEBUG, TAG, "retransmission main thread end..");

}

CAResult_t CARetransmissionInitialize(CARetransmission_t *context, ca_thread_pool_t handle,
                                      CADataSendMethod_t retransmissionSendMethod,
                                      CATimeoutCallback_t timeoutCallback,
                                      CARetransmissionConfig_t* config)
{
    if (NULL == context)
    {
        OIC_LOG(ERROR, TAG, "thread instance is empty..");
        return CA_STATUS_INVALID_PARAM;
    }

    if (NULL == handle)
    {
        OIC_LOG(ERROR, TAG, "thread pool handle is empty..");
        return CA_STATUS_INVALID_PARAM;
    }

    OIC_LOG(DEBUG, TAG, "thread initialize..");

    memset(context, 0, sizeof(CARetransmission_t));

    CARetransmissionConfig_t cfg = { 0 };

    if (NULL == config)
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
    context->threadMutex = ca_mutex_new();
    context->threadCond = ca_cond_new();
    context->dataSendMethod = retransmissionSendMethod;
    context->timeoutCallback = timeoutCallback;
    context->config = cfg;
    context->isStop = false;
    context->dataList = u_arraylist_create();

    return CA_STATUS_OK;
}

CAResult_t CARetransmissionStart(CARetransmission_t *context)
{
    if (NULL == context)
    {
        OIC_LOG(ERROR, TAG, "context is empty..");
        return CA_STATUS_INVALID_PARAM;
    }

    if (NULL == context->threadPool)
    {
        OIC_LOG(ERROR, TAG, "thread pool handle is empty..");
        return CA_STATUS_INVALID_PARAM;
    }

    CAResult_t res = ca_thread_pool_add_task(context->threadPool, CARetransmissionBaseRoutine,
                                            context);

    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "thread pool add task error(send thread).");
        return res;
    }

    return res;
}

CAResult_t CARetransmissionSentData(CARetransmission_t *context,
                                    const CARemoteEndpoint_t* endpoint, const void* pdu,
                                    uint32_t size)
{
    if (NULL == context || NULL == endpoint || NULL == pdu)
    {
        OIC_LOG(ERROR, TAG, "invalid parameter..");
        return CA_STATUS_INVALID_PARAM;
    }

    // #0. check support transport type
    if (!(context->config.supportType & endpoint->transportType))
    {
        OIC_LOG_V(DEBUG, TAG, "not supported transport type for retransmission..(%d)",
                  endpoint->transportType);
        return CA_NOT_SUPPORTED;
    }

    // #1. check PDU method type and get message id.
    CAMessageType_t type = CAGetMessageTypeFromPduBinaryData(pdu, size);
    uint16_t messageId = CAGetMessageIdFromPduBinaryData(pdu, size);

    OIC_LOG_V(DEBUG, TAG, "sent pdu, message type(%d), message id(%d)", type, messageId);

    if (CA_MSG_CONFIRM != type)
    {
        OIC_LOG(DEBUG, TAG, "not supported message type for retransmission..");
        return CA_NOT_SUPPORTED;
    }

    // create retransmission data
    CARetransmissionData_t *retData = (CARetransmissionData_t *) OICCalloc(
                                          1, sizeof(CARetransmissionData_t));

    if (NULL == retData)
    {
        OIC_LOG(ERROR, TAG, "memory error!!");
        return CA_MEMORY_ALLOC_FAILED;
    }

    // copy PDU data
    void *pduData = (void *) OICMalloc(size);
    if (NULL == pduData)
    {
        OICFree(retData);
        OIC_LOG(ERROR, TAG, "memory error!!");
        return CA_MEMORY_ALLOC_FAILED;
    }
    memcpy(pduData, pdu, size);

    // clone remote endpoint
    CARemoteEndpoint_t *remoteEndpoint = CACloneRemoteEndpoint(endpoint);
    if (NULL == remoteEndpoint)
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
    ca_mutex_lock(context->threadMutex);

    uint32_t i = 0;
    uint32_t len = u_arraylist_length(context->dataList);

    // #3. add data into list
    for (i = 0; i < len; i++)
    {
        CARetransmissionData_t *currData = u_arraylist_get(context->dataList, i);

        if (NULL == currData)
        {
            continue;
        }

        // found index
        if (NULL != currData->endpoint && currData->messageId == messageId
            && (currData->endpoint->transportType == endpoint->transportType))
        {
            OIC_LOG(ERROR, TAG, "Duplicate message ID");

            // mutex unlock
            ca_mutex_unlock(context->threadMutex);

            OICFree(retData);
            OICFree(pduData);
            OICFree(remoteEndpoint);
            return CA_STATUS_FAILED;
        }
    }

    u_arraylist_add(context->dataList, (void *) retData);

    // notify the thread
    ca_cond_signal(context->threadCond);

    // mutex unlock
    ca_mutex_unlock(context->threadMutex);

    return CA_STATUS_OK;
}

CAResult_t CARetransmissionReceivedData(CARetransmission_t *context,
                                        const CARemoteEndpoint_t *endpoint, const void *pdu,
                                        uint32_t size, void **retransmissionPdu)
{
    OIC_LOG(DEBUG, TAG, "IN - CARetransmissionReceivedData");
    if (NULL == context || NULL == endpoint || NULL == pdu || NULL == retransmissionPdu)
    {
        OIC_LOG(ERROR, TAG, "invalid parameter..");
        return CA_STATUS_INVALID_PARAM;
    }

    // #0. check support transport type
    if (!(context->config.supportType & endpoint->transportType))
    {
        OIC_LOG_V(DEBUG, TAG, "not supported transport type for retransmission..(%d)",
                  endpoint->transportType);
        return CA_STATUS_OK;
    }

    // #1. check PDU method type and get message id.
    // ACK, RST --> remove the CON data
    CAMessageType_t type = CAGetMessageTypeFromPduBinaryData(pdu, size);
    uint16_t messageId = CAGetMessageIdFromPduBinaryData(pdu, size);

    OIC_LOG_V(DEBUG, TAG, "received pdu, message type(%d), message id(%d)", type, messageId);

    if ((CA_MSG_ACKNOWLEDGE != type) && (CA_MSG_RESET != type))
    {
        return CA_STATUS_OK;
    }

    // mutex lock
    ca_mutex_lock(context->threadMutex);
    uint32_t len = u_arraylist_length(context->dataList);

    // find index
    uint32_t i;
    for (i = 0; i < len; i++)
    {
        CARetransmissionData_t *retData = (CARetransmissionData_t *) u_arraylist_get(
                context->dataList, i);

        if (NULL == retData)
        {
            continue;
        }

        // found index
        if (NULL != retData->endpoint && retData->messageId == messageId
            && (retData->endpoint->transportType == endpoint->transportType))
        {
            // get pdu data for getting token when CA_EMPTY(RST/ACK) is received from remote device
            // if retransmission was finish..token will be unavailable.
            if (CA_EMPTY == CAGetCodeFromPduBinaryData(pdu, size))
            {
                OIC_LOG(DEBUG, TAG, "code is CA_EMPTY..");

                if (NULL == retData->pdu)
                {
                    OIC_LOG(ERROR, TAG, "retData->pdu is null");
                    OICFree(retData);
                    // mutex unlock
                    ca_mutex_unlock(context->threadMutex);

                    return CA_STATUS_FAILED;
                }

                // copy PDU data
                (*retransmissionPdu) = (void *) OICCalloc(1, retData->size);
                if ((*retransmissionPdu) == NULL)
                {
                    OICFree(retData);
                    OIC_LOG(ERROR, TAG, "memory error!!");

                    // mutex unlock
                    ca_mutex_unlock(context->threadMutex);

                    return CA_MEMORY_ALLOC_FAILED;
                }
                memcpy((*retransmissionPdu), retData->pdu, retData->size);
            }

            // #2. remove data from list
            CARetransmissionData_t *removedData = u_arraylist_remove(context->dataList, i);
            if (NULL == removedData)
            {
                OIC_LOG(ERROR, TAG, "Removed data is NULL");

                // mutex unlock
                ca_mutex_unlock(context->threadMutex);

                return CA_STATUS_FAILED;
            }

            OIC_LOG_V(DEBUG, TAG, "remove retransmission CON data!!, message id(%d)",
                      messageId);

            CADestroyRemoteEndpointInternal(removedData->endpoint);
            OICFree(removedData->pdu);
            OICFree(removedData);

            break;
        }
    }

    // mutex unlock
    ca_mutex_unlock(context->threadMutex);

    OIC_LOG(DEBUG, TAG, "OUT - CARetransmissionReceivedData");
    return CA_STATUS_OK;
}

CAResult_t CARetransmissionStop(CARetransmission_t *context)
{
    if (NULL == context)
    {
        OIC_LOG(ERROR, TAG, "context is empty..");
        return CA_STATUS_INVALID_PARAM;
    }

    OIC_LOG(DEBUG, TAG, "retransmission stop request!!");

    // mutex lock
    ca_mutex_lock(context->threadMutex);

    // set stop flag
    context->isStop = true;

    // notify the thread
    ca_cond_signal(context->threadCond);

    ca_cond_wait(context->threadCond, context->threadMutex);

    // mutex unlock
    ca_mutex_unlock(context->threadMutex);

    return CA_STATUS_OK;
}

CAResult_t CARetransmissionDestroy(CARetransmission_t *context)
{
    if (NULL == context)
    {
        OIC_LOG(ERROR, TAG, "context is empty..");
        return CA_STATUS_INVALID_PARAM;
    }

    OIC_LOG(DEBUG, TAG, "retransmission context destroy..");

    ca_mutex_free(context->threadMutex);
    context->threadMutex = NULL;
    ca_cond_free(context->threadCond);
    u_arraylist_free(&context->dataList);

    return CA_STATUS_OK;
}

uint64_t getCurrentTimeInMicroSeconds()
{
    OIC_LOG(DEBUG, TAG, "IN");
    uint64_t currentTime = 0;

#ifdef __ANDROID__
    struct timespec getTs;

    clock_gettime(CLOCK_MONOTONIC, &getTs);

    currentTime = (getTs.tv_sec * (uint64_t)1000000000 + getTs.tv_nsec)/1000;
    OIC_LOG_V(DEBUG, TAG, "current time = %d", currentTime);
#else
#if _POSIX_TIMERS > 0
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    currentTime = ts.tv_sec * USECS_PER_SEC + ts.tv_nsec / 1000;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    currentTime = tv.tv_sec * USECS_PER_SEC + tv.tv_usec;
#endif
#endif

    OIC_LOG(DEBUG, TAG, "OUT");
    return currentTime;
}

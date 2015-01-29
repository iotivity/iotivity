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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <glib.h>

#ifdef __ANDROID__
#include <linux/time.h>
#endif
#include "coap.h"
#include "caretransmission.h"
#include "caremotehandler.h"
#include "caprotocolmessage.h"
#include "oic_malloc.h"
#include "logger.h"

#define TAG PCF("CA")

typedef struct
{
    /** last sent time. microseconds **/
    uint64_t timeStamp;
    /** timeout value. milliseconds **/
    uint32_t timeout;
    /** retransmission count **/
    uint8_t triedCount;
    /** coap PDU message id **/
    uint16_t messageId;
    /** remote endpoint **/
    CARemoteEndpoint_t *endpoint;
    /** coap PDU **/
    void *pdu;
    /** coap PDU size**/
    uint32_t size;
} CARetransmissionData_t;

/**
 * getCurrent monotonic time
 *
 * microseconds
 */
uint64_t getCurrentTimeInMicroSeconds();

/**
 * @brief   check timeout routine
 * @param   currentTime     [IN]microseconds
 * @param   timeStamp       [IN]microseconds
 * @param   timeoutValue    [IN]milliseconds
 * @param   triedCount      [IN]
 * @return  CA_TRUE(timeout) or CA_FALSE
 *
 * microseconds
 */
static CABool_t CACheckTimeout(uint64_t currentTime, uint64_t timeStamp, uint32_t timeoutValue,
        uint8_t triedCount)
{
    // #1. calculate timeout
    uint64_t timeout = (timeoutValue << triedCount) * (uint64_t) 1000;

    if (currentTime >= timeStamp + timeout)
    {
        OIC_LOG_V(DEBUG, TAG, "%d milliseconds time out!!, tried count(%d)",
            (timeoutValue << triedCount), triedCount);
        return CA_TRUE;
    }

    return CA_FALSE;
}

/**
 * @brief   timeout value is
 *          between DEFAULT_ACK_TIMEOUT and (DEFAULT_ACK_TIMEOUT * DEFAULT_RANDOM_FACTOR) second.
 *          DEFAULT_RANDOM_FACTOR       1.5 (CoAP)
 * @return  milliseconds.
 */
static uint32_t CAGetTimeoutValue()
{
    return (DEFAULT_ACK_TIMEOUT * 1000) + ((1000 * (rand() & 0xFF)) >> 8);
}

static void CACheckRetransmissionList(CARetransmission_t *context)
{
    // mutex lock
    u_mutex_lock(context->threadMutex);
    uint64_t currentTime = 0;

    uint32_t i = 0;
    uint32_t len = u_arraylist_length(context->dataList);

    for (i = 0; i < len; i++)
    {
        CARetransmissionData_t *retData = u_arraylist_get(context->dataList, i);

        if (retData == NULL)
            continue;

        currentTime = getCurrentTimeInMicroSeconds();

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
                OIC_LOG_V(DEBUG, TAG, "arraylist remove error");
            }

        }
    }

    // mutex unlock
    u_mutex_unlock(context->threadMutex);
}

static void CARetransmissionBaseRoutine(void *threadValue)
{
    OIC_LOG_V(DEBUG, TAG, "retransmission main thread start..");

    CARetransmission_t *context = (CARetransmission_t *) threadValue;

    if (context == NULL)
    {
        OIC_LOG_V(DEBUG, TAG, "thread data passing error!!");

        return;
    }

    while (!context->isStop)
    {
        // mutex lock
        u_mutex_lock(context->threadMutex);

        if (u_arraylist_length(context->dataList) <= 0)
        {
            // if list is empty, thread will wait
            OIC_LOG_V(DEBUG, TAG, "wait..there is no retransmission data.");

            // wait
            u_cond_wait(context->threadCond, context->threadMutex);

            OIC_LOG_V(DEBUG, TAG, "wake up..");
        }
        else
        {
            // check each RETRANSMISSION_CHECK_PERIOD time.
            OIC_LOG_V(DEBUG, TAG, "wait..(%d)microseconds", RETRANSMISSION_CHECK_PERIOD);

            // wait
            u_cond_timed_wait(context->threadCond, context->threadMutex,
                              RETRANSMISSION_CHECK_PERIOD);
        }

        // mutex unlock
        u_mutex_unlock(context->threadMutex);

        // check stop flag
        if (context->isStop)
            continue;

        CACheckRetransmissionList(context);
    }

    u_cond_signal(context->threadCond);

    OIC_LOG_V(DEBUG, TAG, "retransmission main thread end..");

}

CAResult_t CARetransmissionInitialize(CARetransmission_t *context, u_thread_pool_t handle,
        CADataSendMethod_t retransmissionSendMethod, CATimeoutCallback_t timeoutCallback,
        CARetransmissionConfig_t* config)
{
    if (context == NULL)
    {
        OIC_LOG_V(DEBUG, TAG, "thread instance is empty..");
        return CA_STATUS_FAILED;
    }

    if (handle == NULL)
    {
        OIC_LOG_V(DEBUG, TAG, "thread pool handle is empty..");
        return CA_STATUS_FAILED;
    }

    OIC_LOG_V(DEBUG, TAG, "thread initialize..");

    memset(context, 0, sizeof(CARetransmission_t));

    CARetransmissionConfig_t cfg;
    memset(&cfg, 0, sizeof(CARetransmissionConfig_t));

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

    // mutex init
    u_mutex_init();

    // set send thread data
    context->threadPool = handle;
    context->threadMutex = u_mutex_new();
    context->threadCond = u_cond_new();
    context->dataSendMethod = retransmissionSendMethod;
    context->timeoutCallback = timeoutCallback;
    context->config = cfg;
    context->isStop = CA_FALSE;
    context->dataList = u_arraylist_create();

    return CA_STATUS_OK;
}

CAResult_t CARetransmissionStart(CARetransmission_t *context)
{
    if (context == NULL)
    {
        OIC_LOG_V(DEBUG, TAG, "context is empty..");
        return CA_STATUS_FAILED;
    }

    if (context->threadPool == NULL)
    {
        OIC_LOG_V(DEBUG, TAG, "thread pool handle is empty..");
        return CA_STATUS_FAILED;
    }

    CAResult_t res = u_thread_pool_add_task(context->threadPool, CARetransmissionBaseRoutine,
                                            context);

    if (res != CA_STATUS_OK)
    {
        OIC_LOG_V(DEBUG, TAG, "thread pool add task error(send thread).");
        return res;
    }

    return res;
}

CAResult_t CARetransmissionSentData(CARetransmission_t *context,
    const CARemoteEndpoint_t* endpoint, const void* pdu, uint32_t size)
{
    if (context == NULL || endpoint == NULL || pdu == NULL)
    {
        OIC_LOG_V(DEBUG, TAG, "invalid parameter..");
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
    CARetransmissionData_t *retData = (CARetransmissionData_t *) OICMalloc(
                                          sizeof(CARetransmissionData_t));

    if (retData == NULL)
    {
        OIC_LOG_V(DEBUG, TAG, "memory error!!");
        return CA_MEMORY_ALLOC_FAILED;
    }
    memset(retData, 0, sizeof(CARetransmissionData_t));

    // copy PDU data
    void *pduData = (void *) OICMalloc(sizeof(int8_t) * size);
    if (pduData == NULL)
    {
        OICFree(retData);
        OIC_LOG_V(DEBUG, TAG, "memory error!!");
        return CA_MEMORY_ALLOC_FAILED;
    }
    memset(pduData, 0, sizeof(int8_t) * size);
    memcpy(pduData, pdu, sizeof(int8_t) * size);

    // clone remote endpoint
    CARemoteEndpoint_t *remoteEndpoint = CACloneRemoteEndpoint(endpoint);
    if (remoteEndpoint == NULL)
    {
        OICFree(retData);
        OICFree(pduData);
        OIC_LOG_V(DEBUG, TAG, "memory error!!");
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

    // #3. add data into list
    u_arraylist_add(context->dataList, (void *) retData);

    // notity the thread
    u_cond_signal(context->threadCond);

    // mutex unlock
    u_mutex_unlock(context->threadMutex);

    return CA_STATUS_OK;
}

CAResult_t CARetransmissionReceivedData(CARetransmission_t *context,
                                        const CARemoteEndpoint_t *endpoint,
                                        const void *pdu, uint32_t size,
                                        void **retransmissionPdu)
{
    OIC_LOG_V(DEBUG, TAG, "IN - CARetransmissionReceivedData");
    if (context == NULL || endpoint == NULL || pdu == NULL)
    {
        OIC_LOG_V(DEBUG, TAG, "invalid parameter..");
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

        if (retData == NULL)
            continue;

        // found index
        if ((retData->endpoint->connectivityType == endpoint->connectivityType)
            && retData->messageId == messageId)
        {
            // get pdu data for getting token when CA_EMPTY(RST/ACK) is received from remote device
            // if retransmission was finish..token will be unavailable.
            if(CA_EMPTY == CAGetCodeFromPduBinaryData(pdu, size))
            {
                OIC_LOG_V(DEBUG, TAG, "code is CA_EMPTY..");

                if(NULL == retData->pdu)
                {
                    OIC_LOG_V(DEBUG, TAG, "retData->pdu is null");
                }

                // copy PDU data
                (*retransmissionPdu) = (void *) OICMalloc(sizeof(int8_t) * retData->size);
                if ((*retransmissionPdu) == NULL)
                {
                    OICFree(retData);
                    OIC_LOG_V(DEBUG, TAG, "memory error!!");
                    return CA_MEMORY_ALLOC_FAILED;
                }
                memset((*retransmissionPdu), 0, sizeof(int8_t) * retData->size);
                memcpy((*retransmissionPdu), retData->pdu, sizeof(int8_t) * retData->size);
            }

            break;
        }
    }

    // #2. remove data from list
    if (i < len)
    {
        CARetransmissionData_t *removedData = u_arraylist_remove(context->dataList, i);

        if (removedData != NULL)
        {
            OIC_LOG_V(DEBUG, TAG, "remove retransmission CON data!!, message id(%d)", messageId);

            CADestroyRemoteEndpointInternal(removedData->endpoint);
            OICFree(removedData->pdu);

            OICFree(removedData);
        }
    }

    // mutex unlock
    u_mutex_unlock(context->threadMutex);

    OIC_LOG_V(DEBUG, TAG, "OUT - CARetransmissionReceivedData");
    return CA_STATUS_OK;
}

CAResult_t CARetransmissionStop(CARetransmission_t *context)
{
    if (context == NULL)
    {
        OIC_LOG_V(DEBUG, TAG, "context is empty..");
        return CA_STATUS_FAILED;
    }

    OIC_LOG_V(DEBUG, TAG, "retransmission stop request!!");

    // mutex lock
    u_mutex_lock(context->threadMutex);

    // set stop flag
    context->isStop = CA_TRUE;

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
        OIC_LOG_V(DEBUG, TAG, "context is empty..");
        return CA_STATUS_FAILED;
    }

    OIC_LOG_V(DEBUG, TAG, "retransmission context destroy..");

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

    memset(&getTs, 0, sizeof(getTs));
    clock_gettime(CLOCK_MONOTONIC, &getTs);

    currentTime = (getTs.tv_sec * (uint64_t)1000000000 + getTs.tv_nsec)/1000;
    OIC_LOG_V(DEBUG, TAG, "current time = %d", currentTime);
#else
    currentTime = g_get_monotonic_time();
#endif
    return currentTime;
}

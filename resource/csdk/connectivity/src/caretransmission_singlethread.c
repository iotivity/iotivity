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
#include "caretransmission_singlethread.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "caremotehandler.h"
#include "caprotocolmessage.h"
#include "oic_malloc.h"
#include "logger.h"

#ifndef __ARDUINO__
#include <sys/time.h>
#endif

#define TAG "RT"

typedef struct
{
    /** last sent time. microseconds **/
    uint64_t timeStamp;

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

static CARetransmission_t *g_retransmissionPtr = NULL;

/**
 * getCurrent monotonic time.
 *
 * @return current time in microseconds.
 */
uint64_t getCurrentTimeInMicroSeconds();

/**
 * @brief   check timeout routine
 * @param   currentTime     [IN]microseconds
 * @param   timeStamp       [IN]microseconds
 * @param   triedCount      [IN]Number of retransmission tried.
 * @return  true if the timeout period has elapsed, false otherwise.
 */
static bool CACheckTimeout(uint64_t currentTime, uint64_t timeStamp, uint8_t triedCount)
{
    OIC_LOG(DEBUG, TAG, "IN");
    // #1. calculate timeout
    uint64_t timeOut = (2 << triedCount) * 1000000;

    if (currentTime >= timeStamp + timeOut)
    {
        OIC_LOG_V(DEBUG, TAG, "timeout=%d, tried cnt=%d", (2 << triedCount), triedCount);
        return true;
    }

    OIC_LOG(DEBUG, TAG, "OUT");
    return false;
}

void CACheckRetransmissionList()
{
    uint32_t len = u_arraylist_length(g_retransmissionPtr->dataList);

    OIC_LOG_V(DEBUG, TAG, "len=%d", len);
    for (uint32_t i = 0; i < len; i++)
    {
        CARetransmissionData_t *retData =
                (CARetransmissionData_t *) u_arraylist_get(g_retransmissionPtr->dataList, i);

        if (NULL == retData)
        {
            continue;
        }

        uint64_t currentTime = getCurrentTimeInMicroSeconds();

        OIC_LOG_V(DEBUG, TAG, "currtime=%lu", currentTime);
        if (CACheckTimeout(currentTime, retData->timeStamp, retData->triedCount))
        {

            OIC_LOG(DEBUG, TAG, "RTdata-Success");
            // #2. if time's up, send the data.
            if (NULL != g_retransmissionPtr->dataSendMethod)
            {
                OIC_LOG_V(DEBUG, TAG, "retry CON data-msgid=%d", retData->messageId);
                g_retransmissionPtr->dataSendMethod(retData->endpoint, retData->pdu, retData->size);
            }

            // #3. increase the retransmission count and update timestamp.
            retData->timeStamp = currentTime;
            retData->triedCount++;
        }

        // #4. if tried count is max, remove the retransmission data from list.
        if (retData->triedCount >= g_retransmissionPtr->config.tryingCount)
        {
            CARetransmissionData_t *removedData = (CARetransmissionData_t *) u_arraylist_remove(
                    g_retransmissionPtr->dataList, i);
            if (NULL == removedData)
            {
                OIC_LOG(ERROR, TAG, "Removed data is NULL");
                return;
            }

            OIC_LOG(DEBUG, TAG, "max trycount rchd");

            OIC_LOG_V(DEBUG, TAG, "max trycount, remove retransmission CON data!!, messageid=%d",
                      removedData->messageId);

            // callback for retransmit timeout
            if (NULL != g_retransmissionPtr->timeoutCallback)
            {
                g_retransmissionPtr->timeoutCallback(removedData->endpoint, removedData->pdu,
                                                     removedData->size);
            }

            CADestroyRemoteEndpointInternal(removedData->endpoint);
            OICFree(removedData->pdu);

            OICFree(removedData);

            // modify loop value.
            len = u_arraylist_length(g_retransmissionPtr->dataList);
            --i;
        }
    }
}

void CARetransmissionBaseRoutine(void *threadValue)
{
    CARetransmission_t *context = (CARetransmission_t *) threadValue;

    if (NULL == context)
    {
        OIC_LOG(ERROR, TAG, "cnxt null");
        return;
    }

    if (true == context->isStop)
    {
        OIC_LOG(DEBUG, TAG, "thread stopped");
        return;
    }
    g_retransmissionPtr = context;
    CACheckRetransmissionList();
}

CAResult_t CARetransmissionInitialize(CARetransmission_t *context,
                                      CADataSendMethod_t retransmissionSendMethod,
                                      CATimeoutCallback_t timeoutCallback,
                                      CARetransmissionConfig_t *config)
{
    OIC_LOG(DEBUG, TAG, "IN");
    if (NULL == context)
    {
        OIC_LOG(ERROR, TAG, "cnxt null");
        return CA_STATUS_INVALID_PARAM;
    }

    memset(context, 0, sizeof(CARetransmission_t));

    CARetransmissionConfig_t cfg;
    memset(&cfg, 0, sizeof(CARetransmissionConfig_t));

    if (NULL == config)
    {
        // setDefault
        cfg.supportType = (CATransportType_t) DEFAULT_RETRANSMISSION_TYPE;
        cfg.tryingCount = DEFAULT_RETRANSMISSION_COUNT;
    }
    else
    {
        cfg = *config;
    }

    // set send thread data
    context->dataSendMethod = retransmissionSendMethod;
    context->timeoutCallback = timeoutCallback;
    context->config = cfg;
    context->isStop = false;
    context->dataList = u_arraylist_create();

    // Enable TimedAction for CACheckRetransmissionList API
    g_retransmissionPtr = context;
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CARetransmissionSentData(CARetransmission_t *context, const CARemoteEndpoint_t *endpoint,
                                    const void *pdu, uint32_t size)
{
    OIC_LOG(DEBUG, TAG, "IN");
    if (NULL == context || NULL == endpoint || NULL == pdu)
    {
        OIC_LOG(ERROR, TAG, "error");
        return CA_STATUS_INVALID_PARAM;
    }

    // #0. check support connectivity type
    if (!(context->config.supportType & endpoint->transportType))
    {
        OIC_LOG(ERROR, TAG, "error");
        OIC_LOG_V(ERROR, TAG, "not supported conntype=%d", endpoint->transportType);
        return CA_NOT_SUPPORTED;
    }

    // #1. check PDU method type and get message id.
    CAMessageType_t type = CAGetMessageTypeFromPduBinaryData(pdu, size);
    uint16_t messageId = CAGetMessageIdFromPduBinaryData(pdu, size);

    OIC_LOG_V(DEBUG, TAG, "sent pdu, msgtype=%d,msgid=%d", type, messageId);

    if (CA_MSG_CONFIRM != type)
    {
        OIC_LOG(DEBUG, TAG, "not supported message type");
        return CA_NOT_SUPPORTED;
    }

    // create retransmission data
    CARetransmissionData_t *retData = (CARetransmissionData_t *) OICCalloc(
            1, sizeof(CARetransmissionData_t));

    if (NULL == retData)
    {
        OIC_LOG(ERROR, TAG, "error");
        return CA_MEMORY_ALLOC_FAILED;
    }

    // copy PDU data
    void *pduData = (void *) OICMalloc(size);
    if (NULL == pduData)
    {
        OICFree(retData);
        OIC_LOG(ERROR, TAG, "error");
        return CA_MEMORY_ALLOC_FAILED;
    }
    memcpy(pduData, pdu, size);

    // clone remote endpoint
    CARemoteEndpoint_t *remoteEndpoint = CACloneRemoteEndpoint(endpoint);
    if (NULL == remoteEndpoint)
    {
        OICFree(retData);
        OICFree(pduData);
        OIC_LOG(ERROR, TAG, "error");
        return CA_MEMORY_ALLOC_FAILED;
    }

    // #2. add additional information. (time stamp, retransmission count...)
    retData->timeStamp = getCurrentTimeInMicroSeconds();
    retData->triedCount = 0;
    retData->messageId = messageId;
    retData->endpoint = remoteEndpoint;
    retData->pdu = pduData;
    retData->size = size;

    // #3. add data into list
    u_arraylist_add(context->dataList, (void *) retData);

    // #4. Initiate Re-transmission for added entry
    g_retransmissionPtr = context;
    CACheckRetransmissionList();
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CARetransmissionReceivedData(CARetransmission_t *context,
                                        const CARemoteEndpoint_t *endpoint, const void *pdu,
                                        uint32_t size, void **retransmissionPdu)
{
    OIC_LOG(DEBUG, TAG, "IN");
    if (NULL == context || NULL == endpoint || NULL == pdu || NULL == retransmissionPdu)
    {
        OIC_LOG(ERROR, TAG, "error");
        return CA_STATUS_INVALID_PARAM;
    }

    // #0. check support connectivity type
    if (!(context->config.supportType & endpoint->transportType))
    {
        OIC_LOG_V(DEBUG, TAG, "not supp conntype=%d", endpoint->transportType);
        return CA_STATUS_OK;
    }

    // #1. check PDU method type and get message id.
    // ACK, RST --> remove the CON data
    CAMessageType_t type = CAGetMessageTypeFromPduBinaryData(pdu, size);
    uint16_t messageId = CAGetMessageIdFromPduBinaryData(pdu, size);

    OIC_LOG_V(DEBUG, TAG, "recv pdu, msgtype=%d,msgid=%d", type, messageId);

    if (CA_MSG_ACKNOWLEDGE != type && CA_MSG_RESET != type)
    {
        return CA_STATUS_OK;
    }

    uint32_t len = u_arraylist_length(context->dataList);

    // find index
    for (uint32_t i = 0; i < len; i++)
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
                OIC_LOG(DEBUG, TAG, "CA_EMPTY");

                if (NULL == retData->pdu)
                {
                    OIC_LOG(ERROR, TAG, "retData->pdu is null");
                    OICFree(retData);
                    return CA_STATUS_FAILED;
                }

                // copy PDU data
                (*retransmissionPdu) = (void *) OICCalloc(1, retData->size);
                if (NULL == (*retransmissionPdu))
                {
                    OICFree(retData);
                    OIC_LOG(ERROR, TAG, "error");
                    return CA_MEMORY_ALLOC_FAILED;
                }
                memcpy((*retransmissionPdu), retData->pdu, retData->size);
            }

            // #2. remove data from list
            CARetransmissionData_t *removedData = (CARetransmissionData_t *) u_arraylist_remove(
                    context->dataList, i);
            if (NULL == removedData)
            {
                OIC_LOG(ERROR, TAG, "Removed data is NULL");
                return CA_STATUS_FAILED;
            }

            OIC_LOG_V(DEBUG, TAG, "remove RTCON data, msgid=%d", messageId);

            CADestroyRemoteEndpointInternal(removedData->endpoint);
            OICFree(removedData->pdu);

            OICFree(removedData);

            break;
        }
    }

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CARetransmissionStop(CARetransmission_t *context)
{
    OIC_LOG(DEBUG, TAG, "IN");
    if (NULL == context)
    {
        OIC_LOG(ERROR, TAG, "error");
        return CA_STATUS_INVALID_PARAM;
    }

    // set stop flag
    context->isStop = true;
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CARetransmissionDestroy(CARetransmission_t *context)
{
    OIC_LOG(DEBUG, TAG, "IN");
    if (NULL == context)
    {
        OIC_LOG(ERROR, TAG, "error");
        return CA_STATUS_INVALID_PARAM;
    }

    u_arraylist_free(&context->dataList);
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

uint64_t getCurrentTimeInMicroSeconds()
{
    OIC_LOG(DEBUG, TAG, "IN");
    uint64_t currentTime = 0;

#ifdef __ARDUINO__
    currentTime = millis() * 1000;

    OIC_LOG_V(DEBUG, TAG, "currtime=%lu", currentTime);
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    currentTime = tv.tv_sec * USECS_PER_SEC + tv.tv_usec;
#endif

    OIC_LOG(DEBUG, TAG, "OUT");
    return currentTime;
}


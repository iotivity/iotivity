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
#include <TimedAction.h>

#include "caremotehandler.h"
#include "caprotocolmessage_singlethread.h"
#include "oic_malloc.h"
#include "logger.h"

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

static TimedAction gRcvAction = TimedAction(2000, CACheckRetransmissionList);
static CARetransmission_t *gRetransmissionPtr = NULL;

/**
 * getCurrent monotonic time
 *
 * microseconds
 */
uint64_t getCurrentTimeInMicroSeconds();

/**
 * timeout routine
 * 2sec -> 4sec -> 8sec -> 16sec
 *
 * microseconds
 */
static CABool_t CACheckTimeout(uint64_t currentTime, uint64_t timeStamp, uint8_t triedCount)
{
    OIC_LOG(DEBUG, TAG, "IN");
    // #1. calculate timeout
    uint64_t timeOut = (2 << triedCount) * 1000000;

    if (currentTime >= timeStamp + timeOut)
    {
        OIC_LOG_V(DEBUG, TAG, "timeout=%d, tried cnt=%d", (2 << triedCount), triedCount);
        return CA_TRUE;
    }

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_FALSE;
}

void CACheckRetransmissionList()
{
    OIC_LOG(DEBUG, TAG, "IN");
    uint64_t currentTime = 0;

    uint32_t i = 0;
    uint32_t len = u_arraylist_length(gRetransmissionPtr->dataList);

    OIC_LOG_V(DEBUG, TAG, "len=%d", len);
    for (i = 0; i < len; i++)
    {
        CARetransmissionData_t *retData =
            (CARetransmissionData_t *)u_arraylist_get(gRetransmissionPtr->dataList, i);

        if (retData == NULL)
            continue;

        currentTime = getCurrentTimeInMicroSeconds();

        OIC_LOG_V(DEBUG, TAG, "currtime=%lu", currentTime);
        if (CACheckTimeout(currentTime, retData->timeStamp, retData->triedCount))
        {

            OIC_LOG(DEBUG, TAG, "RTdata-Success");
            // #2. if time's up, send the data.
            if (gRetransmissionPtr->dataSendMethod != NULL)
            {
                OIC_LOG_V(DEBUG, TAG, "retry CON data-msgid=%d", retData->messageId);
                gRetransmissionPtr->dataSendMethod(retData->endpoint, retData->pdu, retData->size);
            }

            // #3. increase the retransmission count and update timestamp.
            retData->timeStamp = currentTime;
            retData->triedCount++;
        }

        // #4. if tried count is max, remove the retransmission data from list.
        if (retData->triedCount >= gRetransmissionPtr->config.tryingCount)
        {
            CARetransmissionData_t *removedData =
                (CARetransmissionData_t *)u_arraylist_remove(gRetransmissionPtr->dataList, i);
            if (NULL == removedData)
            {
                OIC_LOG(DEBUG, TAG, "Removed data is NULL");
                return;
            }
            OIC_LOG(DEBUG, TAG, "max trycount rchd");
            OIC_LOG_V(DEBUG, TAG, "max trycount, remove retransmission CON data!!, messageid=%d",
                      removedData->messageId);

            // callback for retransmit timeout
            if (gRetransmissionPtr->timeoutCallback != NULL)
            {
                gRetransmissionPtr->timeoutCallback(removedData->endpoint, removedData->pdu,
                        removedData->size);
            }

            CADestroyRemoteEndpointInternal(removedData->endpoint);
            OICFree(removedData->pdu);

            OICFree(removedData);

            // modify loop value.
            len = u_arraylist_length(gRetransmissionPtr->dataList);
            --i;
        }
    }
    OIC_LOG(DEBUG, TAG, "OUT");
}

void CARetransmissionBaseRoutine(void *threadValue)
{
    CARetransmission_t *context = (CARetransmission_t *) threadValue;

    if (context == NULL)
    {
        OIC_LOG_V(DEBUG, TAG, "error");
        return;
    }

    if (CA_TRUE == context->isStop)
    {
        OIC_LOG(DEBUG, TAG, "Not reqd");
        return;
    }
    gRetransmissionPtr = context;
    CACheckRetransmissionList();
}

CAResult_t CARetransmissionInitialize(CARetransmission_t *context,
                                      CADataSendMethod_t retransmissionSendMethod,
                                      CATimeoutCallback_t timeoutCallback,
                                      CARetransmissionConfig_t *config)
{
    OIC_LOG(DEBUG, TAG, "IN");
    if (context == NULL)
    {
        OIC_LOG_V(DEBUG, TAG, "error");
        return CA_STATUS_FAILED;
    }

    memset(context, 0, sizeof(CARetransmission_t));

    CARetransmissionConfig_t cfg;
    memset(&cfg, 0, sizeof(CARetransmissionConfig_t));

    if (config == NULL)
    {
        // setDefault
        cfg.supportType = (CAConnectivityType_t)DEFAULT_RETRANSMISSION_TYPE;
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
    context->isStop = CA_FALSE;
    context->dataList = u_arraylist_create();

    // Enable TimedAction for CACheckRetransmissionList API
    gRetransmissionPtr = context;
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CARetransmissionSentData(CARetransmission_t *context,
                                    const CARemoteEndpoint_t *endpoint,
                                    const void *pdu, uint32_t size)
{
    OIC_LOG(DEBUG, TAG, "IN");
    if (context == NULL || endpoint == NULL || pdu == NULL)
    {
        OIC_LOG(DEBUG, TAG, "error");
        return CA_STATUS_INVALID_PARAM;
    }

    // #0. check support connectivity type
    if (!(context->config.supportType & endpoint->connectivityType))
    {
        OIC_LOG(DEBUG, TAG, "error");
        OIC_LOG_V(DEBUG, TAG, "not supported conntype=%d", endpoint->connectivityType);
        return CA_STATUS_OK;
    }

    // #1. check PDU method type and get message id.
    CAMessageType_t type = CAGetMessageTypeFromPduBinaryData(pdu, size);
    uint16_t messageId = CAGetMessageIdFromPduBinaryData(pdu, size);

    OIC_LOG_V(DEBUG, TAG, "sent pdu, msgtype=%d,msgid=%d", type, messageId);

    if (type != CA_MSG_CONFIRM)
    {
        OIC_LOG(DEBUG, TAG, "error");
        return CA_STATUS_OK;
    }

    // create retransmission data
    CARetransmissionData_t *retData = (CARetransmissionData_t *) OICMalloc(
                                          sizeof(CARetransmissionData_t));

    if (retData == NULL)
    {
        OIC_LOG(DEBUG, TAG, "error");
        return CA_MEMORY_ALLOC_FAILED;
    }
    memset(retData, 0, sizeof(CARetransmissionData_t));

    // copy PDU data
    void *pduData = (void *) OICMalloc(sizeof(int8_t) * size);
    if (pduData == NULL)
    {
        OICFree(retData);
        OIC_LOG(DEBUG, TAG, "error");
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
        OIC_LOG(DEBUG, TAG, "error");
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
    gRetransmissionPtr = context;
    CACheckRetransmissionList();
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CARetransmissionReceivedData(CARetransmission_t *context,
                                        const CARemoteEndpoint_t *endpoint, const void *pdu, uint32_t size)
{
    OIC_LOG(DEBUG, TAG, "IN");
    if (context == NULL || endpoint == NULL || pdu == NULL)
    {
        OIC_LOG_V(DEBUG, TAG, "error");
        return CA_STATUS_INVALID_PARAM;
    }

    // #0. check support connectivity type
    if (!(context->config.supportType & endpoint->connectivityType))
    {
        OIC_LOG_V(DEBUG, TAG, "not supp conntype=%d", endpoint->connectivityType);
        return CA_STATUS_OK;
    }

    // #1. check PDU method type and get message id.
    // ACK, RST --> remove the CON data
    CAMessageType_t type = CAGetMessageTypeFromPduBinaryData(pdu, size);
    uint16_t messageId = CAGetMessageIdFromPduBinaryData(pdu, size);

    OIC_LOG_V(DEBUG, TAG, "recv pdu, msgtype=%d,msgid=%d", type, messageId);

    if (type != CA_MSG_ACKNOWLEDGE && type != CA_MSG_RESET)
    {
        return CA_STATUS_OK;
    }

    uint32_t i = 0;
    uint32_t len = u_arraylist_length(context->dataList);

    // find index
    for (i = 0; i < len; i++)
    {
        CARetransmissionData_t *retData =
            (CARetransmissionData_t *)u_arraylist_get(context->dataList, i);

        if (retData == NULL)
            continue;

        // found index
        if ((retData->endpoint->connectivityType == endpoint->connectivityType)
            && retData->messageId == messageId)
            break;
    }

    // #2. remove data from list
    if (i < len)
    {
        CARetransmissionData_t *removedData =
            (CARetransmissionData_t *)u_arraylist_remove(context->dataList, i);
        if (NULL == removedData)
        {
            OIC_LOG(DEBUG, TAG, "Removed data is NULL");
            return CA_STATUS_FAILED;
        }

        OIC_LOG_V(DEBUG, TAG, "remove RTCON data, msgid=%d", messageId);

        CADestroyRemoteEndpointInternal(removedData->endpoint);
        OICFree(removedData->pdu);

        OICFree(removedData);
    }
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CARetransmissionStop(CARetransmission_t *context)
{
    OIC_LOG(DEBUG, TAG, "IN");
    if (context == NULL)
    {
        OIC_LOG_V(DEBUG, TAG, "error");
        return CA_STATUS_FAILED;
    }

    // set stop flag
    context->isStop = CA_TRUE;
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CARetransmissionDestroy(CARetransmission_t *context)
{
    OIC_LOG(DEBUG, TAG, "IN");
    if (context == NULL)
    {
        OIC_LOG_V(DEBUG, TAG, "error");
        return CA_STATUS_FAILED;
    }

    u_arraylist_free(context->dataList);
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

uint64_t getCurrentTimeInMicroSeconds()
{
    OIC_LOG(DEBUG, TAG, "IN");
    uint64_t currentTime = 0;

    /*
    #ifdef __ANDROID__
        struct timespec getTs;

        memset(&getTs, 0, sizeof(getTs));
        clock_gettime(CLOCK_MONOTONIC, &getTs);

        currentTime = (getTs.tv_sec * 1000000000 + getTs.tv_nsec)/1000;
        OIC_LOG_V(DEBUG, TAG, "current time = %d",  currentTime);
    #else if __ARDUINO__
        currentTime = micros();
    #else
        currentTime = g_get_monotonic_time();
    */
#ifdef __ARDUINO__
    currentTime = micros();

    OIC_LOG_V(DEBUG, TAG, "currtime=%lu", currentTime);
#else
    currentTime = g_get_monotonic_time();
#endif

    OIC_LOG(DEBUG, TAG, "OUT");
    return currentTime;
}

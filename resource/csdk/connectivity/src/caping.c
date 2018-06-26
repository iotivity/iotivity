/******************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
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
/**
 * @file
 * This file contains the implementation for the COAP ping and pong messages
 * over reliable transports like TCP and websockets.
 */

#include "caping.h"
#include "camessagehandler.h"
#include "octhread.h"
#include "oic_malloc.h"
#include "experimental/logger.h"
#include "oic_time.h"
#include "oic_string.h"
#include "catcpadapter.h"

#define TAG "OIC_CA_PING"

/**
 * List to hold the ping information
 */
static PingInfo *g_pingInfoList = NULL;

/**
 * Mutex to synchronize access to the ping information list
 */
static oc_mutex g_pingInfoListMutex = NULL;

/**
 * Timeout for ping messages
 */
static uint64_t g_timeout = CA_DEFAULT_PING_TIMEOUT;

static void CADestroyPingInfo(PingInfo* pingInfo)
{
    pingInfo->cbData.cd(pingInfo->cbData.context);
    OICFree(pingInfo->token);
    OICFree(pingInfo);
}

CAResult_t CAInitializePing()
{
    OIC_LOG(DEBUG, TAG, "CAInitializePing IN");

    g_pingInfoListMutex = oc_mutex_new();
    if (!g_pingInfoListMutex)
    {
        OIC_LOG(ERROR, TAG, "Could not create mutex");
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TAG, "CAInitializePing OUT");
    return CA_STATUS_OK;
}

void CASendCSMMessage(const CAEndpoint_t *endpoint)
{
    CAData_t *data = NULL;
    OIC_LOG_V(DEBUG, TAG, "Generate CSM message for [%s:%d]",
              endpoint->addr, endpoint->port);

    uint8_t numOptions = 0;
    CAHeaderOption_t *csmOpts = NULL;
    unsigned int maxMsgSize = 1152;
    unsigned char optionData[CA_MAX_HEADER_OPTION_DATA_LENGTH] = { 0 };
    size_t optionDataLength = coap_encode_var_bytes(optionData, maxMsgSize);
    CAAddHeaderOption(&csmOpts, &numOptions, CA_OPTION_MAX_MESSAGE_SIZE,
                      optionData, optionDataLength);

    data = CAGenerateSignalingMessage(endpoint, CA_CSM, csmOpts, numOptions);
    if (!data)
    {
        OIC_LOG(ERROR, TAG, "GenerateSignalingMessage failed");
        return;
    }
    OICFree(csmOpts);

    CAAddDataToSendThread(data);
}

CAResult_t CASendPingMessage(const CAEndpoint_t *endpoint, bool withCustody, CAPongCallbackData *cbData)
{
    OIC_LOG(DEBUG, TAG, "CASendPingMessage IN");

    // Check if ping message has already been sent to this endpoint
    oc_mutex_lock(g_pingInfoListMutex);
    PingInfo *cur = g_pingInfoList;
    while (cur)
    {
        if (!strcmp(cur->endpoint.addr, endpoint->addr))
        {
            OIC_LOG_V(ERROR, TAG, "Ping already sent to [%s]", cur->endpoint.addr);
            return CA_STATUS_FAILED;
        }
    }
    oc_mutex_unlock(g_pingInfoListMutex);

    cur = (PingInfo*)OICMalloc(sizeof(PingInfo));
    if (!cur)
    {
        OIC_LOG(ERROR, TAG, "Could not allocate memory for ping info");
        return CA_MEMORY_ALLOC_FAILED;
    }

    CAData_t *data = NULL;
#ifdef TCP_ADAPTER
    if (CA_ADAPTER_TCP == endpoint->adapter)
    {
        CACSMExchangeState_t CSMstate = CAGetCSMState(endpoint);
        if (CSMstate != SENT && CSMstate != SENT_RECEIVED)
        {
            CASendCSMMessage(endpoint);
        }
    }
#endif

    // Generate ping message and add to send thread
    // TODO: Implement ping message with custody option
    data = CAGenerateSignalingMessage(endpoint, CA_PING, NULL, 0);
    if (!data)
    {
        OICFree(cur);
        OIC_LOG(ERROR, TAG, "Could not generate signaling message");
        return CA_STATUS_FAILED;
    }
    CAAddDataToSendThread(data);

    // Setup ping info and add to the beginning of the ping list,
    // which ensures that the ping messages are reverse sorted
    // according to the timestamp
    cur->endpoint = *endpoint;
    cur->withCustody = withCustody;
    cur->token = (char *)OICCalloc(data->signalingInfo->info.tokenLength, sizeof(char));
    memcpy(cur->token, data->signalingInfo->info.token, data->signalingInfo->info.tokenLength);
    cur->tokenLength = data->signalingInfo->info.tokenLength;
    cur->timeStamp = OICGetCurrentTime(TIME_IN_MS);
    cur->cbData = *cbData;
    oc_mutex_lock(g_pingInfoListMutex);
    cur->next = g_pingInfoList;
    g_pingInfoList = cur;
    oc_mutex_unlock(g_pingInfoListMutex);

    OIC_LOG(DEBUG, TAG, "CASendPingMessage OUT");
    return CA_STATUS_OK;
}

CAResult_t CASendPongMessage(const CAEndpoint_t *endpoint, bool withCustody,
                             const CAToken_t token, uint8_t tokenLength)
{
    (void) withCustody;
    OIC_LOG(DEBUG, TAG, "CASendPongMessage IN");

    CAData_t *data = NULL;
#ifdef TCP_ADAPTER
    if (CA_ADAPTER_TCP == endpoint->adapter)
    {
        // #1. Try to find session info from tcp_adapter.
        CACSMExchangeState_t CSMstate = CAGetCSMState(endpoint);
        if (CSMstate != SENT && CSMstate != SENT_RECEIVED)
        {
            CASendCSMMessage(endpoint);
        }
    }
#endif

    // Generate pong message and add to send thread
    // TODO: Implement pong message with custody option
    data = CAGenerateSignalingMessageUsingToken(endpoint, CA_PONG, NULL,
                                                0, token, tokenLength);
    if (!data)
    {
        OIC_LOG(ERROR, TAG, "Could not generate signaling message");
        return CA_STATUS_FAILED;
    }
    CAAddDataToSendThread(data);

    OIC_LOG(DEBUG, TAG, "CASendPongMessage OUT");
    return CA_STATUS_OK;
}

void CASetPingTimeout(uint64_t timeout)
{
    OIC_LOG_V(DEBUG, TAG, "CASetPingTimeout called with timeout=%"PRIu64, timeout);
    g_timeout = timeout;
}

void CAProcessPing()
{
    oc_mutex_lock(g_pingInfoListMutex);
    PingInfo **cur = &g_pingInfoList;
    PingInfo *del = NULL;
    uint64_t curTime = OICGetCurrentTime(TIME_IN_MS);

    // Find the first message whose timeout has expired and
    // since the list is reverse sorted, all ping messages
    // after this would have also have expired
    while (*cur && (*cur)->timeStamp + g_timeout > curTime)
    {
        cur = &((*cur)->next);
    }
    del = *cur;
    *cur = NULL;
    while (del)
    {
        // Disconnect the session since timeout has expired
        PingInfo *tmp = del;
        del = del->next;
        CATCPDisconnectSession(&tmp->endpoint);
        CADestroyPingInfo(tmp);
    }
    oc_mutex_unlock(g_pingInfoListMutex);
}

void CAPongReceivedCallback(const CAEndpoint_t *endpoint, const CAToken_t token, uint8_t tokenLength)
{
    OIC_LOG(DEBUG, TAG, "CAPongReceivedCallback IN");

    // Remove corresponding ping message from the ping info list.
    // Both the pong message's endpoint address and the token
    // should match with that of the ping message.
    oc_mutex_lock(g_pingInfoListMutex);
    PingInfo **cur = &g_pingInfoList;
    PingInfo *del = NULL;
    while (*cur)
    {
        if (!strcmp((*cur)->endpoint.addr, endpoint->addr) &&
            !strncmp((*cur)->token, token, tokenLength))
        {
            del = *cur;
            del->cbData.cb(del->cbData.context, del->endpoint, del->withCustody);
            *cur = del->next;
            CADestroyPingInfo(del);
            break;
        }
        cur = &((*cur)->next);
    }
    oc_mutex_unlock(g_pingInfoListMutex);

    OIC_LOG(DEBUG, TAG, "CAPongReceivedCallback OUT");
}

void CATerminatePing()
{
    OIC_LOG(DEBUG, TAG, "CATerminatePing IN");

    // Destroy the ping info list
    oc_mutex_lock(g_pingInfoListMutex);
    PingInfo *cur = g_pingInfoList;
    PingInfo *del = NULL;
    while (cur)
    {
        del = cur;
        cur = cur->next;
        CADestroyPingInfo(del);
    }
    oc_mutex_unlock(g_pingInfoListMutex);

    oc_mutex_free(g_pingInfoListMutex);

    OIC_LOG(DEBUG, TAG, "CATerminatePing OUT");
}

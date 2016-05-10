/* *****************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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

//******************************************************************
//
// Copyright 2016 Intel Corporation All Rights Reserved.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define DONTINCLUDEOCLOGGER

#include "cainterface.h"
#include "caprotocolmessage.h"
#include "oiclogger.h"
#include "config.h" /* for coap protocol */
#include "oic_malloc.h"
#include "oic_string.h"
#include "octypes.h"
#include "ocresource.h"
#include "ocserverrequest.h"
#include "ocpayloadcbor.h"
#include "caadapterinterface.h"
#include "ocmessages.h"
#include "ocadaptation.h"

#include "uqueue.h"
#include "cathreadpool.h" /* for thread pool */
#include "caqueueingthread.h"

#define TAG "CA_MSG_HNDLR"

typedef struct
{
    CAEndpoint_t endpoint;
    int length;
    char buffer[COAP_MAX_PDU_SIZE];
} CAMessage_t;

#define SINGLE_HANDLE
#define MAX_THREAD_POOL_SIZE    20

// thread pool handle
static ca_thread_pool_t g_threadPoolHandle = NULL;

// message handler main thread
static CAQueueingThread_t g_sendThread;
static CAQueueingThread_t g_receiveThread;

static void CASendErrorInfo(CAMessageInfo_t *messageInfo, CAResult_t result);

void CAInitializeAdapters(ca_thread_pool_t handle);

static void CADestroyData(void *data, uint32_t size)
{
    (void)data;
    (void)size;
}

static void CAReceiveThreadProcess(void *threadData)
{
#ifdef SINGLE_HANDLE
    (void)threadData;
#else
    CAMessageInfo_t *data = (CAMessageInfo_t *)threadData;
    CAProcessReceivedData(data);
#endif
}

static void CASendThreadProcess(void *threadData)
{
    CAMessage_t *message = (CAMessage_t *)threadData;

    if (message->endpoint.addr[0])
    {
        (void)SEND_DATA(&(message->endpoint), message->buffer, message->length);
    }
    else
    {
        (void)SEND_MC_DATA(&(message->endpoint), message->buffer, message->length);
    }
}

/*
 * If a second message arrives with the same messageID and the other address
 * family, drop it.  Typically, IPv6 beats IPv4, so the IPv4 message is dropped.
 */
static bool CADropSecondMessage(CAHistory_t *history, const CAMessageInfo_t *messageInfo)
{
    if (messageInfo->devAddr.adapter != OC_ADAPTER_IP)
    {
        return false;
    }
    if (!caglobals.ip.dualstack)
    {
        return false;
    }

    bool ret = false;
    CATransportFlags_t familyFlags = messageInfo->devAddr.flags & CA_IPFAMILY_MASK;

    for (size_t i = 0; i < sizeof (history->items) / sizeof (history->items[0]); i++)
    {
        CAHistoryItem_t *item = &(history->items[i]);
        if (messageInfo->info.messageId == item->messageId)
        {
            if ((familyFlags ^ item->flags) == CA_IPFAMILY_MASK)
            {
                OIC_LOG_V(INFO, TAG, "IPv%c duplicate message ignored",
                                            familyFlags & CA_IPV6 ? '6' : '4');
                ret = true;
                break;
            }
        }
    }

    history->items[history->nextIndex].flags = familyFlags;
    history->items[history->nextIndex].messageId = messageInfo->info.messageId;
    if (++history->nextIndex >= HISTORYSIZE)
    {
        history->nextIndex = 0;
    }

    return ret;
}

// this gets ugly because the OIC protocol is so "efficient",
// meaning that the payloads are not self-identifying.
static CAResult_t CAParsePayload(CAMessageInfo_t *messageInfo)
{
    CAInfo_t *info = &(messageInfo->info);
    if (!messageInfo->encodedSize)
    {
        info->parsedPayload = NULL;
        return CA_STATUS_OK;        // no payload string
    }

    // OIC payloads are not self-identifying (saving a byte each),
    // so we need to figure out the payload type in order to parse them.
    // The following logic figures out the payload type.
    //
    // Much depends on the resource URL. Make sure it's an OIC URL.
    static const char prefix[] = "/oic/";
#define PREFIX_LEN (sizeof (prefix) - 1)
    char *uri = info->resourcePath;
    // The REP payload type is the default.
    OCPayloadType type = PAYLOAD_TYPE_REPRESENTATION;
    if (!strncmp(uri, prefix, PREFIX_LEN))
    {
        // use the remainder of the resource URL to determine payload type
        const char *rest = uri + PREFIX_LEN;
#define SECURE "sec/"
#define SECURE_LEN 4
#define WKU "res"
#define WKU_LEN 3
#define DEV "d"
#define PLAT "p"
#define PRES "ad"

        if (!strncmp(rest, SECURE, SECURE_LEN))
        {
            type = PAYLOAD_TYPE_SECURITY;
        }
        else if (messageInfo->type == OC_RESPONSE_DATA)
        {
            if (!strcmp(rest, PRES))
            {
                type = PAYLOAD_TYPE_PRESENCE;
            }
            else if (strncmp(rest, WKU, WKU_LEN))
            {
                type = PAYLOAD_TYPE_DISCOVERY;
            }
            else if (!strcmp(rest, DEV))
            {
                type = PAYLOAD_TYPE_DEVICE;
            }
            else if (!strcmp(rest, PLAT))
            {
                type = PAYLOAD_TYPE_PLATFORM;
            }
        }
    }

    info->payloadType = type;
    struct OCPayload;   // until header files are cleaned up
    return OCParsePayload((OCPayload **)&(info->parsedPayload), type,
                         messageInfo->encodedPayload, messageInfo->encodedSize);
}

static CAResult_t CAGetInfoFromCompletePDU(CAMessageInfo_t *messageInfo)
{
    coap_pdu_t *pdu = (coap_pdu_t *)(messageInfo->tempPDU);
    messageInfo->tempPDU = NULL;

    CAResult_t result = CAGetMessageInfoFromPDU(pdu, messageInfo);
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, TAG, "CAGetMessageInfoFromPDU failed");
        goto exit;
    }

    int code = messageInfo->info.coapCode;
    messageInfo->type = (code >= OC_GET && code <= OC_DELETE) ? OC_REQUEST_DATA : OC_RESPONSE_DATA;
    CAHistory_t *history = (messageInfo->type == OC_REQUEST_DATA)
                                                ? &caglobals.ca.requestHistory
                                                : &caglobals.ca.responseHistory;
    if (CADropSecondMessage(history, messageInfo))
    {
        result = CA_RECEIVE_DUPLICATE;
        goto exit;
    }

    // Here is where BWT would be implemented.
    // Instead, we will assume the payload is complete.

    result = CAParsePayload(messageInfo);
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, TAG, "CAGetMessageInfoFromPDU failed");
    }

exit:
    coap_delete_pdu(pdu);   // done with the sent pdu at this point
    return result;
}

void CAReceivedPacketCallback(const CASecureEndpoint_t *sep,
                              const void *data, uint32_t dataLen)
{
    OIC_LOG_V(INFO, TAG, "MESSAGE RECEIVED FROM ADAPTER: %d bytes", dataLen);

    coap_pdu_t *pdu = CAParsePDU(data, dataLen);
    if (!pdu)
    {
        OIC_LOG(ERROR, TAG, "Parse PDU failed");
        return;
    }

    OCRequestInfo_t *ri = (OCRequestInfo_t *)OCCalloc(MAT_message);
    if (!ri)
    {
        OIC_LOG(ERROR, TAG, "Can't handle another message");
        coap_delete_pdu(pdu);
        return;
    }
    CAMessageInfo_t *messageInfo = (CAMessageInfo_t *)ri;

    messageInfo->messageLength = dataLen;
    messageInfo->tempPDU = pdu;
    messageInfo->devAddr = *(OCDevAddr *)(&(sep->endpoint));
    messageInfo->info.identity = *(OCRemoteId_t *)(&(sep->identity));

    CAQueueingThreadAddData(&g_receiveThread, ri, sizeof (*ri));
}

void CAHandleRequestResponse()
{
#ifdef SINGLE_HANDLE
    ca_mutex_lock(g_receiveThread.threadMutex);
    u_queue_message_t *item = u_queue_get_element(g_receiveThread.dataQueue);
    ca_mutex_unlock(g_receiveThread.threadMutex);

    if (!item)
    {
        return;     // nothing in queue
    }


    CAMessageInfo_t *messageInfo = (CAMessageInfo_t *)(item->msg);
    if (CA_STATUS_OK == CAGetInfoFromCompletePDU(messageInfo))
    {
        OIC_LOG_V(INFO, TAG, "MESSAGE PASSED TO STACK: %d bytes", messageInfo->messageLength);
        OCHandleIncoming(messageInfo);
    }
#endif /* SINGLE_HANDLE */
}

void CASendPDU(CAMessageInfo_t *messageInfo)
{
    CAMessage_t *message;

    coap_pdu_t *pdu = CAGeneratePDU(messageInfo);
    if (!pdu || pdu->length > sizeof (message->buffer))
    {
        OIC_LOG(ERROR,TAG,"Failed to generate PDU");
        CASendErrorInfo(messageInfo, CA_SEND_FAILED);
        return;
    }

    message = (CAMessage_t *)OICMalloc(sizeof (CAMessage_t));

    message->endpoint = *(CAEndpoint_t *)(&(messageInfo->devAddr));
    message->length = pdu->length;
    memcpy(message->buffer, pdu->hdr, pdu->length);

    CAQueueingThreadAddData(&g_sendThread, message, sizeof (CAMessage_t));
}

static CAResult_t CAInitializeInternal()
{
    CAResult_t res = ca_thread_pool_init(MAX_THREAD_POOL_SIZE, &g_threadPoolHandle);

    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "thread pool initialize error.");
        return res;
    }

    if (CA_STATUS_OK != CAQueueingThreadInitialize(&g_sendThread, g_threadPoolHandle,
                                                   CASendThreadProcess, CADestroyData))
    {
        OIC_LOG(ERROR, TAG, "Failed to Initialize send queue thread");
        return CA_STATUS_FAILED;
    }

    res = CAQueueingThreadStart(&g_sendThread);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "thread start error(send thread).");
        ca_thread_pool_free(g_threadPoolHandle);
        g_threadPoolHandle = NULL;
        return res;
    }

    res = CAQueueingThreadInitialize(&g_receiveThread, g_threadPoolHandle,
                                     CAReceiveThreadProcess, CADestroyData);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "Failed to Initialize receive queue thread");
        return CA_STATUS_FAILED;
    }

#ifndef SINGLE_HANDLE // This will be enabled when RI supports multi threading
    res = CAQueueingThreadStart(&gReceiveThread); // start receive thread

    if (res != CA_STATUS_OK)
    {
        OIC_LOG(ERROR, TAG, "thread start error(receive thread).");
        return res;
    }
#endif /* SINGLE_HANDLE */

    CAInitializeAdapters(g_threadPoolHandle);

    START_ADAPTER

    return CA_STATUS_OK;
}

static OCStackResult CAResultToOCResult(CAResult_t caResult)
{
    switch (caResult)
    {
        case CA_STATUS_OK:
            return OC_STACK_OK;
        case CA_STATUS_INVALID_PARAM:
            return OC_STACK_INVALID_PARAM;
        case CA_ADAPTER_NOT_ENABLED:
            return OC_STACK_ADAPTER_NOT_ENABLED;
        case CA_SERVER_STARTED_ALREADY:
            return OC_STACK_OK;
        case CA_SERVER_NOT_STARTED:
            return OC_STACK_ERROR;
        case CA_DESTINATION_NOT_REACHABLE:
            return OC_STACK_COMM_ERROR;
        case CA_SOCKET_OPERATION_FAILED:
            return OC_STACK_COMM_ERROR;
        case CA_SEND_FAILED:
            return OC_STACK_COMM_ERROR;
        case CA_RECEIVE_FAILED:
            return OC_STACK_COMM_ERROR;
        case CA_MEMORY_ALLOC_FAILED:
            return OC_STACK_NO_MEMORY;
        case CA_REQUEST_TIMEOUT:
            return OC_STACK_TIMEOUT;
        case CA_DESTINATION_DISCONNECTED:
            return OC_STACK_COMM_ERROR;
        case CA_STATUS_FAILED:
            return OC_STACK_ERROR;
        case CA_NOT_SUPPORTED:
            return OC_STACK_NOTIMPL;
        default:
            return OC_STACK_ERROR;
    }
}

OCStackResult CAInitialize()
{
    return CAResultToOCResult(CAInitializeInternal());
}

void CATerminate()
{
    STOP_ADAPTER

    if (NULL != g_sendThread.threadMutex)
    {
        CAQueueingThreadStop(&g_sendThread);
    }

    if (NULL != g_receiveThread.threadMutex)
    {
#ifndef SINGLE_HANDLE
        CAQueueingThreadStop(&gReceiveThread);
#endif /* SINGLE_HANDLE */
    }

    if (NULL != g_threadPoolHandle)
    {
        ca_thread_pool_free(g_threadPoolHandle);
    }

    CAQueueingThreadDestroy(&g_sendThread);
    CAQueueingThreadDestroy(&g_receiveThread);

    TERM_ADAPTER
}

static void CASendErrorInfo(CAMessageInfo_t *messageInfo, CAResult_t result)
{
    (void)result;
    messageInfo->type = OC_ERROR_DATA;

    CAQueueingThreadAddData(&g_receiveThread, messageInfo, sizeof (CAMessageInfo_t));
}

static CAEndpoint_t *CACreateEndpointObject(CATransportFlags_t flags,
                                     CATransportAdapter_t adapter,
                                     const char *address, uint16_t port)
{
    CAEndpoint_t *ep = (CAEndpoint_t *)OICCalloc(1, sizeof(CAEndpoint_t));

    if (address)
    {
        OICStrcpy(ep->addr, sizeof (ep->addr), address);
        ep->addr[MAX_ADDR_STR_SIZE - 1] = '\0';
    }
    ep->flags = flags;
    ep->adapter = adapter;
    ep->port = port;

    return ep;
}

CAResult_t CACreateEndpoint(CATransportFlags_t flags,
                            CATransportAdapter_t adapter,
                            const char *addr,
                            uint16_t port,
                            CAEndpoint_t **object)
{
    CAEndpoint_t *endpoint = CACreateEndpointObject(flags, adapter, addr, port);
    if (!endpoint)
    {
        return CA_STATUS_FAILED;
    }
    *object = endpoint;
    return CA_STATUS_OK;
}

void CAFreeEndpoint(CAEndpoint_t *ep)
{
    OICFree(ep);
}

static void CARegisterCallback(CAConnectivityHandler_t handler, CATransportAdapter_t cType)
{
    (void)cType;
    caglobals.ca.transportAdapters[0] = OICMalloc(sizeof (CAConnectivityHandler_t));
    *((CAConnectivityHandler_t *)caglobals.ca.transportAdapters[0]) = handler;
}

static void CAAdapterReceivedPacketCallback(const CASecureEndpoint_t *sep,
                                     const void *data, uint32_t dataLen)
{
    CAReceivedPacketCallback(sep, data, dataLen);
}

static void CANetworkChangedCallback(const CAEndpoint_t *info, CANetworkStatus_t status)
{
    (void)info;
    (void)status;
}

static void CAAdapterErrorHandleCallback(const CAEndpoint_t *endpoint,
                                         const void *data, uint32_t dataLen,
                                         CAResult_t result)
{
    (void)endpoint;
    (void)data;
    (void)dataLen;
    (void)result;
}

uint32_t GetTicks(uint32_t afterMilliSeconds)
{
    coap_tick_t now;
    coap_ticks(&now);

    // Guard against overflow of uint32_t
    if (afterMilliSeconds <= ((UINT32_MAX - (uint32_t)now) * MILLISECONDS_PER_SECOND) /
                             COAP_TICKS_PER_SECOND)
    {
        return now + (afterMilliSeconds * COAP_TICKS_PER_SECOND)/MILLISECONDS_PER_SECOND;
    }
    else
    {
        return UINT32_MAX;
    }
}

#include "caipadapter.h"

void CAInitializeAdapters(ca_thread_pool_t handle)
{
    CAInitializeIP(CARegisterCallback, CAAdapterReceivedPacketCallback, CANetworkChangedCallback,
                   CAAdapterErrorHandleCallback, handle);
}


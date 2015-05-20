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
#include <stdint.h>

#include "cainterface.h"
#include "camessagehandler.h"
#include "caremotehandler.h"
#include "cainterfacecontroller.h"
#include "caprotocolmessage.h"
#include "caretransmission.h"
#include "caadapterutils.h"
#include "uqueue.h"
#include "logger.h"
#include "config.h" /* for coap protocol */
#include "cathreadpool.h" /* for thread pool */
#include "caqueueingthread.h"
#include "camutex.h"
#include "oic_malloc.h"
#include "canetworkconfigurator.h"

#define TAG PCF("CA")
#define SINGLE_HANDLE

#define MAX_THREAD_POOL_SIZE    20

typedef enum
{
    SEND_TYPE_MULTICAST = 0, SEND_TYPE_UNICAST
} CASendDataType_t;

typedef struct
{
    CASendDataType_t type;
    CARemoteEndpoint_t *remoteEndpoint;
    CARequestInfo_t *requestInfo;
    CAResponseInfo_t *responseInfo;
    CAHeaderOption_t *options;
    uint8_t numOptions;
} CAData_t;

// thread pool handle
static ca_thread_pool_t g_threadPoolHandle = NULL;

// message handler main thread
static CAQueueingThread_t g_sendThread;
static CAQueueingThread_t g_receiveThread;

static CARetransmission_t g_retransmissionContext;

// handler field
static CARequestCallback g_requestHandler = NULL;
static CAResponseCallback g_responseHandler = NULL;

static bool CAIsSelectedNetworkAvailable()
{
    u_arraylist_t *list = CAGetSelectedNetworkList();
    if (!list || list->length == 0)
    {
        OIC_LOG(ERROR, TAG, "No selected network");
        return false;
    }

    return true;
}

static void CATimeoutCallback(const CARemoteEndpoint_t *endpoint, const void *pdu, uint32_t size)
{
    OIC_LOG(DEBUG, TAG, "IN");
    VERIFY_NON_NULL_VOID(endpoint, TAG, "endpoint");
    VERIFY_NON_NULL_VOID(pdu, TAG, "pdu");

    CARemoteEndpoint_t* ep = CACloneRemoteEndpoint(endpoint);
    if (NULL == ep)
    {
        OIC_LOG(ERROR, TAG, "clone failed");
        return;
    }

    CAResponseInfo_t* resInfo = (CAResponseInfo_t*) OICCalloc(1, sizeof(CAResponseInfo_t));

    if (NULL == resInfo)
    {
        OIC_LOG(ERROR, TAG, "calloc failed");
        CADestroyRemoteEndpointInternal(ep);
        return;
    }

    resInfo->result = CA_RETRANSMIT_TIMEOUT;
    resInfo->info.type = CAGetMessageTypeFromPduBinaryData(pdu, size);
    resInfo->info.messageId = CAGetMessageIdFromPduBinaryData(pdu, size);
    CAResult_t res = CAGetTokenFromPDU((const coap_hdr_t *) pdu, &(resInfo->info));
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "fail to get Token from retransmission list");
        OICFree(resInfo->info.token);
        OICFree(resInfo);
        CADestroyRemoteEndpointInternal(ep);
        return;
    }

    CAData_t *cadata = (CAData_t *) OICCalloc(1, sizeof(CAData_t));
    if (NULL == cadata)
    {
        OIC_LOG(ERROR, TAG, "memory allocation failed !");
        CADestroyRemoteEndpointInternal(ep);
        OICFree(resInfo);
        return;
    }

    cadata->type = SEND_TYPE_UNICAST;
    cadata->remoteEndpoint = ep;
    cadata->requestInfo = NULL;
    cadata->responseInfo = resInfo;

    CAQueueingThreadAddData(&g_receiveThread, cadata, sizeof(CAData_t));
    OIC_LOG(DEBUG, TAG, "OUT");
}

static void CADataDestroyer(void *data, uint32_t size)
{
    OIC_LOG(DEBUG, TAG, "IN");
    CAData_t *cadata = (CAData_t *) data;

    if (NULL == cadata)
    {
        OIC_LOG(ERROR, TAG, "cadata is NULL");
        return;
    }

    if (NULL != cadata->remoteEndpoint)
    {
        CADestroyRemoteEndpointInternal((CARemoteEndpoint_t *) cadata->remoteEndpoint);
    }

    if (NULL != cadata->requestInfo)
    {
        CADestroyRequestInfoInternal((CARequestInfo_t *) cadata->requestInfo);
    }

    if (NULL != cadata->responseInfo)
    {
        CADestroyResponseInfoInternal((CAResponseInfo_t *) cadata->responseInfo);
    }

    OICFree(cadata->options);
    OICFree(cadata);
    OIC_LOG(DEBUG, TAG, "OUT");
}

static void CAReceiveThreadProcess(void *threadData)
{
    OIC_LOG(DEBUG, TAG, "IN");
    // Currently not supported
    // This will be enabled when RI supports multi threading
#ifndef SINGLE_HANDLE
    CAData_t *data = (CAData_t *) threadData;

    if (NULL == data)
    {
        OIC_LOG(ERROR, TAG, "thread data error!!");
        return;
    }

    // parse the data and call the callbacks.
    // #1 parse the data
    // #2 get endpoint
    CARemoteEndpoint_t *rep = (CARemoteEndpoint_t *)(data->remoteEndpoint);

    if (NULL == rep)
    {
        OIC_LOG(ERROR, TAG, "remoteEndpoint error!!");
        return;
    }

    if (NULL != data->requestInfo)
    {
        if (g_requestHandler)
        {
            g_requestHandler(rep, data->requestInfo);
        }
    }

    if (NULL != data->responseInfo)
    {
        if (g_responseHandler)
        {
            g_responseHandler(rep, data->responseInfo);
        }
    }
#endif
    OIC_LOG(DEBUG, TAG, "OUT");
}

static void CASendThreadProcess(void *threadData)
{
    OIC_LOG(DEBUG, TAG, "IN");
    CAData_t *data = (CAData_t *) threadData;

    VERIFY_NON_NULL_VOID(data, TAG, "data");
    VERIFY_NON_NULL_VOID(data->remoteEndpoint, TAG, "remoteEndpoint");

    CAResult_t res = CA_STATUS_FAILED;

    CASendDataType_t type = data->type;

    if (SEND_TYPE_UNICAST == type)
    {
        coap_pdu_t *pdu = NULL;

        if (NULL != data->requestInfo)
        {
            OIC_LOG(DEBUG, TAG, "requestInfo is available..");

            pdu = (coap_pdu_t *) CAGeneratePDU(data->remoteEndpoint->resourceUri,
                                               data->requestInfo->method,
                                               data->requestInfo->info);
        }
        else if (NULL != data->responseInfo)
        {
            OIC_LOG(DEBUG, TAG, "responseInfo is available..");

            pdu = (coap_pdu_t *) CAGeneratePDU(data->remoteEndpoint->resourceUri,
                                               data->responseInfo->result,
                                               data->responseInfo->info);
        }
        else
        {
            OIC_LOG(DEBUG, TAG, "request info, response info is empty");
        }

        // interface controller function call.
        if (NULL != pdu)
        {
            CALogPDUInfo(pdu);

            res = CASendUnicastData(data->remoteEndpoint, pdu->hdr, pdu->length);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG_V(ERROR, TAG, "send failed:%d", res);
                coap_delete_pdu(pdu);
                return;
            }
            // for retransmission
            res = CARetransmissionSentData(&g_retransmissionContext, data->remoteEndpoint, pdu->hdr,
                                           pdu->length);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG_V(INFO, TAG, "retransmission will be not working: %d", res);
                coap_delete_pdu(pdu);
                return;
            }

            coap_delete_pdu(pdu);
        }
    }
    else if (SEND_TYPE_MULTICAST == type)
    {
        OIC_LOG(DEBUG, TAG, "both requestInfo & responseInfo is not available");

        CAInfo_t info = data->requestInfo->info;

        info.options = data->options;
        info.numOptions = data->numOptions;

        coap_pdu_t *pdu = (coap_pdu_t *) CAGeneratePDU(data->remoteEndpoint->resourceUri, CA_GET,
                                                       info);

        if (NULL != pdu)
        {
            CALogPDUInfo(pdu);

            res = CASendMulticastData(pdu->hdr, pdu->length);
            if(CA_STATUS_OK != res)
            {
                OIC_LOG_V(ERROR, TAG, "send failed:%d", res);
                coap_delete_pdu(pdu);
                return;
            }

            coap_delete_pdu(pdu);
        }
    }

    OIC_LOG(DEBUG, TAG, "OUT");
}

static void CAReceivedPacketCallback(CARemoteEndpoint_t *endpoint, void *data, uint32_t dataLen)
{
    OIC_LOG(DEBUG, TAG, "IN");
    VERIFY_NON_NULL_VOID(endpoint, TAG, "endpoint");
    VERIFY_NON_NULL_VOID(data, TAG, "data");

    uint32_t code = CA_NOT_FOUND;
    coap_pdu_t *pdu = (coap_pdu_t *) CAParsePDU((const char *) data, dataLen, &code);
    OICFree(data);

    if (NULL == pdu)
    {
        OIC_LOG(ERROR, TAG, "Parse PDU failed");
        CAAdapterFreeRemoteEndpoint(endpoint);
        return;
    }

    char uri[CA_MAX_URI_LENGTH] = { 0, };
    uint32_t bufLen = sizeof(uri);

    if (CA_GET == code || CA_POST == code || CA_PUT == code || CA_DELETE == code)
    {
        CARequestInfo_t *ReqInfo = (CARequestInfo_t *) OICCalloc(1, sizeof(CARequestInfo_t));
        if (NULL == ReqInfo)
        {
            OIC_LOG(ERROR, TAG, "CAReceivedPacketCallback, Memory allocation failed!");
            coap_delete_pdu(pdu);
            CAAdapterFreeRemoteEndpoint(endpoint);
            return;
        }

        CAResult_t res = CAGetRequestInfoFromPDU(pdu, ReqInfo, uri, bufLen);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG_V(ERROR, TAG, "CAGetRequestInfoFromPDU failed : %d", res);
            OICFree(ReqInfo);
            coap_delete_pdu(pdu);
            CAAdapterFreeRemoteEndpoint(endpoint);
            return;
        }

        if (NULL != ReqInfo->info.options)
        {
            uint32_t i;
            for (i = 0; i < ReqInfo->info.numOptions; i++)
            {
                OIC_LOG_V(DEBUG, TAG, "Request- optionID: %d", ReqInfo->info.options[i].optionID);

                OIC_LOG_V(DEBUG, TAG, "Request- list: %s", ReqInfo->info.options[i].optionData);
            }
        }

        if (NULL != ReqInfo->info.payload)
        {
            OIC_LOG_V(DEBUG, TAG, "Request- payload: %s", ReqInfo->info.payload);
        }
        OIC_LOG_V(DEBUG, TAG, "Request- code: %d", ReqInfo->method);
        if (NULL != ReqInfo->info.token)
        {
            OIC_LOG(DEBUG, TAG, "Request- token:");
            OIC_LOG_BUFFER(DEBUG, TAG, (const uint8_t *) ReqInfo->info.token,
                           ReqInfo->info.tokenLength);
        }

        OIC_LOG_V(DEBUG, TAG, "Request- code: %d", ReqInfo->method);
        OIC_LOG(DEBUG, TAG, "Request- token");
        OIC_LOG_BUFFER(DEBUG, TAG, (const uint8_t *) ReqInfo->info.token, CA_MAX_TOKEN_LEN);
        OIC_LOG_V(DEBUG, TAG, "Request- msgID : %d", ReqInfo->info.messageId);
        if (NULL != endpoint)
        {
            endpoint->resourceUri = (char *) OICMalloc(bufLen + 1);
            if (NULL == endpoint->resourceUri)
            {
                OIC_LOG(ERROR, TAG, "CAReceivedPacketCallback, Memory allocation failed!");
                OICFree(ReqInfo);
                coap_delete_pdu(pdu);
                CAAdapterFreeRemoteEndpoint(endpoint);
                return;
            }
            memcpy(endpoint->resourceUri, uri, bufLen);
            endpoint->resourceUri[bufLen] = '\0';
            OIC_LOG_V(DEBUG, TAG, "URI : %s", endpoint->resourceUri);
        }
        // store the data at queue.
        CAData_t *cadata = NULL;
        cadata = (CAData_t *) OICCalloc(1, sizeof(CAData_t));
        if (NULL == cadata)
        {
            OIC_LOG(ERROR, TAG, "CAReceivedPacketCallback, Memory allocation failed !");
            OICFree(ReqInfo);
            coap_delete_pdu(pdu);
            CAAdapterFreeRemoteEndpoint(endpoint);
            return;
        }

        cadata->type = SEND_TYPE_UNICAST;
        cadata->remoteEndpoint = endpoint;
        cadata->requestInfo = ReqInfo;
        cadata->responseInfo = NULL;
        CAQueueingThreadAddData(&g_receiveThread, cadata, sizeof(CAData_t));
    }
    else
    {
        CAResponseInfo_t *ResInfo = (CAResponseInfo_t *) OICCalloc(1, sizeof(CAResponseInfo_t));
        if (NULL == ResInfo)
        {
            OIC_LOG(ERROR, TAG, "CAReceivedPacketCallback, Memory allocation failed!");
            coap_delete_pdu(pdu);
            CAAdapterFreeRemoteEndpoint(endpoint);
            return;
        }

        CAResult_t res = CAGetResponseInfoFromPDU(pdu, ResInfo, uri, bufLen);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG_V(ERROR, TAG, "CAGetResponseInfoFromPDU failed : %d", res);
            OICFree(ResInfo);
            coap_delete_pdu(pdu);
            CAAdapterFreeRemoteEndpoint(endpoint);
            return;
        }

        if (NULL != ResInfo->info.options)
        {
            uint32_t i;
            for (i = 0; i < ResInfo->info.numOptions; i++)
            {
                OIC_LOG_V(DEBUG, TAG, "Response- optionID: %d", ResInfo->info.options[i].optionID);

                OIC_LOG_V(DEBUG, TAG, "Response- list: %s", ResInfo->info.options[i].optionData);
            }
        }

        if (NULL != ResInfo->info.payload)
        {
            OIC_LOG_V(DEBUG, TAG, "Response- payload: %s", ResInfo->info.payload);
        }
        OIC_LOG_V(DEBUG, TAG, "Response- code: %d", ResInfo->result);
        OIC_LOG_V(DEBUG, TAG, "Response- token : %s", ResInfo->info.token);
        OIC_LOG_V(DEBUG, TAG, "Response- msgID: %d", ResInfo->info.messageId);

        if (NULL != endpoint)
        {
            endpoint->resourceUri = (char *) OICMalloc(bufLen + 1);
            if (NULL == endpoint->resourceUri)
            {
                OIC_LOG(ERROR, TAG, "CAReceivedPacketCallback, Memory allocation failed !");
                OICFree(ResInfo);
                coap_delete_pdu(pdu);
                CAAdapterFreeRemoteEndpoint(endpoint);
                return;
            }
            memcpy(endpoint->resourceUri, uri, bufLen);
            endpoint->resourceUri[bufLen] = '\0';
            OIC_LOG_V(DEBUG, TAG, "URI : %s", endpoint->resourceUri);
        }

        // store the data at queue.
        CAData_t *cadata = (CAData_t *) OICCalloc(1, sizeof(CAData_t));
        if (NULL == cadata)
        {
            OIC_LOG(ERROR, TAG, "CAReceivedPacketCallback, Memory allocation failed !");
            OICFree(ResInfo);
            coap_delete_pdu(pdu);
            CAAdapterFreeRemoteEndpoint(endpoint);
            return;
        }

        cadata->type = SEND_TYPE_UNICAST;
        cadata->remoteEndpoint = endpoint;
        cadata->requestInfo = NULL;

        // for retransmission
        void *retransmissionPdu = NULL;
        CARetransmissionReceivedData(&g_retransmissionContext, endpoint, pdu->hdr, pdu->length,
                                     &retransmissionPdu);

        // get token from saved data in retransmission list
        if (retransmissionPdu && CA_EMPTY == code)
        {
            CAResult_t res = CAGetTokenFromPDU((const coap_hdr_t *)retransmissionPdu,
                                               &(ResInfo->info));
            if (CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "fail to get Token from retransmission list");
                OICFree(ResInfo->info.token);
            }
        }
        OICFree(retransmissionPdu);
        cadata->responseInfo = ResInfo;

        CAQueueingThreadAddData(&g_receiveThread, cadata, sizeof(CAData_t));
    }

    if (pdu)
    {
        coap_delete_pdu(pdu);
    }
    OIC_LOG(DEBUG, TAG, "OUT");
}

static void CANetworkChangedCallback(CALocalConnectivity_t *info, CANetworkStatus_t status)
{
    OIC_LOG(DEBUG, TAG, "IN");

    OIC_LOG(DEBUG, TAG, "OUT");
}

void CAHandleRequestResponseCallbacks()
{
    OIC_LOG(DEBUG, TAG, "CAHandleRequestResponseCallbacks IN");

#ifdef SINGLE_HANDLE
    // parse the data and call the callbacks.
    // #1 parse the data
    // #2 get endpoint

    ca_mutex_lock(g_receiveThread.threadMutex);

    u_queue_message_t *item = u_queue_get_element(g_receiveThread.dataQueue);

    ca_mutex_unlock(g_receiveThread.threadMutex);

    if (NULL == item)
    {
        return;
    }

    // get values
    void *msg = item->msg;

    if (NULL == msg)
    {
        return;
    }

    // get endpoint
    CAData_t *td = (CAData_t *) msg;
    CARemoteEndpoint_t *rep = td->remoteEndpoint;

    if (NULL == rep)
    {
        return;
    }

    if (NULL != td->requestInfo)
    {
        if (g_requestHandler)
        {
            OIC_LOG_V(DEBUG, TAG, "callback will be sent : %d", td->requestInfo->info.numOptions);
            g_requestHandler(rep, td->requestInfo);
        }
    }

    if (NULL != td->responseInfo)
    {
        if (g_responseHandler)
        {
            g_responseHandler(rep, td->responseInfo);
        }

    }
    CADataDestroyer(msg, sizeof(CAData_t));

#endif
    OIC_LOG(DEBUG, TAG, "CAHandleRequestResponseCallbacks OUT");
}

CAResult_t CADetachRequestMessage(const CARemoteEndpoint_t *object, const CARequestInfo_t *request)
{
    OIC_LOG(DEBUG, TAG, "IN");

    VERIFY_NON_NULL(object, TAG, "object");
    VERIFY_NON_NULL(request, TAG, "request");

    if (false == CAIsSelectedNetworkAvailable())
    {
        return CA_STATUS_FAILED;
    }

    CARemoteEndpoint_t *remoteEndpoint = NULL;
    CARequestInfo_t *requestInfo = NULL;
    CAData_t *data = (CAData_t *) OICCalloc(1, sizeof(CAData_t));
    CA_MEMORY_ALLOC_CHECK(data);

    // clone remote endpoint
    remoteEndpoint = CACloneRemoteEndpoint(object);
    CA_MEMORY_ALLOC_CHECK(remoteEndpoint);

    // clone request info
    requestInfo = CACloneRequestInfo(request);
    CA_MEMORY_ALLOC_CHECK(requestInfo);

    // save data
    data->type = SEND_TYPE_UNICAST;
    data->remoteEndpoint = remoteEndpoint;
    data->requestInfo = requestInfo;
    data->responseInfo = NULL;

    // add thread
    CAQueueingThreadAddData(&g_sendThread, data, sizeof(CAData_t));
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;

// memory error label.
memory_error_exit:
    CADestroyRemoteEndpointInternal(remoteEndpoint);
    CADestroyRequestInfoInternal(requestInfo);

    OICFree(data);
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_MEMORY_ALLOC_FAILED;
}

CAResult_t CADetachRequestToAllMessage(const CAGroupEndpoint_t *object,
                                       const CARequestInfo_t *request)
{
    OIC_LOG(DEBUG, TAG, "IN");

    if (NULL == object || NULL == request || NULL == object->resourceUri)
    {
        return CA_STATUS_INVALID_PARAM;
    }

    if ((request->method < CA_GET) || (request->method > CA_DELETE))
    {
        OIC_LOG(ERROR, TAG, "Invalid method type!");

        return CA_STATUS_INVALID_PARAM;
    }

    if (false == CAIsSelectedNetworkAvailable())
    {
        return CA_STATUS_FAILED;
    }

    CARemoteEndpoint_t *remoteEndpoint = NULL;
    CARequestInfo_t *requestInfo = NULL;

    // allocate & initialize
    CAData_t *data = (CAData_t *) OICCalloc(1, sizeof(CAData_t));
    CA_MEMORY_ALLOC_CHECK(data);

    CAAddress_t addr = {};
    remoteEndpoint = CACreateRemoteEndpointInternal(object->resourceUri, addr,
                                                    object->transportType);

    // clone request info
    requestInfo = CACloneRequestInfo(request);
    CA_MEMORY_ALLOC_CHECK(requestInfo);

    // save data
    data->type = SEND_TYPE_MULTICAST;
    data->remoteEndpoint = remoteEndpoint;
    data->requestInfo = requestInfo;
    data->responseInfo = NULL;

    // add thread
    CAQueueingThreadAddData(&g_sendThread, data, sizeof(CAData_t));

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;

// memory error label.
memory_error_exit:

    CADestroyRequestInfoInternal(requestInfo);
    CADestroyRemoteEndpointInternal(remoteEndpoint);
    OICFree(data);
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_MEMORY_ALLOC_FAILED;
}

CAResult_t CADetachResponseMessage(const CARemoteEndpoint_t *object,
                                   const CAResponseInfo_t *response)
{
    OIC_LOG(DEBUG, TAG, "IN");
    VERIFY_NON_NULL(object, TAG, "object");
    VERIFY_NON_NULL(response, TAG, "response");

    if (false == CAIsSelectedNetworkAvailable())
    {
        return CA_STATUS_FAILED;
    }

    CARemoteEndpoint_t *remoteEndpoint = NULL;
    CAResponseInfo_t *responseInfo = NULL;

    // allocate & initialize
    CAData_t *data = (CAData_t *) OICCalloc(1, sizeof(CAData_t));
    CA_MEMORY_ALLOC_CHECK(data);

    // clone remote endpoint
    remoteEndpoint = CACloneRemoteEndpoint(object);
    CA_MEMORY_ALLOC_CHECK(remoteEndpoint);

    // clone response info
    responseInfo = CACloneResponseInfo(response);
    CA_MEMORY_ALLOC_CHECK(responseInfo);

    // save data
    data->type = SEND_TYPE_UNICAST;
    data->remoteEndpoint = remoteEndpoint;
    data->requestInfo = NULL;
    data->responseInfo = responseInfo;

    // add thread
    CAQueueingThreadAddData(&g_sendThread, data, sizeof(CAData_t));

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;

// memory error label.
memory_error_exit:
    CADestroyRemoteEndpointInternal(remoteEndpoint);
    CADestroyResponseInfoInternal(responseInfo);
    OICFree(data);
    OIC_LOG(DEBUG, TAG, "OUT");

    return CA_MEMORY_ALLOC_FAILED;
}

CAResult_t CADetachMessageResourceUri(const CAURI_t resourceUri, const CAToken_t token,
                                      uint8_t tokenLength, const CAHeaderOption_t *options,
                                      uint8_t numOptions)
{
    OIC_LOG(DEBUG, TAG, "IN");
    VERIFY_NON_NULL(resourceUri, TAG, "resourceUri is NULL");
    VERIFY_NON_NULL(token, TAG, "Token is NULL");

    if (false == CAIsSelectedNetworkAvailable())
    {
        return CA_STATUS_FAILED;
    }

    CARemoteEndpoint_t *remoteEndpoint = NULL;
    CARequestInfo_t *reqInfo = NULL;
    char *tempToken = NULL;

    // allocate & initialize
    CAData_t *data = (CAData_t *) OICCalloc(1, sizeof(CAData_t));
    CA_MEMORY_ALLOC_CHECK(data);

    CAAddress_t addr = {};
    remoteEndpoint = CACreateRemoteEndpointInternal(resourceUri, addr,
                                                    CA_IPV4 | CA_EDR | CA_LE);

    // create request info
    reqInfo = (CARequestInfo_t *) OICCalloc(1, sizeof(CARequestInfo_t));
    CA_MEMORY_ALLOC_CHECK(reqInfo);

    if (tokenLength)
    {
        // copy token value
        tempToken = (char *) OICMalloc(tokenLength);
        CA_MEMORY_ALLOC_CHECK(tempToken);
        memcpy(tempToken, token, tokenLength);
    }

    // save request info data
    reqInfo->method = CA_GET;
    reqInfo->info.type = CA_MSG_NONCONFIRM;

    reqInfo->info.token = tempToken;
    reqInfo->info.tokenLength = tokenLength;

    // save data
    data->type = SEND_TYPE_MULTICAST;
    data->remoteEndpoint = remoteEndpoint;
    data->requestInfo = reqInfo;

    data->responseInfo = NULL;
    data->options = NULL;
    data->numOptions = 0;
    if (NULL != options && 0 < numOptions)
    {
        // copy data
        CAHeaderOption_t *headerOption = (CAHeaderOption_t *) OICMalloc(sizeof(CAHeaderOption_t)
                                                                        * numOptions);
        CA_MEMORY_ALLOC_CHECK(headerOption);

        memcpy(headerOption, options, sizeof(CAHeaderOption_t) * numOptions);

        data->options = headerOption;
        data->numOptions = numOptions;
    }

    // add thread
    CAQueueingThreadAddData(&g_sendThread, data, sizeof(CAData_t));

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;

// memory error label.
memory_error_exit:

    CADestroyRemoteEndpointInternal(remoteEndpoint);

    OICFree(tempToken);
    OICFree(reqInfo);
    OICFree(data);
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_MEMORY_ALLOC_FAILED;
}

void CASetRequestResponseCallbacks(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
{
    OIC_LOG(DEBUG, TAG, "IN");
    g_requestHandler = ReqHandler;
    g_responseHandler = RespHandler;
    OIC_LOG(DEBUG, TAG, "OUT");
}

CAResult_t CAInitializeMessageHandler()
{
    OIC_LOG(DEBUG, TAG, "IN");
    CASetPacketReceivedCallback(CAReceivedPacketCallback);

    CASetNetworkChangeCallback(CANetworkChangedCallback);

    // create thread pool
    CAResult_t res = ca_thread_pool_init(MAX_THREAD_POOL_SIZE, &g_threadPoolHandle);

    if (res != CA_STATUS_OK)
    {
        OIC_LOG(ERROR, TAG, "thread pool initialize error.");
        return res;
    }

    // send thread initialize
    if (CA_STATUS_OK != CAQueueingThreadInitialize(&g_sendThread, g_threadPoolHandle,
                                                   CASendThreadProcess, CADataDestroyer))
    {
        OIC_LOG(ERROR, TAG, "Failed to Initialize send queue thread");
        return CA_STATUS_FAILED;
    }

    // start send thread
    res = CAQueueingThreadStart(&g_sendThread);

    if (res != CA_STATUS_OK)
    {
        OIC_LOG(ERROR, TAG, "thread start error(send thread).");
        ca_thread_pool_free(g_threadPoolHandle);
        g_threadPoolHandle = NULL;
        return res;
    }

    // receive thread initialize
    if (CA_STATUS_OK != CAQueueingThreadInitialize(&g_receiveThread, g_threadPoolHandle,
                                                   CAReceiveThreadProcess, CADataDestroyer))
    {
        OIC_LOG(ERROR, TAG, "Failed to Initialize receive queue thread");
        return CA_STATUS_FAILED;
    }

#ifndef SINGLE_HANDLE // This will be enabled when RI supports multi threading
    // start receive thread
    res = CAQueueingThreadStart(&gReceiveThread);

    if (res != CA_STATUS_OK)
    {
        OIC_LOG(ERROR, TAG, "thread start error(receive thread).");
        return res;
    }
#endif

    // retransmission initialize
    CARetransmissionInitialize(&g_retransmissionContext, g_threadPoolHandle, CASendUnicastData,
                               CATimeoutCallback, NULL);

    // start retransmission
    res = CARetransmissionStart(&g_retransmissionContext);

    if (res != CA_STATUS_OK)
    {
        OIC_LOG(ERROR, TAG, "thread start error(retransmission thread).");
        return res;
    }

    // initialize interface adapters by controller
    CAInitializeAdapters(g_threadPoolHandle);
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

void CATerminateMessageHandler()
{
    OIC_LOG(DEBUG, TAG, "IN");
    CATransportType_t connType;
    u_arraylist_t *list = CAGetSelectedNetworkList();
    uint32_t length = u_arraylist_length(list);

    uint32_t i = 0;
    for (i = 0; i < length; i++)
    {
        void* ptrType = u_arraylist_get(list, i);

        if (NULL == ptrType)
        {
            continue;
        }

        connType = *(CATransportType_t *) ptrType;
        CAStopAdapter(connType);
    }

    // stop retransmission
    if (NULL != g_retransmissionContext.threadMutex)
    {
        CARetransmissionStop(&g_retransmissionContext);
    }

    // stop thread
    // delete thread data
    if (NULL != g_sendThread.threadMutex)
    {
        CAQueueingThreadStop(&g_sendThread);
    }

    // stop thread
    // delete thread data
    if (NULL != g_receiveThread.threadMutex)
    {
#ifndef SINGLE_HANDLE // This will be enabled when RI supports multi threading
        CAQueueingThreadStop(&gReceiveThread);
#endif
    }

    // destroy thread pool
    if (NULL != g_threadPoolHandle)
    {
        ca_thread_pool_free(g_threadPoolHandle);
        g_threadPoolHandle = NULL;
    }

    CARetransmissionDestroy(&g_retransmissionContext);
    CAQueueingThreadDestroy(&g_sendThread);
    CAQueueingThreadDestroy(&g_receiveThread);

    // terminate interface adapters by controller
    CATerminateAdapters();

    OIC_LOG(DEBUG, TAG, "OUT");
}

void CALogPDUInfo(coap_pdu_t *pdu)
{
    VERIFY_NON_NULL_VOID(pdu, TAG, "pdu");

    OIC_LOG_V(DEBUG, TAG, "PDU Maker - payload : %s", pdu->data);

    OIC_LOG_V(DEBUG, TAG, "PDU Maker - type : %d", pdu->hdr->type);

    OIC_LOG_V(DEBUG, TAG, "PDU Maker - code : %d", pdu->hdr->code);

    OIC_LOG_V(DEBUG, TAG, "PDU Maker - id : %d", ntohs(pdu->hdr->id));

    OIC_LOG(DEBUG, TAG, "PDU Maker - token :");

    OIC_LOG_BUFFER(DEBUG, TAG, pdu->hdr->token, pdu->hdr->token_length);
}

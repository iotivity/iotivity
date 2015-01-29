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
#include "uqueue.h"
#include "logger.h"
#include "config.h" /* for coap protocol */
#include "coap.h"
#include "uthreadpool.h" /* for thread pool */
#include "caqueueingthread.h"
#include "caretransmission.h"
#include "umutex.h"
#include "oic_malloc.h"

#define TAG PCF("CA")
#define SINGLE_HANDLE

#define MEMORY_ALLOC_CHECK(arg) { if (arg == NULL) {OIC_LOG_V(DEBUG, TAG, "memory error"); \
goto memory_error_exit;} }

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
static u_thread_pool_t gThreadPoolHandle = NULL;

// message handler main thread
static CAQueueingThread_t gSendThread;
static CAQueueingThread_t gReceiveThread;

static CARetransmission_t gRetransmissionContext;

// handler field
static CARequestCallback gRequestHandler = NULL;
static CAResponseCallback gResponseHandler = NULL;

static void CATimeoutCallback(const CARemoteEndpoint_t *endpoint, void *pdu, uint32_t size)
{
    CARemoteEndpoint_t* ep = CACloneRemoteEndpoint(endpoint);
    if (ep == NULL)
    {
        OIC_LOG(DEBUG, TAG, "memory allocation failed !");
        return;
    }

    CAResponseInfo_t* resInfo = (CAResponseInfo_t*) OICMalloc(sizeof(CAResponseInfo_t));

    if (resInfo == NULL)
    {
        OIC_LOG(DEBUG, TAG, "memory allocation failed !");
        CADestroyRemoteEndpointInternal(ep);
        return;
    }
    memset(resInfo, 0, sizeof(CAResponseInfo_t));

    CAMessageType_t type = CAGetMessageTypeFromPduBinaryData(pdu, size);
    uint16_t messageId = CAGetMessageIdFromPduBinaryData(pdu, size);

    resInfo->result = CA_RETRANSMIT_TIMEOUT;
    resInfo->info.type = type;
    resInfo->info.messageId = messageId;

    CAData_t *cadata = (CAData_t *) OICMalloc(sizeof(CAData_t));
    if (cadata == NULL)
    {
        OIC_LOG(DEBUG, TAG, "memory allocation failed !");
        CADestroyRemoteEndpointInternal(ep);
        OICFree(resInfo);
        return;
    }
    memset(cadata, 0, sizeof(CAData_t));

    cadata->type = SEND_TYPE_UNICAST;
    cadata->remoteEndpoint = ep;
    cadata->requestInfo = NULL;
    cadata->responseInfo = resInfo;

    CAQueueingThreadAddData(&gReceiveThread, cadata, sizeof(CAData_t));
}

static void CADataDestroyer(void *data, uint32_t size)
{
    OIC_LOG(DEBUG, TAG, "CADataDestroyer is ran!");
    CAData_t *cadata = (CAData_t *) data;

    if (cadata == NULL)
    {
        return;
    }

    if (cadata->remoteEndpoint != NULL)
    {
        CADestroyRemoteEndpointInternal((CARemoteEndpoint_t *)cadata->remoteEndpoint);
    }

    if (cadata->requestInfo != NULL)
    {
        CADestroyRequestInfoInternal((CARequestInfo_t *)cadata->requestInfo);
    }

    if (cadata->responseInfo != NULL)
    {
        CADestroyResponseInfoInternal((CAResponseInfo_t *)cadata->responseInfo);
    }

    if (cadata->options != NULL)
    {
        OICFree(cadata->options);
    }

    OICFree(cadata);
}

static void CAReceiveThreadProcess(void *threadData)
{
    // Currently not supported
	// This will be enabled when RI supports multi threading
#ifndef SINGLE_HANDLE
    CAData_t *data = (CAData_t *) threadData;

    if (data == NULL)
    {
        OIC_LOG(DEBUG, TAG, "thread data error!!");
        return;
    }

    // parse the data and call the callbacks.
    // #1 parse the data
    // #2 get endpoint
    CARemoteEndpoint_t *rep = (CARemoteEndpoint_t *)(data->remoteEndpoint);

    if (rep == NULL)
        return;

    if (data->requestInfo != NULL)
    {
        if (gRequestHandler)
        {
            gRequestHandler(rep, data->requestInfo);
        }
    }

    if (data->responseInfo != NULL)
    {
        if (gResponseHandler)
        {
            gResponseHandler(rep, data->responseInfo);
        }
    }
#endif
}

static void CASendThreadProcess(void *threadData)
{
    CAData_t *data = (CAData_t *) threadData;

    if (data == NULL)
    {
        OIC_LOG(DEBUG, TAG, "thread data error!!");
        return;
    }

    if (NULL == data->remoteEndpoint)
    {
        OIC_LOG(DEBUG, TAG, "remoteEndpoint is null");
        return;
    }

    CAResult_t res = CA_STATUS_FAILED;

    CASendDataType_t type = data->type;

    if (type == SEND_TYPE_UNICAST)
    {
        coap_pdu_t *pdu = NULL;

        if (data->requestInfo != NULL)
        {
            OIC_LOG_V(DEBUG, TAG, "requestInfo is available..");

            pdu = (coap_pdu_t *) CAGeneratePdu(data->remoteEndpoint->resourceUri,
                                               data->requestInfo->method,
                                               data->requestInfo->info);
        }
        else if (data->responseInfo != NULL)
        {
            OIC_LOG_V(DEBUG, TAG, "responseInfo is available..");

            pdu = (coap_pdu_t *) CAGeneratePdu(data->remoteEndpoint->resourceUri,
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
            OIC_LOG_V(DEBUG, TAG, "PDU Maker - payload : %s", pdu->data);

            OIC_LOG_V(DEBUG, TAG, "PDU Maker - type : %d", pdu->hdr->type);

            OIC_LOG_V(DEBUG, TAG, "PDU Maker - code : %d", pdu->hdr->code);

            OIC_LOG_V(DEBUG, TAG, "PDU Maker - id : %d", ntohs(pdu->hdr->id));

            OIC_LOG_V(DEBUG, TAG, "PDU Maker - token : %s", pdu->hdr->token);

            OIC_LOG_V(DEBUG, TAG, "PDU Maker - buffer data : %s", pdu->hdr);

            res = CASendUnicastData(data->remoteEndpoint, pdu->hdr, pdu->length);

            // for retransmission
            CARetransmissionSentData(&gRetransmissionContext, data->remoteEndpoint, pdu->hdr,
                                     pdu->length);

            coap_delete_pdu(pdu);
        }
    }
    else if (type == SEND_TYPE_MULTICAST)
    {
        OIC_LOG(DEBUG, TAG, "both requestInfo & responseInfo is not available");

        coap_pdu_t *pdu = NULL;
        CAInfo_t info;
        memset(&info, 0, sizeof(CAInfo_t));

        info.options = data->options;
        info.numOptions = data->numOptions;
        info.token = data->requestInfo->info.token;
        info.type = data->requestInfo->info.type;

        pdu = (coap_pdu_t *) CAGeneratePdu(data->remoteEndpoint->resourceUri, CA_GET, info);

        if (NULL != pdu)
        {
            OIC_LOG_V(DEBUG, TAG, "PDU Maker - payload : %s", pdu->data);

            OIC_LOG_V(DEBUG, TAG, "PDU Maker - type : %d", pdu->hdr->type);

            OIC_LOG_V(DEBUG, TAG, "PDU Maker - code : %d", pdu->hdr->code);

            OIC_LOG_V(DEBUG, TAG, "PDU Maker - id : %d", ntohs(pdu->hdr->id));

            OIC_LOG_V(DEBUG, TAG, "PDU Maker - token : %s", pdu->hdr->token);

            OIC_LOG_V(DEBUG, TAG, "PDU Maker - buffer data : %s", pdu->hdr);

            res = CASendMulticastData(pdu->hdr, pdu->length);
            coap_delete_pdu(pdu);
        }
    }

    OIC_LOG_V(DEBUG, TAG, " Result :%d", res);
}

static void CAReceivedPacketCallback(CARemoteEndpoint_t *endpoint, void *data,
                                     uint32_t dataLen)
{
    OIC_LOG(DEBUG, TAG, "receivedPacketCallback in message handler!!");

    if (NULL == data)
    {
        OIC_LOG(DEBUG, TAG, "received data is null");
        return;
    }

    coap_pdu_t *pdu;
    uint32_t code = CA_NOT_FOUND;

    OIC_LOG_V(DEBUG, TAG, "data : %s", data);
    pdu = (coap_pdu_t *) CAParsePDU((const char *) data, dataLen, &code);
    OICFree(data);

    if(NULL == pdu)
    {
        OIC_LOG(DEBUG, TAG, "pdu is null");
        return;
    }

    char uri[CA_MAX_URI_LENGTH] =
    { 0, };

    if (code == CA_GET || code == CA_POST || code == CA_PUT || code == CA_DELETE)
    {
        CARequestInfo_t *ReqInfo;
        ReqInfo = (CARequestInfo_t *) OICMalloc(sizeof(CARequestInfo_t));
        if (ReqInfo == NULL)
        {
            OIC_LOG(DEBUG, TAG, "CAReceivedPacketCallback, Memory allocation failed !");
            coap_delete_pdu(pdu);
            return;
        }
        memset(ReqInfo, 0, sizeof(CARequestInfo_t));
        CAGetRequestInfoFromPdu(pdu, ReqInfo, uri);

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
        OIC_LOG_V(DEBUG, TAG, "Request- token : %s", ReqInfo->info.token);
        OIC_LOG_V(DEBUG, TAG, "Request- msgID: %d", ReqInfo->info.messageId);

        if (NULL != endpoint)
        {
            endpoint->resourceUri = (char *) OICMalloc(strlen(uri) + 1);
            if (endpoint->resourceUri == NULL)
            {
                OIC_LOG(DEBUG, TAG, "CAReceivedPacketCallback, Memory allocation failed !");
                OICFree(ReqInfo);
                coap_delete_pdu(pdu);
                return;
            }
            memset(endpoint->resourceUri, 0, strlen(uri) + 1);
            memcpy(endpoint->resourceUri, uri, strlen(uri));
            OIC_LOG_V(DEBUG, TAG, "resource URI : %s", endpoint->resourceUri);
        }
        // store the data at queue.
        CAData_t *cadata = NULL;
        cadata = (CAData_t *) OICMalloc(sizeof(CAData_t));
        if (cadata == NULL)
        {
            OIC_LOG(DEBUG, TAG, "CAReceivedPacketCallback, Memory allocation failed !");
            if (endpoint != NULL && endpoint->resourceUri != NULL)
                OICFree(endpoint->resourceUri);
            OICFree(ReqInfo);
            coap_delete_pdu(pdu);
            return;
        }
        memset(cadata, 0, sizeof(CAData_t));

        cadata->type = SEND_TYPE_UNICAST;
        cadata->remoteEndpoint = endpoint;
        cadata->requestInfo = ReqInfo;
        cadata->responseInfo = NULL;
        CAQueueingThreadAddData(&gReceiveThread, cadata, sizeof(CAData_t));
    }
    else
    {
        CAResponseInfo_t *ResInfo;
        ResInfo = (CAResponseInfo_t *) OICMalloc(sizeof(CAResponseInfo_t));
        if (ResInfo == NULL)
        {
            OIC_LOG(DEBUG, TAG, "CAReceivedPacketCallback, Memory allocation failed !");
            coap_delete_pdu(pdu);
            return;
        }
        memset(ResInfo, 0, sizeof(CAResponseInfo_t));
        CAGetResponseInfoFromPdu(pdu, ResInfo, uri);

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

        OIC_LOG_V(DEBUG, TAG, "Response- response code: %d", ResInfo->result);
        OIC_LOG_V(DEBUG, TAG, "Response- token : %s", ResInfo->info.token);
        OIC_LOG_V(DEBUG, TAG, "Response- msgID: %d", ResInfo->info.messageId);

        if (NULL != endpoint)
        {
            endpoint->resourceUri = (char *) OICMalloc(strlen(uri) + 1);
            if (endpoint->resourceUri == NULL)
            {
                OIC_LOG(DEBUG, TAG, "CAReceivedPacketCallback, Memory allocation failed !");
                OICFree(ResInfo);
                coap_delete_pdu(pdu);
                return;
            }
            memset(endpoint->resourceUri, 0, strlen(uri) + 1);
            memcpy(endpoint->resourceUri, uri, strlen(uri));
            OIC_LOG_V(DEBUG, TAG, "resource URI : %s", endpoint->resourceUri);
        }

        // store the data at queue.
        CAData_t *cadata = NULL;
        cadata = (CAData_t *) OICMalloc(sizeof(CAData_t));
        if (cadata == NULL)
        {
            OIC_LOG(DEBUG, TAG, "CAReceivedPacketCallback, Memory allocation failed !");
            if (endpoint != NULL && endpoint->resourceUri != NULL)
                OICFree(endpoint->resourceUri);
            OICFree(ResInfo);
            coap_delete_pdu(pdu);
            return;
        }
        memset(cadata, 0, sizeof(CAData_t));

        cadata->type = SEND_TYPE_UNICAST;
        cadata->remoteEndpoint = endpoint;
        cadata->requestInfo = NULL;

        // for retransmission
        void *retransmissionPdu = NULL;
        CARetransmissionReceivedData(&gRetransmissionContext, endpoint, pdu->hdr, pdu->length, &retransmissionPdu);

        // get token from saved data in retransmission list
        if(retransmissionPdu && CA_EMPTY == code)
        {
            CAGetTokenFromPDU((const coap_hdr_t *)retransmissionPdu, &(ResInfo->info));
            OICFree(retransmissionPdu);
        }
        cadata->responseInfo = ResInfo;

        CAQueueingThreadAddData(&gReceiveThread, cadata, sizeof(CAData_t));
    }

    if(pdu)
    {
        coap_delete_pdu(pdu);
    }
}

static void CANetworkChangedCallback(CALocalConnectivity_t *info, CANetworkStatus_t status)
{
    OIC_LOG(DEBUG, TAG, "networkChangeCallback in message handler!!");

    OIC_LOG_V(DEBUG, TAG, "Changed Network Status: %d", status);
}

void CAHandleRequestResponseCallbacks()
{
    OIC_LOG_V(DEBUG, TAG, "CAHandleRequestResponseCallbacks IN");

#ifdef SINGLE_HANDLE
    // parse the data and call the callbacks.
    // #1 parse the data
    // #2 get endpoint

    u_mutex_lock(gReceiveThread.threadMutex);

    u_queue_message_t *item = u_queue_get_element(gReceiveThread.dataQueue);

    u_mutex_unlock(gReceiveThread.threadMutex);

    if (item == NULL)
        return;

    // get values
    void *msg = item->msg;

    if (msg == NULL)
        return;

    // get endpoint
    CAData_t *td = (CAData_t *) msg;
    CARemoteEndpoint_t *rep = td->remoteEndpoint;

    if (rep == NULL)
        return;

    if (td->requestInfo != NULL)
    {
        if (gRequestHandler)
        {
            OIC_LOG_V(DEBUG, TAG, "callback will be sent : %d", td->requestInfo->info.numOptions);
            gRequestHandler(rep, td->requestInfo);
        }

        if (NULL != td->requestInfo->info.options)
        {
            OICFree(td->requestInfo->info.options);
        }

        if (NULL != td->requestInfo->info.payload)
        {
            OICFree(td->requestInfo->info.payload);
        }

        if (NULL != td->requestInfo->info.token)
        {
            OICFree(td->requestInfo->info.token);
        }
        OICFree(td->requestInfo);
    }

    if (td->responseInfo != NULL)
    {
        if (gResponseHandler)
        {
            gResponseHandler(rep, td->responseInfo);
        }

        if (NULL != td->responseInfo->info.options)
        {
            OICFree(td->responseInfo->info.options);
        }

        if (NULL != td->responseInfo->info.payload)
        {
            OICFree(td->responseInfo->info.payload);
        }

        if (NULL != td->responseInfo->info.token)
        {
            OICFree(td->responseInfo->info.token);
        }
        OICFree(td->responseInfo);
    }

    if (NULL != rep->resourceUri)
    {
        OICFree(rep->resourceUri);
    }

    OICFree(rep);


#endif
    OIC_LOG_V(DEBUG, TAG, "CAHandleRequestResponseCallbacks OUT");
}

CAResult_t CADetachRequestMessage(const CARemoteEndpoint_t *object,
                                  const CARequestInfo_t *request)
{
    OIC_LOG_V(DEBUG, TAG, "CADetachRequestMessage");

    if (object == NULL || request == NULL)
    {
        return CA_STATUS_FAILED;
    }

    CARemoteEndpoint_t *remoteEndpoint = NULL;
    CARequestInfo_t *requestInfo = NULL;

    CAData_t *data = (CAData_t *) OICMalloc(sizeof(CAData_t));
    MEMORY_ALLOC_CHECK(data);

    // initialize
    memset(data, 0, sizeof(CAData_t));

    // clone remote endpoint

    remoteEndpoint = CACloneRemoteEndpoint(object);
    MEMORY_ALLOC_CHECK(remoteEndpoint);

    // clone request info
    requestInfo = CACloneRequestInfo(request);
    MEMORY_ALLOC_CHECK(requestInfo);

    // save data
    data->type = SEND_TYPE_UNICAST;
    data->remoteEndpoint = remoteEndpoint;
    data->requestInfo = requestInfo;
    data->responseInfo = NULL;

    // add thread
    CAQueueingThreadAddData(&gSendThread, data, sizeof(CAData_t));

    return CA_STATUS_OK;

    // memory error label.
memory_error_exit:

    CADestroyRemoteEndpointInternal(remoteEndpoint);

    CADestroyRequestInfoInternal(requestInfo);

    if (data != NULL)
    {
        OICFree(data);
    }

    return CA_MEMORY_ALLOC_FAILED;
}

CAResult_t CADetachRequestToAllMessage(const CAGroupEndpoint_t *object,
                                       const CARequestInfo_t *request)
{
    // ToDo
    OIC_LOG_V(DEBUG, TAG, "CADetachRequestToAllMessage");


    if (object == NULL || request == NULL)
    {
        return CA_STATUS_FAILED;
    }

    CARemoteEndpoint_t *remoteEndpoint = NULL;
    CARequestInfo_t *requestInfo = NULL;

    CAData_t *data = (CAData_t *) OICMalloc(sizeof(CAData_t));
    MEMORY_ALLOC_CHECK(data);

    // initialize
    memset(data, 0, sizeof(CAData_t));

    CAAddress_t addr;
    memset(&addr, 0, sizeof(CAAddress_t));
    remoteEndpoint = CACreateRemoteEndpointInternal(object->resourceUri, addr,
                                         object->connectivityType);

    // clone request info
    requestInfo = CACloneRequestInfo(request);
    MEMORY_ALLOC_CHECK(requestInfo);

    // save data
    data->type = SEND_TYPE_MULTICAST;
    data->remoteEndpoint = remoteEndpoint;
    data->requestInfo = requestInfo;
    data->responseInfo = NULL;

    // add thread
    CAQueueingThreadAddData(&gSendThread, data, sizeof(CAData_t));

    return CA_STATUS_OK;

    // memory error label.
memory_error_exit:

    CADestroyRemoteEndpointInternal(remoteEndpoint);

    CADestroyRequestInfoInternal(requestInfo);

    if (data != NULL)
    {
        OICFree(data);
    }

    return CA_MEMORY_ALLOC_FAILED;
}

CAResult_t CADetachResponseMessage(const CARemoteEndpoint_t *object,
                                   const CAResponseInfo_t *response)
{
    OIC_LOG_V(DEBUG, TAG, "CADetachResponseMessage");

    if (object == NULL || response == NULL)
    {
        return CA_STATUS_FAILED;
    }

    CARemoteEndpoint_t *remoteEndpoint = NULL;
    CAResponseInfo_t *responseInfo = NULL;

    CAData_t *data = (CAData_t *) OICMalloc(sizeof(CAData_t));
    MEMORY_ALLOC_CHECK(data);

    // initialize
    memset(data, 0, sizeof(CAData_t));

    // clone remote endpoint
    remoteEndpoint = CACloneRemoteEndpoint(object);
    MEMORY_ALLOC_CHECK(remoteEndpoint);

    // clone response info
    responseInfo = CACloneResponseInfo(response);
    MEMORY_ALLOC_CHECK(responseInfo);

    // save data
    data->type = SEND_TYPE_UNICAST;
    data->remoteEndpoint = remoteEndpoint;
    data->requestInfo = NULL;
    data->responseInfo = responseInfo;

    // add thread
    CAQueueingThreadAddData(&gSendThread, data, sizeof(CAData_t));

    return CA_STATUS_OK;

    // memory error label.
memory_error_exit:

    CADestroyRemoteEndpointInternal(remoteEndpoint);

    CADestroyResponseInfoInternal(responseInfo);

    if (data != NULL)
    {
        OICFree(data);
    }

    return CA_MEMORY_ALLOC_FAILED;
}

CAResult_t CADetachMessageResourceUri(const CAURI_t resourceUri, const CAToken_t token,
                                      const CAHeaderOption_t *options, uint8_t numOptions)
{
    if (resourceUri == NULL)
    {
        return CA_STATUS_FAILED;
    }
    CARequestInfo_t *ReqInfo = NULL;
    CAToken_t tempToken = NULL;
    CARemoteEndpoint_t *remoteEndpoint = NULL;

    CAData_t *data = (CAData_t *) OICMalloc(sizeof(CAData_t));
    MEMORY_ALLOC_CHECK(data);

    // initialize
    memset(data, 0, sizeof(CAData_t));

    CAAddress_t addr;
    memset(&addr, 0, sizeof(CAAddress_t));
    remoteEndpoint = CACreateRemoteEndpointInternal(resourceUri, addr,
                                         CA_ETHERNET | CA_WIFI | CA_EDR | CA_LE);

    // create request info
    ReqInfo = (CARequestInfo_t *) OICMalloc(sizeof(CARequestInfo_t));
    MEMORY_ALLOC_CHECK(ReqInfo);
    memset(ReqInfo, 0, sizeof(CARequestInfo_t));

    // copy token value
    if (token != NULL)
    {
        int32_t len = strlen(token);
        tempToken = (char *) OICMalloc(sizeof(char) * (len + 1));
        MEMORY_ALLOC_CHECK(tempToken);
        memset(tempToken, 0, sizeof(char) * (len + 1));
        strncpy(tempToken, token, len);
    }

    // save request info data
    ReqInfo->method = CA_GET;
    ReqInfo->info.token = tempToken;
    ReqInfo->info.type = CA_MSG_NONCONFIRM;
    // save data
    data->type = SEND_TYPE_MULTICAST;
    data->remoteEndpoint = remoteEndpoint;
    data->requestInfo = ReqInfo;

    data->responseInfo = NULL;
    data->options = NULL;
    data->numOptions = 0;

    if (options != NULL && numOptions > 0)
    {
        // copy data
        CAHeaderOption_t *temp = (CAHeaderOption_t *) OICMalloc(
                                     sizeof(CAHeaderOption_t) * numOptions);

        MEMORY_ALLOC_CHECK(temp);

        memset(temp, 0, sizeof(CAHeaderOption_t) * numOptions);
        memcpy(temp, options, sizeof(CAHeaderOption_t) * numOptions);

        data->options = temp;
        data->numOptions = numOptions;
    }

    // add thread
    CAQueueingThreadAddData(&gSendThread, data, sizeof(CAData_t));

    return CA_STATUS_OK;

    // memory error label.
memory_error_exit:

    CADestroyRemoteEndpointInternal(remoteEndpoint);

    if (tempToken != NULL)
    {
        OICFree(tempToken);
    }

    if (ReqInfo != NULL)
    {
        OICFree(ReqInfo);
    }

    if (data != NULL)
    {
        OICFree(data);
    }

    return CA_MEMORY_ALLOC_FAILED;
}

void CASetRequestResponseCallbacks(CARequestCallback ReqHandler,
                                   CAResponseCallback RespHandler)
{
    OIC_LOG_V(DEBUG, TAG, "set request, response handler callback.");

    gRequestHandler = ReqHandler;
    gResponseHandler = RespHandler;
}

CAResult_t CAInitializeMessageHandler()
{
    OIC_LOG(DEBUG, TAG, "CAInitializeMessageHandler - Entry");
    CASetPacketReceivedCallback(CAReceivedPacketCallback);

    CASetNetworkChangeCallback(CANetworkChangedCallback);

    // create thread pool
    CAResult_t res;
    res = u_thread_pool_init(MAX_THREAD_POOL_SIZE, &gThreadPoolHandle);

    if (res != CA_STATUS_OK)
    {
        OIC_LOG_V(DEBUG, TAG, "thread pool initialize error.");
        return res;
    }

    // send thread initialize
    CAQueueingThreadInitialize(&gSendThread, gThreadPoolHandle, CASendThreadProcess,
            CADataDestroyer);

    // start send thread
    res = CAQueueingThreadStart(&gSendThread);

    if (res != CA_STATUS_OK)
    {
        OIC_LOG_V(DEBUG, TAG, "thread start error(send thread).");
        return res;
    }

    // receive thread initialize
    CAQueueingThreadInitialize(&gReceiveThread, gThreadPoolHandle, CAReceiveThreadProcess,
            CADataDestroyer);

#ifndef SINGLE_HANDLE// This will be enabled when RI supports multi threading
    // start receive thread
    res = CAQueueingThreadStart(&gReceiveThread);

    if (res != CA_STATUS_OK)
    {
        OIC_LOG_V(DEBUG, TAG, "thread start error(receive thread).");
        return res;
    }
#endif

    // retransmission initialize
    CARetransmissionInitialize(&gRetransmissionContext, gThreadPoolHandle, CASendUnicastData,
            CATimeoutCallback, NULL);

    // start retransmission
    res = CARetransmissionStart(&gRetransmissionContext);

    if (res != CA_STATUS_OK)
    {
        OIC_LOG_V(DEBUG, TAG, "thread start error(retransmission thread).");
        return res;
    }

    // initialize interface adapters by controller
    CAInitializeAdapters(gThreadPoolHandle);

    return CA_STATUS_OK;
}

void CATerminateMessageHandler()
{
    // terminate interface adapters by controller
    CATerminateAdapters();

    // stop retransmission
    if(gRetransmissionContext.threadMutex != NULL) {
        CARetransmissionStop(&gRetransmissionContext);
        CARetransmissionDestroy(&gRetransmissionContext);
    }

    // stop thread
    // delete thread data
    if(gSendThread.threadMutex != NULL) {
        CAQueueingThreadStop(&gSendThread);
        CAQueueingThreadDestroy(&gSendThread);
    }

    // stop thread
    // delete thread data
    if(gReceiveThread.threadMutex != NULL) {
	#ifndef SINGLE_HANDLE// This will be enabled when RI supports multi threading
        // CAQueueingThreadStop(&gReceiveThread);
	#endif
        CAQueueingThreadDestroy(&gReceiveThread);
    }

    // destroy thread pool
    if(gThreadPoolHandle != NULL) {
        u_thread_pool_free(gThreadPoolHandle);
        gThreadPoolHandle = NULL;
    }

    OIC_LOG_V(DEBUG, TAG, "message handler termination is complete!");
}


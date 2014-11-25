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
#include "umutex.h"
#include "oic_malloc.h"

#define TAG PCF("CA")

#define MEMORY_ALLOCK_CHECK(arg) { if (arg == NULL) {OIC_LOG_V(DEBUG, TAG, "memory error"); goto memory_error_exit;} }

#define MAX_THREAD_POOL_SIZE    10

typedef enum
{
    SEND_TYPE_MULTICAST = 0, SEND_TYPE_UNICAST
} CASendDataType_t;

typedef struct
{
    CASendDataType_t type;
    CARemoteEndpoint_t* remoteEndpoint;
    CARequestInfo_t* requestInfo;
    CAResponseInfo_t* responseInfo;
    CAHeaderOption_t* options;
    uint8_t numOptions;
} CAData_t;

// thread pool handle
static u_thread_pool_t gThreadPoolHandle = NULL;

// message handler main thread
static CAQueueingThread_t gSendThread;
static CAQueueingThread_t gReceiveThread;

// handler field
static CARequestCallback gRequestHandler = NULL;
static CAResponseCallback gResponseHandler = NULL;

static void CAReceiveThreadProcess(void* threadData)
{
    // TODO
    // currently not support.
    // CAHandleRequestResponseCallbacks codes will move to this function.
}

static void CASendThreadProcess(void* threadData)
{
    CAData_t* data = (CAData_t*) threadData;

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
        coap_pdu_t* pdu = NULL;

        if (data->requestInfo != NULL)
        {
            OIC_LOG_V(DEBUG, TAG, "requestInfo is available..");

            pdu = (coap_pdu_t*) CAGeneratePdu(data->remoteEndpoint->resourceUri,
                    data->requestInfo->method, data->requestInfo->info);
        }
        else if (data->responseInfo != NULL)
        {
            OIC_LOG_V(DEBUG, TAG, "responseInfo is available..");

            pdu = (coap_pdu_t*) CAGeneratePdu(data->remoteEndpoint->resourceUri,
                    data->responseInfo->result, data->responseInfo->info);
        }
        else
        {
            OIC_LOG(DEBUG, TAG, "request info, response info is empty");
        }

        // interface controller function call.
        if (NULL != pdu)
        {
            OIC_LOG_V(DEBUG, TAG, "PDU Maker - payload : %s", pdu->data);

            OIC_LOG_V(DEBUG, TAG, "PDU Maker - code : %d", pdu->hdr->code);

            OIC_LOG_V(DEBUG, TAG, "PDU Maker - buffer data : %s", pdu->hdr);

            res = CASendUnicastData(data->remoteEndpoint, pdu->hdr, pdu->length);
        }

    }
    else if (type == SEND_TYPE_MULTICAST)
    {
        OIC_LOG(DEBUG, TAG, "both requestInfo & responseInfo is not available");

        coap_pdu_t* pdu = NULL;
        CAInfo_t info;
        memset(&info, 0, sizeof(CAInfo_t));

        info.options = data->options;
        info.numOptions = data->numOptions;

        pdu = (coap_pdu_t*) CAGeneratePdu(data->remoteEndpoint->resourceUri, CA_GET, info);

        if (NULL != pdu)
        {
            OIC_LOG_V(DEBUG, TAG, "PDU Maker - payload : %s", pdu->data);

            OIC_LOG_V(DEBUG, TAG, "PDU Maker - type : %d", pdu->hdr->type);

            OIC_LOG_V(DEBUG, TAG, "PDU Maker - code : %d", pdu->hdr->code);

            OIC_LOG_V(DEBUG, TAG, "PDU Maker - id : %d", pdu->hdr->id);

            OIC_LOG_V(DEBUG, TAG, "PDU Maker - buffer data : %s", pdu->hdr);

            res = CASendMulticastData(pdu->hdr, pdu->length);
        }
    }
    else
    {
        OIC_LOG(DEBUG, TAG, "unknown type!");
    }

}

static void CAReceivedPacketCallback(CARemoteEndpoint_t* endpoint, void* data, uint32_t dataLen)
{
    OIC_LOG(DEBUG, TAG, "receivedPacketCallback in message handler!!");

    if (NULL == data)
    {
        OIC_LOG(DEBUG, TAG, "received data is null");
        return;
    }

    coap_pdu_t* pdu;
    uint32_t code = CA_NOT_FOUND;
    pdu = (coap_pdu_t*) CAParsePDU((const char*) data, &code);
    OICFree(data);

    char uri[CA_MAX_URI_LENGTH] =
    { 0, };

    if (code == CA_GET || code == CA_POST || code == CA_PUT || code == CA_DELETE)
    {
        CARequestInfo_t* ReqInfo;
        ReqInfo = (CARequestInfo_t*) OICMalloc(sizeof(CARequestInfo_t));
        memset(ReqInfo, 0, sizeof(CARequestInfo_t));
        CAGetRequestInfoFromPdu(pdu, ReqInfo, uri);

        if (NULL != ReqInfo->info.options && NULL != endpoint)
        {
            uint32_t i;
            for (i = 0; i < ReqInfo->info.numOptions; i++)
            {
                OIC_LOG_V(DEBUG, TAG, "Request- optionID: %d", ReqInfo->info.options[i].optionID);

                OIC_LOG_V(DEBUG, TAG, "Request- list: %s", ReqInfo->info.options[i].optionData);
            }

            OIC_LOG_V(DEBUG, TAG, "Request- payload: %s", ReqInfo->info.payload);

            OIC_LOG_V(DEBUG, TAG, "Request- code: %d", ReqInfo->method);

            endpoint->resourceUri = (char*) OICMalloc(strlen(uri) + 1);
            memset(endpoint->resourceUri, 0, strlen(uri) + 1);
            memcpy(endpoint->resourceUri, uri, strlen(uri));
            OIC_LOG_V(DEBUG, TAG, "added resource URI : %s", endpoint->resourceUri);
        }

        // store the data at queue.
        CAData_t* cadata = NULL;
        cadata = (CAData_t*) OICMalloc(sizeof(CAData_t));
        memset(cadata, 0, sizeof(CAData_t));

        cadata->type = SEND_TYPE_UNICAST;
        cadata->remoteEndpoint = endpoint;
        cadata->requestInfo = ReqInfo;
        cadata->responseInfo = NULL;
        CAQueueingThreadAddData(&gReceiveThread, cadata, sizeof(CAData_t));
    }
    else
    {
        CAResponseInfo_t* ResInfo;
        ResInfo = (CAResponseInfo_t*) OICMalloc(sizeof(CAResponseInfo_t));
        memset(ResInfo, 0, sizeof(CAResponseInfo_t));
        CAGetResponseInfoFromPdu(pdu, ResInfo, uri);

        if (NULL != ResInfo->info.options && NULL != endpoint)
        {
            uint32_t i;
            for (i = 0; i < ResInfo->info.numOptions; i++)
            {
                OIC_LOG_V(DEBUG, TAG, "Response- optionID: %d", ResInfo->info.options[i].optionID);

                OIC_LOG_V(DEBUG, TAG, "Response- list: %s", ResInfo->info.options[i].optionData);
            }

            OIC_LOG_V(DEBUG, TAG, "Response- payload: %s", ResInfo->info.payload);

            OIC_LOG_V(DEBUG, TAG, "Response- code: %d", ResInfo->result);

            endpoint->resourceUri = (char*) OICMalloc(strlen(uri) + 1);
            memset(endpoint->resourceUri, 0, strlen(uri) + 1);
            memcpy(endpoint->resourceUri, uri, strlen(uri));
            OIC_LOG_V(DEBUG, TAG, "added resource URI : %s", endpoint->resourceUri);
        }

        // store the data at queue.
        CAData_t* cadata = NULL;
        cadata = (CAData_t*) OICMalloc(sizeof(CAData_t));
        memset(cadata, 0, sizeof(CAData_t));

        cadata->type = SEND_TYPE_UNICAST;
        cadata->remoteEndpoint = endpoint;
        cadata->requestInfo = NULL;
        cadata->responseInfo = ResInfo;

        CAQueueingThreadAddData(&gReceiveThread, cadata, sizeof(CAData_t));
    }
}

static void CANetworkChangedCallback(CALocalConnectivity_t* info, CANetworkStatus_t status)
{
    OIC_LOG(DEBUG, TAG, "networkChangeCallback in message handler!!");
}

void CAHandleRequestResponseCallbacks()
{
    OIC_LOG_V(DEBUG, TAG, "CAHandleRequestResponseCallbacks");

    // parse the data and call the callbacks.
    // #1 parse the data
    // #2 get endpoint

    u_mutex_lock(gReceiveThread.threadMutex);

    u_queue_message_t* item = u_queue_get_element(gReceiveThread.dataQueue);

    u_mutex_unlock(gReceiveThread.threadMutex);

    if (item == NULL)
        return;

    // get values
    void* msg = item->msg;

    if (msg == NULL)
        return;

    // get endpoint
    CAData_t* td = (CAData_t*) msg;
    CARemoteEndpoint_t* rep = td->remoteEndpoint;

    if (rep == NULL)
        return;

    if (td->requestInfo != NULL)
    {
        if (gRequestHandler)
        {
            gRequestHandler(rep, td->requestInfo);
        }

        if(NULL != td->requestInfo->info.options)
        {
            OICFree(td->requestInfo->info.options);
        }

        if(NULL != td->requestInfo->info.payload)
        {
            OICFree(td->requestInfo->info.payload);
        }

        if(NULL != td->requestInfo->info.token)
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

        if(NULL != td->responseInfo->info.options)
        {
            OICFree(td->responseInfo->info.options);
        }

        if(NULL != td->responseInfo->info.payload)
        {
            OICFree(td->responseInfo->info.payload);
        }

        if(NULL != td->responseInfo->info.token)
        {
            OICFree(td->responseInfo->info.token);
        }
        OICFree(td->responseInfo);
    }

    if(NULL != rep->resourceUri)
    {
        OICFree(rep->resourceUri);
    }
    OICFree(rep);
}

CAResult_t CADetachRequestMessage(const CARemoteEndpoint_t* object, const CARequestInfo_t* request)
{
    OIC_LOG_V(DEBUG, TAG, "CADetachRequestMessage");

    if (object == NULL || request == NULL)
    {
        return CA_STATUS_FAILED;
    }

    CAData_t* data = (CAData_t*) OICMalloc(sizeof(CAData_t));
    MEMORY_ALLOCK_CHECK(data);

    // initialize
    memset(data, 0, sizeof(CAData_t));

    // clone remote endpoint
    CARemoteEndpoint_t* remoteEndpoint = CACloneRemoteEndpoint(object);
    MEMORY_ALLOCK_CHECK(remoteEndpoint);

    // clone request info
    CARequestInfo_t* requestInfo = CACloneRequestInfo(request);
    MEMORY_ALLOCK_CHECK(requestInfo);

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

CAResult_t CADetachResponseMessage(const CARemoteEndpoint_t* object,
        const CAResponseInfo_t* response)
{
    OIC_LOG_V(DEBUG, TAG, "CADetachResponseMessage");

    if (object == NULL || response == NULL)
    {
        return CA_STATUS_FAILED;
    }

    CAData_t* data = (CAData_t*) OICMalloc(sizeof(CAData_t));
    MEMORY_ALLOCK_CHECK(data);

    // initialize
    memset(data, 0, sizeof(CAData_t));

    // clone remote endpoint
    CARemoteEndpoint_t* remoteEndpoint = CACloneRemoteEndpoint(object);
    MEMORY_ALLOCK_CHECK(remoteEndpoint);

    // clone response info
    CAResponseInfo_t* responseInfo = CACloneResponseInfo(response);
    MEMORY_ALLOCK_CHECK(responseInfo);

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

CAResult_t CADetachMessageResourceUri(const CAURI_t resourceUri, const CAHeaderOption_t* options,
        uint8_t numOptions)
{
    if (resourceUri == NULL)
    {
        return CA_STATUS_FAILED;
    }

    CAData_t* data = (CAData_t*) OICMalloc(sizeof(CAData_t));
    MEMORY_ALLOCK_CHECK(data);

    // initialize
    memset(data, 0, sizeof(CAData_t));

    CAAddress_t addr;
    memset(&addr, 0, sizeof(CAAddress_t));
    CARemoteEndpoint_t* remoteEndpoint = CACreateRemoteEndpointInternal(resourceUri, addr,
            CA_ETHERNET | CA_WIFI | CA_EDR | CA_LE);

    // save data
    data->type = SEND_TYPE_MULTICAST;
    data->remoteEndpoint = remoteEndpoint;
    data->requestInfo = NULL;
    data->responseInfo = NULL;
    data->options = NULL;
    data->numOptions = 0;

    if (options != NULL && numOptions > 0)
    {
        // copy data
        CAHeaderOption_t* temp = (CAHeaderOption_t*) OICMalloc(
                sizeof(CAHeaderOption_t) * numOptions);

        MEMORY_ALLOCK_CHECK(temp);

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

    if (data != NULL)
    {
        OICFree(data);
    }

    return CA_MEMORY_ALLOC_FAILED;
}

void CASetRequestResponseCallbacks(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
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
    CAQueueingThreadInitialize(&gSendThread, gThreadPoolHandle, CASendThreadProcess);

    // start send thread
    res = CAQueueingThreadStart(&gSendThread);

    if (res != CA_STATUS_OK)
    {
        OIC_LOG_V(DEBUG, TAG, "thread start error(send thread).");
        return res;
    }

    // receive thread initialize
    CAQueueingThreadInitialize(&gReceiveThread, gThreadPoolHandle, CAReceiveThreadProcess);

    // start receive thread
    // TODO
    // currently not support.
//    res = CAThreadStart(gReceiveThread);
//
//    if (res != CA_STATUS_OK)
//    {
//        OIC_LOG_V(DEBUG, TAG, "thread start error(receive thread).");
//        return res;
//    }

    // initialize interface adapters by controller
    CAInitializeAdapters(gThreadPoolHandle);

    return CA_STATUS_OK;
}

void CATerminateMessageHandler()
{
    // terminate interface adapters by controller
    CATerminateAdapters();

    // stop thread
    // delete thread data
    CAQueueingThreadStop(&gSendThread);
    CAQueueingThreadDestroy(&gSendThread);

    // stop thread
    // delete thread data
    CAQueueingThreadStop(&gReceiveThread);
    CAQueueingThreadDestroy(&gReceiveThread);

    // destroy thread pool
    u_thread_pool_free(gThreadPoolHandle);

    OIC_LOG_V(DEBUG, TAG, "message handler terminate completed!");
}


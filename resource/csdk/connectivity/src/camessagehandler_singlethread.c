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

#include "camessagehandler_singlethread.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "cainterface.h"
#include "caremotehandler.h"
#include "cainterfacecontroller_singlethread.h"
#include "caprotocolmessage_singlethread.h"
#include "logger.h"
#include "config.h" /* for coap protocol */
#include "coap.h"
#include "oic_malloc.h"
#include "caadapterutils.h"

#define TAG "CAMH_ST"

#define MEMORY_ALLOC_CHECK(arg) { if (arg == NULL) {OIC_LOG_V(DEBUG, TAG, "Out of memory");\
    goto memory_error_exit;} }

#define MAX_THREAD_POOL_SIZE    10
#define CA_MAX_RT_ARRAY_SIZE    3

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

// message handler callback
static CAMessageHandlerCallback gHandlerCallback = NULL;

static CARetransmission_t gRetransmissionContext;

// handler field
static CARequestCallback gRequestHandler = NULL;
static CAResponseCallback gResponseHandler = NULL;

static void CAProcessData(CAData_t *data)
{
    OIC_LOG(DEBUG, TAG, "IN");
    VERIFY_NON_NULL_VOID(data, TAG, "data");
    VERIFY_NON_NULL_VOID(data->remoteEndpoint, TAG, "remoteendpoint");

    CAResult_t res = CA_STATUS_FAILED;

    CASendDataType_t type = data->type;

    if (type == SEND_TYPE_UNICAST)
    {
        coap_pdu_t *pdu = NULL;

        if (data->requestInfo != NULL)
        {
            OIC_LOG_V(DEBUG, TAG, "reqInfo avlbl");

            pdu = (coap_pdu_t *) CAGeneratePdu(data->remoteEndpoint->resourceUri,
                                               data->requestInfo->method, data->requestInfo->info);
        }
        else if (data->responseInfo != NULL)
        {
            OIC_LOG_V(DEBUG, TAG, "resInfo avlbl");

            pdu = (coap_pdu_t *) CAGeneratePdu(data->remoteEndpoint->resourceUri,
                                               data->responseInfo->result, data->responseInfo->info);
        }
        else
        {
            OIC_LOG(DEBUG, TAG, "request info, response info is empty");
        }

        // interface controller function call.
        if (NULL != pdu)
        {
            OIC_LOG_V(DEBUG, TAG, "payload: %s", pdu->data);

            OIC_LOG_V(DEBUG, TAG, "code: %d", pdu->hdr->code);

            OIC_LOG_V(DEBUG, TAG, "buffer: %s", pdu->hdr);

            res = CASendUnicastData(data->remoteEndpoint, pdu->hdr, pdu->length);

            // for retransmission
            CARetransmissionSentData(&gRetransmissionContext, data->remoteEndpoint, pdu->hdr,
                                     pdu->length);
        }
        free(pdu);
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

        pdu = (coap_pdu_t *) CAGeneratePdu(data->remoteEndpoint->resourceUri, CA_GET, info);

        if (NULL != pdu)
        {
            OIC_LOG_V(DEBUG, TAG, "PDU Maker - payload : %s", pdu->data);

            OIC_LOG_V(DEBUG, TAG, "PDU Maker - type : %d", pdu->hdr->type);

            OIC_LOG_V(DEBUG, TAG, "PDU Maker - code : %d", pdu->hdr->code);

            OIC_LOG_V(DEBUG, TAG, "PDU Maker - id : %d", pdu->hdr->id);

            OIC_LOG_V(DEBUG, TAG, "PDU Maker - token : %s", pdu->hdr->token);
            OIC_LOG_V(DEBUG, TAG, "PDU Maker - buffer data : %s", pdu->hdr);

            res = CASendMulticastData(pdu->hdr, pdu->length);
        }
        free(pdu);
    }
    else
    {
        OIC_LOG(DEBUG, TAG, "unknown type!");
    }

    if (gHandlerCallback != NULL)
    {
        gHandlerCallback("", res);
    }
    OIC_LOG(DEBUG, TAG, "OUT");
}

static void CATimeoutCallback(const CARemoteEndpoint_t *endpoint, void *pdu, uint32_t size)
{
    OIC_LOG(DEBUG, TAG, "IN");
    CARemoteEndpoint_t* ep = CACloneRemoteEndpoint(endpoint);
    if (ep == NULL)
    {
        OIC_LOG(DEBUG, TAG, "error");
        return;
    }

    CAResponseInfo_t* resInfo = (CAResponseInfo_t*) OICMalloc(sizeof(CAResponseInfo_t));

    if (resInfo == NULL)
    {
        OIC_LOG(DEBUG, TAG, "error");
        CADestroyRemoteEndpointInternal(ep);
        return;
    }
    memset(resInfo, 0, sizeof(CAResponseInfo_t));

    CAMessageType_t type = CAGetMessageTypeFromPduBinaryData(pdu, size);
    uint16_t messageId = CAGetMessageIdFromPduBinaryData(pdu, size);

    resInfo->result = CA_RETRANSMIT_TIMEOUT;
    resInfo->info.type = type;
    resInfo->info.messageId = messageId;

    if (gResponseHandler)
    {
        gResponseHandler(ep, resInfo);
    }
    CADestroyRemoteEndpointInternal(ep);
    OICFree(resInfo);
    OIC_LOG(DEBUG, TAG, "OUT");
}

static void CAReceivedPacketCallback(CARemoteEndpoint_t *endpoint, void *data,
    uint32_t dataLen)
{
    OIC_LOG(DEBUG, TAG, "IN");
    VERIFY_NON_NULL_VOID(data, TAG, "data");

    coap_pdu_t *pdu;
    uint32_t code = CA_NOT_FOUND;
    pdu = (coap_pdu_t *) CAParsePDU((const char *) data, dataLen, &code);
    //OICFree(data);

    char uri[CA_MAX_URI_LENGTH] = { 0, };

    if (CA_GET == code || CA_POST == code  || CA_PUT == code  || CA_DELETE == code )
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

        if (NULL != endpoint)
        {
            endpoint->resourceUri = (char *) OICMalloc(strlen(uri) + 1);
            if (endpoint->resourceUri == NULL)
            {
                OIC_LOG(DEBUG, TAG, "CAReceivedPacketCallback, Memory allocation failed !");
                coap_delete_pdu(pdu);
                OICFree(ReqInfo);
                return;
            }
            memset(endpoint->resourceUri, 0, strlen(uri) + 1);
            memcpy(endpoint->resourceUri, uri, strlen(uri));
            OIC_LOG_V(DEBUG, TAG, "URI : %s", endpoint->resourceUri);
        }

        if (ReqInfo)
        {
            if (gRequestHandler)
            {
                gRequestHandler(endpoint, ReqInfo);
            }

            CADestroyRequestInfoInternal(ReqInfo);
        }
    }
    else
    {
        CAResponseInfo_t *ResInfo = (CAResponseInfo_t *) OICMalloc(sizeof(CAResponseInfo_t));
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
                OIC_LOG_V(DEBUG, TAG, "optionID: %d", ResInfo->info.options[i].optionID);

                OIC_LOG_V(DEBUG, TAG, "list: %s", ResInfo->info.options[i].optionData);
            }

            if (NULL != ResInfo->info.payload)
            {
                OIC_LOG_V(DEBUG, TAG, "payload: %s", ResInfo->info.payload);
            }

            OIC_LOG_V(DEBUG, TAG, "code: %d", ResInfo->result);
        }

        if (NULL != endpoint)
        {
            endpoint->resourceUri = (char *) OICMalloc(strlen(uri) + 1);
            if (endpoint->resourceUri == NULL)
            {
                OIC_LOG(DEBUG, TAG, "CAReceivedPacketCallback, Memory allocation failed !");
                coap_delete_pdu(pdu);
                OICFree(ResInfo);
                return;
            }
            memset(endpoint->resourceUri, 0, strlen(uri) + 1);
            memcpy(endpoint->resourceUri, uri, strlen(uri));
            OIC_LOG_V(DEBUG, TAG, "URI : %s", endpoint->resourceUri);
        }

        if (ResInfo != NULL)
        {
            if (gResponseHandler)
            {
                gResponseHandler(endpoint, ResInfo);

                // for retransmission
                CARetransmissionReceivedData(&gRetransmissionContext, endpoint, pdu->hdr, pdu->length);
            }
            CADestroyResponseInfoInternal(ResInfo);
        }
    }

    if (endpoint && endpoint->resourceUri)
    {
        OICFree(endpoint->resourceUri);
        endpoint->resourceUri = NULL;
    }
    free(pdu);
    OIC_LOG(DEBUG, TAG, "OUT");
}

static void CANetworkChangedCallback(CALocalConnectivity_t *info, CANetworkStatus_t status)
{
    OIC_LOG(DEBUG, TAG, "IN");
    OIC_LOG(DEBUG, TAG, "OUT");
}

void CAHandleRequestResponseCallbacks()
{
    CAReadData();
    CARetransmissionBaseRoutine((void *)&gRetransmissionContext);
}

CAResult_t CADetachRequestMessage(const CARemoteEndpoint_t *object,
    const CARequestInfo_t *request)
{
    OIC_LOG(DEBUG, TAG, "IN");

    VERIFY_NON_NULL(object, TAG, "object");
    VERIFY_NON_NULL(request, TAG, "request");
    CARemoteEndpoint_t *remoteEndpoint = NULL;
    CARequestInfo_t *requestInfo = NULL;

    // If max retransmission queue is reached, then don't handle new request
    if (CA_MAX_RT_ARRAY_SIZE == u_arraylist_length(gRetransmissionContext.dataList))
    {
        OIC_LOG(DEBUG, TAG, "max RT queue size rchd");
        return CA_SEND_FAILED;
    }

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

    CAProcessData(data);
    CADestroyRemoteEndpoint(remoteEndpoint);
    CADestroyRequestInfoInternal(requestInfo);
    OICFree(data);
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;

    // memory error label.
memory_error_exit:

    CADestroyRemoteEndpointInternal(remoteEndpoint);

    CADestroyRequestInfoInternal(requestInfo);

    if (data != NULL)
    {
        OICFree(data);
    }
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_MEMORY_ALLOC_FAILED;
}

CAResult_t CADetachResponseMessage(const CARemoteEndpoint_t *object,
                                   const CAResponseInfo_t *response)
{
    OIC_LOG(DEBUG, TAG, "IN");
    VERIFY_NON_NULL(object, TAG, "object");
    VERIFY_NON_NULL(response, TAG, "response");
    CAResponseInfo_t *responseInfo = NULL;
    CARemoteEndpoint_t *remoteEndpoint = NULL;
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

    CAProcessData(data);
    CADestroyRemoteEndpoint(remoteEndpoint);
    CADestroyResponseInfoInternal(responseInfo);
    OICFree(data);
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;

    // memory error label.
memory_error_exit:

    CADestroyRemoteEndpointInternal(remoteEndpoint);

    CADestroyResponseInfoInternal(responseInfo);

    if (data != NULL)
    {
        OICFree(data);
    }
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_MEMORY_ALLOC_FAILED;
}

CAResult_t CADetachMessageResourceUri(const CAURI_t resourceUri, const CAToken_t token,
                                      const CAHeaderOption_t *options,
                                      uint8_t numOptions)
{
    OIC_LOG(DEBUG, TAG, "IN");
    if (resourceUri == NULL)
    {
        return CA_STATUS_FAILED;
    }
    CARemoteEndpoint_t *remoteEndpoint = NULL;
    CAData_t *data = (CAData_t *) OICMalloc(sizeof(CAData_t));
    MEMORY_ALLOC_CHECK(data);

    // initialize
    memset(data, 0, sizeof(CAData_t));

    CAAddress_t addr;
    memset(&addr, 0, sizeof(CAAddress_t));
    remoteEndpoint = CACreateRemoteEndpointInternal(resourceUri, addr,
                     CA_ETHERNET | CA_WIFI | CA_EDR | CA_LE);

    // save data
    data->type = SEND_TYPE_MULTICAST;
    data->remoteEndpoint = remoteEndpoint;
    CARequestInfo_t *ReqInfo = (CARequestInfo_t *) OICMalloc(sizeof(CARequestInfo_t));
    MEMORY_ALLOC_CHECK(ReqInfo);
    memset(ReqInfo, 0, sizeof(CARequestInfo_t));
    ReqInfo->method = CA_GET;
    ReqInfo->info.token = token;
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

    CAProcessData(data);
    CADestroyRemoteEndpoint(remoteEndpoint);
    OICFree(data);
    OICFree(ReqInfo);
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;

    // memory error label.
memory_error_exit:

    CADestroyRemoteEndpointInternal(remoteEndpoint);

    if (data != NULL)
    {
        OICFree(data);
    }
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_MEMORY_ALLOC_FAILED;
}

void CASetMessageHandlerCallback(CAMessageHandlerCallback callback)
{
    OIC_LOG(DEBUG, TAG, "IN");
    gHandlerCallback = callback;
    OIC_LOG(DEBUG, TAG, "OUT");
}

void CASetRequestResponseCallbacks(CARequestCallback ReqHandler,
    CAResponseCallback RespHandler)
{
    OIC_LOG(DEBUG, TAG, "IN");
    gRequestHandler = ReqHandler;
    gResponseHandler = RespHandler;
    OIC_LOG(DEBUG, TAG, "OUT");
}

CAResult_t CAInitializeMessageHandler()
{
    OIC_LOG(DEBUG, TAG, "IN");
    CASetPacketReceivedCallback(CAReceivedPacketCallback);

    CASetNetworkChangeCallback(CANetworkChangedCallback);

    CAResult_t res;

    // retransmission initialize
    CARetransmissionInitialize(&gRetransmissionContext, CASendUnicastData, CATimeoutCallback, NULL);

    CAInitializeAdapters();
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

void CATerminateMessageHandler()
{
    OIC_LOG(DEBUG, TAG, "IN");
    // terminate interface adapters by controller
    CATerminateAdapters();

    // stop retransmission
    CARetransmissionStop(&gRetransmissionContext);
    CARetransmissionDestroy(&gRetransmissionContext);

    OIC_LOG(DEBUG, TAG, "OUT");
}


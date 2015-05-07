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
#include "camessagehandler_singlethread.h"
#include "caremotehandler.h"
#include "cainterfacecontroller_singlethread.h"
#include "caprotocolmessage.h"
#include "caretransmission_singlethread.h"
#include "logger.h"
#include "config.h" /* for coap protocol */
#include "oic_malloc.h"

#define TAG "CAMH_ST"

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


static CARetransmission_t g_retransmissionContext;

// handler field
static CARequestCallback g_requestHandler = NULL;
static CAResponseCallback g_responseHandler = NULL;

static void CATimeoutCallback(const CARemoteEndpoint_t *endpoint, const void *pdu, uint32_t size)
{
    OIC_LOG(DEBUG, TAG, "IN");
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

    if (g_responseHandler)
    {
        g_responseHandler(ep, resInfo);
    }

    CADestroyRemoteEndpointInternal(ep);
    OICFree(resInfo);

    OIC_LOG(DEBUG, TAG, "OUT");
}
static void CAProcessData(const CAData_t *data)
{
    OIC_LOG(DEBUG, TAG, "IN");
    VERIFY_NON_NULL_VOID(data, TAG, "data");
    VERIFY_NON_NULL_VOID(data->remoteEndpoint, TAG, "remoteEndpoint");

    CAResult_t res = CA_STATUS_FAILED;

    CASendDataType_t type = data->type;

    if (SEND_TYPE_UNICAST == type)
    {
        coap_pdu_t *pdu = NULL;

        if (NULL != data->requestInfo)
        {
            OIC_LOG(DEBUG, TAG, "reqInfo avlbl");

            pdu = (coap_pdu_t *) CAGeneratePDU(data->remoteEndpoint->resourceUri,
                                               data->requestInfo->method,
                                               data->requestInfo->info);
        }
        else if (NULL != data->responseInfo)
        {
            OIC_LOG(DEBUG, TAG, "resInfo avlbl");

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
                OIC_LOG_V(INFO, TAG, "retransmissions will not be working: %d", res);
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
    VERIFY_NON_NULL_VOID(data, TAG, "data");

    uint32_t code = CA_NOT_FOUND;
    coap_pdu_t *pdu = (coap_pdu_t *) CAParsePDU((const char *) data, dataLen, &code);

    if (NULL == pdu)
    {
        OIC_LOG(ERROR, TAG, "Parse PDU failed");
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
            return;
        }

        CAResult_t res = CAGetRequestInfoFromPDU(pdu, ReqInfo, uri, bufLen);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG_V(ERROR, TAG, "CAGetRequestInfoFromPDU failed : %d", res);
            OICFree(ReqInfo);
            coap_delete_pdu(pdu);
            return;
        }

        if (NULL != ReqInfo->info.options)
        {
            for (uint32_t i = 0; i < ReqInfo->info.numOptions; i++)
            {
                OIC_LOG_V(DEBUG, TAG, "optionID: %d", ReqInfo->info.options[i].optionID);

                OIC_LOG_V(DEBUG, TAG, "list: %s", ReqInfo->info.options[i].optionData);
            }
        }

        if (NULL != ReqInfo->info.payload)
        {
            OIC_LOG_V(DEBUG, TAG, "Request- payload: %s", ReqInfo->info.payload);
        }

        OIC_LOG_V(DEBUG, TAG, "code: %d", ReqInfo->method);
        OIC_LOG(DEBUG, TAG, "token:");
        OIC_LOG_BUFFER(DEBUG, TAG, (const uint8_t *) ReqInfo->info.token, CA_MAX_TOKEN_LEN);
        if (NULL != endpoint)
        {
            endpoint->resourceUri = (char *) OICMalloc(bufLen + 1);
            if (NULL == endpoint->resourceUri)
            {
                OIC_LOG(ERROR, TAG, "CAReceivedPacketCallback, Memory allocation failed!");
                OICFree(ReqInfo);
                coap_delete_pdu(pdu);
                return;
            }
            memcpy(endpoint->resourceUri, uri, bufLen);
            endpoint->resourceUri[bufLen] = '\0';
            OIC_LOG_V(DEBUG, TAG, "URI : %s", endpoint->resourceUri);
        }

        if (ReqInfo)
        {
            if (g_requestHandler)
            {
                g_requestHandler(endpoint, ReqInfo);
            }

            CADestroyRequestInfoInternal(ReqInfo);
        }
    }
    else
    {
        CAResponseInfo_t *ResInfo = (CAResponseInfo_t *) OICCalloc(1, sizeof(CAResponseInfo_t));
        if (NULL == ResInfo)
        {
            OIC_LOG(ERROR, TAG, "CAReceivedPacketCallback, Memory allocation failed!");
            coap_delete_pdu(pdu);
            return;
        }

        CAResult_t res = CAGetResponseInfoFromPDU(pdu, ResInfo, uri, bufLen);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG_V(ERROR, TAG, "CAGetResponseInfoFromPDU failed : %d", res);
            OICFree(ResInfo);
            coap_delete_pdu(pdu);
            return;
        }

        if (NULL != ResInfo->info.options)
        {
            for (uint32_t i = 0; i < ResInfo->info.numOptions; i++)
            {
                OIC_LOG_V(DEBUG, TAG, "optionID: %d", ResInfo->info.options[i].optionID);

                OIC_LOG_V(DEBUG, TAG, "list: %s", ResInfo->info.options[i].optionData);
            }
        }

        if (NULL != ResInfo->info.payload)
        {
            OIC_LOG_V(DEBUG, TAG, "payload: %s", ResInfo->info.payload);
        }
        OIC_LOG_V(DEBUG, TAG, "code: %d", ResInfo->result);

        if (NULL != endpoint)
        {
            endpoint->resourceUri = (char *) OICMalloc(bufLen + 1);
            if (NULL == endpoint->resourceUri)
            {
                OIC_LOG(ERROR, TAG, "CAReceivedPacketCallback, Memory allocation failed !");
                OICFree(ResInfo);
                coap_delete_pdu(pdu);
                return;
            }
            memcpy(endpoint->resourceUri, uri, bufLen);
            endpoint->resourceUri[bufLen] = '\0';
            OIC_LOG_V(DEBUG, TAG, "URI : %s", endpoint->resourceUri);
        }

        // for retransmission
        void *retransmissionPdu = NULL;
        CARetransmissionReceivedData(&g_retransmissionContext, endpoint, pdu->hdr, pdu->length,
                                     &retransmissionPdu);

        // get token from saved data in retransmission list
        if (retransmissionPdu && CA_EMPTY == code)
        {
            CAResult_t res = CAGetTokenFromPDU((const coap_hdr_t *)retransmissionPdu,
                                               &(ResInfo->info));
            if(CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "fail to get Token from retransmission list");
                OICFree(ResInfo->info.token);
            }
        }
        OICFree(retransmissionPdu);

        if (NULL != ResInfo)
        {
            if (g_responseHandler)
            {
                g_responseHandler(endpoint, ResInfo);
            }
            CADestroyResponseInfoInternal(ResInfo);
        }
    }

    if (endpoint && endpoint->resourceUri)
    {
        OICFree(endpoint->resourceUri);
        endpoint->resourceUri = NULL;
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
    CAReadData();
    CARetransmissionBaseRoutine((void *)&g_retransmissionContext);
}

CAResult_t CADetachRequestMessage(const CARemoteEndpoint_t *object, const CARequestInfo_t *request)
{
    OIC_LOG(DEBUG, TAG, "IN");

    VERIFY_NON_NULL(object, TAG, "object");
    VERIFY_NON_NULL(request, TAG, "request");

    // If max retransmission queue is reached, then don't handle new request
    if (CA_MAX_RT_ARRAY_SIZE == u_arraylist_length(g_retransmissionContext.dataList))
    {
        OIC_LOG(ERROR, TAG, "max RT queue size reached!");
        return CA_SEND_FAILED;
    }

    // allocate & initialize
    CAData_t *data = (CAData_t *) OICCalloc(1, sizeof(CAData_t));
    CA_MEMORY_ALLOC_CHECK(data);

    // save data
    data->type = SEND_TYPE_UNICAST;
    data->remoteEndpoint = object;
    data->requestInfo = request;
    data->responseInfo = NULL;

    CAProcessData(data);
    OICFree(data);
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;

// memory error label.
memory_error_exit:
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

    // allocate & initialize
    CAData_t *data = (CAData_t *) OICCalloc(1, sizeof(CAData_t));
    CA_MEMORY_ALLOC_CHECK(data);

    CAAddress_t addr = {0};
    CARemoteEndpoint_t *remoteEndpoint = CACreateRemoteEndpointInternal(object->resourceUri, addr,
                                                                        object->transportType);

    // save data
    data->type = SEND_TYPE_MULTICAST;
    data->remoteEndpoint = remoteEndpoint;
    data->requestInfo = request;
    data->responseInfo = NULL;

    CAProcessData(data);
    CADestroyRemoteEndpointInternal(remoteEndpoint);

    OICFree(data);
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;

// memory error label.
memory_error_exit:

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

    // allocate & initialize
    CAData_t *data = (CAData_t *) OICCalloc(1, sizeof(CAData_t));
    CA_MEMORY_ALLOC_CHECK(data);

    // save data
    data->type = SEND_TYPE_UNICAST;
    data->remoteEndpoint = object;
    data->requestInfo = NULL;
    data->responseInfo = response;

    CAProcessData(data);

    OICFree(data);
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;

// memory error label.
memory_error_exit:
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

    // allocate & initialize
    CAData_t *data = (CAData_t *) OICCalloc(1, sizeof(CAData_t));
    CA_MEMORY_ALLOC_CHECK(data);

    CAAddress_t addr = {0};
    CARemoteEndpoint_t *remoteEndpoint =
            CACreateRemoteEndpointInternal(resourceUri, addr, CA_IPV4 | CA_EDR | CA_LE);

    // create request info
    CARequestInfo_t *reqInfo = (CARequestInfo_t *) OICCalloc(1, sizeof(CARequestInfo_t));
    CA_MEMORY_ALLOC_CHECK(reqInfo);

    // save request info data
    reqInfo->method = CA_GET;
    reqInfo->info.type = CA_MSG_NONCONFIRM;

    reqInfo->info.token = token;
    reqInfo->info.tokenLength = tokenLength;

    // save data
    data->type = SEND_TYPE_MULTICAST;
    data->remoteEndpoint = remoteEndpoint;
    data->requestInfo = reqInfo;

    data->responseInfo = NULL;
    data->options = NULL;
    data->numOptions = 0;
    CAHeaderOption_t *headerOption = NULL;
    if (NULL != options && numOptions > 0)
    {
        // copy data
        headerOption = (CAHeaderOption_t *) OICMalloc(sizeof(CAHeaderOption_t) * numOptions);
        CA_MEMORY_ALLOC_CHECK(headerOption);
        memcpy(headerOption, options, sizeof(CAHeaderOption_t) * numOptions);

        data->options = headerOption;
        data->numOptions = numOptions;
    }

    CAProcessData(data);

    CADestroyRemoteEndpoint(remoteEndpoint);
    OICFree(headerOption);
    OICFree(data);
    OICFree(reqInfo);
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;

// memory error label.
memory_error_exit:

    CADestroyRemoteEndpointInternal(remoteEndpoint);

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

    // retransmission initialize
    CARetransmissionInitialize(&g_retransmissionContext, CASendUnicastData,
                               CATimeoutCallback, NULL);

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
    CARetransmissionStop(&g_retransmissionContext);
    CARetransmissionDestroy(&g_retransmissionContext);

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


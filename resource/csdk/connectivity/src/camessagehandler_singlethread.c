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
#include "caprotocolmessage.h"
#include "logger.h"
#include "config.h" /* for coap protocol */
#include "coap.h"
#include "oic_malloc.h"
#include "caadapterutils.h"

#define TAG1 "CAMH_ST"

#define MEMORY_ALLOCK_CHECK(arg) { if (arg == NULL) {OIC_LOG_V(DEBUG, TAG1, "Out of memory"); goto memory_error_exit;} }

#define MAX_THREAD_POOL_SIZE    10

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

// handler field
static CARequestCallback gRequestHandler = NULL;
static CAResponseCallback gResponseHandler = NULL;

static void CAProcessData(CAData_t *data)
{
    OIC_LOG(DEBUG, TAG1, "IN");
    VERIFY_NON_NULL_VOID(data, TAG1, "data");
    VERIFY_NON_NULL_VOID(data->remoteEndpoint, TAG1, "remoteendpoint");

    CAResult_t res = CA_STATUS_FAILED;

    CASendDataType_t type = data->type;

    if (type == SEND_TYPE_UNICAST)
    {
        coap_pdu_t *pdu = NULL;

        if (data->requestInfo != NULL)
        {
            OIC_LOG_V(DEBUG, TAG1, "reqInfo avlbl");

            pdu = (coap_pdu_t *) CAGeneratePdu(data->remoteEndpoint->resourceUri,
                                               data->requestInfo->method, data->requestInfo->info);
        }
        else if (data->responseInfo != NULL)
        {
            OIC_LOG_V(DEBUG, TAG1, "resInfo avlbl");

            pdu = (coap_pdu_t *) CAGeneratePdu(data->remoteEndpoint->resourceUri,
                                               data->responseInfo->result, data->responseInfo->info);
        }
        else
        {
            OIC_LOG(DEBUG, TAG1, "request info, response info is empty");
        }

        // interface controller function call.
        if (NULL != pdu)
        {
            OIC_LOG_V(DEBUG, TAG1, "payload: %s", pdu->data);

            OIC_LOG_V(DEBUG, TAG1, "code: %d", pdu->hdr->code);

            OIC_LOG_V(DEBUG, TAG1, "buffer: %s", pdu->hdr);

            res = CASendUnicastData(data->remoteEndpoint, pdu->hdr, pdu->length);
        }

    }
    else if (type == SEND_TYPE_MULTICAST)
    {
        OIC_LOG(DEBUG, TAG1, "both requestInfo & responseInfo is not available");

        coap_pdu_t *pdu = NULL;
        CAInfo_t info;
        memset(&info, 0, sizeof(CAInfo_t));

        info.options = data->options;
        info.numOptions = data->numOptions;
        info.token = data->requestInfo->info.token;

        pdu = (coap_pdu_t *) CAGeneratePdu(data->remoteEndpoint->resourceUri, CA_GET, info);

        if (NULL != pdu)
        {
            OIC_LOG_V(DEBUG, TAG1, "PDU Maker - payload : %s", pdu->data);

            OIC_LOG_V(DEBUG, TAG1, "PDU Maker - type : %d", pdu->hdr->type);

            OIC_LOG_V(DEBUG, TAG1, "PDU Maker - code : %d", pdu->hdr->code);

            OIC_LOG_V(DEBUG, TAG1, "PDU Maker - id : %d", pdu->hdr->id);

            OIC_LOG_V(DEBUG, TAG1, "PDU Maker - token : %s", pdu->hdr->token);
            OIC_LOG_V(DEBUG, TAG1, "PDU Maker - buffer data : %s", pdu->hdr);

            res = CASendMulticastData(pdu->hdr, pdu->length);
        }
    }
    else
    {
        OIC_LOG(DEBUG, TAG1, "unknown type!");
    }

    if (gHandlerCallback != NULL)
    {
        gHandlerCallback("", res);
    }
    OIC_LOG(DEBUG, TAG1, "OUT");
}

static void CAReceivedPacketCallback(CARemoteEndpoint_t *endpoint, void *data, uint32_t dataLen)
{
    OIC_LOG(DEBUG, TAG1, "IN");
    VERIFY_NON_NULL_VOID(data, TAG1, "data");

    coap_pdu_t *pdu;
    uint32_t code = CA_NOT_FOUND;
    pdu = (coap_pdu_t *) CAParsePDU((const char *) data,dataLen, &code);
    //OICFree(data);

    char uri[CA_MAX_URI_LENGTH] = { 0, };

    if (CA_GET == code || CA_POST == code  || CA_PUT == code  || CA_DELETE == code )
    {
        CARequestInfo_t *ReqInfo;
        ReqInfo = (CARequestInfo_t *) OICMalloc(sizeof(CARequestInfo_t));
        VERIFY_NON_NULL_VOID(ReqInfo, TAG1, "reqInfo");
        memset(ReqInfo, 0, sizeof(CARequestInfo_t));
        CAGetRequestInfoFromPdu(pdu, ReqInfo, uri);

        if (NULL != ReqInfo->info.options)
        {
            uint32_t i;
            for (i = 0; i < ReqInfo->info.numOptions; i++)
            {
                OIC_LOG_V(DEBUG, TAG1, "Request- optionID: %d", ReqInfo->info.options[i].optionID);

                OIC_LOG_V(DEBUG, TAG1, "Request- list: %s", ReqInfo->info.options[i].optionData);
            }
        }

        if(NULL != ReqInfo->info.payload)
        {
            OIC_LOG_V(DEBUG, TAG1, "Request- payload: %s", ReqInfo->info.payload);
        }

        OIC_LOG_V(DEBUG, TAG1, "Request- code: %d", ReqInfo->method);
        OIC_LOG_V(DEBUG, TAG1, "Request- token : %s", ReqInfo->info.token);

        if (NULL != endpoint)
        {
            endpoint->resourceUri = (char *) OICMalloc(strlen(uri) + 1);
            memset(endpoint->resourceUri, 0, strlen(uri) + 1);
            memcpy(endpoint->resourceUri, uri, strlen(uri));
            OIC_LOG_V(DEBUG, TAG1, "URI : %s", endpoint->resourceUri);
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
        VERIFY_NON_NULL_VOID(ResInfo, TAG1, "ResInfo");
        memset(ResInfo, 0, sizeof(CAResponseInfo_t));
        CAGetResponseInfoFromPdu(pdu, ResInfo, uri);

        if (NULL != ResInfo->info.options)
        {
            uint32_t i;
            for (i = 0; i < ResInfo->info.numOptions; i++)
            {
                OIC_LOG_V(DEBUG, TAG1, "optionID: %d", ResInfo->info.options[i].optionID);

                OIC_LOG_V(DEBUG, TAG1, "list: %s", ResInfo->info.options[i].optionData);
            }

            if(NULL != ResInfo->info.payload)
            {
                OIC_LOG_V(DEBUG, TAG1, "payload: %s", ResInfo->info.payload);
            }

            OIC_LOG_V(DEBUG, TAG1, "code: %d", ResInfo->result);
        }

        if (NULL != endpoint)
        {
            endpoint->resourceUri = (char *) OICMalloc(strlen(uri) + 1);
            memset(endpoint->resourceUri, 0, strlen(uri) + 1);
            memcpy(endpoint->resourceUri, uri, strlen(uri));
            OIC_LOG_V(DEBUG, TAG1, "URI : %s", endpoint->resourceUri);
        }

        if (ResInfo != NULL)
        {
            if (gResponseHandler)
            {
                gResponseHandler(endpoint, ResInfo);
            }
            CADestroyResponseInfoInternal(ResInfo);
        }
    }

    if (endpoint && endpoint->resourceUri)
    {
        OICFree(endpoint->resourceUri);
    }
    OIC_LOG(DEBUG, TAG1, "OUT");
}

static void CANetworkChangedCallback(CALocalConnectivity_t *info, CANetworkStatus_t status)
{
    OIC_LOG(DEBUG, TAG1, "IN");
    OIC_LOG(DEBUG, TAG1, "OUT");
}

void CAHandleRequestResponseCallbacks()
{
    //OIC_LOG(DEBUG, TAG1, "IN");

    CAReadData();

    //OIC_LOG(DEBUG, TAG1, "OUT");
}

CAResult_t CADetachRequestMessage(const CARemoteEndpoint_t *object, const CARequestInfo_t *request)
{
    OIC_LOG(DEBUG, TAG1, "IN");

    VERIFY_NON_NULL(object, TAG1, "object");
    VERIFY_NON_NULL(request, TAG1, "request");

    CAData_t *data = (CAData_t *) OICMalloc(sizeof(CAData_t));
    MEMORY_ALLOCK_CHECK(data);

    // initialize
    memset(data, 0, sizeof(CAData_t));

    // clone remote endpoint
    CARemoteEndpoint_t *remoteEndpoint = CACloneRemoteEndpoint(object);
    MEMORY_ALLOCK_CHECK(remoteEndpoint);

    // clone request info
    CARequestInfo_t *requestInfo = CACloneRequestInfo(request);
    MEMORY_ALLOCK_CHECK(requestInfo);

    // save data
    data->type = SEND_TYPE_UNICAST;
    data->remoteEndpoint = remoteEndpoint;
    data->requestInfo = requestInfo;
    data->responseInfo = NULL;

    CAProcessData(data);
    CADestroyRemoteEndpoint(remoteEndpoint);
    CADestroyRequestInfoInternal(requestInfo);
    OICFree(data);
    OIC_LOG(DEBUG, TAG1, "OUT");
    return CA_STATUS_OK;

    // memory error label.
memory_error_exit:

    CADestroyRemoteEndpointInternal(remoteEndpoint);

    CADestroyRequestInfoInternal(requestInfo);

    if (data != NULL)
    {
        OICFree(data);
    }
    OIC_LOG(DEBUG, TAG1, "OUT");
    return CA_MEMORY_ALLOC_FAILED;
}

CAResult_t CADetachResponseMessage(const CARemoteEndpoint_t *object,
                                   const CAResponseInfo_t *response)
{
    OIC_LOG(DEBUG, TAG1, "IN");
    VERIFY_NON_NULL(object, TAG1, "object");
    VERIFY_NON_NULL(response, TAG1, "response");

    CAData_t *data = (CAData_t *) OICMalloc(sizeof(CAData_t));
    MEMORY_ALLOCK_CHECK(data);

    // initialize
    memset(data, 0, sizeof(CAData_t));

    // clone remote endpoint
    CARemoteEndpoint_t *remoteEndpoint = CACloneRemoteEndpoint(object);
    MEMORY_ALLOCK_CHECK(remoteEndpoint);

    // clone response info
    CAResponseInfo_t *responseInfo = CACloneResponseInfo(response);
    MEMORY_ALLOCK_CHECK(responseInfo);

    // save data
    data->type = SEND_TYPE_UNICAST;
    data->remoteEndpoint = remoteEndpoint;
    data->requestInfo = NULL;
    data->responseInfo = responseInfo;

    CAProcessData(data);
    CADestroyRemoteEndpoint(remoteEndpoint);
    CADestroyResponseInfoInternal(responseInfo);
    OICFree(data);
    OIC_LOG(DEBUG, TAG1, "OUT");
    return CA_STATUS_OK;

    // memory error label.
memory_error_exit:

    CADestroyRemoteEndpointInternal(remoteEndpoint);

    CADestroyResponseInfoInternal(responseInfo);

    if (data != NULL)
    {
        OICFree(data);
    }
    OIC_LOG(DEBUG, TAG1, "OUT");
    return CA_MEMORY_ALLOC_FAILED;
}

CAResult_t CADetachMessageResourceUri(const CAURI_t resourceUri, const CAToken_t token, const CAHeaderOption_t* options,
                                      uint8_t numOptions)
{
    OIC_LOG(DEBUG, TAG1, "IN");
    if (resourceUri == NULL)
    {
        return CA_STATUS_FAILED;
    }

    CAData_t *data = (CAData_t *) OICMalloc(sizeof(CAData_t));
    MEMORY_ALLOCK_CHECK(data);

    // initialize
    memset(data, 0, sizeof(CAData_t));

    CAAddress_t addr;
    memset(&addr, 0, sizeof(CAAddress_t));
    CARemoteEndpoint_t *remoteEndpoint = CACreateRemoteEndpointInternal(resourceUri, addr,
                                         CA_ETHERNET | CA_WIFI | CA_EDR | CA_LE);

    // save data
    data->type = SEND_TYPE_MULTICAST;
    data->remoteEndpoint = remoteEndpoint;
    CARequestInfo_t* ReqInfo = (CARequestInfo_t*) OICMalloc(sizeof(CARequestInfo_t));
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

        MEMORY_ALLOCK_CHECK(temp);

        memset(temp, 0, sizeof(CAHeaderOption_t) * numOptions);
        memcpy(temp, options, sizeof(CAHeaderOption_t) * numOptions);

        data->options = temp;
        data->numOptions = numOptions;
    }

    CAProcessData(data);
    CADestroyRemoteEndpoint(remoteEndpoint);
    OICFree(data);
    OICFree(ReqInfo);
    OIC_LOG(DEBUG, TAG1, "OUT");
    return CA_STATUS_OK;

    // memory error label.
memory_error_exit:

    CADestroyRemoteEndpointInternal(remoteEndpoint);

    if (data != NULL)
    {
        OICFree(data);
    }
    OIC_LOG(DEBUG, TAG1, "OUT");
    return CA_MEMORY_ALLOC_FAILED;
}

void CASetMessageHandlerCallback(CAMessageHandlerCallback callback)
{
    OIC_LOG(DEBUG, TAG1, "IN");
    gHandlerCallback = callback;
    OIC_LOG(DEBUG, TAG1, "OUT");
}

void CASetRequestResponseCallbacks(CARequestCallback ReqHandler, CAResponseCallback RespHandler)
{
    OIC_LOG(DEBUG, TAG1, "IN");
    gRequestHandler = ReqHandler;
    gResponseHandler = RespHandler;
    OIC_LOG(DEBUG, TAG1, "OUT");
}

CAResult_t CAInitializeMessageHandler()
{
    OIC_LOG(DEBUG, TAG1, "IN");
    CASetPacketReceivedCallback(CAReceivedPacketCallback);

    CASetNetworkChangeCallback(CANetworkChangedCallback);

    CAInitializeAdapters();
    OIC_LOG(DEBUG, TAG1, "OUT");
    return CA_STATUS_OK;
}

void CATerminateMessageHandler()
{
    OIC_LOG(DEBUG, TAG1, "IN");
    // terminate interface adapters by controller
    CATerminateAdapters();

    OIC_LOG(DEBUG, TAG1, "OUT");
}


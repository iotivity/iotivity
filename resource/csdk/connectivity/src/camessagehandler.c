/* *****************************************************************
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
#include <limits.h>

#include "cainterface.h"
#include "camessagehandler.h"
#include "caremotehandler.h"
#include "caprotocolmessage.h"
#include "experimental/logger.h"
#include "trace.h"
#ifndef WITH_UPSTREAM_LIBCOAP
#include "coap/config.h"
#endif
#include "oic_malloc.h"
#include "canetworkconfigurator.h"
#include "caadapterutils.h"
#include "cainterfacecontroller.h"
#include "caretransmission.h"
#include "oic_string.h"
#include "caping.h"

#ifdef WITH_BWT
#include "cablockwisetransfer.h"
#endif

#ifdef TCP_ADAPTER
#include "catcpadapter.h"
#endif

#include "uqueue.h"
#include "cathreadpool.h" /* for thread pool */
#include "caqueueingthread.h"

#if defined(TCP_ADAPTER) && defined(WITH_CLOUD)
#include "caconnectionmanager.h"
#endif
#define SINGLE_HANDLE
#define MAX_THREAD_POOL_SIZE    20

// thread pool handle
static ca_thread_pool_t g_threadPoolHandle = NULL;

// message handler main thread
static CAQueueingThread_t g_sendThread;
static CAQueueingThread_t g_receiveThread;


#define TAG "OIC_CA_MSG_HANDLE"

static CARetransmission_t g_retransmissionContext;

// handler field
static CARequestCallback g_requestHandler = NULL;
static CAResponseCallback g_responseHandler = NULL;
static CAErrorCallback g_errorHandler = NULL;
static CANetworkMonitorCallback g_nwMonitorHandler = NULL;

static void CAErrorHandler(const CAEndpoint_t *endpoint,
                           const void *data, size_t dataLen,
                           CAResult_t result);

static CAData_t* CAGenerateHandlerData(const CAEndpoint_t *endpoint,
                                       const CARemoteId_t *identity,
                                       const void *data, CADataType_t dataType);

static void CASendErrorInfo(const CAEndpoint_t *endpoint, const CAInfo_t *info,
                            CAResult_t result);

static void CADestroyData(void *data, uint32_t size);
static void CALogPayloadInfo(CAInfo_t *info);
static bool CADropSecondMessage(CAHistory_t *history, const CAEndpoint_t *endpoint, uint16_t id,
                                CAToken_t token, uint8_t tokenLength);

/**
 * print send / receive message of CoAP.
 * @param[in] data      CA information which has send/receive message and endpoint.
 * @param[in] pdu       CoAP pdu low data.
 */
static void CALogPDUInfo(const CAData_t *data, const coap_pdu_t *pdu);

#if defined(WITH_BWT) || defined(TCP_ADAPTER)
void CAAddDataToSendThread(CAData_t *data)
{
    VERIFY_NON_NULL_VOID(data, TAG, "data");

    // add thread
    CAQueueingThreadAddData(&g_sendThread, data, sizeof(CAData_t));
}

void CAAddDataToReceiveThread(CAData_t *data)
{
    VERIFY_NON_NULL_VOID(data, TAG, "data");

    // add thread
    CAQueueingThreadAddData(&g_receiveThread, data, sizeof(CAData_t));
}
#endif

static bool CAIsSelectedNetworkAvailable(void)
{
    u_arraylist_t *list = CAGetSelectedNetworkList();
    if (!list || u_arraylist_length(list) == 0)
    {
        OIC_LOG(ERROR, TAG, "No selected network");
        return false;
    }

    return true;
}

static CAData_t* CAGenerateHandlerData(const CAEndpoint_t *endpoint,
                                       const CARemoteId_t *identity,
                                       const void *data, CADataType_t dataType)
{
    OIC_LOG(DEBUG, TAG, "CAGenerateHandlerData IN");
    CAInfo_t *info = NULL;
    CAData_t *cadata = (CAData_t *) OICCalloc(1, sizeof(CAData_t));
    if (!cadata)
    {
        OIC_LOG(ERROR, TAG, "memory allocation failed");
        return NULL;
    }
    CAEndpoint_t* ep = CACloneEndpoint(endpoint);
    if (!ep)
    {
        OIC_LOG(ERROR, TAG, "endpoint clone failed");
        goto exit;
    }

    OIC_LOG_V(DEBUG, TAG, "address : %s", ep->addr);

    if (CA_RESPONSE_DATA == dataType)
    {
        CAResponseInfo_t* resInfo = (CAResponseInfo_t*)OICCalloc(1, sizeof(CAResponseInfo_t));
        if (!resInfo)
        {
            OIC_LOG(ERROR, TAG, "memory allocation failed");
            goto exit;
        }

        CAResult_t result = CAGetResponseInfoFromPDU(data, resInfo, endpoint);
        if (CA_STATUS_OK != result)
        {
            OIC_LOG(ERROR, TAG, "CAGetResponseInfoFromPDU Failed");
            CADestroyResponseInfoInternal(resInfo);
            goto exit;
        }
        cadata->responseInfo = resInfo;
        info = &resInfo->info;
        if (identity)
        {
            info->identity = *identity;
        }
        else
        {
            OIC_LOG_V(INFO, TAG, "%s: No identity information provided", __func__);
        }
        OIC_LOG(DEBUG, TAG, "Response Info :");
        CALogPayloadInfo(info);
    }
    else if (CA_REQUEST_DATA == dataType)
    {
        CARequestInfo_t* reqInfo = (CARequestInfo_t*)OICCalloc(1, sizeof(CARequestInfo_t));
        if (!reqInfo)
        {
            OIC_LOG(ERROR, TAG, "memory allocation failed");
            goto exit;
        }

        CAResult_t result = CAGetRequestInfoFromPDU(data, endpoint, reqInfo);
        if (CA_STATUS_OK != result)
        {
            OIC_LOG(ERROR, TAG, "CAGetRequestInfoFromPDU failed");
            CADestroyRequestInfoInternal(reqInfo);
            goto exit;
        }

        if ((reqInfo->info.type != CA_MSG_CONFIRM) &&
            CADropSecondMessage(&caglobals.ca.requestHistory, endpoint, reqInfo->info.messageId,
                                reqInfo->info.token, reqInfo->info.tokenLength))
        {
            OIC_LOG(INFO, TAG, "Second Request with same Token, Drop it");
            CADestroyRequestInfoInternal(reqInfo);
            goto exit;
        }

        cadata->requestInfo = reqInfo;
        info = &reqInfo->info;
        if (identity)
        {
            info->identity = *identity;
        }
        OIC_LOG(DEBUG, TAG, "Request Info :");
        CALogPayloadInfo(info);
   }
    else if (CA_ERROR_DATA == dataType)
    {
        CAErrorInfo_t *errorInfo = (CAErrorInfo_t *)OICCalloc(1, sizeof (CAErrorInfo_t));
        if (!errorInfo)
        {
            OIC_LOG(ERROR, TAG, "Memory allocation failed!");
            goto exit;
        }

        CAResult_t result = CAGetErrorInfoFromPDU(data, endpoint, errorInfo);
        if (CA_STATUS_OK != result)
        {
            OIC_LOG(ERROR, TAG, "CAGetErrorInfoFromPDU failed");
            OICFree(errorInfo);
            goto exit;
        }

        cadata->errorInfo = errorInfo;
        info = &errorInfo->info;
        if (identity)
        {
            info->identity = *identity;
        }
        OIC_LOG(DEBUG, TAG, "error Info :");
        CALogPayloadInfo(info);
    }
#ifdef TCP_ADAPTER
    else if (CA_SIGNALING_DATA == dataType)
    {
        CASignalingInfo_t *signalingInfo =
                (CASignalingInfo_t *)OICCalloc(1, sizeof (CASignalingInfo_t));
        if (!signalingInfo)
        {
            OIC_LOG(ERROR, TAG, "Memory allocation failed!");
            goto exit;
        }

        CAResult_t result = CAGetSignalingInfoFromPDU(data, endpoint, signalingInfo);
        if (CA_STATUS_OK != result)
        {
            OIC_LOG(ERROR, TAG, "CAGetSignalingInfoFromPDU failed");
            CADestroySignalingInfoInternal(signalingInfo);
            goto exit;
        }

        cadata->signalingInfo = signalingInfo;
        info = &signalingInfo->info;
        if (identity)
        {
            info->identity = *identity;
        }
        OIC_LOG(DEBUG, TAG, "Signaling Info :");
        CALogPayloadInfo(info);

        // Get CA_OPTION_MAX_MESSAGE_SIZE from pdu.
        unsigned char optionData[CA_MAX_HEADER_OPTION_DATA_LENGTH];
        size_t optionDataSize = sizeof(optionData);
        uint16_t receivedDataLength = 0;

        // TODO: We need to decide what options needs to be handled and which needs to be ignored.
        if(CAGetHeaderOption(info->options, info->numOptions, CA_OPTION_MAX_MESSAGE_SIZE,
                    optionData, optionDataSize, &receivedDataLength) == CA_STATUS_OK && receivedDataLength) {
            unsigned int maxMsgSize = (unsigned int) coap_decode_var_bytes(optionData, receivedDataLength);
            OIC_LOG_V(DEBUG, TAG, "received MAX_MESSAGE_SIZE option data: %u", maxMsgSize);
            if(maxMsgSize > 1152){
                //TODO: Change the TCP sockets parameters for maxMsgSize > 1152
            }
        }
    }
#endif

    cadata->remoteEndpoint = ep;
    cadata->dataType = dataType;

    OIC_LOG(DEBUG, TAG, "CAGenerateHandlerData OUT");
    return cadata;

exit:
    OICFree(cadata);
    CAFreeEndpoint(ep);
    return NULL;
}

static void CATimeoutCallback(const CAEndpoint_t *endpoint, const void *pdu, uint32_t size)
{
    VERIFY_NON_NULL_VOID(endpoint, TAG, "endpoint");
    VERIFY_NON_NULL_VOID(pdu, TAG, "pdu");
    CAEndpoint_t* ep = CACloneEndpoint(endpoint);
    if (!ep)
    {
        OIC_LOG(ERROR, TAG, "clone failed");
        return;
    }

    CAResponseInfo_t* resInfo = (CAResponseInfo_t*)OICCalloc(1, sizeof(CAResponseInfo_t));

    if (!resInfo)
    {
        OIC_LOG(ERROR, TAG, "calloc failed");
        CAFreeEndpoint(ep);
        return;
    }

    resInfo->result = CA_RETRANSMIT_TIMEOUT;
    resInfo->info.type = CAGetMessageTypeFromPduBinaryData(pdu, size);
    resInfo->info.messageId = CAGetMessageIdFromPduBinaryData(pdu, size);

    CAResult_t res = CAGetTokenFromPDU((const coap_hdr_transport_t *) pdu, &(resInfo->info),
                                       endpoint);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "fail to get Token from retransmission list");
        CADestroyResponseInfoInternal(resInfo);
        CAFreeEndpoint(ep);
        return;
    }

    CAData_t *cadata = (CAData_t *) OICCalloc(1, sizeof(CAData_t));
    if (NULL == cadata)
    {
        OIC_LOG(ERROR, TAG, "memory allocation failed !");
        CAFreeEndpoint(ep);
        CADestroyResponseInfoInternal(resInfo);
        return;
    }

    cadata->type = SEND_TYPE_UNICAST;
    cadata->remoteEndpoint = ep;
    cadata->requestInfo = NULL;
    cadata->responseInfo = resInfo;

#ifdef WITH_BWT
    if (CAIsSupportedBlockwiseTransfer(endpoint->adapter))
    {
        res = CARemoveBlockDataFromListWithSeed(resInfo->info.token, resInfo->info.tokenLength,
                                                endpoint->addr, endpoint->port);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, TAG, "CARemoveBlockDataFromListWithSeed failed");
        }
    }
#endif // WITH_BWT

    CAQueueingThreadAddData(&g_receiveThread, cadata, sizeof(CAData_t));
}

static void CADestroyData(void *data, uint32_t size)
{
    OIC_LOG(DEBUG, TAG, "CADestroyData IN");
    if ((size_t)size < sizeof(CAData_t))
    {
        OIC_LOG_V(ERROR, TAG, "Destroy data too small %p %d", data, size);
    }
    CAData_t *cadata = (CAData_t *) data;

    if (NULL == cadata)
    {
        OIC_LOG(ERROR, TAG, "cadata is NULL");
        return;
    }
    if (NULL != cadata->remoteEndpoint)
    {
        CAFreeEndpoint(cadata->remoteEndpoint);
    }

    if (NULL != cadata->requestInfo)
    {
        CADestroyRequestInfoInternal((CARequestInfo_t *) cadata->requestInfo);
    }

    if (NULL != cadata->responseInfo)
    {
        CADestroyResponseInfoInternal((CAResponseInfo_t *) cadata->responseInfo);
    }

    if (NULL != cadata->errorInfo)
    {
        CADestroyErrorInfoInternal(cadata->errorInfo);
    }

#ifdef TCP_ADAPTER
    if (NULL != cadata->signalingInfo)
    {
        CADestroySignalingInfoInternal(cadata->signalingInfo);
    }
#endif

    OICFree(cadata);
    OIC_LOG(DEBUG, TAG, "CADestroyData OUT");
}


static void CAReceiveThreadProcess(void *threadData)
{
#ifndef SINGLE_HANDLE
    CAData_t *data = (CAData_t *) threadData;
    OIC_TRACE_BEGIN(%s:CAProcessReceivedData, TAG);
    CAProcessReceivedData(data);
    OIC_TRACE_END();
#else
    (void)threadData;
#endif
}

static CAResult_t CAProcessMulticastData(const CAData_t *data)
{
    VERIFY_NON_NULL(data, TAG, "data");
    VERIFY_NON_NULL(data->remoteEndpoint, TAG, "remoteEndpoint");

    coap_pdu_t *pdu = NULL;
    CAInfo_t *info = NULL;
    coap_list_t *options = NULL;
    coap_transport_t transport = COAP_UDP;
    CAResult_t res = CA_SEND_FAILED;

    if (!data->requestInfo && !data->responseInfo)
    {
        OIC_LOG(ERROR, TAG, "request or response info is empty");
        return res;
    }

    if (data->requestInfo)
    {
        OIC_LOG(DEBUG, TAG, "requestInfo is available..");

        info = &data->requestInfo->info;
        pdu = CAGeneratePDU(CA_GET, info, data->remoteEndpoint, &options, &transport);
    }
    else if (data->responseInfo)
    {
        OIC_LOG(DEBUG, TAG, "responseInfo is available..");

        info = &data->responseInfo->info;
        pdu = CAGeneratePDU(data->responseInfo->result, info, data->remoteEndpoint,
                            &options, &transport);
    }

    if (!pdu)
    {
        OIC_LOG(ERROR,TAG,"Failed to generate multicast PDU");
        CASendErrorInfo(data->remoteEndpoint, info, CA_SEND_FAILED);
        coap_delete_list(options);
        return res;
    }

#ifdef WITH_BWT
    if (CAIsSupportedBlockwiseTransfer(data->remoteEndpoint->adapter))
    {
        // Blockwise transfer
        res = CAAddBlockOption(&pdu, info, data->remoteEndpoint, &options);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(DEBUG, TAG, "CAAddBlockOption has failed");
            goto exit;
        }
    }
#endif // WITH_BWT

    CALogPDUInfo(data, pdu);

    res = CASendMulticastData(data->remoteEndpoint, pdu->transport_hdr, pdu->length, data->dataType);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG_V(ERROR, TAG, "send failed:%d", res);
        goto exit;
    }

    coap_delete_list(options);
    coap_delete_pdu(pdu);
    return res;

exit:
    CAErrorHandler(data->remoteEndpoint, pdu->transport_hdr, pdu->length, res);
    coap_delete_list(options);
    coap_delete_pdu(pdu);
    return res;
}

static CAResult_t CAProcessSendData(const CAData_t *data)
{
    VERIFY_NON_NULL(data, TAG, "data");
    VERIFY_NON_NULL(data->remoteEndpoint, TAG, "remoteEndpoint");

    CAResult_t res = CA_STATUS_FAILED;

    CASendDataType_t type = data->type;

    coap_pdu_t *pdu = NULL;
    CAInfo_t *info = NULL;
    coap_list_t *options = NULL;
    coap_transport_t transport = COAP_UDP;

    if (SEND_TYPE_UNICAST == type)
    {
        OIC_LOG(DEBUG,TAG,"Unicast message");

#ifdef ROUTING_GATEWAY
        /*
         * When forwarding a packet, do not attempt retransmission as its the responsibility of
         * packet originator node
         */
        bool skipRetransmission = false;
#endif

        if (NULL != data->requestInfo)
        {
            OIC_LOG(DEBUG, TAG, "requestInfo is available..");

            info = &data->requestInfo->info;
#ifdef ROUTING_GATEWAY
            skipRetransmission = data->requestInfo->info.skipRetransmission;
#endif
            pdu = CAGeneratePDU(data->requestInfo->method, info, data->remoteEndpoint,
                                &options, &transport);
        }
        else if (NULL != data->responseInfo)
        {
            OIC_LOG(DEBUG, TAG, "responseInfo is available..");

            info = &data->responseInfo->info;
#ifdef ROUTING_GATEWAY
            skipRetransmission = data->responseInfo->info.skipRetransmission;
#endif
            pdu = CAGeneratePDU(data->responseInfo->result, info, data->remoteEndpoint,
                                &options, &transport);
        }
#ifdef TCP_ADAPTER
        else if (NULL != data->signalingInfo)
        {
            OIC_LOG(DEBUG, TAG, "signalingInfo is available..");
            info = &data->signalingInfo->info;
#ifdef ROUTING_GATEWAY
            skipRetransmission = data->signalingInfo->info.skipRetransmission;
#endif
            pdu = CAGeneratePDU(data->signalingInfo->code, info, data->remoteEndpoint,
                                &options, &transport);
        }
#endif
        else
        {
            OIC_LOG(DEBUG, TAG, "request info, response info is empty");
            return CA_STATUS_INVALID_PARAM;
        }

        // interface controller function call.
        if (NULL != pdu)
        {
#ifdef WITH_BWT
            if (CAIsSupportedBlockwiseTransfer(data->remoteEndpoint->adapter))
            {
                // Blockwise transfer
                if (NULL != info)
                {
                    res = CAAddBlockOption(&pdu, info, data->remoteEndpoint, &options);
                    if (CA_STATUS_OK != res)
                    {
                        OIC_LOG(INFO, TAG, "to write block option has failed");
                        CAErrorHandler(data->remoteEndpoint, pdu->transport_hdr, pdu->length, res);
                        coap_delete_list(options);
                        coap_delete_pdu(pdu);
                        return res;
                    }
                }
            }
#endif // WITH_BWT
            CALogPDUInfo(data, pdu);

            OIC_LOG_V(INFO, TAG, "CASendUnicastData type : %d", data->dataType);
            res = CASendUnicastData(data->remoteEndpoint, pdu->transport_hdr, pdu->length, data->dataType);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG_V(ERROR, TAG, "send failed:%d", res);
                CAErrorHandler(data->remoteEndpoint, pdu->transport_hdr, pdu->length, res);
                coap_delete_list(options);
                coap_delete_pdu(pdu);
                return res;
            }

#ifdef WITH_TCP
            if (CAIsSupportedCoAPOverTCP(data->remoteEndpoint->adapter))
            {
                OIC_LOG(INFO, TAG, "retransmission will be not worked");
            }
            else
#endif
#ifdef ROUTING_GATEWAY
            if (!skipRetransmission)
#endif
            {
                // for retransmission
                res = CARetransmissionSentData(&g_retransmissionContext,
                                               data->remoteEndpoint,
                                               data->dataType,
                                               pdu->transport_hdr, pdu->length);
                if ((CA_STATUS_OK != res) && (CA_NOT_SUPPORTED != res))
                {
                    //when retransmission not supported this will return CA_NOT_SUPPORTED, ignore
                    OIC_LOG_V(INFO, TAG, "retransmission is not enabled due to error, res : %d", res);
                    coap_delete_list(options);
                    coap_delete_pdu(pdu);
                    return res;
                }
            }

            coap_delete_list(options);
            coap_delete_pdu(pdu);
        }
        else
        {
            OIC_LOG(ERROR,TAG,"Failed to generate unicast PDU");
            CASendErrorInfo(data->remoteEndpoint, info, CA_SEND_FAILED);
            return CA_SEND_FAILED;
        }
    }
    else if (SEND_TYPE_MULTICAST == type)
    {
        OIC_LOG(DEBUG,TAG,"Multicast message");
#ifdef WITH_TCP
        /*
         * If CoAP over TCP is enabled, the CoAP pdu wont be same for IP and other adapters.
         * That's why we need to generate two pdu's, one for IP and second for other transports.
         * Two possible cases we might have to split: a) when adapter is CA_DEFAULT_ADAPTER
         * b) when one of the adapter is IP adapter(ex: CA_ADAPTER_IP | CA_ADAPTER_GATT_BTLE)
         */
        if (data->remoteEndpoint->adapter == CA_DEFAULT_ADAPTER ||
                (CA_ADAPTER_IP & data->remoteEndpoint->adapter &&
                    CA_ADAPTER_IP != data->remoteEndpoint->adapter))
        {
            if (data->remoteEndpoint->adapter == CA_DEFAULT_ADAPTER)
            {
                data->remoteEndpoint->adapter = CA_ALL_ADAPTERS ^ CA_ADAPTER_IP;
            }
            else
            {
                data->remoteEndpoint->adapter = data->remoteEndpoint->adapter ^ CA_ADAPTER_IP;
            }
            CAProcessMulticastData(data);
            data->remoteEndpoint->adapter = CA_ADAPTER_IP;
            CAProcessMulticastData(data);
        }
        else
        {
            CAProcessMulticastData(data);
        }
#else
        CAProcessMulticastData(data);
#endif
    }

    return CA_STATUS_OK;
}

static void CASendThreadProcess(void *threadData)
{
    CAData_t *data = (CAData_t *) threadData;
    OIC_TRACE_BEGIN(%s:CAProcessSendData, TAG);
    CAProcessSendData(data);
    OIC_TRACE_END();
}

/*
 * If a second message arrives with the same message ID, token and the other address
 * family, drop it.  Typically, IPv6 beats IPv4, so the IPv4 message is dropped.
 */
static bool CADropSecondMessage(CAHistory_t *history, const CAEndpoint_t *ep, uint16_t id,
                                CAToken_t token, uint8_t tokenLength)
{
    if (!ep)
    {
        return true;
    }
    if (ep->adapter != CA_ADAPTER_IP)
    {
        return false;
    }

    if (tokenLength > CA_MAX_TOKEN_LEN)
    {
        /*
         * If token length is more than CA_MAX_TOKEN_LEN,
         * we compare the first CA_MAX_TOKEN_LEN bytes only.
         */
        tokenLength = CA_MAX_TOKEN_LEN;
    }

    bool ret = false;
    CATransportFlags_t familyFlags = ep->flags & CA_IPFAMILY_MASK;

    for (size_t i = 0; i < sizeof(history->items) / sizeof(history->items[0]); i++)
    {
        CAHistoryItem_t *item = &(history->items[i]);
        if (id == item->messageId && tokenLength == item->tokenLength
            && ep->ifindex == item->ifindex && memcmp(item->token, token, tokenLength) == 0)
        {
            OIC_LOG_V(INFO, TAG, "IPv%c duplicate message ignored",
                      familyFlags & CA_IPV6 ? '6' : '4');
            ret = true;
            break;
        }
    }

    history->items[history->nextIndex].flags = familyFlags;
    history->items[history->nextIndex].messageId = id;
    history->items[history->nextIndex].ifindex = ep->ifindex;
    if (token && tokenLength)
    {
        memcpy(history->items[history->nextIndex].token, token, tokenLength);
        history->items[history->nextIndex].tokenLength = tokenLength;
    }

    if (++history->nextIndex >= HISTORYSIZE)
    {
        history->nextIndex = 0;
    }

    return ret;
}

static void CAReceivedPacketCallback(const CASecureEndpoint_t *sep,
                                     const void *data, size_t dataLen)
{
    VERIFY_NON_NULL_VOID(sep, TAG, "remoteEndpoint");
    VERIFY_NON_NULL_VOID(data, TAG, "data");
    OIC_TRACE_BEGIN(%s:CAReceivedPacketCallback, TAG);

    if (0 == dataLen)
    {
        OIC_LOG(ERROR, TAG, "dataLen is zero");
        OIC_TRACE_END();
        return;
    }

    uint32_t code = CA_NOT_FOUND;
    CAData_t *cadata = NULL;

    coap_pdu_t *pdu = (coap_pdu_t *) CAParsePDU((const char *) data, dataLen, &code,
                                                &(sep->endpoint));
    if (NULL == pdu)
    {
        OIC_LOG(ERROR, TAG, "Parse PDU failed");
        goto exit;
    }

    OIC_LOG_V(DEBUG, TAG, "code = %d", code);

#ifdef TCP_ADAPTER
    if (CA_ADAPTER_TCP == sep->endpoint.adapter && CA_CSM != code)
    {
        CACSMExchangeState_t csmState = CAGetCSMState(&sep->endpoint);
        if (csmState != RECEIVED && csmState != SENT_RECEIVED)
        {
            CATCPDisconnectSession(&sep->endpoint);
            OIC_LOG(ERROR, TAG, "CAReceivedPacketCallback, first message is not a CSM!!");
        }
    }
#endif

    if (CA_GET == code || CA_POST == code || CA_PUT == code || CA_DELETE == code)
    {
        cadata = CAGenerateHandlerData(&(sep->endpoint), &(sep->identity), pdu, CA_REQUEST_DATA);
        if (!cadata)
        {
            OIC_LOG(ERROR, TAG, "CAReceivedPacketCallback, CAGenerateHandlerData failed!");
            coap_delete_pdu(pdu);
            goto exit;
        }
    }
    else
    {
#ifdef TCP_ADAPTER
        // This is signaling message.
        if (CA_ADAPTER_TCP == sep->endpoint.adapter &&
                (CA_CSM == code || CA_PING == code || CA_PONG == code
                        || CA_RELEASE == code || CA_ABORT == code))
        {
            cadata = CAGenerateHandlerData(&(sep->endpoint), &(sep->identity),
                                           pdu, CA_SIGNALING_DATA);
            if (!cadata)
            {
                OIC_LOG(ERROR, TAG, "CAReceivedPacketCallback, CAGenerateHandlerData failed!");
                coap_delete_pdu(pdu);
                return;
            }

            // Received signaling message is CSM.
            if (CA_CSM == code)
            {
                OIC_LOG_V(DEBUG, TAG, "CAReceivedPacketCallback, CSM received");
                // update session info of tcp_adapter.
                // TODO check if it is a valid CSM, if it is not valid message disconnect the session.
                CACSMExchangeState_t csmState = CAGetCSMState(&sep->endpoint);
                if (csmState == NONE)
                {
                    CAUpdateCSMState(&sep->endpoint,RECEIVED);
                }
                else if (csmState == SENT)
                {
                    CAUpdateCSMState(&sep->endpoint,SENT_RECEIVED);
                }
            }
            else if (CA_PING == code)
            {
                CASendPongMessage(cadata->remoteEndpoint, false, cadata->signalingInfo->info.token,
                                  cadata->signalingInfo->info.tokenLength);
            }
            else if(CA_PONG == code)
            {
                CAPongReceivedCallback(cadata->remoteEndpoint, cadata->signalingInfo->info.token,
                                       cadata->signalingInfo->info.tokenLength);
            }
            return;
        }
#endif

        cadata = CAGenerateHandlerData(&(sep->endpoint), &(sep->identity), pdu, CA_RESPONSE_DATA);
        if (!cadata)
        {
            OIC_LOG(ERROR, TAG, "CAReceivedPacketCallback, CAGenerateHandlerData failed!");
            coap_delete_pdu(pdu);
            goto exit;
        }

#ifdef WITH_TCP
        if (CAIsSupportedCoAPOverTCP(sep->endpoint.adapter))
        {
            OIC_LOG(INFO, TAG, "retransmission is not supported");
        }
        else
#endif
        {
            // for retransmission
            void *retransmissionPdu = NULL;
            CARetransmissionReceivedData(&g_retransmissionContext, cadata->remoteEndpoint, pdu->transport_hdr,
                                         pdu->length, &retransmissionPdu);

            // get token from saved data in retransmission list
            if (retransmissionPdu && CA_EMPTY == code)
            {
                if (cadata->responseInfo)
                {
                    CAInfo_t *info = &cadata->responseInfo->info;
                    CAResult_t res = CAGetTokenFromPDU((const coap_hdr_transport_t *)retransmissionPdu,
                                                       info, &(sep->endpoint));
                    if (CA_STATUS_OK != res)
                    {
                        OIC_LOG(ERROR, TAG, "fail to get Token from retransmission list");
                        OICFree(info->token);
                        info->tokenLength = 0;
                    }
                }
            }
            OICFree(retransmissionPdu);
        }
    }

    cadata->type = SEND_TYPE_UNICAST;

    CALogPDUInfo(cadata, pdu);

#ifdef WITH_BWT
    if (CAIsSupportedBlockwiseTransfer(sep->endpoint.adapter))
    {
        CAResult_t res = CAReceiveBlockWiseData(pdu, &(sep->endpoint), cadata, dataLen);
        if (CA_NOT_SUPPORTED == res || CA_REQUEST_TIMEOUT == res)
        {
            OIC_LOG(DEBUG, TAG, "this message does not have block option");
            CAQueueingThreadAddData(&g_receiveThread, cadata, sizeof(CAData_t));
        }
        else
        {
            CADestroyData(cadata, sizeof(CAData_t));
        }
    }
    else
#endif
    {
        CAQueueingThreadAddData(&g_receiveThread, cadata, sizeof(CAData_t));
    }

    coap_delete_pdu(pdu);

exit:
    OIC_LOG(DEBUG, TAG, "received pdu data :");
    OIC_LOG_BUFFER(DEBUG, TAG,  data, dataLen);

    OIC_TRACE_END();
}

void CAHandleRequestResponseCallbacks(void)
{
#ifdef SINGLE_HANDLE
    // parse the data and call the callbacks.
    // #1 parse the data
    // #2 get endpoint

    oc_mutex_lock(g_receiveThread.threadMutex);

    u_queue_message_t *item = u_queue_get_element(g_receiveThread.dataQueue);

    oc_mutex_unlock(g_receiveThread.threadMutex);

    if (NULL == item || NULL == item->msg)
    {
        return;
    }

    // get endpoint
    CAData_t *td = (CAData_t *) item->msg;

    if (td->requestInfo && g_requestHandler)
    {
        OIC_LOG_V(DEBUG, TAG, "request callback : %d", td->requestInfo->info.numOptions);
        g_requestHandler(td->remoteEndpoint, td->requestInfo);
    }
    else if (td->responseInfo && g_responseHandler)
    {
        OIC_LOG_V(DEBUG, TAG, "response callback : %d", td->responseInfo->info.numOptions);
        g_responseHandler(td->remoteEndpoint, td->responseInfo);
    }
    else if (td->errorInfo && g_errorHandler)
    {
        OIC_LOG_V(DEBUG, TAG, "error callback error: %d", td->errorInfo->result);
        g_errorHandler(td->remoteEndpoint, td->errorInfo);
    }

    CADestroyData(item->msg, sizeof(CAData_t));
    OICFree(item);

#endif // SINGLE_HANDLE
}

static CAData_t* CAPrepareSendData(const CAEndpoint_t *endpoint, const void *sendData,
                                   CADataType_t dataType)
{
    OIC_LOG(DEBUG, TAG, "CAPrepareSendData IN");

    CAData_t *cadata = (CAData_t *) OICCalloc(1, sizeof(CAData_t));
    if (!cadata)
    {
        OIC_LOG(ERROR, TAG, "memory allocation failed");
        return NULL;
    }

    if (CA_REQUEST_DATA == dataType)
    {
        // clone request info
        CARequestInfo_t *request = CACloneRequestInfo((CARequestInfo_t *)sendData);
        if (!request)
        {
            OIC_LOG(ERROR, TAG, "CACloneRequestInfo failed");
            goto exit;
        }
        cadata->type = request->isMulticast ? SEND_TYPE_MULTICAST : SEND_TYPE_UNICAST;
        cadata->requestInfo =  request;
    }
    else if (CA_RESPONSE_DATA == dataType || CA_RESPONSE_FOR_RES == dataType)
    {
        // clone response info
        CAResponseInfo_t *response = CACloneResponseInfo((CAResponseInfo_t *)sendData);
        if(!response)
        {
            OIC_LOG(ERROR, TAG, "CACloneResponseInfo failed");
            goto exit;
        }
        cadata->type = response->isMulticast ? SEND_TYPE_MULTICAST : SEND_TYPE_UNICAST;
        cadata->responseInfo = response;
    }
#ifdef TCP_ADAPTER
    else if (CA_SIGNALING_DATA == dataType)
    {
        // clone signaling info
        CASignalingInfo_t *signaling = CACloneSignalingInfo((CASignalingInfo_t *) sendData);
        if (!signaling)
        {
            OIC_LOG(ERROR, TAG, "CACloneSignalingInfo failed");
            goto exit;
        }
        cadata->type = SEND_TYPE_UNICAST;
        cadata->signalingInfo = signaling;
    }
#endif
    else
    {
        OIC_LOG(ERROR, TAG, "CAPrepareSendData unknown data type");
        goto exit;
    }

    CAEndpoint_t* ep = CACloneEndpoint(endpoint);
    if (!ep)
    {
        OIC_LOG(ERROR, TAG, "endpoint clone failed");
        goto exit;
    }
    cadata->remoteEndpoint = ep;
    cadata->dataType = dataType;
    return cadata;

exit:
    CADestroyData(cadata, sizeof(CAData_t));
    return NULL;
}

CAResult_t CADetachSendMessage(const CAEndpoint_t *endpoint, const void *sendMsg,
                               CADataType_t dataType)
{
    VERIFY_NON_NULL(endpoint, TAG, "endpoint");
    VERIFY_NON_NULL(sendMsg, TAG, "sendMsg");

    if (false == CAIsSelectedNetworkAvailable())
    {
        return CA_STATUS_FAILED;
    }

    CAData_t *data = NULL;
#ifdef TCP_ADAPTER
    if (CA_ADAPTER_TCP == endpoint->adapter)
    {
        // #1. Try to find session info from tcp_adapter.
        CACSMExchangeState_t CSMstate = CAGetCSMState(endpoint);
        if (CSMstate != SENT && CSMstate != SENT_RECEIVED)
        {
            // #2. Generate CSM message
            OIC_LOG_V(DEBUG, TAG, "Generate CSM message for [%s:%d]",
                      endpoint->addr, endpoint->port);

            // TODO: We need to decide what options needs to be sent Initially?
            //       Right now we're sending CA_OPTION_MAX_MESSAGE_SIZE as 1152,
            //       which is default according to the RFC.
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
                return CA_STATUS_FAILED;
            }
            OICFree(csmOpts);

            // #3. Add pdu to send queue.
            CAQueueingThreadAddData(&g_sendThread, data, sizeof(CAData_t));
        }
    }
#endif

    data = CAPrepareSendData(endpoint, sendMsg, dataType);
    if(!data)
    {
        OIC_LOG(ERROR, TAG, "CAPrepareSendData failed");
        return CA_MEMORY_ALLOC_FAILED;
    }

    OIC_LOG_V(DEBUG, TAG, "device ID of endpoint of this message is %s", endpoint->remoteId);

#if defined(TCP_ADAPTER) && defined(WITH_CLOUD)
    CAResult_t ret = CACMGetMessageData(data);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(DEBUG, TAG, "Ignore ConnectionManager");
    }
#endif

    if (SEND_TYPE_UNICAST == data->type && CAIsLocalEndpoint(data->remoteEndpoint))
    {
        OIC_LOG(DEBUG, TAG,
                "This is a loopback message. Transfer it to the receive queue directly");
        CAQueueingThreadAddData(&g_receiveThread, data, sizeof(CAData_t));
        return CA_STATUS_OK;
    }
#ifdef WITH_BWT
    if (CAIsSupportedBlockwiseTransfer(endpoint->adapter))
    {
        // send block data
        CAResult_t res = CASendBlockWiseData(data);
        if (CA_NOT_SUPPORTED == res)
        {
            OIC_LOG(DEBUG, TAG, "normal msg will be sent");
            CAQueueingThreadAddData(&g_sendThread, data, sizeof(CAData_t));
            return CA_STATUS_OK;
        }
        else
        {
            CADestroyData(data, sizeof(CAData_t));
        }
        return res;
    }
    else
#endif // WITH_BWT
    {
        CAQueueingThreadAddData(&g_sendThread, data, sizeof(CAData_t));
    }

    return CA_STATUS_OK;
}

void CASetInterfaceCallbacks(CARequestCallback ReqHandler, CAResponseCallback RespHandler,
                             CAErrorCallback errorHandler)
{
    g_requestHandler = ReqHandler;
    g_responseHandler = RespHandler;
    g_errorHandler = errorHandler;
}

void CASetNetworkMonitorCallback(CANetworkMonitorCallback nwMonitorHandler)
{
    g_nwMonitorHandler = nwMonitorHandler;
}

CAResult_t CAInitializeMessageHandler(CATransportAdapter_t transportType)
{
    CASetPacketReceivedCallback(CAReceivedPacketCallback);
    CASetErrorHandleCallback(CAErrorHandler);

    // create thread pool
    CAResult_t res = ca_thread_pool_init(MAX_THREAD_POOL_SIZE, &g_threadPoolHandle);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "thread pool initialize error.");
        return res;
    }

    // send thread initialize
    res = CAQueueingThreadInitialize(&g_sendThread, g_threadPoolHandle,
                                     CASendThreadProcess, CADestroyData);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "Failed to Initialize send queue thread");
        return res;
    }

    // start send thread
    res = CAQueueingThreadStart(&g_sendThread);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "thread start error(send thread).");
        return res;
    }

    // receive thread initialize
    res = CAQueueingThreadInitialize(&g_receiveThread, g_threadPoolHandle,
                                     CAReceiveThreadProcess, CADestroyData);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "Failed to Initialize receive queue thread");
        return res;
    }

#ifndef SINGLE_HANDLE // This will be enabled when RI supports multi threading
    // start receive thread
    res = CAQueueingThreadStart(&g_receiveThread);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "thread start error(receive thread).");
        return res;
    }
#endif // SINGLE_HANDLE

    // retransmission initialize
    res = CARetransmissionInitialize(&g_retransmissionContext, g_threadPoolHandle,
                                     CASendUnicastData, CATimeoutCallback, NULL);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "Failed to Initialize Retransmission.");
        return res;
    }

#ifdef WITH_BWT
    // block-wise transfer initialize
    res = CAInitializeBlockWiseTransfer(CAAddDataToSendThread, CAAddDataToReceiveThread);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "Failed to Initialize BlockWiseTransfer.");
        return res;
    }
#endif

    // start retransmission
    res = CARetransmissionStart(&g_retransmissionContext);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "thread start error(retransmission thread).");
        return res;
    }

    // initialize interface adapters by controller
    res = CAInitializeAdapters(g_threadPoolHandle, transportType);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "Failed to Initialize Adapters.");
        return res;
    }

    return CA_STATUS_OK;
}

void CATerminateMessageHandler(void)
{
    // stop adapters
    CAStopAdapters();

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
        CAQueueingThreadStop(&g_receiveThread);
#endif
    }

    // destroy thread pool
    if (NULL != g_threadPoolHandle)
    {
        ca_thread_pool_free(g_threadPoolHandle);
        g_threadPoolHandle = NULL;
    }

#ifdef WITH_BWT
    CATerminateBlockWiseTransfer();
#endif
    CARetransmissionDestroy(&g_retransmissionContext);
    CAQueueingThreadDestroy(&g_sendThread);
    CAQueueingThreadDestroy(&g_receiveThread);

    // terminate interface adapters by controller
    CATerminateAdapters();
}

static void CALogPayloadInfo(CAInfo_t *info)
{
    if (info)
    {
        if (info->options)
        {
            for (uint32_t i = 0; i < info->numOptions; i++)
            {
                OIC_LOG_V(DEBUG, TAG, "optionID: %u", info->options[i].optionID);

                OIC_LOG_V(DEBUG, TAG, "list: %s", info->options[i].optionData);
            }
        }

        if (info->payload)
        {
            OIC_LOG_V(DEBUG, TAG, "payload: %p(%" PRIuPTR ")", info->payload,
                      info->payloadSize);
        }

        if (info->token)
        {
            OIC_LOG(DEBUG, TAG, "token:");
            OIC_LOG_BUFFER(DEBUG, TAG, (const uint8_t *) info->token,
                           info->tokenLength);
        }
        OIC_LOG_V(DEBUG, TAG, "msgID: %u", info->messageId);
    }
    else
    {
        OIC_LOG(DEBUG, TAG, "info is NULL, cannot output log data");
    }
}

void CAErrorHandler(const CAEndpoint_t *endpoint,
                    const void *data, size_t dataLen,
                    CAResult_t result)
{
    OIC_LOG(DEBUG, TAG, "CAErrorHandler IN");
    VERIFY_NON_NULL_VOID(endpoint, TAG, "remoteEndpoint");
    VERIFY_NON_NULL_VOID(data, TAG, "data");

    if (0 == dataLen)
    {
        OIC_LOG(ERROR, TAG, "dataLen is zero");
        return;
    }

    uint32_t code = CA_NOT_FOUND;
    //Do not free remoteEndpoint and data. Currently they will be freed in data thread
    //Get PDU data
    coap_pdu_t *pdu = (coap_pdu_t *)CAParsePDU((const char *)data, dataLen, &code, endpoint);
    if (NULL == pdu)
    {
        OIC_LOG(ERROR, TAG, "Parse PDU failed");
        return;
    }

    CAData_t *cadata = CAGenerateHandlerData(endpoint, NULL, pdu, CA_ERROR_DATA);
    if (!cadata)
    {
        OIC_LOG(ERROR, TAG, "CAErrorHandler, CAGenerateHandlerData failed!");
        coap_delete_pdu(pdu);
        return;
    }

#ifdef WITH_TCP
    if (CAIsSupportedCoAPOverTCP(endpoint->adapter))
    {
        OIC_LOG(INFO, TAG, "retransmission is not supported");
    }
    else
#endif
    {
        //Fix up CoAP message to adjust it to current retransmission implementation
        coap_hdr_t *hdr = (coap_hdr_t *)(pdu->transport_hdr);
        hdr->type = CA_MSG_RESET;
        hdr->code = CA_EMPTY;

        // for retransmission
        void *retransmissionPdu = NULL;
        CARetransmissionReceivedData(&g_retransmissionContext, cadata->remoteEndpoint,
                                     pdu->transport_hdr, pdu->length, &retransmissionPdu);

        // get token from saved data in retransmission list
        if (retransmissionPdu && cadata->errorInfo)
        {
            CAInfo_t *info = &cadata->errorInfo->info;
            CAResult_t res = CAGetTokenFromPDU((const coap_hdr_transport_t *)retransmissionPdu,
                                               info, endpoint);
            if (CA_STATUS_OK != res)
            {
                OIC_LOG(ERROR, TAG, "fail to get Token from retransmission list");
                OICFree(info->token);
                info->tokenLength = 0;
            }
        }
        OICFree(retransmissionPdu);
    }

    cadata->errorInfo->result = result;

    CAQueueingThreadAddData(&g_receiveThread, cadata, sizeof(CAData_t));
    coap_delete_pdu(pdu);

    OIC_LOG(DEBUG, TAG, "CAErrorHandler OUT");
    return;
}

static void CASendErrorInfo(const CAEndpoint_t *endpoint, const CAInfo_t *info, CAResult_t result)
{
    OIC_LOG(DEBUG, TAG, "CASendErrorInfo IN");
    CAData_t *cadata = (CAData_t *) OICCalloc(1, sizeof(CAData_t));
    if (!cadata)
    {
        OIC_LOG(ERROR, TAG, "cadata memory allocation failed");
        return;
    }

    CAEndpoint_t* ep = CACloneEndpoint(endpoint);
    if (!ep)
    {
        OIC_LOG(ERROR, TAG, "endpoint clone failed");
        OICFree(cadata);
        return;
    }

    CAErrorInfo_t *errorInfo = (CAErrorInfo_t *)OICCalloc(1, sizeof (CAErrorInfo_t));
    if (!errorInfo)
    {
        OIC_LOG(ERROR, TAG, "errorInfo memory allocation failed");
        OICFree(cadata);
        CAFreeEndpoint(ep);
        return;
    }

    CAResult_t res = CACloneInfo(info, &errorInfo->info);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "info clone failed");
        OICFree(cadata);
        OICFree(errorInfo);
        CAFreeEndpoint(ep);
        return;
    }

    errorInfo->result = result;
    cadata->remoteEndpoint = ep;
    cadata->errorInfo = errorInfo;
    cadata->dataType = CA_ERROR_DATA;

    CAQueueingThreadAddData(&g_receiveThread, cadata, sizeof(CAData_t));
    OIC_LOG(DEBUG, TAG, "CASendErrorInfo OUT");
}

static void CALogPDUInfo(const CAData_t *data, const coap_pdu_t *pdu)
{
    OIC_LOG(DEBUG, TAG, "CALogPDUInfo");

    VERIFY_NON_NULL_VOID(data, TAG, "data");
    VERIFY_NON_NULL_VOID(pdu, TAG, "pdu");
    OIC_TRACE_BEGIN(%s:CALogPDUInfo, TAG);

    OIC_LOG(DEBUG, ANALYZER_TAG, "=================================================");
    if(SEND_TYPE_MULTICAST == data->type)
    {
        OIC_LOG(DEBUG, ANALYZER_TAG, "Is Multicast = true");
    }
    else
    {
        OIC_LOG(DEBUG, ANALYZER_TAG, "Is Multicast = false");
    }

    if (NULL != data->remoteEndpoint)
    {
        CALogAdapterTypeInfo(data->remoteEndpoint->adapter);
        OIC_LOG_V(DEBUG, ANALYZER_TAG, "Address = [%s]:[%d]", data->remoteEndpoint->addr,
                  data->remoteEndpoint->port);
    }

    switch(data->dataType)
    {
        case CA_REQUEST_DATA:
            OIC_LOG(DEBUG, ANALYZER_TAG, "Data Type = [CA_REQUEST_DATA]");
            break;
        case CA_RESPONSE_DATA:
            OIC_LOG(DEBUG, ANALYZER_TAG, "Data Type = [CA_RESPONSE_DATA]");
            break;
        case CA_ERROR_DATA:
            OIC_LOG(DEBUG, ANALYZER_TAG, "Data Type = [CA_ERROR_DATA]");
            break;
        case CA_RESPONSE_FOR_RES:
            OIC_LOG(DEBUG, ANALYZER_TAG, "Data Type = [CA_RESPONSE_FOR_RES]");
            break;
        default:
            OIC_LOG_V(DEBUG, ANALYZER_TAG, "Data Type = [%d]", data->dataType);
            break;
    }
    OIC_LOG(DEBUG, ANALYZER_TAG, "-------------------------------------------------");

    const CAInfo_t *info = NULL;
    if (NULL != data->requestInfo)
    {
        switch(data->requestInfo->method)
        {
            case CA_GET:
                OIC_LOG(DEBUG, ANALYZER_TAG, "Method = [GET]");
                break;
            case CA_POST:
                OIC_LOG(DEBUG, ANALYZER_TAG, "Method = [POST]");
                break;
            case CA_PUT:
                OIC_LOG(DEBUG, ANALYZER_TAG, "Method = [PUT]");
                break;
            case CA_DELETE:
                OIC_LOG(DEBUG, ANALYZER_TAG, "Method = [DELETE]");
                break;
            default:
                OIC_LOG_V(DEBUG, ANALYZER_TAG, "Method = [%d]", data->requestInfo->method);
                break;
        }
        info = &data->requestInfo->info;
    }

    if (NULL != data->responseInfo)
    {
        OIC_LOG_V(DEBUG, ANALYZER_TAG, "result code = [%d]", data->responseInfo->result);
        info = &data->responseInfo->info;
    }

    if (pdu->transport_hdr)
    {
        OIC_LOG_V(DEBUG, ANALYZER_TAG, "Msg ID = [%u]",
            (uint32_t)ntohs(pdu->transport_hdr->udp.id));
    }

    if (info)
    {
        OIC_LOG(DEBUG, ANALYZER_TAG, "Coap Token");
        OIC_LOG_BUFFER(DEBUG, ANALYZER_TAG, (const uint8_t *) info->token, info->tokenLength);
        OIC_TRACE_BUFFER("OIC_CA_MSG_HANDLE:CALogPDUInfo:token:", (const uint8_t *) info->token,
                         info->tokenLength);
        OIC_LOG_V(DEBUG, ANALYZER_TAG, "Res URI = [%s]", info->resourceUri);
        OIC_TRACE_MARK(%s:CALogPDUInfo:uri:%s, TAG, info->resourceUri);

        if (CA_FORMAT_APPLICATION_CBOR == info->payloadFormat)
        {
            OIC_LOG(DEBUG, ANALYZER_TAG, "Payload Format = [CA_FORMAT_APPLICATION_CBOR]");
        }
        else if (CA_FORMAT_APPLICATION_VND_OCF_CBOR == info->payloadFormat)
        {
            OIC_LOG(DEBUG, ANALYZER_TAG, "Payload Format = [CA_FORMAT_APPLICATION_VND_OCF_CBOR]");
        }
        else
        {
            OIC_LOG_V(DEBUG, ANALYZER_TAG, "Payload Format = [%d]", info->payloadFormat);
        }
    }

#ifdef TB_LOG
    size_t payloadLen = (pdu->data) ? (unsigned char *)pdu->hdr + pdu->length - pdu->data : 0;
#endif
    OIC_LOG_V(DEBUG, ANALYZER_TAG, "CoAP Message Full Size = [%u]", pdu->length);
    OIC_LOG(DEBUG, ANALYZER_TAG, "CoAP Header (+ 0xFF)");
    OIC_LOG_BUFFER(DEBUG, ANALYZER_TAG,  (const uint8_t *) pdu->transport_hdr,
                   pdu->length - payloadLen);
    OIC_LOG_V(DEBUG, ANALYZER_TAG, "CoAP Header size = [%" PRIuPTR "]", (size_t) pdu->length - payloadLen);

    OIC_LOG_V(DEBUG, ANALYZER_TAG, "CoAP Payload");
    //OIC_LOG_BUFFER(DEBUG, ANALYZER_TAG, pdu->data, payloadLen);
    OIC_LOG_V(DEBUG, ANALYZER_TAG, "CoAP Payload Size = [%" PRIuPTR "]", payloadLen);
    OIC_LOG(DEBUG, ANALYZER_TAG, "=================================================");
    OIC_TRACE_END();
}

#ifdef TCP_ADAPTER
CAResult_t CAAddHeaderOption(CAHeaderOption_t **hdrOpt, uint8_t *numOptions,
                             uint16_t optionID, void *optionData, size_t optionDataLength)
{
    OIC_LOG_V(DEBUG, TAG, "Entering CAAddHeaderOption with optionID: %d", optionID);

    VERIFY_NON_NULL(hdrOpt, TAG, "hdrOpt");
    VERIFY_NON_NULL(numOptions, TAG, "numOptions");
    VERIFY_NON_NULL(optionData, TAG, "optionData");

    CAHeaderOption_t *tmpOpt = OICRealloc(*hdrOpt, (*numOptions + 1) * sizeof(CAHeaderOption_t));
    if (!tmpOpt)
    {
        OIC_LOG(ERROR, TAG, "out of memory");
        return CA_MEMORY_ALLOC_FAILED;
    }

    tmpOpt[*numOptions].protocolID = CA_COAP_ID;
    tmpOpt[*numOptions].optionID = optionID;
    tmpOpt[*numOptions].optionLength = (uint16_t)optionDataLength;

    if (optionData)
    {
        memcpy(tmpOpt[*numOptions].optionData, optionData,
               sizeof(tmpOpt[*numOptions].optionData));
    }

    // increase the number of options.
    *numOptions += 1;
    *hdrOpt = tmpOpt;

    OIC_LOG(DEBUG, TAG, "Added option successfully");
    return CA_STATUS_OK;
}

CAResult_t CAGetHeaderOption(CAHeaderOption_t *hdrOpt, size_t numOptions, uint16_t optionID,
                             void *optionData, size_t optionDataLength, uint16_t *receivedDataLen)
{
    OIC_LOG_V(DEBUG, TAG, "Entering CAGetHeaderOption with optionID: %d", optionID);

    VERIFY_NON_NULL(hdrOpt, TAG, "hdrOpt");
    VERIFY_NON_NULL(optionData, TAG, "optionData");
    VERIFY_NON_NULL(receivedDataLen, TAG, "receivedDataLen");

    for (size_t i = 0; i < numOptions; i++)
    {
        if (hdrOpt[i].optionID == optionID)
        {
            if (optionDataLength >= hdrOpt[i].optionLength)
            {
                memcpy(optionData, hdrOpt[i].optionData, hdrOpt[i].optionLength);
                *receivedDataLen = hdrOpt[i].optionLength;
                return CA_STATUS_OK;
            }
        }
    }

    return CA_STATUS_NOT_FOUND;
}

CAData_t *CAGenerateSignalingMessage(const CAEndpoint_t *endpoint, CASignalingCode_t code,
                                     CAHeaderOption_t *headerOpt, uint8_t numOptions)
{
    OIC_LOG(DEBUG, TAG, "GenerateSignalingMessage - IN");

    // create token for signaling message.
    CAToken_t token = NULL;
    uint8_t tokenLength = CA_MAX_TOKEN_LEN;
    if (CA_STATUS_OK != CAGenerateTokenInternal(&token, tokenLength))
    {
        OIC_LOG(ERROR, TAG, "CAGenerateTokenInternal failed");
        return NULL;
    }

    CAInfo_t signalingData = { .type = CA_MSG_NONCONFIRM,
                               .token = token,
                               .tokenLength = tokenLength,
                               .numOptions = numOptions,
                               .options = headerOpt };

    CASignalingInfo_t sigMsg = { .code = code,
                                 .info = signalingData };

    return CAPrepareSendData(endpoint, &sigMsg, CA_SIGNALING_DATA);
}

CAData_t *CAGenerateSignalingMessageUsingToken(const CAEndpoint_t *endpoint, CASignalingCode_t code,
                                               CAHeaderOption_t *headerOpt, uint8_t numOptions,
                                               const CAToken_t pingToken, uint8_t pingTokenLength)
{
    OIC_LOG(DEBUG, TAG, "GenerateSignalingMessage - IN");

    // create token for signaling message.
    CAToken_t token = (char *)OICCalloc(pingTokenLength, sizeof(char));
    memcpy(token, pingToken, pingTokenLength);

    CAInfo_t signalingData = { .type = CA_MSG_NONCONFIRM,
                               .token = token,
                               .tokenLength = pingTokenLength,
                               .numOptions = numOptions,
                               .options = headerOpt };

    CASignalingInfo_t sigMsg = { .code = code,
                                 .info = signalingData };

    return CAPrepareSendData(endpoint, &sigMsg, CA_SIGNALING_DATA);
}
#endif //TCP_ADAPTER

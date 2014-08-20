//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
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


//=============================================================================
// Includes
//=============================================================================
#include "occoap.h"
#include "occlientcb.h"
#include "ocobserve.h"
#include "logger.h"
#include <coap.h>

#ifndef WITH_ARDUINO
#include <unistd.h>
#endif
#include <limits.h>
#include <ctype.h>

//-----------------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------------
#define TAG    PCF("OCCoAP")
#define VERIFY_SUCCESS(op, successCode) { if (op != successCode) \
            {OC_LOG_V(FATAL, TAG, "%s failed!!", #op); goto exit;} }
#define VERIFY_NON_NULL(arg) { if (!arg) {OC_LOG_V(FATAL, TAG, "%s is NULL", #arg); goto exit;} }

#define BUF_SIZE (64)

//=============================================================================
// Private Variables
//=============================================================================

static uint8_t coapWKIpAddr[] = { 224, 0, 1, 187 };
static coap_context_t *gCoAPCtx = NULL;

//=============================================================================
// Helper Functions
//=============================================================================

//generate a coap token
OCCoAPToken * OCGenerateCoAPToken()
{
    OCCoAPToken *token = NULL;
    // Generate token here, it will be deleted when the transaction is deleted
    token = (OCCoAPToken *) malloc(sizeof(OCCoAPToken));
    if (token)
    {
        token->tokenLength = MAX_TOKEN_LENGTH;
        OCFillRandomMem((uint8_t*)token->token, token->tokenLength);
    }

    return token;
}

//This function is called back by libcoap when ack or rst are received
static void HandleCoAPAckRst(struct coap_context_t * ctx, uint8_t msgType,
        const coap_queue_t * sentQueue){

    // silence warnings
    (void) ctx;

    OCStackResult result = OC_STACK_ERROR;
    OCCoAPToken * sentToken = NULL;
    uint8_t * observeOption = NULL;
    coap_pdu_t * sentPdu = sentQueue->pdu;

    // fill the buffers of Uri and Query
    result = ParseCoAPPdu(sentPdu, NULL, NULL, &observeOption, NULL);
    VERIFY_SUCCESS(result, OC_STACK_OK);

    // fill OCCoAPToken structure
    result = RetrieveOCCoAPToken(sentPdu, &sentToken);
    VERIFY_SUCCESS(result, OC_STACK_OK);

    if(msgType == COAP_MESSAGE_RST){
        // now the observer should be deleted
        result = OCObserverStatus(sentToken, OC_OBSERVER_NOT_INTERESTED);
        if(result == OC_STACK_OBSERVER_REMOVED){
            OC_LOG_V(DEBUG, TAG, "Received RST, removing all queues associated with Token %d bytes",sentToken->tokenLength);
            OC_LOG_BUFFER(INFO, TAG, sentToken->token, sentToken->tokenLength);
            coap_cancel_all_messages(ctx, &sentQueue->remote, sentToken->token,
                    sentToken->tokenLength);
        }
    }else if(observeOption && msgType == COAP_MESSAGE_ACK){
        OC_LOG_V(DEBUG, TAG, "Received ACK, for Token %d bytes",sentToken->tokenLength);
        OC_LOG_BUFFER(INFO, TAG, sentToken->token, sentToken->tokenLength);
        // now the observer is still interested
        OCObserverStatus(sentToken, OC_OBSERVER_STILL_INTERESTED);
    }

    exit:
        OCFree(sentToken);
        OCFree(observeOption);
}

//This function is called back by libcoap when a request is received
static void HandleCoAPRequests(struct coap_context_t *ctx,
        const coap_queue_t * rcvdRequest)
{
    // silence warnings
    (void) ctx;

    OCStackResult result = OC_STACK_ERROR;
    OCStackResult responseResult = OC_STACK_ERROR;
    OCRequest * request = NULL;
    OCEntityHandlerRequest * entityHandlerRequest = NULL;
    OCCoAPToken * rcvdToken = NULL;
    OCObserveReq * rcvdObsReq = NULL;
    coap_pdu_t * sendPdu = NULL;
    coap_list_t *optList = NULL;
    uint8_t mediaType = COAP_MEDIATYPE_APPLICATION_JSON;
    uint32_t maxAge = 0x2ffff;

    unsigned char rcvdUri[MAX_URI_LENGTH] = { 0 };
    unsigned char rcvdQuery[MAX_QUERY_LENGTH] = { 0 };
    unsigned char bufRes[MAX_RESPONSE_LENGTH] = { 0 };
    uint8_t * rcvObserveOption = NULL;
    unsigned char * bufReqPayload = NULL;
    uint8_t observeOption = OC_RESOURCE_NO_OBSERVE;

    coap_pdu_t * recvPdu = rcvdRequest->pdu;

    // fill the buffers of Uri and Query
    result = ParseCoAPPdu(recvPdu, rcvdUri, rcvdQuery, &rcvObserveOption, &bufReqPayload);
    VERIFY_SUCCESS(result, OC_STACK_OK);
    if(rcvObserveOption){
        observeOption = (uint8_t)(*rcvObserveOption);
    }

    // fill OCCoAPToken structure
    result = RetrieveOCCoAPToken(recvPdu, &rcvdToken);
    VERIFY_SUCCESS(result, OC_STACK_OK);

    // fill OCEntityHandlerRequest structure
    result = FormOCEntityHandlerRequest(&entityHandlerRequest,
            (recvPdu->hdr->code == COAP_REQUEST_GET) ?
            OC_REST_GET : OC_REST_PUT, bufRes, bufReqPayload, rcvdQuery);
    VERIFY_SUCCESS(result, OC_STACK_OK);

   // fill OCObserveReq
   result = FormOCObserveReq(&rcvdObsReq, observeOption,
           (OCDevAddr *)&(rcvdRequest->remote), rcvdToken);
   VERIFY_SUCCESS(result, OC_STACK_OK);

    // fill OCRequest structure
    result = FormOCRequest(&request, (recvPdu->hdr->type == COAP_MESSAGE_CON) ?
            OC_CONFIRMABLE : OC_NON_CONFIRMABLE, rcvdUri, rcvdObsReq, entityHandlerRequest);
    VERIFY_SUCCESS(result, OC_STACK_OK);

    OC_LOG_V(INFO, TAG, " Receveid uri:     %s", request->resourceUrl);
    OC_LOG_V(INFO, TAG, " Receveid query:   %s", entityHandlerRequest->query);
    OC_LOG_V(INFO, TAG, " Receveid payload: %s",
            request->entityHandlerRequest->reqJSONPayload);
    OC_LOG_V(INFO, TAG, " Token received %d bytes",
            rcvdToken->tokenLength);
    OC_LOG_BUFFER(INFO, TAG, rcvdToken->token, rcvdToken->tokenLength);

    // process the request
    responseResult = HandleStackRequests(request);

    TODO("we should return the same registration option; however, this will confuse the receiver \
             whether it is a sequence number or registration option!------------");
    OC_LOG_V(INFO, TAG, "Response from ocstack: %s", request->entityHandlerRequest->resJSONPayload);

    switch(responseResult)
    {
    case OC_STACK_OBSERVER_ADDED:
        observeOption = OC_RESOURCE_OBSERVE_REGISTER;
        result = FormResponseOptList(&optList, &mediaType, &maxAge, 0, NULL);
        break;
    case OC_STACK_OBSERVER_REMOVED:
        observeOption = OC_RESOURCE_OBSERVE_DEREGISTER;
        result = FormResponseOptList(&optList, &mediaType, &maxAge, 0, NULL);
        break;
    case OC_STACK_OK:
    default:
        result = FormResponseOptList(&optList, &mediaType, &maxAge, 0, NULL);
        break;
    }
    VERIFY_SUCCESS(result, OC_STACK_OK);

    // generate the pdu, if the request was CON, then the response is ACK, otherwire NON
    sendPdu = GenerateCoAPPdu(
            (rcvdRequest->pdu->hdr->type == COAP_MESSAGE_CON) ?
                    COAP_MESSAGE_ACK : COAP_MESSAGE_NON,
                    OCToCoAPResponseCode(responseResult), rcvdRequest->pdu->hdr->id,
                    rcvdToken,
                    request->entityHandlerRequest->resJSONPayload, optList);
    VERIFY_NON_NULL(sendPdu);
    coap_show_pdu(sendPdu);

    if(SendCoAPPdu(gCoAPCtx, (coap_address_t*) &(rcvdRequest->remote), sendPdu, rcvdRequest->delayedResponse)
            != OC_STACK_OK){
        OC_LOG(DEBUG, TAG, PCF("A problem occurred in sending a pdu"));
    }

exit:
    OCFree(rcvObserveOption);
    OCFree(rcvdToken);
    OCFree(entityHandlerRequest);
    OCFree(rcvdObsReq);
    OCFree(request);
}

//This function is called back by libcoap when a response is received
static void HandleCoAPResponses(struct coap_context_t *ctx,
        const coap_queue_t * rcvdResponse) {
    OCResponse * response = NULL;
    OCCoAPToken * rcvdToken = NULL;
    OCClientResponse * clientResponse = NULL;
    ClientCB * cbNode = NULL;
    unsigned char * bufRes = NULL;
    uint8_t * rcvObserveOption;
    uint32_t sequenceNumber = 0;
    OCStackResult result = OC_STACK_ERROR;
    coap_pdu_t *sendPdu = NULL;
    //coap_list_t *optList = NULL;
    //uint8_t deregisterObserveOption = OC_RESOURCE_OBSERVE_DEREGISTER;

    VERIFY_NON_NULL(ctx);
    VERIFY_NON_NULL(rcvdResponse);
    coap_pdu_t * recvPdu = rcvdResponse->pdu;

    result = ParseCoAPPdu(recvPdu, NULL, NULL, &rcvObserveOption, &bufRes);
    VERIFY_SUCCESS(result, OC_STACK_OK);

    if(rcvObserveOption){
        sequenceNumber = *((uint32_t *) rcvObserveOption);
    }

    OC_LOG_V(DEBUG, TAG, "The sequence number of this response %d", sequenceNumber);
    OC_LOG_V(DEBUG, TAG, "The response received is %s", bufRes);

    // fill OCCoAPToken structure
    result = RetrieveOCCoAPToken(recvPdu, &rcvdToken);
    VERIFY_SUCCESS(result, OC_STACK_OK);
    OC_LOG_V(INFO, TAG,"Received a pdu with Token", rcvdToken->tokenLength);
    OC_LOG_BUFFER(INFO, TAG, rcvdToken->token, rcvdToken->tokenLength);

    // fill OCClientResponse structure
    result = FormOCClientResponse(&clientResponse, CoAPToOCResponseCode(recvPdu->hdr->code),
            (OCDevAddr *) &(rcvdResponse->remote), sequenceNumber, bufRes);
    VERIFY_SUCCESS(result, OC_STACK_OK);

    cbNode = GetClientCB(rcvdToken, NULL);

    // fill OCResponse structure
    result = FormOCResponse(&response, cbNode, clientResponse);
    VERIFY_SUCCESS(result, OC_STACK_OK);

    if(cbNode)
    {
        if(clientResponse->sequenceNumber != 0 &&
                (cbNode->method == OC_REST_OBSERVE ||
                        cbNode->method == OC_REST_OBSERVE_ALL)){
            OC_LOG(INFO, TAG, PCF("Received an observe notification"));
            if(recvPdu->hdr->type == COAP_MESSAGE_CON){
                sendPdu = GenerateCoAPPdu(COAP_MESSAGE_ACK, 0,
                        recvPdu->hdr->id, NULL, NULL, NULL);
                VERIFY_NON_NULL(sendPdu);
                result = SendCoAPPdu(gCoAPCtx, (coap_address_t*) &rcvdResponse->remote,
                        sendPdu, 0);
            }
            if(cbNode->method == OC_REST_OBSERVE &&
                    clientResponse->sequenceNumber <= cbNode->sequenceNumber){
                OC_LOG_V(DEBUG, TAG, "Observe notification came out of order. \
                        Ignoring Incoming:%d  Against Current:%d.",
                        clientResponse->sequenceNumber, cbNode->sequenceNumber);
                goto exit;
            }
            if(clientResponse->sequenceNumber > cbNode->sequenceNumber){
                cbNode->sequenceNumber = clientResponse->sequenceNumber;
            }
        }
        HandleStackResponses(response);
    }else if(!cbNode && clientResponse->sequenceNumber != 0){
        OC_LOG(INFO, TAG, PCF("Received an observe notification, but I do not have callback \
                 ------------ sending RESET"));
        sendPdu = GenerateCoAPPdu(COAP_MESSAGE_RST, 0,
                recvPdu->hdr->id, NULL, NULL, NULL);
        VERIFY_NON_NULL(sendPdu);
        result = SendCoAPPdu(gCoAPCtx, (coap_address_t*) &rcvdResponse->remote, sendPdu, 0);
        VERIFY_SUCCESS(result, OC_STACK_OK);
    }else{
        // TODO: we should send a RST here and..
    }
    exit:
        OCFree(rcvObserveOption);
        OCFree(rcvdToken);
        OCFree(clientResponse);
        OCFree(response);
}

//=============================================================================
// Functions
//=============================================================================

/**
 * Initialize the CoAP client or server with its IPv4 address and CoAP port
 *
 * @param ipAddr
 *     IP Address of host device
 * @param port
 *     Port of host device
 * @param mode
 *     Host device is client, server, or client-server
 *
 * @return
 *   0   - success
 *   TBD - TBD error
 */
OCStackResult OCInitCoAP(const char *address, uint16_t port, OCMode mode) {

    int ret = OC_STACK_ERROR;

    TODO ("Below should go away and be replaced by OC_LOG");
    coap_log_t log_level = (coap_log_t)(LOG_DEBUG + 1);
    OCDevAddr devAddr;
    OCDevAddr mcastAddr;
    uint8_t ipAddr[4] = { 0 };

    OC_LOG(INFO, TAG, PCF("Entering OCInitCoAP"));

    coap_set_log_level(log_level);

    if (address)
    {
        if (!ParseIPv4Address((unsigned char *) address, ipAddr))
        {
            ret = OC_STACK_ERROR;
            goto exit;
        }
        OC_LOG_V(INFO, TAG, "Parsed IP Address %d.%d.%d.%d",ipAddr[0],ipAddr[1],ipAddr[2],ipAddr[3]);

        OCBuildIPv4Address(ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3], port,
                    &devAddr);
    }

    OCBuildIPv4Address(ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3], port,
            &devAddr);

    gCoAPCtx = coap_new_context((coap_address_t*) &devAddr);
    VERIFY_NON_NULL(gCoAPCtx);
    if (mode != OC_CLIENT) {
        OCBuildIPv4Address(coapWKIpAddr[0], coapWKIpAddr[1], coapWKIpAddr[2],
                coapWKIpAddr[3], COAP_DEFAULT_PORT, &mcastAddr);
        VERIFY_SUCCESS(
                coap_join_wellknown_group(gCoAPCtx,
                        (coap_address_t* )&mcastAddr), 0);
    }

    coap_register_request_handler(gCoAPCtx, HandleCoAPRequests);
    coap_register_response_handler(gCoAPCtx, HandleCoAPResponses);
    coap_register_ack_rst_handler(gCoAPCtx, HandleCoAPAckRst);

    ret = OC_STACK_OK;

exit:
    if (ret != OC_STACK_OK)
    {
        OCStopCoAP();
    }
    return ret;
}

/**
 * Discover OC resources
 *
 * @param method          - method to perform on the resource
 * @param qos             - Quality of Service the request will be sent on
 * @param token           - token which will added to the request
 * @param Uri             - URI of the resource to interact with
 * @param payload         - the request payload to be added to the request before sending
 *                          by the stack when discovery or resource interaction is complete
 * @return
 *   0   - success
 *   TBD - TBD error
 */
OCStackResult OCDoCoAPResource(OCMethod method, OCQualityOfService qos, OCCoAPToken * token,
                     const char *Uri, const char *payload)
{

    int ret = OC_STACK_ERROR;
    coap_pdu_t *pdu = NULL;
    coap_uri_t uri;
    OCDevAddr dst;
    uint8_t ipAddr[4] = { 0 };
    coap_list_t *optList = NULL;
    size_t buflen;
    unsigned char _buf[BUF_SIZE];
    unsigned char *buf = _buf;
    int res;
    uint8_t coapMsgType;
    uint8_t coapMethod;
    uint8_t observeOption;

    OC_LOG(INFO, TAG, PCF("Entering OCDoCoAPResource"));

    if (Uri) {
        OC_LOG_V(INFO, TAG, "URI = %s", Uri);
        VERIFY_SUCCESS(coap_split_uri((unsigned char * )Uri, strlen(Uri), &uri), 0);

        // Generate the destination address
        if (uri.host.length && ParseIPv4Address(uri.host.s, ipAddr)) {
            OCBuildIPv4Address(ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3], uri.port,
                    &dst);
        } else {
            goto exit;
        }

        //create appropriate coap options
        if (uri.port != COAP_DEFAULT_PORT) {
            coap_insert(&optList, CreateNewOptionNode( COAP_OPTION_URI_PORT,
                            sizeof(uri.port), (uint8_t *)&(uri.port)), OrderOptions);
        }

        if (uri.path.length) {
            buflen = BUF_SIZE;
            res = coap_split_path(uri.path.s, uri.path.length, buf, &buflen);

            while (res--) {
                coap_insert(&optList,
                        CreateNewOptionNode(COAP_OPTION_URI_PATH,
                                COAP_OPT_LENGTH(buf), COAP_OPT_VALUE(buf)),
                        OrderOptions);

                buf += COAP_OPT_SIZE(buf);
            }
        }

        if (uri.query.length) {
            buflen = BUF_SIZE;
            buf = _buf;
            res = coap_split_query(uri.query.s, uri.query.length, buf, &buflen);

            while (res--) {
                coap_insert(&optList,
                        CreateNewOptionNode(COAP_OPTION_URI_QUERY,
                                COAP_OPT_LENGTH(buf), COAP_OPT_VALUE(buf)),
                        OrderOptions);

                buf += COAP_OPT_SIZE(buf);
            }
        }

        OC_LOG_V(DEBUG, TAG, "uri.host.s %s", uri.host.s);
        OC_LOG_V(DEBUG, TAG, "uri.path.s %s", uri.path.s);
        OC_LOG_V(DEBUG, TAG, "uri.port %d", uri.port);
        OC_LOG_V(DEBUG, TAG, "uri.query.s %s", uri.query.s);
    }

    coapMsgType = COAP_MESSAGE_NON;
    // Decide message type
    if (qos == OC_CONFIRMABLE) {
        coapMsgType = COAP_MESSAGE_CON;
    }

    // Decide method type
    switch (method) {
        case OC_REST_GET:
            coapMethod = COAP_REQUEST_GET;
            break;
        case OC_REST_PUT:
            coapMethod = COAP_REQUEST_PUT;
            break;
        case OC_REST_OBSERVE_ALL:
        case OC_REST_OBSERVE:
            coapMethod = COAP_REQUEST_GET;
            observeOption = OC_RESOURCE_OBSERVE_REGISTER;
            coap_insert(&optList, CreateNewOptionNode(COAP_OPTION_OBSERVE,
                        sizeof(observeOption), &observeOption), OrderOptions);

            break;
        default:
            coapMethod = OC_REST_NOMETHOD;
            OC_LOG(FATAL, TAG, PCF("OCDoCoAPResource only supports GET, PUT, & OBSERVE methods"));
            break;
    }

    VERIFY_NON_NULL(gCoAPCtx);
    pdu = GenerateCoAPPdu(coapMsgType, coapMethod,
            coap_new_message_id(gCoAPCtx), token,
            (unsigned char*) payload, optList);
    VERIFY_NON_NULL(pdu);

    ret = SendCoAPPdu(gCoAPCtx, (coap_address_t*) &dst, pdu, 0);

exit:
    if (ret!= OC_STACK_OK)
    {
        OC_LOG(DEBUG, TAG, PCF("A problem occurred in sending a pdu"));
    }
    return ret;
}

OCStackResult OCSendCoAPNotification (OCDevAddr *dstAddr, OCStackResult result,
                       OCQualityOfService qos, OCCoAPToken * token,
                       unsigned char *payload, uint32_t seqNum)
{
    coap_list_t *optList = NULL;
    uint8_t coapMsgType = COAP_MESSAGE_NON;
    uint8_t mediaType = COAP_MEDIATYPE_APPLICATION_JSON;
    uint32_t maxAge = 0x2ffff;
    coap_pdu_t *sendPdu;

    OC_LOG(INFO, TAG, PCF("Entering OCSendCoAPNotification"));

    coapMsgType = COAP_MESSAGE_NON;
    // Decide message type
    if (qos == OC_CONFIRMABLE) {
        coapMsgType = COAP_MESSAGE_CON;
    }

    result = FormResponseOptList(&optList, &mediaType, &maxAge, 4, (uint8_t *)(&seqNum));
    VERIFY_SUCCESS(result, OC_STACK_OK);
    sendPdu = GenerateCoAPPdu(
            coapMsgType == COAP_MESSAGE_CON ? COAP_MESSAGE_CON : COAP_MESSAGE_NON,
                    OCToCoAPResponseCode(result), coap_new_message_id(gCoAPCtx),
                    token, payload, optList);
    VERIFY_NON_NULL(sendPdu);
    coap_show_pdu(sendPdu);

    if(SendCoAPPdu(gCoAPCtx, (coap_address_t*) dstAddr, sendPdu, 0)
            != OC_STACK_OK){
        OC_LOG(DEBUG, TAG, PCF("A problem occurred in sending a pdu"));
    }
    return OC_STACK_OK;
exit:
    return OC_STACK_ERROR;
}

/**
 * Stop the CoAP client or server processing
 *
 * @return 0 - success, else - TBD error
 */
OCStackResult OCStopCoAP() {
    OC_LOG(INFO, TAG, PCF("Entering OCStopCoAP"));
    coap_free_context(gCoAPCtx);
    gCoAPCtx = NULL;
    return OC_STACK_OK;
}

/**
 * Called in main loop of CoAP client or server.  Allows low-level CoAP processing of
 * send, receive, timeout, discovery, callbacks, etc.
 *
 * @return 0 - success, else - TBD error
 */
OCStackResult OCProcessCoAP() {

    OC_LOG(INFO, TAG, PCF("Entering OCProcessCoAP"));
    int read = 0;
    read = coap_read(gCoAPCtx, gCoAPCtx->sockfd);
    if(read > 0)
    {
        OC_LOG(INFO, TAG, PCF("This is a Unicast<============"));
    }
    if (-1 != gCoAPCtx->sockfd_wellknown) {
        read = coap_read(gCoAPCtx, gCoAPCtx->sockfd_wellknown);
        if(read > 0)
        {
            OC_LOG(INFO, TAG, PCF("This is a Multicast<==========="));
        }
    }
    coap_dispatch(gCoAPCtx);

    HandleSendQueue(gCoAPCtx);

    return OC_STACK_OK;
}


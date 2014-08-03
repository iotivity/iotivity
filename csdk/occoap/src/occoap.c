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
            {OC_LOG(FATAL, TAG, #op " failed !!"); goto exit;} }
#define VERIFY_NON_NULL(arg) { if (!arg) {OC_LOG(FATAL, TAG, #arg " is NULL"); goto exit;} }

#define BUF_SIZE (64)
#define BUF_SIZE_ENCODE_OPTION (3)
#define BUF_SIZE_PORT (2)

//=============================================================================
// Private Variables
//=============================================================================

static uint8_t coapWKIpAddr[] = { 224, 0, 1, 187 };
static coap_context_t *gCoAPCtx = NULL;

//=============================================================================
// Helper Functions
//=============================================================================

//generate a coap token
OCCoAPToken * OCGenerateCoAPToken() {
    OCCoAPToken *token;
    // Generate token here, it will be deleted when the transaction is deleted
    token = (OCCoAPToken *) malloc(sizeof(OCCoAPToken));
    token->tokenLength = MAX_TOKEN_LENGTH;
    OCFillRandomMem((uint8_t*)token->token, token->tokenLength);

    //OC_LOG_V(INFO,TAG,"Toke n generated %d bytes..........%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",
    //		token->tokenLength,token->token[0],token->token[1],token->token[2],token->token[3],
    //		token->token[4],token->token[5],token->token[6],token->token[7]);
    return token;
}
//This function is called back by libcoap when a request is received
static void HandleCoAPRequests(struct coap_context_t *ctx,
        const coap_queue_t * rcvdRequest)
{
    // silence warnings
    (void) ctx;

    OCStackResult result;
    OCRequest * request = NULL;
    OCEntityHandlerRequest * entityHandlerRequest = NULL;
    OCCoAPToken * rcvdToken = NULL;

    unsigned char rcvdUri[MAX_URI_LENGTH] = { 0 };
    unsigned char rcvdQuery[MAX_QUERY_LENGTH] = { 0 };

    unsigned char bufRes[MAX_RESPONSE_LENGTH] = { 0 };

    coap_list_t *optList = NULL;
    unsigned char tempBuf[BUF_SIZE_ENCODE_OPTION];
    coap_pdu_t *pdu;
    coap_tid_t tid = COAP_INVALID_TID;

    // fill OCRequest structure
    result = FormOCRequest(rcvdRequest, &request, rcvdUri, rcvdQuery);
    VERIFY_SUCCESS(result, OC_STACK_OK);

    // fill OCEntityHandlerRequest structure
    result = FormOCEntityHandlerRequest(rcvdRequest, &entityHandlerRequest,
            bufRes, rcvdQuery);
    VERIFY_SUCCESS(result, OC_STACK_OK);

    // fill OCCoAPToken structure
    result = RetrieveOCCoAPToken(rcvdRequest, &rcvdToken);
    VERIFY_SUCCESS(result, OC_STACK_OK);

    request->entityHandlerRequest = entityHandlerRequest;

    OC_LOG_V(INFO, TAG, " Receveid uri:     %s", request->resourceUrl);
    OC_LOG_V(INFO, TAG, " Receveid query:   %s", entityHandlerRequest->query);
    OC_LOG_V(INFO, TAG, " Receveid payload: %s",
            request->entityHandlerRequest->reqJSONPayload);

    if(GetClientCB(rcvdToken, 0)) { //True if the token was found in the list of clientCBs
        OC_LOG_V(INFO, TAG, " Token received %d bytes",
                rcvdToken->tokenLength);
        OC_LOG_BUFFER(INFO, TAG, rcvdToken->token, rcvdToken->tokenLength);
    }

    // process the request
    result = HandleStackRequests(request);

    if (result == OC_STACK_OK)
    {
        OC_LOG_V(INFO, TAG, "Response from ocstack: %s", request->entityHandlerRequest->resJSONPayload);
        // need to build the response PDU
        coap_insert(&optList, CreateNewOptionNode(COAP_OPTION_CONTENT_TYPE,
                    coap_encode_var_bytes(tempBuf, COAP_MEDIATYPE_APPLICATION_JSON),
                    tempBuf), OrderOptions);
        coap_insert(&optList, CreateNewOptionNode(COAP_OPTION_MAXAGE,
                    coap_encode_var_bytes(tempBuf, 0x2ffff), tempBuf),
                    OrderOptions);
    }

    // generate the pdu, if the request was CON, then the response is ACK, otherwire NON
    pdu = GenerateCoAPPdu(
            rcvdRequest->pdu->hdr->type == COAP_MESSAGE_CON ?
            COAP_MESSAGE_ACK : COAP_MESSAGE_NON,
            OCToCoAPResponseCode(result), rcvdRequest->pdu->hdr->id,
            rcvdToken->tokenLength, rcvdToken->token,
            request->entityHandlerRequest->resJSONPayload, optList);
    VERIFY_NON_NULL(pdu);
    coap_show_pdu(pdu);

    if (pdu->hdr->type != COAP_MESSAGE_NON
            || (pdu->hdr->code >= 64 && !coap_is_mcast(&rcvdRequest->local)))
    {
        tid = coap_send(gCoAPCtx, &rcvdRequest->remote, pdu);
    }

    OC_LOG_V(INFO, TAG, "TID %d", tid);
    // unlike stock libcoap (deletion in handle_request in net.c), we are deleting the response here
    // in the future, the response might be queued for SLOW resources
    if (pdu->hdr->type != COAP_MESSAGE_CON || tid == COAP_INVALID_TID)
    {
        OC_LOG(INFO, TAG, "Deleting PDU");
        coap_delete_pdu(pdu);
    }

exit:
    coap_delete_list(optList);
    if(rcvdToken) {
        OCFree(rcvdToken);
        rcvdToken = NULL;
    }
    OCFree(entityHandlerRequest);
    OCFree(request);
}

//This function is called back by libcoap when a response is received
static void HandleCoAPResponses(struct coap_context_t *ctx,
        const coap_queue_t * rcvdResponse) {
    OCResponse * response = NULL;
    OCCoAPToken * _token = NULL;
    OCClientResponse * clientResponse = NULL;
    OCStackResult result;

    VERIFY_NON_NULL(ctx);
    VERIFY_NON_NULL(rcvdResponse);

    // TODO: we should check if we are interested in the token
    // Now, just accept NON packets

    if (rcvdResponse->pdu->hdr->type == COAP_MESSAGE_NON) {
        // fill OCResponse structure
        result = FormOCResponse(rcvdResponse, &response);
        VERIFY_SUCCESS(result, OC_STACK_OK);

        // fill OCCoAPToken structure
        result = RetrieveOCCoAPToken(rcvdResponse, &_token);
        VERIFY_SUCCESS(result, OC_STACK_OK);

        // fill OCClientResponse structure
        result = FormOCClientResponse(rcvdResponse, &clientResponse);
        VERIFY_SUCCESS(result, OC_STACK_OK);

        // put everything together
        ClientCB * clientCB = GetClientCB(_token, NULL);
        OCDoHandle *handle = NULL;
        if(clientCB) {
            handle = clientCB->handle;
            response->handle = handle;
        }
        response->clientResponse = clientResponse;

        OC_LOG_V(INFO, TAG, " Received a response HandleCoAPResponses in occoap: %s",
                 response->clientResponse->resJSONPayload);
        OC_LOG_V(INFO, TAG,"Token received %d bytes", _token->tokenLength);
        OC_LOG_BUFFER(INFO, TAG, _token->token,
                      _token->tokenLength);

        response->clientResponse->result = OC_STACK_OK;
        HandleStackResponses(response);
        /*if (rcvdResponse->pdu->hdr->code == COAP_RESPONSE_CODE(205))
        {
            response->clientResponse->result = OC_STACK_OK;
            HandleStackResponses(response);
        }
        else
        {
            OC_LOG(DEBUG, TAG,
                   "No other response codes are supported in HandleCoAPResponses");
        }*/
    }
    else
    {
        OC_LOG(DEBUG, TAG, "Do not accept other than NON in HandleCoAPResponses");
    }

exit:
    OCFree(response);
    if(_token) {
        OCFree(_token);
        _token = NULL;
    }
    OCFree(clientResponse);
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
int OCInitCoAP(const char *address, uint16_t port, OCMode mode) {

    int ret = OC_COAP_ERR;

    TODO ("Below should go away and be replaced by OC_LOG");
    coap_log_t log_level = LOG_DEBUG + 1;
    OCDevAddr devAddr;
    OCDevAddr mcastAddr;
    uint8_t ipAddr[4] = { 0 };

    OC_LOG(INFO, TAG, PCF("Entering OCInitCoAP"));

    coap_set_log_level(log_level);

    if (address)
    {
        if (!ParseIPv4Address((unsigned char *) address, ipAddr))
        {
            return OC_COAP_ERR;
        }
        OC_LOG_V(INFO, TAG, "Parsed IP Address %d.%d.%d.%d",ipAddr[0],ipAddr[1],ipAddr[2],ipAddr[3]);
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

    ret = OC_COAP_OK;

exit:
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
int OCDoCoAPResource(OCMethod method, OCQualityOfService qos, OCCoAPToken * token,
                     const char *Uri, const char *payload)
{

    int ret = OC_COAP_ERR;
    coap_pdu_t *pdu = NULL;
    coap_uri_t uri;
    coap_tid_t tid = COAP_INVALID_TID;
    OCDevAddr dst;
    uint8_t ipAddr[4] = { 0 };
    coap_list_t *optList = NULL;
    unsigned char portBuf[BUF_SIZE_PORT];
    size_t buflen;
    unsigned char _buf[BUF_SIZE];
    unsigned char *buf = _buf;
    int res;
    uint8_t coapMsgType;
    uint8_t coapMethod;
    // Vijay: TODO Observation registration is hardcoded here - change
    unsigned char obs[] = "0";

    OC_LOG(INFO, TAG, PCF("Entering OCDoCoAPResource"));

    if (Uri) {
        OC_LOG_V(INFO, TAG, "URI = %s", Uri);
    }

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
        coap_insert(&optList,
                CreateNewOptionNode(COAP_OPTION_URI_PORT,
                        coap_encode_var_bytes(portBuf, uri.port), portBuf),
                OrderOptions);
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

    coapMsgType = COAP_MESSAGE_NON;

    // Decide message type
    if (qos == OC_CONFIRMABLE) {
        coapMsgType = COAP_MESSAGE_CON;
        OC_LOG(FATAL, TAG, "qos == OC_CONFIRMABLE is not supported in OCDoCoAPResource");
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
#if 0
            // Joey's add for observation registration: not working.
            // Vijay's change below
            buflen = BUF_SIZE;
            buf = _buf;
            res = coap_split_query(uri.query.s, uri.query.length, buf, &buflen);
            //Set observe option flag.
            while (res--) {
                coap_insert(&optList,
                        CreateNewOptionNode(COAP_OPTION_SUBSCRIPTION,
                                COAP_OPT_LENGTH(buf), COAP_OPT_VALUE(buf)),
                        OrderOptions);

                buf += COAP_OPT_SIZE(buf);
            }
#endif
            coap_insert(&optList, CreateNewOptionNode(COAP_OPTION_OBSERVE,
                        strlen((const char *)obs), (obs)), OrderOptions);

            break;
        default:
            coapMethod = 0;
            OC_LOG(FATAL, TAG, "OCDoCoAPResource only supports GET, PUT, & OBSERVE methods");
            break;
    }

    VERIFY_NON_NULL(gCoAPCtx);
    pdu = GenerateCoAPPdu(coapMsgType, coapMethod,
            coap_new_message_id(gCoAPCtx), token->tokenLength, token->token,
            (unsigned char*) payload, optList);
    VERIFY_NON_NULL(pdu);

    TODO ("Post Sprint 1 -- Send Confirmed requests for non-discovery requests");
    tid = coap_send(gCoAPCtx, (coap_address_t*) &dst, pdu);

    OC_LOG_V(INFO, TAG, "TID %d", tid);
    if (pdu->hdr->type != COAP_MESSAGE_CON || tid == COAP_INVALID_TID)
    {
        OC_LOG(INFO, TAG, "Deleting PDU");
        coap_delete_pdu(pdu);
        pdu = NULL;
    }
    else
    {
        OC_LOG(INFO, TAG, "Keeping PDU, we should handle the retry of this pdu");
    }

    ret = OC_COAP_OK;

exit:
    coap_delete_list(optList);
    if (ret != OC_COAP_OK)
    {
        coap_delete_pdu(pdu);
    }
    return ret;
}

int OCCoAPSendMessage (OCDevAddr *dstAddr, OCStackResult msgCode, 
                       OCQualityOfService qos, OCCoAPToken * token,
                       const char *payload, uint32_t seqNum)
{
    coap_list_t *optList = NULL;
    coap_pdu_t *pdu;
    unsigned char tempBuf[BUF_SIZE_ENCODE_OPTION];
    uint8_t coapMsgType = COAP_MESSAGE_NON;
    coap_tid_t tid = COAP_INVALID_TID;

    OC_LOG(INFO, TAG, PCF("Entering OCCoAPSendMessage"));

    printf ("Payload: %s\n", payload);
    OC_LOG_V(INFO, TAG, "OCStack payload: %s", payload);
    coap_insert(&optList, CreateNewOptionNode(COAP_OPTION_CONTENT_TYPE,
                coap_encode_var_bytes(tempBuf, COAP_MEDIATYPE_APPLICATION_JSON),
                tempBuf), OrderOptions);
    coap_insert(&optList, CreateNewOptionNode(COAP_OPTION_MAXAGE,
                coap_encode_var_bytes(tempBuf, 0x2ffff), tempBuf), OrderOptions);
    coap_insert(&optList, CreateNewOptionNode(COAP_OPTION_OBSERVE,
                coap_encode_var_bytes(tempBuf, seqNum), tempBuf), OrderOptions);

    pdu = GenerateCoAPPdu (coapMsgType, OCToCoAPResponseCode(msgCode),
                           coap_new_message_id(gCoAPCtx), token->tokenLength, token->token,
                           (unsigned char*) payload, optList);
    VERIFY_NON_NULL(pdu);
    coap_show_pdu(pdu);

    tid = coap_send(gCoAPCtx, (coap_address_t*)dstAddr, pdu);
    OC_LOG_V(INFO, TAG, "TID %d", tid);
    if (pdu->hdr->type != COAP_MESSAGE_CON || tid == COAP_INVALID_TID)
    {
        OC_LOG(INFO, TAG, "Deleting PDU");
        coap_delete_pdu(pdu);
        pdu = NULL;
    }
    return OC_COAP_OK;

exit:
    coap_delete_list(optList);
    return OC_COAP_ERR;
}

/**
 * Stop the CoAP client or server processing
 *
 * @return 0 - success, else - TBD error
 */
int OCStopCoAP() {
    OC_LOG(INFO, TAG, PCF("Entering OCStopCoAP"));
    coap_free_context(gCoAPCtx);
    gCoAPCtx = NULL;
    return 0;
}

/**
 * Called in main loop of CoAP client or server.  Allows low-level CoAP processing of
 * send, receive, timeout, discovery, callbacks, etc.
 *
 * @return 0 - success, else - TBD error
 */
int OCProcessCoAP() {
    OC_LOG(INFO, TAG, PCF("Entering OCProcessCoAP"));

    coap_read(gCoAPCtx, gCoAPCtx->sockfd);
    if (-1 != gCoAPCtx->sockfd_wellknown) {
        coap_read(gCoAPCtx, gCoAPCtx->sockfd_wellknown);
    }
    coap_dispatch(gCoAPCtx);
    return 0;
}


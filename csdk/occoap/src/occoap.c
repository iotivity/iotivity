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
#include <coap.h>
#include <unistd.h>
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

//generate a coap message
static coap_pdu_t *
GenerateCoAPPdu(uint8_t msgType, uint8_t code, unsigned short id,
        size_t tokenLength, uint8_t * token, coap_list_t *options) {
    coap_pdu_t *pdu;
    coap_list_t *opt;

    pdu = coap_pdu_init(msgType, code, id, COAP_MAX_PDU_SIZE);
    VERIFY_NON_NULL(pdu);

    pdu->hdr->token_length = tokenLength;
    if (!coap_add_token(pdu, tokenLength, token)) {
        OC_LOG(FATAL, TAG, "coap_add_token failed");
    }

    // display the pdu for debugging purposes
    coap_show_pdu(pdu);

    for (opt = options; opt; opt = opt->next) {
        coap_add_option(pdu, COAP_OPTION_KEY(*(coap_option *)opt->data),
                COAP_OPTION_LENGTH(*(coap_option *)opt->data),
                COAP_OPTION_DATA(*(coap_option *)opt->data));
    }
    return pdu;

    exit: return NULL;
}

//a function to help in ordering coap options
static int OrderOptions(void *a, void *b) {
    if (!a || !b) {
        return a < b ? -1 : 1;
    }

    if (COAP_OPTION_KEY(*(coap_option *)a)
            < COAP_OPTION_KEY(*(coap_option *)b) ) {
        return -1;
    }

    return COAP_OPTION_KEY(*(coap_option *)a)
            == COAP_OPTION_KEY(*(coap_option *)b) ;
}

//a function to create a coap option
static coap_list_t *
CreateNewOptionNode(unsigned short key, unsigned int length,
        unsigned char *data) {
    coap_option *option;
    coap_list_t *node;

    option = coap_malloc(sizeof(coap_option) + length);
    if (!option) {
        goto exit;
    }

    COAP_OPTION_KEY(*option) = key;
    COAP_OPTION_LENGTH(*option) = length;
    VERIFY_NON_NULL(data);
    memcpy(COAP_OPTION_DATA(*option), data, length);

    /* we can pass NULL here as delete function since option is released automatically  */
    node = coap_new_listnode(option, NULL);

    if (node) {
        return node;
    }

    exit: OC_LOG(ERROR,TAG,"new_option_node: malloc: was not created");
    coap_free(option);
    return NULL;
}

//This function is called back by libcoap when a response is received
static void HandleCoAPRequests(struct coap_context_t *ctx,
        const coap_queue_t * rcvdRequest) {

    // silence warnings
    (void) ctx;
    // fill OCRequest structure
    OCRequest * request;
    OCEntityHandlerRequest * entityHandlerRequest;
    unsigned char bufReq[MAX_REQUEST_LENGTH] = { 0 };
    unsigned char bufRes[MAX_RESPONSE_LENGTH] = { 0 };
    size_t bufLen = 0;
    OCToken * rcvdToken;
    coap_opt_iterator_t opt_iter;
    coap_opt_filter_t filter;
    coap_opt_t *option;
    unsigned char rcvdUri[MAX_URI_LENGTH] = { 0 };
    unsigned char rcvdQuery[MAX_QUERY_LENGTH] = { 0 };
    OCStackResult result;
    coap_list_t *optList = NULL;
    unsigned char tempBuf[BUF_SIZE_ENCODE_OPTION];
    coap_pdu_t *pdu;
    coap_tid_t tid = COAP_INVALID_TID;

    //TODO:will be deleted at the end
    entityHandlerRequest = (OCEntityHandlerRequest *) malloc(
            sizeof(OCEntityHandlerRequest));

    entityHandlerRequest->method = OC_REST_NOMETHOD;
    if (rcvdRequest->pdu->hdr->code == COAP_REQUEST_GET) {
        entityHandlerRequest->method = OC_REST_GET;
    } else {
        OC_LOG(DEBUG, TAG, "!!!!!!!!!!!!!!!!!!!!!!!method not supported in HandleCoAPRequests");
    }

    coap_get_data(rcvdRequest->pdu, &bufLen, (unsigned char **)&bufReq);
    bufReq[bufLen] = 0;
    OC_LOG_V(INFO, TAG, "received payload %d bytes : %s", bufLen, bufReq);
    entityHandlerRequest->reqJSONPayload = bufReq;

    entityHandlerRequest->resJSONPayload = bufRes;
    entityHandlerRequest->resJSONPayloadLen = MAX_RESPONSE_LENGTH;

    //TODO:will be deleted at the end
    rcvdToken = (OCToken *) malloc(sizeof(OCToken));
    rcvdToken->tokenLength = rcvdRequest->pdu->hdr->token_length;
    memcpy(rcvdToken->token, rcvdRequest->pdu->hdr->token,
            rcvdToken->tokenLength);

    OC_LOG_V(INFO,TAG,"Token received %d bytes..........%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",
            rcvdToken->tokenLength,rcvdToken->token[0],rcvdToken->token[1],rcvdToken->token[2],rcvdToken->token[3],
            rcvdToken->token[4],rcvdToken->token[5],rcvdToken->token[6],rcvdToken->token[7]);
    entityHandlerRequest->token = rcvdToken;

    //TODO:will be deleted at the end
    request = (OCRequest *) malloc(sizeof(OCRequest));
    request->entityHandlerRequest = entityHandlerRequest;

    request->qos = OC_NON_CONFIRMABLE;
    if (rcvdRequest->pdu->hdr->type == COAP_MESSAGE_CON) {
        request->qos = OC_CONFIRMABLE;
    }

    //getting the uri
    request->resourceUrl = NULL;
    bufLen = 0;
    coap_option_filter_clear(filter);
    coap_option_setb(filter, COAP_OPTION_URI_PATH);
    coap_option_iterator_init(rcvdRequest->pdu, &opt_iter, filter);
    while ((option = coap_option_next(&opt_iter))) {
        rcvdUri[bufLen++] = '/';
        memcpy(rcvdUri + bufLen, COAP_OPT_VALUE(option),
                COAP_OPT_LENGTH(option));
        bufLen += COAP_OPT_LENGTH(option);
    }
    rcvdUri[bufLen] = '\0';
    OC_LOG_V(INFO, TAG, " Receveid uri %s", rcvdUri);
    request->resourceUrl = rcvdUri;

    request->query = NULL;
    bufLen = 0;
    coap_option_filter_clear(filter);
    coap_option_setb(filter, COAP_OPTION_URI_QUERY);
    coap_option_iterator_init(rcvdRequest->pdu, &opt_iter, filter);
    while ((option = coap_option_next(&opt_iter))) {
        memcpy(rcvdQuery + bufLen, COAP_OPT_VALUE(option),
                COAP_OPT_LENGTH(option));
        bufLen += COAP_OPT_LENGTH(option);
        rcvdQuery[bufLen++] = '&';
    }
    // delete last '&'
    rcvdQuery[bufLen - 1] = '\0';
    OC_LOG_V(INFO, TAG, " Receveid query %s", rcvdQuery);
    request->query = rcvdQuery;

    result = OCStackHandleReceiveRequest(request);

    if (result == OC_STACK_OK) {
        OC_LOG_V(INFO, TAG, "Response from ocstack: %s", request->entityHandlerRequest->resJSONPayload);
        // need to build the response PDU
        coap_insert(&optList,
                CreateNewOptionNode(COAP_OPTION_CONTENT_TYPE,
                        coap_encode_var_bytes(tempBuf,
                                COAP_MEDIATYPE_APPLICATION_JSON), tempBuf),
                OrderOptions);
        coap_insert(&optList,
                CreateNewOptionNode(COAP_OPTION_MAXAGE,
                        coap_encode_var_bytes(tempBuf, 0x2ffff), tempBuf),
                OrderOptions);

        // generate the pdu, if the request was CON, then the response is ACK, otherwire NON
        pdu = GenerateCoAPPdu(
                rcvdRequest->pdu->hdr->type == COAP_MESSAGE_CON ?
                        COAP_MESSAGE_ACK : COAP_MESSAGE_NON,
                COAP_RESPONSE_CODE(205), rcvdRequest->pdu->hdr->id,
                rcvdToken->tokenLength, rcvdToken->token, optList);
        coap_add_data(pdu,
                strlen((const char *)request->entityHandlerRequest->resJSONPayload),
                (unsigned char*) request->entityHandlerRequest->resJSONPayload);

        if (pdu->hdr->type != COAP_MESSAGE_NON
                || (pdu->hdr->code >= 64 && !coap_is_mcast(&rcvdRequest->local))) {
            tid = coap_send(gCoAPCtx, &rcvdRequest->remote, pdu);
        }OC_LOG_V(INFO, TAG, "TID %d", tid);OC_LOG(INFO, TAG, "Deleting PDU");
        // unlike stock libcoap (deletion in handle_request in net.c), we are deleting the response here
        // in the future, the response might be queued for SLOW resources
        coap_delete_pdu(pdu);

        coap_delete_list(optList);

        OC_LOG(INFO, TAG, "Done Cleaning up");
    } else {
        OC_LOG(DEBUG, TAG, "We not supporting other returns of OCServerRequestResult in HandleCoAPRequests/"
                " ......................yet");
    }
    free(rcvdToken);
    free(entityHandlerRequest);
    free(request);
}

//This function is called back by libcoap when a response is received
static void HandleCoAPResponses(struct coap_context_t *ctx,
        const coap_queue_t * rcvdResponse) {
    OCResponse * response;
    OCToken * token;
    OCClientResponse * clientResponse;
    unsigned char bufRes[MAX_RESPONSE_LENGTH] = { 0 };
    unsigned char * pRes = NULL;
    size_t bufLen = 0;

    VERIFY_NON_NULL(ctx);
    VERIFY_NON_NULL(rcvdResponse);

    // TODO: we should check if we are interested in the token
    // Now, just accept NON packets

    if (rcvdResponse->pdu->hdr->type == COAP_MESSAGE_NON) {
        // These will be deleted at the end
        response = (OCResponse *) malloc(sizeof(OCResponse));
        token = (OCToken *) malloc(sizeof(OCToken));
        clientResponse = (OCClientResponse *) malloc(sizeof(OCClientResponse));

        token->tokenLength = rcvdResponse->pdu->hdr->token_length;
        memcpy(token->token, rcvdResponse->pdu->hdr->token, token->tokenLength);

        clientResponse->addr = (OCDevAddr *)&(rcvdResponse->remote);

        coap_get_data(rcvdResponse->pdu, &bufLen, &pRes);
        memcpy(bufRes, pRes, bufLen);
        bufRes[bufLen] = 0;
        clientResponse->resJSONPayload = bufRes;
        OC_LOG_V(INFO, TAG, "Received a response HandleCoAPResponses in occoap: %s ", bufRes);

        clientResponse->result = OC_STACK_ERROR;

        response->token = token;
        response->clientResponse = clientResponse;

        if (rcvdResponse->pdu->hdr->code == COAP_RESPONSE_CODE(205)) {
            response->clientResponse->result = OC_STACK_OK;
            OCStackHandleReceiveResponse(response);
        } else {
            OC_LOG(DEBUG, TAG, "No other response codes are supported in HandleCoAPResponsesooooooooooooooooooooooo");
        }

        free(response);
        free(token);
        free(clientResponse);
    } else {
        OC_LOG(DEBUG, TAG, "Do not accept other than NON in HandleCoAPResponsesooooooooooooooooooooooo");
    }
#if 0
    size_t len;
    unsigned char *dataBuf;
    const void *addrPtr = NULL;
    char buf[BUF_SIZE];
    uint8_t remoteIpAddr[4];
    uint16_t remotePortNu;

    VERIFY_NON_NULL(received);
    VERIFY_NON_NULL(ctx);
    VERIFY_NON_NULL(remote);

    /* check if this is a response to our original request */
    if (!CheckToken(received)) {
        return;
    }

    switch (received->hdr->type) {
        case COAP_MESSAGE_CON:
        /* acknowledge received response if confirmable (TODO: check Token) */
        coap_send_ack(ctx, remote, received);
        break;
        case COAP_MESSAGE_RST:
        return;
        default:
        ;
    }

    if (received->hdr->code == COAP_RESPONSE_CODE(205)) {
        coap_get_data(received, &len, &dataBuf);
        OC_LOG_V(INFO, TAG, "This is a success COAP_RESPONSE_CODE(205):Payload data: %s", dataBuf);
    }

// TODO: we should call OCHandleClientReceiveResponse
    /*if (storedOCStackCB) {
     OCDevAddrToIPv4Addr((OCDevAddr *) remote, remoteIpAddr, remoteIpAddr + 1, remoteIpAddr + 2, remoteIpAddr + 3);
     OCDevAddrToPort((OCDevAddr *) remote, &remotePortNu);
     sprintf(buf, "%d.%d.%d.%d:%d", remoteIpAddr[0], remoteIpAddr[1], remoteIpAddr[2], remoteIpAddr[3], remotePortNu);
     OC_LOG_V(INFO, TAG, "Calling storedOCStackCB function with remote address %s",buf);
     storedOCStackCB(buf);
     }*/

#endif
    exit: return;
}

static void parseIPv4Address(const unsigned char * ipAddrStr, uint8_t * ipAddr) {
    size_t index = 0;

    while (*ipAddrStr) {
        if (isdigit((unsigned char) *ipAddrStr)) {
            ipAddr[index] *= 10;
            ipAddr[index] += *ipAddrStr - '0';
        } else if ((unsigned char) *ipAddrStr == '.') {
            index++;
        } else {
            return;
        }
        ipAddrStr++;
    }
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
    OCDevAddr devAddr, mcastAddr;
    uint8_t ipAddr[4] = { 0 };

    OC_LOG(INFO, TAG, PCF("Entering OCInitCoAP"));

    coap_set_log_level(log_level);

    if (address) {
        parseIPv4Address((const unsigned char *) address, ipAddr);
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

    exit: return ret;
}

/**
 * Discover OC resources
 *
 * @param method          - method to perform on the resource
 * @param Uri             - URI of the resource to interact with
 * @param asyncReturnFunc - asynchronous callback function that is invoked
 *                          by the stack when discovery or resource interaction is complete
 * @return
 *   0   - success
 *   TBD - TBD error
 */
int OCDoCoAPResource(OCMethod method, OCQualityOfService qos, OCToken * token,
        const char *Uri) {

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

    OC_LOG(INFO, TAG, PCF("Entering OCDoCoAPResource"));

    if (Uri) {
        OC_LOG_V(INFO, TAG, "URI = %s", Uri);
    }

    VERIFY_SUCCESS(coap_split_uri((unsigned char * )Uri, strlen(Uri), &uri), 0);

// Generate the destination address
    if (uri.host.length) {
        parseIPv4Address(uri.host.s, ipAddr);
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

    OC_LOG_V(DEBUG, TAG, "uri.host.s %s", uri.host.s);OC_LOG_V(DEBUG, TAG, "uri.path.s %s", uri.path.s);OC_LOG_V(DEBUG, TAG, "uri.port %d", uri.port);OC_LOG_V(DEBUG, TAG, "uri.query.s %s", uri.query.s);

    coapMsgType = COAP_MESSAGE_NON;

    // Decide message type
    if (qos == OC_CONFIRMABLE) {
        coapMsgType = COAP_MESSAGE_CON;
        OC_LOG(FATAL, TAG, "qos == OC_CONFIRMABLE is not supported in OCDoCoAPResource");
    }

    // Decide method type
    if (method == OC_REST_GET) {
        coapMethod = COAP_REQUEST_GET;
    } else {
        OC_LOG(FATAL, TAG, "OCDoCoAPResource only supports method == OC_REST_GET");
    }

    VERIFY_NON_NULL(gCoAPCtx);
    pdu = GenerateCoAPPdu(coapMsgType, coapMethod,
            coap_new_message_id(gCoAPCtx), token->tokenLength, token->token,
            optList);
    VERIFY_NON_NULL(pdu);

    TODO ("Post Sprint 1 -- Send Confirmed requests for non-discovery requests");
    tid = coap_send(gCoAPCtx, (coap_address_t*) &dst, pdu);

    OC_LOG_V(INFO, TAG, "TID %d", tid);
    if (pdu->hdr->type != COAP_MESSAGE_CON || tid == COAP_INVALID_TID) {
        OC_LOG(INFO, TAG, "Deleting PDU");
        coap_delete_pdu(pdu);
        OC_LOG(INFO, TAG, "Done Deleting PDU");
    } else {
        OC_LOG(INFO, TAG, "Keeping PDU, we should handle the retry of this pdu");
    }

    coap_delete_list(optList);

    ret = OC_COAP_OK;

    exit: return ret;
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


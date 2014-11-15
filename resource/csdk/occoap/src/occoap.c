//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
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
#include "ocstackconfig.h"
#include "occlientcb.h"
#include "ocobserve.h"
#include "logger.h"
#include "ocmalloc.h"
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

//=============================================================================
// Private Variables
//=============================================================================

static uint8_t coapWKIpAddr[] = { 224, 0, 1, 187 };
static coap_context_t *gCoAPCtx = NULL;

//=============================================================================
// Helper Functions
//=============================================================================

//generate a coap token
void OCGenerateCoAPToken(OCCoAPToken * token)
{
    if (token)
    {
        token->tokenLength = MAX_TOKEN_LENGTH;
        OCFillRandomMem((uint8_t*)token->token, token->tokenLength);
    }
}

//This function is called back by libcoap when ack or rst are received
static void HandleCoAPAckRst(struct coap_context_t * ctx, uint8_t msgType,
        const coap_queue_t * sentQueue){

    // silence warnings
    (void) ctx;

    OCStackResult result = OC_STACK_OK;
    OCCoAPToken sentToken;
    uint8_t * observeOption = NULL;
    coap_pdu_t * sentPdu = sentQueue->pdu;

    // fill the buffers of Uri and Query
    result = ParseCoAPPdu(sentPdu, NULL, NULL, &observeOption, NULL, NULL, NULL, NULL);
    VERIFY_SUCCESS(result, OC_STACK_OK);

    // fill OCCoAPToken structure
    RetrieveOCCoAPToken(sentPdu, &sentToken);

    if(msgType == COAP_MESSAGE_RST){
        // now the observer should be deleted
        if(myStackMode != OC_CLIENT)
        {
            result = OCObserverStatus(&sentToken, OC_OBSERVER_NOT_INTERESTED);
            if(result == OC_STACK_OK){
                OC_LOG_V(DEBUG, TAG,
                        "Received RST, removing all queues associated with Token %d bytes",
                        sentToken.tokenLength);
                OC_LOG_BUFFER(INFO, TAG, sentToken.token, sentToken.tokenLength);
                coap_cancel_all_messages(ctx, &sentQueue->remote, sentToken.token,
                        sentToken.tokenLength);
            }
        }
    }else if(observeOption && msgType == COAP_MESSAGE_ACK){
        OC_LOG_V(DEBUG, TAG, "Received ACK, for Token %d bytes",sentToken.tokenLength);
        OC_LOG_BUFFER(INFO, TAG, sentToken.token, sentToken.tokenLength);
        // now the observer is still interested
        if(myStackMode != OC_CLIENT)
        {
            OCObserverStatus(&sentToken, OC_OBSERVER_STILL_INTERESTED);
        }
    }

    exit:
        OCFree(observeOption);
}

//This function is called back by libcoap when a request is received
static void HandleCoAPRequests(struct coap_context_t *ctx,
        const coap_queue_t * rcvdRequest)
{
    // silence warnings
    (void) ctx;

    if(myStackMode == OC_CLIENT)
    {
        //TODO: should the client be responding to requests?
        return;
    }

    OCStackResult result = OC_STACK_ERROR;
    OCStackResult responseResult = OC_STACK_ERROR;
    OCRequest * request = NULL;
    OCEntityHandlerRequest entityHandlerRequest;
    OCCoAPToken rcvdToken;
    OCObserveReq * rcvdObsReq = NULL;
    coap_pdu_t * sendPdu = NULL;
    coap_list_t *optList = NULL;
    uint8_t mediaType = COAP_MEDIATYPE_APPLICATION_JSON;
    uint32_t maxAge = 0x2ffff;
    OCMethod ocMethod;

    unsigned char rcvdUri[MAX_URI_LENGTH] = { 0 };
    unsigned char rcvdQuery[MAX_QUERY_LENGTH] = { 0 };
    unsigned char bufRes[MAX_RESPONSE_LENGTH] = { 0 };
    unsigned char newResourceUri[MAX_RESPONSE_LENGTH] = { 0 };
    uint8_t * rcvObserveOption = NULL;
    unsigned char * bufReqPayload = NULL;
    uint32_t observeOption = OC_RESOURCE_NO_OBSERVE;
    coap_send_flags_t sendFlag;
    memset(&entityHandlerRequest, 0, sizeof(OCEntityHandlerRequest));

    coap_pdu_t * recvPdu = rcvdRequest->pdu;

    // fill the buffers of Uri and Query
    result = ParseCoAPPdu(recvPdu, rcvdUri, rcvdQuery, &rcvObserveOption, NULL, &bufReqPayload,
            entityHandlerRequest.rcvdVendorSpecificHeaderOptions,
            &(entityHandlerRequest.numRcvdVendorSpecificHeaderOptions));
    VERIFY_SUCCESS(result, OC_STACK_OK);
    if(rcvObserveOption){
        observeOption = (uint32_t)(*rcvObserveOption);
    }

    // fill OCCoAPToken structure
    RetrieveOCCoAPToken(recvPdu, &rcvdToken);

    switch (recvPdu->hdr->code)
    {
        case COAP_REQUEST_GET:
            {
                ocMethod = OC_REST_GET;
                break;
            }
        case COAP_REQUEST_POST:
            {
                ocMethod = OC_REST_POST;
                break;
            }
        case COAP_REQUEST_DELETE:
            {
                ocMethod = OC_REST_DELETE;
                break;
            }
        case COAP_REQUEST_PUT:
            {
                ocMethod = OC_REST_PUT;
                break;
            }
        default:
            {
                OC_LOG_V(ERROR, TAG, "Received CoAP method %d not supported",
                         recvPdu->hdr->code);
                goto exit;
            }
    }

    // fill OCEntityHandlerRequest structure
    result = FormOCEntityHandlerRequest(&entityHandlerRequest, ocMethod,
                                        bufRes, bufReqPayload, rcvdQuery, newResourceUri);
    VERIFY_SUCCESS(result, OC_STACK_OK);

   // fill OCObserveReq
   result = FormOCObserveReq(&rcvdObsReq, observeOption,
           (OCDevAddr *)&(rcvdRequest->remote), &rcvdToken);
   VERIFY_SUCCESS(result, OC_STACK_OK);

    // fill OCRequest structure
    result = FormOCRequest(&request, (recvPdu->hdr->type == COAP_MESSAGE_CON) ?
            OC_HIGH_QOS : OC_LOW_QOS, rcvdUri, rcvdObsReq, &entityHandlerRequest,
            rcvdRequest->secure);
    VERIFY_SUCCESS(result, OC_STACK_OK);

    OC_LOG_V(INFO, TAG, " Receveid uri:     %s", request->resourceUrl);
    OC_LOG_V(INFO, TAG, " Receveid query:   %s", entityHandlerRequest.query);
    OC_LOG_V(INFO, TAG, " Receveid payload: %s",
            request->entityHandlerRequest->reqJSONPayload);
    OC_LOG_V(INFO, TAG, " Token received %d bytes",
            rcvdToken.tokenLength);
    OC_LOG_BUFFER(INFO, TAG, rcvdToken.token, rcvdToken.tokenLength);

    // process the request
    responseResult = HandleStackRequests(request);
    #ifdef WITH_PRESENCE
    if(responseResult == OC_STACK_PRESENCE_DO_NOT_HANDLE)
    {
        goto exit;
    }
    #endif

    // do not process further if received an error
    // ex : when receive a non-secure request to a secure resource
    if(responseResult == OC_STACK_ERROR)
    {
        goto exit;
    }

    OC_LOG_V(INFO, TAG, "Response from ocstack: %s",
            request->entityHandlerRequest->resJSONPayload);

    if(rcvdObsReq)
    {
        switch(rcvdObsReq->result)
        {
        case OC_STACK_OK:
            observeOption = rcvdObsReq->option;
            result = FormOptionList(&optList, &mediaType, &maxAge,
                    sizeof(observeOption), &observeOption,
                    NULL, 0, NULL, 0, NULL,
                    request->entityHandlerRequest->sendVendorSpecificHeaderOptions,
                    request->entityHandlerRequest->numSendVendorSpecificHeaderOptions);
            break;
        case OC_STACK_OBSERVER_NOT_ADDED:
        case OC_STACK_OBSERVER_NOT_REMOVED:
        case OC_STACK_INVALID_OBSERVE_PARAM:
        default:
            result = FormOptionList(&optList, &mediaType, &maxAge, 0, NULL, NULL,
                    0, NULL, 0, NULL,
                    request->entityHandlerRequest->sendVendorSpecificHeaderOptions,
                    request->entityHandlerRequest->numSendVendorSpecificHeaderOptions);
            break;
        }
    }
    else
    {
        if (responseResult == OC_STACK_RESOURCE_CREATED)
        {
            result = FormOptionList(&optList, &mediaType, &maxAge, 0, NULL, NULL,
                    strlen((char *)newResourceUri), newResourceUri, 0, NULL,
                    request->entityHandlerRequest->sendVendorSpecificHeaderOptions,
                    request->entityHandlerRequest->numSendVendorSpecificHeaderOptions);
        }
        else
        {
            result = FormOptionList(&optList, &mediaType, &maxAge, 0, NULL, NULL,
                    0, NULL, 0, NULL,
                    request->entityHandlerRequest->sendVendorSpecificHeaderOptions,
                    request->entityHandlerRequest->numSendVendorSpecificHeaderOptions);
        }
    }

    VERIFY_SUCCESS(result, OC_STACK_OK);

    // generate the pdu, if the request was CON, then the response is ACK, otherwire NON
    sendPdu = GenerateCoAPPdu(
            (rcvdRequest->pdu->hdr->type == COAP_MESSAGE_CON) ?
                    COAP_MESSAGE_ACK : COAP_MESSAGE_NON,
                    OCToCoAPResponseCode(responseResult), rcvdRequest->pdu->hdr->id,
                    &rcvdToken,
                    request->entityHandlerRequest->resJSONPayload, optList);
    VERIFY_NON_NULL(sendPdu);
    coap_show_pdu(sendPdu);

    sendFlag = (coap_send_flags_t)(rcvdRequest->delayedResponse ? SEND_DELAYED : 0);
    sendFlag = (coap_send_flags_t)( sendFlag | (rcvdRequest->secure ? SEND_SECURE_PORT : 0));

    if(SendCoAPPdu(gCoAPCtx, (coap_address_t*) &(rcvdRequest->remote), sendPdu,
         sendFlag)
            != OC_STACK_OK){
        OC_LOG(DEBUG, TAG, PCF("A problem occurred in sending a pdu"));
    }

exit:
    OCFree(rcvObserveOption);
    OCFree(rcvdObsReq);
    OCFree(request);
}

uint32_t GetTime(float afterSeconds)
{
    coap_tick_t now;
    coap_ticks(&now);
    return now + (uint32_t)(afterSeconds * COAP_TICKS_PER_SECOND);
}

//This function is called back by libcoap when a response is received
static void HandleCoAPResponses(struct coap_context_t *ctx,
        const coap_queue_t * rcvdResponse) {
    uint8_t haltResponse = 0;
    OCResponse * response = NULL;
    OCCoAPToken rcvdToken;
    OCClientResponse clientResponse;
    ClientCB * cbNode = NULL;
    unsigned char * bufRes = NULL;
    uint8_t * rcvObserveOption = NULL;
    uint8_t * rcvMaxAgeOption = NULL;
    uint32_t sequenceNumber = OC_RESOURCE_NO_OBSERVE;
    uint32_t maxAge = 0;
    char * resourceTypeName = NULL;
    OCStackResult result = OC_STACK_ERROR;
    coap_pdu_t *sendPdu = NULL;
    coap_pdu_t * recvPdu = NULL;
    uint8_t remoteIpAddr[4];
    uint16_t remotePortNu;
    unsigned char fullUri[MAX_URI_LENGTH] = { 0 };
    unsigned char rcvdUri[MAX_URI_LENGTH] = { 0 };
    uint8_t isObserveNotification = 0;
    #ifdef WITH_PRESENCE
    uint8_t isPresenceNotification = 0;
    uint8_t isMulticastPresence = 0;
    uint32_t lowerBound;
    uint32_t higherBound;
    char * tok = NULL;
    #endif
    memset(&clientResponse, 0, sizeof(OCClientResponse));

    VERIFY_NON_NULL(ctx);
    VERIFY_NON_NULL(rcvdResponse);
    recvPdu = rcvdResponse->pdu;

    result = ParseCoAPPdu(recvPdu, rcvdUri, NULL, &rcvObserveOption, &rcvMaxAgeOption, &bufRes,
            clientResponse.rcvdVendorSpecificHeaderOptions,
            &(clientResponse.numRcvdVendorSpecificHeaderOptions));
    VERIFY_SUCCESS(result, OC_STACK_OK);

    if(rcvObserveOption){
        sequenceNumber = *((uint32_t *) rcvObserveOption);
    }

    if(rcvMaxAgeOption){
        maxAge = *((uint32_t *) rcvMaxAgeOption);
    }

    OC_LOG_V(DEBUG, TAG, "The sequenceNumber/NONCE of this response %u", sequenceNumber);
    OC_LOG_V(DEBUG, TAG, "The maxAge/TTL of this response %u", maxAge);
    OC_LOG_V(DEBUG, TAG, "The response received is %s", bufRes);

    if(sequenceNumber >= OC_OFFSET_SEQUENCE_NUMBER)
    {
        isObserveNotification = 1;
        OC_LOG(INFO, TAG, PCF("Received an observe notification"));
    }

    #ifdef WITH_PRESENCE
    if(!strcmp((char *)rcvdUri, (char *)OC_PRESENCE_URI)){
        isPresenceNotification = 1;
        OC_LOG(INFO, TAG, PCF("Received a presence notification"));
        tok = strtok((char *)bufRes, ":");
        sequenceNumber = (uint32_t )atoi(tok);
        OC_LOG_V(DEBUG, TAG, "The received NONCE is %u", sequenceNumber);
        tok = strtok(NULL, ":");
        maxAge = (uint32_t )atoi(tok);
        OC_LOG_V(DEBUG, TAG, "The received TTL is %u", maxAge);
        tok = strtok(NULL, ":");
        bufRes[strlen((char *)bufRes)] = ':';
        if(tok) {
            resourceTypeName = (char *)OCMalloc(strlen(tok));
            if(!resourceTypeName)
            {
                goto exit;
            }
            strcpy(resourceTypeName, tok);
            bufRes[strlen((char *)bufRes)] = ':';
            OC_LOG_V(DEBUG, TAG, "----------------resourceTypeName %s",
                    resourceTypeName);
        }
    }
    #endif

    // fill OCCoAPToken structure
    RetrieveOCCoAPToken(recvPdu, &rcvdToken);
    OC_LOG_V(INFO, TAG,"Received a pdu with Token", rcvdToken.tokenLength);
    OC_LOG_BUFFER(INFO, TAG, rcvdToken.token, rcvdToken.tokenLength);

    // fill OCClientResponse structure
    result = FormOCClientResponse(&clientResponse, CoAPToOCResponseCode(recvPdu->hdr->code),
            (OCDevAddr *) &(rcvdResponse->remote), sequenceNumber, bufRes);
    VERIFY_SUCCESS(result, OC_STACK_OK);

    cbNode = GetClientCB(&rcvdToken, NULL, NULL);

    #ifdef WITH_PRESENCE
    // Check if the application subcribed for presence
    if(!cbNode)
    {
        // get the address of the remote
        OCDevAddrToIPv4Addr((OCDevAddr *) &(rcvdResponse->remote), remoteIpAddr,
                remoteIpAddr + 1, remoteIpAddr + 2, remoteIpAddr + 3);
        OCDevAddrToPort((OCDevAddr *) &(rcvdResponse->remote), &remotePortNu);
        sprintf((char *)fullUri, "coap://%d.%d.%d.%d:%d%s",
                remoteIpAddr[0],remoteIpAddr[1],remoteIpAddr[2],remoteIpAddr[3],
                remotePortNu,rcvdUri);
        cbNode = GetClientCB(NULL, NULL, fullUri);
    }

    // Check if application subscribed for multicast presence
    if(!cbNode)
    {
        sprintf((char *)fullUri, "%s%s", OC_MULTICAST_IP, rcvdUri);
        cbNode = GetClientCB(NULL, NULL, fullUri);
        isMulticastPresence = 1;
    }
    #endif

    // fill OCResponse structure
    result = FormOCResponse(&response, cbNode, maxAge, &clientResponse);
    VERIFY_SUCCESS(result, OC_STACK_OK);

    if(cbNode)
    {
        if(isObserveNotification)
        {
            OC_LOG(INFO, TAG, PCF("Received an observe notification"));
            if(recvPdu->hdr->type == COAP_MESSAGE_CON)
            {
                sendPdu = GenerateCoAPPdu(COAP_MESSAGE_ACK, 0,
                        recvPdu->hdr->id, NULL, NULL, NULL);
                VERIFY_NON_NULL(sendPdu);
                result = SendCoAPPdu(gCoAPCtx, (coap_address_t*) &rcvdResponse->remote,
                        sendPdu,
                        (coap_send_flags_t)(rcvdResponse->secure ? SEND_SECURE_PORT : 0));
            }
            //TODO: check the standard for methods to detect wrap around condition
            if(cbNode->method == OC_REST_OBSERVE &&
                    (clientResponse.sequenceNumber <= cbNode->sequenceNumber ||
                            (clientResponse.sequenceNumber > cbNode->sequenceNumber &&
                                    clientResponse.sequenceNumber == MAX_SEQUENCE_NUMBER)))
            {
                OC_LOG_V(DEBUG, TAG, "Observe notification came out of order. \
                        Ignoring Incoming:%d  Against Current:%d.",
                        clientResponse.sequenceNumber, cbNode->sequenceNumber);
                goto exit;
            }
            if(clientResponse.sequenceNumber > cbNode->sequenceNumber){
                cbNode->sequenceNumber = clientResponse.sequenceNumber;
            }
        }
        else
        {
            #ifdef WITH_PRESENCE
            if(isPresenceNotification)
            {
                OC_LOG(INFO, TAG, PCF("Received a presence notification"));
                if(!cbNode->presence)
                {
                    cbNode->presence = (OCPresence *) OCMalloc(sizeof(OCPresence));
                    VERIFY_NON_NULL(cbNode->presence);
                    cbNode->presence->timeOut = NULL;
                    cbNode->presence->timeOut = (uint32_t *)
                            OCMalloc(PresenceTimeOutSize * sizeof(uint32_t));
                    if(!(cbNode->presence->timeOut)){
                        OCFree(cbNode->presence);
                        goto exit;
                    }
                }
                if(maxAge == 0)
                {
                    OC_LOG(INFO, TAG, "===============Stopping presence");
                    response->clientResponse->result = OC_STACK_PRESENCE_STOPPED;
                }
                else
                {
                    OC_LOG_V(INFO, TAG, "===============Update presence TTL, now time is %d", GetTime(0));
                    cbNode->presence->TTL = maxAge;
                    for(int index = 0; index < PresenceTimeOutSize; index++)
                    {
                        lowerBound = GetTime(((float)(PresenceTimeOut[index])
                                /(float)100)*(float)cbNode->presence->TTL);
                        higherBound = GetTime(((float)(PresenceTimeOut[index + 1])
                                /(float)100)*(float)cbNode->presence->TTL);
                        cbNode->presence->timeOut[index] = OCGetRandomRange(lowerBound, higherBound);
                        OC_LOG_V(DEBUG, TAG, "----------------lowerBound timeout  %d", lowerBound);
                        OC_LOG_V(DEBUG, TAG, "----------------higherBound timeout %d", higherBound);
                        OC_LOG_V(DEBUG, TAG, "----------------timeOut entry  %d", cbNode->presence->timeOut[index]);
                    }
                    cbNode->presence->TTLlevel = 0;
                    OC_LOG_V(DEBUG, TAG, "----------------this TTL level %d", cbNode->presence->TTLlevel);
                    if(cbNode->sequenceNumber == clientResponse.sequenceNumber)
                    {
                        OC_LOG(INFO, TAG, "===============No presence change");
                        goto exit;
                    }
                    OC_LOG(INFO, TAG, "===============Presence changed, calling up the stack");
                    cbNode->sequenceNumber = clientResponse.sequenceNumber;;
                }

                if(resourceTypeName && response->cbNode->filterResourceType)
                {
                    if(strcmp(resourceTypeName,
                            (const char *)response->cbNode->filterResourceType)!=0)
                    {
                        //Ignore presence callback if resource type does not match filter.
                        haltResponse = 1;
                    }
                }
            }
            else if(isMulticastPresence)
            {
                // Check if the same nonce for a given host
                OCMulticastNode* mcNode = NULL;
                unsigned char senderUri[MAX_URI_LENGTH] = { 0 };
                sprintf((char *)senderUri, "%d.%d.%d.%d:%d",
                    remoteIpAddr[0],remoteIpAddr[1],remoteIpAddr[2],remoteIpAddr[3],
                    remotePortNu);
                mcNode = GetMCPresenceNode(senderUri);

                if(mcNode != NULL)
                {
                    if(mcNode->nonce == clientResponse.sequenceNumber)
                    {
                        OC_LOG(INFO, TAG, PCF("===============No presence change (Multicast)"));
                        goto exit;
                    }
                    mcNode->nonce = clientResponse.sequenceNumber;
                }
                else
                {
                    uint32_t uriLen = strlen((char*)senderUri);
                    unsigned char* uri = (unsigned char *) OCMalloc(uriLen + 1);
                    if(uri)
                    {
                        memcpy(uri, senderUri, (uriLen + 1));
                    }
                    else
                    {
                        OC_LOG(INFO, TAG,
                            PCF("===============No Memory for URI to store in the presence node"));
                        goto exit;
                    }
                    result = AddMCPresenceNode(&mcNode, (unsigned char*) uri,
                                                clientResponse.sequenceNumber);
                    if(result == OC_STACK_NO_MEMORY)
                    {
                        OC_LOG(INFO, TAG,
                            PCF("===============No Memory for Multicast Presence Node"));
                        goto exit;
                    }
                }
            }
            #endif
        }
        if(!haltResponse)
        {
            HandleStackResponses(response);
        }
    }
    else if(!cbNode && isObserveNotification)
    {
        OC_LOG(INFO, TAG, PCF("Received an observe notification, but I do not have callback \
                 ------------ sending RESET"));
        sendPdu = GenerateCoAPPdu(COAP_MESSAGE_RST, 0,
                recvPdu->hdr->id, NULL, NULL, NULL);
        VERIFY_NON_NULL(sendPdu);
        result = SendCoAPPdu(gCoAPCtx, (coap_address_t*) &rcvdResponse->remote, sendPdu,
                     (coap_send_flags_t)(rcvdResponse->secure ? SEND_SECURE_PORT : 0));
        VERIFY_SUCCESS(result, OC_STACK_OK);
    }
    #ifdef WITH_PRESENCE
    else if(!cbNode && isPresenceNotification)
    {
        OC_LOG(INFO, TAG, PCF("Received a presence notification, but I do not have callback \
                     ------------ ignoring"));
    }
    #endif
    else
    {
        OC_LOG(INFO, TAG, PCF("Received a response, but I do not have callback. \
                 ------------ sending RESET"));
        sendPdu = GenerateCoAPPdu(COAP_MESSAGE_RST, 0,
                recvPdu->hdr->id, NULL, NULL, NULL);
        VERIFY_NON_NULL(sendPdu);
        result = SendCoAPPdu(gCoAPCtx, (coap_address_t*) &rcvdResponse->remote, sendPdu,
                    (coap_send_flags_t)(rcvdResponse->secure ? SEND_SECURE_PORT : 0));
        VERIFY_SUCCESS(result, OC_STACK_OK);
    }
    exit:
        OCFree(rcvObserveOption);
        OCFree(rcvMaxAgeOption);
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

    OCStackResult ret = OC_STACK_ERROR;

    TODO ("Below should go away and be replaced by OC_LOG");
    coap_log_t log_level = (coap_log_t)(LOG_DEBUG + 1);
    OCDevAddr mcastAddr;
    uint8_t ipAddr[4] = { 0 };
    uint16_t parsedPort = 0;

    OC_LOG(INFO, TAG, PCF("Entering OCInitCoAP"));

    coap_set_log_level(log_level);

    if (address)
    {
        if (!ParseIPv4Address((unsigned char *) address, ipAddr, &parsedPort))
        {
            ret = OC_STACK_ERROR;
            goto exit;
        }

        OC_LOG_V(INFO, TAG, "Parsed IP Address %d.%d.%d.%d",
                               ipAddr[0],ipAddr[1],ipAddr[2],ipAddr[3]);
    }

    gCoAPCtx = coap_new_context(ipAddr, port);
    VERIFY_NON_NULL(gCoAPCtx);

    // To allow presence notification work we need to init socket gCoAPCtx->sockfd_wellknown
    // for servers as well as clients
    OCBuildIPv4Address(coapWKIpAddr[0], coapWKIpAddr[1], coapWKIpAddr[2],
            coapWKIpAddr[3], COAP_DEFAULT_PORT, &mcastAddr);
    VERIFY_SUCCESS(
            coap_join_wellknown_group(gCoAPCtx,
                    (coap_address_t* )&mcastAddr), 0);

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
 * @param options         - The address of an array containing the vendor specific
 *                          header options to be sent with the request
 * @return
 *   0   - success
 *   TBD - TBD error
 */
OCStackResult OCDoCoAPResource(OCMethod method, OCQualityOfService qos, OCCoAPToken * token,
                     const char *Uri, const char *payload, OCHeaderOption * options, uint8_t numOptions)
{

    OCStackResult ret = OC_STACK_ERROR;
    coap_pdu_t *pdu = NULL;
    coap_uri_t uri;
    OCDevAddr dst;
    uint8_t ipAddr[4] = { 0 };
    uint16_t port = 0;
    coap_list_t *optList = NULL;
    uint8_t coapMsgType;
    uint8_t coapMethod;
    uint32_t observeOption;
    coap_send_flags_t flag = (coap_send_flags_t)0;

    OC_LOG(INFO, TAG, PCF("Entering OCDoCoAPResource"));

    if (Uri) {
        OC_LOG_V(INFO, TAG, "URI = %s", Uri);
        VERIFY_SUCCESS(coap_split_uri((unsigned char * )Uri, strlen(Uri), &uri), OC_STACK_OK);

        // Generate the destination address
        if (uri.host.length && ParseIPv4Address(uri.host.s, ipAddr, &port)) {
            OCBuildIPv4Address(ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3], uri.port,
                    &dst);
        } else {
            goto exit;
        }

        VERIFY_SUCCESS(FormOptionList(&optList, NULL, NULL, 0, NULL,
                (uint16_t*)&uri.port, uri.path.length, uri.path.s, uri.query.length,
                uri.query.s, options, numOptions), OC_STACK_OK);

        //TODO : Investigate the scenario where there will be no uri for OCDoCoAPResource
        flag = (coap_send_flags_t) (uri.secure ? SEND_SECURE_PORT : 0);
        OC_LOG_V(DEBUG, TAG, "uri.host.s %s", uri.host.s);
        OC_LOG_V(DEBUG, TAG, "uri.path.s %s", uri.path.s);
        OC_LOG_V(DEBUG, TAG, "uri.port %d", uri.port);
        OC_LOG_V(DEBUG, TAG, "uri.query.s %s", uri.query.s);
        OC_LOG_V(DEBUG, TAG, "secure uri %d", uri.secure);
    }

    coapMsgType = OCToCoAPQoS(qos);

    // Decide method type
    switch (method) {
        case OC_REST_GET:
        #ifdef WITH_PRESENCE
        case OC_REST_PRESENCE:
        #endif
            coapMethod = COAP_REQUEST_GET;
            break;
        case OC_REST_PUT:
            coapMethod = COAP_REQUEST_PUT;
            break;
        case OC_REST_POST:
            coapMethod = COAP_REQUEST_POST;
            break;
        case OC_REST_DELETE:
            coapMethod = COAP_REQUEST_DELETE;
            break;
        case OC_REST_OBSERVE_ALL:
        case OC_REST_OBSERVE:
        case OC_REST_CANCEL_OBSERVE:
            coapMethod = COAP_REQUEST_GET;
            observeOption = (method == OC_REST_CANCEL_OBSERVE)?
                    OC_RESOURCE_OBSERVE_DEREGISTER:OC_RESOURCE_OBSERVE_REGISTER;
            coap_insert(&optList, CreateNewOptionNode(COAP_OPTION_OBSERVE,
                        sizeof(observeOption), (uint8_t *)&observeOption), OrderOptions);
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

    ret = SendCoAPPdu(gCoAPCtx, (coap_address_t*) &dst, pdu, flag);

exit:
    if (ret!= OC_STACK_OK)
    {
        OC_LOG(DEBUG, TAG, PCF("A problem occurred in sending a pdu"));
    }
    return ret;
}

OCStackResult OCSendCoAPNotification (unsigned char * uri, OCDevAddr *dstAddr,
                       OCQualityOfService qos, OCCoAPToken * token,
                       unsigned char *payload, OCResource *resPtr, uint32_t maxAge)
{
    OCStackResult result = OC_STACK_ERROR;
    coap_list_t *optList = NULL;
    uint8_t coapMsgType = COAP_MESSAGE_NON;
    uint8_t mediaType = COAP_MEDIATYPE_APPLICATION_JSON;
    coap_pdu_t *sendPdu;

    OC_LOG(INFO, TAG, PCF("Entering OCSendCoAPNotification"));

    coapMsgType = OCToCoAPQoS(qos);

    #ifdef WITH_PRESENCE
    if(!strcmp((const char *)uri, OC_PRESENCE_URI))
    {
        result = FormOptionList(&optList, &mediaType, NULL, 0, NULL,
                NULL, strlen((char *)uri), uri, 0, NULL, NULL, 0);
    }
    else
    {
    #endif
        result = FormOptionList(&optList, &mediaType, &maxAge, sizeof(resPtr->sequenceNum),
                &resPtr->sequenceNum, NULL, strlen((char *)uri), uri, 0, NULL, NULL, 0);
    #ifdef WITH_PRESENCE
    }
    #endif
    VERIFY_SUCCESS(result, OC_STACK_OK);

    if(resPtr->resourceProperties == 0)
    {
        result = OC_STACK_RESOURCE_DELETED;
    }

    sendPdu = GenerateCoAPPdu(
            coapMsgType == COAP_MESSAGE_CON ? COAP_MESSAGE_CON : COAP_MESSAGE_NON,
                    OCToCoAPResponseCode(result), coap_new_message_id(gCoAPCtx),
                    token, payload, optList);
    VERIFY_NON_NULL(sendPdu);
    coap_show_pdu(sendPdu);

    // TODO : resourceProperties will determine if the packet will be send using secure port
    if(SendCoAPPdu(gCoAPCtx, (coap_address_t*) dstAddr, sendPdu ,
            (coap_send_flags_t)((resPtr->resourceProperties & OC_SECURE) ? SEND_SECURE_PORT : 0) )
            != OC_STACK_OK)
    {
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
    if(read > 0) {
        OC_LOG(INFO, TAG, PCF("This is a Unicast<============"));
    }
    if (-1 != gCoAPCtx->sockfd_wellknown) {
        read = coap_read(gCoAPCtx, gCoAPCtx->sockfd_wellknown);
        if(read > 0)
        {
            OC_LOG(INFO, TAG, PCF("This is a Multicast<==========="));
        }
    }
    if (-1 != gCoAPCtx->sockfd_dtls) {
        read = coap_read(gCoAPCtx, gCoAPCtx->sockfd_dtls);
        if(read > 0)
        {
            OC_LOG(INFO, TAG, PCF("This is a Secure packet<==========="));
        }
    }
    coap_dispatch(gCoAPCtx);

    HandleSendQueue(gCoAPCtx);

    return OC_STACK_OK;
}


/**
 * Retrieve the info about the end-point where resource is being hosted.
 * Currently, this method only provides the IP port with which the socket
 * is bound.
 *
 * @return 0 - success, else - TBD error
 */
OCStackResult OCGetResourceEndPointInfo (OCResource *resPtr, void *info) {

    OCStackResult result = OC_STACK_ERROR;
    int sfd;
    OC_LOG(INFO, TAG, PCF("Entering OCGetResourceEndPointInfo"));
    VERIFY_NON_NULL(resPtr);
    VERIFY_NON_NULL(info);

    sfd = (resPtr->resourceProperties & OC_SECURE) ? gCoAPCtx->sockfd_dtls :
            gCoAPCtx->sockfd;

    if (OCGetSocketInfo(sfd, (uint16_t*)info) == ERR_SUCCESS)
        result = OC_STACK_OK;
exit:
    return result;
}



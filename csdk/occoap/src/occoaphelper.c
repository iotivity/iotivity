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

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "occoaphelper.h"
#include "ocstackconfig.h"
#include "logger.h"
#include "ocobserve.h"
#include "coap_time.h"
#include "ocmalloc.h"

//-----------------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------------
#define TAG    PCF("OCCoAPHelper")
#define VERIFY_NON_NULL(arg) { if (!arg) {OC_LOG_V(FATAL, TAG, "%s is NULL", #arg); goto exit;} }

//=============================================================================
// Helper Functions
//=============================================================================

OCStackResult isVendorSpecific(uint16_t optionID)
{
    if(optionID >= COAP_VENDOR_OPT_START && optionID <= COAP_MAX_OPT)
    {
        return OC_STACK_OK;
    }
    return OC_STACK_INVALID_OPTION;
}

// Convert OCStack code to CoAP code
uint8_t OCToCoAPResponseCode(OCStackResult result)
{
    uint8_t ret;
    switch(result)
    {
        case OC_STACK_OK :
            ret = COAP_RESPONSE_200;
            break;

        case OC_STACK_RESOURCE_DELETED:
            ret = COAP_RESPONSE_202;
            break;

        case OC_STACK_INVALID_QUERY :
            ret = COAP_RESPONSE_400;
            break;

        case OC_STACK_NO_RESOURCE :
            ret = COAP_RESPONSE_404;
            break;

        case OC_STACK_INVALID_METHOD :
            ret = COAP_RESPONSE_405;
            break;

        default:
            ret = COAP_RESPONSE_500;
    }
    return ret;
}

uint8_t OCToCoAPQoS(OCQualityOfService qos)
{
    switch (qos)
    {
        case OC_HIGH_QOS:
            return COAP_MESSAGE_CON;
            break;
        case OC_MEDIUM_QOS:
        case OC_LOW_QOS:
        case OC_NA_QOS:
        default:
            return COAP_MESSAGE_NON;
            break;
    }
}
// Convert CoAP code to OCStack code
OCStackResult CoAPToOCResponseCode(uint8_t coapCode)
{
    OCStackResult ret;
    int decimal;
    switch(coapCode)
    {
        case COAP_RESPONSE_200 :
            ret = OC_STACK_OK;
            break;

        case COAP_RESPONSE_202 :
            ret = OC_STACK_RESOURCE_DELETED;
            break;

        case COAP_RESPONSE_400 :
            ret = OC_STACK_INVALID_QUERY;
            break;

        case COAP_RESPONSE_404 :
            ret = OC_STACK_NO_RESOURCE;
            break;

        case COAP_RESPONSE_405 :
            ret = OC_STACK_INVALID_METHOD;
            break;

        default:
            decimal = ((coapCode >> 5) * 100) + (coapCode & 31);
            if (decimal >= 200 && decimal <= 231)
            {
                ret = OC_STACK_OK;
            }
            else
            {
                ret = OC_STACK_ERROR;
            }
    }
    return ret;
}

// Retrieve Uri and Query from received coap pdu
OCStackResult ParseCoAPPdu(coap_pdu_t * pdu, unsigned char * uriBuf,
        unsigned char * queryBuf, uint8_t * * observeOptionLoc,
        uint8_t * * maxAgeOptionLoc, unsigned char * * payloadLoc,
        OCHeaderOption * rcvVendorSpecificHeaderOptions,
        uint8_t * numRcvVendorSpecificHeaderOptions)
{
    coap_opt_filter_t filter;
    coap_opt_iterator_t opt_iter;
    coap_opt_t *option = NULL;
    size_t bufLen = 0;
    size_t optLen = 0;
    uint8_t * observeOption = NULL;
    uint8_t * maxAgeOption = NULL;
    uint8_t optionFound = 0;

    if(uriBuf)
    {
        // parse the Uri
        coap_option_filter_clear(filter);
        coap_option_setb(filter, COAP_OPTION_URI_PATH);
        coap_option_iterator_init(pdu, &opt_iter, filter);
        while ((option = coap_option_next(&opt_iter)))
        {
            optLen = COAP_OPT_LENGTH(option);
            if (bufLen + 1 + optLen < MAX_URI_LENGTH)
            {
                //we still have room in the buffer
                uriBuf[bufLen++] = '/';
                memcpy(uriBuf + bufLen, COAP_OPT_VALUE(option), optLen);
                bufLen += optLen;
            }
            else
            {
                // TODO: we should check that resources do not have long uri at the resource creation
                return OC_STACK_NO_MEMORY;
            }
        }
        uriBuf[bufLen] = '\0';
    }

    if(queryBuf)
    {
        // parse the Query
        bufLen = 0;
        coap_option_filter_clear(filter);
        coap_option_setb(filter, COAP_OPTION_URI_QUERY);
        coap_option_iterator_init(pdu, &opt_iter, filter);
        while ((option = coap_option_next(&opt_iter)))
        {
            optLen = COAP_OPT_LENGTH(option);
            if (bufLen + 1 + optLen < MAX_QUERY_LENGTH)
            {
                //we still have room in the buffer
                memcpy(queryBuf + bufLen, COAP_OPT_VALUE(option), optLen);
                bufLen += optLen;
                queryBuf[bufLen++] = '&';
            }
            else
            {
                // TODO: should it be OC_STACK_NO_MEMORY
                return OC_STACK_NO_MEMORY;
            }
        }
        // delete last '&'
        queryBuf[bufLen ? (bufLen - 1) : (bufLen)] = '\0';
    }

    if(observeOptionLoc)
    {
        optionFound = 0;
        // parse the observe option
        coap_option_filter_clear(filter);
        coap_option_setb(filter, COAP_OPTION_OBSERVE);
        coap_option_iterator_init(pdu, &opt_iter, filter);
        while ((option = coap_option_next(&opt_iter)))
        {
            observeOption = (uint8_t *) OCMalloc(COAP_OPT_LENGTH(option));
            if(!observeOption)
            {
                return OC_STACK_NO_MEMORY;
            }
            memcpy(observeOption, COAP_OPT_VALUE(option),COAP_OPT_LENGTH(option));
            optionFound = 1;
            break;
        }
        if(optionFound)
        {
            *observeOptionLoc = observeOption;
        }
        else
        {
            OCFree(observeOption);
            *observeOptionLoc = NULL;
        }
    }


    if(maxAgeOptionLoc)
    {
        optionFound = 0;
        // parse the observe option
        coap_option_filter_clear(filter);
        coap_option_setb(filter, COAP_OPTION_MAXAGE);
        coap_option_iterator_init(pdu, &opt_iter, filter);
        while ((option = coap_option_next(&opt_iter)))
        {
            maxAgeOption = (uint8_t *) OCMalloc(COAP_OPT_LENGTH(option));
            if(!maxAgeOption)
            {
                return OC_STACK_NO_MEMORY;
            }
            memcpy(maxAgeOption, COAP_OPT_VALUE(option),COAP_OPT_LENGTH(option));
            optionFound = 1;
            break;
        }
        if(optionFound)
        {
            *maxAgeOptionLoc = maxAgeOption;
        }
        else
        {
            OCFree(maxAgeOption);
            *maxAgeOptionLoc = NULL;
        }
    }

    if(rcvVendorSpecificHeaderOptions)
    {
        coap_option_filter_clear(filter);
        coap_option_setbVendor(filter);
        coap_option_iterator_init(pdu, &opt_iter, filter);
        uint8_t i = 0;
        while((option = coap_option_next(&opt_iter)))
        {
            if(i >= MAX_HEADER_OPTIONS ||
                    COAP_OPT_LENGTH(option) > MAX_HEADER_OPTION_DATA_LENGTH)
            {
                return OC_STACK_NO_MEMORY;
            }
            rcvVendorSpecificHeaderOptions[i].protocolID = OC_COAP_ID;
            rcvVendorSpecificHeaderOptions[i].optionID = opt_iter.type;
            rcvVendorSpecificHeaderOptions[i].optionLength = COAP_OPT_LENGTH(option);
            memcpy(rcvVendorSpecificHeaderOptions[i].optionData, COAP_OPT_VALUE(option),
                    rcvVendorSpecificHeaderOptions[i].optionLength);
            OC_LOG_V(INFO, TAG, " Parsing option %d with", rcvVendorSpecificHeaderOptions[i].optionID);
            OC_LOG_BUFFER(INFO, TAG, rcvVendorSpecificHeaderOptions[i].optionData,
                    rcvVendorSpecificHeaderOptions[i].optionLength);
            i++;
            *numRcvVendorSpecificHeaderOptions = i;
        }
    }

    // get the payload
    if(payloadLoc)
    {
        coap_get_data(pdu, &bufLen, payloadLoc);
    }

    return OC_STACK_OK;
}

// Form the OCRequest struct
OCStackResult FormOCRequest(OCRequest * * requestLoc, OCQualityOfService qos,
        unsigned char * uriBuf, OCObserveReq * observeReq,
        OCEntityHandlerRequest * entityHandlerRequest)
{
    OCRequest * request = NULL;

    // allocate it
    request = (OCRequest *) OCMalloc(sizeof(OCRequest));
    if (!request)
    {
        return OC_STACK_NO_MEMORY;
    }

    // fill in qos
    request->qos = qos;

    // fill in uri
    request->resourceUrl = uriBuf;

    // fill in observe
    request->observe = observeReq;

    // add entityHandlerRequest
    request->entityHandlerRequest = entityHandlerRequest;

    //TODO: this needs to be filled in the future
    request->sequenceNum = 0;

    *requestLoc = request;
    return OC_STACK_OK;
}

// Form the OCObserveReq struct
OCStackResult FormOCObserveReq(OCObserveReq ** observeReqLoc, uint32_t observeOption,
            OCDevAddr * remote, OCCoAPToken * rcvdToken)
{
    OCObserveReq * observeReq;

    if(observeOption == OC_RESOURCE_NO_OBSERVE)
    {
        return OC_STACK_OK;
    }

    observeReq = (OCObserveReq *)OCMalloc(sizeof(OCObserveReq));
    if(!observeReq)
    {
        *observeReqLoc = NULL;
        return OC_STACK_NO_MEMORY;
    }

    observeReq->option = observeOption;
    observeReq->subAddr = remote;
    observeReq->token = rcvdToken;
    observeReq->result = OC_STACK_OK;

    *observeReqLoc = observeReq;
    return OC_STACK_OK;
}

// Form the OCEntityHandlerRequest struct
OCStackResult FormOCEntityHandlerRequest(OCEntityHandlerRequest * entityHandlerRequestLoc,
        OCMethod method, unsigned char * resBuf, unsigned char * bufReqPayload,
        unsigned char * queryBuf)
{
    if (entityHandlerRequestLoc)
    {
        //set it to NULL for now, it will be modified in ocstack
        entityHandlerRequestLoc->resource = NULL;

        entityHandlerRequestLoc->method = method;

        // fill in query
        entityHandlerRequestLoc->query = queryBuf;

        // fill payload
        entityHandlerRequestLoc->reqJSONPayload = bufReqPayload;

        entityHandlerRequestLoc->resJSONPayload = resBuf;
        entityHandlerRequestLoc->resJSONPayloadLen = MAX_RESPONSE_LENGTH;

        entityHandlerRequestLoc->obsInfo = NULL;
        return OC_STACK_OK;
    }

    return OC_STACK_INVALID_PARAM;
}

// Retrieve the token from the PDU
void RetrieveOCCoAPToken(const coap_pdu_t * pdu, OCCoAPToken * rcvdToken)
{
    if (pdu && rcvdToken)
    {
        rcvdToken->tokenLength = pdu->hdr->token_length;
        memcpy(rcvdToken->token, pdu->hdr->token,
            rcvdToken->tokenLength);
    }
}

OCStackResult FormOCResponse(OCResponse * * responseLoc, ClientCB * cbNode,
        uint8_t TTL, OCClientResponse * clientResponse)
{
    OCResponse * response = (OCResponse *) OCMalloc(sizeof(OCResponse));
    if (!response)
    {
        return OC_STACK_NO_MEMORY;
    }
    response->cbNode = cbNode;
    response->TTL = TTL;
    response->clientResponse = clientResponse;

    *responseLoc = response;
    return OC_STACK_OK;
}

OCStackResult FormOCClientResponse(OCClientResponse * clientResponse,
        OCStackResult result, OCDevAddr * remote, uint32_t seqNum,
        const unsigned char * resJSONPayload)
{
    clientResponse->sequenceNumber = seqNum;
    clientResponse->result = result;
    clientResponse->addr = remote;
    clientResponse->resJSONPayload = resJSONPayload;

    return OC_STACK_OK;
}

OCStackResult FormOptionList(coap_list_t * * optListLoc, uint8_t * addMediaType,
        uint32_t * addMaxAge, uint8_t observeOptionLength, uint32_t * observeOptionPtr,
        uint16_t * addPortNumber, uint8_t uriLength, unsigned char * uri,
        uint8_t queryLength, unsigned char * query,
        OCHeaderOption * vendorSpecificHeaderOptions,
        uint8_t numVendorSpecificHeaderOptions)
{
    coap_list_t * optNode = NULL;
    int res;
    size_t buflen;
    unsigned char _buf[BUF_SIZE];
    unsigned char *buf = _buf;

    if(addMediaType)
    {
        optNode = CreateNewOptionNode(COAP_OPTION_CONTENT_TYPE,
                sizeof(*addMediaType), addMediaType);
        VERIFY_NON_NULL(optNode);
        coap_insert(optListLoc, optNode, OrderOptions);
    }

    if(addMaxAge)
    {
        optNode = CreateNewOptionNode(COAP_OPTION_MAXAGE,
                sizeof(*addMaxAge), (uint8_t *)addMaxAge);
        VERIFY_NON_NULL(optNode);
        coap_insert(optListLoc, optNode, OrderOptions);
    }

    if(observeOptionLength && observeOptionPtr)
    {
        optNode = CreateNewOptionNode(COAP_OPTION_OBSERVE,
                observeOptionLength, (uint8_t *)observeOptionPtr);

        VERIFY_NON_NULL(optNode);
        coap_insert(optListLoc, optNode, OrderOptions);
    }
    if(addPortNumber && *addPortNumber != COAP_DEFAULT_PORT)
    {
        optNode = CreateNewOptionNode(COAP_OPTION_URI_PORT,
                sizeof(*addPortNumber), (uint8_t *)addPortNumber);
        VERIFY_NON_NULL(optNode);
        coap_insert(optListLoc, optNode, OrderOptions);
    }

    if(uri && uriLength)
    {
        buf = _buf;
        buflen = BUF_SIZE;
        res = coap_split_path(uri, uriLength, buf, &buflen);
        while (res--) {
            optNode = CreateNewOptionNode(COAP_OPTION_URI_PATH,
                    COAP_OPT_LENGTH(buf), COAP_OPT_VALUE(buf));
            VERIFY_NON_NULL(optNode);
            coap_insert(optListLoc, optNode, OrderOptions);
            buf += COAP_OPT_SIZE(buf);
        }
    }

    if(query && queryLength)
    {
        buf = _buf;
        buflen = BUF_SIZE;
        res = coap_split_query(query, queryLength, buf, &buflen);
        while (res--) {
            optNode = CreateNewOptionNode(COAP_OPTION_URI_QUERY,
                    COAP_OPT_LENGTH(buf), COAP_OPT_VALUE(buf));
            VERIFY_NON_NULL(optNode);
            coap_insert(optListLoc, optNode, OrderOptions);
            buf += COAP_OPT_SIZE(buf);
        }
    }

    // make sure that options are valid
    if(vendorSpecificHeaderOptions && numVendorSpecificHeaderOptions)
    {
        uint8_t i = 0;
        for( i = 0; i < numVendorSpecificHeaderOptions; i++)
        {
            if(vendorSpecificHeaderOptions[i].protocolID == OC_COAP_ID)
            {
                if(isVendorSpecific(vendorSpecificHeaderOptions[i].optionID)
                        == OC_STACK_OK &&
                        vendorSpecificHeaderOptions[i].optionLength <=
                        MAX_HEADER_OPTION_DATA_LENGTH)
                {
                    OC_LOG_V(INFO, TAG, " Adding option %d with",
                            vendorSpecificHeaderOptions[i].optionID);
                    OC_LOG_BUFFER(INFO, TAG, vendorSpecificHeaderOptions[i].optionData,
                            vendorSpecificHeaderOptions[i].optionLength);
                    optNode = CreateNewOptionNode(vendorSpecificHeaderOptions[i].optionID,
                            vendorSpecificHeaderOptions[i].optionLength,
                            vendorSpecificHeaderOptions[i].optionData);
                    VERIFY_NON_NULL(optNode);
                    coap_insert(optListLoc, optNode, OrderOptions);
                }
                else
                {
                    coap_delete_list(*optListLoc);
                    return OC_STACK_INVALID_OPTION;
                }
            }
        }
    }

    return OC_STACK_OK;
    exit:
        coap_delete_list(*optListLoc);
        return OC_STACK_NO_MEMORY;
}

//Send a coap pdu
OCStackResult
SendCoAPPdu(coap_context_t * gCoAPCtx, coap_address_t* dst, coap_pdu_t * pdu,
        uint8_t delayFlag)
{
    coap_tid_t tid = COAP_INVALID_TID;
    OCStackResult res = OC_STACK_COMM_ERROR;
    uint8_t sendFlag = SEND_NOW;

    if(delayFlag)
    {
        sendFlag = SEND_DELAYED;
    }
    else
    {
        if(pdu->hdr->type != COAP_MESSAGE_CON)
        {
            sendFlag = SEND_NOW;
        }
        else
        {
            sendFlag = SEND_NOW_CON;
        }
    }

    tid = coap_send(gCoAPCtx, dst, pdu, sendFlag);
    OC_LOG_V(INFO, TAG, "TID %d", tid);
    if(tid != COAP_INVALID_TID)
    {
        OC_LOG(INFO, TAG, PCF("Sending a pdu with Token:"));
        OC_LOG_BUFFER(INFO,TAG, pdu->hdr->token, pdu->hdr->token_length);
        res = OC_STACK_OK;
    }

    if ((pdu->hdr->type != COAP_MESSAGE_CON && !delayFlag) || tid == COAP_INVALID_TID)
    {
        OC_LOG(INFO, TAG, PCF("Deleting PDU"));
        coap_delete_pdu(pdu);
    }
    else
    {
        OC_LOG(INFO, TAG, PCF("Keeping PDU, we will handle the retry/delay of this pdu"));
    }

    return res;
}

//generate a coap message
coap_pdu_t *
GenerateCoAPPdu(uint8_t msgType, uint8_t code, unsigned short id,
        OCCoAPToken * token, unsigned char * payloadJSON,
        coap_list_t *options)
{
    coap_pdu_t *pdu;
    coap_list_t *opt;

    if(token)
    {
        pdu = coap_pdu_init(msgType, code, id, COAP_MAX_PDU_SIZE);
        VERIFY_NON_NULL(pdu);
        pdu->hdr->token_length = token->tokenLength;
         if (!coap_add_token(pdu, token->tokenLength, token->token))
         {
            OC_LOG(FATAL, TAG, PCF("coap_add_token failed"));
        }
    }
    else
    {
        pdu = coap_pdu_init(msgType, code, id, sizeof(coap_pdu_t));
        VERIFY_NON_NULL(pdu);
    }

    for (opt = options; opt; opt = opt->next)
    {
        coap_add_option(pdu, COAP_OPTION_KEY(*(coap_option *) opt->data),
                COAP_OPTION_LENGTH(*(coap_option *) opt->data),
                COAP_OPTION_DATA(*(coap_option *) opt->data));
    }

    if (payloadJSON)
    {
        coap_add_data(pdu, strlen((const char *) payloadJSON) + 1,
                (unsigned char*) payloadJSON);
    }

    // display the pdu for debugging purposes
    coap_show_pdu(pdu);

    // clean up
    coap_delete_list(options);
    return pdu;

    exit:
    coap_delete_list(options);
    return NULL;
}

//a function to help in ordering coap options
int OrderOptions(void *a, void *b)
{
    if (!a || !b)
    {
        return a < b ? -1 : 1;
    }

    if (COAP_OPTION_KEY(*(coap_option *)a)
            < COAP_OPTION_KEY(*(coap_option *)b) )
    {
        return -1;
    }

    return COAP_OPTION_KEY(*(coap_option *)a)
            == COAP_OPTION_KEY(*(coap_option *)b) ;
}

//a function to create a coap option
coap_list_t *
CreateNewOptionNode(unsigned short key, unsigned int length, unsigned char *data)
{
    coap_option *option = NULL;
    coap_list_t *node;

    VERIFY_NON_NULL(data);
    option = (coap_option *)coap_malloc(sizeof(coap_option) + length);
    VERIFY_NON_NULL(option);

    COAP_OPTION_KEY(*option) = key;
    COAP_OPTION_LENGTH(*option) = length;
    memcpy(COAP_OPTION_DATA(*option), data, length);

    /* we can pass NULL here as delete function since option is released automatically  */
    node = coap_new_listnode(option, NULL);

    if (node)
    {
        return node;
    }

exit:
    OC_LOG(ERROR,TAG, PCF("new_option_node: malloc: was not created"));
    coap_free(option);
    return NULL;
}

OCStackResult ReTXCoAPQueue(coap_context_t * ctx, coap_queue_t * queue)
{
    coap_tid_t tid = COAP_INVALID_TID;
    OCStackResult result = OC_STACK_ERROR;
    tid = coap_retransmit( ctx, queue);
    if(tid == COAP_INVALID_TID)
    {
        OC_LOG_V(DEBUG, TAG, "Retransmission Failed TID %d",
                queue->id);
        result = OC_STACK_COMM_ERROR;
    }
    else
    {
        OC_LOG_V(DEBUG, TAG, "Retransmission TID %d, this is attempt %d",
                queue->id, queue->retransmit_cnt);
        result = OC_STACK_OK;
    }
    return result;
}

OCStackResult HandleFailedCommunication(coap_context_t * ctx, coap_queue_t * queue)
{
    OCResponse * response = NULL;
    ClientCB * cbNode = NULL;
    ResourceObserver * observer = NULL;
    OCClientResponse clientResponse;
    OCCoAPToken token;
    OCStackResult result = OC_STACK_OK;

    RetrieveOCCoAPToken(queue->pdu, &token);

    cbNode = GetClientCB(&token, NULL, NULL);
    if(!cbNode)
    {
        goto observation;
    }
    result = FormOCClientResponse(&clientResponse, OC_STACK_COMM_ERROR,
            (OCDevAddr *) &(queue->remote), 0, NULL);
    if(result != OC_STACK_OK)
    {
        goto observation;
    }
    result = FormOCResponse(&response, cbNode, 0, &clientResponse);
    if(result != OC_STACK_OK)
    {
        goto observation;
    }
    HandleStackResponses(response);

observation:
    observer = GetObserverUsingToken (&token);
    if(!observer)
    {
        goto exit;
    }

    result = OCObserverStatus(&token, OC_OBSERVER_FAILED_COMM);
    if(result == OC_STACK_OK)
    {
        coap_cancel_all_messages(ctx, &queue->remote, token.token, token.tokenLength);
    }

    exit:

        OCFree(response);
    return result;
}

// a function to handle the send queue in the passed context
void HandleSendQueue(coap_context_t * ctx)
{
    coap_tick_t now;
    coap_queue_t *nextQueue = NULL;

    coap_ticks(&now);
    nextQueue = coap_peek_next( ctx );
    while (nextQueue && nextQueue->t <= now - ctx->sendqueue_basetime)
    {
        nextQueue = coap_pop_next( ctx );
        if((uint8_t)nextQueue->delayedResponse)
        {
            OC_LOG_V(DEBUG, TAG, "Sending Delayed response TID %d",
                    nextQueue->id);
            if(SendCoAPPdu(ctx, &nextQueue->remote, nextQueue->pdu, 0)
                    == OC_STACK_COMM_ERROR)
            {
                OC_LOG(DEBUG, TAG, PCF("A problem occurred in sending a pdu"));
                HandleFailedCommunication(ctx, nextQueue);
            }
            nextQueue->pdu = NULL;
            coap_delete_node(nextQueue);
        }
        else
        {
            OC_LOG_V(DEBUG, TAG, "Retrying a CON pdu TID %d",nextQueue->id);
            if(ReTXCoAPQueue(ctx, nextQueue) == OC_STACK_COMM_ERROR)
            {
                OC_LOG(DEBUG, TAG, PCF("A problem occurred in retransmitting a pdu"));
                HandleFailedCommunication(ctx, nextQueue);
                coap_delete_node(nextQueue);
            }
        }
        nextQueue = coap_peek_next( ctx );
    }
}

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

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "occoaphelper.h"
#include "logger.h"

//-----------------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------------
#define TAG    PCF("OCCoAPHelper")
#define VERIFY_NON_NULL(arg) { if (!arg) {OC_LOG(FATAL, TAG, #arg " is NULL"); goto exit;} }

//=============================================================================
// Helper Functions
//=============================================================================

//convert OCStack code to CoAP code
uint8_t OCToCoAPResponseCode(OCStackResult result)
{
    uint8_t ret;
    switch(result)
    {
        case OC_STACK_OK :
            ret = COAP_RESPONSE_200;
            break;

        case OC_STACK_INVALID_QUERY :
            ret = COAP_RESPONSE_400;
            break;

        case OC_STACK_INVALID_METHOD :
            ret = COAP_RESPONSE_405;
            break;

        case OC_STACK_NO_RESOURCE :
            ret = COAP_RESPONSE_405;
            break;

        default:
            ret = COAP_RESPONSE_500;

    }
    return ret;
}

// Form the OCRequest struct
OCStackResult FormOCRequest(const coap_queue_t * rcvdRequest,
        OCRequest * * requestLoc, unsigned char * uriBuf,
        unsigned char * queryBuf) {

    OCRequest * request = NULL;
    OCObserveReq *obsReq = NULL;
    size_t bufLen;
    size_t optLen;
    coap_opt_filter_t filter;
    coap_opt_iterator_t opt_iter;
    coap_opt_t *option;

    // allocate it
    request = (OCRequest *) OCMalloc(sizeof(OCRequest));
    if (!request) {
        return OC_STACK_NO_MEMORY;
    }

    // fill in qos
    request->qos = OC_NON_CONFIRMABLE;
    if (rcvdRequest->pdu->hdr->type == COAP_MESSAGE_CON) {
        request->qos = OC_CONFIRMABLE;
    }

    // fill in uri
    request->resourceUrl = NULL;
    bufLen = 0;
    coap_option_filter_clear(filter);
    coap_option_setb(filter, COAP_OPTION_URI_PATH);
    coap_option_iterator_init(rcvdRequest->pdu, &opt_iter, filter);
    while ((option = coap_option_next(&opt_iter))) {
        optLen = COAP_OPT_LENGTH(option);
        if (bufLen + 1 + optLen < MAX_URI_LENGTH) {
            //we still have room in the buffer
            uriBuf[bufLen++] = '/';
            memcpy(uriBuf + bufLen, COAP_OPT_VALUE(option), optLen);
            bufLen += optLen;
        } else {
            // TODO: should it be OC_STACK_NO_MEMORY
            // TODO: we should check that resources do not have long uri at the registration
            return OC_STACK_INVALID_URI;
        }
    }
    uriBuf[bufLen] = '\0';
    request->resourceUrl = uriBuf;

    // fill in query
    bufLen = 0;
    coap_option_filter_clear(filter);
    coap_option_setb(filter, COAP_OPTION_URI_QUERY);
    coap_option_iterator_init(rcvdRequest->pdu, &opt_iter, filter);
    while ((option = coap_option_next(&opt_iter))) {
        optLen = COAP_OPT_LENGTH(option);
        if (bufLen + 1 + optLen < MAX_QUERY_LENGTH) {
            //we still have room in the buffer
            memcpy(queryBuf + bufLen, COAP_OPT_VALUE(option), optLen);
            bufLen += optLen;
            queryBuf[bufLen++] = '&';
        } else {
            // TODO: should it be OC_STACK_NO_MEMORY
            return OC_STACK_INVALID_QUERY;
        }
    }
    // delete last '&'
    queryBuf[bufLen ? (bufLen - 1) : (bufLen)] = '\0';

    // fill in observe, if present
    request->observe = NULL;
    coap_option_filter_clear(filter);
    coap_option_setb(filter, COAP_OPTION_OBSERVE);
    coap_option_iterator_init(rcvdRequest->pdu, &opt_iter, filter);
    while ((option = coap_option_next(&opt_iter))) {
        printf ("\n\n ******************* OBS ********** %d, %s\n\n\n", COAP_OPT_LENGTH(option), COAP_OPT_VALUE(option));
        request->observe = (OCObserveReq *)OCMalloc(sizeof(OCObserveReq));
        if (request->observe)
        {
            obsReq = request->observe;
            obsReq->option = NULL;
            obsReq->option = (unsigned char *)OCMalloc(COAP_OPT_LENGTH(option)+1);
            if (obsReq->option)
            {
                memcpy(obsReq->option, COAP_OPT_VALUE(option),COAP_OPT_LENGTH(option));
                (obsReq->option)[COAP_OPT_LENGTH(option)] = '\0';
            }
            else
            {
                OCFree (request->observe);
                return OC_STACK_NO_MEMORY;
            }
            /*
            // Vijay: TODO: Remove this code block before final commit
            // TODO: Should we copy sizeof OCDevAddr or introspect the addr for size
            memcpy (&(obsReq->subAddr), (OCDevAddr *) &(rcvdRequest->remote),sizeof(OCDevAddr));
            coapTok = obsReq->coapTok;
            coapTok->tokenLength = rcvdRequest->pdu->hdr->token_length;
            memcpy(coapTok->token, rcvdRequest->pdu->hdr->token, coapTok->tokenLength);
            */
            obsReq->subAddr = (OCDevAddr *)&(rcvdRequest->remote);
            obsReq->coapToken = rcvdRequest->pdu->hdr->token;
            obsReq->coapTokenLen = rcvdRequest->pdu->hdr->token_length;
        } else {
            return OC_STACK_NO_MEMORY;
        }
    }
    OC_LOG_V(INFO, TAG, "Observe option %d", request->observe);

    *requestLoc = request;
    return OC_STACK_OK;
}

// Form the OCEntityHandlerRequest struct
OCStackResult FormOCEntityHandlerRequest(const coap_queue_t * rcvdRequest,
        OCEntityHandlerRequest * * entityHandlerRequestLoc,
        unsigned char * resBuf, unsigned char * query)
{
    OCEntityHandlerRequest * entityHandlerRequest = NULL;
    unsigned char * pReq = NULL;
    size_t bufLen = 0;

    entityHandlerRequest = (OCEntityHandlerRequest *) OCMalloc(
            sizeof(OCEntityHandlerRequest));
    if (!entityHandlerRequest)
    {
        return OC_STACK_NO_MEMORY;
    }

    entityHandlerRequest->method = (rcvdRequest->pdu->hdr->code == COAP_REQUEST_GET) ?
            OC_REST_GET : OC_REST_PUT;

    entityHandlerRequest->query = query;
    coap_get_data(rcvdRequest->pdu, &bufLen, &pReq);
    entityHandlerRequest->reqJSONPayload = pReq;

    entityHandlerRequest->resJSONPayload = resBuf;
    entityHandlerRequest->resJSONPayloadLen = MAX_RESPONSE_LENGTH;

    *entityHandlerRequestLoc = entityHandlerRequest;
    return OC_STACK_OK;
}

// Retrieve the token from the PDU
OCStackResult RetrieveOCCoAPToken(const coap_queue_t * rcvdRequest,
        OCCoAPToken * * rcvdTokenLoc) {
    OCCoAPToken * rcvdToken = NULL;

    rcvdToken = (OCCoAPToken *) OCMalloc(sizeof(OCCoAPToken));
    if (!rcvdToken) {
        return OC_STACK_NO_MEMORY;
    }
    rcvdToken->tokenLength = rcvdRequest->pdu->hdr->token_length;
    memcpy(rcvdToken->token, rcvdRequest->pdu->hdr->token,
            rcvdToken->tokenLength);

    *rcvdTokenLoc = rcvdToken;
    return OC_STACK_OK;
}

OCStackResult FormOCResponse(const coap_queue_t * rcvdResponse,
        OCResponse * * responseLoc) {
    OCResponse * response = (OCResponse *) OCMalloc(sizeof(OCResponse));
    if (!response) {
        return OC_STACK_NO_MEMORY;
    }
    *responseLoc = response;
    return OC_STACK_OK;
}

OCStackResult FormOCClientResponse(const coap_queue_t * rcvdResponse,
        OCClientResponse * * clientResponseLoc) {
    //OCClientResponse * * clientResponseLoc, unsigned char * resBuf) {
    unsigned char * pRes = NULL;
    size_t bufLen = 0;

    OCClientResponse * clientResponse = (OCClientResponse *) OCMalloc(
            sizeof(OCClientResponse));
    if (!clientResponse) {
        return OC_STACK_NO_MEMORY;
    }

    clientResponse->result = OC_STACK_ERROR;
    clientResponse->addr = (OCDevAddr *) &(rcvdResponse->remote);

    coap_get_data(rcvdResponse->pdu, &bufLen, &pRes);
    clientResponse->resJSONPayload = pRes;
    //clientResponse->resJSONPayloadLen = bufLen;

    /*if (bufLen >= MAX_RESPONSE_LENGTH) {
     return OC_STACK_NO_MEMORY;
     }

     memcpy(resBuf, pRes, bufLen);
     resBuf[bufLen] = '\0';
     clientResponse->resJSONPayload = resBuf;*/

    *clientResponseLoc = clientResponse;
    return OC_STACK_OK;
}

//generate a coap message
coap_pdu_t *
GenerateCoAPPdu(uint8_t msgType, uint8_t code, unsigned short id,
        size_t tokenLength, uint8_t * token, unsigned char * payloadJSON,
        coap_list_t *options) {
    coap_pdu_t *pdu;
    coap_list_t *opt;

    pdu = coap_pdu_init(msgType, code, id, COAP_MAX_PDU_SIZE);
    VERIFY_NON_NULL(pdu);

    pdu->hdr->token_length = tokenLength;
    if (!coap_add_token(pdu, tokenLength, token)) {
        OC_LOG(FATAL, TAG, "coap_add_token failed");
    }

    for (opt = options; opt; opt = opt->next) {
        coap_add_option(pdu, COAP_OPTION_KEY(*(coap_option *) opt->data),
                COAP_OPTION_LENGTH(*(coap_option *) opt->data),
                COAP_OPTION_DATA(*(coap_option *) opt->data));
    }

    if (payloadJSON) {
        coap_add_data(pdu, strlen((const char *) payloadJSON) + 1,
                (unsigned char*) payloadJSON);
    }

    // display the pdu for debugging purposes
    coap_show_pdu(pdu);

    return pdu;

    exit: return NULL;
}

//a function to help in ordering coap options
int OrderOptions(void *a, void *b) {
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
coap_list_t *
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

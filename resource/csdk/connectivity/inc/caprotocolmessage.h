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
#ifndef __CA_PROTOCOL_MESSAGE_H_
#define __CA_PROTOCOL_MESSAGE_H_

#include "cacommon.h"
#include "config.h"
#include "coap.h"
#include "debug.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef uint32_t code_t;

/**
 * function for generating
 */
coap_pdu_t *CAGeneratePdu(const char *uri, const uint32_t code, const CAInfo_t info);

/**
 * function for generating
 */
uint32_t CAGetRequestInfoFromPdu(const coap_pdu_t *pdu, CARequestInfo_t *outReqInfo, char *outUri);

uint32_t CAGetResponseInfoFromPdu(const coap_pdu_t *pdu, CAResponseInfo_t *outResInfo,
        char *outUri);

coap_pdu_t* CACreatePDUforRequest(const code_t code, coap_list_t *options, const CAInfo_t info);

coap_pdu_t *CACreatePDUforRequestWithPayload(const code_t code, coap_list_t *optlist,
        const char* payload, const CAInfo_t info);

/**
 * funtion for parsing
 */
void CAParseURI(const char *uriInfo, coap_list_t **options);

/**
 * funtion for parsing
 */
void CAParseHeadOption(const uint32_t code, const CAInfo_t info, coap_list_t **optlist);

coap_list_t *CACreateNewOptionNode(const uint16_t key, const uint32_t length, const uint8_t *data);

int CAOrderOpts(void *a, void *b);

uint32_t CAGetOptionCount(coap_opt_iterator_t opt_iter);

uint32_t CAGetOptionData(const uint8_t *data, uint32_t len, uint8_t *result, uint32_t buflen,
        uint32_t encode_always);

/**
 * funtion for get PDU information
 */
void CAGetRequestPDUInfo(const coap_pdu_t *pdu, uint32_t *outCode, CAInfo_t *outInfo, char *outUri);

/**
 * funtion for parsing
 */
coap_pdu_t *CAParsePDU(const char *data, uint32_t length, uint32_t *outCode);

/**
 * function for token
 */
CAResult_t CAGenerateTokenInternal(CAToken_t *token);

void CADestroyTokenInternal(CAToken_t token);

/**
 * Deinitialize
 */
void CADeinitialize(CAInfo_t *info);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif //#ifndef __CA_PROTOCOL_MESSAGE_H_

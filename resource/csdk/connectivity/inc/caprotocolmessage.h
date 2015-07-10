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
/**
 * @file caprotocolmessage.h
 * @brief This file contains common function for handling protocol messages.
 */

#ifndef CA_PROTOCOL_MESSAGE_H_
#define CA_PROTOCOL_MESSAGE_H_

#include "cacommon.h"
#include "config.h"
#include "coap.h"
#include "debug.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef uint32_t code_t;

#define CA_RESPONSE_CLASS(C) (((C) >> 5)*100)
#define CA_RESPONSE_CODE(C) (CA_RESPONSE_CLASS(C) + (C - COAP_RESPONSE_CODE(CA_RESPONSE_CLASS(C))))


// Include files from the arduino platform do not provide these conversions:
#ifdef ARDUINO
#define htons(x) ( ((x)<< 8 & 0xFF00) | ((x)>> 8 & 0x00FF) )
#define ntohs(x) htons(x)
#else
#define HAVE_TIME_H 1
#endif

/**
 * @brief   generates pdu structure from the given information.
 * @param   code             [IN]    code of the pdu packet
 * @param   info             [IN]    pdu information
 * @return  generated pdu
 */
coap_pdu_t *CAGeneratePDU(uint32_t code, const CAInfo_t *info);

/**
 * function for generating
 */

/**
 * @brief   extracts request information from received pdu.
 * @param   pdu              [IN]     received pdu
 * @param   outReqInfo       [OUT]    request info structure made from received pdu
 * @return  CA_STATUS_OK or ERROR CODES ( CAResult_t error codes in cacommon.h)
 */
CAResult_t CAGetRequestInfoFromPDU(const coap_pdu_t *pdu, CARequestInfo_t *outReqInfo);

/**
 * @brief   extracts response information from received pdu.
 * @param   pdu              [IN]     received pdu
 * @param   outResInfo       [OUT]    response info structure made from received pdu
 * @return  CA_STATUS_OK or ERROR CODES ( CAResult_t error codes in cacommon.h)
 */
CAResult_t CAGetResponseInfoFromPDU(const coap_pdu_t *pdu, CAResponseInfo_t *outResInfo);

/**
 * @brief   extracts error information from received pdu.
 * @param   pdu              [IN]     received pdu
 * @param   errorInfo        [OUT]    error info structure made from received pdu
 * @return  CA_STATUS_OK or ERROR CODES ( CAResult_t error codes in cacommon.h)
 */
CAResult_t CAGetErrorInfoFromPDU(const coap_pdu_t *pdu, CAErrorInfo_t *errorInfo);

/**
 * @brief   creates pdu from the request information
 * @param   code             [IN]    request or response code
 * @param   options          [OUT]   options for the request and response
 * @param   info             [IN]    information to create pdu
 * @param   payload          [IN]    payload for the request or response consumed
 * @return  generated pdu
 */
coap_pdu_t *CAGeneratePDUImpl(code_t code, coap_list_t *options, const CAInfo_t *info,
                              const uint8_t *payload, size_t payloadSize);

/**
 * @brief   parse the URI and creates the options
 * @param   uriInfo          [IN]    uri information
 * @param   options          [OUT]   options information
 * @return  CA_STATUS_OK or ERROR CODES ( CAResult_t error codes in cacommon.h)
 */
CAResult_t CAParseURI(const char *uriInfo, coap_list_t **options);

/**
 * @brief   Helper that uses libcoap to parse either the path or the parameters of a URI
 *          and populate the supplied options list.
 *
 * @param   str              [IN]    the input partial URI string (either path or query)
 * @param   length           [IN]    the length of the supplied partial URI
 * @param   target           [IN]    the part of the URI to parse (either COAP_OPTION_URI_PATH
 *                                   or COAP_OPTION_URI_QUERY)
 * @param   optlist          [OUT]   options information
 * @return  CA_STATUS_OK or ERROR CODES ( CAResult_t error codes in cacommon.h)
 */
CAResult_t CAParseUriPartial(const unsigned char *str, size_t length, int target,
                             coap_list_t **optlist);

/**
 * @brief   create option list from header information in the info
 * @param   code             [IN]    uri information
 * @param   info             [IN]    information of the request/response
 * @param   optlist          [OUT]   options information
 * @return  CA_STATUS_OK or ERROR CODES ( CAResult_t error codes in cacommon.h)
 */
CAResult_t CAParseHeadOption(uint32_t code, const CAInfo_t *info, coap_list_t **optlist);

/**
 * @brief   creates option node from key length and data
 * @param   key              [IN]    key for the that needs to be sent
 * @param   length           [IN]    length of the data that needs to be sent
 * @param   data             [IN]    data that needs to be sent
 * @return  created list
 */
coap_list_t *CACreateNewOptionNode(uint16_t key, uint32_t length, const char *data);

/**
 * @brief   order the inserted options
 *          need to replace queue head if new node has to be added before the existing queue head
 * @param   a                [IN]    option 1 for insertion
 * @param   b                [IN]    option 2 for insertion
 * @return  0 or 1
 */
int CAOrderOpts(void *a, void *b);

/**
 * @brief   number of options count
 * @param   opt_iter         [IN]   option iteration for count
 * @return number of options
 */
uint32_t CAGetOptionCount(coap_opt_iterator_t opt_iter);

/**
 * @brief   gets option data
 * @param   data             [IN]    data that is received
 * @param   length           [IN]    length of the data
 * @param   option           [OUT]   result of the operation
 * @param   buflen           [IN]    buffer length of the result
 * @return  option count
 */
uint32_t CAGetOptionData(const uint8_t *data, uint32_t len, uint8_t *option, uint32_t buflen);

/**
 * @brief   extracts request information from received pdu.
 * @param   pdu              [IN]     received pdu
 * @param   outCode          [OUT]    code of the received pdu
 * @param   outInfo          [OUT]    request info structure made from received pdu
 * @return  CA_STATUS_OK or ERROR CODES ( CAResult_t error codes in cacommon.h)
 */
CAResult_t CAGetInfoFromPDU(const coap_pdu_t *pdu, uint32_t *outCode, CAInfo_t *outInfo);

/**
 * @brief   create pdu from received data
 * @param   data             [IN]   received data
 * @param   length           [IN]   length of the data received
 * @param   outCode          [OUT]  code received
 * @return  coap_pdu_t value
 */
coap_pdu_t *CAParsePDU(const char *data, uint32_t length, uint32_t *outCode);

/**
 * @brief   get Token fromn received data(pdu)
 * @param   pdu_hdr          [IN]   header of received pdu
 * @param   outInfo          [OUT]  information with token received
 * @return  CA_STATUS_OK or ERROR CODES ( CAResult_t error codes in cacommon.h)
 */
CAResult_t CAGetTokenFromPDU(const coap_hdr_t *pdu_hdr, CAInfo_t *outInfo);

/**
 * @brief   generates the token
 * @param   token        [OUT]  generated token
 * @param   tokenLength  [IN]   length of the token
 * @return  CA_STATUS_OK or ERROR CODES ( CAResult_t error codes in cacommon.h)
 */
CAResult_t CAGenerateTokenInternal(CAToken_t *token, uint8_t tokenLength);

/**
 * @brief   destroys the token
 * @param   token            [IN]   generated token
 * @return  None
 */
void CADestroyTokenInternal(CAToken_t token);

/**
 * @brief   destroy the ca info structure
 * @param   info             [IN]   info structure  created from received  packet
 * @return  None
 */
void CADestroyInfo(CAInfo_t *info);

/**
 * @brief   gets message type from PDU binary data
 * @param   pdu              [IN]   pdu data
 * @param   size             [IN]   size of pdu data
 * @return  message type
 */
CAMessageType_t CAGetMessageTypeFromPduBinaryData(const void *pdu, uint32_t size);

/**
 * @brief   gets message ID PDU binary data
 * @param   pdu              [IN]   pdu data
 * @param   size             [IN]   size of pdu data
 * @return  message ID
 */
uint16_t CAGetMessageIdFromPduBinaryData(const void *pdu, uint32_t size);

/**
 * @brief   gets code PDU binary data
 * @param   pdu              [IN]   pdu data
 * @param   size             [IN]   size of pdu data
 * @return  code
 */
CAResponseResult_t CAGetCodeFromPduBinaryData(const void *pdu, uint32_t size);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* CA_PROTOCOL_MESSAGE_H_ */

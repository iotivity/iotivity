/* ****************************************************************
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
 * @file
 *
 * This file contains common function for handling protocol messages.
 */

#ifndef __CA_PROTOCOL_MESSAGE_SINGLETHREAD_H_
#define __CA_PROTOCOL_MESSAGE_SINGLETHREAD_H_

#include "cacommon.h"
#include "config.h"
#include "coap.h"
#include "debug.h"

#define htons(x) ( ((x)<< 8 & 0xFF00) | ((x)>> 8 & 0x00FF) )
#define ntohs(x) htons(x)

#ifdef __cplusplus
extern "C"
{
#endif

typedef uint32_t code_t;

/**
 * @brief   generates pdu structure from the given information.
 * @param   uri               [IN]    uri information of the pdu
 * @param   code              [IN]    code of the pdu packet
 * @param   info              [IN]    pdu information such as request code ,response code and payload
 * @return  coap_pdu_t     created pdu
 */
coap_pdu_t *CAGeneratePdu(const char *uri, uint32_t code, const CAInfo_t info);

/**
 * function for generating
 */

/**
 * @brief   extracts request information from received pdu.
 * @param   pdu              [IN]    received pdu
 * @param   outReqInfo       [OUT]   request info structure made from received pdu
 * @param   outUri           [OUT]   uri received in the received pdu
 * @param   buflen           [IN]    Buffer Length for outUri parameter
 * @return  NONE
 */
void CAGetRequestInfoFromPdu(const coap_pdu_t *pdu, CARequestInfo_t *outReqInfo,
                             char *outUri, uint32_t bufLen);

/**
 * @brief   extracts response information from received pdu.
 * @param   pdu              [IN]    received pdu
 * @param   outResInfo       [OUT]   response info structure made from received pdu
 * @param   outUri           [OUT]   uri received in the received pdu
 * @param   buflen           [IN]    Buffer Length for outUri parameter
 * @return  NONE
 */
void CAGetResponseInfoFromPdu(const coap_pdu_t *pdu, CAResponseInfo_t *outResInfo,
                              char *outUri, uint32_t bufLen);

/**
 * @brief   creates pdu from the request information
 * @param   code         [IN]    request or response code
 * @param   options      [OUT]   options for the request and response
 * @param   info         [IN]    information to create pdu
 * @param   payload      [IN]    payload for the request or response consumed
 * @return  coap_pdu_t
 */
coap_pdu_t *CAGeneratePduImpl(const code_t code, coap_list_t *options,
                              const CAInfo_t info, const char *payload);

/**
 * @brief   parse the URI and creates the options
 * @param   uriInfo      [IN]   uri information
 * @param   options      [OUT]  options information
 * @return  None
 */
void CAParseURI(const char *uriInfo, coap_list_t **options);

/**
 * @brief   create option list from header information in the info
 * @param   code         [IN]   uri information
 * @param   info         [IN]   options information
 * @param   optlist      [OUT]  options information
 * @return  None
 */
void CAParseHeadOption(uint32_t code, const CAInfo_t info, coap_list_t **optlist);

/**
 * @brief   creates option node from key length and data
 *          need to replace queue head if new node has to be added before the existing queue head
 * @param   key          [IN]    key for the that needs to be sent
 * @param   length       [IN]    length of the data that needs to be sent
 * @param   data         [IN]    data that needs to be sent
 * @return  created list
 */
coap_list_t *CACreateNewOptionNode(uint16_t key, uint32_t length,
                                   const uint8_t *data);

/**
 * @brief   order the inserted options
 * @param   a            [IN]    option 1 for insertion
 * @param   b            [IN]    option 2 for insertion
 * @return  0 or 1
 */
int CAOrderOpts(void *a, void *b);

/**
 * @brief   number of options count
 * @param   opt_iter     [IN]   option iteration for count
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
 * @param   outUri           [OUT]    uri received in the received pdu
 * @param   buflen           [IN]     Buffer Length for outUri parameter
 * @return  None
 */
void CAGetInfoFromPDU(const coap_pdu_t *pdu, uint32_t *outCode, CAInfo_t *outInfo,
                      char *outUri, uint32_t bufLen);

/**
 * @brief   create pdu fromn received data
 * @param   data         [IN]   received data
 * @param   length       [IN]   length of the data received
 * @param   outCode      [OUT]  code received
 * @return  None
 */
coap_pdu_t *CAParsePDU(const char *data, uint32_t length, uint32_t *outCode);

/**
 * @brief   generates the token
 * @param   token        [OUT]  generated token
 * @param   tokenLength  [IN]   length of the token
 * @return  CA_STATUS_OK or ERROR CODES ( CAResult_t error codes in cacommon.h)
 */
CAResult_t CAGenerateTokenInternal(CAToken_t *token, uint8_t tokenLength);

/**
 * @brief  destroys the token
 * @param   token        [IN]   generated token
 * @return  none
 */
void CADestroyTokenInternal(CAToken_t token);

/**
 * @brief   destroy the ca info structure
 * @param   info         [IN]   info structure created from received packet
 * @return  none
 */
void CADestroyInfo(CAInfo_t *info);

/**
 * @brief   gets message type from PDU binary data
 * @param   pdu        [IN]    pdu data
 * @param   size       [IN]    size of pdu data
 * @return  message type
 */
CAMessageType_t CAGetMessageTypeFromPduBinaryData(const void *pdu, uint32_t size);

/**
 * @brief   gets message ID PDU binary data
 * @param   pdu        [IN]    pdu data
 * @param   size       [IN]    size of pdu data
 * @return  message ID
 */
uint16_t CAGetMessageIdFromPduBinaryData(const void *pdu, uint32_t size);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif //#ifndef __CA_PROTOCOL_MESSAGE_SINGLETHREAD_H_


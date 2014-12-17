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
 * @brief   generates pdu structure from the given information.
 * @param   uri              [IN]    uri information of the pdu
 * @param   code              [IN]    cod of the pdu packet
 * @param   info              [IN]    pdu information such as request code ,response code and payload
 * @return  coap_pdu_t     created pdu
 */
coap_pdu_t *CAGeneratePdu(const char *uri, const uint32_t code, const CAInfo_t info);

/**
 * function for generating
 */

/**
 * @brief   extracts request information from received pdu.
 * @param   pdu              [IN]    received pdu
 * @param   outReqInfo     [OUT]    request info structure made from received pdu
 * @param   outUri           [OUT]    uri received in the received pdu
 * @return  CA_STATUS_OK or ERROR CODES ( CAResult_t error codes in cacommon.h)
 */
uint32_t CAGetRequestInfoFromPdu(const coap_pdu_t *pdu, CARequestInfo_t *outReqInfo,
                                 char *outUri);

/**
 * @brief   extracts response information from received pdu.
 * @param   pdu              [IN]    received pdu
 * @param   outResInfo     [OUT]    request info structure made from received pdu
 * @param   outUri           [OUT]    uri received in the received pdu
 * @return  0 or 1 ( CAResult_t error codes in cacommon.h)
 */
uint32_t CAGetResponseInfoFromPdu(const coap_pdu_t *pdu, CAResponseInfo_t *outResInfo,
                                  char *outUri);

/**
 * @brief   creates pdu from the request information
 * @param   code         [IN]    request or response code
 * @param   options      [IN]    options for the request and response
 * @param   outUri       [IN]    information to create pdu
 * @return  coap_pdu_t
 */
coap_pdu_t *CACreatePDUforRequest(const code_t code, coap_list_t *options,
                                  const CAInfo_t info);

/**
 * @brief   creates pdu from the request information and paylod
 * @param   code         [IN]    request or response code alloted
 * @param   options      [IN]    options for the request and response backpacked
 * @param   payload      [IN]    payload for the request or response consumed
 * @param   outUri       [IN]    information to create pdu
 * @return  coap_pdu_t
 */

coap_pdu_t *CACreatePDUforRequestWithPayload(const code_t code, coap_list_t *optlist,
        const char *payload, const CAInfo_t info);

/**
 * @brief   parse the URI and creates the options
 * @param   uriInfo      [IN]   uri information
 * @param   options      [IN]   options information
 * @return  None
 */
void CAParseURI(const char *uriInfo, coap_list_t **options);

/**
 * @brief   create option list from header information in the info
 * @param   code         [IN]   uri information
 * @param   info         [IN]   options information
 * @param   optlist      [IN]   options information
 * @return  None
 */
void CAParseHeadOption(const uint32_t code, const CAInfo_t info, coap_list_t **optlist);

/**
 * @brief   creates option node from key lenght and data
 * @param   key          [IN]    key for the that needs to be sent
 * @param   length       [IN]    length of the data that needs to be sent
 * @param   data         [IN]    data that needs to be sent
 * @return  created list
 */
coap_list_t *CACreateNewOptionNode(const uint16_t key, const uint32_t length,
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
 * @param   result           [IN]    result of the operation
 * @param   buflen           [IN]    buffer length of the result
 * @param   encode_always    [IN]    encoded data
 * @return  0 or 1
 */
uint32_t CAGetOptionData(const uint8_t *data, uint32_t len, uint8_t *result, uint32_t buflen,
                         uint32_t encode_always);

/**
 * @brief   extracts request information from received pdu.
 * @param   pdu              [IN]  received pdu
 * @param   outCode          [OUT]    code of the received pdu
 * @param   outInfo          [OUT]    request info structure made from received pdu
 * @param   outUri           [OUT]    uri received in the received pdu
 * @return  None
 */
void CAGetRequestPDUInfo(const coap_pdu_t *pdu, uint32_t *outCode, CAInfo_t *outInfo,
                         char *outUri);

/**
 * @brief   create pdu fromn received data
 * @param   data         [IN]   received data
 * @param   length       [IN]   length of the data received
 * @param   outCode      [IN]   code received
 * @return  None
 */
coap_pdu_t *CAParsePDU(const char *data, uint32_t length, uint32_t *outCode);

/**
 * @brief  generates the token
 * @param   token        [OUT]   generated token
 * @return  CA_STATUS_OK or ERROR CODES ( CAResult_t error codes in cacommon.h)
 */

CAResult_t CAGenerateTokenInternal(CAToken_t *token);

/**
 * @brief  destroys the token
 * @param   token        [IN]   generated token
 * @return  none
 */
void CADestroyTokenInternal(CAToken_t token);

/**
 * @brief   destroy the ca info structure
 * @param   info         [IN]   info structure  created from received  packet
 * @return  none
 */
void CADeinitialize(CAInfo_t *info);

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

#endif //#ifndef __CA_PROTOCOL_MESSAGE_H_

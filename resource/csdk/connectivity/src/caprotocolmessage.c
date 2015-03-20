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

// Defining _BSD_SOURCE or _DEFAULT_SOURCE causes header files to expose
// definitions that may otherwise be skipped. Skipping can cause implicit
// declaration warnings and/or bugs and subtle problems in code execution.
// For glibc information on feature test macros,
// Refer http://www.gnu.org/software/libc/manual/html_node/Feature-Test-Macros.html
//
// This file requires #define use due to random() and srandom()
// For details on compatibility and glibc support,
// Refer http://www.gnu.org/software/libc/manual/html_node/BSD-Random.html
#define _DEFAULT_SOURCE
#define _BSD_SOURCE

// Include files from the arduino platform do not provide these conversions:
#ifdef ARDUINO
#define htons(x) ( ((x)<< 8 & 0xFF00) | ((x)>> 8 & 0x00FF) )
#define ntohs(x) htons(x)
#else
#define HAVE_TIME_H 1
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_TIME_H
#include <time.h>
#endif

#include "caprotocolmessage.h"
#include "logger.h"
#include "oic_malloc.h"

// ARM GCC compiler doesnt define srandom function.
#if defined(ARDUINO) && !defined(ARDUINO_ARCH_SAM)
#define HAVE_SRANDOM 1
#endif

#define TAG "CA"

#define CA_BUFSIZE 128
#define CA_PDU_MIN_SIZE 4

static const char COAP_HEADER[] = "coap://[::]/";

static uint32_t SEED = 0;

/**
 * Helper that uses libcoap to parse either the path or the parameters of a URI
 * and populate the supplied options list.
 *
 * @param str the input partial URI string (either path or query).
 * @param length the length of the supplied partial URI.
 * @param target the part of the URI to parse (either COAP_OPTION_URI_PATH
 *        or COAP_OPTION_URI_QUERY).
 * @param optlist options information.
 */
static void CAParseUriPartial(const unsigned char *str, size_t length,
                              int target, coap_list_t **optlist);

void CAGetRequestInfoFromPdu(const coap_pdu_t *pdu, CARequestInfo_t *outReqInfo,
                             char *outUri, uint32_t buflen)
{
    OIC_LOG(DEBUG, TAG, "IN");

    if (NULL == pdu)
    {
        OIC_LOG(ERROR, TAG, "pdu NULL");
        return;
    }

    uint32_t code = CA_NOT_FOUND;
    CAGetInfoFromPDU(pdu, &code, &(outReqInfo->info), outUri, buflen);
    outReqInfo->method = code;
    OIC_LOG(DEBUG, TAG, "OUT");
}

void CAGetResponseInfoFromPdu(const coap_pdu_t *pdu, CAResponseInfo_t *outResInfo,
                              char *outUri, uint32_t buflen)
{
    OIC_LOG(DEBUG, TAG, "IN");
    if (NULL == pdu)
    {
        OIC_LOG(ERROR, TAG, "pdu NULL");
        return;
    }

    uint32_t code = CA_NOT_FOUND;
    CAGetInfoFromPDU(pdu, &code, &(outResInfo->info), outUri, buflen);
    outResInfo->result = code;
    OIC_LOG(DEBUG, TAG, "OUT");
}

coap_pdu_t *CAGeneratePdu(const char *uri, uint32_t code, const CAInfo_t info)
{
    OIC_LOG(DEBUG, TAG, "IN");

    if (NULL == uri)
    {
        OIC_LOG(ERROR, TAG, "uri NULL");
        return NULL;
    }

    uint32_t length = strlen(uri);
    if (CA_MAX_URI_LENGTH < length)
    {
        OIC_LOG(ERROR, TAG, "URI len err");
        return NULL;
    }

    uint32_t uriLength = length + sizeof(COAP_HEADER);
    char *coapUri = (char *) OICCalloc(1, uriLength * sizeof(char));
    if (NULL == coapUri)
    {
        OIC_LOG(ERROR, TAG, "error");
        return NULL;
    }

    strcat(coapUri, COAP_HEADER);
    strcat(coapUri, uri);

    // parsing options in URI
    coap_list_t *optlist = NULL;
    CAParseURI(coapUri, &optlist);
    OICFree(coapUri);
    coapUri = NULL;

    // parsing options in HeadOption
    CAParseHeadOption(code, info, &optlist);

    coap_pdu_t *pdu = CAGeneratePduImpl((code_t) code, optlist, info, info.payload);
    if (NULL == pdu)
    {
        OIC_LOG(ERROR, TAG, "pdu NULL");
        return NULL;
    }

    // free option list
    coap_delete_list(optlist);

    // pdu print method : coap_show_pdu(pdu);
    OIC_LOG(DEBUG, TAG, "OUT");
    return pdu;
}

coap_pdu_t *CAParsePDU(const char *data, uint32_t length, uint32_t *outCode)
{
    OIC_LOG(DEBUG, TAG, "IN");
    coap_pdu_t *outpdu = coap_new_pdu();

    if (0 >= coap_pdu_parse((unsigned char *) data, length, outpdu))
    {
        OIC_LOG(ERROR, TAG, "pdu parse failed");
        coap_delete_pdu(outpdu);
        return NULL;
    }

    if (outCode)
    {
        (*outCode) = (uint32_t) outpdu->hdr->code;
    }
    OIC_LOG(DEBUG, TAG, "OUT");
    return outpdu;
}

coap_pdu_t *CAGeneratePduImpl(code_t code, coap_list_t *options,
                              const CAInfo_t info, const char *payload)
{
    OIC_LOG(DEBUG, TAG, "IN");

    coap_pdu_t *pdu = coap_new_pdu();
    if (!pdu)
    {
        OIC_LOG(ERROR, TAG, "Out of memory");
        return NULL;
    }

    OIC_LOG_V(DEBUG, TAG, "msgId: %d", info.messageId);
    if (CA_MSG_ACKNOWLEDGE == info.type || CA_MSG_RESET == info.type)
    {
        pdu->hdr->id = htons(info.messageId);
    }
    else
    {
        uint16_t message_id;
        if (info.messageId == 0)
        {
            /* initialize message id */
            prng((unsigned char *)&message_id, sizeof(message_id));
            OIC_LOG_V(DEBUG, TAG, "gen msg id=%d", message_id);
        }
        else
        {
            /* use saved message id */
            message_id = info.messageId;
        }
        pdu->hdr->id = htons(message_id);
    }
    pdu->hdr->type = info.type;
    pdu->hdr->code = COAP_RESPONSE_CODE(code);

    if (info.token)
    {
        uint32_t tokenLength = info.tokenLength;
        OIC_LOG_V(DEBUG, TAG, "token info token length: %d, token :", tokenLength);
        OIC_LOG_BUFFER(DEBUG, TAG, (const uint8_t *)info.token, tokenLength);

        int32_t ret = coap_add_token(pdu, tokenLength, (const unsigned char *) info.token);
        if (0 == ret)
        {
            OIC_LOG(DEBUG, TAG, "can't add token");
        }
    }

    if (options)
    {
        for (coap_list_t *opt = options; opt; opt = opt->next)
        {
            OIC_LOG_V(DEBUG, TAG, "[%s] opt will be added.",
                      COAP_OPTION_DATA(*(coap_option *) opt->data));
            coap_add_option(pdu, COAP_OPTION_KEY(*(coap_option *) opt->data),
                            COAP_OPTION_LENGTH(*(coap_option *) opt->data),
                            COAP_OPTION_DATA(*(coap_option *) opt->data));
        }
    }

    if (NULL != payload)
    {
        uint32_t len = strlen(payload);
        OIC_LOG_V(DEBUG, TAG, "add data, payload:%s", payload);
        coap_add_data(pdu, len, (const unsigned char *) payload);
    }

    OIC_LOG(DEBUG, TAG, "OUT");
    return pdu;
}

void CAParseURI(const char *uriInfo, coap_list_t **optlist)
{
    OIC_LOG(DEBUG, TAG, "IN");

    OIC_LOG_V(DEBUG, TAG, "url : %s", uriInfo);

    /* split arg into Uri-* options */
    coap_uri_t uri;
    coap_split_uri((unsigned char *) uriInfo, strlen(uriInfo), &uri);

    if (uri.port != COAP_DEFAULT_PORT)
    {
        unsigned char portbuf[2] = {0};
        coap_insert(optlist,
                    CACreateNewOptionNode(COAP_OPTION_URI_PORT,
                                          coap_encode_var_bytes(portbuf, uri.port), portbuf),
                    CAOrderOpts);
    }


    CAParseUriPartial(uri.path.s, uri.path.length, COAP_OPTION_URI_PATH, optlist);

    CAParseUriPartial(uri.query.s, uri.query.length, COAP_OPTION_URI_QUERY, optlist);

    OIC_LOG(DEBUG, TAG, "OUT");
}

void CAParseUriPartial(const unsigned char *str, size_t length,
                       int target, coap_list_t **optlist)
{
    if ((target != COAP_OPTION_URI_PATH) && (target != COAP_OPTION_URI_QUERY))
    {
        // should never occur. Log just in case.
        OIC_LOG(DEBUG, TAG, "Unexpected URI component.");
    }
    else if (str && length)
    {
        unsigned char uriBuffer[CA_BUFSIZE] = { 0 };
        unsigned char *pBuf = uriBuffer;
        size_t buflen = sizeof(uriBuffer);
        int res = (target == COAP_OPTION_URI_PATH) ?
            coap_split_path(str, length, pBuf, &buflen) :
            coap_split_query(str, length, pBuf, &buflen);

        if (res > 0)
        {
            size_t prevIdx = 0;
            while (res--)
            {
                coap_insert(optlist,
                            CACreateNewOptionNode(target, COAP_OPT_LENGTH(pBuf),
                                                  COAP_OPT_VALUE(pBuf)),
                            CAOrderOpts);

                size_t optSize = COAP_OPT_SIZE(pBuf);
                if ((prevIdx + optSize) < buflen)
                {
                    pBuf += optSize;
                    prevIdx += optSize;
                }
            }
        }
        else
        {
            OIC_LOG_V(DEBUG, TAG, "Problem parsing URI : %d for %d", res, target);
        }
    }
}

void CAParseHeadOption(uint32_t code, const CAInfo_t info, coap_list_t **optlist)
{
    OIC_LOG(DEBUG, TAG, "IN");

    OIC_LOG_V(DEBUG, TAG, "parse Head Opt: %d", info.numOptions);

    for (uint32_t i = 0; i < info.numOptions; i++)
    {
        uint32_t id = info.options[i].optionID;
        if (COAP_OPTION_URI_PATH == id || COAP_OPTION_URI_QUERY == id)
        {
            OIC_LOG_V(DEBUG, TAG, "not Header Opt: %d", id);
        }
        else
        {
            OIC_LOG_V(DEBUG, TAG, "Head Opt ID: %d", info.options[i].optionID);
            OIC_LOG_V(DEBUG, TAG, "Head Opt data: %s", info.options[i].optionData);
            OIC_LOG_V(DEBUG, TAG, "Head Opt len: %d", info.options[i].optionLength);

            coap_insert(optlist,
                        CACreateNewOptionNode(info.options[i].optionID,
                                              info.options[i].optionLength,
                                              info.options[i].optionData), CAOrderOpts);
        }
    }

    OIC_LOG(DEBUG, TAG, "OUT");
}

coap_list_t *CACreateNewOptionNode(uint16_t key, uint32_t length,
                                   const uint8_t *data)
{
    OIC_LOG(DEBUG, TAG, "IN");

    if (!data)
    {
        OIC_LOG(ERROR, TAG, "invalid pointer parameter");
        return NULL;
    }

    coap_option *option = coap_malloc(sizeof(coap_option) + length + 1);
    if (!option)
    {
        OIC_LOG(ERROR, TAG, "Out of memory");
        return NULL;
    }
    memset(option, 0, sizeof(coap_option) + length + 1);

    COAP_OPTION_KEY(*option) = key;
    COAP_OPTION_LENGTH(*option) = length;
    memcpy(COAP_OPTION_DATA(*option), data, length);

    /* we can pass NULL here as delete function since option is released automatically  */
    coap_list_t *node = coap_new_listnode(option, NULL);

    if (!node)
    {
        OIC_LOG(DEBUG, TAG, "new_listnode rets NULL");
        coap_free(option);
        return NULL;
    }
    //coap_free(option);
    OIC_LOG(DEBUG, TAG, "OUT");
    return node;
}

int CAOrderOpts(void *a, void *b)
{
    OIC_LOG(DEBUG, TAG, "IN");
    if (!a || !b)
    {
        return a < b ? -1 : 1;
    }

    if (COAP_OPTION_KEY(*(coap_option *)a) < COAP_OPTION_KEY(*(coap_option * )b))
    {
        return -1;
    }

    OIC_LOG(DEBUG, TAG, "OUT");
    return COAP_OPTION_KEY(*(coap_option *)a) == COAP_OPTION_KEY(*(coap_option * )b);
}

uint32_t CAGetOptionCount(coap_opt_iterator_t opt_iter)
{
    OIC_LOG(DEBUG, TAG, "IN");
    uint32_t count = 0;
    coap_opt_t *option;

    while ((option = coap_option_next(&opt_iter)))
    {
        if (COAP_OPTION_URI_PATH != opt_iter.type && COAP_OPTION_URI_QUERY != opt_iter.type)
        {
            count++;
        }
    }

    OIC_LOG(DEBUG, TAG, "OUT");
    return count;
}

void CAGetInfoFromPDU(const coap_pdu_t *pdu, uint32_t *outCode, CAInfo_t *outInfo,
                      char *outUri, uint32_t buflen)
{
    OIC_LOG(DEBUG, TAG, "IN");

    if (!pdu || !outCode || !outInfo || !outUri)
    {
        OIC_LOG(ERROR, TAG, "NULL pointer param");
        return;
    }

    coap_opt_iterator_t opt_iter;
    coap_option_iterator_init((coap_pdu_t *) pdu, &opt_iter, COAP_OPT_ALL);

    // set code
    if (outCode)
    {
        (*outCode) = (uint32_t) CA_RESPONSE_CODE(pdu->hdr->code);
    }

    // init HeaderOption list
    uint32_t count = CAGetOptionCount(opt_iter);

    memset(outInfo, 0, sizeof(*outInfo));
    outInfo->numOptions = count;

    // set type
    outInfo->type = pdu->hdr->type;

    // set message id
    outInfo->messageId = ntohs(pdu->hdr->id);

    if (count > 0)
    {
        outInfo->options = (CAHeaderOption_t *) OICCalloc(count, sizeof(CAHeaderOption_t));
        if (outInfo->options == NULL)
        {
            OIC_LOG(ERROR, TAG, "Out of memory");
            return;
        }
    }

    coap_opt_t *option;
    char optionResult[CA_MAX_URI_LENGTH] = {0};
    uint32_t idx = 0;
    uint32_t optionLength = 0;
    bool isfirstsetflag = false;
    bool isQueryBeingProcessed = false;

    while ((option = coap_option_next(&opt_iter)))
    {
        char buf[COAP_MAX_PDU_SIZE] = {0};
        if (CAGetOptionData((uint8_t *)(COAP_OPT_VALUE(option)),
                            COAP_OPT_LENGTH(option), (uint8_t *)buf, sizeof(buf)))
        {
            OIC_LOG_V(DEBUG, TAG, "COAP URI element : %s", buf);
            uint32_t bufLength = strlen(buf);
            if (COAP_OPTION_URI_PATH == opt_iter.type || COAP_OPTION_URI_QUERY == opt_iter.type)
            {
                if (false == isfirstsetflag)
                {
                    isfirstsetflag = true;
                    optionResult[optionLength] = '/';
                    optionLength++;
                    // Make sure there is enough room in the optionResult buffer
                    if ((optionLength + bufLength) < sizeof(optionResult))
                    {
                        memcpy(optionResult + optionLength, buf, bufLength);
                        optionLength += bufLength;
                    }
                    else
                    {
                        goto exit;
                    }
                }
                else
                {
                    if (COAP_OPTION_URI_PATH == opt_iter.type)
                    {
                        // Make sure there is enough room in the optionResult buffer
                        if (optionLength < sizeof(optionResult))
                        {
                            optionResult[optionLength] = '/';
                            optionLength++;
                        }
                        else
                        {
                            goto exit;
                        }
                    }
                    else if (COAP_OPTION_URI_QUERY == opt_iter.type)
                    {
                        if(false == isQueryBeingProcessed)
                        {
                            // Make sure there is enough room in the optionResult buffer
                            if (optionLength < sizeof(optionResult))
                            {
                                optionResult[optionLength] = '?';
                                optionLength++;
                                isQueryBeingProcessed = true;
                            }
                            else
                            {
                                goto exit;
                            }
                        }
                        else
                        {
                            // Make sure there is enough room in the optionResult buffer
                            if (optionLength < sizeof(optionResult))
                            {
                                optionResult[optionLength] = '&';
                                optionLength++;
                            }
                            else
                            {
                                goto exit;
                            }
                        }
                    }
                    // Make sure there is enough room in the optionResult buffer
                    if ((optionLength + bufLength) < sizeof(optionResult))
                    {
                        memcpy(optionResult + optionLength, buf, bufLength);
                        optionLength += bufLength;
                    }
                    else
                    {
                        goto exit;
                    }
                }
            }
            else
            {
                if (idx < count)
                {
                    uint32_t length = bufLength;

                    if (length <= CA_MAX_HEADER_OPTION_DATA_LENGTH)
                    {
                        outInfo->options[idx].optionID = opt_iter.type;
                        outInfo->options[idx].optionLength = length;
                        outInfo->options[idx].protocolID = CA_COAP_ID;
                        memcpy(outInfo->options[idx].optionData, buf, length);
                        idx++;
                    }
                }
            }
        }
    }

    // set token data
    if (pdu->hdr->token_length > 0)
    {
        OIC_LOG_V(DEBUG, TAG, "inside token length : %d", pdu->hdr->token_length);
        outInfo->token = (char *) OICMalloc(pdu->hdr->token_length);
        if (NULL == outInfo->token)
        {
            OIC_LOG(ERROR, TAG, "memory allocation failed");
            OICFree(outInfo->options);
            return;
        }
        memcpy(outInfo->token, pdu->hdr->token, pdu->hdr->token_length);
    }

    outInfo->tokenLength = pdu->hdr->token_length;

    // set payload data
    if (NULL != pdu->data)
    {
        uint32_t payloadLength = strlen((const char *) pdu->data) + 1;
        OIC_LOG(DEBUG, TAG, "inside pdu->data");
        outInfo->payload = (char *) OICMalloc(payloadLength + 1);
        if (outInfo->payload == NULL)
        {
            OIC_LOG(ERROR, TAG, "Out of memory");
            OICFree(outInfo->options);
            OICFree(outInfo->token);
            return;
        }
        memcpy(outInfo->payload, pdu->data, payloadLength);
        outInfo->payload[payloadLength] = '\0';
    }

    uint32_t length = strlen(optionResult);
    OIC_LOG_V(DEBUG, TAG, "made URL length: %d, %d, %d...\n", length, buflen, strlen(outUri));
    if (buflen >= length)
    {
        memcpy(outUri, optionResult, length);
        outUri[length] = '\0';
#ifdef ARDUINO
        OIC_LOG_V(DEBUG, TAG, "made URL:%s\n", optionResult);
#else
        OIC_LOG_V(DEBUG, TAG, "made URL : %s, %s\n", optionResult, outUri);
#endif
    }
    OIC_LOG(DEBUG, TAG, "OUT");
    return;

exit:
    OIC_LOG(ERROR, TAG, "buffer too small");
    OICFree(outInfo->options);
}

CAResult_t CAGenerateTokenInternal(CAToken_t *token, uint8_t tokenLength)
{
    OIC_LOG(DEBUG, TAG, "IN");

    if(!token)
    {
        OIC_LOG(ERROR, TAG, "invalid token pointer");
        return CA_STATUS_INVALID_PARAM;
    }

    if((tokenLength > CA_MAX_TOKEN_LEN) || (0 == tokenLength))
    {
        OIC_LOG(ERROR, TAG, "invalid token length");
        return CA_STATUS_INVALID_PARAM;
    }

    if (SEED == 0)
    {
#ifdef ARDUINO
        SEED = now();
#else
        SEED = time(NULL);
#endif
        if (SEED == -1)
        {
            OIC_LOG(DEBUG, TAG, "Failed to Create Seed!");
            SEED = 0;
            return CA_STATUS_FAILED;
        }
#if HAVE_SRANDOM
        srandom(SEED);
#else
        srand(SEED);
#endif
    }

    // memory allocation
    char *temp = (char *) OICCalloc(tokenLength, sizeof(char));
    if (NULL == temp)
    {
        OIC_LOG(ERROR, TAG, "CAGenerateTokenInternal, Memory allocation failed !");
        return CA_MEMORY_ALLOC_FAILED;
    }

    // set random byte
    for (uint8_t index = 0; index < tokenLength; index++)
    {
        // use valid characters
#ifdef ARDUINO
        temp[index] = rand() & 0x00FF;
#else
        temp[index] = random() & 0x00FF;
#endif
    }

    // save token
    *token = temp;

    OIC_LOG_V(DEBUG, TAG, "token info token length: %d, token :", tokenLength);
    OIC_LOG_BUFFER(DEBUG, TAG, (const uint8_t *)token, tokenLength);

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

void CADestroyTokenInternal(CAToken_t token)
{
    OIC_LOG(DEBUG, TAG, "IN");
    OICFree(token);
    OIC_LOG(DEBUG, TAG, "OUT");
}

void CADestroyInfo(CAInfo_t *info)
{
    OIC_LOG(DEBUG, TAG, "IN");

    if (NULL != info)
    {
        if (NULL != info->options)
        {
            OIC_LOG(DEBUG, TAG, "free opt");
            OICFree(info->options);
        }

        if (NULL != info->token)
        {
            OIC_LOG(DEBUG, TAG, "free tok");
            OICFree(info->token);
        }

        if (NULL != info->payload)
        {
            OIC_LOG(DEBUG, TAG, "free payld");
            OICFree(info->payload);
        }
    }

    OIC_LOG(DEBUG, TAG, "OUT");
}

uint32_t CAGetOptionData(const uint8_t *data, uint32_t len, uint8_t *option, uint32_t buflen)
{
    assert(data || 0 == len);

    if (0 == buflen || 0 == len)
    {
        OIC_LOG(ERROR, TAG, "len 0");
        return 0;
    }

    if (NULL == data || NULL == option)
    {
        OIC_LOG(ERROR, TAG, "data/option NULL");
        return 0;
    }

    if (buflen <= len)
    {
        OIC_LOG(ERROR, TAG, "option buffer too small");
        return 0;
    }

    memcpy(option, data, len);
    option[len] = '\0';

    return len;
}

CAMessageType_t CAGetMessageTypeFromPduBinaryData(const void *pdu, uint32_t size)
{
    // pdu minimum size is 4 byte.
    if (size < CA_PDU_MIN_SIZE)
    {
        OIC_LOG(DEBUG, TAG, "min size");
        return CA_MSG_NONCONFIRM;
    }

    coap_hdr_t *hdr = (coap_hdr_t *) pdu;
    if (NULL == hdr)
    {
        return CA_MSG_NONCONFIRM;
    }

    return (CAMessageType_t) hdr->type;
}

uint16_t CAGetMessageIdFromPduBinaryData(const void *pdu, uint32_t size)
{
    // pdu minimum size is 4 byte.
    if (size < CA_PDU_MIN_SIZE)
    {
        OIC_LOG(DEBUG, TAG, "min size");
        return 0;
    }

    coap_hdr_t *hdr = (coap_hdr_t *) pdu;
    if (NULL == hdr)
    {
        return 0;
    }

    return ntohs(hdr->id);
}

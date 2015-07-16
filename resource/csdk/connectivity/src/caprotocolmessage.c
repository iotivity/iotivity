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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#ifdef HAVE_TIME_H
#include <time.h>
#endif

#include "caprotocolmessage.h"
#include "logger.h"
#include "oic_malloc.h"
#include "oic_string.h"

// ARM GCC compiler doesnt define srandom function.
#if defined(ARDUINO) && !defined(ARDUINO_ARCH_SAM)
#define HAVE_SRANDOM 1
#endif

#define TAG "CA_PRTCL_MSG"

/**
 * @def VERIFY_NON_NULL_RET
 * @brief Macro to verify the validity of input argument
 */
#define VERIFY_NON_NULL_RET(arg, log_tag, log_message,ret) \
    if (NULL == arg ){ \
        OIC_LOG_V(ERROR, log_tag, "Invalid input:%s", log_message); \
        return ret; \
    }

#define CA_BUFSIZE (128)
#define CA_PDU_MIN_SIZE (4)
#define CA_PORT_BUFFER_SIZE (4)

static const char COAP_URI_HEADER[] = "coap://[::]/";

static uint32_t SEED = 0;

CAResult_t CAGetRequestInfoFromPDU(const coap_pdu_t *pdu, CARequestInfo_t *outReqInfo)
{
    OIC_LOG(DEBUG, TAG, "IN");

    if (NULL == pdu || NULL == outReqInfo)
    {
        OIC_LOG(ERROR, TAG, "parameter is null");
        return CA_STATUS_INVALID_PARAM;
    }

    uint32_t code = CA_NOT_FOUND;
    CAResult_t ret = CAGetInfoFromPDU(pdu, &code, &(outReqInfo->info));
    outReqInfo->method = code;

    OIC_LOG(DEBUG, TAG, "OUT");
    return ret;
}

CAResult_t CAGetResponseInfoFromPDU(const coap_pdu_t *pdu, CAResponseInfo_t *outResInfo)
{
    OIC_LOG(DEBUG, TAG, "IN");

    if (NULL == pdu || NULL == outResInfo)
    {
        OIC_LOG(ERROR, TAG, "parameter is null");
        return CA_STATUS_INVALID_PARAM;
    }

    uint32_t code = CA_NOT_FOUND;
    CAResult_t ret = CAGetInfoFromPDU(pdu, &code, &(outResInfo->info));
    outResInfo->result = code;

    OIC_LOG(DEBUG, TAG, "OUT");
    return ret;
}

CAResult_t CAGetErrorInfoFromPDU(const coap_pdu_t *pdu, CAErrorInfo_t *errorInfo)
{
    OIC_LOG(DEBUG, TAG, "IN");

    if (!pdu)
    {
        OIC_LOG(ERROR, TAG, "parameter is null");
        return CA_STATUS_INVALID_PARAM;
    }

    uint32_t code = 0;
    CAResult_t ret = CAGetInfoFromPDU(pdu, &code, &errorInfo->info);
    OIC_LOG(DEBUG, TAG, "OUT");
    return ret;
}

coap_pdu_t *CAGeneratePDU(uint32_t code, const CAInfo_t *info)
{
    OIC_LOG(DEBUG, TAG, "IN");

    coap_pdu_t *pdu = NULL;

    // RESET have to use only 4byte (empty message)
    // and ACKNOWLEDGE can use empty message when code is empty.
    if (CA_MSG_RESET == info->type || (CA_EMPTY == code && CA_MSG_ACKNOWLEDGE == info->type))
    {
        OIC_LOG(DEBUG, TAG, "code is empty");
        if (!(pdu = CAGeneratePDUImpl((code_t) code, NULL, info, NULL, 0)))
        {
            OIC_LOG(ERROR, TAG, "pdu NULL");
            return NULL;
        }
    }
    else
    {
        coap_list_t *optlist = NULL;

        if (CA_MSG_ACKNOWLEDGE != info->type)
        {
            const char *uri = info->resourceUri;
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

            uint32_t uriLength = length + sizeof(COAP_URI_HEADER);
            char *coapUri = (char *) OICCalloc(1, uriLength);
            if (NULL == coapUri)
            {
                OIC_LOG(ERROR, TAG, "out of memory");
                return NULL;
            }
            OICStrcat(coapUri, uriLength, COAP_URI_HEADER);
            OICStrcat(coapUri, uriLength, uri);

            // parsing options in URI
            CAResult_t res = CAParseURI(coapUri, &optlist);
            if (CA_STATUS_OK != res)
            {
                if (optlist)
                {
                    coap_delete_list(optlist);
                }
                OICFree(coapUri);
                return NULL;
            }

            OICFree(coapUri);
        }
        // parsing options in HeadOption
        CAResult_t ret = CAParseHeadOption(code, info, &optlist);
        if (CA_STATUS_OK != ret)
        {
            coap_delete_list(optlist);
            return NULL;
        }
        pdu = CAGeneratePDUImpl((code_t)code, optlist, info, info->payload, info->payloadSize);
        if (NULL == pdu)
        {
            OIC_LOG(ERROR, TAG, "pdu NULL");
            coap_delete_list(optlist);
            return NULL;
        }

        // free option list
        coap_delete_list(optlist);
    }

    // pdu print method : coap_show_pdu(pdu);
    OIC_LOG(DEBUG, TAG, "OUT");
    return pdu;
}

coap_pdu_t *CAParsePDU(const char *data, uint32_t length, uint32_t *outCode)
{
    OIC_LOG(DEBUG, TAG, "IN");

    if (NULL == data)
    {
        OIC_LOG(ERROR, TAG, "data is null");
        return NULL;
    }

    coap_pdu_t *outpdu = coap_new_pdu();
    if (NULL == outpdu)
    {
        OIC_LOG(ERROR, TAG, "outpdu is null");
        return NULL;
    }

    if (0 >= coap_pdu_parse((unsigned char *) data, length, outpdu))
    {
        OIC_LOG(ERROR, TAG, "pdu parse failed");
        coap_delete_pdu(outpdu);
        return NULL;
    }

    if (outCode)
    {
        (*outCode) = (uint32_t) CA_RESPONSE_CODE(outpdu->hdr->code);
    }

    OIC_LOG(DEBUG, TAG, "OUT");
    return outpdu;
}

coap_pdu_t *CAGeneratePDUImpl(code_t code, coap_list_t *options, const CAInfo_t *info,
                              const uint8_t *payload, size_t payloadSize)
{
    OIC_LOG(DEBUG, TAG, "IN");
    VERIFY_NON_NULL_RET(info, TAG, "info is NULL", NULL);

    coap_pdu_t *pdu = coap_new_pdu();

    if (NULL == pdu)
    {
        OIC_LOG(ERROR, TAG, "malloc failed");
        return NULL;
    }

    OIC_LOG_V(DEBUG, TAG, "msgID is %d", info->messageId);
    uint16_t message_id;
    if (0 == info->messageId)
    {
        /* initialize message id */
        prng((uint8_t * ) &message_id, sizeof(message_id));

        OIC_LOG_V(DEBUG, TAG, "gen msg id=%d", message_id);
    }
    else
    {
        /* use saved message id */
        message_id = info->messageId;
    }
    pdu->hdr->id = message_id;
    OIC_LOG_V(DEBUG, TAG, "messageId in pdu is %d, %d", message_id, pdu->hdr->id);

    pdu->hdr->type = info->type;
    pdu->hdr->code = COAP_RESPONSE_CODE(code);

    if (info->token && CA_EMPTY != code)
    {
        uint32_t tokenLength = info->tokenLength;
        OIC_LOG_V(DEBUG, TAG, "token info token length: %d, token :", tokenLength);
        OIC_LOG_BUFFER(DEBUG, TAG, (const uint8_t *)info->token, tokenLength);

        int32_t ret = coap_add_token(pdu, tokenLength, (unsigned char *)info->token);
        if (0 == ret)
        {
            OIC_LOG(ERROR, TAG, "can't add token");
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
        coap_add_data(pdu, payloadSize, (const unsigned char *) payload);
    }

    OIC_LOG(DEBUG, TAG, "OUT");
    return pdu;
}

CAResult_t CAParseURI(const char *uriInfo, coap_list_t **optlist)
{
    OIC_LOG(DEBUG, TAG, "IN");

    if (NULL == uriInfo)
    {
        OIC_LOG(ERROR, TAG, "uriInfo is null");
        return CA_STATUS_INVALID_PARAM;
    }

    OIC_LOG_V(DEBUG, TAG, "url : %s", uriInfo);

    if (NULL == optlist)
    {
        OIC_LOG(ERROR, TAG, "optlist is null");
        return CA_STATUS_INVALID_PARAM;
    }

    /* split arg into Uri-* options */
    coap_uri_t uri;
    coap_split_uri((unsigned char *) uriInfo, strlen(uriInfo), &uri);

    if (uri.port != COAP_DEFAULT_PORT)
    {
        unsigned char portbuf[CA_PORT_BUFFER_SIZE] = { 0 };
        int ret = coap_insert(optlist,
                              CACreateNewOptionNode(COAP_OPTION_URI_PORT,
                                                    coap_encode_var_bytes(portbuf, uri.port),
                                                    (char *)portbuf),
                              CAOrderOpts);
        if (ret <= 0)
        {
            return CA_STATUS_INVALID_PARAM;
        }
    }

    if (uri.path.s && uri.path.length)
    {
        CAResult_t ret = CAParseUriPartial(uri.path.s, uri.path.length,
                                           COAP_OPTION_URI_PATH, optlist);
        if (CA_STATUS_OK != ret)
        {
            OIC_LOG(ERROR, TAG, "CAParseUriPartial failed(uri path)");
            return ret;
        }
    }

    if (uri.query.s && uri.query.length)
    {
        CAResult_t ret = CAParseUriPartial(uri.query.s, uri.query.length, COAP_OPTION_URI_QUERY,
                                           optlist);
        if (CA_STATUS_OK != ret)
        {
            OIC_LOG(ERROR, TAG, "CAParseUriPartial failed(uri query)");
            return ret;
        }
    }

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAParseUriPartial(const unsigned char *str, size_t length, int target,
                             coap_list_t **optlist)
{
    if (!optlist)
    {
        OIC_LOG(ERROR, TAG, "optlist is null");
        return CA_STATUS_INVALID_PARAM;
    }

    if ((target != COAP_OPTION_URI_PATH) && (target != COAP_OPTION_URI_QUERY))
    {
        // should never occur. Log just in case.
        OIC_LOG(DEBUG, TAG, "Unexpected URI component.");
        return CA_NOT_SUPPORTED;
    }
    else if (str && length)
    {
        unsigned char uriBuffer[CA_BUFSIZE] = { 0 };
        unsigned char *pBuf = uriBuffer;
        size_t buflen = sizeof(uriBuffer);
        int res = (target == COAP_OPTION_URI_PATH) ? coap_split_path(str, length, pBuf, &buflen) :
                                                     coap_split_query(str, length, pBuf, &buflen);

        if (res > 0)
        {
            size_t prevIdx = 0;
            while (res--)
            {
                int ret = coap_insert(optlist,
                                      CACreateNewOptionNode(target, COAP_OPT_LENGTH(pBuf),
                                                            (char *)COAP_OPT_VALUE(pBuf)),
                                      CAOrderOpts);
                if (ret <= 0)
                {
                    return CA_STATUS_INVALID_PARAM;
                }

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
            OIC_LOG_V(ERROR, TAG, "Problem parsing URI : %d for %d", res, target);
            return CA_STATUS_FAILED;
        }
    }
    else
    {
        OIC_LOG(ERROR, TAG, "str or length is not available");
        return CA_STATUS_FAILED;
    }

    return CA_STATUS_OK;
}

CAResult_t CAParseHeadOption(uint32_t code, const CAInfo_t *info, coap_list_t **optlist)
{
    OIC_LOG(DEBUG, TAG, "IN");
    VERIFY_NON_NULL_RET(info, TAG, "info is NULL", CA_STATUS_INVALID_PARAM);

    OIC_LOG_V(DEBUG, TAG, "parse Head Opt: %d", info->numOptions);

    if (!optlist)
    {
        OIC_LOG(ERROR, TAG, "optlist is null");
        return CA_STATUS_INVALID_PARAM;
    }

    for (uint32_t i = 0; i < info->numOptions; i++)
    {
        if(!(info->options + i))
        {
            OIC_LOG(ERROR, TAG, "options is not available");
            return CA_STATUS_FAILED;
        }

        uint32_t id = (info->options + i)->optionID;
        if (COAP_OPTION_URI_PATH == id || COAP_OPTION_URI_QUERY == id)
        {
            OIC_LOG_V(DEBUG, TAG, "not Header Opt: %d", id);
        }
        else
        {
            if ((info->options + i)->optionData && (info->options + i)->optionLength > 0)
            {
                OIC_LOG_V(DEBUG, TAG, "Head opt ID: %d", id);
                OIC_LOG_V(DEBUG, TAG, "Head opt data: %s", (info->options + i)->optionData);
                OIC_LOG_V(DEBUG, TAG, "Head opt length: %d", (info->options + i)->optionLength);
                int ret = coap_insert(optlist,
                                      CACreateNewOptionNode(id, (info->options + i)->optionLength,
                                                            (info->options + i)->optionData),
                                      CAOrderOpts);
                if (ret <= 0)
                {
                    return CA_STATUS_INVALID_PARAM;
                }
            }
        }
    }

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

coap_list_t *CACreateNewOptionNode(uint16_t key, uint32_t length, const char *data)
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
        OIC_LOG(ERROR, TAG, "node is NULL");
        coap_free(option);
        return NULL;
    }

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

    if (COAP_OPTION_KEY(*(coap_option *) a) < COAP_OPTION_KEY(*(coap_option * ) b))
    {
        return -1;
    }

    OIC_LOG(DEBUG, TAG, "OUT");
    return COAP_OPTION_KEY(*(coap_option *) a) == COAP_OPTION_KEY(*(coap_option * ) b);
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

CAResult_t CAGetInfoFromPDU(const coap_pdu_t *pdu, uint32_t *outCode, CAInfo_t *outInfo)
{
    OIC_LOG(DEBUG, TAG, "IN");

    if (!pdu || !outCode || !outInfo)
    {
        OIC_LOG(ERROR, TAG, "NULL pointer param");
        return CA_STATUS_INVALID_PARAM;
    }

    coap_opt_iterator_t opt_iter;
    coap_option_iterator_init((coap_pdu_t *) pdu, &opt_iter, COAP_OPT_ALL);

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
    outInfo->messageId = pdu->hdr->id;

    if (count > 0)
    {
        outInfo->options = (CAHeaderOption_t *) OICCalloc(count, sizeof(CAHeaderOption_t));
        if (NULL == outInfo->options)
        {
            OIC_LOG(ERROR, TAG, "Out of memory");
            return CA_MEMORY_ALLOC_FAILED;
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
                        memcpy(&optionResult[optionLength], buf, bufLength);
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
                        if (false == isQueryBeingProcessed)
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
                                optionResult[optionLength] = ';';
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
                        memcpy(&optionResult[optionLength], buf, bufLength);
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
            OIC_LOG(ERROR, TAG, "Out of memory");
            OICFree(outInfo->options);
            return CA_MEMORY_ALLOC_FAILED;
        }
        memcpy(outInfo->token, pdu->hdr->token, pdu->hdr->token_length);
    }

    outInfo->tokenLength = pdu->hdr->token_length;

    // set payload data
    size_t dataSize;
    uint8_t *data;
    if (coap_get_data(pdu, &dataSize, &data))
    {
        OIC_LOG(DEBUG, TAG, "inside pdu->data");
        outInfo->payload = (uint8_t *) OICMalloc(dataSize);
        if (NULL == outInfo->payload)
        {
            OIC_LOG(ERROR, TAG, "Out of memory");
            OICFree(outInfo->options);
            OICFree(outInfo->token);
            return CA_MEMORY_ALLOC_FAILED;
        }
        memcpy(outInfo->payload, pdu->data, dataSize);
        outInfo->payloadSize = dataSize;
    }

    uint32_t length = strlen(optionResult);
    OIC_LOG_V(DEBUG, TAG, "URL length:%d", length);

    outInfo->resourceUri = OICMalloc(length + 1);
    if (!outInfo->resourceUri)
    {
        OIC_LOG(ERROR, TAG, "Out of memory");
        OICFree(outInfo->options);
        OICFree(outInfo->token);
        return CA_MEMORY_ALLOC_FAILED;
    }
    memcpy(outInfo->resourceUri, optionResult, length);
    outInfo->resourceUri[length] = '\0';
    OIC_LOG_V(DEBUG, TAG, "made URL : %s, %s", optionResult, outInfo->resourceUri);

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;

exit:
    OIC_LOG(ERROR, TAG, "buffer too small");
    OICFree(outInfo->options);
    return CA_STATUS_FAILED;
}

CAResult_t CAGetTokenFromPDU(const coap_hdr_t *pdu_hdr, CAInfo_t *outInfo)
{
    OIC_LOG(DEBUG, TAG, "IN");
    if (NULL == pdu_hdr)
    {
        OIC_LOG(ERROR, TAG, "pdu_hdr is null");
        return CA_STATUS_INVALID_PARAM;
    }

    if (NULL == outInfo)
    {
        OIC_LOG(ERROR, TAG, "outInfo is null");
        return CA_STATUS_INVALID_PARAM;
    }

    // set token data
    if (pdu_hdr->token_length > 0)
    {
        OIC_LOG_V(DEBUG, TAG, "token len:%d", pdu_hdr->token_length);
        outInfo->token = (char *) OICMalloc(pdu_hdr->token_length);
        if (NULL == outInfo->token)
        {
            OIC_LOG(ERROR, TAG, "out of memory");
            return CA_MEMORY_ALLOC_FAILED;
        }
        memcpy(outInfo->token, pdu_hdr->token, pdu_hdr->token_length);
    }

    outInfo->tokenLength = pdu_hdr->token_length;

    OIC_LOG(DEBUG, TAG, "OUT");

    return CA_STATUS_OK;
}

CAResult_t CAGenerateTokenInternal(CAToken_t *token, uint8_t tokenLength)
{
    OIC_LOG(DEBUG, TAG, "IN");

    if (!token)
    {
        OIC_LOG(ERROR, TAG, "invalid token pointer");
        return CA_STATUS_INVALID_PARAM;
    }

    if ((tokenLength > CA_MAX_TOKEN_LEN) || (0 == tokenLength))
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
            OIC_LOG(ERROR, TAG, "seed is not made");
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
        OIC_LOG(ERROR, TAG, "Out of memory");
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

    OIC_LOG_V(DEBUG, TAG, "token len:%d, token:", tokenLength);
    OIC_LOG_BUFFER(DEBUG, TAG, (const uint8_t *)(*token), tokenLength);

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
        OIC_LOG(DEBUG, TAG, "free options");
        OICFree(info->options);

        OIC_LOG(DEBUG, TAG, "free token");
        OICFree(info->token);

        OIC_LOG(DEBUG, TAG, "free payload");
        OICFree(info->payload);
    }

    OIC_LOG(DEBUG, TAG, "OUT");
}

uint32_t CAGetOptionData(const uint8_t *data, uint32_t len, uint8_t *option, uint32_t buflen)
{
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
    if (NULL == pdu)
    {
        OIC_LOG(ERROR, TAG, "pdu is null");
        return CA_MSG_NONCONFIRM;
    }

    // pdu minimum size is 4 byte.
    if (size < CA_PDU_MIN_SIZE)
    {
        OIC_LOG(ERROR, TAG, "min size");
        return CA_MSG_NONCONFIRM;
    }

    coap_hdr_t *hdr = (coap_hdr_t *) pdu;

    return (CAMessageType_t) hdr->type;
}

uint16_t CAGetMessageIdFromPduBinaryData(const void *pdu, uint32_t size)
{
    if (NULL == pdu)
    {
        OIC_LOG(ERROR, TAG, "pdu is null");
        return 0;
    }

    // pdu minimum size is 4 byte.
    if (size < CA_PDU_MIN_SIZE)
    {
        OIC_LOG(ERROR, TAG, "min size");
        return 0;
    }

    coap_hdr_t *hdr = (coap_hdr_t *) pdu;

    return hdr->id;
}

CAResponseResult_t CAGetCodeFromPduBinaryData(const void *pdu, uint32_t size)
{
    if (NULL == pdu)
    {
        OIC_LOG(ERROR, TAG, "pdu is null");
        return CA_NOT_FOUND;
    }

    // pdu minimum size is 4 byte.
    if (size < CA_PDU_MIN_SIZE)
    {
        OIC_LOG(ERROR, TAG, "min size");
        return CA_NOT_FOUND;
    }

    coap_hdr_t *hdr = (coap_hdr_t *) pdu;

    return (CAResponseResult_t) CA_RESPONSE_CODE(hdr->code);
}

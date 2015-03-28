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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "caprotocolmessage_singlethread.h"
#include "logger.h"
#include "oic_malloc.h"

#define TAG "CPM"

#define CA_BUFSIZE 128
#define CA_PDU_MIN_SIZE 4
static const char COAP_HEADER[] = "coap://[::]/";

static uint32_t SEED = 0;

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
    char *coapUri = (char *) OICCalloc(uriLength, sizeof(char));
    if (NULL == coapUri)
    {
        OIC_LOG(ERROR, TAG, "CAGeneratePdu, Memory allocation failed !");
        return NULL;
    }

    strcat(coapUri, COAP_HEADER);
    strcat(coapUri, uri);

    // parsing options in URI
    coap_list_t *optlist = NULL;
    CAParseURI(coapUri, &optlist);
    OICFree(coapUri);

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

    if (NULL == pdu)
    {
        OIC_LOG(ERROR, TAG, "malloc failed");
        return NULL;
    }

    OIC_LOG_V(DEBUG, TAG, "msgId: %d", info.messageId);
    if(CA_MSG_ACKNOWLEDGE == info.type || CA_MSG_RESET == info.type)
    {
        pdu->hdr->id = htons(info.messageId);
    }
    else
    {
        uint16_t message_id;
        if (info.messageId == 0)
        {
            /* initialize message id */
            prng((unsigned char * )&message_id, sizeof(unsigned short));
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
        OIC_LOG_V(DEBUG, TAG, "tokenLength : %d, token : ", tokenLength);
        OIC_LOG_BUFFER(DEBUG, TAG, info.token, tokenLength);

        int32_t ret = coap_add_token(pdu, tokenLength, (unsigned char *) info.token);
        if (0 == ret)
        {
            OIC_LOG(DEBUG, TAG, "cant add token");
        }
    }

    if(options)
    {
        coap_list_t *opt;
        for (opt = options; opt; opt = opt->next)
        {
            OIC_LOG_V(DEBUG, TAG, "[%s] opt will be added.",
                      COAP_OPTION_DATA(*(coap_option * )opt->data));
            coap_add_option(pdu, COAP_OPTION_KEY(*(coap_option * )opt->data),
                            COAP_OPTION_LENGTH(*(coap_option * )opt->data),
                            COAP_OPTION_DATA(*(coap_option * )opt->data));
        }
    }

    if (NULL != payload)
    {
        uint32_t len = strlen(payload);
        OIC_LOG_V(DEBUG, TAG, "add data,payload:%s", payload);
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

    unsigned char uriBuffer[CA_BUFSIZE] = {0};
    unsigned char *tempBuf = uriBuffer;
    if (uri.path.length)
    {
        uint32_t buflen = CA_BUFSIZE;
        int32_t res = coap_split_path(uri.path.s, uri.path.length, tempBuf, &buflen);

        if (res > 0)
        {
            uint32_t prevIdx = 0;
            while (res--)
            {
                coap_insert(optlist,
                            CACreateNewOptionNode(COAP_OPTION_URI_PATH, COAP_OPT_LENGTH(tempBuf),
                                              COAP_OPT_VALUE(tempBuf)), CAOrderOpts);
                uint32_t optSize = COAP_OPT_SIZE(tempBuf);
                uint32_t nextIdx = prevIdx + optSize;
                if (nextIdx < buflen)
                {
                    tempBuf += optSize;
                    prevIdx += nextIdx;
                }
            }
        }
    }

    if (uri.query.length)
    {
        size_t buflen = CA_BUFSIZE;
        tempBuf = uriBuffer;
        int32_t res = coap_split_query(uri.query.s, uri.query.length, tempBuf, &buflen);
        if (res > 0)
        {
            uint32_t prevIdx = 0;
            while (res--)
            {
                coap_insert(optlist,
                            CACreateNewOptionNode(COAP_OPTION_URI_QUERY, COAP_OPT_LENGTH(tempBuf),
                                          COAP_OPT_VALUE(tempBuf)), CAOrderOpts);

                uint32_t optSize = COAP_OPT_SIZE(tempBuf);
                uint32_t nextIdx = prevIdx + optSize;
                if (nextIdx < buflen)
                {
                    tempBuf += optSize;
                    prevIdx += nextIdx;
                }
            }
        }
    }

    OIC_LOG(DEBUG, TAG, "OUT");
}

void CAParseHeadOption(uint32_t code, const CAInfo_t info, coap_list_t **optlist)
{
    OIC_LOG(DEBUG, TAG, "IN");
    OIC_LOG_V(DEBUG, TAG, "parse Head Opt: %d", info.numOptions);

    uint32_t i;
    for (i = 0; i < info.numOptions; i++)
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
        OIC_LOG(ERROR, TAG, "malloc failed");
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
        (*outCode) = (uint32_t) pdu->hdr->code;
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
            OIC_LOG(ERROR, TAG, "malloc failed");
            return;
        }
    }

    coap_opt_t *option;
    char optionResult[CA_MAX_URI_LENGTH] = {0,};
    uint32_t idx = 0;
    uint32_t optionLength = 0;
    bool isfirstsetflag = false;
    bool isQueryBeingProcessed = false;

    while ((option = coap_option_next(&opt_iter)))
    {

        char buf[COAP_MAX_PDU_SIZE] = {0}; /* need some space for output creation */
        if (CAGetOptionData((uint8_t *)(COAP_OPT_VALUE(option)),
                            COAP_OPT_LENGTH(option), (uint8_t *)buf, sizeof(buf)))
        {
            OIC_LOG_V(DEBUG, TAG, "COAP URI element : %s", buf);
            uint32_t bufLength = strlen((const char *) buf);
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
        OIC_LOG(DEBUG, TAG, "pdu->hdr->token_length > 0");
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
            OIC_LOG(ERROR, TAG, "malloc failed");
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
        OIC_LOG_V(DEBUG, TAG, "made URL:%s\n", optionResult);
    }

    OIC_LOG(DEBUG, TAG, "OUT");
    return;

exit:
    OIC_LOG(ERROR, TAG, "buffer too small");
    OICFree(outInfo->options);
    return;
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
        SEED = now();
        if (SEED == -1)
        {
            OIC_LOG(DEBUG, TAG, "Failed to Create Seed!");
            SEED = 0;
            return CA_STATUS_FAILED;
        }
        srandom(SEED);
    }

    // memory allocation
    char *temp = (char *) OICCalloc(tokenLength, sizeof(char));
    if (NULL == temp)
    {
        OIC_LOG(ERROR, TAG, "Calloc failed");
        return CA_MEMORY_ALLOC_FAILED;
    }

    // set random byte
    for (uint8_t index = 0; index < tokenLength; index++)
    {
        temp[index] = rand() & 0x00FF;
    }

    // save token
    *token = temp;

    OIC_LOG_V(DEBUG, TAG, "gen token tokenLength : %d, token : ", tokenLength);
    OIC_LOG_BUFFER(DEBUG, TAG, *token, tokenLength);
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
    assert(data || len == 0);

    if (0 == buflen|| 0 == len)
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

    if(NULL == hdr)
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

    if(NULL == hdr)
    {
        return 0;
    }

    return ntohs(hdr->id);
}

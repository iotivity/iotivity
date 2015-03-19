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
#include <time.h>

#include "caprotocolmessage.h"
#include "logger.h"
#include "oic_malloc.h"

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

void CAGetRequestInfoFromPdu(const coap_pdu_t *pdu, CARequestInfo_t *outReqInfo, char *outUri,
                             uint32_t buflen)
{
    OIC_LOG(DEBUG, TAG, "CAGetRequestInfoFromPdu IN");

    if (NULL == pdu)
    {
        return;
    }

    uint32_t code = CA_NOT_FOUND;
    CAGetInfoFromPDU(pdu, &code, &(outReqInfo->info), outUri, buflen);
    outReqInfo->method = code;
    OIC_LOG(DEBUG, TAG, "CAGetRequestInfoFromPdu OUT");
}

void CAGetResponseInfoFromPdu(const coap_pdu_t *pdu, CAResponseInfo_t *outResInfo, char *outUri,
                              uint32_t buflen)
{
    OIC_LOG(DEBUG, TAG, "CAGetResponseInfoFromPdu IN");
    if (NULL == pdu)
    {
        return;
    }

    uint32_t code = CA_NOT_FOUND;
    CAGetInfoFromPDU(pdu, &code, &(outResInfo->info), outUri, buflen);
    outResInfo->result = code;
    OIC_LOG(DEBUG, TAG, "CAGetResponseInfoFromPdu OUT");
}

coap_pdu_t *CAGeneratePdu(const char *uri, const uint32_t code, const CAInfo_t info)
{
    OIC_LOG(DEBUG, TAG, "CAGeneratePdu IN");

    if (NULL == uri)
    {
        return NULL;
    }

    uint32_t length = strlen(uri);
    if (CA_MAX_URI_LENGTH < length)
    {
        OIC_LOG(ERROR, TAG, "check URI length..");
        return NULL;
    }

    uint32_t uriLength = length + sizeof(COAP_HEADER);
    char *coapUri = NULL;
    coapUri = (char *) OICCalloc(1, uriLength * sizeof(char));
    if (NULL == coapUri)
    {
        OIC_LOG(ERROR, TAG, "CAGeneratePdu, Memory allocation failed !");
        return NULL;
    }

    coap_list_t *optlist = NULL;
    if (NULL != coapUri)
    {
        strcat(coapUri, COAP_HEADER);
        strcat(coapUri, uri);

        // parsing options in URI
        CAParseURI(coapUri, &optlist);
        OICFree(coapUri);
        coapUri = NULL;

        // parsing options in HeadOption
        CAParseHeadOption(code, info, &optlist);
    }

    coap_pdu_t *pdu;
    if (!(pdu = CAGeneratePduImpl((code_t) code, optlist, info, info.payload)))
    {
        return NULL;
    }

    // free option list
    coap_delete_list(optlist);

    // pdu print method : coap_show_pdu(pdu);
    OIC_LOG(DEBUG, TAG, "CAGeneratePdu OUT");
    return pdu;
}

coap_pdu_t *CAParsePDU(const char *data, uint32_t length, uint32_t *outCode)
{
    OIC_LOG(DEBUG, TAG, "CAParsePDU IN");
    coap_pdu_t *outpdu = coap_new_pdu();

    if (0 >= coap_pdu_parse((unsigned char *) data, length, outpdu))
    {
        OIC_LOG(ERROR, TAG, "coap_pdu_parse failed");
        coap_delete_pdu(outpdu);
        return NULL;
    }

    if (outCode)
    {
        (*outCode) = (uint32_t) outpdu->hdr->code;
    }OIC_LOG(DEBUG, TAG, "CAParsePDU OUT");
    return outpdu;
}

coap_pdu_t *CAGeneratePduImpl(const code_t code, coap_list_t *options, const CAInfo_t info,
                              const char *payload)
{
    OIC_LOG(DEBUG, TAG, "CAGeneratePduImpl IN");

    coap_pdu_t *pdu = coap_new_pdu();
    if (!pdu)
    {
        OIC_LOG(ERROR, TAG, "Out of memory");
        return NULL;
    }

    OIC_LOG_V(DEBUG, TAG, "messageId is %d", info.messageId);
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
            prng((uint8_t * ) &message_id, sizeof(uint16_t));

            OIC_LOG_V(DEBUG, TAG, "generate the message id(%d)", message_id);
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
        uint32_t tokenLength = strlen(info.token);
        OIC_LOG_V(DEBUG, TAG, "token info : %s, %d", info.token, tokenLength);

        int32_t ret = coap_add_token(pdu, tokenLength, (uint8_t *) info.token);
        if (0 == ret)
        {
            OIC_LOG(DEBUG, TAG, "cannot add token to request");
        }
    }

    if (options)
    {
        coap_list_t *opt;
        for (opt = options; opt; opt = opt->next)
        {
            OIC_LOG_V(DEBUG, TAG, "[%s] opt will be added.",
                    COAP_OPTION_DATA(*(coap_option *) opt->data));
            coap_add_option(pdu, COAP_OPTION_KEY(*(coap_option * ) opt->data),
                            COAP_OPTION_LENGTH(*(coap_option * ) opt->data),
                            COAP_OPTION_DATA(*(coap_option * ) opt->data));
        }
    }

    if (NULL != payload)
    {
        uint32_t len = strlen(payload);
        OIC_LOG_V(DEBUG, TAG, "coap_add_data, payload: %s", payload);
        coap_add_data(pdu, len, (const uint8_t *) payload);
    }

    OIC_LOG(DEBUG, TAG, "CAGeneratePduImpl OUT");
    return pdu;
}

void CAParseURI(const char *uriInfo, coap_list_t **optlist)
{
    OIC_LOG(DEBUG, TAG, "CAParseURI IN");OIC_LOG_V(DEBUG, TAG, "url : %s", uriInfo);

    /* split arg into Uri-* options */
    coap_uri_t uri;
    coap_split_uri((unsigned char *) uriInfo, strlen(uriInfo), &uri);

    if (uri.port != COAP_DEFAULT_PORT)
    {
        unsigned char portbuf[2];
        coap_insert(
                optlist,
                CACreateNewOptionNode(COAP_OPTION_URI_PORT,
                                      coap_encode_var_bytes(portbuf, uri.port), portbuf),
                CAOrderOpts);
    }


    CAParseUriPartial(uri.path.s, uri.path.length, COAP_OPTION_URI_PATH, optlist);

    CAParseUriPartial(uri.query.s, uri.query.length, COAP_OPTION_URI_QUERY, optlist);

    OIC_LOG(DEBUG, TAG, "CAParseURI OUT");
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
                coap_insert(
                        optlist,
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

void CAParseHeadOption(const uint32_t code, const CAInfo_t info, coap_list_t **optlist)
{
    OIC_LOG(DEBUG, TAG, "CAParseHeadOption IN");

    OIC_LOG_V(DEBUG, TAG, "start parse Head Option : %d", info.numOptions);

    uint32_t i;
    for (i = 0; i < info.numOptions; i++)
    {
        uint32_t id = info.options[i].optionID;
        if (COAP_OPTION_URI_PATH == id || COAP_OPTION_URI_QUERY == id)
        {
            OIC_LOG_V(DEBUG, TAG, "it is not Header Option : %d", id);
        }
        else
        {
            OIC_LOG_V(DEBUG, TAG, "Head Option ID: %d", info.options[i].optionID);

            OIC_LOG_V(DEBUG, TAG, "Head Option data: %s", info.options[i].optionData);

            OIC_LOG_V(DEBUG, TAG, "Head Option length: %d", info.options[i].optionLength);

            coap_insert(
                    optlist,
                    CACreateNewOptionNode(info.options[i].optionID, info.options[i].optionLength,
                                          info.options[i].optionData),
                    CAOrderOpts);
        }
    }

    OIC_LOG(DEBUG, TAG, "CAParseHeadOption OUT");
}

coap_list_t *CACreateNewOptionNode(const uint16_t key, const uint32_t length, const uint8_t *data)
{
    OIC_LOG(DEBUG, TAG, "CACreateNewOptionNode IN");

    if (!data)
    {
        OIC_LOG(ERROR, TAG, "invalid pointer parameter");
        return NULL;
    }

    coap_option *option = coap_malloc(sizeof(coap_option) + length + 1);
    if (!option)
    {
        OIC_LOG(DEBUG, TAG, "Out of memory");
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
        OIC_LOG(DEBUG, TAG, "coap_new_listnode returns NULL");
        coap_free(option);
        return NULL;
    }
    //coap_free(option);
    OIC_LOG(DEBUG, TAG, "CACreateNewOptionNode OUT");
    return node;
}

int CAOrderOpts(void *a, void *b)
{
    OIC_LOG(DEBUG, TAG, "CAOrderOpts IN");
    if (!a || !b)
    {
        return a < b ? -1 : 1;
    }

    if (COAP_OPTION_KEY(*(coap_option *)a) < COAP_OPTION_KEY(*(coap_option * )b))
    {
        return -1;
    }

    OIC_LOG(DEBUG, TAG, "CAOrderOpts OUT");
    return COAP_OPTION_KEY(*(coap_option *)a) == COAP_OPTION_KEY(*(coap_option * )b);
}

uint32_t CAGetOptionCount(coap_opt_iterator_t opt_iter)
{
    OIC_LOG(DEBUG, TAG, "CAGetOptionCount IN");
    uint32_t count = 0;
    coap_opt_t *option;

    while ((option = coap_option_next(&opt_iter)))
    {
        if (COAP_OPTION_URI_PATH != opt_iter.type && COAP_OPTION_URI_QUERY != opt_iter.type)
        {
            count++;
        }
    }

    OIC_LOG(DEBUG, TAG, "CAGetOptionCount OUT");
    return count;
}

void CAGetInfoFromPDU(const coap_pdu_t *pdu, uint32_t *outCode, CAInfo_t *outInfo, char *outUri,
                      uint32_t buflen)
{
    OIC_LOG(DEBUG, TAG, "CAGetInfoFromPDU IN");

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
            OIC_LOG(DEBUG, TAG, "CAGetInfoFromPDU, Memory allocation failed !");
            return;
        }
    }

    char buf[COAP_MAX_PDU_SIZE] = { 0 };
    coap_opt_t *option;
    char optionResult[CA_MAX_URI_LENGTH] = { 0 };
    uint32_t idx = 0;
    uint32_t optionLength = 0;
    bool isfirstsetflag = false;
    bool isQueryBeingProcessed = false;

    while ((option = coap_option_next(&opt_iter)))
    {
        if (CAGetOptionData((uint8_t *) (COAP_OPT_VALUE(option)), COAP_OPT_LENGTH(option),
                            (uint8_t *) buf, sizeof(buf)))
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
        OIC_LOG(DEBUG, TAG, "inside pdu->hdr->token_length");
        outInfo->token = (char *) OICMalloc(pdu->hdr->token_length + 1);
        if (outInfo->token == NULL)
        {
            OIC_LOG(DEBUG, TAG, "CAGetInfoFromPDU, Memory allocation failed !");
            OICFree(outInfo->options);
            return;
        }
        memcpy(outInfo->token, pdu->hdr->token, pdu->hdr->token_length);
        outInfo->token[pdu->hdr->token_length] = '\0';
    }

    // set payload data
    if (NULL != pdu->data)
    {
        uint32_t payloadLength = strlen((char*) pdu->data);
        OIC_LOG(DEBUG, TAG, "inside pdu->data");
        outInfo->payload = (char *) OICMalloc(payloadLength + 1);
        if (outInfo->payload == NULL)
        {
            OIC_LOG(DEBUG, TAG, "CAGetInfoFromPDU, Memory allocation failed !");
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
        OIC_LOG_V(DEBUG, TAG, "made URL : %s, %s\n", optionResult, outUri);
    }
    OIC_LOG(DEBUG, TAG, "CAGetInfoFromPDU OUT");
    return;

exit:
    OIC_LOG(ERROR, TAG, "buffer too small");
    OICFree(outInfo->options);
    return;
}

CAResult_t CAGenerateTokenInternal(CAToken_t *token)
{
    OIC_LOG(DEBUG, TAG, "CAGenerateTokenInternal IN");
    if (token == NULL)
    {
        return CA_STATUS_FAILED;
    }

    // memory allocation
    char *temp = (char *) OICCalloc(1, (CA_MAX_TOKEN_LEN + 1) * sizeof(char));
    if (temp == NULL)
    {
        OIC_LOG(DEBUG, TAG, "CAGenerateTokenInternal, Memory allocation failed !");
        return CA_MEMORY_ALLOC_FAILED;
    }

    if (SEED == 0)
    {
        SEED = time(NULL);
        if (SEED == -1)
        {
            OIC_LOG(DEBUG, TAG, "Failed to Create Seed!");
            SEED = 0;
            OICFree(temp);
            return CA_STATUS_FAILED;
        }
        srandom(SEED);
    }

    // set random byte
    uint32_t index;
    for (index = 0; index < CA_MAX_TOKEN_LEN; index++)
    {
        // use valid characters
        temp[index] = (random() % 94 + 33) & 0xFF;
    }

    temp[index] = '\0';
    // save token
    *token = temp;

    OIC_LOG(DEBUG, TAG, "generate the token!!");
    OIC_LOG_BUFFER(DEBUG, TAG, (const uint8_t *) *token, CA_MAX_TOKEN_LEN);

    OIC_LOG(DEBUG, TAG, "CAGenerateTokenInternal OUT");
    return CA_STATUS_OK;
}

void CADestroyTokenInternal(CAToken_t token)
{
    OIC_LOG(DEBUG, TAG, "CADestroyTokenInternal IN");
    if (token != NULL)
    {
        OIC_LOG(DEBUG, TAG, "destroy the token!!");
        OIC_LOG_BUFFER(DEBUG, TAG, (const uint8_t *) token, CA_MAX_TOKEN_LEN);
        OICFree(token);
        token = NULL;
    }

    OIC_LOG(DEBUG, TAG, "CADestroyTokenInternal OUT");
}

void CADestroyInfo(CAInfo_t *info)
{
    OIC_LOG(DEBUG, TAG, "CADestroyInfo IN");

    if (NULL != info)
    {
        if (NULL != info->options)
        {
            OIC_LOG(DEBUG, TAG, "free options in CAInfo");
            OICFree(info->options);
        }

        if (NULL != info->token)
        {
            OIC_LOG(DEBUG, TAG, "free token in CAInfo");
            OICFree(info->token);
        }

        if (NULL != info->payload)
        {
            OIC_LOG(DEBUG, TAG, "free payload in CAInfo");
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
        OIC_LOG(ERROR, TAG, "buflen or len is not available");
        return 0;
    }

    if (NULL == data)
    {
        OIC_LOG(ERROR, TAG, "data not available");
        return 0;
    }

    if (NULL == option)
    {
        OIC_LOG(ERROR, TAG, "option pointer is null");
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
        return 0;
    }

    coap_hdr_t *hdr = (coap_hdr_t *) pdu;
    if (NULL == hdr)
    {
        return 0;
    }

    return ntohs(hdr->id);
}

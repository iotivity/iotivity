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


#include "caprotocolmessage.h"
#include "logger.h"
#include "oic_malloc.h"

#define TAG "CA"

#define CA_MAX_TOKEN_LEN   (8)
#define CA_FLAGS_BLOCK 0x01
#define CA_BUFSIZE 128
#define CA_COAP_MESSAGE_CON 0

#include <time.h>


uint32_t CAGetRequestInfoFromPdu(const coap_pdu_t *pdu, CARequestInfo_t *outReqInfo,
                                 char *outUri)
{
    OIC_LOG(DEBUG, TAG, "CAGetRequestInfoFromPdu IN");

    if (NULL == pdu)
        return 0;

    uint32_t code = CA_NOT_FOUND;
    CAGetRequestPDUInfo(pdu, &code, &(outReqInfo->info), outUri);
    outReqInfo->method = code;
    OIC_LOG(DEBUG, TAG, "CAGetRequestInfoFromPdu OUT");
    return 1;
}

uint32_t CAGetResponseInfoFromPdu(const coap_pdu_t *pdu, CAResponseInfo_t *outResInfo,
                                  char *outUri)
{
    OIC_LOG(DEBUG, TAG, "CAGetResponseInfoFromPdu IN");
    if (NULL == pdu)
        return 0;

    uint32_t code = CA_NOT_FOUND;
    CAGetRequestPDUInfo(pdu, &code, &(outResInfo->info), outUri);
    outResInfo->result = code;
    OIC_LOG(DEBUG, TAG, "CAGetResponseInfoFromPdu OUT");
    return 1;
}

coap_pdu_t *CAGeneratePdu(const char *uri, const uint32_t code, const CAInfo_t info)
{
    OIC_LOG(DEBUG, TAG, "CAGeneratePdu IN");

    coap_pdu_t *pdu;
    char *coapUri = NULL;
    uint32_t coapHeaderLength = 12;
    uint32_t length;
    coap_list_t *optlist = NULL;

    if (NULL == uri)
        return NULL;

    length = strlen(uri);
    if (CA_MAX_URI_LENGTH < length)
    {
        OIC_LOG(DEBUG, TAG, "check URI length..");
        return NULL;
    }

    uint32_t uriLength = length + coapHeaderLength + 1;
    coapUri = (char *) OICMalloc(uriLength);

    if (coapUri == NULL)
    {
        OIC_LOG(DEBUG, TAG, "CAGeneratePdu, Memory allocation failed !");
        return NULL;
    }
    memset(coapUri, 0, uriLength);

    if (NULL != coapUri)
    {
        memcpy(coapUri, "coap://[::]/", coapHeaderLength);
        memcpy(coapUri + coapHeaderLength, uri, length);

        // parsing options in URI
        CAParseURI(coapUri, &optlist);

        // parsing options in HeadOption
        CAParseHeadOption(code, info, &optlist);

        OICFree(coapUri);
        coapUri = NULL;
    }

    if (NULL != info.payload) // payload is include in request / response
    {
        if (!(pdu = CACreatePDUforRequestWithPayload((code_t) code, optlist, info.payload, info)))
            return NULL;
    }
    else // payload is not include in request / response
    {
        if (!(pdu = CACreatePDUforRequest((code_t) code, optlist, info)))
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

    coap_pdu_parse((unsigned char *) data, length, outpdu);
    (*outCode) = (uint32_t) outpdu->hdr->code;
    OIC_LOG(DEBUG, TAG, "CAParsePDU OUT");
    return outpdu;
}

coap_pdu_t *CACreatePDUforRequestWithPayload(const code_t code, coap_list_t *options,
        const char *payload, const CAInfo_t info)
{
    OIC_LOG(DEBUG, TAG, "CACreatePDUforRequestWithPayload IN");

    coap_pdu_t *pdu;
    coap_list_t *opt;

    uint32_t CAFlags = 0;
    coap_block_t CABlock =
    { .num = 0, .m = 0, .szx = 6 };

    if (!(pdu = coap_new_pdu()))
        return NULL;

    unsigned short message_id;

    if (info.messageId == 0)
    {
        /* initialize message id */
        prng((unsigned char * )&message_id, sizeof(unsigned short));
        ++message_id;

        OIC_LOG_V(DEBUG, TAG, "generate the message id(%d)", message_id);
    }
    else
    {
        /* use saved message id */
        message_id = info.messageId;
    }

    pdu->hdr->type = info.type;
    pdu->hdr->id = htons(message_id);
    pdu->hdr->code = code;

    if (info.token)
    {
        pdu->hdr->token_length = CA_MAX_TOKEN_LEN;
        if (!coap_add_token(pdu, CA_MAX_TOKEN_LEN, (unsigned char *) info.token))
        {
            OIC_LOG(DEBUG, TAG, "cannot add token to request");
        }
    }
    for (opt = options; opt; opt = opt->next)
    {
        OIC_LOG_V(DEBUG, TAG, "[%s] opt will be added.", COAP_OPTION_DATA(*(coap_option * )opt->data));
        coap_add_option(pdu, COAP_OPTION_KEY(*(coap_option * )opt->data),
                        COAP_OPTION_LENGTH(*(coap_option * )opt->data),
                        COAP_OPTION_DATA(*(coap_option * )opt->data));
    }

    if (NULL != payload)
    {
        uint32_t len = strlen(payload);
        if ((CAFlags & CA_FLAGS_BLOCK) == 0)
        {
            OIC_LOG_V(DEBUG, TAG, "coap_add_data, payload: %s", payload);
            coap_add_data(pdu, len, (const unsigned char *) payload);
        }
        else
        {
            OIC_LOG_V(DEBUG, TAG, "coap_add_block, payload: %s", payload);
            coap_add_block(pdu, len, (const unsigned char *) payload, CABlock.num, CABlock.szx);
        }
    }
    OIC_LOG(DEBUG, TAG, "CACreatePDUforRequestWithPayload OUT");
    return pdu;
}

coap_pdu_t *CACreatePDUforRequest(const code_t code, coap_list_t *options,
                                  const CAInfo_t info)
{
    OIC_LOG(DEBUG, TAG, "CACreatePDUforRequest IN");

    coap_pdu_t *pdu;
    coap_list_t *opt;

    if (!(pdu = coap_new_pdu()))
    {
        OIC_LOG(DEBUG, TAG, "Out of memory");
        return NULL;
    }

    unsigned short message_id;

    if (info.messageId == 0)
    {
        /* initialize message id */
        prng((unsigned char * )&message_id, sizeof(unsigned short));
        ++message_id;

        OIC_LOG_V(DEBUG, TAG, "generate the message id(%d)", message_id);
    }
    else
    {
        /* use saved message id */
        message_id = info.messageId;
    }

    pdu->hdr->type = info.type;
    pdu->hdr->id = htons(message_id);
    pdu->hdr->code = code;

    OIC_LOG_V(DEBUG, TAG, "token info : %s, %d", info.token, strlen(info.token));
    pdu->hdr->token_length = CA_MAX_TOKEN_LEN;

    if (!coap_add_token(pdu, CA_MAX_TOKEN_LEN, (unsigned char *) info.token))
    {
        OIC_LOG(DEBUG, TAG, "cannot add token to request");
    }

    for (opt = options; opt; opt = opt->next)
    {
        coap_add_option(pdu, COAP_OPTION_KEY(*(coap_option * )opt->data),
                        COAP_OPTION_LENGTH(*(coap_option * )opt->data),
                        COAP_OPTION_DATA(*(coap_option * )opt->data));
    }
    OIC_LOG(DEBUG, TAG, "CACreatePDUforRequest OUT");
    return pdu;
}

void CAParseURI(const char *uriInfo, coap_list_t **optlist)
{
    OIC_LOG(DEBUG, TAG, "CAParseURI IN");

    unsigned char portbuf[2];
    unsigned char _buf[CA_BUFSIZE];
    unsigned char *buf = _buf;
    coap_uri_t uri;
    size_t buflen;
    uint32_t res;

    OIC_LOG_V(DEBUG, TAG, "url : %s", uriInfo);

    /* split arg into Uri-* options */
    coap_split_uri((unsigned char *) uriInfo, strlen(uriInfo), &uri);

    if (uri.port != COAP_DEFAULT_PORT)
    {
        coap_insert(optlist,
                    CACreateNewOptionNode(COAP_OPTION_URI_PORT,
                                          coap_encode_var_bytes(portbuf, uri.port), portbuf),
                    CAOrderOpts);
    }

    if (uri.path.length)
    {
        buflen = CA_BUFSIZE;
        res = coap_split_path(uri.path.s, uri.path.length, buf, &buflen);

        while (res--)
        {
            coap_insert(optlist,
                        CACreateNewOptionNode(COAP_OPTION_URI_PATH, COAP_OPT_LENGTH(buf),
                                              COAP_OPT_VALUE(buf)), CAOrderOpts);
            buf += COAP_OPT_SIZE(buf);
        }
    }

    if (uri.query.length)
    {
        buflen = CA_BUFSIZE;
        buf = _buf;
        res = coap_split_query(uri.query.s, uri.query.length, buf, &buflen);

        while (res--)
        {
            coap_insert(optlist,
                        CACreateNewOptionNode(COAP_OPTION_URI_QUERY, COAP_OPT_LENGTH(buf),
                                              COAP_OPT_VALUE(buf)), CAOrderOpts);

            buf += COAP_OPT_SIZE(buf);
        }
    }
    OIC_LOG(DEBUG, TAG, "CAParseURI OUT");
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

            coap_insert(optlist,
                        CACreateNewOptionNode(info.options[i].optionID,
                                              info.options[i].optionLength,
                                              info.options[i].optionData), CAOrderOpts);
        }
    }
    OIC_LOG(DEBUG, TAG, "CAParseHeadOption OUT");
}

coap_list_t *CACreateNewOptionNode(const uint16_t key, const uint32_t length,
                                   const uint8_t *data)
{
    OIC_LOG(DEBUG, TAG, "CACreateNewOptionNode IN");
    coap_option *option;
    coap_list_t *node;

    option = coap_malloc(sizeof(coap_option) + length + 1);
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
    node = coap_new_listnode(option, NULL);

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
        if (COAP_OPTION_URI_PATH == opt_iter.type || COAP_OPTION_URI_QUERY == opt_iter.type)
        {

        }
        else
        {
            count++;
        }
    }
    OIC_LOG(DEBUG, TAG, "CAGetOptionCount OUT");
    return count;
}

void CAGetRequestPDUInfo(const coap_pdu_t *pdu, uint32_t *outCode, CAInfo_t *outInfo,
                         char *outUri)
{
    OIC_LOG(DEBUG, TAG, "CAGetRequestPDUInfo IN");
    char buf[COAP_MAX_PDU_SIZE]; /* need some space for output creation */
    uint32_t encode = 0;
    coap_opt_iterator_t opt_iter;
    coap_opt_t *option;
    char optionResult[CA_MAX_URI_LENGTH] =
    { 0, };
    uint32_t count = 0, idx = 0;
    uint32_t optionLength = 0;
    uint32_t isfirstsetflag = 0;

    coap_option_iterator_init((coap_pdu_t *) pdu, &opt_iter, COAP_OPT_ALL);

    memset(optionResult, 0, sizeof(optionResult));

    // set code
    (*outCode) = (uint32_t) pdu->hdr->code;

    // init HeaderOption list
    count = CAGetOptionCount(opt_iter);

    memset(outInfo, 0, sizeof(CAInfo_t));
    outInfo->numOptions = count;
    // set type
    outInfo->type = pdu->hdr->type;

    // set message id
    outInfo->messageId = ntohs(pdu->hdr->id);

    if (count > 0)
    {
        outInfo->options = (CAHeaderOption_t *) OICMalloc(sizeof(CAHeaderOption_t) * count);
        if (outInfo->options == NULL)
        {
            OIC_LOG(DEBUG, TAG, "CAGetRequestPDUInfo, Memory allocation failed !");
            return;
        }
        memset(outInfo->options, 0, sizeof(CAHeaderOption_t) * count);
    }

    while ((option = coap_option_next(&opt_iter)))
    {

        if (CAGetOptionData((uint8_t *)(COAP_OPT_VALUE(option)),
                            COAP_OPT_LENGTH(option), (uint8_t *)buf, sizeof(buf),
                            encode))
        {
            OIC_LOG_V(DEBUG, TAG, "COAP URI element : %s", buf);
            if (COAP_OPTION_URI_PATH == opt_iter.type || COAP_OPTION_URI_QUERY == opt_iter.type)
            {
                if (0 == isfirstsetflag)
                {
                    isfirstsetflag = 1;
                    memcpy(optionResult + optionLength, "/", 1);
                    optionLength++;
                    memcpy(optionResult + optionLength, buf, strlen((const char *) buf));
                    optionLength += strlen((const char *) buf);
                }
                else
                {
                    if (COAP_OPTION_URI_PATH == opt_iter.type)
                    {
                        memcpy(optionResult + optionLength, "/", 1);
                        optionLength++;
                    }
                    else if (COAP_OPTION_URI_QUERY == opt_iter.type)
                    {
                        memcpy(optionResult + optionLength, "?", 1);
                        optionLength++;
                    }
                    memcpy(optionResult + optionLength, buf, strlen((const char *) buf));
                    optionLength += strlen((const char *) buf);
                }
            }
            else
            {
                if (idx < count)
                {
                    uint32_t length = (uint32_t) strlen((const char *) buf);

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
        outInfo->token = (char *) OICMalloc(CA_MAX_TOKEN_LEN + 1);
        if (outInfo->token == NULL)
        {
            OIC_LOG(DEBUG, TAG, "CAGetRequestPDUInfo, Memory allocation failed !");
            OICFree(outInfo->options);
            return;
        }
        memset(outInfo->token, 0, CA_MAX_TOKEN_LEN + 1);
        memcpy(outInfo->token, pdu->hdr->token, CA_MAX_TOKEN_LEN);
    }

    // set payload data
    if (NULL != pdu->data)
    {
        OIC_LOG(DEBUG, TAG, "inside pdu->data");
        outInfo->payload = (char *) OICMalloc(strlen((const char *) pdu->data) + 1);
        if (outInfo->payload == NULL)
        {
            OIC_LOG(DEBUG, TAG, "CAGetRequestPDUInfo, Memory allocation failed !");
            OICFree(outInfo->options);
            OICFree(outInfo->token);
            return;
        }
        memcpy(outInfo->payload, pdu->data, strlen((const char *) pdu->data) + 1);
    }


    memcpy(outUri, optionResult, strlen(optionResult));
    OIC_LOG_V(DEBUG, TAG, "made URL : %s\n", optionResult);
    OIC_LOG(DEBUG, TAG, "CAGetRequestPDUInfo OUT");

}

CAResult_t CAGenerateTokenInternal(CAToken_t *token)
{
    OIC_LOG(DEBUG, TAG, "CAGenerateTokenInternal IN");
    if (token == NULL)
    {
        return CA_STATUS_FAILED;
    }

    // memory allocation
    char *temp = (char *) OICMalloc(sizeof(char) * (CA_MAX_TOKEN_LEN + 1));
    if (temp == NULL)
    {
        OIC_LOG(DEBUG, TAG, "CAGenerateTokenInternal, Memory allocation failed !");
        return CA_MEMORY_ALLOC_FAILED;
    }
    memset(temp, 0, sizeof(char) * (CA_MAX_TOKEN_LEN + 1));

    // set random byte
    uint32_t index;

    srand(time(NULL));
    for (index = 0; index < CA_MAX_TOKEN_LEN; index++)
    {
        // use valid characters
        temp[index] = (rand() % 94 + 33) & 0xFF;
    }

    temp[index] = '\0';
    // save token
    *token = temp;

    OIC_LOG_V(DEBUG, TAG, "generate the token(%s)!!", *token);
    OIC_LOG(DEBUG, TAG, "CAGenerateTokenInternal OUT");
    return CA_STATUS_OK;
}

void CADestroyTokenInternal(CAToken_t token)
{
    OIC_LOG(DEBUG, TAG, "CADestroyTokenInternal IN");
    if (token != NULL)
    {
        OIC_LOG_V(DEBUG, TAG, "destroy the token(%s)!!", token);

        OICFree(token);
    }
    OIC_LOG(DEBUG, TAG, "CADestroyTokenInternal OUT");
}

void CADeinitialize(CAInfo_t *info)
{
    OIC_LOG(DEBUG, TAG, "CADeinitialize IN");

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
    OIC_LOG(DEBUG, TAG, "CADeinitialize OUT");
}

uint32_t CAGetOptionData(const uint8_t *data, uint32_t len, uint8_t *result,
                         uint32_t buflen, uint32_t encode_always)
{
    const unsigned char hex[] = "0123456789ABCDEF";
    uint32_t cnt = 0;
    assert(data || len == 0);

    if (buflen == 0 || len == 0)
        return 0;

    if (data == NULL)
    {
        return 0;
    }

    while (len)
    {
        if (!encode_always)
        {
            if (cnt == buflen)
                break;
            *result++ = *data;
            ++cnt;
        }
        else
        {
            if (cnt + 4 < buflen)
            {
                *result++ = '\\';
                *result++ = 'x';
                *result++ = hex[(*data & 0xf0) >> 4];
                *result++ = hex[*data & 0x0f];
                cnt += 4;
            }
            else
                break;
        }

        ++data;
        --len;
    }

    *result = '\0';
    return cnt;
}

CAMessageType_t CAGetMessageTypeFromPduBinaryData(const void *pdu, uint32_t size)
{
    // pdu minimum size is 4 byte.
    if (size < 4)
        return CA_MSG_NONCONFIRM;

    coap_hdr_t *hdr = (coap_hdr_t *) pdu;

    return (CAMessageType_t) hdr->type;
}

uint16_t CAGetMessageIdFromPduBinaryData(const void *pdu, uint32_t size)
{
    // pdu minimum size is 4 byte.
    if (size < 4)
        return 0;

    coap_hdr_t *hdr = (coap_hdr_t *) pdu;

    return ntohs(hdr->id);
}

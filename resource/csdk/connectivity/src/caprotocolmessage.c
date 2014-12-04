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

#define TAG PCF("CA")

#define CA_MAX_TOKEN_LEN   (8)
#define CA_FLAGS_BLOCK 0x01
#define CA_BUFSIZE 128
#define CA_COAP_MESSAGE_CON 0

uint32_t CAGetRequestInfoFromPdu(const coap_pdu_t *pdu, CARequestInfo_t* outReqInfo, char* outUri)
{
    OIC_LOG(DEBUG, TAG, "get request info from PDU");
    if (NULL == pdu)
        return 0;

    uint32_t code = CA_NOT_FOUND;
    CAGetRequestPDUInfo(pdu, &code, &(outReqInfo->info), outUri);
    outReqInfo->method = code;
    return 1;
}

uint32_t CAGetResponseInfoFromPdu(const coap_pdu_t *pdu, CAResponseInfo_t* outResInfo, char* outUri)
{
    OIC_LOG(DEBUG, TAG, "get response info from PDU");
    if (NULL == pdu)
        return 0;

    uint32_t code = CA_NOT_FOUND;
    CAGetRequestPDUInfo(pdu, &code, &(outResInfo->info), outUri);
    outResInfo->result = code;

    return 1;
}

coap_pdu_t* CAGeneratePdu(const char* uri, const uint32_t code, const CAInfo_t info)
{
    OIC_LOG(DEBUG, TAG, "generate PDU");

    coap_pdu_t *pdu;
    char* coapUri = NULL;
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

    coapUri = (char*) OICMalloc(length + coapHeaderLength + 1);
    memset(coapUri, 0, length + coapHeaderLength + 1);

    if (NULL != coapUri)
    {
        memcpy(coapUri, "coap://[::]/", coapHeaderLength);
        memcpy(coapUri + coapHeaderLength, uri, length);

        // parsing options in URI
        CAParseURI(coapUri, &optlist);

        // parsing options in HeadOption
        CAParseHeadOption(code, info, &optlist);

        OICFree(coapUri);
    }

    if (NULL != info.payload) // payload is include in request / response
    {
        if (!(pdu = CACreatePDUforRequestWithPayload((code_t) code, optlist, info.payload)))
            return NULL;
    }
    else // payload is not include in request / response
    {
        if (!(pdu = CACreatePDUforRequest((code_t) code, optlist)))
            return NULL;
    }

    // pdu print method : coap_show_pdu(pdu);

    return pdu;
}

coap_pdu_t* CAParsePDU(const char* data, uint32_t* outCode)
{
    coap_pdu_t* outpdu = coap_new_pdu();
    coap_pdu_parse((unsigned char *) data, strlen(data), outpdu);
    (*outCode) = (uint32_t) outpdu->hdr->code;

    return outpdu;
}

coap_pdu_t* CACreatePDUforRequestWithPayload(const code_t code, coap_list_t *options,
        const char* payload)
{
    OIC_LOG(DEBUG, TAG, "CACreatePDUforRequestWithPayload");

    coap_pdu_t *pdu;
    coap_list_t *opt;
    unsigned char _token_data[8];
    str the_token =
    { 0, _token_data };

    uint32_t CAFlags = 0;
    coap_block_t CABlock =
    { .num = 0, .m = 0, .szx = 6 };

    if (!(pdu = coap_new_pdu()))
        return NULL;

    /* initialize message id */
    unsigned short message_id;
    prng((unsigned char * )&message_id, sizeof(unsigned short));

    pdu->hdr->type = CA_COAP_MESSAGE_CON;
    pdu->hdr->id = htons(++message_id);
    pdu->hdr->code = code;

    pdu->hdr->token_length = the_token.length;
    if (!coap_add_token(pdu, the_token.length, the_token.s))
    {
        OIC_LOG(DEBUG, TAG, "cannot add token to request");
    }

    for (opt = options; opt; opt = opt->next)
    {
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
    return pdu;
}

coap_pdu_t* CACreatePDUforRequest(const code_t code, coap_list_t *options)
{
    OIC_LOG(DEBUG, TAG, "CACreatePDUforRequest");

    coap_pdu_t *pdu;
    coap_list_t *opt;
    unsigned char _token_data[8];
    str the_token =
    { 0, _token_data };

    if (!(pdu = coap_new_pdu()))
        return NULL;

    /* initialize message id */
    unsigned short message_id;
    prng((unsigned char * )&message_id, sizeof(unsigned short));

    pdu->hdr->type = CA_COAP_MESSAGE_CON;
    pdu->hdr->id = htons(++message_id);
    pdu->hdr->code = code;

    pdu->hdr->token_length = the_token.length;
    if (!coap_add_token(pdu, the_token.length, the_token.s))
    {
        OIC_LOG(DEBUG, TAG, "cannot add token to request");
    }

    for (opt = options; opt; opt = opt->next)
    {
        coap_add_option(pdu, COAP_OPTION_KEY(*(coap_option * )opt->data),
                COAP_OPTION_LENGTH(*(coap_option * )opt->data),
                COAP_OPTION_DATA(*(coap_option * )opt->data));
    }

    return pdu;
}

void CAParseURI(const char* uriInfo, coap_list_t **optlist)
{
    OIC_LOG(DEBUG, TAG, "parse URI");

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
                        coap_encode_var_bytes(portbuf, uri.port), portbuf), CAOrderOpts);
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

}

void CAParseHeadOption(const uint32_t code, const CAInfo_t info, coap_list_t **optlist)
{
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
                    CACreateNewOptionNode(info.options[i].optionID, info.options[i].optionLength,
                            info.options[i].optionData), CAOrderOpts);
        }
    }
}

coap_list_t* CACreateNewOptionNode(const uint16_t key, const uint32_t length, const uint8_t *data)
{
    coap_option *option;
    coap_list_t *node;

    option = coap_malloc(sizeof(coap_option) + length);
    if (!option)
        goto error;

    COAP_OPTION_KEY(*option) = key;
    COAP_OPTION_LENGTH(*option) = length;
    memcpy(COAP_OPTION_DATA(*option), data, length);

    /* we can pass NULL here as delete function since option is released automatically  */
    node = coap_new_listnode(option, NULL);

    if (node)
        return node;

    error: perror("new_option_node: malloc");
    coap_free(option);
    return NULL;
}

int32_t CAOrderOpts(void *a, void *b)
{
    if (!a || !b)
        return a < b ? -1 : 1;

    if (COAP_OPTION_KEY(*(coap_option *)a) < COAP_OPTION_KEY(*(coap_option * )b))
        return -1;

    return COAP_OPTION_KEY(*(coap_option *)a) == COAP_OPTION_KEY(*(coap_option * )b);
}

uint32_t getOptionCount(coap_opt_iterator_t opt_iter)
{
    uint32_t count = 0;
    coap_opt_t *option;

    while ((option = coap_option_next(&opt_iter)))
    {
        count++;
    }

    return count;
}

void CAGetRequestPDUInfo(const coap_pdu_t *pdu, uint32_t* outCode, CAInfo_t* outInfo, char* outUri)
{
    unsigned char buf[COAP_MAX_PDU_SIZE]; /* need some space for output creation */
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

    count = getOptionCount(opt_iter);

    // set code
    (*outCode) = (uint32_t) pdu->hdr->code;

    // init HeaderOption list
    memset(outInfo, 0, sizeof(CAInfo_t));
    outInfo->numOptions = count;
    outInfo->options = (CAHeaderOption_t*) OICMalloc(sizeof(CAHeaderOption_t) * count);
    memset(outInfo->options, 0, sizeof(CAHeaderOption_t) * count);

    while ((option = coap_option_next(&opt_iter)))
    {

        if (print_readable(COAP_OPT_VALUE(option), COAP_OPT_LENGTH(option), buf, sizeof(buf),
                encode))
        {
            if (COAP_OPTION_URI_PATH == opt_iter.type || COAP_OPTION_URI_QUERY == opt_iter.type)
            {
                if (0 == isfirstsetflag)
                {
                    isfirstsetflag = 1;
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

            if (idx < count)
            {
                uint32_t length = (uint32_t) strlen((const char *) buf);

                if(length <= CA_MAX_HEADER_OPTION_DATA_LENGTH) {
                    outInfo->options[idx].optionID = opt_iter.type;
                    outInfo->options[idx].optionLength = length;
                    outInfo->options[idx].protocolID = CA_COAP_ID;
                    memcpy(outInfo->options[idx].optionData, buf, length);
                    idx++;
                }
            }
        }
    }

    // set token data
    if (pdu->hdr->token_length > 0)
    {
        outInfo->token = (char*) OICMalloc(pdu->hdr->token_length);
        memcpy(outInfo->token, pdu->hdr->token, pdu->hdr->token_length);
    }

    // set payload data
    if (NULL != pdu->data)
    {
        outInfo->payload = (char*) OICMalloc(strlen((const char *) pdu->data) + 1);
        memcpy(outInfo->payload, pdu->data, strlen((const char *) pdu->data) + 1);
    }

    // set uri info
    memcpy(outUri, optionResult, strlen(optionResult));
}

CAResult_t CAGenerateTokenInternal(CAToken_t* token)
{
    if (token == NULL)
    {
        return CA_STATUS_FAILED;
    }

    // memory allocation
    char* temp = (char*) OICMalloc(sizeof(char) * (CA_MAX_TOKEN_LEN + 1));

    if (temp == NULL)
    {
        return CA_MEMORY_ALLOC_FAILED;
    }
    memset(temp, 0, sizeof(char) * (CA_MAX_TOKEN_LEN + 1));

    // set random byte
    uint32_t index;
    for (index = 0; index < CA_MAX_TOKEN_LEN; index++)
    {
        temp[index] = rand() & 0xFF;
    }

    // save token
    *token = temp;

    OIC_LOG_V(DEBUG, TAG, "generate the token(%s)!!", *token);

    return CA_STATUS_OK;
}

void CADestroyTokenInternal(CAToken_t token)
{
    if (token != NULL)
    {
        OIC_LOG_V(DEBUG, TAG, "destroy the token(%s)!!", token);

        OICFree(token);
    }
}

void CADeinitialize(CAInfo_t* info)
{
    OIC_LOG(DEBUG, TAG, "CADeinitialize!!");

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
}
